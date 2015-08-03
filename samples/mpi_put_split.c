#include <mpi.h>
#include <assert.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#define COUNT 3
#define MEM_RANK 1
enum {disp_guard = 0, disp_payload = 1};
int main(int argc, char** argv)
{
  int myrank;
  MPI_Win win;
  int *baseptr;
  MPI_Aint local_size;

  MPI_Init(&argc, &argv);

  char processor_name[MPI_MAX_PROCESSOR_NAME];
  int name_len;
  MPI_Get_processor_name(processor_name, &name_len);

  MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
  
  local_size = (myrank == MEM_RANK) ? COUNT : 0;

  MPI_Win_allocate(local_size * sizeof(int), sizeof(int), MPI_INFO_NULL, MPI_COMM_WORLD, &baseptr, &win);

  MPI_Win_lock_all(0, win);

  if (myrank == 0) {
    int values[3] = {1, 42, 10};
    MPI_Put( values, 3, MPI_INT, MEM_RANK, 0, 3, MPI_INT, win);
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

