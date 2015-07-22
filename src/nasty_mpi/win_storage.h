#ifndef __NASTY_MPI_WIN_H_
#define __NASTY_MPI_WIN_H_

#include <mpi.h>
#include <collections/darray.h>

#define NASTY_ID_LEN 6
#define NASTY_ID_MAX_INT 1000000

typedef enum
{
  rma_put,
  rma_get,
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
} Nasty_mpi_get;

typedef struct Nasty_mpi_op
{
  Nasty_mpi_op_type type;
  union
  {
    Nasty_mpi_put put;
    Nasty_mpi_get get;
  } data;
} Nasty_mpi_op;

extern int KEY_NASTY_ID;
extern int KEY_ORIGIN_RANK;

int win_storage_init(void);
void win_storage_finalize(void);

int nasty_win_init(MPI_Win win, MPI_Comm win_comm);
DArray get_rma_ops(MPI_Win win);

#endif
