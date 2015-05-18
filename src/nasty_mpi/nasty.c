#include <nasty_mpi/nasty.h>
#include <collections/kvs.h>
#include <stdio.h>


int MPI_Init(int *argc, char ***argv) {
  printf("Setup...\n");
  fflush(stdout);
  return MPI_SUCCESS;
}

int MPI_Put(const void *origin_addr, int origin_count, MPI_Datatype origin_datatype,
            int target_rank, MPI_Aint target_disp, int target_count,
            MPI_Datatype target_datatype, MPI_Win win) {

return MPI_SUCCESS;
}

int MPI_Finalize(void) {
  printf("Setup...\n");
  fflush(stdout);
  return MPI_SUCCESS;
}
