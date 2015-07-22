#include <stdbool.h>
#include <assert.h>
#include <nasty_mpi/runtime.h>
#include <nasty_mpi/init.h>
#include <macros/logging.h>
#include <util/random.h>

static inline int get_origin_rank(MPI_Win win)
{
  void *rank_attr;
  int flag, rank = -1;
  MPI_Win_get_attr(win, KEY_ORIGIN_RANK, &rank_attr, &flag);
  if (flag) rank = (int) (MPI_Aint) rank_attr;
  return rank;
}

static inline int invoke_mpi(MPI_Win win, Nasty_mpi_op op_info, bool flush)
{
  int rc = -1;

  if (op_info.type == rma_put)
  {
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
          op_info.data.put.origin_addr, op_info.data.put.origin_count, op_info.data.put.origin_datatype,
          op_info.data.put.target_rank, op_info.data.put.target_disp, op_info.data.put.target_count, op_info.data.put.target_datatype
         );

    rc = PMPI_Put(
               op_info.data.put.origin_addr, op_info.data.put.origin_count, op_info.data.put.origin_datatype,
               op_info.data.put.target_rank, op_info.data.put.target_disp, op_info.data.put.target_count, op_info.data.put.target_datatype,
               win);

    if (rc == MPI_SUCCESS && flush) {
      return PMPI_Win_flush(win, op_info.data.put.target_rank);
    }
  }
  else if (op_info.type == rma_get)
  {
    debug("--executing actual get---\n"
          "origin_addr: %p\n"
          "origin_count: %d\n"
          "origin_datatype: %d\n"
          "target_rank: %d\n"
          "target_disp: %td\n"
          "target_count: %d\n"
          "target_datatype: %d\n",
          op_info.data.get.origin_addr, op_info.data.get.origin_count, op_info.data.get.origin_datatype,
          op_info.data.get.target_rank, op_info.data.get.target_disp, op_info.data.get.target_count, op_info.data.get.target_datatype
         );
    rc = PMPI_Get(
               op_info.data.get.origin_addr, op_info.data.get.origin_count, op_info.data.get.origin_datatype,
               op_info.data.get.target_rank, op_info.data.get.target_disp, op_info.data.get.target_count, op_info.data.get.target_datatype,
               win);

    if (rc == MPI_SUCCESS && flush) {
      return PMPI_Win_flush(win, op_info.data.get.target_rank);
    }
  }

  return rc;
}

static inline int execute_ops_intern(MPI_Win win, DArray arr_ops, bool flush)
{
  if (!arr_ops) return -1;

  size_t i; int res;
  for (i = 0; i < (size_t) arr_ops->size; i++)
  {
    Nasty_mpi_op *op_info = DArray_remove(arr_ops, i);
    res = invoke_mpi(win, *op_info, flush);
    free(op_info);

    if (res != MPI_SUCCESS) {
      debug("rank %d could not execute all intercepted mpi rma calls", get_origin_rank(win));
      return res;
    }
  }

  return MPI_SUCCESS;
}

static inline int cache_rma_call(MPI_Win win, Nasty_mpi_op op)
{
  DArray arr_ops = get_rma_ops(win);

  if (arr_ops)
  {
    Nasty_mpi_op *op_info = DArray_new(arr_ops);

    if (op_info == NULL) return 1;

    *op_info = op;

    int res = DArray_push(arr_ops, op_info);

    if (res) {
      free(op_info);
    }

    return res;
  }

  return 1;
}

int handle_rma_call(MPI_Win win, Nasty_mpi_op op)
{

  int res = -1;

  Nasty_mpi_config config = get_nasty_mpi_config();

  if (config.time == maximum_delay) {
    debug("caching call");
    res = cache_rma_call(win, op);
  } else {
    bool flush = false;
    if (config.time == fire_and_sync) {
      flush = true;
    } else if (config.time == random_choice) {
      unsigned int rand_uint = random_seq();
      flush = rand_uint % 2;
    }
    
    res = invoke_mpi(win, op, flush);
  }

  return (res == 0)  ? MPI_SUCCESS : res;
}

int execute_cached_calls(MPI_Win win)
{
  DArray arr_ops = get_rma_ops(win);

  DArray_shuffle(arr_ops);

  return execute_ops_intern(win, arr_ops, false);
}
