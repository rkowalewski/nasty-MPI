/*******************************************************************************
 *    TODO: description of class
 ******************************************************************************/

/*******************************************************************************
 *    INCLUDED FILES
 ******************************************************************************/

#include "mpi_rma_op.h"
#include <string.h>
#include <stdio.h>
#include <assert.h>

/*- includes -*/


/*******************************************************************************
 *    MACROS
 ******************************************************************************/

/*******************************************************************************
 *    PRIVATE METHOD PROTOTYPES
 ******************************************************************************/

static void _vtable_init();

/*******************************************************************************
 *    PRIVATE TYPES
 ******************************************************************************/

/*******************************************************************************
 *    PRIVATE DATA
 ******************************************************************************/

static T_MpiRmaOp_VTable _vtable;
static OOC_BOOL _bool_vtable_initialized = 0;



/*******************************************************************************
 *    PUBLIC DATA
 ******************************************************************************/

/*******************************************************************************
 *    EXTERNAL DATA
 ******************************************************************************/

/*******************************************************************************
 *    EXTERNAL FUNCTION PROTOTYPES
 ******************************************************************************/

/*******************************************************************************
 *    PROTECTED METHODS
 ******************************************************************************/

const T_MpiRmaOp_VTable *_mpi_rma_op__vtable__get(void)
{
   _vtable_init();
   return &_vtable;
}

/*******************************************************************************
 *    PRIVATE METHODS
 ******************************************************************************/

static void _dtor(T_MpiRmaOp *me)
{
  (void) me;
   // some desctruct code
   /*- dtor -*/
}

static void _free(T_MpiRmaOp *me)
{
   OOC_FREE(me);
}

static T_MpiRmaOp * _clone(const T_MpiRmaOp *me)
{
  (void) me;
  assert(false);
}

static DArray _split(const T_MpiRmaOp *me)
{
  (void) me;
  assert(false);
}

static int _execute(const T_MpiRmaOp *me)
{
  (void) me;
  assert(false);
}

static void _dump(const T_MpiRmaOp *me)
{
  printf("target_rank: %d\n", me->p_target_rank);
}

static const T_MpiRmaOp_Info * _info(const T_MpiRmaOp *me)
{
  (void) me;
  assert(false);
}

static void _vtable_init()
{
   if (!_bool_vtable_initialized){
      _vtable.p_dtor           = _dtor;
      _vtable.p_free           = _free;

      _vtable.p_clone = _clone;
      _vtable.p_split = _split;
      _vtable.p_execute = _execute;
      _vtable.p_dump = _dump;
      _vtable.p_info = _info;

      _bool_vtable_initialized = 1;
   }
}

/*******************************************************************************
 *    CONSTRUCTOR, DESTRUCTOR
 ******************************************************************************/

/**
 * Constructor
 *
 */
T_MpiRmaOp_Res mpi_rma_op__ctor(T_MpiRmaOp *me, const T_MpiRmaOp_CtorParams *p_params)
{
   T_MpiRmaOp_Res ret = MPI_OS_OP_RES__OK;
   memset(me, 0x00, sizeof(T_MpiRmaOp));

   //-- init virtual methods

   _vtable_init();

   me->p_vtable = &_vtable;
   me->p_target_rank= p_params->target_rank;

   return ret;
}



/*******************************************************************************
 *    PUBLIC METHODS
 ******************************************************************************/



/*******************************************************************************
 *    ALLOCATOR, DEALLOCATOR
 ******************************************************************************/

/*
 * Allocator and Deallocator
 *
 * Please NOTE: There's not necessary to use them! 
 * For instance, you can just allocate in stack or statically or manually from heap,
 * and use _ctor only.
 */
#if defined OOC_MALLOC && defined OOC_FREE

/* allocator */
T_MpiRmaOp *new_mpi_rma_op(const T_MpiRmaOp_CtorParams *p_params)
{
   T_MpiRmaOp *me = (T_MpiRmaOp *)OOC_MALLOC( sizeof(T_MpiRmaOp) );
   mpi_rma_op__ctor(me, p_params);
   return me;
}

/* deallocator */
void delete_mpi_rma_op(T_MpiRmaOp *me){
   mpi_rma_op__dtor(me);
   me->p_vtable->p_free(me);
}
#endif



/*******************************************************************************
 *    end of file
 ******************************************************************************/


