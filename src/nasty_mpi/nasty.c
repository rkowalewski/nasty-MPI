#include <nasty_mpi/nasty.h>
#include <collections/kvs.h>
#include <stdio.h>

static KVstore store = NULL;

#define cond_mpi_call_success(result) \
  if ((result) == MPI_SUCCESS)

static void free_entry(void *data)
{
  KVentry entry = (KVentry) data;
  free(entry);
}

int MPI_Init(int *argc, char ***argv)
{
  int result = PMPI_Init(argc, argv);

  if (result == MPI_SUCCESS)
  {
    store = kvs_create(5, 5, free_entry);
  }

  return result;
}

int MPI_Put(const void *origin_addr, int origin_count, MPI_Datatype origin_datatype,
            int target_rank, MPI_Aint target_disp, int target_count,
            MPI_Datatype target_datatype, MPI_Win win)
{

  return PMPI_Put(origin_addr, origin_count, origin_datatype,
                  target_rank, target_disp, target_count, target_datatype,
                  win);
}

int MPI_Get(void *origin_addr, int origin_count, MPI_Datatype origin_datatype,
            int target_rank, MPI_Aint target_disp, int target_count,
            MPI_Datatype target_datatype, MPI_Win win)
{
  return PMPI_Get(origin_addr, origin_count, origin_datatype,
                  target_rank, target_disp, target_count, target_datatype,
                  win);
}

int MPI_Win_lock_all(int assert, MPI_Win win)
{
  return PMPI_Win_lock_all(assert, win);
}

int MPI_Win_unlock_all(MPI_Win win)
{
  return MPI_Win_unlock_all(win);
}

int MPI_Finalize(void)
{
  return PMPI_Finalize();
}
