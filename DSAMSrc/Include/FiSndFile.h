/******************
 *
 * File:		FiSndFile.h
 * Purpose:		This Filing reads sound format files using the libsndfile
 * 				library.
 * Comments:	
 * Authors:		L. P. O'Mard
 * Created:		07 Nov 2006
 * Updated:		
 * Copyright:	(c) 2006, Lowel P. O'Mard
 * 
 ******************/

#ifndef	_FISNDFILE_H
#define _FISNDFILE_H	1

/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

/******************************************************************************/
/*************************** External variables *******************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Subroutine declarations **************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

void	Free_SndFile(void);

double	GetDuration_SndFile(WChar *fileName);

BOOLN	ReadFile_SndFile(WChar *fileName, EarObjectPtr data);

BOOLN	WriteFile_SndFile(WChar *fileName, EarObjectPtr data);

__END_DECLS

#endif
