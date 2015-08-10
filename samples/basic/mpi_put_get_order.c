#include <mpi.h>
#include <assert.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#define COUNT 6
#define MEM_RANK 1
enum {disp_guard = 0, disp_payload = 1};
int main(int argc, char** argv)
{
  int myrank;
  MPI_Win win;
  int *baseptr;
  MPI_Aint local_size;

  MPI_Init(&argc, &argv);

  MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
  
  local_size = (myrank == MEM_RANK) ? COUNT : 0;

  MPI_Win_allocate(local_size * sizeof(int), sizeof(int), MPI_INFO_NULL, MPI_COMM_WORLD, &baseptr, &win);

  if (myrank == MEM_RANK)
    for (size_t i = 3; i < COUNT; i++)
      baseptr[i] = i*99;

  MPI_Barrier(MPI_COMM_WORLD);

  MPI_Win_lock_all(0, win);

  if (myrank == 0) {
    int put[3] = {1, 42, 10};
    int get[3];
    MPI_Put( put, 3, MPI_INT, MEM_RANK, 0, 3, MPI_INT, win);
    MPI_Get( get, 3, MPI_INT, MEM_RANK, 3, 3, MPI_INT, win);

    MPI_Win_flush(MEM_RANK, win);

    for (int i = 0; i < 3; i++)
      assert(get[i] == (i+3)*99);
  } 

  MPI_Win_unlock_all(win);

  MPI_Barrier(MPI_COMM_WORLD);


  if (myrank == MEM_RANK) {
    assert(baseptr[disp_guard] == 1);
    assert(baseptr[disp_payload] == 42);
    assert(baseptr[2] == 10);
  }

  MPI_Win_free(&win);
  MPI_Finalize();
  return EXIT_SUCCESS;
}

