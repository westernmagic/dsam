/**********************
 *
 * File:		UtDynaList.c
 * Purpose:		This module contains the general dynamic list management code.
 * Comments:	
 * Author:		L. P. O'Mard
 * Created:		01 Sep 1999
 * Updated:		
 * Copyright:	(c) 1999, University of Essex
 *
 *********************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "GeCommon.h"
#include "UtDynaList.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Subroutines and functions *********************/
/******************************************************************************/


/****************************** GetLastInst ***********************************/

/*
 * This routine returns a pointer to the last DynaList instruction in a
 * simulation instruction list.
 */

DynaListPtr
GetLastInst_Utility_DynaList(DynaListPtr head)
{
	DynaListPtr	p;

	if (head == NULL)
		return(NULL);
	for (p = head; p->next != NULL; p = p->next)
		;
	return(p);

}

/****************************** Append ****************************************/

/*
 * Add a new node to the end of the list.
 * The node must initially be set to NULL.
 * A NULL is returned if it fails in any way, otherwise it returns a pointer
 * to the new node.
 */

DynaListPtr
Append_Utility_DynaList(DynaListPtr *nodePtr, void *data)
{
	static const char *funcName = "Append_Utility_DynaList";
	DynaListPtr	newNode;

	if (*nodePtr && (*nodePtr)->next)
		return(Append_Utility_DynaList(&(*nodePtr)->next, data));
	if ((newNode = (DynaListPtr) malloc(sizeof (DynaList))) == NULL) {
		NotifyError("%s: Out of memory for DynaList.", funcName);
		return(NULL);
	}
	newNode->data = data;
	newNode->previous = *nodePtr;
	if (*nodePtr) {
		newNode->next = (*nodePtr)->next;
		(*nodePtr)->next = newNode;
	} else {
		newNode->next = NULL;
		*nodePtr = newNode;
	}
	return (newNode);

}

/****************************** Pull ******************************************/

/*
 * This routine returns the data at the top of a list, then removes the node
 * from the list.  It assumes that it is passed the top of the list.
 */

void *
Pull_Utility_DynaList(DynaListPtr *nodePtr)
{
	static const char *funcName = "Pull_Utility_DynaList";
	void *data;
	DynaListPtr	ptr;

	if (!*nodePtr) {
		NotifyError("%s: Attempt to 'pull' from empty list.", funcName);
		return (NULL);
	}
	ptr = *nodePtr;
	data = ptr->data;
	*nodePtr = ptr->next;
	if (*nodePtr)
		(*nodePtr)->previous = ptr->previous;
	free(ptr);
	return(data);

}

/****************************** Remove ****************************************/

/*
 * This routine removes a node from a list.
 */

BOOLN
Remove_Utility_DynaList(DynaListPtr *list, DynaListPtr ptr)
{
	static const char *funcName = "Remove_Utility_DynaList";
	DynaListPtr	tempPtr;

	if (!*list) {
		NotifyError("%s: List is empty.", funcName);
		return (FALSE);
	}
	if (!ptr) {
		NotifyError("%s: Node is NULL.", funcName);
		return (FALSE);
	}
	tempPtr = ptr;
	if (!ptr->previous) {
		*list = ptr->next;
		ptr->previous = NULL;
	} else {
		ptr->previous->next = ptr->next;
		ptr->next->previous = ptr->previous;
	}
	free(tempPtr);
	return(TRUE);

}

/****************************** GetMemberData *********************************/

/*
 * This routine returns the data pointer for a specific node position in the
 * list.
 * It returns FALSE if it fails in any way.
 */

void *
GetMemberData_Utility_DynaList(DynaListPtr list, int index)
{
	static const char *funcName = "GetMemberData_Utility_DynaList";
	int		count;

	count = 0;
	while (list) {
		if (count == index)
			return(list->data);
		list = list->next;
		count++;
	}
	NotifyError("%s: Could not find list member %d of %d.\n", funcName, index,
	  count);
	return(NULL);

}
