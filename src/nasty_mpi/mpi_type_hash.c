/*
   (C) 2004 by Argonne National Laboratory.
       See COPYRIGHT in top-level directory.
*/
#include <nasty_mpi/mpi_type_hash.h>
#include <stdlib.h>
#include <macros/logging.h>

unsigned int MPI_type_cirleftshift( unsigned int alpha, unsigned n );
unsigned int MPI_type_cirleftshift( unsigned int alpha, unsigned n )
{
  /* Doing circular left shift of alpha by n bits */
  unsigned int t1, t2;
  t1 = alpha >> (sizeof(unsigned int)-n);
  t2 = alpha << n;
  return t1 | t2;
}

void MPI_type_hash_add(const MPI_type_hash_t *alpha,
                      const MPI_type_hash_t *beta,
                      MPI_type_hash_t *lamda);
void MPI_type_hash_add(const MPI_type_hash_t *alpha,
                      const MPI_type_hash_t *beta,
                      MPI_type_hash_t *lamda)
{
  lamda->value = (alpha->value)
                 ^ MPI_type_cirleftshift(beta->value, alpha->count);
  lamda->count = alpha->count + beta->count;
}

int MPI_type_hash_equal(const MPI_type_hash_t *alpha,
                       const MPI_type_hash_t *beta)
{
  return alpha->count == beta->count && alpha->value == beta->value;
}

unsigned int MPI_type_basic_value(MPI_Datatype type);
unsigned int MPI_type_basic_value(MPI_Datatype type)
{
  /*
     MPI_Datatype's that return 0x0 are as if they are being
     skipped/ignored in the comparison of any 2 MPI_Datatypes.
  */
  if ( type == MPI_DATATYPE_NULL || type == MPI_UB || type == MPI_LB )
    return 0x0;
  else if ( type == MPI_CHAR )
    return 0x1;
  else if ( type == MPI_SIGNED_CHAR )
    return 0x3;
  else if ( type == MPI_UNSIGNED_CHAR )
    return 0x5;
  else if ( type == MPI_BYTE )
    return 0x7;
  else if ( type == MPI_WCHAR )
    return 0x9;
  else if ( type == MPI_SHORT )
    return 0xb;
  else if ( type == MPI_UNSIGNED_SHORT )
    return 0xd;
  else if ( type == MPI_INT )
    return 0xf;
  else if ( type == MPI_UNSIGNED )
    return 0x11;
  else if ( type == MPI_LONG )
    return 0x13;
  else if ( type == MPI_UNSIGNED_LONG )
    return 0x15;
  else if ( type == MPI_FLOAT )
    return 0x17;
  else if ( type == MPI_DOUBLE )
    return 0x19;
  else if ( type == MPI_LONG_DOUBLE )
    return 0x1b;
  else if ( type == MPI_LONG_LONG_INT )
    return 0x1d;
  else if ( type == MPI_UNSIGNED_LONG_LONG )
    return 0x21;

  else if ( type == MPI_FLOAT_INT )
    return 0x8;       /* (0x17,1)@(0xf,1) */
  else if ( type == MPI_DOUBLE_INT )
    return 0x6;       /* (0x19,1)@(0xf,1) */
  else if ( type == MPI_LONG_INT )
    return 0xc;       /* (0x13,1)@(0xf,1) */
  else if ( type == MPI_SHORT_INT )
    return 0x14;      /* (0xb,1)@(0xf,1) */
  else if ( type == MPI_2INT )
    return 0x10;      /* (0xf,1)@(0xf,1) */
  else if ( type == MPI_LONG_DOUBLE_INT )
    return 0x4;       /* (0x1b,1)@(0xf,1) */

  else if ( type == MPI_PACKED )
    return 0x201;

  else {
    debug( "MPI_type_basic_value()) "
             "Unknown basic MPI datatype %x", type );
    return 0;
  }
}

unsigned int MPI_type_basic_count(MPI_Datatype type);
unsigned int MPI_type_basic_count(MPI_Datatype type)
{
  /* MPI_Datatype's that return 0 are being skipped/ignored. */
  if (    type == MPI_DATATYPE_NULL
          || type == MPI_UB
          || type == MPI_LB
     ) return 0;

  else if (    type == MPI_CHAR
               || type == MPI_SIGNED_CHAR
               || type == MPI_UNSIGNED_CHAR
               || type == MPI_BYTE
               || type == MPI_WCHAR
               || type == MPI_SHORT
               || type == MPI_UNSIGNED_SHORT
               || type == MPI_INT
               || type == MPI_UNSIGNED
               || type == MPI_LONG
               || type == MPI_UNSIGNED_LONG
               || type == MPI_FLOAT
               || type == MPI_DOUBLE
               || type == MPI_LONG_DOUBLE
               || type == MPI_LONG_LONG_INT
               || type == MPI_UNSIGNED_LONG_LONG
          ) return 1;

  else if (    type == MPI_FLOAT_INT
               || type == MPI_DOUBLE_INT
               || type == MPI_LONG_INT
               || type == MPI_SHORT_INT
               || type == MPI_2INT
               || type == MPI_LONG_DOUBLE_INT
          ) return 2;

  else if (    type == MPI_PACKED) return 1;

  else {
    debug( "MPI_type_basic_count(): "
             "Unknown basic MPI datatype %x", type );
    return 0;
  }
}


int MPI_type_derived_count(int idx, int *ints, int combiner);
int MPI_type_derived_count(int idx, int *ints, int combiner)
{
  int ii, tot_cnt, dim_A, dim_B;
  tot_cnt = 0;
  switch(combiner) {
  case MPI_COMBINER_DUP :
  case MPI_COMBINER_RESIZED :
    return 1;
  case MPI_COMBINER_CONTIGUOUS :
    return ints[0];

  case MPI_COMBINER_INDEXED_BLOCK :
  case MPI_COMBINER_VECTOR :
  case MPI_COMBINER_HVECTOR :
    return ints[0]*ints[1];

  case MPI_COMBINER_INDEXED :
  case MPI_COMBINER_HINDEXED :
    for ( ii = ints[0]; ii > 0; ii-- ) {
      tot_cnt += ints[ ii ];
    }
    return tot_cnt;

  case MPI_COMBINER_STRUCT :
    return ints[idx+1];

  case MPI_COMBINER_SUBARRAY :
    dim_A   = ints[ 0 ] + 1;
    dim_B   = 2 * ints[ 0 ];
    for ( ii=dim_A; ii<=dim_B; ii++ ) {
      tot_cnt += ints[ ii ];
    }
    return tot_cnt;
  case MPI_COMBINER_DARRAY :
    for ( ii=3; ii<=ints[2]+2; ii++ ) {
      tot_cnt += ints[ ii ];
    }
    return tot_cnt;
  }
  return tot_cnt;
}


void MPI_type_hash(MPI_Datatype type, int cnt, MPI_type_hash_t *type_hash)
{
  int             nints, naddrs, ntypes, combiner;
  int             *ints;
  MPI_Aint        *addrs;
  MPI_Datatype    *types;
  MPI_type_hash_t  curr_hash, next_hash;
  int             type_cnt;
  int             ii;

  /*  Don't know if this makes sense or not */
  if ( cnt <= 0 ) {
    /* (value,count)=(0,0) => skipping of this (type,cnt) in addition */
    type_hash->value = 0;
    type_hash->count = 0;
    return;
  }

  MPI_Type_get_envelope(type, &nints, &naddrs, &ntypes, &combiner);
  if (combiner != MPI_COMBINER_NAMED) {
    ints = NULL;
    if ( nints > 0 )
      ints = (int *) malloc(nints * sizeof(int));
    addrs = NULL;
    if ( naddrs > 0 )
      addrs = (MPI_Aint *) malloc(naddrs * sizeof(MPI_Aint));
    types = NULL;
    if ( ntypes > 0 )
      types = (MPI_Datatype *) malloc(ntypes * sizeof(MPI_Datatype));

    MPI_Type_get_contents(type, nints, naddrs, ntypes, ints, addrs, types);
    type_cnt = MPI_type_derived_count(0, ints, combiner);
    MPI_type_hash(types[0], type_cnt, &curr_hash);

    /*
        ntypes > 1 only for MPI_COMBINER_STRUCT(_INTEGER)
    */
    for( ii=1; ii < ntypes; ii++) {
      type_cnt = MPI_type_derived_count(ii, ints, combiner);
      MPI_type_hash(types[ii], type_cnt, &next_hash);
      MPI_type_hash_add(&curr_hash, &next_hash, &curr_hash);
    }

    if ( ints != NULL )
      free( ints );
    if ( addrs != NULL )
      free( addrs );
    if ( types != NULL )
      free( types );
  }
  else {
    curr_hash.value = MPI_type_basic_value(type);
    curr_hash.count = MPI_type_basic_count(type);
  }

  type_hash->value = curr_hash.value;
  type_hash->count = curr_hash.count;
  for ( ii=1; ii < cnt; ii++ ) {
    MPI_type_hash_add(type_hash, &curr_hash, type_hash);
  }
}
