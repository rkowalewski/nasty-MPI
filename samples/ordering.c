#include <mpi.h>
#include <assert.h>
#include <unistd.h>
#include <stdlib.h>

#define COUNT 1
int main(int argc, char** argv)
{
  int myrank, nprocs;
  MPI_Win win;
  int *baseptr;
  MPI_Aint local_size;

  MPI_Init(&argc, &argv);

  MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

  local_size = (myrank == 2) ? COUNT : 0;

  MPI_Win_allocate(local_size * sizeof(int), sizeof(int), MPI_INFO_NULL, MPI_COMM_WORLD, &baseptr, &win);


  if (myrank == 0) {
    int payload = 42;
    MPI_Put( &payload, 1, MPI_INT, 2, 0, 1, MPI_INT, win);
  } else if (myrank == 1) {
    int value;
    sleep(1);
    MPI_Get( &value, 1, MPI_INT, 2, 0, 1, MPI_INT, win);
    assert(value == 42);
  }


  MPI_Win_free(&win);
  MPI_Finalize();
  return EXIT_SUCCESS;
}
