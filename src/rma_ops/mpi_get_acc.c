/*******************************************************************************
 *    TODO: description of class
 ******************************************************************************/

/*******************************************************************************
 *    INCLUDED FILES
 ******************************************************************************/

#include "mpi_get_acc.h"

//-- for memset()
#include <string.h>

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

static T_MpiRmaOp_VTable _super_vtable;
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

/*******************************************************************************
 *    PRIVATE METHODS
 ******************************************************************************/

//---------------- virtual methods implementations -----------------

/**
 * TODO: here should be virtual methods implementations, like that:
 */
static void _some_method(T_MpiRmaOp *me_super, int a, int b)
{
   //-- call method of superclass (if needed)
   //_mpi_rma_op__vtable__get()->p_some_method(me_super, a, b);

   T_MpiGetAcc *me = mpi_get_acc__get_by_mpi_rma_op(me_super);

   //-- ... some actions
   /*- some_method -*/
}

/**
 * Destructor (virtual)
 */
static void _dtor(T_MpiRmaOp *me_super)
{
   T_MpiGetAcc *me = mpi_get_acc__get_by_mpi_rma_op(me_super);
   // some desctruct code

   /*- dtor -*/

   // NOTE: this is a subclass, so that after performing destruction code,
   // we should call desctructor of superclass:
   _mpi_rma_op__vtable__get()->p_dtor(me_super);
}

#if defined OOC_MALLOC && defined OOC_FREE
/**
 * Memory deallocator (virtual)
 */
static void _free(T_MpiRmaOp *me_super)
{
   T_MpiGetAcc *me = mpi_get_acc__get_by_mpi_rma_op(me_super);
   OOC_FREE( me);
}
#endif


//---------------- just private methods -----------------

static void _vtable_init()
{
   if (!_bool_vtable_initialized){
      //-- firstly, just copy vtable of base class
      _super_vtable = *_mpi_rma_op__vtable__get();

      //-- and then, specify what we need to override.
      //   There are _dtor and _free, inevitably.
      _super_vtable.p_dtor           = _dtor;
      _super_vtable.p_free           = _free;

      //-- and then, our own virtual methods. If we don't override them here,
      //   it's ok: then, methods of base class will be called.
      //_super_vtable.p_some_method    = _some_method;

      //-- remember that vtable is already initialized.
      _bool_vtable_initialized = 1;
   }
}

/*******************************************************************************
 *    CONSTRUCTOR, DESTRUCTOR
 ******************************************************************************/

/**
 * Constructor
 */
T_MpiRmaOp_Res mpi_get_acc__ctor(T_MpiGetAcc *me, const T_MpiGetAcc_CtorParams *p_params)
{
   memset(me, 0x00, sizeof(T_MpiGetAcc));
   T_MpiRmaOp_Res ret = mpi_rma_op__ctor(&me->super.mpi_rma_op, &p_params->super_params);

   if (ret == MPI_OS_OP_RES__OK){

      //-- init virtual methods

      _vtable_init();
      me->super.mpi_rma_op.p_vtable = &_super_vtable;

      //-- some construct code
      /*- ctor -*/

   }
   return ret;
}


/*******************************************************************************
 *    PUBLIC METHODS
 ******************************************************************************/

OOC_BOOL instanceof_mpi_get_acc(const T_MpiRmaOp *me_super)
{
   //-- here we just check vtable pointer.
   return (me_super->p_vtable == &_super_vtable);
}

T_MpiGetAcc *mpi_get_acc__get_by_mpi_rma_op(const T_MpiRmaOp *me_super)
{
   T_MpiGetAcc *p_ret = NULL;

   if (instanceof_mpi_get_acc(me_super)){
      p_ret = OOC_GET_CONTAINER_PT(
            T_MpiGetAcc,
            super.mpi_rma_op,
            me_super
            );
   } else {
      //-- TODO: probably add some run-time error
   }

   return p_ret;
}

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
T_MpiGetAcc *new_mpi_get_acc(const T_MpiGetAcc_CtorParams *p_params)
{
   T_MpiGetAcc *me = (T_MpiGetAcc *)OOC_MALLOC( sizeof(T_MpiGetAcc) );
   mpi_get_acc__ctor(me, p_params);
   return me;
}

/*
 * NOTE: there's no deallocator in subclass; use deallocator of superclass
 * (i.e. `delete_mpi_rma_op()`)
 */

#endif



/*******************************************************************************
 *    end of file
 ******************************************************************************/


