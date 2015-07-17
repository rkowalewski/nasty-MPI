#include <nasty_mpi.h>

#define _map_nasty_put(x) \
  (x) = malloc(sizeof(Nasty_mpi_put)); \
  assert(x); \
  ((Nasty_mpi_put *)(x))->origin_addr = origin_addr; \
  ((Nasty_mpi_put *)(x))->origin_count  = origin_count; \
  ((Nasty_mpi_put *)(x))->origin_datatype = origin_datatype; \
  ((Nasty_mpi_put *)(x))->target_rank = target_rank; \
  ((Nasty_mpi_put *)(x))->target_disp = target_disp; \
  ((Nasty_mpi_put *)(x))->target_count = target_count; \
  ((Nasty_mpi_put *)(x))->target_datatype = target_datatype;

int MPI_Put(const void *origin_addr, int origin_count, MPI_Datatype origin_datatype,
            int target_rank, MPI_Aint target_disp, int target_count,
            MPI_Datatype target_datatype, MPI_Win win)
{

  char win_name[NASTY_ID_LEN + 1];
  fetch_nasty_win_id(win, win_name);
  DArray arr_ops = kvs_get(store, win_name);

  debug("executing nasty put");

  if (arr_ops)
  {
    Nasty_mpi_put *nasty_put = NULL;
    _map_nasty_put(nasty_put);
    Nasty_mpi_op *op_info = DArray_new(arr_ops);
    op_info->type = OP_PUT;
    op_info->data = nasty_put;

    DArray_push(arr_ops, op_info);

    return MPI_SUCCESS;
  }
  else
  {
    return PMPI_Put(origin_addr, origin_count, origin_datatype,
                    target_rank, target_disp, target_count, target_datatype,
                    win);
  }
}
