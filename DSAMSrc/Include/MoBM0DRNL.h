/******************
 *
 * File:		MoBM0DRNL.h
 * Purpose:		Test module for the DRNL Composite filter, used to model the
 *				non-linear response of the Basilar Membrane.
 *				28-05-97 LPO: Corrected linearFLPCutOffScale parameter - it
 *				wasn't being used, and now a negative or zero value disables
 *				the filter altogether.
 *				06-06-97 LPO: added outputScale parameter.
 *				11-06-97 LPO: Introduced new compression function.
 *				27-05-98 LPO: Introduced facility to use different compression
 *				modes.
 *				04-10-98 LPO: Introduced the linear filter order parameter.
 *				11-11-98 LPO: Implemented universal parameter list.  The
 *				compressionPars array had to be done by hand.
 *				23-04-99 LPO: The 'compressionParsFlag' arrays was not being
 *				correctly initialised to 'NULL' in "Init_".
 * Authors:		L. P. O'Mard
 * Created:		11 Mar 1993
 * Updated:		23 Apr 1999
 * Copyright:	(c) 1999, University of Essex.
 * 
 ******************/

#ifndef	_MOBM0DRNL_H
#define _MOBM0DRNL_H	1

#include "UtNameSpecs.h"

/******************************************************************************/
/*************************** Constant definitions *****************************/
/******************************************************************************/

#define BASILARM_DRNL_TEST_NUM_PARS	10

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

typedef enum {

	BASILARM_DRNL_TEST_NONLINGTCASCADE,
	BASILARM_DRNL_TEST_NONLINLPCASCADE,
	BASILARM_DRNL_TEST_COMPRESSIONMODE,
	BASILARM_DRNL_TEST_COMPRESSIONPARS,
	BASILARM_DRNL_TEST_LINGTCASCADE,
	BASILARM_DRNL_TEST_LINLPCASCADE,
	BASILARM_DRNL_TEST_LINCF,
	BASILARM_DRNL_TEST_LINBWIDTH,
	BASILARM_DRNL_TEST_LINSCALER,
	BASILARM_DRNL_TEST_THECFS

} BM0DRNLParSpecifier;

typedef enum {

	DRNLT_COMPRESSION_MODE_ORIGINAL,		
	DRNLT_COMPRESSION_MODE_INVPOWER,
	DRNLT_COMPRESSION_MODE_BROKENSTICK1,
	DRNLT_COMPRESSION_MODE_UPTON_BSTICK,
	DRNLT_COMPRESSION_MODE_NULL

} CompressionModeSpecifier;

typedef struct {

	ParameterSpecifier parSpec;
	
	BOOLN	nonLinGTCascadeFlag, nonLinLPCascadeFlag, compressionModeFlag;
	BOOLN	*compressionParsFlag, linGTCascadeFlag, linLPCascadeFlag, linCFFlag;
	BOOLN	linBwidthFlag, linScalerFlag;
	BOOLN	updateProcessVariablesFlag;
	int		nonLinGTCascade;
	int		nonLinLPCascade;
	int		compressionMode;
	double	*compressionPars;
	int		linGTCascade;
	int		linLPCascade;
	double	linCF;
	double	linBwidth;
	double	linScaler;
	CFListPtr	theCFs;

	/* Private Members */
	NameSpecifier *compressionModeList;
	UniParListPtr	parList;
	int		numChannels;
	int		numCompressionPars;
	GammaToneCoeffsPtr	*nonLinearGT1;
	GammaToneCoeffsPtr	*nonLinearGT2;
	GammaToneCoeffsPtr	*linearGT;
	TwoPoleCoeffsPtr	*nonLinearLP;
	TwoPoleCoeffsPtr	*linearLP;
	EarObjectPtr	linearF;			/* Extra signal for linear filter. */

} BM0DRNL, *BM0DRNLPtr;

/******************************************************************************/
/*************************** External Variables *******************************/
/******************************************************************************/
extern	BM0DRNLPtr	bM0DRNLPtr;

/******************************************************************************/
/*************************** Function Prototypes ******************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

BOOLN	CheckData_BasilarM_DRNL_Test(EarObjectPtr data);

BOOLN	CheckPars_BasilarM_DRNL_Test(void);

BOOLN	Free_BasilarM_DRNL_Test(void);

void	FreeProcessVariables_BasilarM_DRNL_Test(void);

CFListPtr	GetCFListPtr_BasilarM_DRNL_Test(void);

UniParListPtr	GetUniParListPtr_BasilarM_DRNL_Test(void);

BOOLN	Init_BasilarM_DRNL_Test(ParameterSpecifier parSpec);

BOOLN	InitCompressionModeList_BasilarM_DRNL_Test(void);

BOOLN	InitProcessVariables_BasilarM_DRNL_Test(EarObjectPtr data);

BOOLN	PrintPars_BasilarM_DRNL_Test(void);

BOOLN	ReadPars_BasilarM_DRNL_Test(char *fileName);

BOOLN	RunModel_BasilarM_DRNL_Test(EarObjectPtr data);

BOOLN	SetBandWidths_BasilarM_DRNL_Test(char *theBandwidthMode,
		  double *theBandwidths);

BOOLN	SetCFList_BasilarM_DRNL_Test(CFListPtr theCFList);

BOOLN	SetCompressionMode_BasilarM_DRNL_Test(char * theCompressionMode);

BOOLN	SetCompressionPar_BasilarM_DRNL_Test(int index, double parameterValue);

BOOLN	SetCompressionParsArray_BasilarM_DRNL_Test(int mode);

BOOLN	SetLinBwidth_BasilarM_DRNL_Test(double theLinBwidth);

BOOLN	SetLinCF_BasilarM_DRNL_Test(double theLinCF);

BOOLN	SetLinGTCascade_BasilarM_DRNL_Test(int theLinGTCascade);

BOOLN	SetLinScaler_BasilarM_DRNL_Test(double theLinScaler);

BOOLN	SetLinLPCascade_BasilarM_DRNL_Test(int theLinLPCascade);

BOOLN	SetNonLinGTCascade_BasilarM_DRNL_Test(int theNonLinGTCascade);

BOOLN	SetNonLinLPCascade_BasilarM_DRNL_Test(int theNonLinLPCascade);

BOOLN	SetPars_BasilarM_DRNL_Test(int nonLinGTCascade, int nonLinLPCascade,
		  char * compressionMode, double *compressionParsArray,
		  int linGTCascade, int linLPCascade, double linCF, double linBwidth,
		  double linScaler, CFListPtr theCFs);

BOOLN	SetUniParList_BasilarM_DRNL_Test(void);

__END_DECLS

#endif
