/**********************
 *
 * File:		GeNSpecLists.h
 * Purpose:		This module contains the common specifier lists used by various
 *				modules.
 * Comments:	
 * Author:		L. P. O'Mard
 * Created:		26 Nov 1997
 * Updated:
 * Copyright:	(c) 1998, University of Essex
 *
 *********************/

#ifndef _GENSPECLISTS_H
#define _GENSPECLISTS_H 1

#include "UtNameSpecs.h"
#include "UtRandom.h"

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Macro definitions *****************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef	enum {

	GENERAL_BOOLEAN_OFF = 0,
	GENERAL_BOOLEAN_ON,
	GENERAL_BOOLEAN_NULL

} GeneralBooleanSpecifier;

typedef	enum {

	GENERAL_DIAGNOSTIC_OFF_MODE,
	GENERAL_DIAGNOSTIC_SCREEN_MODE,
	GENERAL_DIAGNOSTIC_ERROR_MODE,
	GENERAL_DIAGNOSTIC_FILE_MODE,
	GENERAL_DIAGNOSTIC_MODE_NULL

} GeneralDiagnosticModeSpecifier;

typedef enum {

	GENERAL_FIT_FUNC_EXP1_MODE,
	GENERAL_FIT_FUNC_LINEAR1_MODE,
	GENERAL_FIT_FUNC_LOG1_MODE,
	GENERAL_FIT_FUNC_LOG2_MODE,
	GENERAL_FIT_FUNC_POLY1_MODE,
	GENERAL_FIT_FUNC_NULL

} GeneralFitFuncModeSpecifier;

typedef enum {

	GENERAL_PHASE_RANDOM,
	GENERAL_PHASE_SINE,
	GENERAL_PHASE_COSINE,
	GENERAL_PHASE_ALTERNATING,
	GENERAL_PHASE_SCHROEDER,
	GENERAL_PHASE_PLACK_AND_WHITE,
	GENERAL_PHASE_USER,
	GENERAL_PHASE_NULL

} GeneralPhaseModeSpecifier;

typedef enum {

	GENERAL_EAR_LEFT,
	GENERAL_EAR_RIGHT,
	GENERAL_EAR_BOTH,
	GENERAL_EAR_NULL

} GeneralEarModeSpecifier;

typedef enum {

	GENERAL_SPACINGMODE_ERB,
	GENERAL_SPACINGMODE_LINEAR,
	GENERAL_SPACINGMODE_OCTAVE,
	GENERAL_SPACINGMODE_NULL

} GeneralSpacingModeSpecifier;

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

NameSpecifier *	BooleanList_NSpecLists(int index);

void			CloseDiagnostics_NSpecLists(FILE **fp);

NameSpecifier *	DiagModeList_NSpecLists(int index);

NameSpecifier * EarModeList_NSpecLists(int index);

NameSpecifier *	FitFuncModeList_NSpecLists(int index);

int		GetNumListEntries_NSpecLists(NameSpecifierPtr list);

int		IdentifyDiag_NSpecLists(WChar *mode, NameSpecifierPtr list);

NameSpecifier *	InitNameList_NSpecLists(NameSpecifierPtr prototypeList,
				  WChar *textPtr);

BOOLN	OpenDiagnostics_NSpecLists(FILE **fp, NameSpecifierPtr list, int mode);

NameSpecifier *	PhaseModeList_NSpecLists(int index);

void	SetPhaseArray_NSpecLists(double *phase, long *ranSeed,
		  RandParsPtr randPars, int phaseMode, double phaseVariable,
		  int lowestHarmonic, int numHarmonics);

NameSpecifier *	SpacingModeList_NSpecLists(int index);

__END_DECLS

#endif
