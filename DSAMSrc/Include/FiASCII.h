/******************
 *
 * File:		FiASCII.h
 * Purpose:		This Filing module deals with the writing reading of ASCII
 *				data files.
 * Comments:	14-04-98 LPO: This module has now been separated from the main
 *				FiDataFile module.
 * Authors:		L. P. O'Mard
 * Created:		17 Apr 1998
 * Updated:		
 * Copyright:	(c) 1998, University of Essex
 * 
 ******************/

#ifndef	_FIASCII_H
#define _FIASCII_H	1

/******************************************************************************/
/*************************** Constant Definitions *****************************/
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

BOOLN		ReadFile_ASCII(char *fileName, EarObjectPtr data);

BOOLN		WriteFile_ASCII(char *fileName, EarObjectPtr data);

__END_DECLS

#endif
