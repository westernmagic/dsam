/**********************
 *
 * File:		GeNSpecLists.c
 * Purpose:		This module contains the common specifier lists used by various
 *				modules, and also the common associated actions.
 * Comments:	
 * Author:		L. P. O'Mard
 * Created:		26 Nov 1997
 * Updated:
 * Copyright:	(c) 1998, University of Essex
 *
 *********************/
 
#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG_H */

#include <stdio.h>
#include <stdlib.h>

#include "GeCommon.h"
#include "GeNSpecLists.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Subroutines and functions *********************/
/******************************************************************************/

/****************************** BooleanList ***********************************/

/*
 * This routine returns a name specifier for the boolean mode list.
 * This routine makes no checks on limits.  It is expected to be used in
 * conjunction with the UtNameSpecifier routines.
 */

NameSpecifier *
BooleanList_NSpecLists(int index)
{
	static NameSpecifier	modeList[] = {

					{ "OFF",	GENERAL_BOOLEAN_OFF},
					{ "ON",		GENERAL_BOOLEAN_ON},
					{ "",		GENERAL_BOOLEAN_NULL},
				
				};
	return (&modeList[index]);

}

/****************************** DiagModeList **********************************/

/*
 * This routine returns a name specifier for the diagnostic mode list.
 * If the NULL value is returned, this is normally interpreted as using the
 * searched string as a file name.
 * This routine makes no checks on limits.  It is expected to be used in
 * conjunction with the UtNameSpecifier routines.
 */

NameSpecifier *
DiagModeList_NSpecLists(int index)
{
	static NameSpecifier	modeList[] = {

					{ "OFF",	GENERAL_DIAGNOSTIC_OFF_MODE},
					{ "SCREEN",	GENERAL_DIAGNOSTIC_SCREEN_MODE},
					{ "ERROR",	GENERAL_DIAGNOSTIC_ERROR_MODE},
					{ NO_FILE,	GENERAL_DIAGNOSTIC_FILE_MODE},
					{ "",		GENERAL_DIAGNOSTIC_MODE_NULL},
				
				};
	return (&modeList[index]);

}

/****************************** FitFuncModeList *******************************/

/*
 * This routine returns a name specifier for the fitfunction mode list.
 * This routine makes no checks on limits.  It is expected to be used in
 * conjunction with the UtNameSpecifier routines.
 */

NameSpecifier *
FitFuncModeList_NSpecLists(int index)
{
	static NameSpecifier	modeList[] = {

					{ "LOG_FUNC1",	GENERAL_FIT_FUNC_LOG_FUNC1_MODE},
					{ "POLY_FUNC1",	GENERAL_FIT_FUNC_POLY_FUNC1_MODE},
					{ "",			GENERAL_FIT_FUNC_NULL},
				
				};
	return (&modeList[index]);

}

/****************************** GetNumListEntries *****************************/

/*
 * This function returns the number of list entries.  It is a general
 * method which will allow me to simply change the number of entries without
 * introducing errors.
 */

int
GetNumListEntries_NSpecLists(NameSpecifierPtr list)
{
	static char *funcName = "GetNumListEntries_NSpecLists";
	int		count = 0;

	if (!list) {
		NotifyError("%s: list not initialised, -1 will be returned.", funcName);
		return(-1);
	}
	while ((list++)->name[0] != '\0')
		count++;
	return(count + 1);

}

/****************************** InitNameList **********************************/

/*
 * This routine intialises a name list array, using a prototype.
 */

NameSpecifier *
InitNameList_NSpecLists(NameSpecifierPtr prototypeList, char *textPtr)
{
	static char *funcName = "InitNameList_NSpecLists";
	int		i, numEntries;
	NameSpecifierPtr	list;
	
	if ((numEntries = GetNumListEntries_NSpecLists(prototypeList)) < 1) {
		NotifyError("%s: Prototyp list not set up correctly.", funcName);
		return(NULL);
	}
	if ((list = (NameSpecifier *) calloc(numEntries, sizeof(NameSpecifier))) ==
	  NULL) {
		NotifyError("%s: Could not allocate memory for %d entries.", funcName,
		  numEntries);
		return(NULL);
	}
	for (i = 0; i < numEntries; i++) {
		list[i] = prototypeList[i];
	}
	list[numEntries - 2].name = textPtr;
	return(list);

}

/****************************** IdentifyDiagMode ******************************/

/*
 * This is a special indentify routine for the diagnostic mode.
 * If the mode cannot be found or is the "file" mode then the second from
 * last, i.e. the mode string will be copied to the string pointed to by the
 * name specifier 'name' field of the 'GENERAL_DIAGNOSTIC_FILE_MODE' entry.
 */

int
IdentifyDiag_NSpecLists(char *mode, NameSpecifierPtr list)
{
	int		specifier;

	switch (specifier = Identify_NameSpecifier(mode, list)) {
	case GENERAL_DIAGNOSTIC_FILE_MODE:
	case GENERAL_DIAGNOSTIC_MODE_NULL:
		specifier = GENERAL_DIAGNOSTIC_FILE_MODE;
		sprintf(list[(int) GENERAL_DIAGNOSTIC_FILE_MODE].name, "%s", mode);
		break;
	default:
		;
	}
	return(specifier);

}

/****************************** OpenDiagnostics *******************************/

/*
 * This function sets up the diagnostics.
 * It returns FALSE if it fails in anyway.
 */

BOOLN
OpenDiagnostics_NSpecLists(FILE **fp, NameSpecifierPtr list, int mode)
{
	static const char *funcName = "OpenDiagnostics_NSpecLists";
	char	*fileName;

	switch (mode) {
	case GENERAL_DIAGNOSTIC_OFF_MODE:
		return(TRUE);
	case GENERAL_DIAGNOSTIC_FILE_MODE:
		fileName = list[(int) GENERAL_DIAGNOSTIC_FILE_MODE].name;
		if ((*fp = fopen(fileName, "w")) == NULL) {
			NotifyError("%s: Could not open file '%s' for diagnostics.",
			  funcName, fileName);
			return(FALSE);
		}
		break;
	case GENERAL_DIAGNOSTIC_SCREEN_MODE:
		*fp = stdout;
		break;
	default:
		NotifyError("%s: Mode was not correctly set (%d).", funcName, mode);
		return(FALSE);
	} /* switch */
	return(TRUE);

}

/****************************** CloseDiagnostics ******************************/

/*
 * This routine closes the output file pointer, if it is not the standard
 * output.
 */

void
CloseDiagnostics_NSpecLists(FILE **fp)
{
	if (*fp && (*fp != stdout)) {
		fclose(*fp);
		*fp = NULL;
	}

}

