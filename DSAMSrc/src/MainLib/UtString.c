/**********************
 *
 * File:		UtString.c
 * Purpose:		This module contains routines that do string handling.
 * Comments:	08-06-99 LPO: Added the 'StrNCmpNoCase' routine.
 *				23-08-99 LPO: Moved the 'InitString_Utility_SimScript' to this
 *				module.
 * Author:		L. P. O'Mard
 * Created:		07 Oct 1998
 * Updated:		23 Aug 1999
 * Copyright:	(c) 1999, University of Essex
 *
 *********************/

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG_H */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "GeCommon.h"
#include "UtString.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Subroutines and functions *********************/
/******************************************************************************/


/****************************** ToUpper ***************************************/

/*
 * This routine copies the second string into the first, converting all
 * characters to upper case.
 * It carries out no checks on string lengths or the validity of strings.
 */

void
ToUpper_Utility_String(char *upperCaseString, char *string)
{
	char		*p, *pp;

	for (p = upperCaseString, pp = string; *pp != '\0'; )
		*p++ = toupper(*pp++);
	*p = '\0';

}

/****************************** StrNCmpNoCase *********************************/

/*
 * This routine carries out the standard strncmp function, but it ignores the
 * case of the operands by converting them both to uppercase.
 * The length used is that of the second argument.
 */

int
StrNCmpNoCase_Utility_String(char *fullString, char *abbrevString)
{
	static char *funcName = "StrNCmpNoCase_Utility_String";
	char	upperString[2][LONG_STRING], *string[2];
	int		i, len[2];

	string[0] = fullString;
	string[1] = abbrevString;
	for (i = 0; i < 2; i++) {
		if ((len[i] = strlen(string[i])) > LONG_STRING) {
			NotifyError("%s: String '%s' exceeds current available length "
			  "(%d).", funcName, string[i], LONG_STRING);
			return (-1);
		}
		ToUpper_Utility_String(upperString[i], string[i]);
	}
	return(strncmp(upperString[0], upperString[1], len[1]));

}

/****************************** InitString ************************************/

/*
 * This routine allocates memory for a string and returns a pointer to the
 * the string.
 * It exits terminally if it fails.
 */

char *
InitString_Utility_String(char *string)
{
	static const char	*funcName = "InitString_Utility_String";
	char	*p;

	if ((p = (char *) malloc(strlen(string) + 1)) == NULL) {
		NotifyError("%s: Out of memory for string '%s'.", funcName,
		  string);
		exit(1);
	}
	strcpy(p, string);
	return(p);

}

/****************************** QoutedString **********************************/

/*
 * This routine returns a pointer to a temporary quoted string.  A copy should
 * be made if a permanent string is required.
 */

char *
QuotedString_Utility_String(char *string)
{
	static char		newString[LONG_STRING];

	snprintf(newString, LONG_STRING, "\"%s\"", string);
	return (newString);

}

/**************************** GetSuffix ***************************************/

/*
 * This routine returns the suffix of a file name i.e. any characters
 * after a ".".
 * It returns the entire file name if no suffix is returned.
 * 
 */
 
char *
GetSuffix_Utility_String(char *fileName)
{
	static char scanLine[MAXLINE]; /* - must be static so that memory is kept.*/
	char	*token;

	/* Remember, strtok clobbers scanLine. */
	CopyAndTrunc_Utility_String(scanLine, fileName, MAXLINE);
	strtok(scanLine, ".");		/* Initialisation */
	token = strtok(NULL, " ");
	if (token != NULL)
		return(token);
	else 
		return(fileName);
		
}

/**************************** GetFileNameFPath ********************************/

/*
 * This routine returns the filename with the path removed.
 * It returns the entire file name if there is no file path.
 * 
 */
 
char *
GetFileNameFPath_Utility_String(char *fileName)
{
	char	*p;

	p = strrchr(fileName, '/');
	if (!p)
		p = strrchr(fileName, '\\');
	if (p == NULL)
		return(fileName);
	return(++p);

}

/**************************** CopyAndTrunc ************************************/

/*
 * This routine copies only n parameters from the src, and ensures that the
 * destination is null terminated.
 * 
 */
 
void
CopyAndTrunc_Utility_String(char *dest, char *src, size_t n)
{
	strncpy(dest, src, n);
	dest[n - 1] = '\0';

}
