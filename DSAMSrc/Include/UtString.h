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

void	CopyAndTrunc_Utility_String(char *dest, char *src, size_t n);

char *	GetFileNameFPath_Utility_String(char *fileName);

char *	GetSuffix_Utility_String(char *fileName);

char *	InitString_Utility_String(char *string);

char *	QuotedString_Utility_String(char *string);

void	ToUpper_Utility_String(char *upperCaseString, char *string);

int		StrCmpNoCase_Utility_String(char *s1, char *s2);

int		StrNCmpNoCase_Utility_String(char *fullString, char *abbrevString);

__END_DECLS

#endif
