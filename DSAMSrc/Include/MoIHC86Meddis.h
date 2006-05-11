/**********************
 *
 * File:		MoIHC86Meddis.h
 * Purpose:		This module contains the model Meddis 86 hair cell module.
 * Comments:	The parameters A and B have been changed so that the hair cell
 *				responds correctly for  Intensity levels in dB (SPL).
 * Author:		L. P. O'Mard
 * Created:		18 Feb 1993
 * Updated:		07 May 1998
 * Copyright:	(c) 1998, University of Essex
 *
 *********************/

#ifndef	_MOIHC86MEDDIS_H
#define _MOIHC86MEDDIS_H	1
 
/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

#define MEDDIS86_NUM_PARS			9
#define	MEDDIS86_MAX_DT	0.0001		/* Magic number for model */

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

typedef enum {

	MEDDIS86_PERM_CONST_A,
	MEDDIS86_PERM_CONST_B,
	MEDDIS86_RELEASE_RATE_G,
	MEDDIS86_REPLENISH_RATE_Y,
	MEDDIS86_LOSS_RATE_L,
	MEDDIS86_REPROCESS_RATE_X,
	MEDDIS86_RECOVERY_RATE_R,
	MEDDIS86_MAX_FREE_POOL_M,
	MEDDIS86_FIRING_RATE_H

} Meddis86ParSpecifier;

typedef struct {

		double	cleftC;			/* Quantity of transmitter in the cleft */
		double	reservoirQ;		/* Quantity of transmitter in the free pool */
		double	reprocessedW;	/* Quantity of transm. in the reproc. store */

} HairCellVars, *HairCellVarsPtr;
	
typedef struct {

	ParameterSpecifier parSpec;
	
	BOOLN	permConstAFlag, permConstBFlag, releaseRateFlag, replenishRateFlag;
	BOOLN	lossRateFlag, reprocessRateFlag, recoveryRateFlag, maxFreePoolFlag;
	BOOLN	firingRateFlag;
	BOOLN	updateProcessVariablesFlag;
	
	double	permConst_A;		/* Permeability constant (?). */
	double	permConst_B;		/* Units per second */
	double	releaseRate_g;		/* Release rate (units per second). */
	double	replenishRate_y;	/* Replenishment rate (units per second). */
	double	lossRate_l;			/* Loss rate (units per second). */
	double	reprocessRate_x;	/* Reprocessing rate (units per second). */
	double	recoveryRate_r;		/* Recovery rate (units per second). */
	double	maxFreePool_M;		/* Max. no. of transm. packets in free pool */
	double	firingRate_h;		/* Firing rate (spikes per second). */
	
	/* Private members */
	UniParListPtr	parList;
	HairCellVars	*hCChannels;
	double	dt, ymdt, xdt, ydt, l_Plus_rdt, rdt, gdt, hdt;
	
} HairCell, *HairCellPtr;

/******************************************************************************/
/*************************** External Variables *******************************/
/******************************************************************************/

extern	HairCellPtr		hairCellPtr;

/******************************************************************************/
/*************************** Function Prototypes ******************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

BOOLN	CheckData_IHC_Meddis86(EarObjectPtr data);

BOOLN	CheckPars_IHC_Meddis86(void);

BOOLN	Init_IHC_Meddis86(ParameterSpecifier parSpec);

BOOLN	InitProcessVariables_IHC_Meddis86(EarObjectPtr data);

BOOLN	Free_IHC_Meddis86(void);

void	FreeProcessVariables_IHC_Meddis86(void);

UniParListPtr	GetUniParListPtr_IHC_Meddis86(void);

BOOLN	PrintPars_IHC_Meddis86(void);

BOOLN	ReadPars_IHC_Meddis86(WChar *fileName);

BOOLN	RunModel_IHC_Meddis86(EarObjectPtr data);

BOOLN	SetFiringRate_IHC_Meddis86(double theSetFiringRate);

BOOLN	SetLossRate_IHC_Meddis86(double theLossRate);

BOOLN	SetMaxFreePool_IHC_Meddis86(double theMaxFreePool);

BOOLN	InitModule_IHC_Meddis86(ModulePtr theModule);

BOOLN	SetParsPointer_IHC_Meddis86(ModulePtr theModule);

BOOLN	SetPars_IHC_Meddis86(double A, double B, double g, double y, double l,
		  double x, double r, double M, double h);
		  
BOOLN	SetPermConstA_IHC_Meddis86(double thePermConstA);

BOOLN	SetPermConstB_IHC_Meddis86(double thePermConstB);

BOOLN	SetRecoveryRate_IHC_Meddis86(double theRecoveryRate);

BOOLN	SetReleaseRate_IHC_Meddis86(double theReleaseRate);

BOOLN	SetReplenishRate_IHC_Meddis86(double theReplenishRate);

BOOLN	SetReprocessRate_IHC_Meddis86(double theReprocessRate);

BOOLN	SetUniParList_IHC_Meddis86(void);

__END_DECLS

#endif
