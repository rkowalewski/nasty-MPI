#ifndef __RAND_SEQUENCE_H_
#define __RAND_SEQUENCE_H_

#include <stdlib.h>

void random_init(unsigned int seedBase, unsigned int seedOffset);
unsigned int random_seq();
void random_string_seq(size_t len, char *dst);

#endif
