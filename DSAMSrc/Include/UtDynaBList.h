/**********************
 *
 * File:		UtDynaBList.h
 * Purpose:		This module contains the generic dynamic binary list management
 *				code.
 * Comments:	
 * Author:		L. P. O'Mard
 * Created:		23 Nov 2000
 * Updated:		
 * Copyright:	(c) 2000 CNBH, University of Essex
 *
 *********************/

#ifndef _UTDYNABLIST_H
#define _UTDYNABLIST_H 1

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef struct DynaBList {

	void			*data;
	struct DynaBList	*left;
	struct DynaBList	*right;

} DynaBList, *DynaBListPtr;

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

DynaBListPtr	FindElement_Utility_DynaBList(DynaBListPtr nodePtr,
				  int (* CmpFunc)(void *, void *), void *data);

void	FreeList_Utility_DynaBList(DynaBListPtr *nodePtr);

DynaBListPtr	Insert_Utility_DynaBList(DynaBListPtr *nodePtr, int (* CmpFunc)(
				  void *, void *), void *data);

void	PrintList_Utility_DynaBList(DynaBListPtr nodePtr, void (* PrintFunc)(
		  void *));

__END_DECLS

#endif
