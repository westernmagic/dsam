/******************
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
 * Copyright:	(c) 1999, University of Essex.
 * 
 ******************/

#ifndef	_MOBMDRNL_H
#define _MOBMDRNL_H	1

/******************************************************************************/
/*************************** Constant definitions *****************************/
/******************************************************************************/

#define BASILARM_DRNL_NUM_PARS	6
#define	DRNL_LIN_LP_APPLN		2		/* No. of times the filter is applied.*/

/******************************************************************************/
/*************************** Macro definitions ********************************/
/******************************************************************************/

#define	DRNL_QUAD_FUNC(F, A, B, C)	((A) + (B) * (F) + (C)* (F) * (F))

#define	DRNL_NON_LIN_COMP_A(F)	DRNL_QUAD_FUNC(F, 35, -0.04, 2.85e-5)

#define	DRNL_NON_LIN_COMP_B(F)	DRNL_QUAD_FUNC(F, 2.71e-3, 2.01e-6, -9.83e-11)

#define	DRNL_LIN_CF(F)			DRNL_QUAD_FUNC(F, 355.0, 0.3454, 3.182e-5)

#define	DRNL_LIN_WIDTH(F)		DRNL_QUAD_FUNC(F, 31.94, 0.2134, -1.0657e-5)

#define	DRNL_LIN_ATTEN(F)		DRNL_QUAD_FUNC(F, 30.19, -0.00447, 4.384e-7)

#define	DRNL_LIN_LP(F)			DRNL_QUAD_FUNC(F, 0.8867, 0.000291, -1.816e-8)

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

typedef enum {

	BASILARM_DRNL_ORDER,
	BASILARM_DRNL_NONLINEARBROADENINGCOEFF,
	BASILARM_DRNL_COMPRESSIONPOWER,
	BASILARM_DRNL_LINEARBROADENINGCOEFF,
	BASILARM_DRNL_OUTPUTSCALE,
	BASILARM_DRNL_THECFS

} BMDRNLParSpecifier;

typedef struct {

	ParameterSpecifier parSpec;
	
	BOOLN	orderFlag;
	BOOLN	nonLinearBroadeningCoeffFlag, compressionPowerFlag, outputScaleFlag;
	BOOLN	linearBroadeningCoeffFlag;
	BOOLN	updateProcessVariablesFlag;
	int		order;					/* Total (nonLinear filter) line order. */
	double	nonLinearBroadeningCoeff;	/* NonLinear broadening coefficient */
	double	compressionPower;		/* - for broken stick compression. */
	double	linearBroadeningCoeff;	/* Linear broadening coefficient */
	double	outputScale;			/* - scales to experimental data. */
	CFListPtr		theCFs;			/* Pointer to centre frequency structure. */
	
	/* Private Members */
	UniParListPtr	parList;
	int		numChannels;
	double	*attenuationdB;
	double	*compressionA;
	double	*compressionB;
	GammaToneCoeffsPtr	*nonLinearGT1;
	GammaToneCoeffsPtr	*nonLinearGT2;
	GammaToneCoeffsPtr	*linearGT;
	TwoPoleCoeffsPtr	*linearLP;
	EarObjectPtr	linearF;			/* Extra signal for linear filter. */

} BMDRNL, *BMDRNLPtr;

/******************************************************************************/
/*************************** External Variables *******************************/
/******************************************************************************/

extern	BMDRNLPtr	bMDRNLPtr;

/******************************************************************************/
/*************************** Function Prototypes ******************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

BOOLN	CheckPars_BasilarM_DRNL(void);

CFListPtr	GetCFListPtr_BasilarM_DRNL(void);

UniParListPtr	GetUniParListPtr_BasilarM_DRNL(void);

BOOLN	Init_BasilarM_DRNL(ParameterSpecifier parSpec);

BOOLN	InitProcessVariables_BasilarM_DRNL(EarObjectPtr data);

BOOLN	Free_BasilarM_DRNL(void);

BOOLN	FreeProcessVariables_BasilarM_DRNL(void);

BOOLN	PrintPars_BasilarM_DRNL(void);

BOOLN	ReadPars_BasilarM_DRNL(char *fileName);

BOOLN	RunModel_BasilarM_DRNL(EarObjectPtr data);

BOOLN	SetBandwidths_BasilarM_DRNL(char *theBandwidthMode,
		  double *theBandwidths);

BOOLN	SetCFList_BasilarM_DRNL(CFList *theCFList);

BOOLN	SetCompressionPower_BasilarM_DRNL(double theCompressionPower);

BOOLN	SetNonLinearBroadeningCoeff_BasilarM_DRNL(double 
		  theNonLinearBroadeningCoeff);

BOOLN	SetOrder_BasilarM_DRNL(int theOrder);

BOOLN	SetOutputScale_BasilarM_DRNL(double theOutputScale);

BOOLN	SetPars_BasilarM_DRNL(int theOrder, double theNonLinearBroadeningCoeff,
		  double theCompressionPower, double linearBroadeningCoeff,
		  CFListPtr theCFs, double outputScale);
		  
BOOLN	SetLinearBroadeningCoeff_BasilarM_DRNL(double theLinearBroadeningCoeff);

BOOLN	SetUniParList_BasilarM_DRNL(void);

__END_DECLS

#endif