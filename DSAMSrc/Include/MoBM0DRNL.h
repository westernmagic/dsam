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

#define BASILARM_DRNL_TEST_NUM_PARS	11
#define	LP_CUT_OFF_SCALE			2.5	/* LP cut-off = scale * CF */
#define	DRNLT_WIDE_LP_APPLN			2	/* No. of times the filter is applied.*/

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

typedef enum {

	BASILARM_DRNL_TEST_ORDER,
	BASILARM_DRNL_TEST_COMPRESSIONMODE,
	BASILARM_DRNL_TEST_NARROWBROADENINGCOEFF,
	BASILARM_DRNL_TEST_COMPRESSIONPARS,
	BASILARM_DRNL_TEST_WIDEFORDER,
	BASILARM_DRNL_TEST_WIDEFCENTREFREQ,
	BASILARM_DRNL_TEST_WIDEFWIDTH,
	BASILARM_DRNL_TEST_WIDEATTENUATION,
	BASILARM_DRNL_TEST_WIDEFLPCUTOFFSCALE,
	BASILARM_DRNL_TEST_OUTPUTSCALE,
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
	
	BOOLN	orderFlag;
	BOOLN	nonLinearBroadeningCoeffFlag, *compressionParsFlag;
	BOOLN	linearFCentreFreqFlag,linearFWidthFlag, linearAttenuationFlag;
	BOOLN	linearFLPCutOffScaleFlag, outputScaleFlag, compressionModeFlag;
	BOOLN	linearFOrderFlag;
	BOOLN	updateProcessVariablesFlag;
	int		order;					/* Total (nonLinear filter) line order. */
	int		linearFOrder;			/* Linear filter line order. */
	int		compressionMode;		/* To specify alternate modes. */
	double	*compressionPars;		/* Adjusts compr. response */
	double	nonLinearBroadeningCoeff;	/* NonLinear broadening coefficient */
	double	linearFCentreFreq;		/* Linear filter centre frequency. */
	double	linearFWidth;			/* High frequency cut-off.*/
	double	linearAttenuation;		/* Linear filter attenuation. */
	double	linearFLPCutOffScale;	/* - filter low-pass cut-off scale. */
	double	outputScale;			/* - scales to experimental data. */
	CFListPtr		theCFs;			/* Pointer to centre frequency structure. */

	/* Private Members */
	NameSpecifier *compressionModeList;
	UniParListPtr	parList;
	int		numChannels;
	int		numCompressionPars;
	double	*linearAttenuationdB;
	GammaToneCoeffsPtr	*nonLinearGT1;
	GammaToneCoeffsPtr	*nonLinearGT2;
	GammaToneCoeffsPtr	*linearGT;
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

BOOLN	CheckPars_BasilarM_DRNL_Test(void);

CFListPtr	GetCFListPtr_BasilarM_DRNL_Test(void);

UniParListPtr	GetUniParListPtr_BasilarM_DRNL_Test(void);

BOOLN	Init_BasilarM_DRNL_Test(ParameterSpecifier parSpec);
		  
BOOLN	InitCompressionModeList_BasilarM_DRNL_Test(void);

BOOLN	InitProcessVariables_BasilarM_DRNL_Test(EarObjectPtr data);

BOOLN	Free_BasilarM_DRNL_Test(void);

void	FreeProcessVariables_BasilarM_DRNL_Test(void);

BOOLN	PrintPars_BasilarM_DRNL_Test(void);

BOOLN	ReadPars_BasilarM_DRNL_Test(char *fileName);

BOOLN	RunModel_BasilarM_DRNL_Test(EarObjectPtr data);

BOOLN	SetBandwidths_BasilarM_DRNL_Test(char *theBandwidthMode,
		  double *theBandwidths);

BOOLN	SetCFList_BasilarM_DRNL_Test(CFList *theCFList);

BOOLN	SetCompressionMode_BasilarM_DRNL_Test(char *theCompressionMode);

BOOLN	SetCompressionPar_BasilarM_DRNL_Test(int index, double parameterValue);

BOOLN	SetCompressionParsArray_BasilarM_DRNL_Test(int mode);

BOOLN	SetNonLinearBroadeningCoeff_BasilarM_DRNL_Test(double 
		  theNonLinearBroadeningCoeff);

BOOLN	SetLinearFLPCutOffScale_BasilarM_DRNL_Test(
		  double theLinearFLPCutOffScale);

BOOLN	SetOrder_BasilarM_DRNL_Test(int theOrder);

BOOLN	SetOutputScale_BasilarM_DRNL_Test(double theOutputScale);

BOOLN	SetPars_BasilarM_DRNL_Test(int theOrder, int theLinearFOrder,
		  double theNonLinearBroadeningCoeff, double *theCompressionParsArray,
		  double linearFCentreFreq, double linearFWidth,
		  double linearAttenuation, double linearFLPCutOffScale,
		  CFListPtr theCFs, double outputScale, char *compressionMode);

BOOLN	SetUniParList_BasilarM_DRNL_Test(void);

BOOLN	SetLinearAttenuation_BasilarM_DRNL_Test(double theLinearAttenuation);

BOOLN	SetLinearFOrder_BasilarM_DRNL_Test(int theLinearFOrder);

BOOLN	SetLinearFWidth_BasilarM_DRNL_Test(double theLinearFWidth);

BOOLN	SetLinearFCentreFreq_BasilarM_DRNL_Test(double theLinearFCentreFreq);

__END_DECLS

#endif
