#include <nasty_mpi/rma_call_signature.h>


int MPI_rma_call_signature_compare(MPI_rma_call_signature_t *alpha, MPI_rma_call_signature_t *beta)
{
  return
    alpha->target_rank == beta->target_rank &&
    MPI_type_hash_equal(&alpha->origin_type_hash, &beta->origin_type_hash) &&
    MPI_type_hash_equal(&alpha->target_type_hash, &beta->target_type_hash) &&
    alpha->target_disp == beta->target_disp;
}

void MPI_rma_call_signature(Nasty_mpi_op op, MPI_rma_call_signature_t *call_signature)
{

  (void) op;
  (void) call_signature;

}
