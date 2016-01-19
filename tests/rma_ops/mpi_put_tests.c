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
  };

  int *origin_addr = malloc(sizeof(int));
  *origin_addr = 0x123;

  MPI_Win win = NULL;

  T_MpiPut_CtorParams params =
  {
    .super_params = super,
    .origin_addr = origin_addr,
    .origin_count = 1,
    .origin_datatype = MPI_INT,
    .target_disp = 0,
    .target_count = 1,
    .target_datatype = MPI_INT,
    .win = win
  };

  put = new_mpi_put(&params);

  mu_assert(put != NULL, "new_mpi_put FAILED.");
  T_MpiRmaOp *inner = mpi_put__mpi_rma_op__get(put);
  mu_assert(inner->p_target_rank == 10, "super->target_rank must be 10.");
  mu_assert(* (int *) *put->p_origin_addr == 0x123, "origin_addr must be 0x123.");
  mu_assert(put->p_origin_count == 1, "put->origin_count must be 10.");
  mu_assert(put->p_origin_datatype == MPI_INT, "put->p_origin_datatype must be MPI_INT.");
  mu_assert(put->p_target_disp == 0, "put->target_disp must be 0.");
  mu_assert(put->p_target_count == 1, "put->target_count must be 10.");
  mu_assert(put->p_target_datatype == MPI_INT, "put->target_datatype must be MPI_INT.");

  return NULL;
}

char* test__delete_mpi_rma_op(void)
{
  T_MpiRmaOp *inner = mpi_put__mpi_rma_op__get(put);
  mu_assert(inner != NULL, "inner must not be null");

  //free allocated origin buffer
  free(*(put->p_origin_addr));

  //call destructor
  delete_mpi_rma_op(inner);

  return NULL;
}


char * test__mpi_rma_op__clone(void)
{
  T_MpiRmaOp *put_base = mpi_put__mpi_rma_op__get(put);

  T_MpiRmaOp *clone_base = mpi_rma_op__clone(put_base);

  T_MpiPut *clone = mpi_put__get_by_mpi_rma_op(clone_base);

  mu_assert(clone != NULL, "clone must not be NULL");
  mu_assert(clone->p_origin_addr != NULL, "clone->p_origin_addr must not be NULL");
  mu_assert(*clone->p_origin_addr != NULL, "*clone->p_origin_addr must not be NULL");
  mu_assert(* (int *) *clone->p_origin_addr == * (int *) *put->p_origin_addr, "origin_addr value does not equal");

  mu_assert(clone->p_is_copy, "clone->p_is_copy must be true");
  mu_assert(clone_base->p_target_rank == put_base->p_target_rank, "target rank does not equal");
  mu_assert(clone->p_origin_count == put->p_origin_count, "origin_count does not equal");
  mu_assert(clone->p_origin_datatype== put->p_origin_datatype, "origin_datatype does not equal");
  mu_assert(clone->p_target_disp == put->p_target_disp, "target_disp does not equal");
  mu_assert(clone->p_target_count == put->p_target_count, "target_count does not equal");
  mu_assert(clone->p_target_datatype == put->p_target_datatype, "target_datatype does not equal");

  delete_mpi_rma_op(clone_base);

  return NULL;
}

char * all_tests()
{
  mu_suite_start();

  //is necessary for mpi_type_get_envelope
  PMPI_Init(NULL, NULL);

  mu_run_test(test__new_mpi_put);
  mu_run_test(test__mpi_rma_op__clone);
  mu_run_test(test__delete_mpi_rma_op);

  PMPI_Finalize();

  return NULL;
}

RUN_TESTS(all_tests)
