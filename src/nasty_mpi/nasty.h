#ifndef __NASTY_H
#define __NASTY_H

#include <stdlib.h>
#include <mpi.h>

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

#define _map_nasty_put(x) \
  (x) = malloc(sizeof(Nasty_mpi_put)); \
  ((Nasty_mpi_put *)(x))->origin_addr = origin_addr; \
  ((Nasty_mpi_put *)(x))->origin_count  = origin_count; \
  ((Nasty_mpi_put *)(x))->origin_datatype = origin_datatype; \
  ((Nasty_mpi_put *)(x))->target_rank = target_rank; \
  ((Nasty_mpi_put *)(x))->target_disp = target_disp; \
  ((Nasty_mpi_put *)(x))->target_count = target_count; \
  ((Nasty_mpi_put *)(x))->target_datatype = target_datatype; \

#endif
