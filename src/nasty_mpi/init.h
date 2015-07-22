#ifndef __NASTY_MPI_INIT_H_
#define __NASTY_MPI_INIT_H_

#include <stdbool.h>

typedef enum
{
  maximum_delay,
  fire_immediate,
  fire_and_sync,
  choose_any,
} Submit_time;

typedef enum
{
  random_order,
  program_order,
  get_after_put,
  put_after_get,
} Submit_order;

typedef struct Nasty_mpi_config
{
  //SUBMIT_TIME
  Submit_time time;
  //SUBMIT_ORDER
  Submit_order order;
  //Split RMA Operations with basic Datatypes
  bool split_rma_ops;
} Nasty_mpi_config;

int nasty_mpi_init(int *argc, char ***argv);
void nasty_mpi_finalize(void);

Submit_order get_submit_order(void);
Submit_time get_submit_time(void);

#endif

