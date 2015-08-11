#include <stdbool.h>
#include <assert.h>
#include <stdlib.h>
#include <nasty_mpi/runtime.h>
#include <nasty_mpi/init.h>
#include <nasty_mpi/mpi_op.h>
#include <macros/logging.h>
#include <util/random.h>
#include <time.h>

//if the programmer fires 50 times the same mpi operation without a flush, we forward it to the mpi library...
#define MAX_SIGNATURE_LOOKUP_COUNT 50

static inline int get_origin_rank(MPI_Win win)
{
  void *rank_attr;
  int flag, rank = -1;
  MPI_Win_get_attr(win, KEY_ORIGIN_RANK, &rank_attr, &flag);
  if (flag) rank = (int) (MPI_Aint) rank_attr;
  return rank;
}

static inline void _sleep_milliseconds(unsigned int millis)
{
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
    /*
    debug("--executing actual put---\n"
          "origin_rank: %d\n"
          "origin_addr: %p\n"
          "origin_count: %d\n"
          "origin_datatype: %d\n"
          "target_rank: %d\n"
          "target_disp: %td\n"
          "target_count: %d\n"
          "target_datatype: %d\n",
          get_origin_rank(win),
          op_info->data.put.origin_addr, op_info->data.put.origin_count, op_info->data.put.origin_datatype,
          op_info->target_rank, op_info->data.put.target_disp, op_info->data.put.target_count, op_info->data.put.target_datatype
         );
    */

    rc = PMPI_Put(
           op_info->data.put.origin_addr, op_info->data.put.origin_count, op_info->data.put.origin_datatype,
           op_info->target_rank, op_info->data.put.target_disp, op_info->data.put.target_count, op_info->data.put.target_datatype,
           win);

    if (rc == MPI_SUCCESS && flush)
    {
      return PMPI_Win_flush(op_info->target_rank, win);
    }
  }
  else if (op_info->type == rma_get)
  {
    /*
    debug("--executing actual get---\n"
          "origin_addr: %p\n"
          "origin_count: %d\n"
          "origin_datatype: %d\n"
          "target_rank: %d\n"
          "target_disp: %td\n"
          "target_count: %d\n"
          "target_datatype: %d\n",
          op_info->data.get.origin_addr, op_info->data.get.origin_count, op_info->data.get.origin_datatype,
          op_info->target_rank, op_info->data.get.target_disp, op_info->data.get.target_count, op_info->data.get.target_datatype
         );
         */
    rc = PMPI_Get(
           op_info->data.get.origin_addr, op_info->data.get.origin_count, op_info->data.get.origin_datatype,
           op_info->target_rank, op_info->data.get.target_disp, op_info->data.get.target_count, op_info->data.get.target_datatype,
           win);

    if (rc == MPI_SUCCESS && flush)
    {
      return PMPI_Win_flush(op_info->target_rank, win);
    }
  }

  return rc;
}

static inline void split_ops(DArray arr_ops)
{
  size_t num = (size_t) arr_ops->size;
  for (size_t i = 0; i < num; i++)
  {
    Nasty_mpi_op *op_info = DArray_get(arr_ops, i);

    DArray divided_ops = Nasty_mpi_op_divide(op_info);

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
  DArray arr_ops = nasty_win_get_mpi_ops(win);

  if (!arr_ops) return 0;

  Nasty_mpi_op_signature_t signature;
  Nasty_mpi_op_signature(op, &signature);
  DArray found_by_signature = DArray_find(arr_ops, find_op_by_signature, &signature);

  if (!(DArray_is_empty(found_by_signature)))
  {
    Nasty_mpi_op *cached_op = DArray_get(found_by_signature, 0);
    int rc = 0;
    if (++cached_op->signature.lookup_count == MAX_SIGNATURE_LOOKUP_COUNT)
    {
#ifndef NDEBUG
      char type_str[MAX_OP_TYPE_STRLEN + 1];
      Nasty_mpi_op_type_str(cached_op, type_str);

      debug("The same %s operation has been fired already %d times without any synchronization action!\n", type_str, cached_op->signature.lookup_count);
#endif
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
  if (rank != EXECUTE_OPS_OF_ANY_RANK)
  {
    DArray groups = DArray_create(sizeof(DArray), 1);
    DArray ops_of_rank = DArray_find(all_ops, find_ops_by_rank, &rank);
    DArray_set(groups, 0, ops_of_rank) ;
    DArray_remove_all(all_ops, ops_of_rank);

    //filter by rank
    return groups;
  }

  return DArray_group_by(all_ops, group_ops_by_rank);
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
    submit_time = random_seq() % 2;
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

int nasty_mpi_execute_cached_calls(MPI_Win win, int target_rank)
{
  //int origin_rank = get_origin_rank(win);
  DArray all_ops = nasty_win_get_mpi_ops(win);

  if (DArray_is_empty(all_ops)) return MPI_SUCCESS;

  //filter all operations by target rank
  DArray grouped_by_rank = group_ops(all_ops, target_rank);

  if (DArray_is_empty(grouped_by_rank))
  {
    DArray_destroy(grouped_by_rank);
    return MPI_SUCCESS;
  }

  Nasty_mpi_config config = get_nasty_mpi_config();

  int res = MPI_SUCCESS;

  while (!DArray_is_empty(grouped_by_rank))
  {
    DArray ops = DArray_pop(grouped_by_rank);


    if (DArray_is_empty(ops))
    {
      DArray_destroy(ops);
      continue;
    }

    //split MPI_Put and MPI_Get operations
    if (config.split_rma_ops)
      split_ops(ops);

    if (config.order != program_order)
      reorder_ops(ops, config.order);

    //execute all operations
    size_t count = (size_t) DArray_count(ops);

    for (size_t i = 0; i < count; i++)
    {
      Nasty_mpi_op *op_info = DArray_remove(ops, i);

      if (!op_info) continue;
      //add some latency by sleep for a random number of milliseconds (between 0 and 1500)
      if (count > 1)
        _sleep_milliseconds(random_seq() % 1103);


      res = invoke_mpi(win, op_info, false);

      if (res != MPI_SUCCESS)
      {
        return res;
        debug("rank %d could not execute all intercepted mpi rma calls", get_origin_rank(win));
      }

      DArray_free(op_info);
    }

    DArray_destroy(ops);
  }

  DArray_destroy(grouped_by_rank);

  return res;
}
