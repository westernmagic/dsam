/**********************
 *
 * File:		UtHalfWRectify.h
 * Purpose:		This proces module half-wave rectifies a signal, setting
 *				all negative values to zero.
 * Comments:	Written using ModuleProducer version 1.2.6 (Jul 22 1999).
 * Author:		L. P. O'Mard
 * Created:		22 Jul 1999
 * Updated:	
 * Copyright:	(c) 1999, University of Essex.
 *
 *********************/

#ifndef _UTHALFWRECTIFY_H
#define _UTHALFWRECTIFY_H 1

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

BOOLN	CheckData_Utility_HalfWaveRectify(EarObjectPtr data);

BOOLN	InitModule_Utility_HalfWaveRectify(ModulePtr theModule);

BOOLN	SetParsPointer_Utility_HalfWaveRectify(ModulePtr theModule);

BOOLN	Process_Utility_HalfWaveRectify(EarObjectPtr data);

__END_DECLS

#endif
