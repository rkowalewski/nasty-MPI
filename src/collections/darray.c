#include <string.h>
#include <time.h>
#include <collections/darray.h>
#include <util/random.h>
#include <assert.h>
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>

#define MIN(a,b) (((a)<(b))?(a):(b))

DArray DArray_create(size_t element_size, size_t initial_capacity)
{
  DArray array;

  if (!(initial_capacity > 0) || !(array = malloc(sizeof(struct DArray_s)))) return NULL;
  array->capacity = initial_capacity;

  if (!(array->contents = calloc(initial_capacity, sizeof(void *)))) return NULL;

  array->size = 0;
  array->element_size = element_size;
  array->expand_rate = DEFAULT_EXPAND_RATE;

  return array;
}

void DArray_clear(DArray array)
{
  if (array && array->element_size > 0)
  {
    int i = 0;
    for (i = 0; i < array->capacity; i++)
    {
      if (array->contents[i] != NULL)
      {
        void *el = DArray_remove(array, i);
        free(el);
      }
    }
    array->size = 0;
  }
}

static inline int DArray_resize(DArray array, size_t newsize)
{
  if (newsize <= 0) return -1;
  array->capacity = newsize;

  void *contents;
  if (!(contents = realloc(array->contents, array->capacity * sizeof(void *)))) return -1;

  array->contents = contents;

  return 0;
}

int DArray_expand(DArray array)
{
  size_t old_capacity = array->capacity;
  if (DArray_resize(array, array->capacity + array->expand_rate)) return -1;

  memset(array->contents + old_capacity, 0, array->expand_rate * sizeof(void *));
  return 0;
}

int DArray_null_sort(const void *a, const void *b)
{
  //move only null values to the end and keep everything as it is
  const void *elementA = * ((void **) a);
  const void *elementB = * ((void **) b);

  if ( elementA == elementB)
    return 0;
  else if (!elementA)
    return 1;
  else if (!elementB)
    return -1;

  return 0;
}

void DArray_sort(DArray array, DArray_sort_fn *compar)
{
  if (! (array && array->size)) return;

  if (!compar) compar = DArray_null_sort;

  qsort(array->contents, array->capacity, sizeof(void*), compar);
}

int DArray_contract(DArray array)
{

  int last_valid_idx;

  for(last_valid_idx = array->capacity - 1; last_valid_idx >= (int) array->expand_rate - 1; --last_valid_idx)
  {
    if (array->contents[last_valid_idx] != NULL)
      break;
  }

  int reminder = ((size_t) last_valid_idx) % array->expand_rate;

  int new_cap = last_valid_idx;
  int res = 0;
  if (reminder) {
    new_cap = new_cap + array->expand_rate - reminder;

    int res = 0;

    if (array->capacity != new_cap) {
      if ((res = DArray_resize(array, new_cap)) == 0) {
        array->size = last_valid_idx + 1;
      }
    }
  }

  return res;
}


void DArray_destroy(DArray array)
{
  if (array)
  {
    if (array->contents) free(array->contents);
    free(array);
  }
}

void DArray_clear_destroy(DArray array)
{
  DArray_clear(array);
  DArray_destroy(array);
}

int DArray_push(DArray array, void *el)
{
  array->contents[array->size] = el;
  array->size++;

  if (DArray_end(array) >= DArray_capacity(array))
  {
    return DArray_expand(array);
  }
  else
  {
    return 0;
  }
}

void *DArray_pop(DArray array)
{
  if (array->size - 1 < 0) return NULL;

  void *el = DArray_remove(array, array->size - 1);
  array->size--;

  if (DArray_end(array) > (int)array->expand_rate && DArray_end(array) % array->expand_rate)
  {
    DArray_sort(array, DArray_null_sort);
    DArray_contract(array);
  }

  return el;
}

void DArray_remove_all(DArray array, DArray to_remove)
{
  if (!to_remove || !array || to_remove->size > array->size) return;

  assert(to_remove->size <= array->size);

  for (size_t i = 0; i < (size_t) to_remove->size; i++)
  {
    void *rem = DArray_get(to_remove, i);
    for (size_t j = 0; j < (size_t) array->size; j++) {
      void *el = DArray_get(array, j);
      if (rem == el) DArray_remove(array, j);
    }
  }
}


int DArray_push_all(DArray dst, DArray src)
{
  if (!dst || !src || DArray_count(src) == 0) return -1;

  assert(dst->element_size == src->element_size);

  for (size_t i = 0; i < (size_t) src->size; i++)
    DArray_push(dst, DArray_get(src, i));

  return 0;
}

int DArray_ensure_capacity(DArray array, int minCapacity)
{
  if (!array) return -1;

  if (array->capacity < minCapacity) {
    size_t old_capacity = array->capacity;
    if (DArray_resize(array, minCapacity) == -1) return -1;
    memset(array->contents + old_capacity, 0, (array->capacity - old_capacity) * sizeof(void *));
  }

  return 0;
}


/*
void* DArray_bsearch(const void **key, DArray arr,
                         int (*sort_fn)(const void *, const void *))
{
  if (!arr) return NULL;
  return bsearch(key, arr->contents, DArray_count(arr), sizeof(void *),
                 sort_fn);
}

*/
