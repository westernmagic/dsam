/**********************
 *
 * File:		MoHCRPMeddis.h
 * Purpose:		This module contains the model for the hair cell receptor
 *				potential.
 * Comments:	07-05-98 LPO: This model has now changed to a new version.  It
 *				uses the Meddis 86 permeability function with the low-pass
 *				filter.
 * Author:		L. P. O'Mard
 * Created:		12 Jul 1993
 * Updated:		07 May 1998
 * Copyright:	(c) 1998, University of Essex
 *
 *********************/

#ifndef	_MOHCRPMEDDIS_H
#define _MOHCRPMEDDIS_H	1
 
/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

#define IHCRP_MEDDIS_NUM_PARS			4

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

typedef enum {

	IHCRP_MEDDIS_PERMCONST_A,
	IHCRP_MEDDIS_PERMCONST_B,
	IHCRP_MEDDIS_RELEASERATE_G,
	IHCRP_MEDDIS_MTIMECONST_TM

} MeddisRPParSpecifier;

typedef struct {

	ParameterSpecifier parSpec;
	
	BOOLN	permConstAFlag, permConstBFlag, releaseRateFlag, mTimeConstTmFlag;
	BOOLN	updateProcessVariablesFlag;
	
	double	permConst_A;		/* For K+ probability: base Amplitude (uPa). */
	double	permConst_B;		/* For K+ probability: amplitude scale (/uPa) */
	double	releaseRate_g;		/* Release rate (units per second). */
	double	mTimeConst_tm;		/* Membrane time constant, s. */

	/* Private members */
	UniParListPtr	parList;
	double	*lastOutput;
	
} MeddisRP, *MeddisRPPtr;

/******************************************************************************/
/*************************** External Variables *******************************/
/******************************************************************************/

extern	MeddisRPPtr	meddisRPPtr;

/******************************************************************************/
/*************************** Function Prototypes ******************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

BOOLN	CheckData_IHCRP_Meddis(EarObjectPtr data);

BOOLN	CheckPars_IHCRP_Meddis(void);

BOOLN	Free_IHCRP_Meddis(void);

void	FreeProcessVariables_IHCRP_Meddis(void);

double	GetRestingResponse_IHCRP_Meddis(void);

UniParListPtr	GetUniParListPtr_IHCRP_Meddis(void);

BOOLN	Init_IHCRP_Meddis(ParameterSpecifier parSpec);

BOOLN	InitProcessVariables_IHCRP_Meddis(EarObjectPtr data);

BOOLN	PrintPars_IHCRP_Meddis(void);

BOOLN	ReadPars_IHCRP_Meddis(char *fileName);

BOOLN	RunModel_IHCRP_Meddis(EarObjectPtr data);

BOOLN	SetMTimeConstTm_IHCRP_Meddis(double theMTimeConstTm);

BOOLN	SetPermConstA_IHCRP_Meddis(double thePermConstA);

BOOLN	SetPermConstB_IHCRP_Meddis(double thePermConstB);

BOOLN	InitModule_IHCRP_Meddis(ModulePtr theModule);

BOOLN	SetParsPointer_IHCRP_Meddis(ModulePtr theModule);

BOOLN	SetPars_IHCRP_Meddis(double aA, double bB, double g, double tm);
		  
BOOLN	SetReleaseRate_IHCRP_Meddis(double theReleaseRate);

BOOLN	SetUniParList_IHCRP_Meddis(void);

__END_DECLS

#endif
