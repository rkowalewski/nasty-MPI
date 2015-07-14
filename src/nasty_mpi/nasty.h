#ifndef __NASTY_H
#define __NASTY_H

#include <stdlib.h>
#include <mpi.h>
#include <assert.h>
#include <nasty_mpi/dbg.h>
#include <collections/kvs.h>
#include <collections/darray.h>

extern KVstore store;

#define NASTY_ID_LEN 10
void fetch_nasty_win_id(MPI_Win win, char* dst);
int init_nasty_win_id(MPI_Win win);

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



#define debug_nasty_call(T, ...) \
  if ((T) == OP_PUT) \
    debug("executing buffered put\norigin_addr: %p\norigin_count: %d\ntarget_rank: %d\ntarget_disp: %d\ntarget_count: %d\n", __VA_ARGS__); \
  else if ((T) == OP_GET) \
    debug("executing buffered get\norigin_addr: %p\norigin_count: %d\ntarget_rank: %d\ntarget_disp: %d\ntarget_count: %d\n", __VA_ARGS__);


#endif
