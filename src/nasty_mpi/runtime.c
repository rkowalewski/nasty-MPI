#include <stdbool.h>
#include <assert.h>
#include <nasty_mpi/runtime.h>
#include <nasty_mpi/init.h>
#include <nasty_mpi/mpi_op.h>
#include <macros/logging.h>
#include <util/random.h>

//if the programmer fires 50 times the same mpi operation without a flush, we forward it to the mpi library...
#define MAX_SIGNATURE_LOOKUP_COUNT 50

static inline int get_origin_rank(MPI_Win win)
{
  void *rank_attr;
  int flag, rank = -1;
  MPI_Win_get_attr(win, KEY_ORIGIN_RANK, &rank_attr, &flag);
  if (flag) rank = (int) (MPI_Aint) rank_attr;
  return rank;
}

static inline int invoke_mpi(MPI_Win win, Nasty_mpi_op *op_info, bool flush)
{

  if (op_info == NULL) return MPI_SUCCESS;
  int rc = -1;

  if (op_info->type == rma_put)
  {
    /*
    debug("--executing actual put---\n"
          "origin_rank: %d\n"
          "origin_addr: %p\n"
          "origin_count: %d\n"
          "origin_datatype: %d\n"
          "target_rank: %d\n"
          "target_disp: %td\n"
          "target_count: %d\n"
          "target_datatype: %d\n",
          get_origin_rank(win),
          op_info->data.put.origin_addr, op_info->data.put.origin_count, op_info->data.put.origin_datatype,
          op_info->data.put.target_rank, op_info->data.put.target_disp, op_info->data.put.target_count, op_info->data.put.target_datatype
         );
    */
    rc = PMPI_Put(
           op_info->data.put.origin_addr, op_info->data.put.origin_count, op_info->data.put.origin_datatype,
           op_info->data.put.target_rank, op_info->data.put.target_disp, op_info->data.put.target_count, op_info->data.put.target_datatype,
           win);

    if (rc == MPI_SUCCESS && flush) {
      return PMPI_Win_flush(win, op_info->data.put.target_rank);
    }
  }
  else if (op_info->type == rma_get)
  {
    /*
    debug("--executing actual get---\n"
          "origin_addr: %p\n"
          "origin_count: %d\n"
          "origin_datatype: %d\n"
          "target_rank: %d\n"
          "target_disp: %td\n"
          "target_count: %d\n"
          "target_datatype: %d\n",
          op_info->data.get.origin_addr, op_info->data.get.origin_count, op_info->data.get.origin_datatype,
          op_info->data.get.target_rank, op_info->data.get.target_disp, op_info->data.get.target_count, op_info->data.get.target_datatype
         );
    */
    rc = PMPI_Get(
           op_info->data.get.origin_addr, op_info->data.get.origin_count, op_info->data.get.origin_datatype,
           op_info->data.get.target_rank, op_info->data.get.target_disp, op_info->data.get.target_count, op_info->data.get.target_datatype,
           win);

    if (rc == MPI_SUCCESS && flush) {
      return PMPI_Win_flush(win, op_info->data.get.target_rank);
    }
  }

  return rc;
}

static inline void split_ops(DArray arr_ops)
{
  for (size_t i = 0; i < (size_t) arr_ops->size; i++) {
    Nasty_mpi_op *op_info = DArray_get(arr_ops, i);

    if (!op_info || !Nasty_mpi_op_is_divisible(op_info)) continue;

    debug("splitting %s!", "operations");

    MPI_Aint lb, extent, disp, base;
    int offset;
    Nasty_mpi_op *new_op;

    if (op_info->type == rma_put) {
      Nasty_mpi_put put = op_info->data.put;
      MPI_Get_address(put.origin_addr, &base);
      MPI_Type_get_extent(put.origin_datatype, &lb, &extent);
      for (disp = put.target_disp, offset = 0; disp < put.target_disp + put.target_count; disp++, offset++) {
        new_op = DArray_new(arr_ops);
        new_op->type = rma_put;
        new_op->data.put = put;
        new_op->data.put.origin_count = 1;
        new_op->data.put.origin_addr = (void*) (base + offset * extent);
        new_op->data.put.target_disp = disp;
        new_op->data.put.target_count = 1;
        Nasty_mpi_op_signature(new_op, &new_op->signature);
        DArray_push(arr_ops, new_op);
      }
    } else if (op_info->type == rma_get) {
      Nasty_mpi_get get = op_info->data.get;
      MPI_Get_address(get.origin_addr, &base);
      MPI_Type_get_extent(get.origin_datatype, &lb, &extent);
      for (disp = get.target_disp, offset = 0; disp < get.target_disp + get.target_count; disp++, offset++) {
        new_op = DArray_new(arr_ops);
        new_op->type = rma_get;
        new_op->data.get = get;
        new_op->data.get.origin_count = 1;
        new_op->data.get.origin_addr = (void*) (base + offset * extent);
        new_op->data.get.target_disp = disp;
        new_op->data.get.target_count = 1;
        Nasty_mpi_op_signature(new_op, &new_op->signature);
        DArray_push(arr_ops, new_op);
      }
    }

    DArray_remove(arr_ops, i);
    DArray_free(op_info);
  }
}

static inline void reorder_ops(DArray arr_ops)
{
  DArray_shuffle(arr_ops);
}

static inline int cache_rma_call(MPI_Win win, Nasty_mpi_op *op)
{
  DArray arr_ops = nasty_win_get_mpi_ops(win);

  if (!arr_ops) return 0;

  Nasty_mpi_op_signature_t signature;
  signature.lookup_count = 0;
  Nasty_mpi_op_signature(op, &signature);
  size_t i;
  for (i = 0; i < (size_t) arr_ops->size; i++) {
    Nasty_mpi_op *cached_op = DArray_get(arr_ops, i);
    if (cached_op && Nasty_mpi_op_signature_equal(&signature, &cached_op->signature)) {
      cached_op->signature.lookup_count++;
      if (cached_op->signature.lookup_count == MAX_SIGNATURE_LOOKUP_COUNT) {
        const char* type_str;
        if (cached_op->type == rma_put)
          type_str = "MPI_Put";
        else if (cached_op->type == rma_get)
          type_str = "MPI_Get";
        else
          type_str = "undefined";

        fprintf(stderr, "The same %s operation has been fired already %d times! Probably a flush is missing!\n", type_str, cached_op->signature.lookup_count);
        //DArray_remove(arr_ops, i);
        /*
        if (!cached_op->is_sent) {
          int rc = invoke_mpi(win, cached_op, false);
          cached_op->is_sent = 1;
          return rc;
        }
        */
        //DArray_free(cached_op);
      }
      return 0;
    }
  }

  Nasty_mpi_op *op_info = DArray_new(arr_ops);

  if (op_info == NULL) return -1;

  op_info->type = op->type;
  op_info->data = op->data;
  op_info->signature = signature;
  op_info->is_sent = 0;

  int res = DArray_push(arr_ops, op_info);

  if (res) {
    DArray_free(op_info);
    return res;
  }

  return 0;
}

int nasty_mpi_handle_op(MPI_Win win, Nasty_mpi_op *op)
{

  int res = 0;
  if (op == NULL) return res;

  Nasty_mpi_config config = get_nasty_mpi_config();

  Submit_time submit_time = config.time;

  if (submit_time == random_choice) {
    submit_time = random_seq() % 3;
  }

  if (submit_time == maximum_delay) {
    res = cache_rma_call(win, op);
  } else {
    bool flush = submit_time == fire_and_sync;
    res = invoke_mpi(win, op, flush);
  }

  return (res == 0)  ? MPI_SUCCESS : res;
}

int filter_by_rank(void* el, void* args)
{
  if (!el || !args) return 0;

  int rank = * (int*) args;
  Nasty_mpi_op *op = (Nasty_mpi_op *) el;
  if (op->type == rma_put)
    return op->data.put.target_rank == rank;
  else if (op->type == rma_get)
    return op->data.get.target_rank == rank;

  return 0;
}

int nasty_mpi_execute_cached_calls(MPI_Win win, int rank)
{
  DArray all_ops = nasty_win_get_mpi_ops(win);
  if (!all_ops || all_ops->size == 0) return MPI_SUCCESS;

  DArray ops_to_process = (rank != EXECUTE_OPS_OF_ANY_RANK) ? DArray_filter(all_ops, filter_by_rank, &rank) : all_ops;

  Nasty_mpi_config config = get_nasty_mpi_config();

  if (config.split_rma_ops)
    split_ops(ops_to_process);

  reorder_ops(ops_to_process);

  size_t i;
  int res = MPI_SUCCESS;
  for (i = 0; i < (size_t) ops_to_process->size && res == MPI_SUCCESS; i++)
  {
    Nasty_mpi_op *op_info = DArray_get(ops_to_process, i);
    res = invoke_mpi(win, op_info, false);

    if (res != MPI_SUCCESS) {
      debug("rank %d could not execute all intercepted mpi rma calls", get_origin_rank(win));
    }
  }

  if (ops_to_process != all_ops) {
    DArray_remove_all(all_ops, ops_to_process);
    DArray_clear_destroy(ops_to_process);
  } else {
    DArray_clear(all_ops);
  }

  return res;
}
