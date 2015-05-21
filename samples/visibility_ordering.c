#include <mpi.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>

#define COUNT 2
enum {disp_guard = 0, disp_payload = 1};

void execute_rma(int myrank, MPI_Win win)
{
  int guard, payload;

  MPI_Win_lock_all(0, win);

  if ( myrank == 0 )
  {
    guard = 1;
    payload = 42;

    //Do busy work
    sleep(5);

    //Keep order of puts through flushes
    MPI_Put( &payload, 1, MPI_INT, 2, disp_payload, 1, MPI_INT, win);

    //MPI_Win_flush(2, win);

    MPI_Put( &guard, 1, MPI_INT, 2, disp_guard, 1, MPI_INT, win);

    //MPI_Win_flush(2, win);
  }

  if ( myrank == 1 )
  {
    while (!guard )
    {
      //Poll until the flag is true (possible in the unified model)
      MPI_Get( &guard, 1, MPI_INT, 2, disp_guard, 1, MPI_INT, win );
      //MPI_Win_flush_local(myrank, win);
    }

    MPI_Get( &payload, 1, MPI_INT, 2, disp_payload, 1, MPI_INT, win);
    assert(payload == 42);
  }

  MPI_Win_unlock_all(win);

  //The unlock guarantees completion of the last Remote-Get operation, so the value must equal '42'
  //if (myrank == 1) assert(payload == 42);
}
int main(int argc, char *argv[])
{
  int myrank, nprocs;
  MPI_Win win;
  int *baseptr;
  MPI_Aint local_size;

  MPI_Init(&argc, &argv);

  MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

  local_size = (myrank == 2) ? COUNT : 0;

  MPI_Win_allocate(local_size * sizeof(int), sizeof(int), MPI_INFO_NULL, MPI_COMM_WORLD, &baseptr, &win);

  if (myrank != 2)
  {
    execute_rma(myrank, win);
  }

  MPI_Win_free(&win);
  MPI_Finalize();
  return EXIT_SUCCESS;
}
