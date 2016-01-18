#include <mpi.h>
#include <stdbool.h>
#include <rma_ops/mpi_put.h>
#include "../minunit.h"

T_MpiPut *put = NULL;

char * test__new_mpi_put(void)
{
  T_MpiRmaOp_CtorParams super =
  {
    .target_rank = 10,
    .is_splittable = true,
    .is_atomic = false,
    .type = MPI_OS_WRITE
  };

  int origin_addr = 0x123;
  MPI_Win win = NULL;

  T_MpiPut_CtorParams params =
  {
    .super_params = super,
    .origin_addr = &origin_addr,
    .origin_count = 10,
    .origin_datatype = MPI_INT,
    .target_disp = 0,
    .target_count = 10,
    .target_datatype = MPI_INT,
    .win = win
  };

  put = new_mpi_put(&params);

  mu_assert(put != NULL, "new_mpi_put FAILED.");
  T_MpiRmaOp *inner = mpi_put__mpi_rma_op__get(put);

  mu_assert(inner->p_target_rank == 10, "super->target_rank must be 10.");
  mu_assert(inner->p_is_atomic == false, "super->is_atomic must be false.");
  mu_assert(inner->p_is_splittable == true, "super->is_splittable must be true.");
  mu_assert(inner->p_type == MPI_OS_WRITE, "super->type must be MPI_OS_WRITE.");
  mu_assert(* (int *) put->p_origin_addr == 0x123, "origin_addr must be 0x123.");
  mu_assert(put->p_origin_count == 10, "put->origin_count must be 10.");
  mu_assert(put->p_origin_datatype == MPI_INT, "put->p_origin_datatype must be MPI_INT.");
  mu_assert(put->p_target_disp == 0, "put->target_disp must be 0.");
  mu_assert(put->p_target_count == 10, "put->target_count must be 10.");
  mu_assert(put->p_target_datatype == MPI_INT, "put->target_datatype must be MPI_INT.");

  return NULL;

}

char* test__delete_mpi_rma_op(void)
{
  T_MpiRmaOp *inner = mpi_put__mpi_rma_op__get(put);
  mu_assert(inner != NULL, "inner must not be null");
  delete_mpi_rma_op(inner);

  return NULL;
}

char * all_tests()
{
  mu_suite_start();

  mu_run_test(test__new_mpi_put);
  mu_run_test(test__delete_mpi_rma_op);


  return NULL;
}

RUN_TESTS(all_tests)
