#include "../minunit.h"
#include <collections/kvs.h>
#include <assert.h>
#include <stdlib.h>

static KVstore store = NULL;
static int keys[5] = {1, 2, 3, 4, 5};
static char *values[5] = {"apple", "orange", "melon", "pineapple", "lemon"};

char *test_create()
{
  store = kvs_create(5, 5, NULL);
  mu_assert(store != NULL, "KVstore creation failed.");
  mu_assert(store->pairs != NULL, "contents are wrong in store.");
  mu_assert(store->capacity == 5, "Wrong capacity.");
  mu_assert(store->size == 0, "size is not at right spot.");
  mu_assert(store->expand_rate == 5, "Wrong expand_rate in store.");
  return NULL;
}

char *test_put()
{
  for (int i = 0; i < 5; i++)
  {
    mu_assert(kvs_put(store, &keys[i], values[i]) == 0, "put not successful");
  }


  mu_assert(store->capacity == 10, "capacity is not correct after 5 insertions.");


  for (int i = 0; i < 10; i++)
  {
    if (i < 5)
    {
      mu_assert(store->pairs[i] != NULL, "entry must not be null.");
      mu_assert(store->pairs[i]->key == &keys[i], "wrong key added.");
    }
    else
    {
      mu_assert(!store->pairs[i], "spot must be empty");
    }
  }

  return NULL;
}

char *test_get()
{
  for (int i = 0; i < 5; i++)
  {
    char *retrieved = kvs_get(store, &keys[i]);
    mu_assert(retrieved == values[i], "Wrong value in kvs");
  }

  return NULL;
}

char *test_update()
{
  char *hello = "hello";
  kvs_put(store, &keys[2], hello);
  mu_assert(strcmp(kvs_get(store, &keys[2]), hello) == 0, "wrong value for key");
  return NULL;
}

char* test_clear_destroy()
{
  kvs_clear_destroy(store);
  return NULL;
}

char * all_tests()
{
  mu_suite_start();
  mu_run_test(test_create);
  mu_run_test(test_put);
  mu_run_test(test_get);
  mu_run_test(test_update);
  mu_run_test(test_clear_destroy);


  return NULL;
}

RUN_TESTS(all_tests);
