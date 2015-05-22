#include <time.h>
#include <util/random.h>
#include <stdlib.h>


static unsigned int is_initialized = 0;
static const char CHARSET[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789#?!";
static const unsigned int charset_len = (unsigned int) (sizeof(CHARSET) - 1);

static inline void check_random_seed(void)
{
  if (is_initialized) return;
  srand((unsigned) time(NULL));
  is_initialized = 1;
}

static inline void swap(void **src, void **dst, int i, int j)
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

extern void random_set_seed_initialized(unsigned int value)
{
  is_initialized = value;
} 

extern void generate_random_string(size_t len, char *str)
{
  check_random_seed();
  unsigned int n;
  for (n = 0; n < len; n++)
  {
    int key = rand() % charset_len;          // per-iteration instantiation
    str[n] = CHARSET[key];
  }

  str[len] = '\0';
}

extern void arr_shuffle(size_t len, void **src, void **dst)
{
  check_random_seed();

  if (!len || !src || !dst) return;

  int i, j;

  for (j = len - 1; j > 1; j--)
  {
    i = (rand() % j) + 1;
    swap(src, dst, i - 1, j - 1);
  }
}


