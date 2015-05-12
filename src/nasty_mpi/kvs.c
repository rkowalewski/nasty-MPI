#include <nasty_mpi/kvs.h>
#include <string.h>

static inline int kvs_resize(KVstore store, size_t newsize);
static inline int kvs_expand_internal(KVstore store);
static inline int kvs_indexOf(KVstore store, const void *key);

KVstore kvs_create(size_t initial_capacity, size_t expand_rate, kvs_entry_free entry_free)
{
  KVstore kvs;

  if (!initial_capacity > 0 || !(kvs = malloc(sizeof(struct KVstore)))) return NULL;
  kvs->capacity = initial_capacity;

  if (!(kvs->pairs = calloc(initial_capacity, sizeof(KVentry)))) return NULL;

  kvs->size = 0;
  kvs->expand_rate = expand_rate ? expand_rate : DEFAULT_EXPAND_RATE;

  kvs->entry_free = entry_free ? entry_free : free;

  return kvs;
}


int kvs_put(KVstore store, const void *key, void *value)
{
  if (!store || !key) return -1;

  int idx = kvs_indexOf(store, key);

  if (idx > -1)
  {
    store->pairs[idx]->value = value;
    return 0;
  }
  else
  {
    KVentry entry = malloc(sizeof(struct KVentry));
    entry->key = key;
    entry->value = value;
    store->pairs[store->size] = entry;
    store->size++;

    return (kvs_end(store) >= kvs_capacity(store)) ? kvs_expand_internal(store) : 0;
  }
}

void* kvs_get(KVstore store, const void *key)
{
  if (!store || !key) return NULL;

  int idx = kvs_indexOf(store, key);

  return idx > -1 ? store->pairs[idx]->value : NULL;
}

void* kvs_remove(KVstore store, const void *key)
{
  if (!store || !key) return NULL;

  int idx = kvs_indexOf(store, key);
  if (idx > -1)
  {
    KVentry entry = store->pairs[idx];
    store->pairs[idx] = NULL;
    void *value = entry->value;
    free(entry);
    return value;
  }

  return NULL;
}

void kvs_clear(KVstore store)
{
  if (!store) return;
  int i = 0;
  for (i = 0; i < store->capacity; i++)
  {
    if (store->pairs[i])
    {
      store->entry_free(store->pairs[i]);
    }
  }
}

void kvs_destroy(KVstore store)
{
  if (store)
  {
    if (store->pairs) free(store->pairs);
    free(store);
  }
}


void kvs_clear_destroy(KVstore store)
{
  kvs_clear(store);
  kvs_destroy(store);
}

static inline int kvs_indexOf(KVstore store, const void *key)
{
  //Do Linear search
  for (int i = 0; i < store->size; i++)
  {
    if (store->pairs[i]->key == key)
    {
      return i;
    }
  }

  return -1;
}

static inline int kvs_resize(KVstore store, size_t newsize)
{
  if (newsize <= 0) return -1;
  store->capacity = newsize;

  KVentry *pairs;
  if (!(pairs = realloc(store->pairs, store->capacity * sizeof(KVentry)))) return -1;

  store->pairs = pairs;

  return 0;
}

static inline int kvs_expand_internal(KVstore store)
{
  if (!store) return -1;
  size_t old_capacity = store->capacity;
  if (kvs_resize(store, store->capacity + store->expand_rate)) return -1;

  memset(store->pairs + old_capacity, 0, store->expand_rate * sizeof(struct KVentry));
  return 0;
}
