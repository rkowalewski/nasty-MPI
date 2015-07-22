#ifndef __NASTY_MPI_RUNTIME_H
#define __NASTY_MPI_RUNTIME_H

#include <stdarg.h>
#include <mpi.h>
#include <nasty_mpi/win_storage.h>
#include <nasty_mpi/init.h>

int execute_cached_calls(MPI_Win win);
int handle_rma_call(MPI_Win win, Nasty_mpi_op op);

#endif

