/**********************
 *
 * File:		FlMultiBPass.h
 * Purpose:		This is an implementation of an outer-/middle-ear pre-emphasis
 *				filter using the filters.c module.
 * Comments:	Written using ModuleProducer version 1.2.2 (Nov 11 1998).
 *				A parallel, multiple band-pass filter is used to model the
 *				effects of the outer- and middle-ear on an input signal.
 *				The first data EarObject in the BPass is the process EarObject.
 *				12-11-98 LPO: I have changed this module to use arrays rather
 *				than a data structure.  This change makes it easier to do the
 *				universal parameter implementation.
 * Author:		L. P. O'Mard
 * Created:		20 Jul 1998
 * Updated:		12 Nov 1998
 * Copyright:	(c) 1998, University of Essex.
 *
 *********************/

#ifndef _FLBANDPASS_H
#define _FLBANDPASS_H 1

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define MULTIBPASS_NUM_PARS			5

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef enum {

	MULTIBPASS_NUMFILTERS,
	MULTIBPASS_ORDER,
	MULTIBPASS_PREATTENUATION,
	MULTIBPASS_LOWERCUTOFFFREQ,
	MULTIBPASS_UPPERCUFOFFFREQ

} MultiBPassFParSpecifier;

typedef struct {

	BandPassCoeffsPtr	*coefficients;
	EarObjectPtr		data;

} BPassPars, *BPassParsPtr;
	
typedef struct {

	ParameterSpecifier	parSpec;

	BOOLN	updateProcessVariablesFlag;
	BOOLN	numFiltersFlag;
	int		numFilters;
	int		*order;
	double	*preAttenuation;
	double	*upperCutOffFreq;
	double	*lowerCutOffFreq;

	/* Private members */
	UniParListPtr	parList;
	int		numChannels;
	BPassPars	*bPassPars;

} MultiBPassF, *MultiBPassFPtr;

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

extern	MultiBPassFPtr	multiBPassFPtr;

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

BOOLN	CheckData_Filter_MultiBPass(EarObjectPtr data);

BOOLN	CheckPars_Filter_MultiBPass(void);

BOOLN	Free_Filter_MultiBPass(void);

void	FreeProcessVariables_Filter_MultiBPass(void);

UniParListPtr	GetUniParListPtr_Filter_MultiBPass(void);

BOOLN	Init_Filter_MultiBPass(ParameterSpecifier parSpec);

BOOLN	InitProcessVariables_Filter_MultiBPass(EarObjectPtr data);

BOOLN	PrintPars_Filter_MultiBPass(void);

BOOLN	ReadPars_Filter_MultiBPass(char *fileName);

BOOLN	RunModel_Filter_MultiBPass(EarObjectPtr data);

BOOLN	SetIndividualLowerCutOffFreq_Filter_MultiBPass(int theIndex,
		  double theLowerCutOffFreq);

BOOLN	SetIndividualOrder_Filter_MultiBPass(int theIndex, int theOrder);

BOOLN	SetIndividualPreAttenuation_Filter_MultiBPass(int theIndex,
		  double thePreAttenuation);

BOOLN	SetIndividualUpperCutOffFreq_Filter_MultiBPass(int theIndex,
		  double theUpperCutOffFreq);

BOOLN	SetLowerCutOffFreq_Filter_MultiBPass(double *theLowerCutOffFreq);

BOOLN	SetNumFilters_Filter_MultiBPass(int theNumFilters);

BOOLN	SetOrder_Filter_MultiBPass(int *theOrder);

BOOLN	SetPars_Filter_MultiBPass(int numFilters, int *order,
		  double *preAttenuation, double *upperCutOffFreq,
		  double *lowerCutOffFreq);

BOOLN	SetPreAttenuation_Filter_MultiBPass(double *thePreAttenuation);

BOOLN	SetUniParList_Filter_MultiBPass(void);

BOOLN	SetUpperCutOffFreq_Filter_MultiBPass(double *theUpperCutOffFreq);

__END_DECLS

#endif