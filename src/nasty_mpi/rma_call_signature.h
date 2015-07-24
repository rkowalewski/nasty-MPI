#ifndef __RMA_SIGNATURE_H_
#define __RMA_SIGNATURE_H_

#include <mpi.h>
#include <nasty_mpi/init.h>
#include <nasty_mpi/mpi_type_hash.h>
#include <nasty_mpi/win_storage.h>

typedef struct {
  MPI_type_hash_t origin_type_hash;
  MPI_type_hash_t target_type_hash;
  int target_disp;
  int target_rank;
} MPI_rma_call_signature_t;

int MPI_rma_call_signature_compare(MPI_rma_call_signature_t *alpha, MPI_rma_call_signature_t *beta);
void MPI_rma_call_signature(Nasty_mpi_op op, MPI_rma_call_signature_t *call_signature);

#endif
