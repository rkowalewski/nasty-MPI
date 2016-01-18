/*******************************************************************************
 *    TODO: description of class
 ******************************************************************************/

#ifndef _MPI_RMA_OP_H
#define _MPI_RMA_OP_H

/*******************************************************************************
 *    INCLUDED FILES
 ******************************************************************************/

#include "ooc.h"
#include <stdbool.h>
#include <collections/darray.h>
#include <mpi.h>



/*******************************************************************************
 *    MACROS
 ******************************************************************************/

/*******************************************************************************
 *    PUBLIC TYPES
 ******************************************************************************/

typedef struct S_MpiRmaOp T_MpiRmaOp;

/*
 * Result codes
 */
typedef enum E_MpiRmaOp_Res
{
  MPI_OS_OP_RES__OK,
} T_MpiRmaOp_Res;

typedef enum S_MpiRmaOp_Type
{
  MPI_OS_WRITE, MPI_OS_READ,
} T_MpiRmaOp_Type;

typedef struct S_MpiRmaOp_CtorParams
{
  int target_rank;
  bool is_splittable;
  bool is_atomic;
  T_MpiRmaOp_Type type;
} T_MpiRmaOp_CtorParams;



/*******************************************************************************
 *    VTABLE
 ******************************************************************************/

/**
 * Destructor
 */
typedef void (T_MpiRmaOp_Dtor)                (T_MpiRmaOp *me);

/**
 * Memory deallocator. Needs to be overridden because pointer to subclass not
 * necessarily matches pointer to superclass.
 */
typedef void  (T_MpiRmaOp_Free) (T_MpiRmaOp *me);

typedef T_MpiRmaOp * (T_MpiRmaOp_Clone) (const T_MpiRmaOp *me);

typedef DArray (T_MpiRmaOp_Split) (const T_MpiRmaOp *me);

typedef int (T_MpiRmaOp_Execute) (const T_MpiRmaOp *me);

typedef void (T_MpiRmaOp_Dump) (const T_MpiRmaOp *me);


/**
 * Virtual functions table (vtable)
 */
typedef struct S_MpiRmaOp_VTable
{
  T_MpiRmaOp_Dtor             *p_dtor;
  T_MpiRmaOp_Free             *p_free;
  T_MpiRmaOp_Clone            *p_clone;
  T_MpiRmaOp_Split            *p_split;
  T_MpiRmaOp_Execute          *p_execute;
  T_MpiRmaOp_Dump             *p_dump; 
} T_MpiRmaOp_VTable;



/*******************************************************************************
 *    OBJECT CONTEXT
 ******************************************************************************/

/*
 * Object context
 */
struct S_MpiRmaOp
{

  //-- TODO: add fields to object context
  /*- object_context -*/

  struct S_MpiRmaOp_VTable *p_vtable;
  int p_target_rank;
  bool p_is_splittable;
  bool p_is_atomic;
  T_MpiRmaOp_Type p_type;
};



/*******************************************************************************
 *    CONSTRUCTOR, DESTRUCTOR PROTOTYPES
 ******************************************************************************/

/**
 * Constructor
 *
 * @return MPI_OS_OP_RES__OK if everything is ok, otherwise returns error code
 */
T_MpiRmaOp_Res mpi_rma_op__ctor(T_MpiRmaOp *me, const T_MpiRmaOp_CtorParams *p_params);

/**
 * Destructor
 */
static inline void mpi_rma_op__dtor(T_MpiRmaOp *me)
{
  me->p_vtable->p_dtor(me);
}



/*******************************************************************************
 *    PUBLIC METHOD PROTOTYPES
 ******************************************************************************/

static inline T_MpiRmaOp * mpi_rma_op__clone(const T_MpiRmaOp *me)
{
  return me->p_vtable->p_clone(me);
}

static inline DArray mpi_rma_op__split(const T_MpiRmaOp *me)
{
  return me->p_vtable->p_split(me);
}

static inline int mpi_rma_op__execute(const T_MpiRmaOp *me)
{
  return me->p_vtable->p_execute(me);
}

static inline void mpi_rma_op__dump(const T_MpiRmaOp *me)
{
  me->p_vtable->p_dump(me);
}
/*******************************************************************************
 *    PROTECTED METHOD PROTOTYPES
 ******************************************************************************/

/**
 * Returns pointer to vtable for class MpiOsOp.
 */
const T_MpiRmaOp_VTable *_mpi_rma_op__vtable__get(void);


/*******************************************************************************
 *    ALLOCATOR, DEALLOCATOR PROTOTYPES
 ******************************************************************************/

/*
 * Allocator and Deallocator
 *
 * Please NOTE: There's not necessary to use them!
 * For instance, you can just allocate in stack or statically or manually from
 * heap, and use _ctor only.
 */
#if defined OOC_MALLOC && defined OOC_FREE

/* allocator */
T_MpiRmaOp *new_mpi_rma_op(const T_MpiRmaOp_CtorParams *p_params);

/* deallocator */
void delete_mpi_rma_op(T_MpiRmaOp *me);

#endif


#endif // _MPI_RMA_OP_H

/*******************************************************************************
 *    end of file
 ******************************************************************************/
