/*******************************************************************************
 * OOC: object-oriented C
 *
 * Configuration file
 *
 ******************************************************************************/

#ifndef _OOC_CFG_H
#define _OOC_CFG_H

/*******************************************************************************
 *    INCLUDED FILES
 ******************************************************************************/

//-- needed for malloc, free
#include <stdlib.h>


/*******************************************************************************
 *    MACROS
 ******************************************************************************/


#define     OOC_MALLOC(size)      malloc(size)
#define     OOC_FREE(pt)          free(pt)

#define     OOC_EMPTY_FUNC_ERR_ECHO()   //LOG_ERR("not implemented")



#endif /* _OOC_CFG_H */
