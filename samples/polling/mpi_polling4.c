#include "../testbench.h"

#define COUNT 2

enum {disp_guard = 0, disp_payload = 1};
void run_rma_test(int nprocs_per_node)
{
  int myrank, nprocs;
  MPI_Win win;
  int *baseptr;
  MPI_Aint local_size;

  MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

  if (nprocs < nprocs_per_node * 3)
  {
    if (!myrank) printf("should start program with at least %d processes\n", nprocs_per_node * 3);
    MPI_Finalize();
    exit(EXIT_FAILURE);
  }
  int mem_rank = nprocs_per_node * 2 + nprocs_per_node / 2;
  //int receiver_rank =  mem_rank - nprocs_per_node;
  int receiver_rank = nprocs_per_node / 2;

  local_size = (myrank == mem_rank) ? COUNT : 0;

  MPI_Win_allocate(local_size * sizeof(int), sizeof(int), MPI_INFO_NULL, MPI_COMM_WORLD, &baseptr, &win);

  //initialize memory
  if (myrank == mem_rank)
    for (size_t i = 0; i < COUNT; i++) {
      baseptr[i] = 0;
    }

  MPI_Barrier(MPI_COMM_WORLD);

  MPI_Win_lock_all(0, win);

  if (!myrank) {
    int payload = 42;
    int flag = 1;
    MPI_Put( &payload, 1, MPI_INT, mem_rank, disp_payload, 1, MPI_INT, win);
    //Flush 1
    //MPI_Win_flush(mem_rank, win);

    MPI_Put( &flag, 1, MPI_INT, mem_rank, disp_guard, 1, MPI_INT, win);
    //Flush 2
    MPI_Win_flush(mem_rank, win);

  }
  if (myrank == receiver_rank) {
    int guard = 0, value;
    while (!guard)
    {
      MPI_Get(&guard, 1, MPI_INT, mem_rank, disp_guard, 1, MPI_INT, win);
      //Flush 3
      MPI_Win_flush(mem_rank, win);
    }
    MPI_Get(&value, 1, MPI_INT, mem_rank, disp_payload, 1, MPI_INT, win);
    //Flush 4
    MPI_Win_flush(mem_rank, win);
    assert(value == 42);
  }
  
  if (myrank == mem_rank) {
    //do nothing
  }

  MPI_Win_unlock_all(win);

  MPI_Barrier(MPI_COMM_WORLD);

  MPI_Win_free(&win);
}
