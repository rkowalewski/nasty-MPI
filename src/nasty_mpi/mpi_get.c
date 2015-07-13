#include <nasty_mpi/nasty.h>

#define _map_nasty_get(x) \
  (x) = malloc(sizeof(Nasty_mpi_get)); \
  assert(x); \
  ((Nasty_mpi_get *)(x))->origin_addr = origin_addr; \
  ((Nasty_mpi_get *)(x))->origin_count  = origin_count; \
  ((Nasty_mpi_get *)(x))->origin_datatype = origin_datatype; \
  ((Nasty_mpi_get *)(x))->target_rank = target_rank; \
  ((Nasty_mpi_get *)(x))->target_disp = target_disp; \
  ((Nasty_mpi_get *)(x))->target_count = target_count; \
  ((Nasty_mpi_get *)(x))->target_datatype = target_datatype;


int MPI_Get(void *origin_addr, int origin_count, MPI_Datatype origin_datatype,
            int target_rank, MPI_Aint target_disp, int target_count,
            MPI_Datatype target_datatype, MPI_Win win)
{
  char win_name[NASTY_ID_LEN + 1];
  fetch_nasty_win_id(win, win_name);
  DArray arr_ops = kvs_get(store, win_name);

  if (arr_ops)
  {
    Nasty_mpi_get *nasty_get;
    _map_nasty_get(nasty_get);

    Nasty_mpi_op *op_info = DArray_new(arr_ops);
    op_info->type = OP_GET;
    op_info->data = nasty_get;

    DArray_push(arr_ops, op_info);
    return MPI_SUCCESS;
  }
  else
  {
    return PMPI_Get(origin_addr, origin_count, origin_datatype,
                    target_rank, target_disp, target_count, target_datatype,
                    win);
  }
}
