#include <time.h>
#include <util/random.h>
#include <stdlib.h>


static unsigned int is_initalized = 0;
static const char CHARSET[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789,.-#'?!";
static const unsigned int charset_len = (unsigned int) (sizeof(CHARSET) - 1);

static inline void initalize_random(void)
{
  if (is_initalized) return;
  srand((unsigned) time(NULL));
  is_initalized = 1;
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

void generate_random_string(size_t len, char *str)
{
  initalize_random();
  unsigned int n;
  for (n = 0; n < len; n++)
  {
    int key = rand() % charset_len;          // per-iteration instantiation
    str[n] = CHARSET[key];
  }

  str[len] = '\0';
}

void arr_shuffle(size_t len, void **src, void **dst)
{
  initalize_random();

  if (!len || !src || !dst) return;

  int i, j;

  for (j = len - 1; j > 1; j--)
  {
    i = (rand() % j) + 1;
    swap(src, dst, i - 1, j - 1);
  }
}


