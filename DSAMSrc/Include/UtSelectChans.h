/**********************
 *
 * File:		UtSelectChans.h
 * Purpose:		This routine copies the input signal to the output signal,
 *				but copies only the specified input channels to the output
 *				signal.
 *				The channels are marked as zero in the array supplied as an
 *				argument.
 * Comments:	Written using ModuleProducer version 1.9 (May 27 1996).
 *				N. B. the array setting is uncontrolled, but this cannot be
 *				helped right now.
 *				There are no PrintPars nor ReadPars routines, as these are not
 *				appropriate for this type of module.
 *				19-03-97 LPO: Added "mode" parameter.
 *				02-03-05 LPO: This module is not being thread enabled as the
 *				number of input channels defines the channel processing.
 * Author:		L. P. O'Mard
 * Created:		11 Jun 1996
 * Updated:		19 Mar 1997
 * Copyright:	(c) 2005, CNBH, University of Essex
 *
 *********************/

#ifndef _UTSELECTCHANS_H
#define _UTSELECTCHANS_H 1

#include "UtNameSpecs.h"

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define UTILITY_SELECTCHANNELS_NUM_PARS			4

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef enum {

	UTILITY_SELECTCHANNELS_MODE,
	UTILITY_SELECTCHANNELS_SELECTIONMODE,
	UTILITY_SELECTCHANNELS_NUMCHANNELS,
	UTILITY_SELECTCHANNELS_SELECTIONARRAY

} SelectChanParSpecifier;

typedef enum {

	UTILITY_SELECTCHANNELS_SELECTIONMODE_ALL,
	UTILITY_SELECTCHANNELS_SELECTIONMODE_MIDDLE,
	UTILITY_SELECTCHANNELS_SELECTIONMODE_LOWEST,
	UTILITY_SELECTCHANNELS_SELECTIONMODE_HIGHEST,
	UTILITY_SELECTCHANNELS_SELECTIONMODE_USER,
	UTILITY_SELECTCHANNELS_SELECTIONMODE_NULL

} SelectChanSelectionModeSpecifier;

typedef	enum {

	SELECT_CHANS_ZERO_MODE,
	SELECT_CHANS_REMOVE_MODE,
	SELECT_CHANS_EXPAND_MODE,
	SELECT_CHANS_NULL

} SelectChansModeSpecifier;

typedef struct {

	ParameterSpecifier	parSpec;

	BOOLN	updateProcessVariablesFlag;
	int		mode;
	int		selectionMode;
	Float	*selectionArray;

	/* Private members */
	NameSpecifier *modeList;
	NameSpecifier	*selectionModeList;
	UniParListPtr	parList;
	int		numChannels;

} SelectChan, *SelectChanPtr;

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

extern	SelectChanPtr	selectChanPtr;

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

BOOLN	AllocNumChannels_Utility_SelectChannels(int numChannels);

BOOLN	CheckData_Utility_SelectChannels(EarObjectPtr data);

BOOLN	Free_Utility_SelectChannels(void);

UniParListPtr	GetUniParListPtr_Utility_SelectChannels(void);

BOOLN	InitProcessVariables_Utility_SelectChannels(EarObjectPtr data);

BOOLN	InitSelectionModeList_Utility_SelectChannels(void);

BOOLN	Init_Utility_SelectChannels(ParameterSpecifier parSpec);

BOOLN	InitModeList_Utility_SelectChannels(void);

BOOLN	PrintPars_Utility_SelectChannels(void);

BOOLN	Process_Utility_SelectChannels(EarObjectPtr data);

BOOLN	SetDefaulEnabledPars_Utility_SelectChannels(void);

void	SetEnabledState_Utility_SelectChannels(void);

BOOLN	SetIndividualSelection_Utility_SelectChannels(int theIndex,
		  Float theSelection);

BOOLN	SetMode_Utility_SelectChannels(WChar *theMode);

BOOLN	InitModule_Utility_SelectChannels(ModulePtr theModule);

BOOLN	SetNumChannels_Utility_SelectChannels(int theNumChannels);

BOOLN	SetParsPointer_Utility_SelectChannels(ModulePtr theModule);

BOOLN	SetSelectionArray_Utility_SelectChannels(Float *theSelectionArray);

BOOLN	SetSelectionMode_Utility_SelectChannels(WChar * theSelectionMode);

BOOLN	SetUniParList_Utility_SelectChannels(void);

__END_DECLS

#endif
