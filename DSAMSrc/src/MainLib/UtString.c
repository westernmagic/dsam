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
ToUpper_Utility_String(WChar *upperCaseString, WChar *string)
{
	WChar		*p, *pp;

	for (p = upperCaseString, pp = string; *pp != '\0'; )
		*p++ = toupper(*pp++);
	*p = '\0';

}

/****************************** StrCmpNoCase *********************************/

/*
 * This routine carries out the standard DSAM_strcmp function, but it ignores
 * the case of the operands by converting them both to uppercase.
 */

int
StrCmpNoCase_Utility_String(WChar *s1, WChar *s2)
{
	static WChar *funcName = wxT("StrCmpNoCase_Utility_String");
	WChar	upperString[2][LONG_STRING], *string[2];
	int		i, len[2];

	string[0] = s1;
	string[1] = s2;
	for (i = 0; i < 2; i++) {
		if ((len[i] = DSAM_strlen(string[i])) > LONG_STRING) {
			NotifyError(wxT("%s: String '%s' exceeds current available length "
			  "(%d)."), funcName, string[i], LONG_STRING);
			return (-1);
		}
		ToUpper_Utility_String(upperString[i], string[i]);
	}
	return(DSAM_strcmp(upperString[0], upperString[1]));

}

/****************************** StrNCmpNoCase *********************************/

/*
 * This routine carries out the standard strncmp function, but it ignores the
 * case of the operands by converting them both to uppercase.
 * The length used is that of the second argument.
 */

int
StrNCmpNoCase_Utility_String(WChar *fullString, WChar *abbrevString)
{
	static WChar *funcName = wxT("StrNCmpNoCase_Utility_String");
	WChar	upperString[2][LONG_STRING], *string[2];
	int		i, len[2];

	string[0] = fullString;
	string[1] = abbrevString;
	for (i = 0; i < 2; i++) {
		if ((len[i] = DSAM_strlen(string[i])) > LONG_STRING) {
			NotifyError(wxT("%s: String '%s' exceeds current available length "
			  "(%d)."), funcName, string[i], LONG_STRING);
			return (-1);
		}
		ToUpper_Utility_String(upperString[i], string[i]);
	}
	return(DSAM_strncmp(upperString[0], upperString[1], len[1]));

}

/****************************** InitString ************************************/

/*
 * This routine allocates memory for a string and returns a pointer to the
 * the string.
 * It exits terminally if it fails.
 */

WChar *
InitString_Utility_String(WChar *string)
{
	static const WChar	*funcName = wxT("InitString_Utility_String");
	WChar	*p;

	if ((p = (WChar *) calloc((DSAM_strlen(string) + 1), sizeof(WChar))) ==
	  NULL) {
		NotifyError(wxT("%s: Out of memory for string '%s'."), funcName,
		  string);
		exit(1);
	}
	DSAM_strcpy(p, string);
	return(p);

}

/****************************** QoutedString **********************************/

/*
 * This routine returns a pointer to a temporary quoted string.  A copy should
 * be made if a permanent string is required.
 */

WChar *
QuotedString_Utility_String(WChar *string)
{
	static WChar		newString[LONG_STRING];

	DSAM_snprintf(newString, LONG_STRING, wxT("\"%s\""), string);
	return (newString);

}

/**************************** GetSuffix ***************************************/

/*
 * This routine returns the suffix of a file name i.e. any WCharacters
 * after a ".".
 * It returns the entire file name if no suffix is returned.
 */
 
WChar *
GetSuffix_Utility_String(WChar *fileName)
{
	WChar	*p;

	if ((p = DSAM_strrchr(fileName, '.')) != NULL)
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
 
WChar *
GetFileNameFPath_Utility_String(WChar *fileName)
{
	WChar	*p;

	p = DSAM_strrchr(fileName, '/');
	if (!p)
		p = DSAM_strrchr(fileName, '\\');
	if (p == NULL)
		return(fileName);
	return(++p);

}

/**************************** RemoveChar **************************************/

/*
 * This routine removes a specified character from a string.
 * It expects the string to be properly terminated with a null character.
 */
 
WChar *
RemoveChar_Utility_String(WChar *string, WChar c)
{
	WChar	*p1, *p2;

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
 
WChar *
SubStrReplace_Utility_String(WChar *string, WChar *subString, WChar *repString)
{
	WChar	*s;
	int		subSLen = DSAM_strlen(subString), repSLen = DSAM_strlen(repString);

	if ((s = DSAM_strstr(string, subString)) == NULL)
		return(NULL);
	memmove(s + repSLen, s + subSLen, DSAM_strlen(s) - subSLen + 1);
	memcpy(s, repString, repSLen);
	return(string);

}

/**************************** ConvUTF8 ****************************************/

/*
 * This function returns a pointer to a UTF8 string.  if working in non-unicode
 * format, then the original string is returned.
 * The returned string should be considered as temporary.
 */
 
char *
ConvUTF8_Utility_String(WChar *src)
{
#	ifndef DSAM_USE_UNICODE
	return(src);
#	else
	static const WChar *funcName = wxT("ConvUTF8_Utility_String");
	static char	dest[MAXLINE];

	if (wcstombs(dest, src, MAXLINE - 1) < 0 ) {
		NotifyError(wxT("%s: Failed to convert wide character string (%d)."),
		  funcName, MAXLINE);
		return(NULL);
	}
	return(dest);
#endif

}
