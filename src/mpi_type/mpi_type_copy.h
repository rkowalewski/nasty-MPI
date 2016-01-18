#ifndef _MPI_BUFFER_COPY_H
#define _MPI_BUFFER_COPY_H

//include MPI
#include <mpi.h>

void * mpi_buffer_copy(const void *src, MPI_Datatype type, int count);
#endif
