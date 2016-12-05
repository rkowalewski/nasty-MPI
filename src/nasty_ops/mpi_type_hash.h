/*
   (C) 2001 by Argonne National Laboratory.
       See COPYRIGHT in top-level directory.
*/
#include <mpi.h>

#ifndef __MPI_TYPE_HASH_H_
#define __MPI_TYPE_HASH_H_
typedef struct {
        unsigned int value;
        unsigned int count;
} MPI_type_hash_t;

int MPI_type_hash_equal(const MPI_type_hash_t *alpha,
                       const MPI_type_hash_t *beta);
void MPI_type_hash(MPI_Datatype type, int cnt, MPI_type_hash_t *dt_hash);
#endif
