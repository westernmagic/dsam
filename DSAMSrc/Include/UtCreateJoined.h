/**********************
 *
 * File:		UtCreateJoined.h
 * Purpose:		To create a signal by joining the output signals of multiply
 *				connected EarObjects.
 * Comments:	Written using ModuleProducer version 1.11 (Apr  9 1997).
 * Author:		L. P. O'Mard
 * Created:		26 Aug 1997
 * Updated:	
 * Copyright:	(c) 1997, University of Essex
 *
 *********************/

#ifndef _UTCREATEJOINED_H
#define _UTCREATEJOINED_H 1

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

BOOLN	CheckData_Utility_CreateJoined(EarObjectPtr data);

BOOLN	Process_Utility_CreateJoined(EarObjectPtr data);

BOOLN	InitModule_Utility_CreateJoined(ModulePtr theModule);

BOOLN	SetParsPointer_Utility_CreateJoined(ModulePtr theModule);

__END_DECLS

#endif
