/**********************
 *
 * File:		UtIonChanList.h
 * Purpose:		This module generates the ion channel tables list, reading the
 *				tables from file or generating them as required.
 * Comments:	Written using ModuleProducer version 1.10 (Oct 13 1996).
 *				13-01-97: LPO: Introduce Rothman Channel list mode.
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

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define ICLIST_NUM_CONSTANT_PARS	5
#define ICLIST_IC_NUM_CONSTANT_PARS	4
#define	E_OVER_R					11.8e3
#define	ABSOLUTE_ZERO_DEGREES		-273.0
#define	ICLIST_NUM_GATES			2

/******************************************************************************/
/****************************** Macro definitions *****************************/
/******************************************************************************/

#define ROTHMAN_ALL_ZERO_PARS(A, B, C, D, E, F, G, H, I, J, K) ((fabs(A) + \
		  fabs(B) + fabs(C) + fabs(D) + fabs(E) + fabs(F) + fabs(G) + \
		  fabs(H) + fabs(I) + fabs(J) + fabs(K)) <= DBL_EPSILON)
		  
#define ROTHMAN_TF(A, T)	(pow(A, ((T) - 22) / 10))

#define SET_IC_GATE_ARRAY(GATE)	((GATE).ptr = (GATE).array)

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

/*
 * Ion channel modes.
 */

typedef enum {

	ICLIST_MANIS_MODE,
	ICLIST_ROTHMAN_MODE,
	ICLIST_USER_MODE,
	ICLIST_NULL

} ICListSpecifier;

typedef enum {

	ICLIST_PRINTTABLESMODE,
	ICLIST_BASELEAKAGECOND,
	ICLIST_LEAKAGEPOT,
	ICLIST_MODE,
	ICLIST_NUMCHANNELS,
	ICLIST_PAR_1,
	ICLIST_PAR_2

} ICListParSpecifier;

typedef enum {

	ICLIST_IC_DESCRIPTION,
	ICLIST_IC_EQUILIBRIUM_POT,
	ICLIST_IC_CONDUCTANCE,
	ICLIST_IC_ACTIVATION_EXPONENT,
	ICLIST_IC_PAR_1,
	ICLIST_IC_PAR_2,
	ICLIST_IC_PAR_3,
	ICLIST_IC_PAR_4,
	ICLIST_IC_PAR_5,
	ICLIST_IC_PAR_6,
	ICLIST_IC_PAR_7,
	ICLIST_IC_PAR_8,
	ICLIST_IC_PAR_9,
	ICLIST_IC_PAR_10,
	ICLIST_IC_PAR_11,
	ICLIST_IC_PAR_12,
	ICLIST_IC_PAR_13,
	ICLIST_IC_PAR_14,
	ICLIST_IC_PAR_15,
	ICLIST_IC_PAR_16,
	ICLIST_IC_PAR_17,
	ICLIST_IC_PAR_18,
	ICLIST_IC_PAR_19,
	ICLIST_IC_PAR_20,
	ICLIST_IC_PAR_21,
	ICLIST_IC_PAR_22,
	ICLIST_IC_PAR_23,
	ICLIST_IC_PAR_24,
	ICLIST_IC_PAR_25

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
	
} ICRothmanPars, *ICRothmanParsPtr;	

typedef	struct {

	ICGateArray	halfMaxV;
	ICGateArray	zZ;
	ICGateArray	tau;
	
} ICManisPars, *ICManisParsPtr;
	
typedef struct {

	BOOLN	updateFlag;
	char	description[MAXLINE];
	int		mode;
	int		numTableEntries;
	int		activationExponent;
	double	equilibriumPot;
	double	temperature;
	double	baseMaxConductance, maxConductance, conductanceQ10;
	union {
	
		ICManisPars		manis;
		ICRothmanPars	rothman;
		char			fileName[MAXLINE];
	
	} pars;
	double	minVoltage;
	double	maxVoltage;
	double	dV;
	ICTableEntry	*table;
	UniParListPtr	parList;
	
} IonChannel, *IonChannelPtr;

typedef struct IonChanList {

	BOOLN	updateFlag;
	int		mode;
	int		printTablesMode;
	double	temperature;
	double	baseLeakageCond, leakageCond, leakageCondQ10;
	double	leakagePot;
	int		numChannels;
	IonChannel	*ionChannels;

	/* Private members */
	struct IonChanList **parentPtr;
	NameSpecifier	*printTablesModeList;
	UniParListPtr	parList;
	char	diagFileName[MAXLINE];

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
		  const char *callingFunction);

BOOLN	CheckPars_IonChanList(IonChanListPtr theICList);

BOOLN	Free_IonChanList(IonChanListPtr *theICList);

void	FreeIonChannel_IonChanList(IonChannelPtr theIC);

int		GetNumParsGeneral_IonChanList(int mode);

int		GetNumParsIonChannels_IonChanList(int mode);

BOOLN	GetParsManis_IonChanList(IonChanListPtr theICs, FILE *fp);

BOOLN	GetParsRothman_IonChanList(IonChanListPtr theICs, FILE *fp);

BOOLN	GetParsUser_IonChanList(IonChanListPtr theICs, FILE *fp);

ICTableEntryPtr	GetTableEntry_IonChanList(IonChannelPtr theIC, double voltage);

BOOLN	GetTablePars_IonChanList(IonChanListPtr theICs, FILE *fp);

void	GenerateManis_IonChanList(IonChanListPtr theICs);

void	GenerateRothman_IonChanList(IonChanListPtr theICs);

IonChanListPtr	Init_IonChanList(char *callingFunctionName);

BOOLN	SetIonChannel_IonChanList(IonChanListPtr theICList, int index);

void	PrintIonChannelPars_IonChanList(IonChanListPtr theICList, int index);

BOOLN	PrintPars_IonChanList(IonChanListPtr theICList);

void	PrintTables_IonChanList(IonChanListPtr theICList);

BOOLN	ReadIonChannel_IonChanList(IonChanListPtr theICs, int index,
		  char *fileName);

IonChanListPtr	ReadPars_IonChanList(FILE *fp);

BOOLN	ResetIonChannel_IonChanList(IonChanListPtr theICList, uInt index);

double	RothmanAlpha_IonChanList(double a, double b, double c, double d,
		  double e, double f, double g, double h, double i, double j, double k,
		  double mV, double mDV, double temperature);

double	RothmanBeta_IonChanList(double a, double b, double c, double d,
		  double e, double f, double g, double h, double i, double j, double k,
		  double mV, double mDV, double temperature);

BOOLN	SetBaseLeakageCond_IonChanList(IonChanListPtr theICs,
		  double baseLeakageCond);

BOOLN	SetGeneralUniParList_IonChanList(IonChanListPtr theICs);

BOOLN	SetICActivationExponent_IonChanList(IonChannelPtr theIC,
		  int theActivationExponent);

BOOLN	SetICBaseMaxConductance_IonChanList(IonChannelPtr theIC,
		  double theBaseMaxConductance);

BOOLN	SetICDescription_IonChanList(IonChannelPtr theIC, char *theDescription);

BOOLN	SetICEquilibriumPot_IonChanList(IonChannelPtr theIC,
		  double theEquilibriumPot);

BOOLN	SetICFileName_IonChanList(IonChannelPtr theIC, char *fileName);

BOOLN	SetICManisHalfMaxV_IonChanList(IonChannelPtr theIC, int index,
		  double theHalfMaxV);

BOOLN	SetICManisZ_IonChanList(IonChannelPtr theIC, int index, double theZ);

BOOLN	SetICManisTau_IonChanList(IonChannelPtr theIC, int index,
		  double theTau);

BOOLN	SetICMaxVoltage_IonChanList(IonChannelPtr theIC, double theMaxVoltage);

BOOLN	SetICMinVoltage_IonChanList(IonChannelPtr theIC, double theMinVoltage);

BOOLN	SetICRothmanAlphaA_IonChanList(IonChannelPtr theIC, int index,
		  double value);

BOOLN	SetICRothmanAlphaB_IonChanList(IonChannelPtr theIC, int index,
		  double value);

BOOLN	SetICRothmanAlphaC_IonChanList(IonChannelPtr theIC, int index,
		  double value);

BOOLN	SetICRothmanAlphaD_IonChanList(IonChannelPtr theIC, int index,
		  double value);

BOOLN	SetICRothmanAlphaE_IonChanList(IonChannelPtr theIC, int index,
		  double value);

BOOLN	SetICRothmanAlphaF_IonChanList(IonChannelPtr theIC, int index,
		  double value);

BOOLN	SetICRothmanAlphaG_IonChanList(IonChannelPtr theIC, int index,
		  double value);

BOOLN	SetICRothmanAlphaH_IonChanList(IonChannelPtr theIC, int index,
		  double value);

BOOLN	SetICRothmanAlphaI_IonChanList(IonChannelPtr theIC, int index,
		  double value);

BOOLN	SetICRothmanAlphaJ_IonChanList(IonChannelPtr theIC, int index,
		  double value);

BOOLN	SetICRothmanAlphaK_IonChanList(IonChannelPtr theIC, int index,
		  double value);

BOOLN	SetICRothmanBetaA_IonChanList(IonChannelPtr theIC, int index,
		  double value);

BOOLN	SetICRothmanBetaB_IonChanList(IonChannelPtr theIC, int index,
		  double value);

BOOLN	SetICRothmanBetaC_IonChanList(IonChannelPtr theIC, int index,
		  double value);

BOOLN	SetICRothmanBetaD_IonChanList(IonChannelPtr theIC, int index,
		  double value);

BOOLN	SetICRothmanBetaE_IonChanList(IonChannelPtr theIC, int index,
		  double value);

BOOLN	SetICRothmanBetaF_IonChanList(IonChannelPtr theIC, int index,
		  double value);

BOOLN	SetICRothmanBetaG_IonChanList(IonChannelPtr theIC, int index,
		  double value);

BOOLN	SetICRothmanBetaH_IonChanList(IonChannelPtr theIC, int index,
		  double value);

BOOLN	SetICRothmanBetaI_IonChanList(IonChannelPtr theIC, int index,
		  double value);

BOOLN	SetICRothmanBetaJ_IonChanList(IonChannelPtr theIC, int index,
		  double value);

BOOLN	SetICRothmanBetaK_IonChanList(IonChannelPtr theIC, int index,
		  double value);

BOOLN	SetICVoltageStep_IonChanList(IonChannelPtr theIC,
		  double theVoltageStep);

BOOLN	SetIonChannelUniParList_IonChanList(IonChanListPtr theICs, int index);

BOOLN	SetLeakageCondQ10_IonChanList(IonChanListPtr theICs,
		  int theLeakageCondQ10);

BOOLN	SetLeakagePot_IonChanList(IonChanListPtr theICs, double leakagePot);

BOOLN	SetMode_IonChanList(IonChanListPtr theICs, char *modeName);

BOOLN	SetNumChannels_IonChanList(IonChanListPtr theICs, int numChannels);

BOOLN	SetParentICList_IonChanList(IonChanListPtr *iCPtr,
		  IonChanListPtr theICList);

BOOLN	SetPrintTablesMode_IonChanList(IonChanListPtr theICs, char *modeName);

BOOLN	SetTemperature_IonChanList(IonChanListPtr theICs,
		  double theTemperature);

__END_DECLS

#endif
