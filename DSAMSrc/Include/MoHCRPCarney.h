/**********************
 *
 * File:		MoHCRPCarney.h
 * Purpose:		Laurel H. Carney IHC receptor potential module: Carney L. H.
 *				(1993) "A model for the responses of low-frequency
 *				auditory-nerve fibers in cat", JASA, 93, pp 401-417.
 * Comments:	Written using ModuleProducer version 1.9 (Feb 29 1996).
 *				It expects to be used in conjuction with a BM module.
 * Authors:		L. P. O'Mard modified from L. H. Carney's code
 * Created:		15 Mar 1996
 *
 *********************/

#ifndef _MOHCRPCARNEY_H
#define _MOHCRPCARNEY_H 1

#include "UtBandwidth.h"

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define IHCRP_CARNEY_NUM_PARS			7

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef enum {

	IHCRP_CARNEY_CUTOFFFREQUENCY,
	IHCRP_CARNEY_HCOPERATINGPOINT,
	IHCRP_CARNEY_ASYMMETRICALBIAS,
	IHCRP_CARNEY_MAXHCVOLTAGE,
	IHCRP_CARNEY_WAVEDELAYCOEFF,
	IHCRP_CARNEY_WAVEDELAYLENGTH,
	IHCRP_CARNEY_REFERENCEPOT

} CarneyRPParSpecifier;

typedef struct {

	double	x;
	ChanLen	numLastSamples;
	double	waveLast;
	double	waveTempLast;
	double	iHCLast;
	double	iHCTempLast;
	ChanData	*lastOutputSection, *lastOutputStore;

} CarneyRPCoeffs, *CarneyRPCoeffsPtr;

typedef struct {

	ParameterSpecifier	parSpec;

	BOOLN	cutOffFrequencyFlag, hCOperatingPointFlag, asymmetricalBiasFlag;
	BOOLN	maxHCVoltageFlag, waveDelayCoeffFlag, waveDelayLengthFlag;
	BOOLN	referencePotFlag;
	BOOLN	updateProcessVariablesFlag;

	double	cutOffFrequency;
	double	hCOperatingPoint;
	double	asymmetricalBias;
	double	maxHCVoltage;
	double	waveDelayCoeff;
	double	waveDelayLength;
	double	referencePot;		/* Reference potential */

	/* Private members */
	UniParListPtr	parList;
	int		numChannels;
	CarneyRPCoeffsPtr	*coefficients;

} CarneyRP, *CarneyRPPtr;

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

extern	CarneyRPPtr	carneyRPPtr;

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

BOOLN	CheckData_IHCRP_Carney(EarObjectPtr data);

BOOLN	CheckPars_IHCRP_Carney(void);

BOOLN	Free_IHCRP_Carney(void);

void	FreeCarneyRPCoeffs_IHCRP_Carney(CarneyRPCoeffsPtr *p);

BOOLN	FreeProcessVariables_IHCRP_Carney(void);

UniParListPtr	GetUniParListPtr_IHCRP_Carney(void);

BOOLN	Init_IHCRP_Carney(ParameterSpecifier parSpec);

CarneyRPCoeffsPtr	InitCarneyRPCoeffs_IHCRP_Carney(double cF, double dt);

BOOLN	InitProcessVariables_IHCRP_Carney(EarObjectPtr data);

BOOLN	PrintPars_IHCRP_Carney(void);

BOOLN	ReadPars_IHCRP_Carney(char *fileName);

BOOLN	RunModel_IHCRP_Carney(EarObjectPtr data);

BOOLN	SetAsymmetricalBias_IHCRP_Carney(double theAsymmetricalBias);

BOOLN	SetCutOffFrequency_IHCRP_Carney(double theCutOffFrequency);

BOOLN	SetHCOperatingPoint_IHCRP_Carney(double theHCOperatingPoint);

BOOLN	SetMaxHCVoltage_IHCRP_Carney(double theMaxHCVoltage);

BOOLN	InitModule_IHCRP_Carney(ModulePtr theModule);

BOOLN	SetParsPointer_IHCRP_Carney(ModulePtr theModule);

BOOLN	SetPars_IHCRP_Carney(double cutOffFrequency, double HCOperatingPoint,
		  double asymmetricalBias, double maxHCVoltage, double referencePot,
		  double waveDelayCoeff, double waveDelayLength);

BOOLN	SetReferencePot_IHCRP_Carney(double theReferencePot);

BOOLN	SetUniParList_IHCRP_Carney(void);

BOOLN	SetWaveDelayCoeff_IHCRP_Carney(double theWaveDelayCoeff);

BOOLN	SetWaveDelayLength_IHCRP_Carney(double theWaveDelayLength);

__END_DECLS

#endif
