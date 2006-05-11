/**********************
 *
 * File:		PaComUtils.c
 * Purpose:		This module contains the set command routines for the MPI
 *				parallel modules.
 * Comments:	.
 * Author:		
 * Created:		Jan 31 1995
 * Updated:	
 * Copyright:	(c) 1998, University of Essex.
 *
 *********************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG_H */

#ifdef MPI_SUPPORT	/* To stop package dependency problems. */
#	include "mpi.h"
#endif
#include "PaComUtils.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Subroutines and functions *********************/
/******************************************************************************/

/****************************** AddToQueue ************************************/

/*
 * This routine sets and returns a pointer to a SetCom structure.
 * At the moment, the structure never "owns" the parameter, only a pointer
 * to it.
 * It returns NULL if it fails in any way.
 */

BOOLN
AddToQueue_SetComUtil(SetComPtr *start, void *parameter, int parCount,
  TypeSpecifier type, WChar *label, CommandSpecifier command,
  ScopeSpecifier scope)
{
	static const WChar *funcName = wxT("AddToQueue_SetComUtil");
	int			i;
	SetComPtr	p, pp;
	
	if ((p = (SetComPtr) malloc(sizeof(SetCom))) == NULL) {
		NotifyError(wxT("%s: Out of memory for SetComPtr (label = '%s')."),
		  funcName, label);
		return(FALSE);
	}
	if ((p->label = (WChar *) malloc(strlen(label) + 1)) == NULL) {
		NotifyError(wxT("%s: Out of memory for label."), funcName);
		free(p);
		return(FALSE);
	}
	DSAM_strcpy(p->label, label);
	p->parCount = parCount;
	p->scope = scope;
	p->command = command;
	p->type = type;
	switch (type) {
	case PA_STRING:
		if ((p->u.string = (WChar *) malloc(p->parCount)) == NULL) {
			NotifyError(wxT("%s: Out of memory for string '%s'."),
			  (WChar *) parameter);
			return(FALSE);
		}
		DSAM_strcpy(p->u.string, (WChar *) parameter);
		break;
	case PA_INT:
		if ((p->u.intVal = (int *) calloc(p->parCount, sizeof(int))) == NULL) {
			NotifyError(wxT("%s: Out of memory for integer array ([%d])."),
			  p->parCount);
			return(FALSE);
		}
		for (i = 0; i < p->parCount; i++)
			p->u.intVal[i] = ((int *) parameter)[i];
		break;
	case PA_DOUBLE:
		if ((p->u.doubleVal = (double *) calloc(p->parCount, sizeof(double))) ==
		  NULL) {
			NotifyError(wxT("%s: Out of memory for double array ([%d])."),
			  p->parCount);
			return(FALSE);
		}
		for (i = 0; i < p->parCount; i++)
			p->u.doubleVal[i] = ((double *) parameter)[i];
		break;
	case PA_LONG:
		if ((p->u.longVal = (long *) calloc(p->parCount, sizeof(long))) ==
		  NULL) {
			NotifyError(wxT("%s: Out of memory for long array ([%d])."),
			  p->parCount);
			return(FALSE);
		}
		for (i = 0; i < p->parCount; i++)
			p->u.longVal[i] = ((long *) parameter)[i];
		break;
	case PA_VOID:
		/* Nothing to be done here. */
		break;
	default:
		NotifyError(wxT("%s: Illegal parameter type (%d)."), funcName, (int)
		  type);
		free(p->label);
		free(p);
		return(FALSE);
	} /* switch */
	p->next = NULL;
	if (*start == NULL) {
		*start = p;
		return(TRUE);
	}
	for (pp = *start; pp->next != NULL; pp = pp->next)
		;
	pp->next = p;
	return(TRUE);

}

/****************************** RemoveFromQueue *******************************/

/*
 * This routine removes a SetCom from the head of a Queue and frees the
 * allocated memory.
 * I think that freeing u.string will free any memory allocated for the union,
 * otherwise I will have to use a 'switch' statement.
 */

void
RemoveFromQueue_SetComUtil(SetComPtr *head)
{
	static const WChar *funcName = wxT("RemoveFromQueue_SetComUtil");
	SetComPtr	p;

	if (*head == NULL)
		return;
	p = *head;
	*head = p->next;
	if (p->label)
		free(p->label);
	switch (p->type) {
	case PA_STRING:
		if (p->u.string)
			free (p->u.string);
		break;
	case PA_INT:
		if (p->u.intVal)
			free (p->u.intVal);
		break;
	case PA_DOUBLE:
		if (p->u.doubleVal)
			free (p->u.doubleVal);
		break;
	case PA_LONG:
		if (p->u.longVal)
			free (p->u.longVal);
		break;
	case PA_VOID:
		/* Nothing to be done here. */
		break;
	default:
		NotifyError(wxT("%s: Illegal parameter type (%d)."), funcName,
		  (int) p->type);
	} /* switch */
	free(p);

}

/****************************** RemoveQueue ***********************************/

/*
 * This routine removes all members of a queue.
 * The head of the queue is then set to NULL.
 */

void
RemoveQueue_SetComUtil(SetComPtr *head)
{
	while (*head != NULL)
		RemoveFromQueue_SetComUtil(head);

}

/****************************** PrintQueue ************************************/

/*
 * This routine prints the commands in a set-command Queue.
 * It is used for debugging purposes.
 */

#ifdef DEBUG

void
PrintQueue_SetComUtil(SetComPtr p)
{
	static const WChar *funcName = wxT("Execute_SetComUtil");
	int		i;
	
	printf(wxT("Command queue:-\n"));
	while (p != NULL) {
		printf(wxT("label = '%s', "), p->label);
		printf(wxT("value = "));
		switch (p->type) {
		case PA_STRING:
			printf(wxT("'%s'"), p->u.string);
			break;
		case PA_INT:
			for (i = 0; i < p->parCount; i++)
				printf(wxT("[%d]: %d"), i, p->u.intVal[i]);
			break;
		case PA_DOUBLE:
			for (i = 0; i < p->parCount; i++)
				printf(wxT("[%d]: %g"), i, p->u.doubleVal[i]);
			break;
		case PA_LONG:
			for (i = 0; i < p->parCount; i++)
				printf(wxT("[%d]: %lu"), i, p->u.longVal[i]);
			break;
		default:
			NotifyError(wxT("%s: Illegal parameter type (%d)."), funcName,
			  (int) p->type);
			break;
		} /* switch */
		printf(wxT("\n"));
		p = p->next;
	}

}

#endif /* DEBUG */
