/*******************************************************************************
 *    TODO: description of class
 ******************************************************************************/

#ifndef _MPI_FETCH_AND_OP_H
#define _MPI_FETCH_AND_OP_H


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

typedef struct S_MpiFetchAndOp T_MpiFetchAndOp;

typedef struct S_MpiFetchAndOp_CtorParams {
   T_MpiRmaOp_CtorParams super_params;

   //-- add your ctor params here
} T_MpiFetchAndOp_CtorParams;



/*******************************************************************************
 *    OBJECT CONTEXT
 ******************************************************************************/

/*
 * Object context
 */
struct S_MpiFetchAndOp {
   struct {
      T_MpiRmaOp mpi_rma_op;
   } super;

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
T_MpiRmaOp_Res mpi_fetch_and_op__ctor(T_MpiFetchAndOp *me, const T_MpiFetchAndOp_CtorParams *p_params);



/*******************************************************************************
 *    PUBLIC METHOD PROTOTYPES
 ******************************************************************************/

/**
 * Returns whether pointer to superclass (`T_MpiRmaOp`) points to an instance
 * of subclass (`T_MpiFetchAndOp`).
 *
 * NOTE: works only for 1-level inheritance!
 */
OOC_BOOL instanceof_mpi_fetch_and_op(const T_MpiRmaOp *me_super);

/**
 * Returns pointer to subclass by pointer to superclass
 */
T_MpiFetchAndOp *mpi_fetch_and_op__get_by_mpi_rma_op(const T_MpiRmaOp *me_super);

/**
 * Returns pointer to superclass (i.e. `T_MpiRmaOp`)
 */
static inline T_MpiRmaOp *mpi_fetch_and_op__mpi_rma_op__get(T_MpiFetchAndOp *me)
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
T_MpiFetchAndOp *new_mpi_fetch_and_op(const T_MpiFetchAndOp_CtorParams *p_params);

#endif


#endif // _MPI_FETCH_AND_OP_H

/*******************************************************************************
 *    end of file
 ******************************************************************************/

