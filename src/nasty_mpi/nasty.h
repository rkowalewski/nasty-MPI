#ifndef __NASTY_H
#define __NASTY_H

#include <stdlib.h>
#include <mpi.h>
#include <nasty_mpi/dbg.h>

typedef enum
{
  OP_PUT,
  OP_GET,
} Nasty_mpi_op_type;

typedef struct Nasty_mpi_put
{
  const void *origin_addr;
  int origin_count;
  MPI_Datatype origin_datatype;
  int target_rank;
  MPI_Aint target_disp;
  int target_count;
  MPI_Datatype target_datatype;
  //MPI_Win win;
} Nasty_mpi_put;

typedef struct Nasty_mpi_get
{
  void *origin_addr;
  int origin_count;
  MPI_Datatype origin_datatype;
  int target_rank;
  MPI_Aint target_disp;
  int target_count;
  MPI_Datatype target_datatype;
// MPI_Win win
} Nasty_mpi_get;

typedef struct Nasty_mpi_op
{
  Nasty_mpi_op_type type;
  void *data;
} Nasty_mpi_op;

int MPI_Init(int *argc, char ***argv);
int MPI_Finalize(void);

int MPI_Put(const void *origin_addr, int origin_count, MPI_Datatype origin_datatype,
            int target_rank, MPI_Aint target_disp, int target_count,
            MPI_Datatype target_datatype, MPI_Win win);

int MPI_Get(void *origin_addr, int origin_count, MPI_Datatype origin_datatype,
            int target_rank, MPI_Aint target_disp, int target_count,
            MPI_Datatype target_datatype, MPI_Win win);

int MPI_Win_lock_all(int assert, MPI_Win win);
int MPI_Win_unlock_all(MPI_Win win);

int MPI_Win_allocate(MPI_Aint size, int disp_unit, MPI_Info info,
                      MPI_Comm comm, void *baseptr, MPI_Win *win);

#define debug_nasty_call(T, ...) \
  if ((T) == OP_PUT) \
    debug("buffering put:\norigin_addr: %p\norigin_count: %d\ntarget_rank: %d\ntarget_disp: %d\ntarget_count: %d\n", __VA_ARGS__); \
  else if ((T) == OP_GET) \
    debug("buffering get:\norigin_addr: %p\norigin_count: %d\ntarget_rank: %d\ntarget_disp: %d\ntarget_count: %d\n", __VA_ARGS__);


#endif
