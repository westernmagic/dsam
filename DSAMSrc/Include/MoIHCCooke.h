/*******
 *
 * File:		MoIHCCooke.h
 * Purpose: 	Header file for the Cooke 1991 hair cell.
 * Comments:	
 * Authors:		M. Cooke, L.P.O'Mard.
 * Created:		21 May 1995
 * Updated:	
 * Copyright:	(c) 1998, University of Essex.
 *
 ********/

#ifndef	_MOHCCOOKE_H
#define _MOHCCOOKE_H	1
 
/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

#define IHC_COOKE91_NUM_PARS			5

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

typedef enum {

	IHC_COOKE91_CRAWFORDCONST,
	IHC_COOKE91_RELEASEFRACTION,
	IHC_COOKE91_REFILLFRACTION,
	IHC_COOKE91_SPONTRATE,
	IHC_COOKE91_MAXSPIKERATE

} CookeHCParSpecifier;

typedef struct {

	double	vimm;
	double	vrel;
	double	crel;
	double	vres;
	double	cimm;

} CookeHCVars, *CookeHCVarsPtr;
	
typedef struct {

	ParameterSpecifier parSpec;
	
	BOOLN	crawfordConstFlag, releaseFractionFlag, refillFractionFlag;
	BOOLN	spontRateFlag, maxSpikeRateFlag;
	BOOLN	updateProcessVariablesFlag;

	double	crawfordConst;		/* Crawford and Fettiplace c Value */
	double	releaseFraction;	/* Release fraction. */
	double	refillFraction;		/* Replenishment fraction. */
	double	spontRate;			/* desired spontaneous firing rate. */
	double	maxSpikeRate;		/* maximum possible firing rate. */

	/* Private members */
	UniParListPtr	parList;
	double	vmin, k, l, rateScale;
	CookeHCVars	*hCChannels;

} CookeHC, *CookeHCPtr;

/******************************************************************************/
/*************************** External Variables *******************************/
/******************************************************************************/

extern	CookeHCPtr	cookeHCPtr;

/******************************************************************************/
/*************************** Function Prototypes ******************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

BOOLN	CheckData_IHC_Cooke91(EarObjectPtr data);

BOOLN	CheckPars_IHC_Cooke91(void);

BOOLN	Free_IHC_Cooke91(void);

void	FreeProcessVariables_IHC_Cooke91(void);

UniParListPtr	GetUniParListPtr_IHC_Cooke91(void);

BOOLN	Init_IHC_Cooke91(ParameterSpecifier parSpec);

BOOLN	InitProcessVariables_IHC_Cooke91(EarObjectPtr data);

BOOLN	PrintPars_IHC_Cooke91(void);

BOOLN	ReadPars_IHC_Cooke91(char *fileName);

BOOLN	RunModel_IHC_Cooke91(EarObjectPtr data);

BOOLN	SetCrawfordConst_IHC_Cooke91(double theCrawfordConst);

BOOLN	SetMaxSpikeRate_IHC_Cooke91(double theMaxSpikeRate);

BOOLN	InitModule_IHC_Cooke91(ModulePtr theModule);

BOOLN	SetParsPointer_IHC_Cooke91(ModulePtr theModule);

BOOLN	SetPars_IHC_Cooke91(double crawfordConst, double releaseFraction,
		  double refillFraction, double spontRate, double maxSpikeRate);

BOOLN	SetRefillFraction_IHC_Cooke91(double theRefillFraction);

BOOLN	SetReleaseFraction_IHC_Cooke91(double theReleaseFraction);

BOOLN	SetSpontRate_IHC_Cooke91(double theRecoveryRate);

BOOLN	SetUniParList_IHC_Cooke91(void);

__END_DECLS

#endif
