/**********************
 *
 * File:		UtDynaBList.c
 * Purpose:		This module contains the generic dynamic binary list management
 *				code.
 * Comments:	
 * Author:		L. P. O'Mard
 * Created:		23 Nov 2000
 * Updated:		
 * Copyright:	(c) 2000 CNBH, University of Essex
 *
 *********************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "GeCommon.h"
#include "UtDynaBList.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Subroutines and functions *********************/
/******************************************************************************/

/****************************** Insert ****************************************/

/*
 * Add a new node to the binary list.
 * The node must initially be set to NULL.
 * A NULL is returned if it fails in any way, otherwise it returns a pointer
 * to the new node.
 * The 'CmpFunc' is used to compare the node data.  It is expected to return
 * negative, xero and positive integer values when the test comparison is
 * less than, equal and greater than respectively.
 */

DynaBListPtr
Insert_Utility_DynaBList(DynaBListPtr *nodePtr, int (* CmpFunc)(void *, void *),
  void *data)
{
	static const char *funcName = "Insert_Utility_DynaBList";
	int		cmpResult;
	DynaBListPtr	newNode;

	if (nodePtr && *nodePtr) {
		cmpResult = CmpFunc((*nodePtr)->data, data);
		if (cmpResult == 0) {
			NotifyError("%s: Duplicated data.", funcName);
			return(NULL);
		}
		if (cmpResult > 0)
			return(Insert_Utility_DynaBList(&(*nodePtr)->right, CmpFunc, data));
		else
			return(Insert_Utility_DynaBList(&(*nodePtr)->left, CmpFunc, data));
	}
	if (*nodePtr) {
		NotifyError("%s: Duplicate label entry. Not inserted.", funcName);
		return(NULL);
	}
	if ((newNode = (DynaBListPtr) malloc(sizeof (DynaBList))) == NULL) {
		NotifyError("%s: Out of memory for DynaList.", funcName);
		return(NULL);
	}
	newNode->data = data;
	newNode->left = NULL;
	newNode->right = NULL;

	if (nodePtr)
		*nodePtr = newNode;
	return(newNode);

}

/****************************** FreeList **************************************/

/*
 * This routine frees the dynamic binary list.
 */

void
FreeList_Utility_DynaBList(DynaBListPtr *nodePtr)
{
	if (!*nodePtr)
		return;
	FreeList_Utility_DynaBList(&(*nodePtr)->left);
	FreeList_Utility_DynaBList(&(*nodePtr)->right);
	free(*nodePtr);
	*nodePtr = NULL;

}

/****************************** PrintList *************************************/

/*
 * This routine traverses the list and prints out the elements in ascending
 * order.
 */

void
PrintList_Utility_DynaBList(DynaBListPtr nodePtr, void (* PrintFunc)(void *))
{
	if (!nodePtr)
		return;
	PrintList_Utility_DynaBList(nodePtr->right, PrintFunc);
	PrintFunc(nodePtr->data);
	PrintList_Utility_DynaBList(nodePtr->left, PrintFunc);

}

/****************************** FindElement ***********************************/

/*
 * This routine returns with a specified element from the list
 */

DynaBListPtr
FindElement_Utility_DynaBList(DynaBListPtr nodePtr, int (* CmpFunc)(void *,
  void *), void *data)
{
	static const char *funcName = "FindElement_Utility_DynaBList";

	if (!nodePtr) {
		NotifyError("%s: Element not found.", funcName);
		return(NULL);
	}
	if (CmpFunc(nodePtr->data, data) > 0)
		return(FindElement_Utility_DynaBList(nodePtr->right, CmpFunc, data));
	if (CmpFunc(nodePtr->data, data) < 0)
		return(FindElement_Utility_DynaBList(nodePtr->left, CmpFunc, data));
	return(nodePtr);

}

