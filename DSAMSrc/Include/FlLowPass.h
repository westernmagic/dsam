/**********************
 *
 * File:		FlLowPass.h
 * Purpose:		This is an implementation of a 1st order Butterworth low-pass
 *				filter.  The filter is used to approximate the effects of
 *				dendrite filtering.
 * Comments:	It uses the filter IIR filter from the UtFilter module.
 * Author:		L. P. O'Mard
 * Created:		12 Jul 1993
 * Updated:		03 Dec 1996
 * Copyright:	(c) 1998, University of Essex
 *
 **********************/

#ifndef	_FLLOWPASS_H
#define _FLLOWPASS_H	1

#include "UtNameSpecs.h"

/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

#define FILTER_LOW_PASS_NUM_PARS			3

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

typedef enum {

	FILTER_LOW_PASS_MODE,
	FILTER_LOW_PASS_CUTOFFFREQUENCY,
	FILTER_LOW_PASS_SIGNALMULTIPLIER

} LowPassFParSpecifier;

typedef	enum {

	FILTER_LOW_PASS_MODE_NORMAL,
	FILTER_LOW_PASS_MODE_SCALED,
	FILTER_LOW_PASS_MODE_NULL

} LowPassFModeSpecifier;

typedef struct {
	
	ParameterSpecifier parSpec;
	
	BOOLN	modeFlag, cutOffFrequencyFlag, signalMultiplierFlag;
	BOOLN	updateProcessVariablesFlag;
	int		mode;
	double	cutOffFrequency;
	double	signalMultiplier;	/* potential mV */

	/* Private members */
	UniParListPtr	parList;
	int		numChannels;
	ContButt1CoeffsPtr	*coefficients;
	NameSpecifier *modeList;
	
} LowPassF, *LowPassFPtr;

/******************************************************************************/
/*************************** External Variables *******************************/
/******************************************************************************/

extern	LowPassFPtr	lowPassFPtr;

/******************************************************************************/
/*************************** Function Prototypes ******************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

BOOLN	CheckPars_Filter_LowPass(void);

BOOLN	Free_Filter_LowPass(void);

void	FreeProcessVariables_Filter_LowPass(void);

UniParListPtr	GetUniParListPtr_Filter_LowPass(void);

BOOLN	Init_Filter_LowPass(ParameterSpecifier parSpec);

BOOLN	InitModeList_Filter_LowPass(void);

BOOLN	InitProcessVariables_Filter_LowPass(EarObjectPtr data);

BOOLN	PrintPars_Filter_LowPass(void);

BOOLN	ReadPars_Filter_LowPass(char *fileName);

BOOLN	RunModel_Filter_LowPass(EarObjectPtr data);

BOOLN	SetCutOffFrequency_Filter_LowPass(double theCutOffFrequency);

BOOLN	SetMode_Filter_LowPass(char *theMode);

BOOLN	InitModule_Filter_LowPass(ModulePtr theModule);

BOOLN	SetParsPointer_Filter_LowPass(ModulePtr theModule);

BOOLN	SetPars_Filter_LowPass(char *mode, double theLowerCutOffFreq,
		  double theSignalMultiplier);
 
BOOLN	SetSignalMultiplier_Filter_LowPass(double theUpperCutOffFreq);

BOOLN	SetUniParList_Filter_LowPass(void);

__END_DECLS

#endif
