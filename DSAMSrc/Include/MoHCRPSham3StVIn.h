/**********************
 *
 * File:		MoHCRPSham3StVIn.h
 * Purpose:		
 * Comments:	Written using ModuleProducer version 1.3.2 (Mar 27 2001).
 * Author:		
 * Created:		20 Aug 2001
 * Updated:	
 * Copyright:	(c) 2001, 
 *
 *********************/

#ifndef _MOHCRPSHAM3STVIN_H
#define _MOHCRPSHAM3STVIN_H 1

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define IHCRP_SHAMMA3STATEVELIN_NUM_PARS			14

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef enum {

	IHCRP_SHAMMA3STATEVELIN_ENDOCOCHLEARPOT_ET,
	IHCRP_SHAMMA3STATEVELIN_REVERSALPOT_EK,
	IHCRP_SHAMMA3STATEVELIN_REVERSALPOTCORRECTION,
	IHCRP_SHAMMA3STATEVELIN_TOTALCAPACITANCE_C,
	IHCRP_SHAMMA3STATEVELIN_RESTINGCONDUCTANCE_G0,
	IHCRP_SHAMMA3STATEVELIN_KCONDUCTANCE_GK,
	IHCRP_SHAMMA3STATEVELIN_MAXMCONDUCTANCE_GMAX,
	IHCRP_SHAMMA3STATEVELIN_CILIATIMECONST_TC,
	IHCRP_SHAMMA3STATEVELIN_CILIACOUPLINGGAIN_C,
	IHCRP_SHAMMA3STATEVELIN_REFERENCEPOT,
	IHCRP_SHAMMA3STATEVELIN_SENSITIVITY_S0,
	IHCRP_SHAMMA3STATEVELIN_SENSITIVITY_S1,
	IHCRP_SHAMMA3STATEVELIN_OFFSET_X0,
	IHCRP_SHAMMA3STATEVELIN_OFFSET_X1

} Sham3StVInParSpecifier;

typedef struct {

	ParameterSpecifier	parSpec;

	BOOLN	endocochlearPot_EtFlag, reversalPot_EkFlag;
	BOOLN	reversalPotCorrectionFlag, totalCapacitance_CFlag;
	BOOLN	restingConductance_G0Flag, kConductance_GkFlag;
	BOOLN	maxMConductance_GmaxFlag, ciliaTimeConst_tcFlag;
	BOOLN	ciliaCouplingGain_CFlag, referencePotFlag, sensitivity_s0Flag;
	BOOLN	sensitivity_s1Flag, offset_x0Flag, offset_x1Flag;
	BOOLN	updateProcessVariablesFlag;
	double	endocochlearPot_Et;
	double	reversalPot_Ek;
	double	reversalPotCorrection;
	double	totalCapacitance_C;
	double	restingConductance_G0;
	double	kConductance_Gk;
	double	maxMConductance_Gmax;
	double	ciliaTimeConst_tc;
	double	ciliaCouplingGain_C;
	double	referencePot;
	double	sensitivity_s0;
	double	sensitivity_s1;
	double	offset_x0;
	double	offset_x1;

	/* Private members */
	UniParListPtr	parList;
	double	*lastCiliaDisplacement_u;
	double	*lastInput;
	double	*lastOutput;

} Sham3StVIn, *Sham3StVInPtr;

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

extern	Sham3StVInPtr	sham3StVInPtr;

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
* macros, to allow the safe use of C libraries with C++ libraries - defined
* in GeCommon.h.
*/
__BEGIN_DECLS

BOOLN	CheckData_IHCRP_Shamma3StateVelIn(EarObjectPtr data);

BOOLN	CheckPars_IHCRP_Shamma3StateVelIn(void);

BOOLN	FreeProcessVariables_IHCRP_Shamma3StateVelIn(void);

BOOLN	Free_IHCRP_Shamma3StateVelIn(void);

double	GetRestingResponse_IHCRP_Shamma(void);

UniParListPtr	GetUniParListPtr_IHCRP_Shamma3StateVelIn(void);

BOOLN	InitModule_IHCRP_Shamma3StateVelIn(ModulePtr theModule);

BOOLN	InitProcessVariables_IHCRP_Shamma3StateVelIn(EarObjectPtr data);

BOOLN	Init_IHCRP_Shamma3StateVelIn(ParameterSpecifier parSpec);

BOOLN	PrintPars_IHCRP_Shamma3StateVelIn(void);

BOOLN	ReadPars_IHCRP_Shamma3StateVelIn(char *fileName);

BOOLN	RunModel_IHCRP_Shamma3StateVelIn(EarObjectPtr data);

BOOLN	SetCiliaCouplingGain_C_IHCRP_Shamma3StateVelIn(
		  double theCiliaCouplingGain_C);

BOOLN	SetCiliaTimeConst_tc_IHCRP_Shamma3StateVelIn(
		  double theCiliaTimeConst_tc);

BOOLN	SetEndocochlearPot_Et_IHCRP_Shamma3StateVelIn(
		  double theEndocochlearPot_Et);

BOOLN	SetKConductance_Gk_IHCRP_Shamma3StateVelIn(double theKConductance_Gk);

BOOLN	SetMaxMConductance_Gmax_IHCRP_Shamma3StateVelIn(
		  double theMaxMConductance_Gmax);

BOOLN	SetOffset_x0_IHCRP_Shamma3StateVelIn(double theOffset_x0);

BOOLN	SetOffset_x1_IHCRP_Shamma3StateVelIn(double theOffset_x1);

BOOLN	SetParsPointer_IHCRP_Shamma3StateVelIn(ModulePtr theModule);

BOOLN	SetPars_IHCRP_Shamma3StateVelIn(double endocochlearPot_Et,
		  double reversalPot_Ek, double reversalPotCorrection,
		  double totalCapacitance_C, double restingConductance_G0,
		  double kConductance_Gk, double maxMConductance_Gmax,
		  double ciliaTimeConst_tc, double ciliaCouplingGain_C,
		  double referencePot, double sensitivity_s0, double sensitivity_s1,
		  double offset_x0, double offset_x1);

BOOLN	SetReferencePot_IHCRP_Shamma3StateVelIn(double theReferencePot);

BOOLN	SetRestingConductance_G0_IHCRP_Shamma3StateVelIn(
		  double theRestingConductance_G0);

BOOLN	SetReversalPotCorrection_IHCRP_Shamma3StateVelIn(
		  double theReversalPotCorrection);

BOOLN	SetReversalPot_Ek_IHCRP_Shamma3StateVelIn(double theReversalPot_Ek);

BOOLN	SetSensitivity_s0_IHCRP_Shamma3StateVelIn(double theSensitivity_s0);

BOOLN	SetSensitivity_s1_IHCRP_Shamma3StateVelIn(double theSensitivity_s1);

BOOLN	SetTotalCapacitance_C_IHCRP_Shamma3StateVelIn(
		  double theTotalCapacitance_C);

BOOLN	SetUniParList_IHCRP_Shamma3StateVelIn(void);

__END_DECLS

#endif
