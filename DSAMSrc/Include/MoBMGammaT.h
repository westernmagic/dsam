/**********************
 *
 * File:		MoBMGammaT.h
 * Purpose:		This is an implementation of a Gamma Tone filter using the
 *				UtFilters.c module.
 * Comments:	The Gamma Tone filter is used to model the response of the 
 *				Basilar Membrane.
 * Author:		L. P. O'Mard
 * Created:		12 Jul 1993
 * Updated:		
 * Copyright:	(c) 1997,  University of Essex
 *
 **********************/

#ifndef	_MOBMGAMMAT_H
#define _MOBMGAMMAT_H	1

/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

#define BM_GAMMAT_MOD_NAME			wxT("BM_GAMMAT")
#define BM_GAMMAT_NUM_PARS			3

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

typedef enum {

	BASILARM_GAMMAT_INTERLEAVEMODE,
	BM_GAMMAT_CASCADE,
	BM_GAMMAT_THE_CFS

} GammaTParSpecifier;

typedef struct {

	ParameterSpecifier parSpec;

	BOOLN		cascadeFlag;
	BOOLN		updateProcessVariablesFlag;
	int			interleaveMode;
	int			cascade;	/* Defines the Q value of the gamma tone filters.*/
	CFListPtr	theCFs;		/* Pointer to centre frequency structure. */
	
	/* Private members */
	UniParListPtr	parList;
	int			numChannels;
    GammaToneCoeffsPtr	*coefficients;

} BMGammaT, *BMGammaTPtr;

/******************************************************************************/
/*************************** External Variables *******************************/
/******************************************************************************/

extern	BMGammaTPtr	bMGammaTPtr;

/******************************************************************************/
/*************************** Function Prototypes ******************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

BOOLN	CheckData_BasilarM_GammaT(EarObjectPtr data);

CFListPtr	GetCFListPtr_BasilarM_GammaT(void);

BOOLN	Free_BasilarM_GammaT(void);

void	FreeProcessVariables_BasilarM_GammaT(void);

UniParListPtr	GetUniParListPtr_BasilarM_GammaT(void);

BOOLN	Init_BasilarM_GammaT(ParameterSpecifier parSpec);

BOOLN	InitProcessVariables_BasilarM_GammaT(EarObjectPtr data);

BOOLN	PrintPars_BasilarM_GammaT(void);

BOOLN	ReadPars_BasilarM_GammaT(WChar *fileName);

BOOLN	RunModel_BasilarM_GammaT(EarObjectPtr data);

BOOLN	SetBandwidths_BasilarM_GammaT(WChar *theBandwidthMode,
		  double *theBandwidths);

BOOLN SetCFList_BasilarM_GammaT(CFListPtr theCFList);

BOOLN	SetCascade_BasilarM_GammaT(int theCascade);

BOOLN	InitModule_BasilarM_GammaT(ModulePtr theModule);

BOOLN	SetInterleaveMode_BasilarM_GammaT(WChar * theInterleaveMode);

BOOLN	SetParsPointer_BasilarM_GammaT(ModulePtr theModule);

BOOLN	SetPars_BasilarM_GammaT(int theCascade, CFListPtr theCFs);

BOOLN	SetUniParList_BasilarM_GammaT(void);

__END_DECLS

#endif
