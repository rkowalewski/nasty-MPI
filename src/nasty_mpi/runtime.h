#ifndef __NASTY_MPI_RUNTIME_H

#define __NASTY_MPI_RUNTIME_H

#include <mpi.h>
#include <nasty_mpi/win_storage.h>
#include <nasty_mpi/mpi_op.h>
#include <nasty_mpi/init.h>
#include <collections/darray_algos.h>

#define EXECUTE_OPS_OF_ANY_RANK -123

int nasty_mpi_handle_op(MPI_Win win, Nasty_mpi_op *op);
int nasty_mpi_execute_cached_calls(MPI_Win win, int rank);

#endif

