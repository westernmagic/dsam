/**********************
 *
 * File:		GeNSpecLists.h
 * Purpose:		This module contains the common specifier lists used by various
 *				modules.
 * Comments:	
 * Author:		L. P. O'Mard
 * Created:		26 Nov 1997
 * Updated:
 * Copyright:	(c) 1998, University of Essex
 *
 *********************/

#ifndef _GENSPECLISTS_H
#define _GENSPECLISTS_H 1

#include "UtNameSpecs.h"

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Macro definitions *****************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef	enum {

	GENERAL_DIAGNOSTIC_OFF_MODE,
	GENERAL_DIAGNOSTIC_SCREEN_MODE,
	GENERAL_DIAGNOSTIC_ERROR_MODE,
	GENERAL_DIAGNOSTIC_FILE_MODE,
	GENERAL_DIAGNOSTIC_MODE_NULL

} GeneralDiagnosticModeSpecifier;

typedef	enum {

	GENERAL_BOOLEAN_OFF = 0,
	GENERAL_BOOLEAN_ON,
	GENERAL_BOOLEAN_NULL

} GeneralBooleanSpecifier;

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

NameSpecifier *	BooleanList_NSpecLists(int index);

void			CloseDiagnostics_NSpecLists(FILE **fp);

NameSpecifier *	DiagModeList_NSpecLists(int index);

int		GetNumListEntries_NSpecLists(NameSpecifierPtr list);

int		IdentifyDiag_NSpecLists(char *mode, NameSpecifierPtr list);

NameSpecifier *	InitNameList_NSpecLists(NameSpecifierPtr prototypeList,
				  char *textPtr);

BOOLN	OpenDiagnostics_NSpecLists(FILE **fp, NameSpecifierPtr list, int mode);

__END_DECLS

#endif
