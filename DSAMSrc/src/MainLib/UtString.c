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

/****************************** StrCmpNoCase *********************************/

/*
 * This routine carries out the standard strcmp function, but it ignores the
 * case of the operands by converting them both to uppercase.
 */

int
StrCmpNoCase_Utility_String(char *s1, char *s2)
{
	static char *funcName = "StrCmpNoCase_Utility_String";
	char	upperString[2][LONG_STRING], *string[2];
	int		i, len[2];

	string[0] = s1;
	string[1] = s2;
	for (i = 0; i < 2; i++) {
		if ((len[i] = strlen(string[i])) > LONG_STRING) {
			NotifyError("%s: String '%s' exceeds current available length "
			  "(%d).", funcName, string[i], LONG_STRING);
			return (-1);
		}
		ToUpper_Utility_String(upperString[i], string[i]);
	}
	return(strcmp(upperString[0], upperString[1]));

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
 */
 
char *
GetSuffix_Utility_String(char *fileName)
{
	char	*p;

	if ((p = strrchr(fileName, '.')) != NULL)
		return(p + 1);
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

/**************************** RemoveChar **************************************/

/*
 * This routine removes a specified character from a string.
 * It expects the string to be properly terminated with a null character.
 */
 
char *
RemoveChar_Utility_String(char *string, char c)
{
	char	*p1, *p2;

	for (p1 = p2 = string; *p2; p1++)
		if (*p1 != c)
			*p2++ = *p1;
	*p2 = '\0';
	return(string);

}

/**************************** SubStrReplace ***********************************/

/*
 * This routine substitutes a substring within a string.  It returns
 * NULL if the sub-string is not found. otherwise it returns the position
 * of the start of the substituted string.
 * It expects the argument strings to be properly terminated with a null
 * character.
 * It expects the 'string' variable to be large enough to hold the newly created
 * string.
 */
 
char *
SubStrReplace_Utility_String(char *string, char *subString, char *repString)
{
	char	*s;
	int		subSLen = strlen(subString), repSLen = strlen(repString);

	if ((s = strstr(string, subString)) == NULL)
		return(NULL);
	memmove(s + repSLen, s + subSLen, strlen(s) - subSLen + 1);
	memcpy(s, repString, repSLen);
	return(string);

}
