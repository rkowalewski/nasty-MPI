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
  .split_rma_ops = true,
};

static bool _isInitialized = false;

static inline void load_config(void)
{
  char* val = getenv("SUBMIT_TIME");

  if (val) {
    if (strcmp(val, "fire_immediate") == 0) {
      config.time = fire_immediate;
    } 
    /*else if (strcmp(val, "fire_and_sync")) {
      config.time = fire_immediate;
      config.sync_all_ops = true;
    }*/
    else if (strcmp(val, "random_choice") == 0) {
      config.time = random_choice;
    }
  }

  val = getenv("SUBMIT_ORDER");

  if (val) {
    if (strcmp(val, "program_order") == 0) {
      config.order = program_order;
    } else if (strcmp(val, "put_after_get") == 0) {
      config.order = put_after_get;
    } else if (strcmp(val, "get_after_put") == 0) {
      config.order = get_after_put;
    }
  }
}

int nasty_mpi_init(int *argc, char ***argv)
{
  (void) argc;
  (void) argv;
  if (_isInitialized) return -1;
  _isInitialized = true;
  int size, rank;
  //initialize PRNG
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  unsigned int seed = (unsigned int) time(NULL) + rank * size;
  random_init(seed, seed + 1);
  win_storage_init();
  load_config();
  return 0;
}

Nasty_mpi_config get_nasty_mpi_config(void)
{
  return config;
}
void nasty_mpi_finalize(void)
{
  win_storage_finalize();
}
