#include <nasty_mpi/mpi_op.h>
#include <stdlib.h>
#include <assert.h>

int Nasty_mpi_op_signature_equal(Nasty_mpi_op_signature_t *alpha, Nasty_mpi_op_signature_t *beta)
{
  if (alpha == NULL && beta == NULL)
    return 1;
  else if (alpha == NULL || beta == NULL)
    return 0;
  else
    return alpha->target_rank == beta->target_rank &&
    MPI_type_hash_equal(&alpha->origin_type_hash, &beta->origin_type_hash) &&
    MPI_type_hash_equal(&alpha->target_type_hash, &beta->target_type_hash) &&
    alpha->target_disp == beta->target_disp;
}

void Nasty_mpi_op_signature(Nasty_mpi_op *op, Nasty_mpi_op_signature_t *call_signature)
{
  assert(call_signature);

  if (op->type == rma_put) {
    Nasty_mpi_put put = op->data.put;
    MPI_type_hash(put.origin_datatype, put.origin_count, &call_signature->origin_type_hash);
    MPI_type_hash(put.target_datatype, put.target_count, &call_signature->target_type_hash);
    call_signature->target_disp = put.target_disp;
    call_signature->target_rank = put.target_rank;

  } else if (op->type == rma_get) {
    Nasty_mpi_get get = op->data.get;
    MPI_type_hash(get.origin_datatype, get.origin_count, &call_signature->origin_type_hash);
    MPI_type_hash(get.target_datatype, get.target_count, &call_signature->target_type_hash);
    call_signature->target_disp = get.target_disp;
    call_signature->target_rank = get.target_rank;
  }
}

int Nasty_mpi_op_is_divisible(Nasty_mpi_op *op)
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
