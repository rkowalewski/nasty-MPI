#include <nasty_ops/nasty_mpi_op.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <macros/logging.h>

static size_t hash(size_t val)
{
  //adapted from http://burtleburtle.net/bob/hash/integer.html
  val = (val ^ 61) ^ (val >> 16);
  val = val + (val << 3);
  val = val ^ (val >> 4);
  val = val * 0x27d4eb2d;
  val = val ^ (val >> 15);
  return val;

}

int Nasty_mpi_op_signature_equal(Nasty_mpi_op_signature_t *alpha, Nasty_mpi_op_signature_t *beta)
{
  if (alpha == NULL && beta == NULL)
    return 1;
  else if (alpha == NULL || beta == NULL)
    return 0;
  else
    return alpha->params_hash == beta->params_hash &&
           MPI_type_hash_equal(&alpha->origin_type_hash, &beta->origin_type_hash) &&
           MPI_type_hash_equal(&alpha->target_type_hash, &beta->target_type_hash);
}

void Nasty_mpi_op_signature(Nasty_mpi_op *op, Nasty_mpi_op_signature_t *call_signature)
{
  assert(call_signature);
  assert(op);

  //hash over values of origin_addr, target_rank, target_disp, type
  size_t to_hash = op->target_rank + op->type;

  if (op->type == rma_put) {
    Nasty_mpi_put put = op->data.put;
    MPI_type_hash(put.origin_datatype, put.origin_count, &call_signature->origin_type_hash);
    MPI_type_hash(put.target_datatype, put.target_count, &call_signature->target_type_hash);
    to_hash += (size_t) put.origin_addr;
    to_hash += put.target_disp;


  } else if (op->type == rma_get) {
    Nasty_mpi_get get = op->data.get;
    MPI_type_hash(get.origin_datatype, get.origin_count, &call_signature->origin_type_hash);
    MPI_type_hash(get.target_datatype, get.target_count, &call_signature->target_type_hash);
    to_hash += (size_t) get.origin_addr;
    to_hash += get.target_disp;
  }

  call_signature->params_hash = hash(to_hash);
  call_signature->lookup_count = 0;
}


static int nasty_mpi_op_type_extent(MPI_Datatype type, MPI_Aint *size) {
  MPI_Aint lb;
  return MPI_Type_get_extent(type, &lb, size);
}

static int Nasty_mpi_op_is_divisible(Nasty_mpi_op *op, int disp)
{
  if (op == NULL) return 0;

  int is_same_types = MPI_type_hash_equal(&op->signature.origin_type_hash, &op->signature.target_type_hash);

  if (is_same_types) {
    int nints, naddrs, ntypes, combiner;
    MPI_Datatype type;

    if (op->type == rma_get && op->data.get.origin_count > 1)
      type = op->data.get.origin_datatype;
    else if (op->type == rma_put && op->data.put.origin_count > 1)
      type = op->data.put.origin_datatype;
    else
      return 0;

    MPI_Type_get_envelope(type, &nints, &naddrs, &ntypes, &combiner);
    return combiner == MPI_COMBINER_NAMED;
  }

  return 0;
}

void Nasty_mpi_op_type_str(Nasty_mpi_op *op, char* str)
{
  assert(str);
  if (op->type == rma_put)
    snprintf(str, MAX_OP_TYPE_STRLEN + 1, "%s", "MPI_Put");
  else if (op->type == rma_get)
    snprintf(str, MAX_OP_TYPE_STRLEN + 1, "%s", "MPI_Get");
  else
    snprintf(str, MAX_OP_TYPE_STRLEN + 1, "%s", "undefined");
}

DArray Nasty_mpi_op_divide(Nasty_mpi_op *op_info, int win_disp_unit)
{

  if (!op_info || !Nasty_mpi_op_is_divisible(op_info, win_disp_unit)) return NULL;

  assert(win_disp_unit >= 0);

  MPI_Aint extent, origin_base, target_base, target_disp_step = 1;
  int offset;
  Nasty_mpi_op *new_op;
  DArray divided_ops = NULL;

  if (op_info->type == rma_put) {
    Nasty_mpi_put put = op_info->data.put;
    nasty_mpi_op_type_extent(put.origin_datatype, &extent);

    assert(extent >= (MPI_Aint) 0);

    if (win_disp_unit != extent) {
      MPI_Aint reminder = extent % win_disp_unit;

      if (reminder && (reminder != extent)) {
        debug("cannot split operations where the ratio between the window's disp_unit (%zu) and the type's extent (%zu) are odd", (size_t) win_disp_unit, (size_t) extent);
        return NULL;
      }

      target_disp_step = extent;
    }

    divided_ops = DArray_create(sizeof(Nasty_mpi_op), put.target_count);
    MPI_Get_address(put.origin_addr, &origin_base);
    target_base = put.target_disp;

    for (offset = 0; offset < put.target_count; ++offset) {
      new_op = DArray_new(divided_ops);
      new_op->type = rma_put;
      new_op->target_rank = op_info->target_rank;
      new_op->is_sent = 0;
      new_op->data.put = put;
      new_op->data.put.origin_count = 1;
      //In the new MPI Standard (>= 3.1) we should use MPI_Aint_add for safe portability
      //see: http://www.open-mpi.de/doc/v2.0/man3/MPI_Aint_add.3.php
      new_op->data.put.origin_addr = (void*) (origin_base + ((MPI_Aint)offset) * extent);
      new_op->data.put.target_disp = target_base + ((MPI_Aint)offset) * target_disp_step;
      new_op->data.put.target_count = 1;
      Nasty_mpi_op_signature(new_op, &new_op->signature);
      DArray_push(divided_ops, new_op);
    }
  } else if (op_info->type == rma_get) {
    Nasty_mpi_get get = op_info->data.get;
    nasty_mpi_op_type_extent(get.origin_datatype, &extent);

    assert(extent >= (MPI_Aint) 0);

    if (win_disp_unit != extent) {
      MPI_Aint reminder = extent % win_disp_unit;

      if (reminder && (reminder != extent)) {
        debug("cannot split operations where the ratio between the window's disp_unit (%zu) and the type's extent (%zu) are odd", (size_t) win_disp_unit, (size_t) extent);
        return NULL;
      }

      target_disp_step = extent;
    }

    divided_ops = DArray_create(sizeof(Nasty_mpi_op), get.target_count);
    MPI_Get_address(get.origin_addr, &origin_base);
    target_base = get.target_disp;

    for (offset = 0; offset < get.target_count; ++offset) {
      new_op = DArray_new(divided_ops);
      new_op->type = rma_get;
      new_op->target_rank = op_info->target_rank;
      new_op->is_sent = 0;
      new_op->data.get = get;
      new_op->data.get.origin_count = 1;
      new_op->data.get.origin_addr = (void*) (origin_base + ((MPI_Aint)offset) * extent);
      new_op->data.get.target_disp = target_base + ((MPI_Aint)offset) * target_disp_step;
      new_op->data.get.target_count = 1;
      Nasty_mpi_op_signature(new_op, &new_op->signature);
      DArray_push(divided_ops, new_op);
    }
  }

  return divided_ops;
}

void Nasty_mpi_op_type_to_str(const Nasty_mpi_op * op_info, char * const buf, size_t len) {
  (void) len;
  assert(len > 10);
  switch (op_info->type) {
    case rma_put:
      strcpy(buf, "MPI_Get");
      break;
    case rma_get:
      strcpy(buf, "MPI_Put");
      break;
    default: ;
  }
}
