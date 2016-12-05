#include "../testbench.h"
#define COUNT 3
enum {disp_guard = 0, disp_payload = 1};
void run_rma_test(int nprocs_per_node)
{
  int myrank, nprocs;
  int mem_rank;
  MPI_Win win;
  int *baseptr;
  MPI_Aint local_size;

  MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

  if (nprocs < nprocs_per_node * 2)
  {
    if (!myrank) printf("should start program with at least %d processes\n", nprocs_per_node * 2);
    MPI_Finalize();
    exit(EXIT_FAILURE);
  }

  mem_rank = nprocs_per_node + nprocs_per_node / 2;

  local_size = (myrank == mem_rank) ? COUNT : 0;

  MPI_Win_allocate(local_size * sizeof(int), sizeof(int), MPI_INFO_NULL, MPI_COMM_WORLD, &baseptr, &win);

  MPI_Win_lock_all(0, win);

  if (myrank == 0)
  {
    int values[COUNT] = {1, 42, 10};
    MPI_Put( values, COUNT, MPI_INT, mem_rank, 0, COUNT, MPI_INT, win);
  }

  MPI_Win_unlock_all(win);

  MPI_Barrier(MPI_COMM_WORLD);

  if (myrank == mem_rank)
  {
    assert(baseptr[disp_guard] == 1);
    assert(baseptr[disp_payload] == 42);
    assert(baseptr[2] == 10);
  }

  MPI_Win_free(&win);
}

