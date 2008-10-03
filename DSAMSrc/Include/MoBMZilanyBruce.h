/**********************
 *
 * File:		MoBMZilanyBruce.h
 * Purpose:		This is the Zilany and Bruce BM filter module.
 *				It was revised from code provided by I. C. Bruce
 * 				(zbcatmodel2007v2.tar.gz).
 *				Zilany, M. S. A. and Bruce, I. C. (2006). "Modeling auditory-
 * 				nerve responses for high sound pressure levels in the normal
 * 				and impaired auditory periphery," Journal of the Acoustical
 * 				Society of America 120(3):1446-1466.
 * 				Zilany, M. S. A. and Bruce, I. C. (2007). "Representation of
 * 				the vowel /eh/ in normal and impaired auditory nerve fibers:
 * 				Model predictions of responses in cats," Journal of the
 * 				Acoustical Society of America 122(1):402-417.
 * Comments:	Written using ModuleProducer version 1.6.0 (Mar  4 2008).
 * Author:		Revised by L. P. O'Mard
 * Created:		04 Mar 2008
 * Updated:	
 * Copyright:	(c) 2008, L. P. O'Mard
 *
 *********************/

#ifndef _MOBMZILANYBRUCE_H
#define _MOBMZILANYBRUCE_H 1

#include "UtNameSpecs.h"

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define BASILARM_ZILANYBRUCE_MOD_NAME			wxT("BM_ZILANYBRUCE")
#define BASILARM_ZILANYBRUCE_NUM_PARS			21

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef enum {

	BASILARM_ZILANYBRUCE_OUTPUTMODE,
	BASILARM_ZILANYBRUCE_MICROPAINPUT,
	BASILARM_ZILANYBRUCE_WBORDER,
	BASILARM_ZILANYBRUCE_S0,
	BASILARM_ZILANYBRUCE_X1,
	BASILARM_ZILANYBRUCE_S1,
	BASILARM_ZILANYBRUCE_SHIFTCP,
	BASILARM_ZILANYBRUCE_CUTOFFCP,
	BASILARM_ZILANYBRUCE_LPORDER,
	BASILARM_ZILANYBRUCE_DC,
	BASILARM_ZILANYBRUCE_RC1,
	BASILARM_ZILANYBRUCE_COHC,
	BASILARM_ZILANYBRUCE_OHCASYM,
	BASILARM_ZILANYBRUCE_IHCASYM,
	BASILARM_ZILANYBRUCE_NBORDER,
	BASILARM_ZILANYBRUCE_AIHC0,
	BASILARM_ZILANYBRUCE_BIHC,
	BASILARM_ZILANYBRUCE_CIHC,
	BASILARM_ZILANYBRUCE_CUTOFFIHCLP,
	BASILARM_ZILANYBRUCE_IHCLPORDER,
	BASILARM_ZILANYBRUCE_CFLIST

} BMZBParSpecifier;

typedef enum {

	BASILARM_ZILANYBRUCE_OUTPUTMODE_C1_FILTER,
	BASILARM_ZILANYBRUCE_OUTPUTMODE_C2_FILTER,
	BASILARM_ZILANYBRUCE_OUTPUTMODE_C1_VIHC,
	BASILARM_ZILANYBRUCE_OUTPUTMODE_C2_VIHC,
	BASILARM_ZILANYBRUCE_OUTPUTMODE_VIHC,
	BASILARM_ZILANYBRUCE_OUTPUTMODE_NULL

} ZBBMOutputModeSpecifier;

typedef struct {

	ParameterSpecifier	parSpec;

	BOOLN	updateProcessVariablesFlag;
	int		outputMode;
	int		microPaInput;
	int		wborder;
	double	s0;
	double	x1;
	double	s1;
	double	shiftCP;
	double	cutOffCP;
	int		lPOrder;
	double	dc;
	double	rC1;
	double	cOHC;
	double	ohcasym;
	double	ihcasym;
	int		nBorder;
	double	aIHC0;
	double	bIHC;
	double	cIHC;
	double	cutOffIHCLP;
	int		iHCLPOrder;
	CFListPtr	cFList;

	/* Private members */
	NameSpecifier	*outputModeList;
	UniParListPtr	parList;
	int		numChannels;
	TLowPass	*ohcLowPass;
	TLowPass	*ihcLowPass;
	ZBGCCoeffs	*c1Filter;
	ZBGCCoeffs	*c2Filter;
	ZBWBGTCoeffsPtr	*wbgt;
	EarObjectPtr	tmpgain;

} BMZB, *BMZBPtr;

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

extern	BMZBPtr	bMZBPtr;

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
* macros, to allow the safe use of C libraries with C++ libraries - defined
* in GeCommon.h.
*/
__BEGIN_DECLS

BOOLN	CheckData_BasilarM_ZilanyBruce(EarObjectPtr data);

BOOLN	FreeProcessVariables_BasilarM_ZilanyBruce(void);

BOOLN	Free_BasilarM_ZilanyBruce(void);

CFListPtr	GetCFListPtr_BasilarM_ZilanyBruce(void);

UniParListPtr	GetUniParListPtr_BasilarM_ZilanyBruce(void);

BOOLN	InitModule_BasilarM_ZilanyBruce(ModulePtr theModule);

BOOLN	InitOutputModeList_BasilarM_ZilanyBruce(void);

BOOLN	InitProcessVariables_BasilarM_ZilanyBruce(EarObjectPtr data);

BOOLN	Init_BasilarM_ZilanyBruce(ParameterSpecifier parSpec);

BOOLN	PrintPars_BasilarM_ZilanyBruce(void);

BOOLN	RunModel_BasilarM_ZilanyBruce(EarObjectPtr data);

BOOLN	SetAIHC0_BasilarM_ZilanyBruce(double theAIHC0);

BOOLN	SetBIHC_BasilarM_ZilanyBruce(double theBIHC);

BOOLN	SetCFList_BasilarM_ZilanyBruce(CFListPtr theCFList);

BOOLN	SetCIHC_BasilarM_ZilanyBruce(double theCIHC);

BOOLN	SetCOHC_BasilarM_ZilanyBruce(double theCOHC);

BOOLN	SetCutOffCP_BasilarM_ZilanyBruce(double theCutOffCP);

BOOLN	SetCutOffIHCLP_BasilarM_ZilanyBruce(double theCutOffIHCLP);

BOOLN	SetDc_BasilarM_ZilanyBruce(double theDc);

BOOLN	SetIHCLPOrder_BasilarM_ZilanyBruce(int theIHCLPOrder);

BOOLN	SetIhcasym_BasilarM_ZilanyBruce(double theIhcasym);

BOOLN	SetLPOrder_BasilarM_ZilanyBruce(int theLPOrder);

BOOLN	SetMicroPaInput_BasilarM_ZilanyBruce(WChar * theMicroPaInput);

BOOLN	SetNBorder_BasilarM_ZilanyBruce(int theNBorder);

BOOLN	SetOhcasym_BasilarM_ZilanyBruce(double theOhcasym);

BOOLN	SetOutputMode_BasilarM_ZilanyBruce(WChar * theOutputMode);

BOOLN	SetParsPointer_BasilarM_ZilanyBruce(ModulePtr theModule);

BOOLN	SetRC1_BasilarM_ZilanyBruce(double theRC1);

BOOLN	SetS0_BasilarM_ZilanyBruce(double theS0);

BOOLN	SetS1_BasilarM_ZilanyBruce(double theS1);

BOOLN	SetShiftCP_BasilarM_ZilanyBruce(double theShiftCP);

BOOLN	SetUniParList_BasilarM_ZilanyBruce(void);

BOOLN	SetWborder_BasilarM_ZilanyBruce(int theWborder);

BOOLN	SetX1_BasilarM_ZilanyBruce(double theX1);

__END_DECLS

#endif
