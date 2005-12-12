/**********************
 *
 * File:		UtRedceChans.h
 * Purpose:		This routine reduces multiple channel data to a lower number of
 *				channels.
 * Comments:	Written using ModuleProducer version 1.8.
 *				The input channels are divided over the output channels.  Each
 *				time step of the appropriate number of channels of a signal
 *				data structure is summed, then the average is calculated by
 *				dividing by the number of channels summed.  The input channels
 *				are divided over the output channels. The no. of output
 *				channels must be a multiple of the number of input channels. It
 *				overwrites previous data if the output signal has already been
 *				initialised.
 *				This routine always sets the data->updateProcessFlag, to reset
 *				the output signal to zero.
 *				02-03-97 LPO: this module now treats interleaved signals
 *				correctly.
 *				02-03-05 LPO: This module is not being thread enabled as the
 *				number of input channels defines the channel processing.
 * Author:		L.P.O'Mard
 * Created:		21 Dec 1995
 * Updated:		02 Mar 1997
 * Copyright:	(c) 2005, CNBH, University of Essex. 
 *
**********************/

#ifndef _UTREDCECHANS_H
#define _UTREDCECHANS_H 1

#include "UtNameSpecs.h"

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define UTILITY_REDUCECHANNELS_NUM_PARS			2

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef enum {

	UTILITY_REDUCECHANNELS_MODE,
	UTILITY_REDUCECHANNELS_NUMCHANNELS

} ReduceChansParSpecifier;

typedef	enum {

	REDUCE_CHANS_AVERAGE_MODE,
	REDUCE_CHANS_SUM_MODE,
	REDUCE_CHANS_NULL

} ReduceChansModeSpecifier;

typedef struct {

	ParameterSpecifier	parSpec;

	BOOLN	modeFlag, numChannelsFlag;
	int		mode;
	int		numChannels;

	/* Private members */
	NameSpecifier *modeList;
	UniParListPtr	parList;

} ReduceChans, *ReduceChansPtr;

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

extern	ReduceChansPtr	reduceChansPtr;

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

BOOLN	CheckData_Utility_ReduceChannels(EarObjectPtr data);

BOOLN	CheckPars_Utility_ReduceChannels(void);

BOOLN	Free_Utility_ReduceChannels(void);

BOOLN	Init_Utility_ReduceChannels(ParameterSpecifier parSpec);

UniParListPtr	GetUniParListPtr_Utility_ReduceChannels(void);

BOOLN	InitModeList_Utility_ReduceChannels(void);

BOOLN	PrintPars_Utility_ReduceChannels(void);

BOOLN	Process_Utility_ReduceChannels(EarObjectPtr data);

BOOLN	ReadPars_Utility_ReduceChannels(char *fileName);

void	ResetProcess_Utility_ReduceChannels(EarObjectPtr data);

BOOLN	SetMode_Utility_ReduceChannels(char *theMode);

BOOLN	InitModule_Utility_ReduceChannels(ModulePtr theModule);

BOOLN	SetNumChannels_Utility_ReduceChannels(int theNumChannels);

BOOLN	SetParsPointer_Utility_ReduceChannels(ModulePtr theModule);

BOOLN	SetPars_Utility_ReduceChannels(char *mode, int numChannels);

BOOLN	SetUniParList_Utility_ReduceChannels(void);

__END_DECLS

#endif
