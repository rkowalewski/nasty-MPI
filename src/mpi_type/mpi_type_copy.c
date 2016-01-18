#include "mpi_type_copy.h"
#include <string.h>
#include <stdlib.h>
#include <wchar.h>
#include <assert.h>

void *mpi_buffer_copy(const void *src, MPI_Datatype type, int count)
{
  int nints, naddrs, ntypes, combiner;

  if (count <= 0) return NULL;

  void *copy = NULL;

  MPI_Type_get_envelope(type, &nints, &naddrs, &ntypes, &combiner);
  //currently we only support predefined data types
  if (combiner != MPI_COMBINER_NAMED) return NULL;

  int size;
  MPI_Type_size(type, &size);

  copy = malloc(size * count);
  assert(copy);
  memcpy(copy, src, size * count);

  return copy;
}
