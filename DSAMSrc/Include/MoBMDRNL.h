/**********************
 *
 * File:		MoBMDRNL.h
 * Purpose:		The DRNL Composite filter is used to model the response of the
 *				basilar membrane.
 * Comments:	This is an implementation of a digital filter using the
 *				filters.c module.
 *				28-05-97 LPO: Corrected linearFLPCutOffScale parameter - it
 *				wasn't being used, and now a negative or zero value disables
 *				the filter altogether.
 *				06-06-97 LPO: added outputScale parameter.
 *				21-12-98 LPO: Implemented new parameters for filter bank.
 *				These parameters use the broken stick compression - see
 *				UtFilters.
 *				12-01-99 LPO: Implemented universal parameters list.
 * Authors:		L. P. O'Mard
 * Created:		12 Jul 1993
 * Updated:		12 Jan 1999
 * Copyright:	(c) 1999-2000, University of Essex.
 *
 *********************/

#ifndef _MOBMDRNL_H
#define _MOBMDRNL_H 1

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define BM_DRNL_NUM_PARS			12
#define BM_DRNL_NUM_SUB_PROCESSES	1

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef enum {

	BM_DRNL_LINEARF

} BMDRNLSubProcessSpecifier;

typedef enum {

	BM_DRNL_NONLINGTCASCADE,
	BM_DRNL_NONLINLPCASCADE,
	BM_DRNL_NONLINBWIDTH,
	BM_DRNL_COMPRSCALEA,
	BM_DRNL_COMPRSCALEB,
	BM_DRNL_COMPREXPONENT,
	BM_DRNL_LINGTCASCADE,
	BM_DRNL_LINLPCASCADE,
	BM_DRNL_LINCF,
	BM_DRNL_LINBWIDTH,
	BM_DRNL_LINSCALEG,
	BM_DRNL_THECFS

} BMDRNLParSpecifier;

typedef struct {

	ParameterSpecifier	parSpec;

	BOOLN	nonLinGTCascadeFlag, nonLinLPCascadeFlag, comprExponentFlag;
	BOOLN	linGTCascadeFlag, linLPCascadeFlag;
	BOOLN	updateProcessVariablesFlag;
	int		nonLinGTCascade;
	int		nonLinLPCascade;
	ParArrayPtr	nonLinBwidth;
	ParArrayPtr	comprScaleA;
	ParArrayPtr	comprScaleB;
	double	comprExponent;
	int		linGTCascade;
	int		linLPCascade;
	ParArrayPtr	linCF;
	ParArrayPtr	linBwidth;
	ParArrayPtr	linScaleG;
	CFListPtr	theCFs;

	/* Private members */
	NameSpecifier	*fitFuncModeList;
	UniParListPtr	parList;
	int		numChannels;
	double	*compressionA;
	double	*compressionB;
	GammaToneCoeffsPtr	*nonLinearGT1;
	GammaToneCoeffsPtr	*nonLinearGT2;
	GammaToneCoeffsPtr	*linearGT;
	ContButtCoeffsPtr	*nonLinearLP;
	ContButtCoeffsPtr	*linearLP;	
	EarObjectPtr	linearF;			/* Extra signal for linear filter. */

} BMDRNL, *BMDRNLPtr;

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

extern	BMDRNLPtr	bMDRNLPtr;

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
* macros, to allow the safe use of C libraries with C++ libraries - defined
* in GeCommon.h.
*/
__BEGIN_DECLS

void	ApplyScale_BasilarM_DRNL(EarObjectPtr data, SignalDataPtr signal,
		  ParArrayPtr p);

BOOLN	CheckData_BasilarM_DRNL(EarObjectPtr data);

BOOLN	CheckPars_BasilarM_DRNL(void);

BOOLN	Free_BasilarM_DRNL(void);

BOOLN	FreeProcessVariables_BasilarM_DRNL(void);

CFListPtr	GetCFListPtr_BasilarM_DRNL(void);

double	GetFitFuncValue_BasilarM_DRNL(ParArrayPtr p, double linCF);

double	GetNonLinBandwidth_BasilarM_DRNL(BandwidthModePtr modePtr,
		  double theCF);

int	GetNumFitFuncPars_BasilarM_DRNL(int mode);

UniParListPtr	GetUniParListPtr_BasilarM_DRNL(void);

BOOLN	InitProcessVariables_BasilarM_DRNL(EarObjectPtr data);

BOOLN	Init_BasilarM_DRNL(ParameterSpecifier parSpec);

BOOLN	PrintPars_BasilarM_DRNL(void);

BOOLN	ReadPars_BasilarM_DRNL(WChar *fileName);

BOOLN	RunModel_BasilarM_DRNL(EarObjectPtr data);

BOOLN	SetCFList_BasilarM_DRNL(CFListPtr theCFList);

BOOLN	SetComprExponent_BasilarM_DRNL(double theComprExponent);

BOOLN	SetComprScaleA_BasilarM_DRNL(ParArrayPtr theComprScaleA);

BOOLN	SetComprScaleB_BasilarM_DRNL(ParArrayPtr theComprScaleB);

BOOLN	SetLinBwidth_BasilarM_DRNL(ParArrayPtr theLinBwidth);

BOOLN	SetLinCF_BasilarM_DRNL(ParArrayPtr theLinCF);

BOOLN	SetLinGTCascade_BasilarM_DRNL(int theLinGTCascade);

BOOLN	SetLinLPCascade_BasilarM_DRNL(int theLinLPCascade);

BOOLN	SetLinScaleG_BasilarM_DRNL(ParArrayPtr theLinScaleG);

BOOLN	InitModule_BasilarM_DRNL(ModulePtr theModule);

BOOLN	SetNonLinBwidth_BasilarM_DRNL(ParArrayPtr theNonLinBwidth);

BOOLN	SetNonLinGTCascade_BasilarM_DRNL(int theNonLinGTCascade);

BOOLN	SetNonLinLPCascade_BasilarM_DRNL(int theNonLinLPCascade);

BOOLN	SetParsPointer_BasilarM_DRNL(ModulePtr theModule);

BOOLN	SetPars_BasilarM_DRNL(int nonLinGTCascade, int nonLinLPCascade,
		  ParArrayPtr nonLinBwidth, ParArrayPtr comprScaleA,
		  ParArrayPtr comprScaleB, double comprExponent, int linGTCascade,
		  int linLPCascade, ParArrayPtr linCF, ParArrayPtr linBwidth,
		  ParArrayPtr linScaleG, CFListPtr theCFs);

BOOLN	SetUniParList_BasilarM_DRNL(void);

__END_DECLS

#endif
