#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#include <mpi.h>
#include <assert.h>
#include <string.h>
#include <nasty_mpi/init.h>
#include <nasty_mpi/win_storage.h>
#include <macros/logging.h>
#include <util/random.h>

static Nasty_mpi_config config = {
  .time = maximum_delay,
  .order = random_order,
  .split_rma_ops = true
};

static bool _isInitialized = false;

static inline void load_config(void)
{
  char* val = getenv("SUBMIT_TIME");

  if (val) {
    if (strcmp(val, "fire_immediate") == 0) {
      config.time = fire_immediate;
    } else if (strcmp(val, "fire_and_sync")) {
      config.time = fire_and_sync;
    } else if (strcmp(val, "choose_any")) {
      config.time = choose_any;
    }
  }

  val = getenv("SUBMIT_ORDER");

  if (val) {
    if (strcmp(val, "program_order") == 0) {
      config.order = program_order;
    } else if (strcmp(val, "put_after_get")) {
      config.order = put_after_get;
    } else if (strcmp(val, "get_after_put")) {
      config.order = get_after_put;
    }
  }
}


int nasty_mpi_init(int *argc, char ***argv)
{
  (void) argc;
  (void) argv;
  if (_isInitialized) return -1;
  int size, rank;
  //initialize PRNG
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  unsigned int seed = (unsigned int) time(NULL) + rank * size;
  random_init(seed, seed + 1);
  win_storage_init();
  load_config();
  _isInitialized = true;
  return 0;
}

Submit_order get_submit_order(void)
{
  return config.order;
}

Submit_time get_submit_time(void)
{
  return config.time;
}

void nasty_mpi_finalize(void)
{
  win_storage_finalize();
}
