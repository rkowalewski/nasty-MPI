#ifndef __RAND_H_
#define __RAND_H_

#include <stdlib.h>

extern void generate_random_string(size_t len, char* str);
extern void arr_shuffle(size_t len, void **src, void **dst);
extern void random_set_seed_initialized(unsigned int value);

#endif
