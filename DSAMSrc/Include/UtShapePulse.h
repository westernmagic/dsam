/**********************
 *
 * File:		UtShapePulse.h
 * Purpose:		This module turns a train of detected spikes into a pulse
 *				train with set magnitudes and durations.
 * Comments:	Written using ModuleProducer version 1.9 (Feb 29 1996).
 * Author:		L. P. O'Mard
 * Created:		18th April 1996
 * Updated:
 * Copyright:	(c) 1998, University of Essex.
 *
 *********************/

#ifndef _UTSHAPEPULSE_H
#define _UTSHAPEPULSE_H 1

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define UTILITY_SHAPEPULSE_NUM_PARS			3

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef enum {

	UTILITY_SHAPEPULSE_EVENTTHRESHOLD,
	UTILITY_SHAPEPULSE_PULSEDURATION,
	UTILITY_SHAPEPULSE_PULSEMAGNITUDE

} ShapePulseParSpecifier;

typedef struct {

	ParameterSpecifier	parSpec;

	BOOLN	updateProcessVariablesFlag;
	Float	eventThreshold;
	Float	pulseDuration;
	Float	pulseMagnitude;

	/* Private members */
	Float	*remainingPulseTime;
	UniParListPtr	parList;

} ShapePulse, *ShapePulsePtr;

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

extern	ShapePulsePtr	shapePulsePtr;

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

BOOLN	CheckData_Utility_ShapePulse(EarObjectPtr data);

BOOLN	Free_Utility_ShapePulse(void);

void	FreeProcessVariables_Utility_ShapePulse(void);

UniParListPtr	GetUniParListPtr_Utility_ShapePulse(void);

BOOLN	Init_Utility_ShapePulse(ParameterSpecifier parSpec);

BOOLN	InitProcessVariables_Utility_ShapePulse(EarObjectPtr data);

BOOLN	PrintPars_Utility_ShapePulse(void);

BOOLN	Process_Utility_ShapePulse(EarObjectPtr data);

void	ResetProcess_Utility_ShapePulse(EarObjectPtr data);

BOOLN	SetEventThreshold_Utility_ShapePulse(Float theEventThreshold);

BOOLN	InitModule_Utility_ShapePulse(ModulePtr theModule);

BOOLN	SetParsPointer_Utility_ShapePulse(ModulePtr theModule);

BOOLN	SetPulseDuration_Utility_ShapePulse(Float thePulseDuration);

BOOLN	SetPulseMagnitude_Utility_ShapePulse(Float thePulseMagnitude);

BOOLN	SetUniParList_Utility_ShapePulse(void);

__END_DECLS

#endif
