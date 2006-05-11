/**********************
 *
 * File:		UtNameSpecs.h
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

#ifndef _UTNAMESPECS_H
#define _UTNAMESPECS_H 1

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef struct {

	WChar	*name;
	int		specifier;

} NameSpecifier, *NameSpecifierPtr;

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

int		Identify_NameSpecifier(WChar *name, NameSpecifierPtr list);

int		GetNullSpec_NameSpecifier(NameSpecifierPtr list);

void	PrintList_NameSpecifier(NameSpecifierPtr list);

__END_DECLS

#endif
