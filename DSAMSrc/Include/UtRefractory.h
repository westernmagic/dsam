/**********************
 *
 * File:		UtRefractory.h
 * Purpose:		Applies a refractory effect adjustment for spike probabilities.
 *				R. Meddis and M. J. Hewitt (1991)"Virtual pitch and phase
 *				sensitivity of a computer modle of the auditory periphery. 1:
 *				pitch identification" Journal of the Acoustical Society of
 *				America, 89, pp 2866-2882.
 * Comments:	Written using ModuleProducer version 1.9 (May 27 1996).
 * Author:		L. P. O'Mard, revised from code by M. J. Hewitt
 * Created:		26 Jun 1996
 * Updated:	
 * Copyright:	(c) Loughborough University
 *
 *********************/

#ifndef _UTREFRACTORY_H
#define _UTREFRACTORY_H 1

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define UTILITY_REFRACTORYADJUST_NUM_PARS			1

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef enum {

	UTILITY_REFRACTORYADJUST_REFRACTORYPERIOD

} RefractAdjParSpecifier;

typedef struct {

	ParameterSpecifier	parSpec;

	BOOLN	refractoryPeriodFlag;
	BOOLN	updateProcessVariablesFlag;
	double	refractoryPeriod;

	/* Private members */
	UniParListPtr	parList;
	int		numChannels;
	double	**lastOutput;

} RefractAdj, *RefractAdjPtr;

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

extern	RefractAdjPtr	refractAdjPtr;

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

BOOLN	CheckData_Utility_RefractoryAdjust(EarObjectPtr data);

BOOLN	CheckPars_Utility_RefractoryAdjust(void);

BOOLN	Free_Utility_RefractoryAdjust(void);

void	FreeProcessVariables_Utility_RefractoryAdjust(void);

UniParListPtr	GetUniParListPtr_Utility_RefractoryAdjust(void);

BOOLN	Init_Utility_RefractoryAdjust(ParameterSpecifier parSpec);

BOOLN	InitProcessVariables_Utility_RefractoryAdjust(EarObjectPtr data);

BOOLN	PrintPars_Utility_RefractoryAdjust(void);

BOOLN	Process_Utility_RefractoryAdjust(EarObjectPtr data);

BOOLN	ReadPars_Utility_RefractoryAdjust(char *fileName);

BOOLN	SetPars_Utility_RefractoryAdjust(double refractoryPeriod);

BOOLN	SetRefractoryPeriod_Utility_RefractoryAdjust(
		  double theRefractoryPeriod);

BOOLN	SetUniParList_Utility_RefractoryAdjust(void);

__END_DECLS

#endif
