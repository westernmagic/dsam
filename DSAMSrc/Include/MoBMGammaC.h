/**********************
 *
 * File:		MoBMGammaC.h
 * Purpose:		
 * Comments:	Written using ModuleProducer version 1.2.10 (Oct  5 2000).
 * Authors:		Masashi Unoki and L. P. O'Mard
 * Created:		06 Oct 2000
 * Updated:		07 Dec 2000
 * Copyright:	(c) 2000, CNBH 
 *
 *********************/

#ifndef _MOBMGAMMAC_H
#define _MOBMGAMMAC_H 1

#include "UtNameSpecs.h"

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define BASILARM_GAMMACHIRP_NUM_PARS				9
#define BASILARM_GAMMACHIRP_COEFF_PS_EST			0.65	/* CoefPsEst */
#define BASILARM_GAMMACHIRP_COEFF_CMPRS				1.0	/* Cmprs     */
#define BASILARM_GAMMACHIRP_DELAY_TIME_LI			100.0	/* T_LI	[ms] */	

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef enum {

	BASILARM_GAMMACHIRP_DIAGNOSTICMODE,
	BASILARM_GAMMACHIRP_OPMODE,
	BASILARM_GAMMACHIRP_CASCADE,
	BASILARM_GAMMACHIRP_BCOEFF,
	BASILARM_GAMMACHIRP_CCOEFF0,
	BASILARM_GAMMACHIRP_CCOEFF1,
	BASILARM_GAMMACHIRP_CLOWERLIM,
	BASILARM_GAMMACHIRP_CUPPERLIM,
	BASILARM_GAMMACHIRP_THECFS

} BMGammaCParSpecifier;

typedef enum {

	BASILARM_GAMMACHIRP_OPMODE_FEEDBACK,
	BASILARM_GAMMACHIRP_OPMODE_FEEDFORWARD,
	BASILARM_GAMMACHIRP_OPMODE_NOCONTROL,
	BASILARM_GAMMACHIRP_OPMODE_NULL

} BasilarMOpModeSpecifier;

typedef struct {

	ParameterSpecifier	parSpec;

	BOOLN	diagnosticModeFlag, opModeFlag, cascadeFlag;
	BOOLN	bCoeffFlag, cCoeff0Flag, cCoeff1Flag;
	BOOLN	cLowerLimFlag, cUpperLimFlag;
	BOOLN	updateProcessVariablesFlag;
	int		diagnosticMode;
	int		opMode;		/* SwCntl */
	int		cascade;	
	double	bCoeff;
	double	cCoeff0, cCoeff1;	/* cEst=cCoeff0+cCoeff1*PsEst */
	double	cLowerLim;
	double	cUpperLim;
	CFListPtr	theCFs;

	/* Private members */
	NameSpecifier			*diagnosticModeList;
	NameSpecifier			*opModeList;
	UniParListPtr			parList;
	char					diagnosticString[MAX_FILE_PATH];	
	int						numChannels;
	GammaToneCoeffsPtr		*coefficientsGT;	/* Gammatone coeffs (OZ)  */
	ERBGammaToneCoeffsPtr	*coefficientsERBGT;	/* Gammatone coeffs (ERB) */
	AsymCmpCoeffsPtr		*coefficientsAC;	/* AsymCmpF coeffs  */
	OnePoleCoeffsPtr		*coefficientsLI;	/* LeakyInt coeffs  */
	double					*winPsEst;			/* Weighting func.  */
	double					coefPsEst;			/* PsEst coeff      */
	double					cmprs;				/* compression coef */
	double					delaytimeLI;		/* DelayTime of LI  */
	CntlGammaCPtr			*cntlGammaC;		/* Control param.   */

} BMGammaC, *BMGammaCPtr;

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

extern	BMGammaCPtr	bMGammaCPtr;

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
* macros, to allow the safe use of C libraries with C++ libraries - defined
* in GeCommon.h.
*/
__BEGIN_DECLS

BOOLN	CheckData_BasilarM_GammaChirp(EarObjectPtr data);

BOOLN	CheckPars_BasilarM_GammaChirp(void);

void	FreeProcessVariables_BasilarM_GammaChirp(void);

BOOLN	Free_BasilarM_GammaChirp(void);

CFListPtr	GetCFListPtr_BasilarM_GammaChirp(void);

UniParListPtr	GetUniParListPtr_BasilarM_GammaChirp(void);

BOOLN	InitDiagnosticModeList_BasilarM_GammaChirp(void);

BOOLN	InitOpModeList_BasilarM_GammaChirp(void);

BOOLN	InitProcessVariables_BasilarM_GammaChirp(EarObjectPtr data);

BOOLN	Init_BasilarM_GammaChirp(ParameterSpecifier parSpec);

BOOLN	PrintPars_BasilarM_GammaChirp(void);

BOOLN	ReadPars_BasilarM_GammaChirp(char *fileName);

BOOLN	RunModel_BasilarM_GammaChirp(EarObjectPtr data);

BOOLN	SetBCoeff_BasilarM_GammaChirp(double theBCoeff);

BOOLN	SetBandWidths_BasilarM_GammaChirp(char *theBandwidthMode,
		  double *theBandwidths);

BOOLN	SetCCoeff0_BasilarM_GammaChirp(double theCCoeff0);

BOOLN	SetCCoeff1_BasilarM_GammaChirp(double theCCoeff1);

BOOLN	SetCFList_BasilarM_GammaChirp(CFListPtr theCFList);

BOOLN	SetCLowerLim_BasilarM_GammaChirp(double theCLowerLim);

BOOLN	SetCUpperLim_BasilarM_GammaChirp(double theCUpperLim);

BOOLN	SetCascade_BasilarM_GammaChirp(int theCascade);

BOOLN	SetDiagnosticMode_BasilarM_GammaChirp(char * theDiagnosticMode);

BOOLN	InitModule_BasilarM_GammaChirp(ModulePtr theModule);

BOOLN	SetOpMode_BasilarM_GammaChirp(char * theOpMode);

BOOLN	SetParsPointer_BasilarM_GammaChirp(ModulePtr theModule);

BOOLN	SetPars_BasilarM_GammaChirp(char * diagnosticMode, char * opMode,
		  int cascade, double bCoeff, double cCoeff0, double cCoeff1,
		  double cLowerLim, double cUpperLim, CFListPtr theCFs);

BOOLN	SetUniParList_BasilarM_GammaChirp(void);

__END_DECLS

#endif
