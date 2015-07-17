#include <nasty_mpi.h>

int MPI_Win_allocate(MPI_Aint size, int disp_unit, MPI_Info info,
                     MPI_Comm comm, void *baseptr, MPI_Win *win)
{
  int result = PMPI_Win_allocate(size, disp_unit, info, comm, baseptr, win);

  if (result == MPI_SUCCESS)
  {
    result = init_nasty_win_id(*win);
  }

  return result;
}
