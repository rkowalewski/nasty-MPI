#include <nasty_mpi/win_storage.h>
#include <collections/kvs.h>
#include <collections/darray.h>
#include <util/random.h>
#include <assert.h>
#include <stdio.h>

//public
int KEY_NASTY_ID = MPI_KEYVAL_INVALID;
int KEY_ORIGIN_RANK = MPI_KEYVAL_INVALID;
//private
static KVstore win_storage = NULL;

//tear down for nasty MPI attributes - execution when window gets freed
int MPEi_nasty_id_free(MPI_Win win, int keyval, void *attr_val, void *extra_state);

int win_storage_init(void)
{
 win_storage = kvs_create(5, 5);
 return win_storage != NULL ? 0 : 1;
}

void win_storage_finalize(void)
{
  for (int i = 0; i < win_storage->size; i++)
  {
    if (win_storage->pairs[i]) {
      DArray arr_ops = win_storage->pairs[i]->value;
      DArray_clear_destroy(arr_ops);
    }
  }

  kvs_clear_destroy(win_storage);
}

static inline void nastyid_to_str(const unsigned int val, char *dst)
{
  assert(dst);
  size_t len = NASTY_ID_LEN + 1;
  int n = snprintf(dst, len, "%u", val);
  assert(((size_t) n) < len);
}

int nasty_win_init(MPI_Win win, MPI_Comm win_comm)
{
  //generate random nasty id and cache it in the window
  if (KEY_NASTY_ID == MPI_KEYVAL_INVALID) {
    MPI_Win_create_keyval(MPI_WIN_NULL_COPY_FN, MPEi_nasty_id_free, &KEY_NASTY_ID, NULL);
  }
  void *attr_val;
  int flag;
  MPI_Win_get_attr(win, KEY_NASTY_ID, &attr_val, &flag);
  if (flag) return -1;

  unsigned int nasty_id = random_seq() % NASTY_ID_MAX_INT;
  MPI_Win_set_attr(win, KEY_NASTY_ID, (void *) (MPI_Aint) nasty_id);

  //store empty array for rma opeartions in key value store
  char nasty_id_str[NASTY_ID_LEN + 1];
  nastyid_to_str(nasty_id, nasty_id_str);

  DArray array = DArray_create(sizeof(Nasty_mpi_op), 10);
  kvs_put(win_storage, nasty_id_str, array);
  //cache origin rank in window
  if (KEY_ORIGIN_RANK == MPI_KEYVAL_INVALID) {
    MPI_Win_create_keyval(MPI_WIN_NULL_COPY_FN, MPI_WIN_NULL_DELETE_FN, &KEY_ORIGIN_RANK, NULL);
  }

  int rank;
  MPI_Comm_rank(win_comm, &rank);
  MPI_Win_set_attr(win, KEY_ORIGIN_RANK, (void *) (MPI_Aint) rank);

  return 0;
}

DArray get_rma_ops(MPI_Win win)
{
  void *attr_val;
  int flag;

  MPI_Win_get_attr(win, KEY_NASTY_ID, &attr_val, &flag);
  assert(flag);

  unsigned int nasty_id = (unsigned int) (MPI_Aint) attr_val;
  assert(nasty_id);

  char nasty_id_str[NASTY_ID_LEN + 1];
  nastyid_to_str(nasty_id, nasty_id_str);

  DArray array = kvs_get(win_storage, nasty_id_str);

  if (array == NULL) {
    array = DArray_create(sizeof(Nasty_mpi_op), 10);
    kvs_put(win_storage, nasty_id_str, array);
  }

  return array;
}

int MPEi_nasty_id_free(MPI_Win win, int keyval, void *attr_val, void *extra_state)
{
  //suppress compiler warning
  (void) keyval; (void) win; (void) extra_state;

  unsigned int nasty_id = (unsigned int) (MPI_Aint) attr_val;
  char nasty_id_str[NASTY_ID_LEN + 1];
  nastyid_to_str(nasty_id, nasty_id_str);

  DArray array = kvs_remove(win_storage, nasty_id_str);
  DArray_clear_destroy(array);

  return MPI_SUCCESS;
}