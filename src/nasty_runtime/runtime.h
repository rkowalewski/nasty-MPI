#ifndef __NASTY_MPI_RUNTIME_H

#define __NASTY_MPI_RUNTIME_H

#include <mpi.h>
#include <collections/darray_algos.h>
#include <nasty_ops/nasty_mpi_op.h>
#include "init.h"
#include "win_storage.h"

#define EXECUTE_OPS_OF_ANY_RANK -123

int nasty_mpi_handle_op(MPI_Win win, Nasty_mpi_op *op);
int nasty_mpi_execute_cached_calls(MPI_Win win, int rank);

#endif

