/**********************
 *
 * File:		FlBandPass.h
 * Purpose:		This is an implementation of a pre-emphasis band pass filter
 *				using the filters.c module.
 *				Beauchamp K. and Yuen C. (1979) "Digital Methods for signal
 *				analysis", George Allen & Unwin, London. P 256-257.
 * Comments:	The band-pass filter is used to model the effects of the
 *				outer- and middle-ear on an input signal.
 *				LPO (10-09-95): A pre-attenuation parameter has been added to
 *				the module definition.
 *				20-07-98 LPO: Name changed from MoPreEmph.[ch]
 * Author:		L. P. O'Mard
 * Created:		12 Jul 1993
 * Updated:		20 Jul 1998
 * Copyright:	(c) 1998, University of Essex
 *
 **********************/

#ifndef	_FLBANDPASS_H
#define _FLBANDPASS_H	1

/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

#define BANDPASS_NUM_PARS			4

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

typedef enum {

	BANDPASS_FILTERCASCADE,
	BANDPASS_PREATTENUATION,
	BANDPASS_LOWERCUTOFFFREQ,
	BANDPASS_UPPERCUTOFFFREQ

} BandPassFParSpecifier;

typedef struct {

	ParameterSpecifier parSpec;

	BOOLN	updateProcessVariablesFlag;
	int		cascade;
	Float	preAttenuation;			/* Pre-attenuation for filter. */
	Float	upperCutOffFreq, lowerCutOffFreq;

	/* Private Members */
	UniParListPtr	parList;
	int		numChannels;
	BandPassCoeffsPtr	*coefficients;

} BandPassF, *BandPassFPtr;

/******************************************************************************/
/*************************** External Variables *******************************/
/******************************************************************************/

extern	BandPassFPtr	bandPassFPtr;

/******************************************************************************/
/*************************** Function Prototypes ******************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

BOOLN	Init_Filter_BandPass(ParameterSpecifier parSpec);

BOOLN	InitProcessVariables_Filter_BandPass(EarObjectPtr data);

BOOLN	Free_Filter_BandPass(void);

void	FreeProcessVariables_Filter_BandPass(void);

UniParListPtr	GetUniParListPtr_Filter_BandPass(void);

BOOLN	PrintPars_Filter_BandPass(void);

void	ResetProcess_Filter_BandPass(EarObjectPtr data);

BOOLN	RunModel_Filter_BandPass(EarObjectPtr data);

BOOLN	SetLowerCutOffFreq_Filter_BandPass(Float theLowerCutOffFreq);

BOOLN	SetCascade_Filter_BandPass(int theCascade);

BOOLN	SetPreAttenuation_Filter_BandPass(Float thePreAttenuation);

BOOLN	InitModule_Filter_BandPass(ModulePtr theModule);

BOOLN	SetParsPointer_Filter_BandPass(ModulePtr theModule);

BOOLN	SetUpperCutOffFreq_Filter_BandPass(Float theUpperCutOffFreq);

BOOLN	SetUniParList_Filter_BandPass(void);

__END_DECLS

#endif
