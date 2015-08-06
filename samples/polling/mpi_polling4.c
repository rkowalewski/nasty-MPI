#include <mpi.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

#define COUNT 2
#define SENDER 0
#define RECEIVER 20
#define MEM_RANK 40
#define MEM_COUNT 2

enum {disp_guard = 0, disp_payload = 1};
int main(int argc, char** argv)
{
  int myrank/*, nprocs*/;
  MPI_Win win;
  int *baseptr;
  MPI_Aint local_size;

  MPI_Init(&argc, &argv);

  MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

  local_size = (myrank == MEM_RANK) ? MEM_COUNT : 0;

  MPI_Win_allocate(local_size * sizeof(int), sizeof(int), MPI_INFO_NULL, MPI_COMM_WORLD, &baseptr, &win);

  //initialize memory
  if (myrank == MEM_RANK)
    for (size_t i = 0; i < MEM_COUNT; i++) {
      baseptr[i] = 0;
    }

  MPI_Barrier(MPI_COMM_WORLD);

  MPI_Win_lock_all(0, win);

  if (myrank == SENDER) {
    int payload = 42;
    int flag = 1;

    MPI_Put( &payload, 1, MPI_INT, MEM_RANK, disp_payload, 1, MPI_INT, win);
    MPI_Put( &flag, 1, MPI_INT, MEM_RANK, disp_guard, 1, MPI_INT, win);
    //Flush 1
    MPI_Win_flush(MEM_RANK, win);

    //Flush 2

  } else if (myrank == RECEIVER) {
    int guard = 0, value;
    while (!guard)
    {
    //nanosleep(&ts, NULL);
      MPI_Get(&guard, 1, MPI_INT, MEM_RANK, disp_guard, 1, MPI_INT, win);
      //Flush 3
      MPI_Win_flush(MEM_RANK, win);
    }
    MPI_Get(&value, 1, MPI_INT, MEM_RANK, disp_payload, 1, MPI_INT, win);
    //Flush 4
    MPI_Win_flush(MEM_RANK, win);
    assert(value == 42);
  } else if (myrank == MEM_RANK) {
    //do nothing
  }

  MPI_Win_unlock_all(win);
  MPI_Barrier(MPI_COMM_WORLD);

  MPI_Win_free(&win);
  MPI_Finalize();
  return EXIT_SUCCESS;
}
