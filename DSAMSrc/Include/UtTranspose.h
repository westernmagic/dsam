/**********************
 *
 * File:		UtTranspose.h
 * Purpose:		This module takes an input signal and converts the n channels
 *				of m samples to m channels of n samples.
 * Comments:	Written using ModuleProducer version 1.4.2 (Dec 19 2003).
 * Author:		L. P. O'Mard
 * Created:		13 Jul 2004
 * Updated:	
 * Copyright:	(c) 2004, CNBH, University of Essex
 *
 *********************/

#ifndef _UTTRANSPOSE_H
#define _UTTRANSPOSE_H 1

#include "UtNameSpecs.h"

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define UTILITY_TRANSPOSE_NUM_PARS			1
#define	UTILITY_TRANSPOSE_MOD_NAME			"UTILITY_TRANSPOSE"

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef enum {

	UTILITY_TRANSPOSE_MODE

} TransposeParSpecifier;

typedef enum {

	UTILITY_TRANSPOSE_STANDARD_MODE,
	UTILITY_TRANSPOSE_FIRST_CHANNEL_MODE,
	UTILITY_TRANSPOSE_MODE_NULL

} TransposeModeSpecifier;

typedef struct {

	ParameterSpecifier	parSpec;

	BOOLN	modeFlag;
	int		mode;

	/* Private members */
	NameSpecifier	*modeList;
	UniParListPtr	parList;

} Transpose, *TransposePtr;

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

extern	TransposePtr	transposePtr;

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
* macros, to allow the safe use of C libraries with C++ libraries - defined
* in GeCommon.h.
*/
__BEGIN_DECLS

BOOLN	CheckData_Utility_Transpose(EarObjectPtr data);

BOOLN	CheckPars_Utility_Transpose(void);

BOOLN	Free_Utility_Transpose(void);

UniParListPtr	GetUniParListPtr_Utility_Transpose(void);

BOOLN	InitModeList_Utility_Transpose(void);

BOOLN	InitModule_Utility_Transpose(ModulePtr theModule);

BOOLN	Init_Utility_Transpose(ParameterSpecifier parSpec);

BOOLN	PrintPars_Utility_Transpose(void);

BOOLN	Process_Utility_Transpose(EarObjectPtr data);

BOOLN	SetMode_Utility_Transpose(char * theMode);

BOOLN	SetParsPointer_Utility_Transpose(ModulePtr theModule);

BOOLN	SetUniParList_Utility_Transpose(void);

__END_DECLS

#endif
