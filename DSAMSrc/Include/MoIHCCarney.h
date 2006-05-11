/**********************
 *
 * File:		MoIHCCarney.h
 * Purpose:		Laurel H. Carney IHC synapse module: Carney L. H. (1993)
 *				"A model for the responses of low-frequency auditory-nerve
 *				fibers in cat", JASA, 93, pp 401-417.
 * Comments:	Written using ModuleProducer version 1.9 (Feb 29 1996).
 * Authors:		L. P. O'Mard modified from L. H. Carney's code
 * Created:		30 April 1996
 * Updated:	
 * Copyright:	(c) 1998, University of Essex.
 *
 *********************/

#ifndef _MOIHCCARNEY_H
#define _MOIHCCARNEY_H 1

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define IHC_CARNEY_NUM_PARS						10
#define	CARNEY_IHC_RESTING_LOCAL_CONC_FACTOR	2
#define	CARNEY_IHC_RESTING_GLOBAL_CONC_FACTOR	3

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef enum {

	IHC_CARNEY_MAXHCVOLTAGE,
	IHC_CARNEY_RESTINGRELEASERATE,
	IHC_CARNEY_RESTINGPERM,
	IHC_CARNEY_MAXGLOBALPERM,
	IHC_CARNEY_MAXLOCALPERM,
	IHC_CARNEY_MAXIMMEDIATEPERM,
	IHC_CARNEY_MAXLOCALVOLUME,
	IHC_CARNEY_MINLOCALVOLUME,
	IHC_CARNEY_MAXIMMEDIATEVOLUME,
	IHC_CARNEY_MINIMMEDIATEVOLUME

} CarneyHCParSpecifier;

typedef struct {

	double	vI;
	double	vL;
	double	cI;
	double	cL;

} CarneyHCVars, *CarneyHCVarsPtr;

typedef struct {

	ParameterSpecifier	parSpec;

	BOOLN	maxHCVoltageFlag, restingReleaseRateFlag, restingPermFlag;
	BOOLN	maxGlobalPermFlag, maxLocalPermFlag, maxImmediatePermFlag;
	BOOLN	maxLocalVolumeFlag, minLocalVolumeFlag, maxImmediateVolumeFlag;
	BOOLN	minImmediateVolumeFlag;
	BOOLN	updateProcessVariablesFlag;

	double	maxHCVoltage;		/* Vmax */
	double	restingReleaseRate;	/* R0 */
	double	restingPerm;		/* Prest */
	double	maxGlobalPerm;		/* PGmax */
	double	maxLocalPerm;		/* PLmax */
	double	maxImmediatePerm;	/* PImax */
	double	maxLocalVolume;		/* VLmax */
	double	minLocalVolume;		/* VLmin */
	double	maxImmediateVolume;	/* VImax */
	double	minImmediateVolume;	/* VImin */

	/* Private members */
	UniParListPtr	parList;
	double	dt, cG, pIMaxMinusPrest, pLMaxMinusPrest, pGMaxMinusPrest;
	CarneyHCVars	*hCChannels;

} CarneyHC, *CarneyHCPtr;

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

extern	CarneyHCPtr	carneyHCPtr;

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

BOOLN	CheckData_IHC_Carney(EarObjectPtr data);

BOOLN	CheckPars_IHC_Carney(void);

BOOLN	Free_IHC_Carney(void);

void	FreeProcessVariables_IHC_Carney(void);

UniParListPtr	GetUniParListPtr_IHC_Carney(void);

BOOLN	Init_IHC_Carney(ParameterSpecifier parSpec);

BOOLN	InitProcessVariables_IHC_Carney(EarObjectPtr data);

BOOLN	PrintPars_IHC_Carney(void);

BOOLN	ReadPars_IHC_Carney(WChar *fileName);

BOOLN	RunModel_IHC_Carney(EarObjectPtr data);

BOOLN	SetMaxGlobalPerm_IHC_Carney(double theMaxGlobalPerm);

BOOLN	SetMaxHCVoltage_IHC_Carney(double theMaxHCVoltage);

BOOLN	SetMaxImmediatePerm_IHC_Carney(double theMaxImmediatePerm);

BOOLN	SetMaxImmediateVolume_IHC_Carney(double theMaxImmediateVolume);

BOOLN	SetMaxLocalPerm_IHC_Carney(double theMaxLocalPerm);

BOOLN	SetMaxLocalVolume_IHC_Carney(double theMaxLocalVolume);

BOOLN	SetMinImmediateVolume_IHC_Carney(double theMinImmediateVolume);

BOOLN	SetMinLocalVolume_IHC_Carney(double theMinLocalVolume);

BOOLN	InitModule_IHC_Carney(ModulePtr theModule);

BOOLN	SetParsPointer_IHC_Carney(ModulePtr theModule);

BOOLN	SetPars_IHC_Carney(double maxHCVoltage, double restingReleaseRate,
		  double restingPerm, double maxGlobalPerm, double maxLocalPerm,
		  double maxImmediatePerm, double maxLocalVolume,
		  double minLocalVolume, double maxImmediateVolume,
		  double minImmediateVolume);

BOOLN	SetRestingPerm_IHC_Carney(double theRestingPerm);

BOOLN	SetRestingReleaseRate_IHC_Carney(double theRestingReleaseRate);

BOOLN	SetUniParList_IHC_Carney(void);

__END_DECLS

#endif
