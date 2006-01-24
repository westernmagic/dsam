/**********************
 *
 * File:		MoIHCMeddis2000.h
 * Purpose:		This module contains the routines for synaptic adaptation of
 *				inner hair cells. It is a stochastic model, and with the
 *				exception of a probabilistic output option, is the same as the
 *				Meddis 95 module, but without the transmitter release stage.
 *				It is intended to be used with the Calcium transmitter release
 *				stage.
 * Comments:	Written using ModuleProducer version 1.2.5 (May  7 1999).
 * Authors:		L.P. O'Mard, revised Chris Sumner (7th May 2000).
 * Created:		16 Mar 2000
 * Updated:	
 * Copyright:	(c) 2000, CNBH University of Essex.
 *
 *********************/

#ifndef _MOIHCMEDDIS2000_H
#define _MOIHCMEDDIS2000_H 1

#include "UtNameSpecs.h"

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define IHC_MEDDIS2000_NUM_PARS		19
#define MEDDIS2000_MAX_DT			0.00005		/* Magic number for model */
#define MEDDIS2000_MOD_NAME			"IHC_MEDDIS2000"

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef enum {

	IHC_MEDDIS2000_OPMODE,
	IHC_MEDDIS2000_DIAGMODE,
	IHC_MEDDIS2000_CACONDMODE,
	IHC_MEDDIS2000_CLEFTREPLENISHMODE,
	IHC_MEDDIS2000_RANSEED,
	IHC_MEDDIS2000_CAVREV,
	IHC_MEDDIS2000_BETACA,
	IHC_MEDDIS2000_GAMMACA,
	IHC_MEDDIS2000_PCA,
	IHC_MEDDIS2000_GCAMAX,
	IHC_MEDDIS2000_PERM_CA0,
	IHC_MEDDIS2000_PERM_Z,
	IHC_MEDDIS2000_TAUCACHAN,
	IHC_MEDDIS2000_TAUCONCCA,
	IHC_MEDDIS2000_MAXFREEPOOL_M,
	IHC_MEDDIS2000_REPLENISHRATE_Y,
	IHC_MEDDIS2000_LOSSRATE_L,
	IHC_MEDDIS2000_REPROCESSRATE_X,
	IHC_MEDDIS2000_RECOVERYRATE_R

} HairCell2ParSpecifier;

typedef struct {

	double 	actCa;		/* Activation of Calcium channels */
	double 	concCa;		/* Concentration of Calcium at the synapse */
	double	reservoirQ;		/* Quantity of transmitter in the free pool */
	double	cleftC;		/* Quantity of transmitter in the cleft */
	double	reprocessedW;	/* Quantity of transm. in the reproc. store */

} HairCellVars2, *HairCellVars2Ptr;


typedef enum {

	IHC_MEDDIS2000_OPMODE_SPIKE,
	IHC_MEDDIS2000_OPMODE_PROB,
	IHC_MEDDIS2000_OPMODE_NULL

} IHCOpModeSpecifier;

typedef enum {

	IHC_MEDDIS2000_CACONDMODE_ORIGINAL,
	IHC_MEDDIS2000_CACONDMODE_REVISION1,
	IHC_MEDDIS2000_CACONDMODE_NULL

} IHCCaCondModeSpecifier;

typedef enum {

	IHC_MEDDIS2000_CLEFTREPLENISHMODE_ORIGINAL,
	IHC_MEDDIS2000_CLEFTREPLENISHMODE_UNITY,
	IHC_MEDDIS2000_CLEFTREPLENISHMODE_NULL

} IHCCleftReplenishModeSpecifier;

typedef struct {

	ParameterSpecifier	parSpec;

	BOOLN	opModeFlag, diagModeFlag, caCondModeFlag, cleftReplenishModeFlag;
	BOOLN	ranSeedFlag, CaVrevFlag, betaCaFlag, gammaCaFlag, pCaFlag;
	BOOLN	GCaMaxFlag, perm_Ca0Flag, perm_zFlag, tauCaChanFlag, tauConcCaFlag;
	BOOLN	maxFreePool_MFlag, replenishRate_yFlag, lossRate_lFlag;
	BOOLN	reprocessRate_xFlag, recoveryRate_rFlag;
	BOOLN	updateProcessVariablesFlag;

	int		opMode;
	int		diagMode;
	int		caCondMode;
	int		cleftReplenishMode;
	long	ranSeed;
	double	CaVrev;
	double	betaCa;
	double	gammaCa;
	double	pCa;
	double	GCaMax;
	double	perm_Ca0;
	double	perm_z;
	double	tauCaChan;
	double	tauConcCa;
	int		maxFreePool_M;
	double	replenishRate_y;
	double	lossRate_l;
	double	reprocessRate_x;
	double	recoveryRate_r;

	/* Private members */
	NameSpecifier	*opModeList;
	NameSpecifier	*diagModeList;
	NameSpecifier	*caCondModeList;
	NameSpecifier	*cleftReplenishModeList;
	UniParListPtr	parList;
	HairCellVars2	*hCChannels;
	char		diagFileName[MAX_FILE_PATH];
	FILE		*fp;
	double		ydt, rdt, xdt, zdt, k0pow, l_Plus_rdt, dt_Over_tauConcCa;
	double		dtOverTauCaChan;

} HairCell2, *HairCell2Ptr;

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

extern	HairCell2Ptr	hairCell2Ptr;

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
* macros, to allow the safe use of C libraries with C++ libraries - defined
* in GeCommon.h.
*/
__BEGIN_DECLS

BOOLN	CheckData_IHC_Meddis2000(EarObjectPtr data);

BOOLN	CheckPars_IHC_Meddis2000(void);

BOOLN	Free_IHC_Meddis2000(void);

UniParListPtr	GetUniParListPtr_IHC_Meddis2000(void);

BOOLN	InitCaCondModeList_IHC_Meddis2000(void);

BOOLN	InitCleftReplenishModeList_IHC_Meddis2000(void);

BOOLN	InitDiagModeList_IHC_Meddis2000(void);

BOOLN	InitOpModeList_IHC_Meddis2000(void);

BOOLN	Init_IHC_Meddis2000(ParameterSpecifier parSpec);

BOOLN	PrintPars_IHC_Meddis2000(void);

BOOLN	ReadPars_IHC_Meddis2000(char *fileName);

void	ResetProcess_IHC_Meddis2000(EarObjectPtr data);

BOOLN	RunModel_IHC_Meddis2000(EarObjectPtr data);

BOOLN	SetBetaCa_IHC_Meddis2000(double theBetaCa);

BOOLN	SetCaCondMode_IHC_Meddis2000(char * theCaCondMode);

BOOLN	SetCaVrev_IHC_Meddis2000(double theCaVrev);

BOOLN	SetCleftReplenishMode_IHC_Meddis2000(char * theCleftReplenishMode);

BOOLN	SetDiagMode_IHC_Meddis2000(char * theDiagMode);

void	SetEnabledPars_IHC_Meddis2000(void);

BOOLN	SetGCaMax_IHC_Meddis2000(double theGCaMax);

BOOLN	SetGammaCa_IHC_Meddis2000(double theGammaCa);

BOOLN	SetLossRate_l_IHC_Meddis2000(double theLossRate_l);

BOOLN	SetMaxFreePool_M_IHC_Meddis2000(int theMaxFreePool_M);

BOOLN	InitModule_IHC_Meddis2000(ModulePtr theModule);

BOOLN	SetOpMode_IHC_Meddis2000(char * theOpMode);

BOOLN	SetPCa_IHC_Meddis2000(double thePCa);

BOOLN	SetParsPointer_IHC_Meddis2000(ModulePtr theModule);

BOOLN	SetPars_IHC_Meddis2000(char * opMode, char * diagMode, long ranSeed,
		  double CaVrev, double betaCa, double gammaCa,
		  double pCa, double GCaMax, double perm_Ca0, double perm_z,
		  double tauCaChan, double tauConcCa, int maxFreePool_M,
		  double replenishRate_y, double lossRate_l, double reprocessRate_x,
		  double recoveryRate_r);

BOOLN	SetPerm_Ca0_IHC_Meddis2000(double thePerm_Ca0);

BOOLN	SetPerm_z_IHC_Meddis2000(double thePerm_z);

BOOLN	SetRanSeed_IHC_Meddis2000(long theRanSeed);

BOOLN	SetRecoveryRate_r_IHC_Meddis2000(double theRecoveryRate_r);

BOOLN	SetReplenishRate_y_IHC_Meddis2000(double theReplenishRate_y);

BOOLN	SetReprocessRate_x_IHC_Meddis2000(double theReprocessRate_x);

BOOLN	SetTauCaChan_IHC_Meddis2000(double theTauCaChan);

BOOLN	SetTauConcCa_IHC_Meddis2000(double theTauConcCa);

BOOLN	SetUniParList_IHC_Meddis2000(void);

void	FreeProcessVariables_IHC_Meddis2000(void);

__END_DECLS

#endif
