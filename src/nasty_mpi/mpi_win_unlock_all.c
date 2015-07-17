#include <nasty_mpi.h>

static inline int execute_nasty_op(MPI_Win win, Nasty_mpi_op *op_info)
{
  if (!win) return -1;
  if (op_info->type == OP_PUT)
  {
    Nasty_mpi_put *put = op_info->data;
    debug("--executing actual put---\n"
        "origin_addr: %p\n"
        "origin_count: %d\n"
        "origin_datatype: %d\n"
        "target_rank: %d\n"
        "target_disp: %td\n"
        "target_count: %d\n"
        "target_datatype: %d\n",
        put->origin_addr, put->origin_count, put->origin_datatype,
        put->target_rank, put->target_disp, put->target_count, put->target_datatype);


    return PMPI_Put(put->origin_addr, put->origin_count, put->origin_datatype,
                    put->target_rank, put->target_disp, put->target_count, put->target_datatype,
                    win);
  }
  else if (op_info->type == OP_GET)
  {
    Nasty_mpi_get *get = op_info->data;
    return PMPI_Get(get->origin_addr, get->origin_count, get->origin_datatype,
                    get->target_rank, get->target_disp, get->target_count, get->target_datatype,
                    win);
  }

  return -1;
}


int MPI_Win_unlock_all(MPI_Win win)
{
  char win_name[NASTY_ID_LEN + 1];
  fetch_nasty_win_id(win, win_name);
  DArray arr_ops = kvs_get(store, win_name);

  if (arr_ops)
  {
    DArray_shuffle(arr_ops);

    int i;
    for (i = 0; i < arr_ops->size; i++)
    {
      Nasty_mpi_op *op_info = DArray_remove(arr_ops, i);
      execute_nasty_op(win, op_info);
      free(op_info->data);
      free(op_info);
    }
  }


  return PMPI_Win_unlock_all(win);
}

