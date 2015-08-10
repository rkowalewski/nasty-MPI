#include <collections/kvs.h>
#include <string.h>

static inline int kvs_resize(KVstore store, size_t newsize);
static inline int kvs_expand_internal(KVstore store);
static inline int kvs_indexOf(KVstore store, char *key);

KVstore kvs_create(size_t initial_capacity, size_t expand_rate)
{
  KVstore kvs;

  if (!(initial_capacity > 0) || !(kvs = malloc(sizeof(struct KVstore)))) return NULL;
  kvs->capacity = initial_capacity;

  if (!(kvs->pairs = calloc(initial_capacity, sizeof(KVentry)))) return NULL;

  kvs->size = 0;
  kvs->expand_rate = expand_rate ? expand_rate : DEFAULT_EXPAND_RATE;

  return kvs;
}


void* kvs_put(KVstore store, char *key, void *value)
{
  if (!store || !key) return NULL;

  int idx = kvs_indexOf(store, key);

  if (idx > -1)
  {
    void *old_value = store->pairs[idx]->value;
    store->pairs[idx]->value = value;
    return old_value;
  }
  else
  {
    KVentry entry = malloc(sizeof(struct KVentry));
    size_t key_len = strlen(key) + 1;
    char* key_val = malloc(key_len * sizeof(char));
    memcpy(key_val, key, key_len);
    entry->key = key_val;
    entry->value = value;
    store->pairs[store->size] = entry;
    store->size++;

    if (kvs_end(store) >= kvs_capacity(store)) kvs_expand_internal(store);
    return NULL;
  }
}

void* kvs_get(KVstore store, char *key)
{
  if (!store || !key) return NULL;


  int idx = kvs_indexOf(store, key);

  return idx > -1 ? store->pairs[idx]->value : NULL;
}

void* kvs_remove(KVstore store, char *key)
{
  if (!store || !key) return NULL;

  int idx = kvs_indexOf(store, key);
  if (idx > -1)
  {
    KVentry entry = store->pairs[idx];
    store->pairs[idx] = NULL;
    void *value = entry->value;
    free(entry->key);
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
      free(store->pairs[i]->key);
      free(store->pairs[i]);
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

static inline int kvs_indexOf(KVstore store, char *key)
{
  if (!key) return -1;
  //Do Linear search
  int i;
  for (i = 0; i < store->size; i++)
  {
    if (store->pairs[i] && strcmp(store->pairs[i]->key, key) == 0)
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

  memset(store->pairs + old_capacity, 0, store->expand_rate * sizeof(KVentry));

  return 0;
}
