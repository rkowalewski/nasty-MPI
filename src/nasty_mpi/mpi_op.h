#ifndef __NASTY_MPI_OP_H
#define __NASTY_MPI_OP_H

#include <nasty_mpi/mpi_type_hash.h>
#include <collections/darray.h>
#include <stddef.h>

#define MAX_OP_TYPE_STRLEN 100

typedef struct {
  MPI_type_hash_t origin_type_hash;
  MPI_type_hash_t target_type_hash;
  size_t params_hash;
  //Dirty, we have to refactor this and move out to a separate data structure
  int lookup_count;
} Nasty_mpi_op_signature_t;

typedef enum
{
  rma_get,
  rma_put,
} Nasty_mpi_op_type;


typedef struct Nasty_mpi_put
{
  const void *origin_addr;
  int origin_count;
  MPI_Datatype origin_datatype;
  MPI_Aint target_disp;
  int target_count;
  MPI_Datatype target_datatype;
} Nasty_mpi_put;

typedef struct Nasty_mpi_get
{
  void *origin_addr;
  int origin_count;
  MPI_Datatype origin_datatype;
  MPI_Aint target_disp;
  int target_count;
  MPI_Datatype target_datatype;
} Nasty_mpi_get;

typedef struct Nasty_mpi_op
{
  Nasty_mpi_op_type type;
  int target_rank;
  union
  {
    Nasty_mpi_put put;
    Nasty_mpi_get get;
  } data;
  Nasty_mpi_op_signature_t signature;
  int is_sent;
} Nasty_mpi_op;

void Nasty_mpi_op_signature(Nasty_mpi_op *op, Nasty_mpi_op_signature_t *call_signature);
void Nasty_mpi_op_type_str(Nasty_mpi_op *op, char* str);
DArray Nasty_mpi_op_divide(Nasty_mpi_op *op);

int Nasty_mpi_op_signature_equal(Nasty_mpi_op_signature_t *alpha, Nasty_mpi_op_signature_t *beta);
int Nasty_mpi_op_is_divisible(Nasty_mpi_op *op);


#endif
