/**********************
 *
 * File:		AnALSR.h
 * Purpose:		This proces module calculates the average localised
 *				synchronised rate (ALSR).
 *				Yound E. D. and Sachs M. B. (1979) "Representation of steady-
 *				state vowels in the temporal aspects of the discharge patterns
 *				of populations of auditory-nerve fibers", J. Acoust. Soc. Am,
 *				Vol 66, pages 1381-1403.
 * Comments:	Written using ModuleProducer version 1.3.3 (Oct  1 2001).
 *				It expects multi-channel input from a period histogram
 * Author:		L. P. O'Mard
 * Created:		22 Jan 2002
 * Updated:		
 * Copyright:	(c) 2002, CNBH, University of Essex.
 *
 *********************/

#ifndef _ANALSR_H
#define _ANALSR_H 1

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define ANALYSIS_ALSR_NUM_PARS			3

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef enum {

	ANALYSIS_ALSR_LOWERAVELIMIT,
	ANALYSIS_ALSR_UPPERAVELIMIT,
	ANALYSIS_ALSR_NORMALISE

} ALSRParSpecifier;

typedef struct {

	ParameterSpecifier	parSpec;

	BOOLN	lowerAveLimitFlag, upperAveLimitFlag, normaliseFlag;
	BOOLN	updateProcessVariablesFlag;
	double	lowerAveLimit;
	double	upperAveLimit;
	double	normalise;

	/* Private members */
	UniParListPtr	parList;
	EarObjectPtr	modulusFT;

} ALSR, *ALSRPtr;

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

extern	ALSRPtr	aLSRPtr;

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
* macros, to allow the safe use of C libraries with C++ libraries - defined
* in GeCommon.h.
*/
__BEGIN_DECLS

BOOLN	Calc_Analysis_ALSR(EarObjectPtr data);

BOOLN	CheckData_Analysis_ALSR(EarObjectPtr data);

BOOLN	CheckPars_Analysis_ALSR(void);

BOOLN	FreeProcessVariables_Analysis_ALSR(void);

BOOLN	Free_Analysis_ALSR(void);

UniParListPtr	GetUniParListPtr_Analysis_ALSR(void);

BOOLN	InitModule_Analysis_ALSR(ModulePtr theModule);

BOOLN	InitProcessVariables_Analysis_ALSR(EarObjectPtr data);

BOOLN	Init_Analysis_ALSR(ParameterSpecifier parSpec);

BOOLN	PrintPars_Analysis_ALSR(void);

BOOLN	ReadPars_Analysis_ALSR(char *fileName);

BOOLN	SetLowerAveLimit_Analysis_ALSR(double theLowerAveLimit);

BOOLN	SetNormalise_Analysis_ALSR(double theNormalise);

BOOLN	SetParsPointer_Analysis_ALSR(ModulePtr theModule);

BOOLN	SetUniParList_Analysis_ALSR(void);

BOOLN	SetUpperAveLimit_Analysis_ALSR(double theUpperAveLimit);

__END_DECLS

#endif
