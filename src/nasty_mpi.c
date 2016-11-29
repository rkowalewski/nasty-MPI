#include <nasty_mpi.h>
#include <stdbool.h>


#define _map_put_get_attrs(x) \
  (x).origin_addr = origin_addr; \
  (x).origin_count  = origin_count; \
  (x).origin_datatype = origin_datatype; \
  (x).target_disp = target_disp; \
  (x).target_count = target_count; \
  (x).target_datatype = target_datatype;

int MPI_Init(int *argc, char ***argv)
{
  int result = PMPI_Init(argc, argv);

  if (result == MPI_SUCCESS) {
    int rank = MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    debug("calling nasty_mpi_init %d", rank);
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

int MPI_Win_create(void *base, MPI_Aint size, int disp_unit,
                   MPI_Info info, MPI_Comm comm, MPI_Win *win)
{
  int result = PMPI_Win_create(base, size, disp_unit, info, comm, win);

  if (result == MPI_SUCCESS)
  {
    nasty_win_init(*win, comm);
  }

  return result;

}

int MPI_Win_create_dynamic(MPI_Info info, MPI_Comm comm, MPI_Win *win)
{
  int result = PMPI_Win_create_dynamic(info, comm, win);

  if (result == MPI_SUCCESS)
  {
    nasty_win_init(*win, comm);
  }

  return result;
}

int MPI_Win_allocate_shared(MPI_Aint size, int disp_unit, MPI_Info info, MPI_Comm comm,
                                 void *baseptr, MPI_Win *win)
{
  int result = PMPI_Win_allocate_shared(size, disp_unit, info, comm, baseptr, win);

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

int MPI_Win_lock(int lock_type, int rank, int assert, MPI_Win win)
{
  int rc = PMPI_Win_lock(lock_type, rank, assert, win);

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
        "target_rank: %d\n"
        "target_disp: %td\n"
        "target_count: %d\n",
        origin_addr, origin_count,
        target_rank, target_disp, target_count
       );
  Nasty_mpi_op op_info;
  op_info.type = rma_put;
  op_info.target_rank = target_rank;
  _map_put_get_attrs(op_info.data.put);

  if (nasty_mpi_handle_op(win, &op_info) != MPI_SUCCESS) {
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
  debug("--caching get---\n"
        "origin_addr: %p\n"
        "origin_count: %d\n"
        "target_rank: %d\n"
        "target_disp: %td\n"
        "target_count: %d\n",
        origin_addr, origin_count,
        target_rank, target_disp, target_count
       );
  Nasty_mpi_op op_info;
  op_info.type = rma_get;
  op_info.target_rank = target_rank;
  _map_put_get_attrs(op_info.data.get);

  if (nasty_mpi_handle_op(win, &op_info) != MPI_SUCCESS) {
    return PMPI_Get(origin_addr, origin_count, origin_datatype,
                    target_rank, target_disp, target_count, target_datatype,
                    win);

  }

  return MPI_SUCCESS;
}

int MPI_Win_flush(int rank, MPI_Win win)
{
  //execute all cached calls
  nasty_mpi_execute_cached_calls(win, rank, false);
  return PMPI_Win_flush(rank, win);
}

int MPI_Win_flush_all(MPI_Win win)
{
  //execute all cached calls
  nasty_mpi_execute_cached_calls(win, EXECUTE_OPS_OF_ANY_RANK, false);
  return PMPI_Win_flush_all(win);
}

int MPI_Win_flush_local(int rank, MPI_Win win)
{
  //execute all cached calls
  nasty_mpi_execute_cached_calls(win, rank, false);
  return PMPI_Win_flush_local(rank, win);
}

int MPI_Win_flush_local_all(MPI_Win win)
{
  //execute all cached calls
  nasty_mpi_execute_cached_calls(win, EXECUTE_OPS_OF_ANY_RANK, false);
  return PMPI_Win_flush_local_all(win);
}

int MPI_Win_unlock_all(MPI_Win win)
{
  //execute all cached calls
  int rc = nasty_mpi_execute_cached_calls(win, EXECUTE_OPS_OF_ANY_RANK, true);
  //unlock nasty window
  nasty_win_unlock(win);

  if (rc != MPI_SUCCESS) return rc;
  //do real unock
  return PMPI_Win_unlock_all(win);
}

int MPI_Win_unlock(int rank, MPI_Win win)
{
  //execute all cached calls
  int rc = nasty_mpi_execute_cached_calls(win, rank, true);
  //unlock nasty window
  nasty_win_unlock(win);

  if (rc != MPI_SUCCESS) return rc;
  //do real unock
  return PMPI_Win_unlock(rank, win);
}

int MPI_Finalize(void)
{
  nasty_mpi_finalize();
  return PMPI_Finalize();
}
