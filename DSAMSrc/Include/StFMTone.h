/**********************
 *
 * File:		StFMTone.h
 * Purpose:		Frequency modulated pure tone signal generation paradigm.
 * Comments:	Written using ModuleProducer version 1.5.
 * Author:		Almudena
 * Created:		Nov 28 1995
 * Updated:		12 Mar 1997
 * Copyright:	(c) 1998, University of Essex
 *
 **********************/

#ifndef _STFMTONE_H
#define _STFMTONE_H 1

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define PURETONE_FM_NUM_PARS			8

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef enum {

	PURETONE_FM_FREQUENCY,
	PURETONE_FM_INTENSITY,
	PURETONE_FM_PHASE,
	PURETONE_FM_MODULATIONDEPTH,
	PURETONE_FM_MODULATIONFREQUENCY,
	PURETONE_FM_MODULATIONPHASE,
	PURETONE_FM_DURATION,
	PURETONE_FM_SAMPLINGINTERVAL

} FMToneParSpecifier;

typedef struct {

	ParameterSpecifier	parSpec;

	BOOLN	frequencyFlag, intensityFlag, durationFlag, dtFlag, phaseFlag;
	BOOLN	modulationDepthFlag, modulationFrequencyFlag, modulationPhaseFlag;
	double	frequency;
	double	intensity;
	double	duration;
	double	dt;
	double	phase;
	double	modulationDepth;
	double	modulationFrequency;
	double	modulationPhase;

	/* Private members */
	UniParListPtr	parList;

} FMTone, *FMTonePtr;

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

extern	FMTonePtr	fMTonePtr;

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

BOOLN	CheckData_PureTone_FM(EarObjectPtr data);

BOOLN	CheckPars_PureTone_FM(void);

BOOLN	Free_PureTone_FM(void);

BOOLN	GenerateSignal_PureTone_FM(EarObjectPtr data);

UniParListPtr	GetUniParListPtr_PureTone_FM(void);

BOOLN	Init_PureTone_FM(ParameterSpecifier parSpec);

BOOLN	PrintPars_PureTone_FM(void);

BOOLN	ReadPars_PureTone_FM(WChar *fileName);

BOOLN	SetModulationDepth_PureTone_FM(double theModulationDepth);

BOOLN	SetDuration_PureTone_FM(double theDuration);

BOOLN	SetModulationFrequency_PureTone_FM(double theModulationFrequency);

BOOLN	SetFrequency_PureTone_FM(double theFrequency);

BOOLN	SetIntensity_PureTone_FM(double theIntensity);

BOOLN	InitModule_PureTone_FM(ModulePtr theModule);

BOOLN	SetParsPointer_PureTone_FM(ModulePtr theModule);

BOOLN	SetPars_PureTone_FM(double frequency, double intensity,
		  double duration, double samplingInterval, double phase,
		  double modulationDepth, double modulationFrequency,
		  double modulationPhase);

BOOLN	SetModulationPhase_PureTone_FM(double thePhaseFM);

BOOLN	SetPhase_PureTone_FM(double thePhase);

BOOLN	SetSamplingInterval_PureTone_FM(double theSamplingInterval);

BOOLN	SetUniParList_PureTone_FM(void);

__END_DECLS

#endif
