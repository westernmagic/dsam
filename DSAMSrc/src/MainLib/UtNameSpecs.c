/**********************
 *
 * File:		UtNameSpecs.c
 * Purpose:		This module incorporates the unified name specification code
 *				for the use of names to describe functions: name specifiers.
 * Comments:	14-12-96 LPO: The Identify_NameSpecifier routine has been
 *				improved so that it can identify abreviated names.
 * Author:		L. P. O'Mard.
 * Created:		10 Jun 1996
 * Updated:		14 Dec 1996
 * Copyright:	(c) 1998, University of Essex.
 *
 *********************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG_H */

#include "GeCommon.h"
#include "UtNameSpecs.h"
#include "UtString.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Subroutines and functions *********************/
/******************************************************************************/

/************************** GetNullSpec ***************************************/

/*
 * This routine returns the null specifier at the end of a list.
 * It assumes that the list is not NULL.
 */

int
GetNullSpec_NameSpecifier(NameSpecifierPtr list)
{
	static char *funcName = "GetNullSpec_NameSpecifier";
	for (  ; list->name[0] != '\0'; list++)
		;
	if (list->name[0] != '\0') {
		NotifyError("%s: Incorrectly set up list.", funcName);
		exit(1);
	}	
	return(list->specifier);

}

/************************** Identify ******************************************/

/*
 * This routine identifies a name from a supplied list, returning the
 * appropriate number, or zero if it does not find identify the name.
 * All lists are expected to end with the null, "", string.
 * The names in the list are all assumed to be in upper case.
 * It ends terminally (exit(1)) if the list is not initialised.
 */

int
Identify_NameSpecifier(char *name, NameSpecifierPtr list)
{
	static char *funcName = "Identify_NameSpecifier";

	if (list == NULL) {
		NotifyError("%s: List not correctly initialised.", funcName);
		exit(1);
	}
	if (name == NULL) {
		NotifyError("%s: Illegal null name.", funcName);
		return(GetNullSpec_NameSpecifier(list));
	}
	if (strlen(name) >= MAXLINE) {
		NotifyError("%s: Search name is too long (%s)", funcName, name);
		return(GetNullSpec_NameSpecifier(list));
	}
	for (  ; StrNCmpNoCase_Utility_String(list->name, name) != 0 &&
	  list->name[0] != '\0'; list++)
		;
	return(list->specifier);

}

/************************** PrintList *****************************************/

/*
 * This routine lists all of the names in a name specifier list.
 */

void
PrintList_NameSpecifier(NameSpecifierPtr list)
{
	static char *funcName = "PrintList_NameSpecifier";

	if (list == NULL) {
		NotifyError("%s: List not correctly initialised.", funcName);
		exit(1);
	}
	DPrint("Name specifier list:-\n");
	for (  ; list->name[0] != '\0'; list++)
		DPrint("\t%s\n", list->name);

}

