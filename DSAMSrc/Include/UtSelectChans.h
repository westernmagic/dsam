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
 * Author:		L. P. O'Mard
 * Created:		11 Jun 1996
 * Updated:		19 Mar 1997
 * Copyright:	(c) 1998, University of Essex
 *
 *********************/

#ifndef _UTSELECTCHANS_H
#define _UTSELECTCHANS_H 1

#include "UtNameSpecs.h"

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define UTILITY_SELECTCHANNELS_NUM_PARS			3

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef enum {

	UTILITY_SELECTCHANNELS_MODE,
	UTILITY_SELECTCHANNELS_NUMCHANNELS,
	UTILITY_SELECTCHANNELS_SELECTIONARRAY

} SelectChanParSpecifier;

typedef	enum {

	SELECT_CHANS_ZERO_MODE,
	SELECT_CHANS_REMOVE_MODE,
	SELECT_CHANS_NULL

} SelectChansModeSpecifier;

typedef struct {

	ParameterSpecifier	parSpec;

	BOOLN	modeFlag, numChannelsFlag;
	int		mode;
	int		numChannels;
	int		*selectionArray;

	/* Private members */
	NameSpecifier *modeList;
	UniParListPtr	parList;

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

BOOLN	CheckPars_Utility_SelectChannels(void);

BOOLN	Free_Utility_SelectChannels(void);

UniParListPtr	GetUniParListPtr_Utility_SelectChannels(void);

BOOLN	Init_Utility_SelectChannels(ParameterSpecifier parSpec);

BOOLN	InitModeList_Utility_SelectChannels(void);

BOOLN	PrintPars_Utility_SelectChannels(void);

BOOLN	Process_Utility_SelectChannels(EarObjectPtr data);

BOOLN	ReadPars_Utility_SelectChannels(char *fileName);

BOOLN	SetIndividualSelection_Utility_SelectChannels(int theIndex,
		  int theSelection);

BOOLN	SetMode_Utility_SelectChannels(char *theMode);

BOOLN	SetNumChannels_Utility_SelectChannels(int theNumChannels);

BOOLN	SetPars_Utility_SelectChannels(char *mode, int numChannels,
		  int *selectionArray);

BOOLN	SetSelectionArray_Utility_SelectChannels(int *theSelectionArray);

BOOLN	SetUniParList_Utility_SelectChannels(void);

__END_DECLS

#endif
