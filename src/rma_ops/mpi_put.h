/*******************************************************************************
 *    TODO: description of class
 ******************************************************************************/

#ifndef _MPI_PUT_H
#define _MPI_PUT_H


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

typedef struct S_MpiPut T_MpiPut;

typedef struct S_MpiPut_CtorParams
{
  T_MpiRmaOp_CtorParams super_params;
  const void *origin_addr;
  int origin_count;
  MPI_Datatype origin_datatype;
  //target_rank is in parent class
  int target_disp;
  int target_count;
  MPI_Datatype target_datatype;
  MPI_Win win;
} T_MpiPut_CtorParams;



/*******************************************************************************
 *    OBJECT CONTEXT
 ******************************************************************************/

/*
 * Object context
 */
struct S_MpiPut
{
  struct
  {
    T_MpiRmaOp mpi_rma_op;
  } super;

  const void *p_origin_addr;
  int p_origin_count;
  MPI_Datatype p_origin_datatype;
  //target_rank is in parent class
  int p_target_disp;
  int p_target_count;
  MPI_Datatype p_target_datatype;
  MPI_Win p_win;

  /*- object_context -*/
};



/*******************************************************************************
 *    CONSTRUCTOR, DESTRUCTOR PROTOTYPES
 ******************************************************************************/

/**
 * Constructor
 *
 * @return MPI_OS_OP_RES__OK if everything is ok, otherwise returns error code
 */
T_MpiRmaOp_Res mpi_put__ctor(T_MpiPut *me, const T_MpiPut_CtorParams *p_params);



/*******************************************************************************
 *    PUBLIC METHOD PROTOTYPES
 ******************************************************************************/

/**
 * Returns whether pointer to superclass (`T_MpiRmaOp`) points to an instance
 * of subclass (`T_MpiPut`).
 *
 * NOTE: works only for 1-level inheritance!
 */
OOC_BOOL instanceof_mpi_put(const T_MpiRmaOp *me_super);

/**
 * Returns pointer to subclass by pointer to superclass
 */
T_MpiPut *mpi_put__get_by_mpi_rma_op(const T_MpiRmaOp *me_super);

/**
 * Returns pointer to superclass (i.e. `T_MpiRmaOp`)
 */
static inline T_MpiRmaOp *mpi_put__mpi_rma_op__get(T_MpiPut *me)
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
T_MpiPut *new_mpi_put(const T_MpiPut_CtorParams *p_params);

#endif


#endif // _MPI_PUT_H

/*******************************************************************************
 *    end of file
 ******************************************************************************/
