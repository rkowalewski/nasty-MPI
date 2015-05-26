#include "../minunit.h"
#include <collections/kvs.h>
#include <util/random.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

static KVstore store = NULL;
#define KEY_LEN 10
static char *keys[5];
static char *values[5] = {"apple", "orange", "melon", "pineapple", "lemon"};

char *test_create()
{
  store = kvs_create(5, 5, NULL);
  mu_assert(store != NULL, "KVstore creation failed.");
  mu_assert(store->pairs != NULL, "contents are wrong in store.");
  mu_assert(store->capacity == 5, "Wrong capacity.");
  mu_assert(store->size == 0, "size is not at right spot.");
  mu_assert(store->expand_rate == 5, "Wrong expand_rate in store.");

  int i, j;
  for (i = 0; i < 5; i++)
  {
    keys[i] = malloc((KEY_LEN + 1) * sizeof(char));
    generate_random_string(KEY_LEN, keys[i]);
  }

  for (i = 0; i < 5; i++)
  {
    char *fst = keys[i];
    for (j = 0; j < 5; j++)
    {
      char *snd = keys[j];
      if (fst != snd)
      {
        mu_assert(strncmp(fst, snd, KEY_LEN) != 0, "strings are not random");
      }
    }
  }

  return NULL;
}

char *test_put()
{
  for (int i = 0; i < 5; i++)
  {
    mu_assert(kvs_put(store, keys[i], values[i]) == 0, "put not successful");
  }

  

  mu_assert(store->capacity == 10, "capacity is not correct after 5 insertions.");


  for (int i = 0; i < 10; i++)
  {
    if (i < 5)
    {
      mu_assert(store->pairs[i] != NULL, "entry must not be null.");
      mu_assert(strncmp(store->pairs[i]->key, keys[i], KEY_LEN) == 0, "wrong key added.");
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
    char *retrieved = kvs_get(store, keys[i]);
    mu_assert(strncmp(retrieved, values[i], KEY_LEN) == 0, "Wrong value in kvs");
  }

  return NULL;
}

char *test_update()
{
  char *hello = "hello";
  kvs_put(store, keys[2], hello);
  mu_assert(strncmp(kvs_get(store, keys[2]), hello, KEY_LEN) == 0, "wrong value for key");
  return NULL;
}

char* test_clear_destroy()
{
  kvs_clear_destroy(store);

  for (int i = 0; i < 5; i++)
  {
    free(keys[i]);
  }

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
