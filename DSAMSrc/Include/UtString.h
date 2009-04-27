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

void	ConvWCSIOFormat_Utility_String(wchar_t *dest, const wchar_t *src);

BOOLN	ConvIOFormat_Utility_String(WChar *dest, const WChar *s, size_t size);

WChar *	MBSToWCS_Utility_String(const char *mb);

WChar *	MBSToWCS2_Utility_String(const char *mb);

WChar	MBToWC_Utility_String(const char *mb);

char *	ConvUTF8_Utility_String(const WChar *src);

int	fprintf_Utility_String(FILE *fp, const WChar *format, ...);

WChar *	GetFileNameFPath_Utility_String(WChar *fileName);

WChar *	GetSuffix_Utility_String(WChar *fileName);

WChar *	InitString_Utility_String(WChar *string);

WChar *	QuotedString_Utility_String(WChar *string);

void	ToUpper_Utility_String(WChar *upperCaseString, const WChar *string);

WChar *	RemoveChar_Utility_String(WChar *string, WChar c);

int		Snprintf_Utility_String(WChar *str, size_t size,  const WChar *format, ...);

int		StrCmpNoCase_Utility_String(WChar *s1, WChar *s2);

int		StrNCmpNoCase_Utility_String(const WChar *fullString, const WChar *abbrevString);

WChar *	SubStrReplace_Utility_String(WChar *string, WChar *subString,
		  WChar *repString);

int		Vsnprintf_Utility_String(WChar *str, size_t size, const WChar *format,
		  va_list args);

__END_DECLS

#endif
