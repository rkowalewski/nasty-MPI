#include "darray_algos.h"
#include <util/random.h>

static inline void swap(void **src, void **dst, size_t i, size_t j)
{
  if (src == dst)
  {
    void *tmp;
    tmp = dst[i];
    dst[i] = dst[j];
    dst[j] = tmp;
  }
  else
  {
    dst[i] = src[j];
    dst[j] = src[i];
  }
}

void DArray_shuffle(DArray array)
{
  if (!array || !(array->size > 1)) return;

  size_t i, j;

  if (array->size == 2) {
    //unsigned int rand = random_seq();
    //i = (size_t) (rand % 2);
    //if (i == 0)
      swap(array->contents, array->contents, 0, 1);
  } else {
    for (j = array->size - 1; j > 1; j--)
    {
      i = (random_seq() % j) + 1;
      swap(array->contents, array->contents, i - 1, j - 1);
    }
  }
}


DArray DArray_find(DArray array, DArray_find_fn *filter_fn, void* args)
{
  if (DArray_is_empty(array)) return NULL;

  DArray filtered = DArray_create(array->element_size, array->size);

  for (int i = 0; i < DArray_count(array); i++) {
    void *item = DArray_get(array, i);

    if (filter_fn(item, args))
      DArray_push(filtered, item);
  }

  return filtered;
}


DArray DArray_group_by(DArray array, DArray_group_by_fn *group_by_fn)
{
  if (DArray_is_empty(array) || !group_by_fn) return NULL;

  DArray groups = DArray_create(sizeof(DArray), 10);
  DArray group;
  int idx;
  void *el;

  size_t count = (size_t) DArray_count(array);
  for (size_t i = 0; i < count; i++) {
    el = DArray_remove(array, i);
    if (!el) continue;
    idx = group_by_fn(el);
    if (idx == -1) continue;

    group = DArray_get(groups, idx);

    if (!group) {
      group = DArray_create(array->element_size, DArray_count(array) + 1);
      DArray_ensure_capacity(groups, idx + 1);
      DArray_set(groups, idx, group);
    }

    DArray_push(group, el);
  }

  return groups;
}
