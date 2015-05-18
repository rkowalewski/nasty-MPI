#ifndef __NASTY_H
#define __NASTY_H

#include <stdlib.h>
#include <mpi.h>

typedef enum
{
  PUT, GET
} nasty_mpi_fn_type;

typedef struct nasty_mpi_put
{
  const void *origin_addr;
  int origin_count;
  MPI_Datatype origin_datatype;
  int target_rank;
  MPI_Aint target_disp;
  int target_count;
  MPI_Datatype target_datatype;
  //MPI_Win win;
} nasty_mpi_put;

typedef struct nasty_mpi_get
{
  void *origin_addr;
  int origin_count;
  MPI_Datatype origin_datatype;
  int target_rank;
  MPI_Aint target_disp;
  int target_count;
  MPI_Datatype target_datatype;
// MPI_Win win
} nasty_mpi_get;

typedef struct nasty_mpi_fn
{
  nasty_mpi_fn_type type;
  union
  {
    nasty_mpi_put put;
    nasty_mpi_get get;
  } data;
} nasty_mpi_fn;

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

#endif
