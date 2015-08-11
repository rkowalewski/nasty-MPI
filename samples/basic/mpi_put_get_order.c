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

  if (nprocs < nprocs_per_node * 2)
  {
    if (!myrank) printf("should start program with at least %d processes\n", nprocs_per_node * 2);
    MPI_Finalize();
    exit(EXIT_FAILURE);
  }
  int mem_rank = nprocs_per_node + nprocs_per_node / 2;

  local_size = (myrank == mem_rank) ? COUNT : 0;

  MPI_Win_allocate(local_size * sizeof(int), sizeof(int), MPI_INFO_NULL, MPI_COMM_WORLD, &baseptr, &win);

  if (myrank == mem_rank)
    for (size_t i = 3; i < COUNT; i++)
      baseptr[i] = i * 99;

  MPI_Barrier(MPI_COMM_WORLD);

  MPI_Win_lock_all(0, win);

  if (myrank == 0)
  {
    int put[3] = {1, 42, 10};
    int get[3];
    MPI_Put( put, 3, MPI_INT, mem_rank, 0, 3, MPI_INT, win);
    MPI_Get( get, 3, MPI_INT, mem_rank, 3, 3, MPI_INT, win);

    MPI_Win_flush(mem_rank, win);

    for (int i = 0; i < 3; i++)
      assert(get[i] == (i + 3) * 99);
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

