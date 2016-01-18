/*******************************************************************************
 *    TODO: description of class
 ******************************************************************************/

/*******************************************************************************
 *    INCLUDED FILES
 ******************************************************************************/

#include "mpi_put.h"

//-- for memset()
#include <string.h>
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

static T_MpiRmaOp * _clone(const T_MpiRmaOp *me_super)
{
  return _mpi_rma_op__vtable__get()->p_clone(me_super);
}

/**
 * TODO: here should be virtual methods implementations, like that:
static DArray _split(const T_MpiRmaOp *me_super)
{
  return _mpi_rma_op__vtable__get()->p_split(me_super);
}

*/
static int _execute(const T_MpiRmaOp *me_super)
{
  T_MpiPut *me = mpi_put__get_by_mpi_rma_op(me_super);
  return MPI_Put(
      *(me->p_origin_addr), me->p_origin_count, me->p_origin_datatype,
      me_super->p_target_rank,
      me->p_target_disp, me->p_target_count, me->p_target_datatype, me->p_win);
}

/**
 * Destructor (virtual)
 */
static void _dtor(T_MpiRmaOp *me_super)
{
  T_MpiPut *me = mpi_put__get_by_mpi_rma_op(me_super);

  OOC_FREE(me->p_origin_addr);

    //call parent constructor
  _mpi_rma_op__vtable__get()->p_dtor(me_super);
}

#if defined OOC_MALLOC && defined OOC_FREE
/**
 * Memory deallocator (virtual)
 */
static void _free(T_MpiRmaOp *me_super)
{
  T_MpiPut *me = mpi_put__get_by_mpi_rma_op(me_super);
  OOC_FREE( me);
}
#endif


//---------------- just private methods -----------------

static void _vtable_init()
{
  if (!_bool_vtable_initialized)
  {
    //-- firstly, just copy vtable of base class
    _super_vtable = *_mpi_rma_op__vtable__get();

    //-- and then, specify what we need to override.
    //   There are _dtor and _free, inevitably.
    _super_vtable.p_dtor           = _dtor;
    _super_vtable.p_free           = _free;

    //-- and then, our own virtual methods. If we don't override them here,
    //   it's ok: then, methods of base class will be called.
    _super_vtable.p_execute = _execute;
    _super_vtable.p_clone = _clone;

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
T_MpiRmaOp_Res mpi_put__ctor(T_MpiPut *me, const T_MpiPut_CtorParams *p_params)
{
  memset(me, 0x00, sizeof(T_MpiPut));
  T_MpiRmaOp_Res ret = mpi_rma_op__ctor(&me->super.mpi_rma_op, &p_params->super_params);

  if (ret == MPI_OS_OP_RES__OK)
  {

    //-- init virtual methods
    _vtable_init();
    me->super.mpi_rma_op.p_vtable = &_super_vtable;

    /*- ctor -*/
    //copy origin addr to local memory
    me->p_origin_addr = malloc(sizeof(void *));
    assert(me->p_origin_addr);
    *me->p_origin_addr = p_params->origin_addr;

    me->p_origin_datatype = p_params->origin_datatype;
    me->p_origin_count = p_params->origin_count;
    me->p_target_disp = p_params->target_disp;
    me->p_target_count = p_params->target_count;
    me->p_target_datatype = p_params->target_datatype;
    me->p_win = p_params->win;

  }
  return ret;
}


/*******************************************************************************
 *    PUBLIC METHODS
 ******************************************************************************/

OOC_BOOL instanceof_mpi_put(const T_MpiRmaOp *me_super)
{
  //-- here we just check vtable pointer.
  return (me_super->p_vtable == &_super_vtable);
}

T_MpiPut *mpi_put__get_by_mpi_rma_op(const T_MpiRmaOp *me_super)
{
  T_MpiPut *p_ret = NULL;

  if (instanceof_mpi_put(me_super))
  {

    p_ret = OOC_GET_CONTAINER_PT(
              T_MpiPut,
              super.mpi_rma_op,
              me_super
            );
  }
  else
  {
    //-- TODO: probably add some run-time error
  }

  return p_ret;
}

/*******************************************************************************
 *    ALLOCATOR, DEALLOCATOR
 ******************************************************************************/

#if defined OOC_MALLOC && defined OOC_FREE

/* allocator */
T_MpiPut *new_mpi_put(const T_MpiPut_CtorParams *p_params)
{
  T_MpiPut *me = (T_MpiPut *)OOC_MALLOC( sizeof(T_MpiPut) );
  mpi_put__ctor(me, p_params);
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
