/**********************
 *
 * File:		FlFIR.h
 * Purpose:		This routine uses the Remez exchange algorithm to calculate a
 *				Finite Impulse Response (FIR) filter which is then run as a
 *				process.
 * Comments:	Written using ModuleProducer version 1.2.9 (Aug 10 2000).
 * Author:		L. P. O'Mard.
 * Created:		04 Dec 2000
 * Updated:	
 * Copyright:	(c) 2000, CNBH University of Essex
 *
 *********************/

#ifndef _FLFIR_H
#define _FLFIR_H 1

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define FILTER_FIR_NUM_PARS			6

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef enum {

	FILTER_FIR_TYPE,
	FILTER_FIR_NUMTAPS,
	FILTER_FIR_NUMBANDS,
	FILTER_FIR_BANDFREQS,
	FILTER_FIR_DESIRED,
	FILTER_FIR_WEIGHTS,

} FIRParSpecifier;

typedef enum {

	FILTER_FIR_ARBITRARY_TYPE,
	FILTER_FIR_BANDPASS_TYPE,
	FILTER_FIR_DIFFERENTIATOR_TYPE,
	FILTER_FIR_HILBERT_TYPE,
	FILTER_FIR_USER_TYPE,
	FILTER_FIR_TYPE_NULL

} FilterTypeSpecifier;

typedef struct {

	ParameterSpecifier	parSpec;

	BOOLN	typeFlag, numTapsFlag, numBandsFlag;
	BOOLN	updateProcessVariablesFlag;
	int		type;
	int		numTaps;
	int		numBands;
	double	*bandFreqs, *desired, *weights;

	/* Private members */
	NameSpecifier	*typeList;
	UniParListPtr	parList;

	FIRCoeffsPtr	coeffs;

} FIR, *FIRPtr;

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

extern	FIRPtr	fIRPtr;

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
* macros, to allow the safe use of C libraries with C++ libraries - defined
* in GeCommon.h.
*/
__BEGIN_DECLS

BOOLN	AllocNumBands_Filter_FIR(int numBands);

BOOLN	CheckData_Filter_FIR(EarObjectPtr data);

BOOLN	CheckPars_Filter_FIR(void);

void	FreeProcessVariables_Filter_FIR(void);

BOOLN	Free_Filter_FIR(void);

int		GetType_Filter_FIR(void);

UniParListPtr	GetUniParListPtr_Filter_FIR(void);

BOOLN	InitProcessVariables_Filter_FIR(EarObjectPtr data);

BOOLN	InitTypeList_Filter_FIR(void);

BOOLN	Init_Filter_FIR(ParameterSpecifier parSpec);

BOOLN	PrintPars_Filter_FIR(void);

BOOLN	ReadPars_Filter_FIR(char *fileName);

BOOLN	RunProcess_Filter_FIR(EarObjectPtr data);

BOOLN	SetBandFreqs_Filter_FIR(double *theBandFreqs);

BOOLN	SetDesired_Filter_FIR(double *theDesired);

BOOLN	SetIndividualBand_Filter_FIR(int theIndex, double theBand);

BOOLN	SetIndividualDesired_Filter_FIR(int theIndex, double theDesired);

BOOLN	SetIndividualWeight_Filter_FIR(int theIndex, double theWeight);

BOOLN	InitModule_Filter_FIR(ModulePtr theModule);

BOOLN	SetNumBands_Filter_FIR(int theNumBands);

BOOLN	SetNumTaps_Filter_FIR(int theNumTaps);

BOOLN	SetParsPointer_Filter_FIR(ModulePtr theModule);

BOOLN	SetPars_Filter_FIR(char * type, int numTaps, int numBands,
		  double *bandFreqs, double *desired, double *weights);

BOOLN	SetType_Filter_FIR(char * theType);

BOOLN	SetUniParList_Filter_FIR(void);

BOOLN	SetWeights_Filter_FIR(double *theWeights);

__END_DECLS

#endif
