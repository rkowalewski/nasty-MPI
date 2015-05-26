#include <time.h>
#include <util/random.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

static const char CHARSET[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
static unsigned int m_index = 0;
static unsigned int m_intermediateOffset = 0;
static short int m_isInitialized = 0;

static unsigned int permuteQPR(unsigned int x)
{
  static const unsigned int prime = 4294967291u;
  if (x >= prime)
    return x;  // The 5 integers out of range are mapped to themselves.
  unsigned int residue = ((unsigned long long) x * x) % prime;
  return (x <= prime / 2) ? residue : prime - residue;
}



void random_init(unsigned int seedBase, unsigned int seedOffset)
{
  assert(seedBase > 0 && seedOffset > 0);
  m_index = permuteQPR(permuteQPR(seedBase) + 0x682f0161);
  m_intermediateOffset = permuteQPR(permuteQPR(seedOffset) + 0x46790905);
  m_isInitialized = 1;
}

unsigned int random_seq()
{
  if (!m_isInitialized)
  {
    unsigned int seed = (unsigned int) time(NULL);
    random_init(seed, seed + 1);
  }

  return permuteQPR((permuteQPR(m_index++) + m_intermediateOffset) ^ 0x5bf03635);
}

void random_string_seq(size_t len, char *dst)
{
  assert(dst != NULL);
  size_t charset_len = strlen(CHARSET);
  size_t n, key;
  for (n = 0; n < len; n++)
  {
    key = random_seq() % charset_len;          // per-iteration instantiation
    dst[n] = CHARSET[key];
  }

  dst[len] = '\0';
}
