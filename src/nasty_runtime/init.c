#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#include <mpi.h>
#include <assert.h>
#include <string.h>
#include <macros/logging.h>
#include <util/random.h>
#include <errno.h>
#include <limits.h>
#include "init.h"
#include "win_storage.h"

static Nasty_mpi_config config = {
  .time = maximum_delay,
  .order = random_order,
  .split_rma_ops = true,
  .mpich_asynch_progress = false,
  //Default value is 31
  .sleep_interval = 31,
};

static bool _isInitialized = false;


int nasty_mpi_finalized = 0;

static inline long getenv_long(const char * env) {
  if (NULL == env) return -1;
  if (strlen(env) == 0) return -1;

  char * endptr;
  errno = 0;

  long val = strtol(env, &endptr, 10);

  if ((errno == ERANGE && (val == LONG_MAX || val == LONG_MIN))
      || (errno != 0 && val == 0)) {
    return -1;
  }

  if (endptr == env) {
    return -1;
  }

  return val;
}

static inline void load_config(void)
{
  char* val = getenv("NASTY_SUBMIT_TIME");
  char submit_time[100] = {'\0'};
  char submit_order[100] = {'\0'};

  if (val) {
    if (strcmp(val, "fire_immediate") == 0) {
      config.time = fire_immediate;
      strcpy(submit_time, "fire_immediate");
    }
    /*else if (strcmp(val, "fire_and_sync")) {
      config.time = fire_immediate;
      config.sync_all_ops = true;
    }*/
    else if (strcmp(val, "random_choice") == 0) {
      config.time = random_choice;
      strcpy(submit_time, "random");
    }
  }

  if (strlen(submit_time) == 0)
    strcpy(submit_time, "maximum_delay");

  val = getenv("NASTY_SUBMIT_ORDER");

  if (val) {
    if (strcmp(val, "program_order") == 0) {
      config.order = program_order;
      strcpy(submit_order, "program_order");
    } else if (strcmp(val, "put_after_get") == 0) {
      config.order = put_after_get;
      strcpy(submit_order, "put_after_get");
    } else if (strcmp(val, "get_after_put") == 0) {
      config.order = get_after_put;
      strcpy(submit_order, "get_after_put");
    }
  }

  if (strlen(submit_order) == 0)
    strcpy(submit_order, "random_order");


  val = getenv("MPICH_ASYNCH_PROGRESS");

  if (val) {
    int val_long = getenv_long(val);
    if (val_long == -1 || val_long == 0)
      config.mpich_asynch_progress = false;
    else
      config.mpich_asynch_progress = true;
  }

  val = getenv("NASTY_SLEEP_INTERVAL");
  if (val) {
    int val_sleep = getenv_long(val);
    if (val_sleep == -1)
      config.sleep_interval = 0;
    else
      config.sleep_interval = val_sleep;
  }

  log_info("Configuration parameters: {submit_time: %s, submit_order: %s, sleep_interval: %ld}", submit_time, submit_order, config.sleep_interval);
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

  log_info("Initialization successful");

  return 0;
}

Nasty_mpi_config get_nasty_mpi_config(void)
{
  return config;
}
void nasty_mpi_finalize(void)
{
  win_storage_finalize();
  nasty_mpi_finalized = 1;
  log_info("Cleanup successful");
}
