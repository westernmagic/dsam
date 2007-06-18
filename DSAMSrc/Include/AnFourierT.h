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
/*************************** Macro Definitions ********************************/
/******************************************************************************/

#if HAVE_FFTW3
#	define AN_FT_PTR_RE(Z)	(*(Z))[0]
#	define AN_FT_PTR_IM(Z)	(*(Z))[1]
#	define AN_FT_RE(Z)		(Z)[0]
#	define AN_FT_IM(Z)		(Z)[1]
#	define AN_FT_MODULUS(Z)	(sqrt(AN_FT_RE(Z) * AN_FT_RE(Z) + AN_FT_IM(Z) * \
			AN_FT_IM(Z)))
#	define AN_FT_MALLOC		fftw_malloc
#	define AN_FT_FREE		fftw_free
#else
#	define AN_FT_PTR_RE(Z)	(Z)->re
#	define AN_FT_PTR_IM(Z)	(Z)->im	
#	define AN_FT_RE(Z)		(Z).re
#	define AN_FT_IM(Z)		(Z).im
#	define AN_FT_MODULUS	MODULUS_CMPLX
#	define AN_FT_MALLOC		malloc
#	define AN_FT_FREE		free
#endif

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
	int		numThreads;
	int		numOutChans;
	double	dBSPLFactor;
	ChanLen	fTLength;
	ComplxPtr	*fT;
#	if HAVE_FFTW3
	fftw_plan	*plan;
	ComplxPtr	*fTOut;
#	endif

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

BOOLN	ReadPars_Analysis_FourierT(WChar *fileName);

void	ResetProcess_Analysis_FourierT(EarObjectPtr data);

BOOLN	InitModule_Analysis_FourierT(ModulePtr theModule);

BOOLN	SetOutputMode_Analysis_FourierT(WChar * theOutputMode);

BOOLN	SetParsPointer_Analysis_FourierT(ModulePtr theModule);

BOOLN	SetPars_Analysis_FourierT(WChar * outputMode);

BOOLN	SetUniParList_Analysis_FourierT(void);

__END_DECLS

#endif
