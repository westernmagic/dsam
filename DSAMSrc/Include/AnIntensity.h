/**********************
 *
 * File:		AnIntensity.h
 * Purpose:		This module calculates the intensity for a signal, starting
 *				from an offset position.
 * Comments:	Written using ModuleProducer version 1.9 (May 27 1996).
 * Author:		L. P. O'Mard
 * Created:		12 Jun 1996
 * Updated:	
 * Copyright:	(c) 1998, University of Essex.
 *
 *********************/

#ifndef _ANINTENSITY_H
#define _ANINTENSITY_H 1

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define ANALYSIS_INTENSITY_NUM_PARS			2

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef enum {

	ANALYSIS_INTENSITY_TIMEOFFSET,
	ANALYSIS_INTENSITY_EXTENT

} IntensityParSpecifier;

typedef struct {

	ParameterSpecifier	parSpec;

	BOOLN	timeOffsetFlag, extentFlag;
	double	timeOffset;
	double	extent;

	/* Private members */
	UniParListPtr	parList;
	ChanLen	timeOffsetIndex, wExtent;

} Intensity, *IntensityPtr;

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

extern	IntensityPtr	intensityPtr;

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

BOOLN	Calc_Analysis_Intensity(EarObjectPtr data);

BOOLN	CheckData_Analysis_Intensity(EarObjectPtr data);

BOOLN	CheckPars_Analysis_Intensity(void);

BOOLN	Free_Analysis_Intensity(void);

UniParListPtr	GetUniParListPtr_Analysis_Intensity(void);

BOOLN	Init_Analysis_Intensity(ParameterSpecifier parSpec);

BOOLN	PrintPars_Analysis_Intensity(void);

BOOLN	ReadPars_Analysis_Intensity(WChar *fileName);

BOOLN	InitModule_Analysis_Intensity(ModulePtr theModule);

BOOLN	SetExtent_Analysis_Intensity(double theExtent);

BOOLN	SetParsPointer_Analysis_Intensity(ModulePtr theModule);

BOOLN	SetPars_Analysis_Intensity(double timeOffset);

BOOLN	SetTimeOffset_Analysis_Intensity(double theTimeOffset);

BOOLN	SetUniParList_Analysis_Intensity(void);

__END_DECLS

#endif
