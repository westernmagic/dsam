/**********************
 *
 * File:		TrSetDBSPL.h
 * Purpose:		This program calculates a signal's current intensity then
 *				adjusts it to a specified intensity.
 * Comments:	Written using ModuleProducer version 1.12 (Oct 12 1997).
 *				The intensity is calculated starting from an offset position.
 *				23-04-98 LPO: This routine will only work once, until reset.
 *				This means that it will only work for the second segment in
 *				segmented mode.
 * Author:		L. P. O'Mard
 * Created:		06 Nov 1997
 * Updated:		23 Apr 1998
 * Copyright:	(c) 1997, University of Essex
 *
 *********************/

#ifndef _TRSETDBSPL_H
#define _TRSETDBSPL_H 1

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define SETDBSPL_NUM_PARS			2

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef enum {

	SETDBSPL_TIMEOFFSET,
	SETDBSPL_INTENSITY

} SetDBSPLParSpecifier;

typedef struct {

	ParameterSpecifier	parSpec;

	BOOLN	timeOffsetFlag, intensityFlag;
	BOOLN	updateProcessVariablesFlag;
	double	timeOffset;
	double	intensity;

	/* Private members */
	UniParListPtr	parList;
	double	scale;

} SetDBSPL, *SetDBSPLPtr;

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

extern	SetDBSPLPtr	setDBSPLPtr;

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

BOOLN	CheckData_Transform_SetDBSPL(EarObjectPtr data);

BOOLN	CheckPars_Transform_SetDBSPL(void);

BOOLN	Free_Transform_SetDBSPL(void);

BOOLN	FreeProcessVariables_Transform_SetDBSPL(void);

UniParListPtr	GetUniParListPtr_Transform_SetDBSPL(void);

BOOLN	Init_Transform_SetDBSPL(ParameterSpecifier parSpec);

BOOLN	InitProcessVariables_Transform_SetDBSPL(EarObjectPtr data);

BOOLN	PrintPars_Transform_SetDBSPL(void);

BOOLN	Process_Transform_SetDBSPL(EarObjectPtr data);

BOOLN	ReadPars_Transform_SetDBSPL(WChar *fileName);

BOOLN	SetIntensity_Transform_SetDBSPL(double theIntensity);

BOOLN	InitModule_Transform_SetDBSPL(ModulePtr theModule);

BOOLN	SetParsPointer_Transform_SetDBSPL(ModulePtr theModule);

BOOLN	SetPars_Transform_SetDBSPL(double timeOffset, double intensity);

BOOLN	SetTimeOffset_Transform_SetDBSPL(double theTimeOffset);

BOOLN	SetUniParList_Transform_SetDBSPL(void);

__END_DECLS

#endif
