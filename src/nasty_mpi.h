#ifndef __NASTY_MPI_H_
#define __NASTY_MPI_H_

#include <stdlib.h>
#include <mpi.h>
#include <collections/darray.h>
#include <collections/kvs.h>
#include <macros/logging.h>
#include <nasty_runtime/win_storage.h>
#include <nasty_runtime/init.h>
#include <nasty_runtime/runtime.h>

int MPI_Init(int *argc, char ***argv);
int MPI_Finalize(void);

int MPI_Put(const void *origin_addr, int origin_count, MPI_Datatype origin_datatype,
            int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype,
            MPI_Win win);

int MPI_Get(void *origin_addr, int origin_count, MPI_Datatype origin_datatype,
           int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype,
           MPI_Win win);

int MPI_Win_lock_all(int assert, MPI_Win win);
int MPI_Win_unlock_all(MPI_Win win);

int MPI_Win_allocate(MPI_Aint size, int disp_unit, MPI_Info info,
    MPI_Comm comm, void *baseptr, MPI_Win *win);

int MPI_Win_create(void *base, MPI_Aint size, int disp_unit,
    MPI_Info info, MPI_Comm comm, MPI_Win *win);

int MPI_Win_flush(int rank, MPI_Win win);
int MPI_Win_flush_local(int rank, MPI_Win win);
#endif

