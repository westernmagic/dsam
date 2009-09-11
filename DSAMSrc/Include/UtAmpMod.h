/**********************
 *
 * File:		UtAmpMod.h
 * Purpose:		Utility to amplitude modulate signal.
 * Comments:	Written using ModuleProducer version 1.8.
 * Author:		L. P. O'Mard
 * Created:		21 Dec 1995
 * Updated:
 * Copyright:	(c) 1998, University of Essex
 *
 *********************/

#ifndef _UTAMPMOD_H
#define _UTAMPMOD_H 1

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define UTILITY_AMPMOD_NUM_PARS			4

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef enum {

	UTILITY_AMPMOD_NUMFREQUENCIES,
	UTILITY_AMPMOD_MODULATIONDEPTHS,
	UTILITY_AMPMOD_FREQUENCIES,
	UTILITY_AMPMOD_PHASES

} AmpModParSpecifier;

typedef struct {

	ParameterSpecifier	parSpec;

	int		numFrequencies;
	Float	*modulationDepths, *frequencies, *phases;

	/* Private members */
	UniParListPtr	parList;

} AmpMod, *AmpModPtr;

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

extern	AmpModPtr	ampModPtr;

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

BOOLN	AllocNumFrequencies_Utility_AmpMod(int numFrequencies);

BOOLN	CheckData_Utility_AmpMod(EarObjectPtr data);

BOOLN	Free_Utility_AmpMod(void);

BOOLN	Init_Utility_AmpMod(ParameterSpecifier parSpec);

UniParListPtr	GetUniParListPtr_Utility_AmpMod(void);

BOOLN	PrintPars_Utility_AmpMod(void);

BOOLN	Process_Utility_AmpMod(EarObjectPtr data);

BOOLN	SetDefaultNumFrequenciesArrays_Utility_AmpMod(void);

BOOLN	SetFrequencies_Utility_AmpMod(Float *theFrequencies);

BOOLN	SetIndividualFreq_Utility_AmpMod(int theIndex, Float theFrequency);

BOOLN	SetIndividualDepth_Utility_AmpMod(int theIndex, Float theModDepth);

BOOLN	SetIndividualPhase_Utility_AmpMod(int theIndex, Float thePhase);

BOOLN	SetModulationDepths_Utility_AmpMod(Float *theModulationDepths);

BOOLN	InitModule_Utility_AmpMod(ModulePtr theModule);

BOOLN	SetNumFrequencies_Utility_AmpMod(int theNumFrequencies);

BOOLN	SetParsPointer_Utility_AmpMod(ModulePtr theModule);

BOOLN	SetPhases_Utility_AmpMod(Float *thePhases);

BOOLN	SetUniParList_Utility_AmpMod(void);

__END_DECLS

#endif
