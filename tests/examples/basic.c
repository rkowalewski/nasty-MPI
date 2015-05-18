#include <mpi.h>

int main(int argc, char *argv[]) {
  int rank;
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  if (!rank) {
    int send = 10;
    MPI_Send(&send, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
  } else {
    int value;
    MPI_Recv(&value, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  }
  MPI_Finalize();
  return 0;
}
