#include <collections/kvs.h>
#include <collections/darray.h>
#include <macros/logging.h>
#include <util/random.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include "win_storage.h"

//public
int KEY_NASTY_ID = MPI_KEYVAL_INVALID;
//private
static KVstore win_storage = NULL;

//tear down for nasty MPI attributes - execution when window gets freed
int MPEi_nasty_id_free(MPI_Win win, int keyval, void *attr_val, void *extra_state);

static inline int uint_to_str(unsigned int val, char * dst, size_t len) {
  assert(len);
  assert(dst);
  int n = snprintf(dst, len, "%u", val);
  return ((size_t) n) >= len;
}

int win_storage_init(void)
{
  win_storage = kvs_create(5, 5);
  //in the success case we should return 0, thats why we compare with NULL
  return win_storage == NULL;
}

void win_storage_finalize(void)
{
  for (int i = 0; i < win_storage->size; i++)
  {
    if (win_storage->pairs[i]) {
      win_info_t * win = win_storage->pairs[i]->value;
      DArray_clear_destroy(win->pending_operations);
      free(win);
    }
  }

  kvs_clear_destroy(win_storage);
}

static inline int get_nasty_id(MPI_Win win, char *dst, size_t len)
{
  assert(dst);

  //fetch attribute value
  void *attr_val;
  int flag;
  //&log_info("get_nasty_id: calling win_get_attr");
  MPI_Win_get_attr(win, KEY_NASTY_ID, &attr_val, &flag);
  if (!flag || !attr_val) return 1;
  unsigned int nasty_id = (unsigned int) (MPI_Aint) attr_val;
  return uint_to_str(nasty_id, dst, len);
}

int nasty_win_init(MPI_Win win, MPI_Comm win_comm, int disp_unit)
{
  //generate random nasty id and cache it in the window
  if (KEY_NASTY_ID == MPI_KEYVAL_INVALID) {
    MPI_Win_create_keyval(MPI_WIN_NULL_COPY_FN, MPEi_nasty_id_free, &KEY_NASTY_ID, NULL);
  }
  void *attr_val;
  int flag;
  //log_info("nasty_win_init: calling win_get_attr");
  MPI_Win_get_attr(win, KEY_NASTY_ID, &attr_val, &flag);
  if (flag) return -1;

  unsigned int nasty_id = random_seq() % NASTY_ID_MAX_INT;
  MPI_Win_set_attr(win, KEY_NASTY_ID, (void *) (MPI_Aint) nasty_id);

  char nasty_id_str[NASTY_ID_LEN + 1];
  uint_to_str(nasty_id, nasty_id_str, NASTY_ID_LEN + 1);

  win_info_t *old, *new;
  new = malloc(sizeof(win_info_t));

  if (NULL == new) {
    return -1;
  }

  new->pending_operations = DArray_create(sizeof(Nasty_mpi_op), 10);
  new->disp_unit = disp_unit;
  int success = MPI_Comm_rank(win_comm, &new->origin_rank);
  assert(success == MPI_SUCCESS);

  old = kvs_put(win_storage, nasty_id_str, new);

  if (NULL != old) {
    if (NULL != old->pending_operations)
      DArray_clear_destroy(old->pending_operations);
    free(old);
  }

  return 0;
}

int nasty_win_lock(MPI_Win win)
{
  char nasty_id[NASTY_ID_LEN + 1];
  int ret = get_nasty_id(win, nasty_id, NASTY_ID_LEN + 1);

  if (ret) {
    log_err("window is not properly initialized (no nasty id!");
    return 1;
  }

  assert(kvs_get(win_storage, nasty_id));

  return 0;
}

int nasty_win_unlock(MPI_Win win)
{
  char nasty_id[NASTY_ID_LEN + 1];
  int ret = get_nasty_id(win, nasty_id, NASTY_ID_LEN + 1);

  if (ret) {
    log_err("window is not properly initialized (no nasty id!");
    return 1;
  }

  win_info_t * info = kvs_remove(win_storage, nasty_id);

  if (info) {
    DArray_clear_destroy(info->pending_operations);
    free(info);
  }

  return 0;
}

win_info_t nasty_win_get_info(MPI_Win win)
{
  char nasty_id[NASTY_ID_LEN + 1];
  int ret = get_nasty_id(win, nasty_id, NASTY_ID_LEN + 1);

  win_info_t info, *cached;
  info.pending_operations = NULL;
  if (ret) {
    log_err("window is not properly initialized (no nasty id!");
    return info;
  }
  cached = kvs_get(win_storage, nasty_id);
  info = *cached;
  assert(info.pending_operations);
  return info;
}

int MPEi_nasty_id_free(MPI_Win win, int keyval, void *attr_val, void *extra_state)
{
  //suppress compiler warning
  (void) keyval;
  (void) extra_state;
  (void) attr_val;
  (void) win;

  int flag;

  PMPI_Initialized(&flag);

  if (flag)
    nasty_win_unlock(win);

  return MPI_SUCCESS;
}
