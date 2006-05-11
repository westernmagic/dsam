/**********************
 *
 * File:		UtIonChanList.h
 * Purpose:		This module generates the ion channel tables list, reading the
 *				tables from file or generating them as required.
 * Comments:	Written using ModuleProducer version 1.10 (Oct 13 1996).
 *				13-01-97: LPO: Introduce HHuxley Channel list mode.
 *				26-11-97 LPO: Using new GeNSpecLists module for 'mode'
 *				parameter.
 *				18-11-98 LPO: Introduced the 'SetParentICList_IonChanList' and
 *				the 'parentPtr' so that a ICList can always set the pointer
 *				which is pointing to it.
 *				08-12-98 LPO: Separated the IC channel generation routines into
 *				GetPars... and Generate... routines.
 *				14-12-98 LPO: Converted the 'InitTable_' so that it initialises
 *				the table for a single channel.  This means that it can be used
 *				by the 'ReadIonChannel_' routine as well as the others.
 *				03-02-99 LPO: Introduced the 'mode' field for the 'IonChannel'
 *				structure. This allows the 'SetBaseMaxConducance' routine
 *				called from the GUI to set the correct maxConductance.  I also
 *				had to add the 'temperature' field too.
 *				Changed the 'InitTable_' routine to 'SetIonChannel'.
 * Author:		L. P. O'Mard
 * Created:		13 Oct 1996
 * Updated:		03 Feb 1999
 * Copyright:	(c) 1999, University of Essex.
 *
 *********************/

#ifndef _UTIONCHANLIST_H
#define _UTIONCHANLIST_H 1

#include "UtDynaList.h"

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define ICLIST_NUM_PARS				9
#define ICLIST_NUM_CONSTANT_PARS	7

#define ICLIST_IC_NUM_PARS			33
#define ICLIST_IC_NUM_CONSTANT_PARS	6

#define	E_OVER_R					11.8e3
#define	ABSOLUTE_ZERO_DEGREES		-273.0
#define	ICLIST_NUM_GATES			2

/******************************************************************************/
/****************************** Macro definitions *****************************/
/******************************************************************************/

#define HHUXLEY_ALL_ZERO_PARS(A, B, C, D, E, F, G, H, I, J, K) ((fabs(A) + \
		  fabs(B) + fabs(C) + fabs(D) + fabs(E) + fabs(F) + fabs(G) + \
		  fabs(H) + fabs(I) + fabs(J) + fabs(K)) <= DBL_EPSILON)
		  
#define HHUXLEY_TF(A, T)	(pow(A, ((T) - 22) / 10))

#define SET_IC_GATE_ARRAY(GATE)	((GATE).ptr = (GATE).array)

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

/*
 * Ion channel modes.
 */

typedef enum {

	ICLIST_BOLTZMANN_MODE,
	ICLIST_HHUXLEY_MODE,
	ICLIST_FILE_MODE,
	ICLIST_NULL

} ICModeSpecifier;

typedef enum {

	ICLIST_PRINTTABLESMODE,
	ICLIST_BASELEAKAGECOND,
	ICLIST_LEAKAGEPOT,
	ICLIST_NUMCHANNELS,
	ICLIST_MIN_VOLT,
	ICLIST_MAX_VOLT,
	ICLIST_DV,
	ICLIST_TEMPERATURE,
	ICLIST_LEAKAGE_COND_Q10

} ICListParSpecifier;

typedef enum {

	ICLIST_IC_DESCRIPTION,
	ICLIST_IC_MODE,
	ICLIST_IC_ENABLED,
	ICLIST_IC_EQUILIBRIUM_POT,
	ICLIST_IC_CONDUCTANCE,
	ICLIST_IC_ACTIVATION_EXPONENT,
	ICLIST_IC_CONDUCTANCE_Q10,
	ICLIST_IC_V_HALF,
	ICLIST_IC_Z,
	ICLIST_IC_TAU,
	ICLIST_IC_FILE_NAME,
	ICLIST_IC_ALPHA_A,
	ICLIST_IC_ALPHA_B,
	ICLIST_IC_ALPHA_C,
	ICLIST_IC_ALPHA_D,
	ICLIST_IC_ALPHA_E,
	ICLIST_IC_ALPHA_F,
	ICLIST_IC_ALPHA_G,
	ICLIST_IC_ALPHA_H,
	ICLIST_IC_ALPHA_I,
	ICLIST_IC_ALPHA_J,
	ICLIST_IC_ALPHA_K,
	ICLIST_IC_BETA_A,
	ICLIST_IC_BETA_B,
	ICLIST_IC_BETA_C,
	ICLIST_IC_BETA_D,
	ICLIST_IC_BETA_E,
	ICLIST_IC_BETA_F,
	ICLIST_IC_BETA_G,
	ICLIST_IC_BETA_H,
	ICLIST_IC_BETA_I,
	ICLIST_IC_BETA_J,
	ICLIST_IC_BETA_K

} IonChannelParSpecifier;

typedef struct {

	double	yY, ty;
	double	zZ, tz;
	
} ICTableEntry, *ICTableEntryPtr;

typedef struct {

	double	array[ICLIST_NUM_GATES];
	double	*ptr;

} ICGateArray, *ICGateArrayPtr;

typedef struct {

	ICGateArray	aA, aB, aC, aD, aE, aF, aG, aH, aI, aJ, aK;
	ICGateArray	bA, bB, bC, bD, bE, bF, bG, bH, bI, bJ, bK;
	
} ICHHuxleyPars, *ICHHuxleyParsPtr;	

typedef	struct {

	ICGateArray	halfMaxV;
	ICGateArray	zZ;
	ICGateArray	tau;
	
} ICBoltzmannPars, *ICBoltzmannParsPtr;
	
typedef struct {

	BOOLN	updateFlag;
	ICModeSpecifier		mode;
	BOOLN	enabled;
	WChar	description[MAXLINE];
	int		numTableEntries;
	double	activationExponent;
	double	equilibriumPot;
	double	temperature;
	double	baseMaxConductance, maxConductance, conductanceQ10;
	double	minVoltage;
	double	maxVoltage;
	double	dV;
	ICBoltzmannPars	boltzmann;
	ICHHuxleyPars	hHuxley;
	WChar			fileName[MAX_FILE_PATH];
	double			(* PowFunc)(double, double);
	ICTableEntry	*table;
	UniParListPtr	parList;
	
} IonChannel, *IonChannelPtr;

typedef struct IonChanList {

	BOOLN	updateFlag;
	BOOLN	useTemperatureCalcFlag;
	int		printTablesMode;
	double	temperature;
	double	baseLeakageCond, leakageCond, leakageCondQ10;
	double	leakagePot;
	int		numChannels;
	double	minVoltage;
	double	maxVoltage;
	double	dV;
	DynaListPtr	ionChannels;

	/* Private members */
	NameSpecifier	*printTablesModeList;
	UniParListPtr	parList;
	IonChannelPtr	currentIC;			/* Used by universal parameter list. */
	int		oldNumChannels;
	int		numTableEntries;
	int		oldNumTableEntries;
	WChar	diagFileName[MAX_FILE_PATH];

} IonChanList, *IonChanListPtr;

/******************************************************************************/
/****************************** Global Variables ******************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

BOOLN	CheckInit_IonChanList(IonChanListPtr theICList,
		  const WChar *callingFunction);

BOOLN	CheckInitIC_IonChanList(IonChannelPtr theIC,
		  const WChar *callingFunction);

BOOLN	CheckPars_IonChanList(IonChanListPtr theICList);

BOOLN	Free_IonChanList(IonChanListPtr *theICList);

void	FreeIonChannel_IonChanList(IonChannelPtr *theIC);

void	FreeIonChannels_IonChanList(IonChanListPtr theICList);

IonChanListPtr	GenerateDefault_IonChanList(void);

BOOLN	GetParsBoltzmann_IonChanList(IonChannelPtr theIC, FILE *fp);

BOOLN	GetParsHHuxley_IonChanList(IonChannelPtr theIC, FILE *fp);

ICTableEntryPtr	GetTableEntry_IonChanList(IonChannelPtr theIC, double voltage);

void	GenerateBoltzmann_IonChanList(IonChannelPtr theIC);

void	GenerateHHuxley_IonChanList(IonChannelPtr theIC);

IonChanListPtr	Init_IonChanList(const WChar *callingFunctionName);

IonChannelPtr	InitIonChannel_IonChanList(const WChar *callingFunctionName,
				  int numTableEntries);

void	PrintIonChannelPars_IonChanList(IonChannelPtr theIC);

BOOLN	PrintPars_IonChanList(IonChanListPtr theICList);

void	PrintTables_IonChanList(IonChanListPtr theICList);

BOOLN	ReadGeneralPars_IonChanList(FILE *fp, IonChanListPtr theICs);

BOOLN	ReadVoltageTable_IonChanList(IonChannelPtr theIC, FILE *fp);

IonChanListPtr	ReadPars_IonChanList(FILE *fp);

BOOLN	ReadICGeneralPars_IonChanList(FILE **fp, ICModeSpecifier mode,
		  WChar *fileName, WChar *description, WChar *enabled,
		  double *equilibriumPot, double *baseMaxConductance,
		  double *activationExponent);

BOOLN	ReadICPars_IonChanList(IonChanListPtr theICs, IonChannelPtr theIC,
		  FILE *fp);

BOOLN	ResetIonChannel_IonChanList(IonChanListPtr theICs, IonChannelPtr theIC);

double	HHuxleyAlpha_IonChanList(double a, double b, double c, double d,
		  double e, double f, double g, double h, double i, double j, double k,
		  double mV, double mDV, double temperature);

double	HHuxleyBeta_IonChanList(double a, double b, double c, double d,
		  double e, double f, double g, double h, double i, double j, double k,
		  double mV, double mDV, double temperature);

BOOLN	SetBaseLeakageCond_IonChanList(IonChanListPtr theICs,
		  double baseLeakageCond);

BOOLN	PrepareIonChannels_IonChanList(IonChanListPtr theICs);

BOOLN	SetGeneralUniParList_IonChanList(IonChanListPtr theICs);

BOOLN	SetGeneralUniParListMode_CFList(IonChanListPtr theICs);

BOOLN	SetGeneratedPars_IonChanList(IonChanListPtr theICs);

BOOLN	SetICActivationExponent_IonChanList(IonChannelPtr theIC,
		  double theActivationExponent);

BOOLN	SetICBaseMaxConductance_IonChanList(IonChannelPtr theIC,
		  double theBaseMaxConductance);

BOOLN	SetICConductanceQ10_IonChanList(IonChannelPtr theIC,
		  double theConductanceQ10);

BOOLN	SetICDescription_IonChanList(IonChannelPtr theIC,
		  WChar *theDescription);

BOOLN	SetICEnabled_IonChanList(IonChannelPtr theIC, WChar *theICEnabled);

BOOLN	SetICEquilibriumPot_IonChanList(IonChannelPtr theIC,
		  double theEquilibriumPot);

BOOLN	SetICFileName_IonChanList(IonChannelPtr theIC, WChar *fileName);

BOOLN	SetICGeneralPars_IonChanList(IonChannelPtr theIC, ICModeSpecifier mode,
		  WChar *description, WChar *enabled, double equilibriumPot,
		  double baseMaxConductance, double activationExponent);

void	SetICGeneralParsFromICList_IonChanList(IonChannelPtr theIC,
		  IonChanListPtr theICs);

BOOLN	SetICBoltzmannHalfMaxV_IonChanList(IonChannelPtr theIC, int index,
		  double theHalfMaxV);

BOOLN	SetICBoltzmannZ_IonChanList(IonChannelPtr theIC, int index,
		  double theZ);

BOOLN	SetICBoltzmannTau_IonChanList(IonChannelPtr theIC, int index,
		  double theTau);

BOOLN	SetICMode_IonChanList(IonChannelPtr theIC, WChar *modeName);

BOOLN	SetICHHuxleyAlphaA_IonChanList(IonChannelPtr theIC, int index,
		  double value);

BOOLN	SetICHHuxleyAlphaB_IonChanList(IonChannelPtr theIC, int index,
		  double value);

BOOLN	SetICHHuxleyAlphaC_IonChanList(IonChannelPtr theIC, int index,
		  double value);

BOOLN	SetICHHuxleyAlphaD_IonChanList(IonChannelPtr theIC, int index,
		  double value);

BOOLN	SetICHHuxleyAlphaE_IonChanList(IonChannelPtr theIC, int index,
		  double value);

BOOLN	SetICHHuxleyAlphaF_IonChanList(IonChannelPtr theIC, int index,
		  double value);

BOOLN	SetICHHuxleyAlphaG_IonChanList(IonChannelPtr theIC, int index,
		  double value);

BOOLN	SetICHHuxleyAlphaH_IonChanList(IonChannelPtr theIC, int index,
		  double value);

BOOLN	SetICHHuxleyAlphaI_IonChanList(IonChannelPtr theIC, int index,
		  double value);

BOOLN	SetICHHuxleyAlphaJ_IonChanList(IonChannelPtr theIC, int index,
		  double value);

BOOLN	SetICHHuxleyAlphaK_IonChanList(IonChannelPtr theIC, int index,
		  double value);

BOOLN	SetICHHuxleyBetaA_IonChanList(IonChannelPtr theIC, int index,
		  double value);

BOOLN	SetICHHuxleyBetaB_IonChanList(IonChannelPtr theIC, int index,
		  double value);

BOOLN	SetICHHuxleyBetaC_IonChanList(IonChannelPtr theIC, int index,
		  double value);

BOOLN	SetICHHuxleyBetaD_IonChanList(IonChannelPtr theIC, int index,
		  double value);

BOOLN	SetICHHuxleyBetaE_IonChanList(IonChannelPtr theIC, int index,
		  double value);

BOOLN	SetICHHuxleyBetaF_IonChanList(IonChannelPtr theIC, int index,
		  double value);

BOOLN	SetICHHuxleyBetaG_IonChanList(IonChannelPtr theIC, int index,
		  double value);

BOOLN	SetICHHuxleyBetaH_IonChanList(IonChannelPtr theIC, int index,
		  double value);

BOOLN	SetICHHuxleyBetaI_IonChanList(IonChannelPtr theIC, int index,
		  double value);

BOOLN	SetICHHuxleyBetaJ_IonChanList(IonChannelPtr theIC, int index,
		  double value);

BOOLN	SetICHHuxleyBetaK_IonChanList(IonChannelPtr theIC, int index,
		  double value);

void	SetICPowFunc_IonChanList(IonChannelPtr theIC);

BOOLN	SetIonChannelUniParList_IonChanList(IonChanListPtr theICs,
		  IonChannelPtr theIC);

BOOLN	SetIonChannelUniParListMode_IonChanList(IonChannelPtr theIC);

BOOLN	SetLeakageCondQ10_IonChanList(IonChanListPtr theICs,
		  double theLeakageCondQ10);

BOOLN	SetLeakagePot_IonChanList(IonChanListPtr theICs, double leakagePot);

BOOLN	SetMaxVoltage_IonChanList(IonChanListPtr theICs, double theMaxVoltage);

BOOLN	SetMinVoltage_IonChanList(IonChanListPtr theICs, double theMinVoltage);

BOOLN	SetNumChannels_IonChanList(IonChanListPtr theICs, int numChannels);

BOOLN	SetPrintTablesMode_IonChanList(IonChanListPtr theICs, WChar *modeName);

BOOLN	SetTemperature_IonChanList(IonChanListPtr theICs,
		  double theTemperature);

BOOLN	SetVoltageStep_IonChanList(IonChanListPtr theICs,
		  double theVoltageStep);

__END_DECLS

#endif
