/**********************
 *
 * File:		MoHCRPLopezPoveda.h
 * Purpose:		This module is the hair-cell receptor potential from
 * 				Lopez-Poveda E.A., Eustaquio-Martın A. (2006) "A biophysical
 *				model of the inner hair cell: the contribution of potassium
 *				current to peripheral compression", J. Assoc. Res. Otolaryngol.,
 *				7, 218–235.
 * Comments:	Written using ModuleProducer version 1.6.1 (Jul  14 2008).
 *				The BM-cilia coupling process was added to this module to make
 * 				it equivalent to the previous HCRP_... modules.  IT can be
 * 				turned-off to get the IHC output as produced in the the paper.
 * Author:		L. P. O'Mard
 * Created:		14 Jul 2008
 * Updated:	
 * Copyright:	(c) 2008, L. P. O'Mard.
 *
 *********************/

#ifndef _MOHCRPLOPEZPOVEDA_H
#define _MOHCRPLOPEZPOVEDA_H 1

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define IHCRP_LOPEZPOVEDA_MOD_NAME			wxT("IHCRP_LOPEZPOVEDA")
#define IHCRP_LOPEZPOVEDA_NUM_PARS			31
#define IHCRP_LOPEZPOVEDA_NUM_STATE_VARS	3

/******************************************************************************/
/****************************** Macro definitions *****************************/
/******************************************************************************/

#define CURRENT_TIME_CONST(TMIN, TDIFF, A, B, VM) ((TMIN) + (TDIFF) / \
		  (1.0 + exp(((A) + (VM)) / (B))))
#define BOLTZMANN_2ORDER(A, E_SUB, E11, E12, E21, E22)	((A) / (1.0 + \
		  exp(((E11) - (E_SUB)) / (E12)) * (1.0 + exp(((E21) - (E_SUB)) / (E22)))))

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef enum {

	IHCRP_LOPEZPOVEDA_BMCILIACOUPLINGMODE,
	IHCRP_LOPEZPOVEDA_CILIATC,
	IHCRP_LOPEZPOVEDA_CILIACGAIN,
	IHCRP_LOPEZPOVEDA_ET,
	IHCRP_LOPEZPOVEDA_RP,
	IHCRP_LOPEZPOVEDA_RT,
	IHCRP_LOPEZPOVEDA_GL,
	IHCRP_LOPEZPOVEDA_GM,
	IHCRP_LOPEZPOVEDA_S0,
	IHCRP_LOPEZPOVEDA_S1,
	IHCRP_LOPEZPOVEDA_U0,
	IHCRP_LOPEZPOVEDA_U1,
	IHCRP_LOPEZPOVEDA_CA,
	IHCRP_LOPEZPOVEDA_CB,
	IHCRP_LOPEZPOVEDA_NUMCURRENTS,
	IHCRP_LOPEZPOVEDA_LABEL,
	IHCRP_LOPEZPOVEDA_EK,
	IHCRP_LOPEZPOVEDA_G,
	IHCRP_LOPEZPOVEDA_V1,
	IHCRP_LOPEZPOVEDA_VS1,
	IHCRP_LOPEZPOVEDA_V2,
	IHCRP_LOPEZPOVEDA_VS2,
	IHCRP_LOPEZPOVEDA_TAU1MAX,
	IHCRP_LOPEZPOVEDA_A1,
	IHCRP_LOPEZPOVEDA_B1,
	IHCRP_LOPEZPOVEDA_TAU1MIN,
	IHCRP_LOPEZPOVEDA_TAU2MAX,
	IHCRP_LOPEZPOVEDA_A2,
	IHCRP_LOPEZPOVEDA_B2,
	IHCRP_LOPEZPOVEDA_TAU2MIN,
	IHCRP_LOPEZPOVEDA_VMREST

} LopezPovedaParSpecifier;

typedef struct {

	double	Ekp;
	double	tau1Diff, tau2Diff;
	double	GkVTN, GkVT;
	double	stateVector[IHCRP_LOPEZPOVEDA_NUM_STATE_VARS];

} LPIonChannel, *LPIonChannelPtr;

typedef struct {

	ParameterSpecifier	parSpec;

	BOOLN	updateProcessVariablesFlag;
	int		bMCiliaCouplingMode;
	double	ciliaTC;	/* BM/cilia displacement time constant, s. */
	double	ciliaCGain;	/* Cilia coupling gain. */
	double	Et;			/* Endocochlear potential, Et (V). */
	double	Rp;			/* Epithelium resistance, Rp (Ohm). */
	double	Rt;			/* Epithelium resistance, Rt (Ohm). */
	double	gL;			/* Apical leakage conductance, gL (S). */
	double	Gm;			/* Maximum mechanical conductance, Gm (S). */
	double	s0;			/* Displacement sensitivity, s0 (1/m). */
	double	s1;			/* Displacement sensitivity, s1 (1/m). */
	double	u0;			/* Displacement offset, u0 (m). */
	double	u1;			/* Displacement offset, u1 (m). */
	double	Ca;			/* Apical capacitance, Ca (F). */
	double	Cb;			/* Apical capacitance, Cb (F). */
	int		numCurrents;	/* Number of currents - default is two: fast & slow.*/
	WChar	**label;	/* Conductance label. */
	double	*Ek;		/* Reversal potential, Ek (V). */
	double	*G;			/* Maximum conductance, Gf (S). */
	double	*V1;		/* Half-activation potential, V1 (V). */
	double	*vS1;		/* Voltage sensitivity constant, S1 (V). */
	double	*V2;		/* Half-activation potential, V2 (V). */
	double	*vS2;		/* Voltage sensitivity constant, S2 (V). */
	double	*tau1Max;	/* Voltage time constant, tau1 max. (s). */
	double	*A1;		/* Voltage time constant, A1 (s). */
	double	*B1;		/* Voltage time constant, B1 (s). */
	double	*tau1Min;	/* Voltage time constant, tau1Min (s). */
	double	*tau2Max;	/* Voltage time constant, tau2Max (s). */
	double	*A2;		/* Voltage time constant, A2 (s). */
	double	*B2;		/* Voltage time constant, B2 (s). */
	double	*tau2Min;	/* Voltage time constant, tau2Min (s). */
	double	VMRest;

	/* Private members */
	UniParListPtr	parList;
	LPIonChannelPtr	*lPICList;
	int		numChannels;
	double	*lastCiliaDisplacement_u;
	double	*lastOutput;
	double	CTotalOverDt, VOC, cGaindt, dtOverTc;

} LopezPoveda, *LopezPovedaPtr;

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

extern	LopezPovedaPtr	lopezPovedaPtr;

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
* macros, to allow the safe use of C libraries with C++ libraries - defined
* in GeCommon.h.
*/
__BEGIN_DECLS

BOOLN	AllocNumCurrents_IHCRP_LopezPoveda(int numCurrents);

BOOLN	CheckData_IHCRP_LopezPoveda(EarObjectPtr data);

BOOLN	FreeProcessVariables_IHCRP_LopezPoveda(void);

BOOLN	Free_IHCRP_LopezPoveda(void);

UniParListPtr	GetUniParListPtr_IHCRP_LopezPoveda(void);

BOOLN	InitModule_IHCRP_LopezPoveda(ModulePtr theModule);

BOOLN	InitProcessVariables_IHCRP_LopezPoveda(EarObjectPtr data);

BOOLN	Init_IHCRP_LopezPoveda(ParameterSpecifier parSpec);

BOOLN	PrintPars_IHCRP_LopezPoveda(void);

BOOLN	RunModel_IHCRP_LopezPoveda(EarObjectPtr data);

BOOLN	SetA1_IHCRP_LopezPoveda(double *theA1);

BOOLN	SetA2_IHCRP_LopezPoveda(double *theA2);

BOOLN	SetB1_IHCRP_LopezPoveda(double *theB1);

BOOLN	SetB2_IHCRP_LopezPoveda(double *theB2);

BOOLN	SetBMCiliaCouplingMode_IHCRP_LopezPoveda(
		  WChar * theBMCiliaCouplingMode);

BOOLN	SetCa_IHCRP_LopezPoveda(double theCa);

BOOLN	SetCb_IHCRP_LopezPoveda(double theCb);

BOOLN	SetCiliaCGain_IHCRP_LopezPoveda(double theCiliaCGain);

BOOLN	SetCiliaTC_IHCRP_LopezPoveda(double theCiliaTC);

BOOLN	SetDefaultNumCurrentsArrays_IHCRP_LopezPoveda(void);

BOOLN	SetEk_IHCRP_LopezPoveda(double *theEk);

BOOLN	SetEt_IHCRP_LopezPoveda(double theEt);

BOOLN	SetGL_IHCRP_LopezPoveda(double theGL);

BOOLN	SetG_IHCRP_LopezPoveda(double *theG);

BOOLN	SetGm_IHCRP_LopezPoveda(double theGm);

BOOLN	SetIndividualA1_IHCRP_LopezPoveda(int theIndex, double theA1);

BOOLN	SetIndividualA2_IHCRP_LopezPoveda(int theIndex, double theA2);

BOOLN	SetIndividualB1_IHCRP_LopezPoveda(int theIndex, double theB1);

BOOLN	SetIndividualB2_IHCRP_LopezPoveda(int theIndex, double theB2);

BOOLN	SetIndividualEk_IHCRP_LopezPoveda(int theIndex, double theEk);

BOOLN	SetIndividualG_IHCRP_LopezPoveda(int theIndex, double theG);

BOOLN	SetIndividualLabel_IHCRP_LopezPoveda(int theIndex, WChar *theLabel);

BOOLN	SetIndividualVS1_IHCRP_LopezPoveda(int theIndex, double theVS1);

BOOLN	SetIndividualVS2_IHCRP_LopezPoveda(int theIndex, double theVS2);

BOOLN	SetIndividualTau1Max_IHCRP_LopezPoveda(int theIndex,
		  double theTau1Max);

BOOLN	SetIndividualTau1Min_IHCRP_LopezPoveda(int theIndex,
		  double theTau1Min);

BOOLN	SetIndividualTau2Max_IHCRP_LopezPoveda(int theIndex,
		  double theTau2Max);

BOOLN	SetIndividualTau2Min_IHCRP_LopezPoveda(int theIndex,
		  double theTau2Min);

BOOLN	SetIndividualV1_IHCRP_LopezPoveda(int theIndex, double theV1);

BOOLN	SetIndividualV2_IHCRP_LopezPoveda(int theIndex, double theV2);

BOOLN	SetLabel_IHCRP_LopezPoveda(WChar **theLabel);

BOOLN	SetNumCurrents_IHCRP_LopezPoveda(int theNumCurrents);

BOOLN	SetParsPointer_IHCRP_LopezPoveda(ModulePtr theModule);

BOOLN	SetRp_IHCRP_LopezPoveda(double theRp);

BOOLN	SetRt_IHCRP_LopezPoveda(double theRt);

BOOLN	SetS0_IHCRP_LopezPoveda(double theS0);

BOOLN	SetS1_IHCRP_LopezPoveda(double theS1);

BOOLN	SetTau1Max_IHCRP_LopezPoveda(double *theTau1Max);

BOOLN	SetTau1Min_IHCRP_LopezPoveda(double *theTau1Min);

BOOLN	SetTau2Max_IHCRP_LopezPoveda(double *theTau2Max);

BOOLN	SetTau2Min_IHCRP_LopezPoveda(double *theTau2Min);

BOOLN	SetU0_IHCRP_LopezPoveda(double theU0);

BOOLN	SetU1_IHCRP_LopezPoveda(double theU1);

BOOLN	SetUniParList_IHCRP_LopezPoveda(void);

BOOLN	SetV1_IHCRP_LopezPoveda(double *theV1);

BOOLN	SetV2_IHCRP_LopezPoveda(double *theV2);

BOOLN	SetVS1_IHCRP_LopezPoveda(double *theVS1);

BOOLN	SetVS2_IHCRP_LopezPoveda(double *theVS2);

BOOLN	SetVMRest_IHCRP_LopezPoveda(double theVMRest);

__END_DECLS

#endif
