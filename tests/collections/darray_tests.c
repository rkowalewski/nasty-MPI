#include "../minunit.h"
#include <collections/darray.h>
#include <util/random.h>
#include <assert.h>
#include <stdlib.h>

static DArray array = NULL;
static int *val1 = NULL;
static int *val2 = NULL;

char *test_create()
{
  array = DArray_create(sizeof(int), 5);
  mu_assert(array != NULL, "DArray_create failed.");
  mu_assert(array->contents != NULL, "contents are wrong in darray");
  mu_assert(array->size == 0, "size isn't at the right spot");
  mu_assert(array->element_size == sizeof(int), "element size is wrong.");
  mu_assert(array->capacity == 5, "wrong capacity length on initial size");

  return NULL;
}

char *test_destroy()
{
  DArray_destroy(array);

  return NULL;
}

char *test_new()
{
  val1 = DArray_new(array);
  mu_assert(val1 != NULL, "failed to make a new element");

  val2 = DArray_new(array);
  mu_assert(val2 != NULL, "failed to make a new element");

  return NULL;
}

char *test_set()
{
  DArray_set(array, 0, val1);
  DArray_set(array, 1, val2);

  return NULL;
}

char *test_get()
{
  mu_assert(DArray_get(array, 0) == val1, "Wrong first value.");
  mu_assert(DArray_get(array, 1) == val2, "Wrong second value.");

  return NULL;
}

char *test_remove()
{
  int *val_check = DArray_remove(array, 0);
  mu_assert(val_check != NULL, "Should not get NULL.");
  mu_assert(*val_check == *val1, "Should get the first value.");
  mu_assert(DArray_get(array, 0) == NULL, "Should be gone.");
  DArray_free(val_check);

  val_check = DArray_remove(array, 1);
  mu_assert(val_check != NULL, "Should not get NULL.");
  mu_assert(*val_check == *val2, "Should get the first value.");
  mu_assert(DArray_get(array, 1) == NULL, "Should be gone.");
  DArray_free(val_check);

  return NULL;
}

char *test_expand_contract()
{
  int old_capacity = array->capacity;

  DArray_expand(array);
  mu_assert((unsigned int)array->capacity == old_capacity + array->expand_rate, "Wrong size after expand.");

  DArray_contract(array);
  mu_assert((unsigned int)array->capacity == array->expand_rate + 1, "Should stay at the expand_rate at least.");

  DArray_contract(array);
  mu_assert((unsigned int)array->capacity == array->expand_rate + 1, "Should stay at the expand_rate at least.");

  return NULL;
}

/*
static void print_array(DArray array)
{
  int *val;
  for (int i = 0; i < array->size; i++)
  {
    val = DArray_get(array, i);
    if (val)
    {
      printf("item: %d, value :%d\n", i, *val);
    }
  }
}
*/

char *test_push_pop()
{
  mu_assert(array->size == 2, "Wrong size.");
  mu_assert(array->capacity == 5, "Wrong capacity.");
  int i = 0;
  for (i = 0; i < 100; i++)
  {
    int *val = DArray_new(array);
    *val = i * 333;
    DArray_push(array, val);
  }
  mu_assert(array->size == 102, "Wrong capacity.");
  mu_assert(array->capacity == 105, "Wrong capacity.");

  for (i = 99; i >= 0; i--)
  {
    int *val = DArray_pop(array);
    mu_assert(val != NULL, "Shouldn't get a NULL.");
    mu_assert(*val == i * 333, "Wrong value.");
    DArray_free(val);
  }

  mu_assert(array->capacity == 12, "Wrong capacity.");

  return NULL;
}

char *test_push_all()
{
  int values[2] = {10, 20};

  DArray_set(array, 0, (void*) &values[0]);
  DArray_set(array, 1, (void*) &values[1]);

  DArray src = DArray_create(array->element_size, 10);

  int *val;
  for (size_t idx = 0; idx < 5; idx++)
  {
    val = DArray_new(src);
    *val = idx;
    DArray_push(src, (void*) val);
  }

  int old_size = array->size;
  DArray_push_all(array, src);
  mu_assert(array->size == old_size + src->size, "wrong size");

  DArray_remove(array, 0);
  DArray_remove(array, 1);

  DArray_clear_destroy(src);

  return NULL;
}

char * test_natural_sort(void)
{
  int vals[3] = {1, 2, 3};
  DArray arr = DArray_create(sizeof(int), 10);
  arr->size = 8;

  DArray_set(arr, 1, &vals[0]);
  DArray_set(arr, 4, &vals[1]);
  DArray_set(arr, 7, &vals[2]);

  for (size_t i = 0; i < (size_t) DArray_count(arr); i++)
  {
  }

  DArray_sort(arr, NULL);

  for (size_t i = 0; i < (size_t) DArray_count(arr); i++)
  {
    void *item = DArray_get(arr, i);
    if (i > 2) {
      mu_assert(!item, "must be null");
    }
    else {
      mu_assert(item, "must not be null");
    }
  }

  DArray_destroy(arr);

  return NULL;
}
/*
char* test_shuffle()
{
  unsigned test;

  for (int i = 0; i < 20; i++)
  {
    test = random_seq();
    printf("random number: %u", test);
  }

  return NULL;
}



int sort_fn(const void *a, const void* b)
{
  return (int) * ((int *)a) - *((int *)b);
}

int cmp_ptr(const void *key, const void *el)
{
  const void **a = key;
  const void **b = el;
  const int *ptr1 = (int *) *a;
  const int *ptr2 = (int *) *b;

  if (*a > *b) return 1;
  if (*a < *b) return -1;

  return 0;
}


char *test_shuffle_sort()
{
  DArray array = DArray_create(sizeof(int), 10, NULL);

  int *val;
  for (int i = 0; i < 10; i++)
  {
    val = DArray_new(array);
    *val = i;
    DArray_push(array, val);
  }

  DArray_shuffle(array);
  print_array(array);
  DArray_sort(array, sort_fn);
  print_array(array);

  printf("lookup value %d, index %d\n", *(int *)array->contents[3], 3);
  int **found = DArray_bsearch(&array->contents[3], array, cmp_ptr);

  if (found)
  {
    printf("found: %d\n", *(int*)*found);
  }
  return NULL;
}

*/

char * all_tests()
{
  mu_suite_start();

  mu_run_test(test_create);
  mu_run_test(test_new);
  mu_run_test(test_set);
  mu_run_test(test_get);
  mu_run_test(test_remove);
  mu_run_test(test_push_pop);
  mu_run_test(test_push_all);
  mu_run_test(test_expand_contract);
  mu_run_test(test_destroy);
  mu_run_test(test_natural_sort);


  return NULL;
}

RUN_TESTS(all_tests)
