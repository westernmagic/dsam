/**********************
 *
 * File:		MoBMCarney.h
 * Purpose:		Laurel H. Carney basilar membrane module: Carney L. H. (1993)
 *				"A model for the responses of low-frequency auditory-nerve
 *				fibers in cat", JASA, 93, pp 401-417.
 * Comments:	Written using ModuleProducer version 1.9 (Feb 29 1996).
 *				21-03-97 LPO: Corrected binaural processing bug.
 * Authors:		L. P. O'Mard modified from L. H. Carney's code
 * Created:		12 Mar 1996
 * Updated:		21 Mar 1997
 * Copyright:	(c) 1997, University of Essex.
 *
 *********************/

#ifndef _MOBMCARNEY_H
#define _MOBMCARNEY_H 1

#include "UtCmplxM.h"
#include "UtBandwidth.h"
#include "UtCFList.h"

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define BASILARM_CARNEY_NUM_PARS			6

/*
 * Use original fit for Tl (latency vs. CF in msec) from Carney & Yin '88 
 *and then correct by .75 cyles to go from PEAK delay to ONSET delay
 */
 
#define	BM_CARNEY_SS0	6.0e-3		/* mm */
#define	BM_CARNEY_CC0	1.1e-3		/* ms */
#define	BM_CARNEY_SS1	2.2e-3		/* mm */
#define	BM_CARNEY_CC1	1.1e-3		/* ms */

/******************************************************************************/
/****************************** Macro definitions *****************************/
/******************************************************************************/

#define	BM_CARNEY_A(C, TAU0)	(1.0 / (1.0 + (C) * ((TAU0) * 3.0 / 2.0)))

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef enum {

	BASILARM_CARNEY_CASCADE,
	BASILARM_CARNEY_CUTOFFFREQUENCY,
	BASILARM_CARNEY_HCOPERATINGPOINT,
	BASILARM_CARNEY_ASYMMETRICALBIAS,
	BASILARM_CARNEY_MAXHCVOLTAGE,
	BASILARM_CARNEY_CFLIST

} BMCarneyParSpecifier;

typedef struct {

	double	x;
	double	tau0;
	Complex	*fLast;
	double	oHCLast;
	double	oHCTempLast;

} CarneyGTCoeffs, *CarneyGTCoeffsPtr;

typedef struct {

	ParameterSpecifier	parSpec;

	BOOLN	cascadeFlag, cutOffFrequencyFlag, hCOperatingPointFlag;
	BOOLN	asymmetricalBiasFlag, maxHCVoltageFlag;
	BOOLN	updateProcessVariablesFlag;

	int		cascade;
	double	cutOffFrequency;
	double	hCOperatingPoint;
	double	asymmetricalBias;
	double	maxHCVoltage;
	CFListPtr	cFList;

	/* Private members */
	UniParListPtr	parList;
	int		numChannels, numComplexCoeffs, numThreads;
	double	c, aA, c1LP, c2LP, pix2xDt;
	ComplexPtr *f;
	CarneyGTCoeffsPtr	*coefficients;

} BMCarney, *BMCarneyPtr;

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

extern	BMCarneyPtr	bMCarneyPtr;

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

BOOLN	CheckData_BasilarM_Carney(EarObjectPtr data);

BOOLN	CheckPars_BasilarM_Carney(void);

BOOLN	Free_BasilarM_Carney(void);

void	FreeCarneyGTCoeffs_BasilarM_Carney(CarneyGTCoeffsPtr *p);

BOOLN	FreeProcessVariables_BasilarM_Carney(void);

CFListPtr	GetCFListPtr_BasilarM_Carney(void);

UniParListPtr	GetUniParListPtr_BasilarM_Carney(void);

BOOLN	Init_BasilarM_Carney(ParameterSpecifier parSpec);

CarneyGTCoeffsPtr	InitCarneyGTCoeffs_BasilarM_Carney(int cascade, double cF);

BOOLN	InitProcessVariables_BasilarM_Carney(EarObjectPtr data);

BOOLN	PrintPars_BasilarM_Carney(void);

BOOLN	ReadPars_BasilarM_Carney(char *fileName);

BOOLN	RunModel_BasilarM_Carney(EarObjectPtr data);

BOOLN	SetAsymmetricalBias_BasilarM_Carney(double theAsymmetricalBias);

BOOLN	SetCFList_BasilarM_Carney(CFListPtr theCFList);

BOOLN	SetCutOffFrequency_BasilarM_Carney(double theCutOffFrequency);

BOOLN	SetMaxHCVoltage_BasilarM_Carney(double theMaxHCVoltage);

BOOLN	SetHCOperatingPoint_BasilarM_Carney(double theHCOperatingPoint);

BOOLN	SetCascade_BasilarM_Carney(int theCascade);

BOOLN	InitModule_BasilarM_Carney(ModulePtr theModule);

BOOLN	SetParsPointer_BasilarM_Carney(ModulePtr theModule);

BOOLN	SetPars_BasilarM_Carney(int cascade, double cutOffFrequency,
		  double hCOperatingPoint, double asymmetricalBias,
		  double maxHCVoltage, CFListPtr cFList);

BOOLN	SetUniParList_BasilarM_Carney(void);

__END_DECLS

#endif
