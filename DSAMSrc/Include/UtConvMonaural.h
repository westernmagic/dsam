/**********************
 *
 * File:		UtConvMonaural.h
 * Purpose:		This module converts a binaural signal to a monaural signal.
 * Comments:	Written using ModuleProducer version 1.10 (Jan  3 1997).
 * Author:		L. P. O'Mard
 * Convd:		5 Mar 1997
 * Updated:	
 * Copyright:	(c) 1997, University of Essex.
 *
 *********************/

#ifndef _UTCONVMONAURAL_H
#define _UTCONVMONAURAL_H 1

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

BOOLN	CheckData_Utility_ConvMonaural(EarObjectPtr data);

BOOLN	InitModule_Utility_ConvMonaural(ModulePtr theModule);

BOOLN	SetParsPointer_Utility_ConvMonaural(ModulePtr theModule);

BOOLN	Process_Utility_ConvMonaural(EarObjectPtr data);

__END_DECLS

#endif
