#include <mpi.h>
#include <nasty_mpi/nasty.h>
#include <collections/kvs.h>
#include <stdio.h>


int MPI_Init(int *argc, char ***argv)
{
  printf("Setup...\n");
  fflush(stdout);

  return PMPI_Init(argc, argv);
}

int MPI_Put(const void *origin_addr, int origin_count, MPI_Datatype origin_datatype,
            int target_rank, MPI_Aint target_disp, int target_count,
            MPI_Datatype target_datatype, MPI_Win win)
{

  return PMPI_Put(origin_addr, origin_count, origin_datatype,
                  target_rank, target_disp, target_count, target_datatype,
                  win);
}

int MPI_Get(void *origin_addr, int origin_count, MPI_Datatype origin_datatype,
            int target_rank, MPI_Aint target_disp, int target_count,
            MPI_Datatype target_datatype, MPI_Win win)
{
  return PMPI_Get(origin_addr, origin_count, origin_datatype,
                 target_rank, target_disp, target_count, target_datatype,
                 win);
}

int MPI_Win_lock_all(int assert, MPI_Win win) {
  return PMPI_Win_lock_all(assert, win);
}

int MPI_Win_unlock_all(MPI_Win win) {
  return MPI_Win_unlock_all(win);
}

int MPI_Finalize(void)
{
  printf("Setup...\n");
  fflush(stdout);
  return PMPI_Finalize();
}
