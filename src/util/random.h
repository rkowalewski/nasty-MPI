#ifndef __RAND_H_
#define __RAND_H_

#include <stdlib.h>

void generate_random_string(size_t len, char* str);
void arr_shuffle(size_t len, void **src, void **dst);

#endif
