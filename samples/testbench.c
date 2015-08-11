#include "testbench.h"

int main(int argc, char** argv)
{
  int myrank, nprocs, nprocs_per_node;

  MPI_Init(&argc, &argv);

  MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

  if (argc < 2)
  {
    if (!myrank) printf("usage: ./executable <nprocs_per_node>\n");
    MPI_Finalize();
    return EXIT_FAILURE;
  }

  if (!myrank)
  {
    nprocs_per_node = atoi(argv[1]);
  }

  MPI_Bcast(&nprocs_per_node, 1, MPI_INT, 0, MPI_COMM_WORLD);

  run_rma_test(nprocs_per_node);

  MPI_Finalize();
  return EXIT_SUCCESS;
}

