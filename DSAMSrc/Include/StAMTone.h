/**********************
 *
 * File:		StAMTone.h
 * Purpose:		This module contains the methods for the AM-tone (amplitude
 *				modulated) generation paradigm.
 * Comments:	09-07-98 LPO: Carrier frequency is now in sine phase and not cos
 *				phase.
 * Author:		L. P. O'Mard
 * Created:		12 Jul 1993
 * Updated:		09 Jul 1998
 * Copyright:	(c) 1998, University of Essex
 *
 *********************/

#ifndef	_STAMTONE_H
#define _STAMTONE_H	1
 
/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

#define PURETONE_AM_NUM_PARS		6
#define	AM_TONE_NUM_CHANNELS		1		/* No. of stimulus channels to
											 * initialise. */

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

typedef enum {

	AM_TONE_FREQUENCY,
	AM_TONE_MODULATIONFREQUENCY,
	AM_TONE_MODULATIONDEPTH,
	AM_TONE_INTENSITY,
	AM_TONE_DURATION,
	AM_TONE_SAMPLINGINTERVAL

} AMToneParSpecifier;

typedef struct {

	ParameterSpecifier parSpec;
	
	BOOLN	frequencyFlag, modulationFrequencyFlag;
	BOOLN	modulationDepthFlag, durationFlag, dtFlag;
	BOOLN	intensityFlag;
	double	frequency, modulationFrequency;
	double  modulationDepth, intensity;
	double	duration, dt;

	/* Private members */
	UniParListPtr	parList;

} AMTone, *AMTonePtr;
	
/******************************************************************************/
/*************************** External Variables *******************************/
/******************************************************************************/

extern	AMTonePtr	aMTonePtr;

/******************************************************************************/
/*************************** Function Prototypes ******************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

BOOLN	CheckPars_PureTone_AM(void);

BOOLN	Free_PureTone_AM(void);

BOOLN	GenerateSignal_PureTone_AM(EarObjectPtr theObject);

UniParListPtr	GetUniParListPtr_PureTone_AM(void);

BOOLN	Init_PureTone_AM(ParameterSpecifier parSpec);

BOOLN	PrintPars_PureTone_AM(void);

BOOLN	ReadPars_PureTone_AM(char *fileName);

BOOLN	SetFrequency_PureTone_AM(double theFrequency);

BOOLN	SetDuration_PureTone_AM(double theDuration);

BOOLN	SetIntensity_PureTone_AM(double theIntensity);

BOOLN	SetModulationFrequency_PureTone_AM(double theModulationFrequency);

BOOLN	InitModule_PureTone_AM(ModulePtr theModule);

BOOLN	SetParsPointer_PureTone_AM(ModulePtr theModule);

BOOLN	SetPars_PureTone_AM(double theFrequency, double theModulationFrequency,
		  double theModulationDepth, double theIntensity, double theDuration,
		  double theSamplingInterval);

BOOLN	SetModulationDepth_PureTone_AM(double theModulationDepth);

BOOLN	SetSamplingInterval_PureTone_AM(double theSamplingInterval);

BOOLN	SetUniParList_PureTone_AM(void);

__END_DECLS

#endif
