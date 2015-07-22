#include <nasty_mpi/runtime.h>
#include <nasty_mpi/init.h>
#include <macros/logging.h>
#include <assert.h>

static inline int get_origin_rank(MPI_Win win)
{
  void *rank_attr;
  int flag, rank = -1;
  MPI_Win_get_attr(win, KEY_ORIGIN_RANK, &rank_attr, &flag);
  if (flag) rank = (int) (MPI_Aint) rank_attr;
  return rank;
}

static inline int execute_nasty_op(MPI_Win win, Nasty_mpi_op *op_info)
{
  if (!op_info) return MPI_SUCCESS;

  if (op_info->type == rma_put)
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
          op_info->data.put.origin_addr, op_info->data.put.origin_count, op_info->data.put.origin_datatype,
          op_info->data.put.target_rank, op_info->data.put.target_disp, op_info->data.put.target_count, op_info->data.put.target_datatype
         );


    return PMPI_Put(
             op_info->data.put.origin_addr, op_info->data.put.origin_count, op_info->data.put.origin_datatype,
             op_info->data.put.target_rank, op_info->data.put.target_disp, op_info->data.put.target_count, op_info->data.put.target_datatype,
             win);
  }
  else if (op_info->type == rma_get)
  {
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
    return PMPI_Get(
             op_info->data.get.origin_addr, op_info->data.get.origin_count, op_info->data.get.origin_datatype,
             op_info->data.get.target_rank, op_info->data.get.target_disp, op_info->data.get.target_count, op_info->data.get.target_datatype,
             win);
  }

  return MPI_SUCCESS;
}

int execute_cached_calls(MPI_Win win)
{
  DArray arr_ops = get_rma_ops(win);

  //Submit_time submit_time = get_submit_time();
  //Submit_order submit_order = get_submit_order();


  DArray_shuffle(arr_ops);

  int i, res;
  for (i = 0; i < arr_ops->size; i++)
  {
    Nasty_mpi_op *op_info = DArray_remove(arr_ops, i);
    res = execute_nasty_op(win, op_info);
    free(op_info);

    if (res != MPI_SUCCESS) {
      debug("rank %d could not execute all intercepted mpi rma calls", get_origin_rank(win));
      return res;
    }
  }

  return MPI_SUCCESS;
}
