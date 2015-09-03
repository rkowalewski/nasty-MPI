#ifndef __NASTY_MPI_PUT_H
#define __NASTY_MPI_PUT_H

#include <mpi.h>
#include <stdlib.h>
#include <stdbool.h>
#include "mpi_type_hash.h"

typedef struct Nasty_mpi_put
{
  const void *origin_addr;
  int origin_count;
  MPI_Datatype origin_datatype;
  MPI_Aint target_disp;
  int target_count;
  MPI_Datatype target_datatype;
} Nasty_mpi_put;

#endif

