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

#define BM_GAMMT_NUM_PARS			2

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

typedef enum {

	BM_GAMMT_ORDER,
	BM_GAMMAT_THE_CFS

} GammaTParSpecifier;

typedef struct {

	ParameterSpecifier parSpec;

	BOOLN		orderFlag;
	BOOLN		updateProcessVariablesFlag;
	int			order;		/* Defines the Q value of the gamma tone filters.*/
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

BOOLN	CheckPars_BasilarM_GammaT(void);

CFListPtr	GetCFListPtr_BasilarM_GammaT(void);

BOOLN	Init_BasilarM_GammaT(ParameterSpecifier parSpec);

BOOLN	InitProcessVariables_BasilarM_GammaT(EarObjectPtr data);

BOOLN	Free_BasilarM_GammaT(void);

void	FreeProcessVariables_BasilarM_GammaT(void);

UniParListPtr	GetUniParListPtr_BasilarM_GammaT(void);

BOOLN	PrintPars_BasilarM_GammaT(void);

BOOLN	ReadPars_BasilarM_GammaT(char *fileName);

BOOLN	RunModel_BasilarM_GammaT(EarObjectPtr data);

BOOLN	SetBandwidths_BasilarM_GammaT(char *theBandwidthMode,
		  double *theBandwidths);

BOOLN SetCFList_BasilarM_GammaT(CFListPtr theCFList);

BOOLN	SetOrder_BasilarM_GammaT(int theOrder);

BOOLN	SetPars_BasilarM_GammaT(int theOrder, CFListPtr theCFs);

BOOLN	SetUniParList_BasilarM_GammaT(void);

__END_DECLS

#endif
