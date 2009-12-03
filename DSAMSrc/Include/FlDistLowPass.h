/**********************
 *
 * File:		FlDistLowPass.h
 * Purpose:		This is an implementation of a 1st order Butterworth low-pass
 *				filter.  The filter is used to approximate the effects of
 *				dendrite filtering.
 *				This version has been revised from FlLowPass and produces a
 *				different filter for each channel.
 * Comments:	It uses the filter IIR filter from the UtFilter module.
 * Author:		L. P. O'Mard
 * Created:		26 Nov 2009
 * Updated:
 * Copyright:	(c) 2009,
 *
 **********************/

#ifndef	_FLDISTLOWPASS_H
#define _FLDISTLOWPASS_H	1

#include "UtNameSpecs.h"

/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

#define FILTER_DIST_LOW_PASS_NUM_PARS			3
#define FILTER_DIST_LOW_PASS_MOD_NAME			wxT("FILT_DISTLOWPASS")

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

typedef enum {

	FILTER_DIST_LOW_PASS_MODE,
	FILTER_DIST_LOW_PASS_SIGNALMULTIPLIER,
	FILTER_DISTLOWPASS_FREQDISTR

} DLowPassFParSpecifier;

typedef	enum {

	FILTER_DIST_LOW_PASS_MODE_NORMAL,
	FILTER_DIST_LOW_PASS_MODE_SCALED,
	FILTER_DIST_LOW_PASS_MODE_NULL

} DLowPassFModeSpecifier;

typedef	enum {

	FILTER_DIST_LOW_PASS_FREQ_DISTR_STANDARD,
	FILTER_DIST_LOW_PASS_FREQ_DISTR_USER,
	FILTER_DIST_LOW_PASS_FREQ_DISTR_NULL

} DLowPassFDistSpecifier;

typedef struct {

	ParameterSpecifier parSpec;

	BOOLN	modeFlag, cutOffFrequencyFlag, signalMultiplierFlag;
	BOOLN	updateProcessVariablesFlag;
	int		mode;
	Float	signalMultiplier;	/* potential mV */
	ParArrayPtr	freqDistr;

	/* Private members */
	UniParListPtr	parList;
	int		numChannels;
	ContButt1CoeffsPtr	*coefficients;
	NameSpecifier *modeList;

} DLowPassF, *DLowPassFPtr;

/******************************************************************************/
/*************************** External Variables *******************************/
/******************************************************************************/

extern	DLowPassFPtr	dLowPassFPtr;

/******************************************************************************/
/*************************** Function Prototypes ******************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

BOOLN	CheckPars_Filter_DLowPass(void);

BOOLN	Free_Filter_DLowPass(void);

int	GetNumFreqDistrPars_Filter_LowPassDist(int mode);

void	FreeProcessVariables_Filter_DLowPass(void);

UniParListPtr	GetUniParListPtr_Filter_DLowPass(void);

BOOLN	Init_Filter_DLowPass(ParameterSpecifier parSpec);

BOOLN	InitProcessVariables_Filter_DLowPass(EarObjectPtr data);

NameSpecifier *	ModeList_Filter_DLowPass(int index);

BOOLN	PrintPars_Filter_DLowPass(void);

BOOLN	RunProcess_Filter_DLowPass(EarObjectPtr data);

void	SetDefaultFreqDistr_Filter_DLowPass(ParArrayPtr freqDistr);

BOOLN	SetFreqDistr_Filter_DLowPass(ParArrayPtr theFreqDistr);

BOOLN	SetMode_Filter_DLowPass(WChar *theMode);

BOOLN	InitModule_Filter_DLowPass(ModulePtr theModule);

BOOLN	SetParsPointer_Filter_DLowPass(ModulePtr theModule);

BOOLN	SetSignalMultiplier_Filter_DLowPass(Float theUpperCutOffFreq);

BOOLN	SetUniParList_Filter_DLowPass(void);

__END_DECLS

#endif
