/**********************
 *
 * File:		AnFourierT.h
 * Purpose:		This module runs a basic Fourier Analysis.
 * Comments:	
 * Author:		L. P. O'Mard
 * Created:		18-01-94
 * Updated:		
 * Copyright:	(c) 2000, University of Essex
 *
 **********************/

#ifndef	_ANFOURIERT_H
#define _ANFOURIERT_H	1

#include "UtCmplxM.h"
#include "UtNameSpecs.h"

/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

#define ANALYSIS_FOURIERT_NUM_PARS			1

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef enum {

	ANALYSIS_FOURIERT_OUTPUTMODE

} FourierTParSpecifier;

typedef enum {

	ANALYSIS_FOURIERT_MODULUS_OUTPUTMODE,
	ANALYSIS_FOURIERT_PHASE_OUTPUTMODE,
	ANALYSIS_FOURIERT_COMPLEX_OUTPUTMODE,
	ANALYSIS_FOURIERT_DB_SPL_OUTPUTMODE,
	ANALYSIS_FOURIERT_OUTPUTMODE_NULL

} AnalysisOutputModeSpecifier;

typedef struct {

	ParameterSpecifier	parSpec;
	BOOLN	updateProcessVariablesFlag;

	BOOLN	outputModeFlag;
	int		outputMode;

	/* Private members */
	NameSpecifier	*outputModeList;
	UniParListPtr	parList;
	ChanLen	fTLength;
	Complex	*fT;

} FourierT, *FourierTPtr;

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

extern	FourierTPtr	fourierTPtr;

/******************************************************************************/
/*************************** Function Prototypes ******************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

BOOLN	Calc_Analysis_FourierT(EarObjectPtr data);

BOOLN	CheckData_Analysis_FourierT(EarObjectPtr data);

BOOLN	CheckPars_Analysis_FourierT(void);

BOOLN	Free_Analysis_FourierT(void);

void	FreeProcessVariables_Analysis_FourierT(void);

UniParListPtr	GetUniParListPtr_Analysis_FourierT(void);

BOOLN	InitOutputModeList_Analysis_FourierT(void);

BOOLN	Init_Analysis_FourierT(ParameterSpecifier parSpec);

BOOLN	InitProcessVariables_Analysis_FourierT(EarObjectPtr data);

BOOLN	PrintPars_Analysis_FourierT(void);

BOOLN	ReadPars_Analysis_FourierT(char *fileName);

BOOLN	InitModule_Analysis_FourierT(ModulePtr theModule);

BOOLN	SetOutputMode_Analysis_FourierT(char * theOutputMode);

BOOLN	SetParsPointer_Analysis_FourierT(ModulePtr theModule);

BOOLN	SetPars_Analysis_FourierT(char * outputMode);

BOOLN	SetUniParList_Analysis_FourierT(void);

__END_DECLS

#endif
