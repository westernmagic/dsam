/**********************
 *
 * File:		AnSAImage.h
 * Purpose:		This module implements the AIM stabilised auditory image (SAI)
 *				process.
 *				See Patterson R. D. and Allerhand M. H. (1995) "Time-domain
 *				modeling of peripheral auditory processing: A modular
 *				Architecture and a software platform", J. Acoust. Soc. Am. 98,
 *				1890-1894.
 * Comments:	Written using ModuleProducer version 1.12 (Oct 12 1997).
 *				This module uses the UtStrobe utility module.
 *				29-10-97 LPO: Introduced 'dataBuffer' EarObject for the
 *				correct treatment during segment processing.  The previous data
 *				is assumed to be the negative width plus zero.
 *				04-11-97 LPO: Introduced the correct 'strobe is time zero'
 *				behaviour.
 *				04-11-97 LPO: Improved buffer algorithm so the signal segments
 *				less than the SAI length can be used.
 *				18-11-97 LPO: Introduced the inputDecayRate parameter.
 *				11-12-97 LPO: Allowed the "delayed" strobe modes to be processed
 *				correctly, by interpreting delays as an additional shift using
 *				the postive and negative widths.
 *				28-05-98 LPO: Put in check for zero image decay half-life.
 *				09-06-98 LPO: The strobe information is now output as part of
 *				the diagnostics.
 *				12-06-98 LPO: Corrected numLastSamples problem, which was
 *				causing strobes to be processed passed the end of the strobe
 *				data.
 *				30-06-98 LPO: Introduced use of the SignalData structure's
 *				'numWindowFrames' field.
 *				08-10-98 LPO: The initialisation and free'ing of the 'Strobe'
 *				utility module is now done at the same time as the SAI module.
 *				This was necessary for the UniPar implementation.
 * Author:		L. P. O'Mard
 * Created:		12 Oct 1997
 * Updated:		08 Oct 1998
 * Copyright:	(c) 1998, University of Essex
 *
 *********************/

#ifndef _ANSAIMAGE_H
#define _ANSAIMAGE_H 1

#include "UtStrobe.h"
#include "UtNameSpecs.h"

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define SAI_NUM_PARS			7

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef enum {

	SAI_DIAGNOSTIC_MODE,
	SAI_INTEGRATION_MODE,
	SAI_STROBE_SPECIFICATION,
	SAI_NEGATIVE_WIDTH,
	SAI_POSITIVE_WIDTH,
	SAI_INPUT_DECAY_RATE,
	SAI_IMAGE_DECAY_HALF_LIFE

} SAIParSpecifier;

typedef enum {

	SAI_INTEGRATION_MODE_STI,
	SAI_INTEGRATION_MODE_AC,
	SAI_INTEGRATION_MODE_NULL

} AnalysisIntegrationModeSpecifier;

typedef struct {

	ParameterSpecifier	parSpec;

	BOOLN	diagnosticModeFlag, integrationModeFlag, strobeSpecificationFlag;
	BOOLN	negativeWidthFlag, positiveWidthFlag, inputDecayRateFlag;
	BOOLN	imageDecayHalfLifeFlag;
	BOOLN	updateProcessVariablesFlag;
	int		diagnosticMode;
	int		integrationMode;
	char	strobeSpecification[MAX_FILE_PATH];
	double	negativeWidth;
	double	positiveWidth;
	double	inputDecayRate;
	double	imageDecayHalfLife;

	/* Private members */
	NameSpecifier	*diagnosticModeList;
	NameSpecifier	*integrationModeList;
	UniParListPtr	parList;
	char			diagnosticString[MAX_FILE_PATH];
	double			*inputDecay;
	FILE			*fp;
	Strobe			strobe;
	ChanLen			inputCount;
	ChanLen			zeroIndex;
	ChanLen			positiveWidthIndex;
	ChanLen			lastFrameLength;
	EarObjectPtr	strobeData;
	EarObjectPtr	dataBuffer;
	EarObjectPtr	strobeDataBuffer;

} SAImage, *SAImagePtr;

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

extern	SAImagePtr	sAImagePtr;

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

BOOLN	CheckData_Analysis_SAI(EarObjectPtr data);

BOOLN	CheckPars_Analysis_SAI(void);

void	DecayImage_Analysis_SAI(EarObjectPtr data);

BOOLN	Free_Analysis_SAI(void);

BOOLN	FreeProcessVariables_Analysis_SAI(void);

UniParListPtr	GetUniParListPtr_Analysis_SAI(void);

BOOLN	InitIntegrationModeList_Analysis_SAI(void);

BOOLN	Init_Analysis_SAI(ParameterSpecifier parSpec);

BOOLN	InitInputDecayArray_Analysis_SAI(EarObjectPtr data);

BOOLN	InitProcessVariables_Analysis_SAI(EarObjectPtr data);

void	OutputStrobeData_Analysis_SAI(void);

BOOLN	PrintPars_Analysis_SAI(void);

BOOLN	Process_Analysis_SAI(EarObjectPtr data);

void	ProcessFrameSection_Analysis_SAI(EarObjectPtr data,
		  ChanData **strobeStatePtrs, ChanData **dataPtrs, ChanLen strobeOffset,
		  ChanLen frameOffset, ChanLen sectionLength);

void	PushBufferData_Analysis_SAI(EarObjectPtr data, ChanLen frameLength);

BOOLN	ReadPars_Analysis_SAI(char *fileName);

BOOLN	SetDiagnosticMode_Analysis_SAI(char *theDiagnosticMode);

BOOLN	SetImageDecayHalfLife_Analysis_SAI(double theImageDecayHalfLife);

BOOLN	SetInputDecayRate_Analysis_SAI(double theInputDecayRate);

BOOLN	SetIntegrationMode_Analysis_SAI(char * theIntegrationMode);

BOOLN	SetDelay_Analysis_SAI(double theDelay);

BOOLN	SetDelayTimeout_Analysis_SAI(double theDelayTimeout);

BOOLN	SetNegativeWidth_Analysis_SAI(double theNegativeWidth);

BOOLN	SetPars_Analysis_SAI(char *diagnosticMode, char *integrationMode,
		  char *strobeSpecification, double negativeWidth, double
		  positiveWidth, double inputDecayRate, double imageDecayHalfLife);

BOOLN	SetPositiveWidth_Analysis_SAI(double thePositiveWidth);

BOOLN	SetStrobeSpecification_Analysis_SAI(char *theStrobeSpecification);

BOOLN	SetThresholdDecayRate_Analysis_SAI(double theThresholdDecayRate);

BOOLN	SetThreshold_Analysis_SAI(double theThreshold);

BOOLN	SetTypeMode_Analysis_SAI(char *theTypeMode);

BOOLN	SetUniParList_Analysis_SAI(void);

__END_DECLS

#endif
