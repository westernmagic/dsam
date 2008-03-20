/**********************
 *
 * File:		FlZBMiddleEar.h
 * Purpose:		This is the Zilany and Bruce middle-ear filter module.
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
 * Comments:	Written using ModuleProducer version 1.6.0 (Mar  3 2008).
 *				This first revision follows the existing code with as little
 *				change as possible.
 * 				It uses an EarObject to allocate the memory for the mey arrays.
 * Author:		Revised by L. P. O'Mard
 * Created:		03 Mar 2008
 * Updated:	
 * Copyright:	(c) 2008, CNBH, University of Cambridge.
 *
 *********************/

#ifndef _FLZBMIDDLEEAR_H
#define _FLZBMIDDLEEAR_H 1

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define FILTER_ZBMIDDLEEAR_MOD_NAME		wxT("Filt_ZILANYBRUCEME")
#define FILTER_ZBMIDDLEEAR_NUM_PARS				2
#define ZB_ME_NUM_STATE_VECTORS					3

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef enum {

	FILTER_ZBMIDDLEEAR_GAINMAX,
	FILTER_ZBMIDDLEEAR_PREWARPINGFREQ

} FLZBMEarParSpecifier;

typedef struct {

	ParameterSpecifier	parSpec;

	BOOLN	updateProcessVariablesFlag;
	double	gainMax;
	double	preWarpingFreq;

	/* Private members */
	UniParListPtr	parList;
	double	m11, m12;
	double	m21, m22, m23, m24, m25, m26;
	double	m31, m32, m33, m34, m35, m36;
	double	*lastInput;
	EarObjectPtr	mey;

} FLZBMEar, *FLZBMEarPtr;

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

extern	FLZBMEarPtr	fLZBMEarPtr;

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
* macros, to allow the safe use of C libraries with C++ libraries - defined
* in GeCommon.h.
*/
__BEGIN_DECLS

BOOLN	CheckData_Filter_ZBMiddleEar(EarObjectPtr data);

BOOLN	FreeProcessVariables_Filter_ZBMiddleEar(void);

BOOLN	Free_Filter_ZBMiddleEar(void);

UniParListPtr	GetUniParListPtr_Filter_ZBMiddleEar(void);

BOOLN	InitModule_Filter_ZBMiddleEar(ModulePtr theModule);

BOOLN	InitProcessVariables_Filter_ZBMiddleEar(EarObjectPtr data);

BOOLN	Init_Filter_ZBMiddleEar(ParameterSpecifier parSpec);

BOOLN	PrintPars_Filter_ZBMiddleEar(void);

BOOLN	RunModel_Filter_ZBMiddleEar(EarObjectPtr data);

BOOLN	SetGainMax_Filter_ZBMiddleEar(double theGainMax);

BOOLN	SetParsPointer_Filter_ZBMiddleEar(ModulePtr theModule);

BOOLN	SetPreWarpingFreq_Filter_ZBMiddleEar(double thePreWarpingFreq);

BOOLN	SetUniParList_Filter_ZBMiddleEar(void);

__END_DECLS

#endif
