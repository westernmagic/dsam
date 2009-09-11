/**********************
 *
 * File:		TrCollectSignals.h
 * Purpose:		This transform produces and output signal with a number of
 *				channels equal to the sum of the input signal channels.
 * Comments:	Written using ModuleProducer version 1.4.2 (Dec 19 2003).
 * Author:		L. P. O'Mard
 * Created:		22 Apr 2004
 * Updated:
 * Copyright:	(c) 2004, CNBH, University of Essex
 *
 *********************/

#ifndef _TRCOLLECTSIGNALS_H
#define _TRCOLLECTSIGNALS_H 1

#include "UtNameSpecs.h"

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define TRANSFORM_COLLECTSIGNALS_NUM_PARS			2

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef enum {

	TRANSFORM_COLLECTSIGNALS_LABELMODE,
	TRANSFORM_COLLECTSIGNALS_LABELS

} CollectSigsParSpecifier;

typedef enum {

	TRANSFORM_COLLECTSIGNALS_LABELMODE_CHAN_INDEX,
	TRANSFORM_COLLECTSIGNALS_LABELMODE_INPUT_LABELS,
	TRANSFORM_COLLECTSIGNALS_LABELMODE_USER,
	TRANSFORM_COLLECTSIGNALS_LABELMODE_NULL

} TransformLabelModeSpecifier;

typedef struct {

	ParameterSpecifier	parSpec;

	int		labelMode;
	Float	*labels;

	/* Private members */
	NameSpecifier	*labelModeList;
	UniParListPtr	parList;
	int		numChannels;

} CollectSigs, *CollectSigsPtr;

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

extern	CollectSigsPtr	collectSigsPtr;

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
* macros, to allow the safe use of C libraries with C++ libraries - defined
* in GeCommon.h.
*/
__BEGIN_DECLS

BOOLN	CheckData_Transform_CollectSignals(EarObjectPtr data);

BOOLN	Free_Transform_CollectSignals(void);

UniParListPtr	GetUniParListPtr_Transform_CollectSignals(void);

BOOLN	InitLabelModeList_Transform_CollectSignals(void);

BOOLN	InitModule_Transform_CollectSignals(ModulePtr theModule);

BOOLN	Init_Transform_CollectSignals(ParameterSpecifier parSpec);

BOOLN	PrintPars_Transform_CollectSignals(void);

BOOLN	Process_Transform_CollectSignals(EarObjectPtr data);

void	SetEnabledState_Transform_CollectSignals(void);

BOOLN	SetIndividualLabel_Transform_CollectSignals(int theIndex, Float
		  theLabel);

BOOLN	SetLabelMode_Transform_CollectSignals(WChar * theLabelMode);

BOOLN	SetLabels_Transform_CollectSignals(Float *theLabels);

BOOLN	SetParsPointer_Transform_CollectSignals(ModulePtr theModule);

BOOLN	SetUniParList_Transform_CollectSignals(void);

__END_DECLS

#endif
