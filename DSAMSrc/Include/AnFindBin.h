/**********************
 *
 * File:		AnFindBin.h
 * Purpose:		This module finds the maximum/minimum bin values for a signal
 * Comments:	Written using ModuleProducer version 1.9 (Feb 29 1996).
 *				The search is implemented using a bin "window" of "binWidth".
 * Author:		L. P. O'Mard
 * Created:		8 Mar 1996
 * Updated:		9 Jun 1996
 * Copyright:	(c) Loughborough University
 *
 *********************/

#ifndef _ANFINDBIN_H
#define _ANFINDBIN_H 1

#include "UtNameSpecs.h"

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define ANALYSIS_FINDBIN_NUM_PARS			4

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef enum {

	ANALYSIS_FINDBIN_MODE,
	ANALYSIS_FINDBIN_BINWIDTH,
	ANALYSIS_FINDBIN_TIMEOFFSET,
	ANALYSIS_FINDBIN_TIMEWIDTH

} FindBinParSpecifier;

typedef enum {

	FIND_BIN_MIN_VALUE_MODE,
	FIND_BIN_MIN_INDEX_MODE,
	FIND_BIN_MAX_VALUE_MODE,
	FIND_BIN_MAX_INDEX_MODE,
	FIND_BIN_NULL

} FindBinModeSpecifier;

typedef struct {

	ParameterSpecifier	parSpec;

	BOOLN	modeFlag, binWidthFlag, timeOffsetFlag, timeWidthFlag;
	int		mode;
	double	binWidth;
	double	timeOffset;
	double	timeWidth;

	/* Private members */
	NameSpecifier *modeList;
	UniParListPtr	parList;
	ChanLen	binWidthIndex, timeWidthIndex, timeOffsetIndex;

} FindBin, *FindBinPtr;

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

extern	FindBinPtr	findBinPtr;

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

BOOLN	Calc_Analysis_FindBin(EarObjectPtr data);

BOOLN	CheckData_Analysis_FindBin(EarObjectPtr data);

BOOLN	CheckPars_Analysis_FindBin(void);

BOOLN	Free_Analysis_FindBin(void);

UniParListPtr	GetUniParListPtr_Analysis_FindBin(void);

BOOLN	Init_Analysis_FindBin(ParameterSpecifier parSpec);

BOOLN	InitModeList_Analysis_FindBin(void);

BOOLN	PrintPars_Analysis_FindBin(void);

BOOLN	ReadPars_Analysis_FindBin(char *fileName);

BOOLN	SetBinWidth_Analysis_FindBin(double theBinWidth);

BOOLN	SetMode_Analysis_FindBin(char *theMode);

BOOLN	InitModule_Analysis_FindBin(ModulePtr theModule);

BOOLN	SetParsPointer_Analysis_FindBin(ModulePtr theModule);

BOOLN	SetPars_Analysis_FindBin(char *mode, double binWidth,
			  double timeOffset, double timeWidth);

BOOLN	SetTimeOffset_Analysis_FindBin(double theTimeOffset);

BOOLN	SetTimeWidth_Analysis_FindBin(double theTimeWidth);

BOOLN	SetUniParList_Analysis_FindBin(void);

__END_DECLS

#endif
