/**********************
 *
 * File:		MoHCRPShamma.h
 * Purpose:		This module contains the model for the Shamme hair cell receptor
 *				potential: Shamm, S. A. Chadwick R. S. Wilbur W. J. Morrish
 *				K. A. and Rinzel J.(1986) "A biophysical model oc cochlear
 *				processing: Intensity dependence of pure tone responses",
 *				J. Acoust. Soc. Am. [80], pp 133-145.
 * Comments:	The program uses a mixture of "hard" and "generic" programming:
 *				The test module is used in "hard" programming mode for testing
 *				purposes.
 * Author:		L. P. O'Mard
 * Created:		18 Feb 1993
 * Updated:	    13 Sep 1996
 * Copyright:	(c) 1998, University of Essex.
 *
 **********************/

#ifndef	_MOHCRPSHAMMA_H
#define _MOHCRPSHAMMA_H	1
 
/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

#define IHCRP_SHAMMA_NUM_PARS			12

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

typedef enum {

	IHCRP_SHAMMA_ENDOCOCHLEARPOT_ET,
	IHCRP_SHAMMA_REVERSALPOT_EK,
	IHCRP_SHAMMA_REVERSALPOTCORRECTION,
	IHCRP_SHAMMA_TOTALCAPACITANCE_C,
	IHCRP_SHAMMA_RESTINGCONDUCTANCE_G0,
	IHCRP_SHAMMA_KCONDUCTANCE_GK,
	IHCRP_SHAMMA_MAXMCONDUCTANCE_GMAX,
	IHCRP_SHAMMA_BETA,
	IHCRP_SHAMMA_GAMMA,
	IHCRP_SHAMMA_CILIATIMECONST_TC,
	IHCRP_SHAMMA_CILIACOUPLINGGAIN_C,
	IHCRP_SHAMMA_REFERENCEPOT

} ShammaParSpecifier;

typedef struct {

	ParameterSpecifier parSpec;
	
	BOOLN	endocochlearPotEtFlag, reversalPotEkFlag;
	BOOLN	reversalPotCorrectionFlag, totalCapacitanceCFlag;
	BOOLN	restingConductanceG0Flag, kConductanceGkFlag;
	BOOLN	maxMConductanceGmaxFlag, betaFlag, gammaFlag;
	BOOLN	ciliaTimeConstTcFlag, ciliaCouplingGainFlag, referencePotFlag;
	BOOLN	updateProcessVariablesFlag;

	double	endocochlearPot_Et;	/* Endocochlear potential, mV. */
	double	reversalPot_Ek;		/* Reversal potential, mV. */
	double	reversalPotCorrection;	/* Rp/(Rt+Rp), mV. */
	double	totalCapacitance_C;	/* Total capacitance C = Ca + Cb, pF. */
	double	restingConductance_G0;	/* Resting Conductance, G0. */
	double	kConductance_Gk;	/* Potassium conductance, S (Seimens). */
	double	maxMConductance_Gmax;	/* Maximum mechanical conductance, S. */
	double	beta;				/* beta = exp(-G1/RT), dimensionless. */
	double	gamma;				/* gamma = Z1/RT, u/m. */
	double	ciliaTimeConst_tc;	/* BM/cilia displacement time constant, s. */
	double	ciliaCouplingGain_C;/* Cilia coupling gain. */
	double	referencePot;		/* Reference potential */

	/* Private members */
	UniParListPtr	parList;
	double	*lastCiliaDisplacement_u, *lastInput, *lastOutput;
	double	leakageConductance_Ga, dtOverC, gkEpk, dtOverTc, cGain, max_u;
	
} Shamma, *ShammaPtr;

/******************************************************************************/
/*************************** External Variables *******************************/
/******************************************************************************/

extern	ShammaPtr	shammaPtr;

/******************************************************************************/
/*************************** Function Prototypes ******************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

BOOLN	CheckPars_IHCRP_Shamma(void);

BOOLN	Free_IHCRP_Shamma(void);

void	FreeProcessVariables_IHCRP_Shamma(void);

UniParListPtr	GetUniParListPtr_IHCRP_Shamma(void);

BOOLN	Init_IHCRP_Shamma(ParameterSpecifier parSpec);

BOOLN	InitProcessVariables_IHCRP_Shamma(EarObjectPtr data);

BOOLN	PrintPars_IHCRP_Shamma(void);

BOOLN	ReadPars_IHCRP_Shamma(char *fileName);

BOOLN	RunModel_IHCRP_Shamma(EarObjectPtr data);

BOOLN	SetBeta_IHCRP_Shamma(double theBeta);

BOOLN	SetCiliaCouplingGain_IHCRP_Shamma(double theCiliaCouplingGain);

BOOLN	SetCiliaTimeConstTc_IHCRP_Shamma(double theCiliaTimeConstTc);

BOOLN	SetEndocochlearPot_IHCRP_Shamma(double theEndocochlearPot);

BOOLN	SetGamma_IHCRP_Shamma(double theGamma);

BOOLN	SetKConductance_IHCRP_Shamma(double theKConductance);

BOOLN	SetMaxMConductance_IHCRP_Shamma(double theMaxMConductance);

BOOLN	InitModule_IHCRP_Shamma(ModulePtr theModule);

BOOLN	SetParsPointer_IHCRP_Shamma(ModulePtr theModule);

BOOLN	SetPars_IHCRP_Shamma(double Et, double Ek,
		  double reversalPotCorrection, double C, double G0, double Gk,
		  double Gmax, double beta, double gamma, double tc, double n, 
		  double Eref);

BOOLN	SetReferencePot_IHCRP_Shamma(double theReferencePot);

BOOLN	SetRestingConductance_IHCRP_Shamma(double theRestingConductance);

BOOLN	SetReversalPot_IHCRP_Shamma(double theReversalPot);

BOOLN	SetReversalPotCorrection_IHCRP_Shamma(double
		  theReversalPotCorrection);
		  
BOOLN	SetTotalCapacitance_IHCRP_Shamma(double theTotalCapacitance);

BOOLN	SetUniParList_IHCRP_Shamma(void);

__END_DECLS

#endif
