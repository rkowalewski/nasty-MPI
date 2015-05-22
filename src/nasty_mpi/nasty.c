#include <nasty_mpi/nasty.h>
#include <collections/kvs.h>
#include <collections/darray.h>
#include <util/random.h>
#include <assert.h>


static KVstore store = NULL;

#define _map_nasty_get(x) \
  (x) = malloc(sizeof(Nasty_mpi_get)); \
  ((Nasty_mpi_get *)(x))->origin_addr = origin_addr; \
  ((Nasty_mpi_get *)(x))->origin_count  = origin_count; \
  ((Nasty_mpi_get *)(x))->origin_datatype = origin_datatype; \
  ((Nasty_mpi_get *)(x))->target_rank = target_rank; \
  ((Nasty_mpi_get *)(x))->target_disp = target_disp; \
  ((Nasty_mpi_get *)(x))->target_count = target_count; \
  ((Nasty_mpi_get *)(x))->target_datatype = target_datatype;

#define _map_nasty_put(x) \
  (x) = malloc(sizeof(Nasty_mpi_put)); \
  ((Nasty_mpi_put *)(x))->origin_addr = origin_addr; \
  ((Nasty_mpi_put *)(x))->origin_count  = origin_count; \
  ((Nasty_mpi_put *)(x))->origin_datatype = origin_datatype; \
  ((Nasty_mpi_put *)(x))->target_rank = target_rank; \
  ((Nasty_mpi_put *)(x))->target_disp = target_disp; \
  ((Nasty_mpi_put *)(x))->target_count = target_count; \
  ((Nasty_mpi_put *)(x))->target_datatype = target_datatype;

/* Forward declarations */
static void free_nasty_mpi_op(void *data);
static void free_kv_entry(void *data);
static inline char* get_win_name(MPI_Win win);

int MPI_Init(int *argc, char ***argv)
{
  int result = PMPI_Init(argc, argv);

  if (result == MPI_SUCCESS)
  {
    store = kvs_create(5, 5, free_kv_entry);
  }

  return result;
}

int MPI_Put(const void *origin_addr, int origin_count, MPI_Datatype origin_datatype,
            int target_rank, MPI_Aint target_disp, int target_count,
            MPI_Datatype target_datatype, MPI_Win win)
{
  Nasty_mpi_put *nasty_put = NULL;
  char* win_name = get_win_name(win);
  DArray arr_ops = kvs_get(store, &win_name);
  if (arr_ops)
  {

    Nasty_mpi_op *op_info = DArray_new(arr_ops);
    op_info->type = OP_PUT;
    op_info->data = nasty_put;

    DArray_push(arr_ops, op_info);

    return MPI_SUCCESS;
  }
  else
  {
    return PMPI_Put(origin_addr, origin_count, origin_datatype,
                    target_rank, target_disp, target_count, target_datatype,
                    win);
  }
}

int MPI_Get(void *origin_addr, int origin_count, MPI_Datatype origin_datatype,
            int target_rank, MPI_Aint target_disp, int target_count,
            MPI_Datatype target_datatype, MPI_Win win)
{
  Nasty_mpi_get *nasty_get = NULL;

  char* win_name = get_win_name(win);
  DArray arr_ops = kvs_get(store, &win_name);

  if (arr_ops)
  {
    _map_nasty_get(nasty_get);

    Nasty_mpi_op *op_info = DArray_new(arr_ops);
    op_info->type = OP_GET;
    op_info->data = nasty_get;

    DArray_push(arr_ops, op_info);
    return MPI_SUCCESS;
  }
  else
  {
    return PMPI_Get(origin_addr, origin_count, origin_datatype,
                    target_rank, target_disp, target_count, target_datatype,
                    win);
  }
}

int MPI_Win_lock_all(int assert, MPI_Win win)
{
  int result = PMPI_Win_lock_all(assert, win);

  if (result == MPI_SUCCESS)
  {
    debug("locking success");

    char* win_name = get_win_name(win);
    DArray arr_ops = kvs_get(store, &win_name);

    if (!arr_ops)
    {
      arr_ops = DArray_create(sizeof(Nasty_mpi_op), 10, free_nasty_mpi_op);
      kvs_put(store, &win_name, arr_ops);
    }
  }

  return result;
}

static inline int execute_nasty_op(MPI_Win win, Nasty_mpi_op *op_info)
{
  if (!win) return -1;
  if (op_info->type == OP_PUT)
  {
    Nasty_mpi_put *put = op_info->data;
    debug_nasty_call(OP_PUT, put->origin_addr, put->origin_count, put->target_rank, (unsigned int) put->target_disp, put->target_count);
    /*
    return PMPI_Put(put->origin_addr, put->origin_count, put->origin_datatype,
                    put->target_rank, put->target_disp, put->target_count, put->target_datatype,
                    win);
                    */
  }
  else if (op_info->type == OP_GET)
  {
    Nasty_mpi_get *get = op_info->data;
    debug_nasty_call(OP_GET, get->origin_addr, get->origin_count, get->target_rank, (unsigned int) get->target_disp, get->target_count);

    /*
    return PMPI_Get(get->origin_addr, get->origin_count, get->origin_datatype,
                    get->target_rank, get->target_disp, get->target_count, get->target_datatype,
                    win);
                    */
  }

  return -1;
}


int MPI_Win_unlock_all(MPI_Win win)
{
  char* win_name = get_win_name(win);
  DArray arr_ops = kvs_get(store, &win_name);

  if (arr_ops)
  {
    DArray_shuffle(arr_ops);

    int i;
    for (i = 0; i < arr_ops->size; i++)
    {
      Nasty_mpi_op *op_info = DArray_remove(arr_ops, i);
      execute_nasty_op(win, op_info);
    }

    DArray_clear(arr_ops);
  }

  return PMPI_Win_unlock_all(win);
}

int MPI_Win_allocate(MPI_Aint size, int disp_unit, MPI_Info info,
                     MPI_Comm comm, void *baseptr, MPI_Win *win)
{

  int result = PMPI_Win_allocate(size, disp_unit, info, comm, baseptr, win);

  if (result == MPI_SUCCESS)
  {
    char *name = malloc(10 * sizeof(char));
    generate_random_string(10, name);
    result = MPI_Win_set_name(*win, name);
  }

  return result;
}

int MPI_Finalize(void)
{
  kvs_clear_destroy(store);
  return PMPI_Finalize();
}

static void free_nasty_mpi_op(void *data)
{
  Nasty_mpi_op *op = (Nasty_mpi_op *) data;
  free(op->data);
  free(op);
}

static void free_kv_entry(void *data)
{
  //Do not free the key (win)
  KVentry entry = data;
  DArray_clear_destroy(entry->value);
  free(entry);
}

static inline char* get_win_name(MPI_Win win)
{
  char* win_name = NULL;
  int len;
  MPI_Win_get_name(win, win_name, &len);
  assert(len > 0);
  return win_name;
}
