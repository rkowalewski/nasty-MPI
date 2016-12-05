#include "../testbench.h"
#include <unistd.h>
#define COUNT 10
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

  MPI_Win_create_dynamic(MPI_INFO_NULL, MPI_COMM_WORLD, &win);

  MPI_Win_lock_all(0, win);

  //MPI_Win_allocate(local_size * sizeof(int), sizeof(int), MPI_INFO_NULL, MPI_COMM_WORLD, &baseptr, &win);


  int type_size;
  MPI_Type_size(MPI_INT, &type_size);

  size_t nbytes = COUNT * type_size;

  assert(MPI_Alloc_mem(nbytes, MPI_INFO_NULL, &baseptr) == MPI_SUCCESS);
  assert(MPI_Win_attach(win, baseptr, nbytes) == MPI_SUCCESS);

  MPI_Aint ldisp;
  MPI_Aint *disps = malloc(nprocs * sizeof(MPI_Aint));

  assert(MPI_Get_address(baseptr, &ldisp) == MPI_SUCCESS);

  assert(MPI_Allgather(&ldisp, 1, MPI_AINT, disps, nprocs, MPI_AINT, MPI_COMM_WORLD) == MPI_SUCCESS);


  if (myrank == 0)
  {
    for (size_t idx = 0; idx < COUNT; ++idx) {
      baseptr[idx] = idx * COUNT + 1;
    }
//    MPI_Put( values, COUNT, MPI_INT, mem_rank, 0, COUNT, MPI_INT, win);
  }

  MPI_Barrier(MPI_COMM_WORLD);

  if (myrank == mem_rank) {
    assert(MPI_Get(baseptr, 10, MPI_INT, 0, disps[0], 10, MPI_INT, win) == MPI_SUCCESS);
    assert(MPI_Win_flush(0, win) == MPI_SUCCESS);

    for (size_t idx = 0; idx < COUNT; ++idx) {
      assert(baseptr[idx] == idx * 10 + 1);
    }
  }

  MPI_Barrier(MPI_COMM_WORLD);

  MPI_Win_unlock_all(win);

  MPI_Barrier(MPI_COMM_WORLD);

  MPI_Win_free(&win);

  printf("Test finished\n");
}

