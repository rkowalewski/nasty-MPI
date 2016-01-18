/*******************************************************************************
 *    TODO: description of class
 ******************************************************************************/

#ifndef _MPI_GET_H
#define _MPI_GET_H


/*******************************************************************************
 *    INCLUDED FILES
 ******************************************************************************/

#include "mpi_rma_op.h"


/*******************************************************************************
 *    MACROS
 ******************************************************************************/

/*******************************************************************************
 *    PUBLIC TYPES
 ******************************************************************************/

typedef struct S_MpiGet T_MpiGet;

typedef struct S_MpiGet_CtorParams
{
  T_MpiRmaOp_CtorParams super_params;
  void *origin_addr;
  int origin_count;
  MPI_Datatype origin_datatype;
  //target rank is in super class
  MPI_Aint target_disp;
  int target_count;
  MPI_Datatype target_datatype;
  MPI_Win win;
} T_MpiGet_CtorParams;



/*******************************************************************************
 *    OBJECT CONTEXT
 ******************************************************************************/

/*
 * Object context
 */
struct S_MpiGet
{
  struct
  {
    T_MpiRmaOp mpi_rma_op;
  } super;

  /*- object_context -*/
  void *p_origin_addr;
  int p_origin_count;
  MPI_Datatype p_origin_datatype;
  //target rank is in super class
  MPI_Aint p_target_disp;
  int p_target_count;
  MPI_Datatype p_target_datatype;
  MPI_Win p_win;
};



/*******************************************************************************
 *    CONSTRUCTOR, DESTRUCTOR PROTOTYPES
 ******************************************************************************/

/**
 * Constructor
 *
 * @return MPI_OS_OP_RES__OK if everything is ok, otherwise returns error code
 */
T_MpiRmaOp_Res mpi_get__ctor(T_MpiGet *me, const T_MpiGet_CtorParams *p_params);



/*******************************************************************************
 *    PUBLIC METHOD PROTOTYPES
 ******************************************************************************/

/**
 * Returns whether pointer to superclass (`T_MpiRmaOp`) points to an instance
 * of subclass (`T_MpiGet`).
 *
 * NOTE: works only for 1-level inheritance!
 */
OOC_BOOL instanceof_mpi_get(const T_MpiRmaOp *me_super);

/**
 * Returns pointer to subclass by pointer to superclass
 */
T_MpiGet *mpi_get__get_by_mpi_rma_op(const T_MpiRmaOp *me_super);

/**
 * Returns pointer to superclass (i.e. `T_MpiRmaOp`)
 */
static inline T_MpiRmaOp *mpi_get__mpi_rma_op__get(T_MpiGet *me)
{
  return &(me->super.mpi_rma_op);
}




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
T_MpiGet *new_mpi_get(const T_MpiGet_CtorParams *p_params);

#endif


#endif // _MPI_GET_H

/*******************************************************************************
 *    end of file
 ******************************************************************************/
