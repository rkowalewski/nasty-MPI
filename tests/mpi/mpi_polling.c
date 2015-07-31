#include <mpi.h>
#include <assert.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#define COUNT 2
#define MEM_RANK 40

enum {disp_guard = 0, disp_payload = 1};
int main(int argc, char** argv)
{
  int myrank/*, nprocs*/;
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

  MPI_Barrier(MPI_COMM_WORLD);

  MPI_Win_lock_all(0, win);

  if (myrank == 0) {
    printf("Hello world from processor %s on rank %d\n", processor_name, myrank);
    int payload = 42;
    int flag = 1;

    MPI_Put( &payload, 1, MPI_INT, MEM_RANK, disp_payload, 1, MPI_INT, win);
    //Flush 1
    MPI_Win_flush(MEM_RANK, win);
    
    MPI_Put( &flag, 1, MPI_INT, MEM_RANK, disp_guard, 1, MPI_INT, win);
    //Flush 2
    MPI_Win_flush(MEM_RANK, win);

  } else if (myrank == 16) {
    printf("Hello world from processor %s on rank %d\n", processor_name, myrank);
    int guard = 0, value;
    while (!guard)
    {
      MPI_Get(&guard, 1, MPI_INT, MEM_RANK, disp_guard, 1, MPI_INT, win);
      //Flush 3
      //MPI_Win_flush(MEM_RANK, win);
    }
    MPI_Get(&value, 1, MPI_INT, MEM_RANK, disp_payload, 1, MPI_INT, win);
    //Flush 4
    MPI_Win_flush(MEM_RANK, win);
    assert(value == 42);
  } else if (myrank == MEM_RANK) {
    printf("Hello world from processor %s on rank %d\n", processor_name, myrank);
  }

  MPI_Win_unlock_all(win);
  MPI_Barrier(MPI_COMM_WORLD);
  MPI_Win_free(&win);
  MPI_Finalize();
  return EXIT_SUCCESS;
}
