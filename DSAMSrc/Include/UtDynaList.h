/**********************
 *
 * File:		UtDynaList.h
 * Purpose:		This module contains the generic dynamic list management code.
 * Comments:	
 * Author:		L. P. O'Mard
 * Created:		01 Sep 1999
 * Updated:		
 * Copyright:	(c) 1999, University of Essex
 *
 *********************/

#ifndef _UTDYNALIST_H
#define _UTDYNALIST_H 1

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef struct DynaList {

	void			*data;
	struct DynaList	*next;				/* To link to next node */

} DynaList, *DynaListPtr;

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

DynaListPtr	Append_Utility_DynaList(DynaListPtr *nodePtr, void *data);

void *		Pull_Utility_DynaList(DynaListPtr *nodePtr);

__END_DECLS

#endif
