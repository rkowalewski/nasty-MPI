#include <nasty_mpi.h>
#include <assert.h>


#define _map_put_get_attrs(x) \
  (x).origin_addr = origin_addr; \
  (x).origin_count  = origin_count; \
  (x).origin_datatype = origin_datatype; \
  (x).target_rank = target_rank; \
  (x).target_disp = target_disp; \
  (x).target_count = target_count; \
  (x).target_datatype = target_datatype;

int MPI_Init(int *argc, char ***argv)
{
  int result = PMPI_Init(argc, argv);

  if (result == MPI_SUCCESS) {
    nasty_mpi_init(argc, argv);
  }

  return result;
}

int MPI_Win_allocate(MPI_Aint size, int disp_unit, MPI_Info info,
                     MPI_Comm comm, void *baseptr, MPI_Win *win)
{
  int result = PMPI_Win_allocate(size, disp_unit, info, comm, baseptr, win);

  if (result == MPI_SUCCESS)
  {
    nasty_win_init(*win, comm);
  }

  return result;
}

int MPI_Win_lock_all(int assert, MPI_Win win)
{
  int rc = PMPI_Win_lock_all(assert, win);

  if (rc == MPI_SUCCESS) nasty_win_lock(win);

  return rc;
}

int MPI_Put(const void *origin_addr, int origin_count, MPI_Datatype origin_datatype,
            int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype,
            MPI_Win win)
{

  debug("--caching put---\n"
        "origin_addr: %p\n"
        "origin_count: %d\n"
        "origin_datatype: %d\n"
        "target_rank: %d\n"
        "target_disp: %td\n"
        "target_count: %d\n"
        "target_datatype: %d\n",
        origin_addr, origin_count, origin_datatype,
        target_rank, target_disp, target_count, target_datatype
       );

  Nasty_mpi_op op_info;
  op_info.type = rma_put;
  _map_put_get_attrs(op_info.data.put);

  if (handle_rma_call(win, op_info) != MPI_SUCCESS) {
    return PMPI_Put(origin_addr, origin_count, origin_datatype,
                    target_rank, target_disp, target_count, target_datatype,
                    win);

  }

  return MPI_SUCCESS;
}

int MPI_Get(void *origin_addr, int origin_count, MPI_Datatype origin_datatype,
            int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype,
            MPI_Win win)
{
  Nasty_mpi_op op_info;
  op_info.type = rma_get;
  _map_put_get_attrs(op_info.data.get);

  if (handle_rma_call(win, op_info) != MPI_SUCCESS) {
    return PMPI_Get(origin_addr, origin_count, origin_datatype,
                    target_rank, target_disp, target_count, target_datatype,
                    win);

  }

  return MPI_SUCCESS;
}


int MPI_Win_unlock_all(MPI_Win win)
{
  //execute all cached calls
  execute_cached_calls(win);
  //unlock nasty window
  nasty_win_unlock(win);
  //do real unock
  return PMPI_Win_unlock_all(win);
}


int MPI_Finalize(void)
{
  nasty_mpi_finalize();
  return PMPI_Finalize();
}
