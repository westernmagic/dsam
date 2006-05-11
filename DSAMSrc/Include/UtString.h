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

#ifndef	_UTSTRING_H
#define _UTSTRING_H	1

/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Macro definitions ********************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

/******************************************************************************/
/*************************** External Variables *******************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Global Subroutines *******************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

char *	ConvUTF8_Utility_String(WChar *src);

WChar *	GetFileNameFPath_Utility_String(WChar *fileName);

WChar *	GetSuffix_Utility_String(WChar *fileName);

WChar *	InitString_Utility_String(WChar *string);

WChar *	QuotedString_Utility_String(WChar *string);

void	ToUpper_Utility_String(WChar *upperCaseString, WChar *string);

WChar *	RemoveChar_Utility_String(WChar *string, WChar c);

int		StrCmpNoCase_Utility_String(WChar *s1, WChar *s2);

int		StrNCmpNoCase_Utility_String(WChar *fullString, WChar *abbrevString);

WChar *	SubStrReplace_Utility_String(WChar *string, WChar *subString,
		  WChar *repString);

__END_DECLS

#endif
