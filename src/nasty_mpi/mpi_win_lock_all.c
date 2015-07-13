#include <nasty_mpi/nasty.h>
int MPI_Win_lock_all(int assert, MPI_Win win)
{
  int result = PMPI_Win_lock_all(assert, win);

  if (result == MPI_SUCCESS)
  {
    char win_name[NASTY_ID_LEN + 1];
    win_get_nasty_id(win, win_name);
    DArray arr_ops = kvs_get(store, win_name);

    if (!arr_ops)
    {
      arr_ops = DArray_create(sizeof(Nasty_mpi_op), 10);
      kvs_put(store, win_name, arr_ops);
    }
  }

  return result;
}
