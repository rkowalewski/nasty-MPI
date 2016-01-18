/*******************************************************************************
 *    TODO: description of class
 ******************************************************************************/

#ifndef _MPI_CMP_SWAP_H
#define _MPI_CMP_SWAP_H


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

typedef struct S_MpiCmpSwap T_MpiCmpSwap;

typedef struct S_MpiCmpSwap_CtorParams {
   T_MpiRmaOp_CtorParams super_params;

   //-- add your ctor params here
} T_MpiCmpSwap_CtorParams;



/*******************************************************************************
 *    OBJECT CONTEXT
 ******************************************************************************/

/*
 * Object context
 */
struct S_MpiCmpSwap {
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
T_MpiRmaOp_Res mpi_cmp_swap__ctor(T_MpiCmpSwap *me, const T_MpiCmpSwap_CtorParams *p_params);



/*******************************************************************************
 *    PUBLIC METHOD PROTOTYPES
 ******************************************************************************/

/**
 * Returns whether pointer to superclass (`T_MpiRmaOp`) points to an instance
 * of subclass (`T_MpiCmpSwap`).
 *
 * NOTE: works only for 1-level inheritance!
 */
OOC_BOOL instanceof_mpi_cmp_swap(const T_MpiRmaOp *me_super);

/**
 * Returns pointer to subclass by pointer to superclass
 */
T_MpiCmpSwap *mpi_cmp_swap__get_by_mpi_rma_op(const T_MpiRmaOp *me_super);

/**
 * Returns pointer to superclass (i.e. `T_MpiRmaOp`)
 */
static inline T_MpiRmaOp *mpi_cmp_swap__mpi_rma_op__get(T_MpiCmpSwap *me)
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
T_MpiCmpSwap *new_mpi_cmp_swap(const T_MpiCmpSwap_CtorParams *p_params);

#endif


#endif // _MPI_CMP_SWAP_H

/*******************************************************************************
 *    end of file
 ******************************************************************************/

