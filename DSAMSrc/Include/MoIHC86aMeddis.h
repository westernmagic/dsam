/******************
 *
 * File:		MoIHC86aMeddis.h
 * Purpose:		This module contains the routines for the Meddis 86a Hair cell
 *				model module.
 * Comments:	It is expects input from a IHC receptor potential model module.
 * Authors:		L. P. O'Mard
 * Created:		12 Jul 1996
 * Updated:		05 Sep 1996
 * Copyright:	(c) 1998, University of Essex
 * 
 ******************/

#ifndef	_MOIHC86AMEDDIS_H
#define _MOIHC86AMEDDIS_H	1
 
/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

#define IHC_MEDDIS86A_NUM_PARS	8
#define	MEDDIS86A_MAX_DT		0.0001		/* Magic number for model */

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

typedef enum {

	IHC_MEDDIS86A_MAXFREEPOOL_M,
	IHC_MEDDIS86A_REPLENISHRATE_Y,
	IHC_MEDDIS86A_LOSSRATE_L,
	IHC_MEDDIS86A_REPROCESSRATE_X,
	IHC_MEDDIS86A_RECOVERYRATE_R,
	IHC_MEDDIS86A_PERMEABILITYPH_H,
	IHC_MEDDIS86A_PERMEABILITYPZ_Z,
	IHC_MEDDIS86A_FIRINGRATE_H2

} HairCell3ParSpecifier;

typedef struct {

	double	cleftC;			/* Quantity of transmitter in the cleft */
	double	reservoirQ;		/* Quantity of transmitter in the free pool */
	double	reprocessedW;	/* Quantity of transm. in the reproc. store */

} HairCellVars3, *HairCellVars3Ptr;
	
typedef struct {

	ParameterSpecifier parSpec;
	
	BOOLN	replenishRateFlag, lossRateFlag, reprocessRateFlag;
	BOOLN	recoveryRateFlag, maxFreePoolFlag, permeabilityPHFlag;
	BOOLN	permeabilityPZFlag, firingRateFlag;
	BOOLN	updateProcessVariablesFlag;

	double	maxFreePool_M;		/* Max. no. of transm. packets in free pool */
	double	replenishRate_y;	/* Replenishment rate (units per second). */
	double	lossRate_l;			/* Loss rate (units per second). */
	double	reprocessRate_x;	/* Reprocessing rate (units per second). */
	double	recoveryRate_r;		/* Recovery rate (units per second). */
	double	permeabilityPH_h;	/* Permeability constant. */
	double	permeabilityPZ_z;	/* Permeability constant. */
	double	firingRate_h2;		/* Permeability constant. */

	/* Private members */
	UniParListPtr	parList;
	HairCellVars3	*hCChannels;

} HairCell3, *HairCell3Ptr;

/******************************************************************************/
/*************************** External Variables *******************************/
/******************************************************************************/

extern	HairCell3Ptr	hairCell3Ptr;

/******************************************************************************/
/*************************** Function Prototypes ******************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

BOOLN	CheckData_IHC_Meddis86a(EarObjectPtr data);

BOOLN	CheckPars_IHC_Meddis86a(void);

BOOLN	Free_IHC_Meddis86a(void);

void	FreeProcessVariables_IHC_Meddis86a(void);

UniParListPtr	GetUniParListPtr_IHC_Meddis86a(void);

BOOLN	Init_IHC_Meddis86a(ParameterSpecifier parSpec);

BOOLN	InitProcessVariables_IHC_Meddis86a(EarObjectPtr data);

BOOLN	PrintPars_IHC_Meddis86a(void);

BOOLN	ReadPars_IHC_Meddis86a(char *fileName);

BOOLN	RunModel_IHC_Meddis86a(EarObjectPtr data);

BOOLN	SetFiringRate_IHC_Meddis86a(double theSetFiringRate);

BOOLN	SetLossRate_IHC_Meddis86a(double theLossRate);

BOOLN	SetMaxFreePool_IHC_Meddis86a(double theMaxFreePool);

BOOLN	SetPars_IHC_Meddis86a(double y, double l, double x, double r, double M,
		  double h, double z, double h2);
		  
BOOLN	SetPermeabilityPH_IHC_Meddis86a(double thePermeabilityPH);

BOOLN	SetPermeabilityPZ_IHC_Meddis86a(double thePermeabilityPZ);

BOOLN	SetReplenishRate_IHC_Meddis86a(double theReplenishRate);

BOOLN	SetReprocessRate_IHC_Meddis86a(double theReprocessRate);

BOOLN	SetRecoveryRate_IHC_Meddis86a(double theRecoveryRate);

BOOLN	SetUniParList_IHC_Meddis86a(void);

__END_DECLS

#endif
