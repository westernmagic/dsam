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

#include "UtNameSpecs.h"

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define UTILITY_CONVMONAURAL_NUM_PARS			1

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef enum {

	UTILITY_CONVMONAURAL_MODE

} CMonauralParSpecifier;

typedef enum {

	UTILITY_CONVMONAURAL_MODE_ADD,
	UTILITY_CONVMONAURAL_MODE_LEFT,
	UTILITY_CONVMONAURAL_MODE_RIGHT,
	UTILITY_CONVMONAURAL_MODE_NULL

} UtilityConvMonauralModeSpecifier;

typedef struct {

	ParameterSpecifier	parSpec;

	int		mode;

	/* Private members */
	NameSpecifier	*modeList;
	UniParListPtr	parList;

} CMonaural, *CMonauralPtr;

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

BOOLN	Free_Utility_ConvMonaural(void);

UniParListPtr	GetUniParListPtr_Utility_ConvMonaural(void);

BOOLN	InitModeList_Utility_ConvMonaural(void);

BOOLN	CheckData_Utility_ConvMonaural(EarObjectPtr data);

BOOLN	InitModule_Utility_ConvMonaural(ModulePtr theModule);

BOOLN	SetParsPointer_Utility_ConvMonaural(ModulePtr theModule);

BOOLN	Init_Utility_ConvMonaural(ParameterSpecifier parSpec);

BOOLN	PrintPars_Utility_ConvMonaural(void);

BOOLN	Process_Utility_ConvMonaural(EarObjectPtr data);

BOOLN	SetMode_Utility_ConvMonaural(WChar * theMode);

BOOLN	SetParsPointer_Utility_ConvMonaural(ModulePtr theModule);

BOOLN	SetUniParList_Utility_ConvMonaural(void);

__END_DECLS

#endif
