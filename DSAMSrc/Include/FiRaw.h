/******************
 *
 * File:		FiRaw.h
 * Purpose:		This Filing module deals with the writing reading of raw binary
 *				data files.
 * Comments:	14-04-98 LPO: This module has now been separated from the main
 *				FiDataFile module.
 *				17-04-98 LPO: Corrected unix pipe reading behaviour, required
 *				because pipes cannot use the "fseek" function used by
 *				SetPosition_UPortableIO(...).
 *				30-04-98 LPO: Used the _WorldTime_EarObject macro to correct
 *				the writing of the first segment in segmented mode, i.e. the
 *				file is created for writing not appending.
 *				Note that in segmented mode the normalisation calculation
 *				operates on the first segment only.
 * Authors:		L. P. O'Mard
 * Created:		14 Apr 1998
 * Updated:		30 Apr 1998
 * Copyright:	(c) 1998, University of Essex
 * 
 ******************/

#ifndef	_FIRAW_H
#define _FIRAW_H	1

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

BOOLN		ReadFile_Raw(char *fileName, EarObjectPtr data);

BOOLN		WriteFile_Raw(char *fileName, EarObjectPtr data);

__END_DECLS

#endif
