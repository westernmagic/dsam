/**********************
 *
 * File:		UtDelay.h
 * Purpose:		This module introduces a initialDelay in a monaural signal or an
 *				interaural time difference (ITD) in a binaural signal.
 * Comments:	Written using ModuleProducer version 1.10 (Jan  3 1997).
 *				For binaural signals if the initialDelay is positive, then the
 *				right (second) channel is initialDelayed relative to the left
 *				channel, and vice versus for negative initialDelays.
 *				Monaural signals always treat initialDelays as positive values.
 * Author:		L. P. O'Mard
 * Created:		08 Apr 1997
 * Updated:	
 * Copyright:	(c) 1997, University of Essex
 *
 *********************/

#ifndef _UTDELAY_H
#define _UTDELAY_H 1

#include "UtNameSpecs.h"

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define UTILITY_DELAY_NUM_PARS			3

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef enum {

	UTILITY_DELAY_MODE,
	UTILITY_DELAY_INITIALDELAY,
	UTILITY_DELAY_FINALDELAY

} Delay2ParSpecifier;

typedef enum {

	DELAY_SINGLE_MODE,
	DELAY_LINEAR_MODE,
	DELAY_NULL

} DelayModeSpecifier;

typedef struct {

	ParameterSpecifier	parSpec;

	BOOLN	modeFlag, initialDelayFlag, finalDelayFlag;
	int		mode;
	double	initialDelay;
	double	finalDelay;

	/* Private members */
	NameSpecifier	*modeList;
	UniParListPtr	parList;

} Delay2, *Delay2Ptr;

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

extern	Delay2Ptr	delay2Ptr;

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

BOOLN	CheckData_Utility_Delay(EarObjectPtr data);

BOOLN	CheckPars_Utility_Delay(void);

BOOLN	Free_Utility_Delay(void);

UniParListPtr	GetUniParListPtr_Utility_Delay(void);

BOOLN	Init_Utility_Delay(ParameterSpecifier parSpec);

BOOLN	InitModeList_Utility_Delay(void);

BOOLN	PrintPars_Utility_Delay(void);

BOOLN	Process_Utility_Delay(EarObjectPtr data);

BOOLN	ReadPars_Utility_Delay(char *fileName);

BOOLN	SetFinalDelay_Utility_Delay(double theFinalDelay);

BOOLN	SetInitialDelay_Utility_Delay(double theInitialDelay);

BOOLN	SetMode_Utility_Delay(char *theMode);

BOOLN	InitModule_Utility_Delay(ModulePtr theModule);

BOOLN	SetParsPointer_Utility_Delay(ModulePtr theModule);

BOOLN	SetPars_Utility_Delay(char *mode, double initialDelay,
		  double finalDelay);

BOOLN	SetUniParList_Utility_Delay(void);

__END_DECLS

#endif
