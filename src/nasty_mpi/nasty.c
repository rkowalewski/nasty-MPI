#include <nasty_mpi/nasty.h>

#include <time.h>
#include <util/random.h>

KVstore store = NULL;

int MPI_Init(int *argc, char ***argv)
{
  int result = PMPI_Init(argc, argv);

  if (result == MPI_SUCCESS)
  {
    int size, rank;
    store = kvs_create(5, 5);

    //initialize PRNG
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    unsigned int seed = (unsigned int) time(NULL) + rank * size;
    random_init(seed, seed + 1);
  }

  return result;
}

void fetch_nasty_win_id(MPI_Win win, char *dst)
{
  int len;
  MPI_Win_get_name(win, dst, &len);
  assert(len == NASTY_ID_LEN);
}

int init_nasty_win_id(MPI_Win win)
{
  char win_name[NASTY_ID_LEN + 1];
  random_string_seq(NASTY_ID_LEN, win_name);
  return MPI_Win_set_name(win, win_name);
}


int MPI_Finalize(void)
{
  for (int i = 0; i < store->size; i++)
  {
    DArray arr_ops = store->pairs[i]->value;
    DArray_destroy(arr_ops);
  }

  kvs_clear_destroy(store);
  return PMPI_Finalize();
}

