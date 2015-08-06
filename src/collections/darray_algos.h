#ifndef __DARRAY_ALGOS_H
#define __DARRAY_ALGOS_H
#include "darray.h"

typedef int (DArray_find_fn) (const void *el, void *args);
typedef int (DArray_sort_fn) (const void *a, const void *b);
typedef int (DArray_group_by_fn) (const void *el);

DArray DArray_find(DArray array, DArray_find_fn *find_fn, void *args);

void DArray_shuffle(DArray array);

void DArray_sort(DArray array, DArray_sort_fn *compar);

DArray DArray_group_by(DArray array, DArray_group_by_fn *group_by_fn);
#endif
