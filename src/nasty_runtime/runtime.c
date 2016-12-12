#include <stdbool.h>
#include <assert.h>
#include <stdlib.h>
#include <time.h>
#include <macros/logging.h>
#include <util/random.h>
#include "runtime.h"
#include "init.h"

extern int nanosleep(const struct timespec *req, struct timespec *rem);

//if the programmer fires 50 times the same mpi operation without a flush, we forward it to the mpi library...
#define MAX_SIGNATURE_LOOKUP_COUNT 50

static inline void _sleep_milliseconds(unsigned int millis)
{
  debug("sleeping for %u ms", millis);
  struct timespec ts;
  ts.tv_sec = millis / 1000;
  ts.tv_nsec = (millis % 1000) * 1000000;
  nanosleep(&ts, NULL);
}

static inline int invoke_mpi(MPI_Win win, Nasty_mpi_op *op_info, bool flush)
{

  if (op_info == NULL || op_info->is_sent) return MPI_SUCCESS;
  int rc = -1;
  if (op_info->type == rma_put)
  {
    debug("--executing actual put---");
    debug("--MPI_Put: origin { addr: %p count: %d }",
          op_info->data.put.origin_addr,
          op_info->data.put.origin_count);
    debug("--MPI_Put  target { rank: %d disp: %td count: %d }",
          op_info->target_rank,
          op_info->data.put.target_disp,
          op_info->data.put.target_count);

    rc = PMPI_Put(
           op_info->data.put.origin_addr, op_info->data.put.origin_count, op_info->data.put.origin_datatype,
           op_info->target_rank, op_info->data.put.target_disp, op_info->data.put.target_count, op_info->data.put.target_datatype,
           win);
  }
  else if (op_info->type == rma_get)
  {
    debug("--executing actual get---");
    debug("--MPI_Get: origin { addr: %p count: %d }",
          op_info->data.get.origin_addr,
          op_info->data.get.origin_count);
    debug("--MPI_Get: target { rank: %d disp: %td count: %d }",
          op_info->target_rank,
          op_info->data.get.target_disp,
          op_info->data.get.target_count);

    rc = PMPI_Get(
           op_info->data.get.origin_addr, op_info->data.get.origin_count, op_info->data.get.origin_datatype,
           op_info->target_rank, op_info->data.get.target_disp, op_info->data.get.target_count, op_info->data.get.target_datatype,
           win);
  }

  if (rc == MPI_SUCCESS && flush)
  {
    char buf[128];
    Nasty_mpi_op_type_to_str(op_info, buf, 128);
    debug("flushing %s", buf);

    bool _flush_local = (random_seq() % 2);
    if (_flush_local)
      return PMPI_Win_flush_local(op_info->target_rank, win);
    else
      return PMPI_Win_flush(op_info->target_rank, win);
  }

  return rc;
}

static inline void split_ops(DArray arr_ops, int disp_unit)
{
  size_t num = (size_t) arr_ops->size;

  for (size_t i = 0; i < num; i++)
  {
    Nasty_mpi_op *op_info = DArray_get(arr_ops, i);

    DArray divided_ops = Nasty_mpi_op_divide(op_info, disp_unit);

    if (divided_ops)
    {
      debug("dividing 1 old mpi operation in %d separate operations", DArray_count(divided_ops));
      DArray_push_all(arr_ops, divided_ops);
      //free the old operation
      DArray_remove(arr_ops, i);
      DArray_free(op_info);
      DArray_destroy(divided_ops);
    }
  }
}

static inline int sort_null_values(const void* a, const void *b)
{
  if (a == NULL && b == NULL)
    return 0;
  else if (a == NULL)
    return 1;
  else if (b == NULL)
    return -1;

  return -2;
}

int find_op_by_signature(const void* el, void* args)
{
  if (!el || !args) return 0;

  Nasty_mpi_op_signature_t *signature = (Nasty_mpi_op_signature_t *) args;

  return Nasty_mpi_op_signature_equal(&((Nasty_mpi_op *) el)->signature, signature);
}

static int cache_rma_call(MPI_Win win, Nasty_mpi_op *op)
{
  win_info_t info = nasty_win_get_info(win);
  DArray arr_ops = info.pending_operations;

  if (NULL == arr_ops) return 0;

  Nasty_mpi_op_signature_t signature;
  Nasty_mpi_op_signature(op, &signature);
  DArray found_by_signature = DArray_find(arr_ops, find_op_by_signature, &signature);

  if (!(DArray_is_empty(found_by_signature)))
  {
    Nasty_mpi_op *cached_op = DArray_get(found_by_signature, 0);
    int rc = 0;
    if (++cached_op->signature.lookup_count == MAX_SIGNATURE_LOOKUP_COUNT)
    {
      char type_str[MAX_OP_TYPE_STRLEN + 1];
      Nasty_mpi_op_type_str(cached_op, type_str);

      debug("The same %s operation has been fired already %d times without any synchronization action!\n", type_str, cached_op->signature.lookup_count);

      Nasty_mpi_config config = get_nasty_mpi_config();

      if (config.mpich_asynch_progress)
      {
        rc = invoke_mpi(win, cached_op, false);
        DArray_remove_all(arr_ops, found_by_signature);
        DArray_clear(found_by_signature);
      }
    }
    DArray_destroy(found_by_signature);
    return rc;
  }

  Nasty_mpi_op *op_info = DArray_new(arr_ops);

  if (op_info == NULL) return -1;

  op_info->type = op->type;
  op_info->data = op->data;
  op_info->target_rank = op->target_rank;
  op_info->signature = signature;
  op_info->is_sent = 0;

  int res = DArray_push(arr_ops, op_info);

  if (res)
  {
    DArray_free(op_info);
    return res;
  }

  return 0;
}

int group_ops_by_rank(const void *el)
{
  Nasty_mpi_op *op = (Nasty_mpi_op *) el;
  if (!op) return -1;
  return op->target_rank;
}

int group_ops_by_type(const void *el)
{
  Nasty_mpi_op *op = (Nasty_mpi_op *) el;
  if (!op) return -1;
  return op->type;
}

static inline DArray reorder_ops(DArray arr_ops, Submit_order order)
{
  //continue with test if there are different types
  if (order == random_order)
  {
    DArray_shuffle(arr_ops);
  }
  else if (order == put_after_get || order == get_after_put)
  {
    DArray grouped_by_type = DArray_group_by(arr_ops, group_ops_by_type);

    DArray puts = DArray_get(grouped_by_type, rma_put);
    DArray_shuffle(puts);
    DArray gets = DArray_get(grouped_by_type, rma_get);
    DArray_shuffle(gets);

    if (order == put_after_get)
    {
      DArray_push_all(arr_ops, gets);
      DArray_push_all(arr_ops, puts);
    }
    else
    {
      DArray_push_all(arr_ops, puts);
      DArray_push_all(arr_ops, gets);
    }

    DArray_destroy(puts);
    DArray_destroy(gets);
    DArray_destroy(grouped_by_type);
  }

  return arr_ops;
}

int find_ops_by_rank(const void* el, void* args)
{
  if (!el || !args) return 0;

  int rank = * (int*) args;
  Nasty_mpi_op *op = (Nasty_mpi_op *) el;
  return op->target_rank == rank;

  return 0;
}

static DArray group_ops(DArray all_ops, int rank)
{
  DArray groups;

  if (rank != EXECUTE_OPS_OF_ANY_RANK)
  {
    DArray groups = DArray_create(sizeof(DArray), 10);
    DArray ops_of_rank = DArray_find(all_ops, find_ops_by_rank, &rank);
    DArray_set(groups, 0, ops_of_rank) ;
    DArray_remove_all(all_ops, ops_of_rank);

    //filter by rank
    return groups;
  } else {
    groups = DArray_group_by(all_ops, group_ops_by_rank);
  }


  return groups;
}

int nasty_mpi_handle_op(MPI_Win win, Nasty_mpi_op *op)
{
  int res = 0;
  //int origin_rank = get_origin_rank(win);
  if (op == NULL) return res;

  Nasty_mpi_config config = get_nasty_mpi_config();

  Submit_time submit_time = config.time;

  if (submit_time == random_choice)
  {
    //make random choice between maximum_delay or fire_immediate
    submit_time = (Submit_time) (random_seq() % 2);
  }

  if (submit_time == maximum_delay)
  {
    res = cache_rma_call(win, op);
  }
  else
  {
    //bool flush = submit_time == fire_and_sync;
    //Case: fire_immediate
    Nasty_mpi_op *op_info = malloc(sizeof(Nasty_mpi_op));
    if (!op_info) return -1;
    op_info->data = op->data;
    op_info->type = op->type;
    op_info->is_sent = 0;
    op_info->target_rank = op->target_rank;
    res = invoke_mpi(win, op_info, false);
    free(op_info);
  }

  return (res == 0)  ? MPI_SUCCESS : res;
}
/*
static void _dumpArray(DArray arr)
{
  for (size_t i = 0; i < (size_t) DArray_count(arr); i++) {
    void *item = DArray_get(arr, i);
    if (item)
      debug("index: %zu, item: %p", i, item);
  }
}
*/

int nasty_mpi_execute_cached_calls(MPI_Win win, int target_rank, bool mayFlush)
{
  //int origin_rank = get_origin_rank(win);

  win_info_t win_info = nasty_win_get_info(win);

  DArray all_ops = win_info.pending_operations;

  if (DArray_is_empty(all_ops)) return MPI_SUCCESS;

  //filter all operations by target rank
  DArray grouped_by_rank = group_ops(all_ops, target_rank);

  if (DArray_is_empty(grouped_by_rank))
  {
    DArray_clear_destroy(grouped_by_rank);
    return MPI_SUCCESS;
  }

  Nasty_mpi_config config = get_nasty_mpi_config();

  int res = MPI_SUCCESS;

  log_info("executing cached operations for %d ranks", DArray_count(grouped_by_rank));

  while (!DArray_is_empty(grouped_by_rank))
  {
    DArray ops = DArray_pop(grouped_by_rank);


    if (DArray_is_empty(ops))
    {
      DArray_clear_destroy(ops);
      continue;
    }

    //split MPI_Put and MPI_Get operations
    if (config.split_rma_ops)
      split_ops(ops, win_info.disp_unit);

    if (config.order != program_order)
      reorder_ops(ops, config.order);

    //execute all operations
    size_t count = (size_t) DArray_count(ops);

    //if (count > 0)
      //log_info("number of operations for rank %d: %zu", ((Nasty_mpi_op *) DArray_get(ops, 0))->target_rank, count);

    for (size_t i = 0; i < count; i++)
    {
      Nasty_mpi_op *op_info = DArray_remove(ops, i);

      if (!op_info) continue;
      //add some latency by sleep for a random number of milliseconds (between 0 and 1500)
      if (config.sleep_interval > 0) {
        _sleep_milliseconds(random_seq() % config.sleep_interval);
      }

      bool _flush = (mayFlush && (op_info->type == rma_put)) ? random_seq() % 2 : 0;

      res = invoke_mpi(win, op_info, _flush);

      if (res != MPI_SUCCESS)
      {
        return res;
        log_err("rank %d could not execute all intercepted mpi rma calls", win_info.origin_rank);
      }

      DArray_free(op_info);
    }

    DArray_clear_destroy(ops);
  }

  DArray_clear_destroy(grouped_by_rank);

  //sort that all null values are in the end
  DArray_sort(all_ops, NULL);
  //remove null values
  DArray_contract(all_ops);

  log_info("window array (%p) --> capacity: %d, size: %d", (void *) all_ops, all_ops->capacity, all_ops->size);

  return res;
}
