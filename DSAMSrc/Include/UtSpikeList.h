/**********************
 *
 * File:		UtSpikeList.h
 * Purpose:		This utility module contains the routines for producing a spike
 *				train list from a signal.
 * Comments:	This implementation uses a linked list to record the spike
 *				times.  This list can be re-used, or it can retain the previous
 *				values.
 *				28-04-98 LPO: Correct segmented mode bug by adding the timeIndex
 *				value and also the riseDetected and lastValue arrays to the
 *				SpikeListSpec structure.
 * Author:		L. P. O'Mard
 * Created:		01 Apr 1996
 * Updated:		28 Apr 1998
 * Copyright:	(c) 1998, University of Essex
 *
 *********************/

#ifndef _UTSPIKELIST_H
#define _UTSPIKELIST_H 1

#include "GeCommon.h"
#include "GeSignalData.h"

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef struct SpikeSpec {

	int		number;
	ChanLen	timeIndex;
	struct SpikeSpec *next;
	
} SpikeSpec, *SpikeSpecPtr;

typedef struct {

	BOOLN			*riseDetected;
	uShort			numChannels;
	double			*lastValue;
	ChanLen			timeIndex;
	SpikeSpecPtr	*head;
	SpikeSpecPtr	*tail;
	SpikeSpecPtr	*current;

} SpikeListSpec, *SpikeListSpecPtr;

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

void	Free_SpikeList(SpikeSpecPtr *head);

void	FreeListSpec_SpikeList(SpikeListSpecPtr *p);

BOOLN	GenerateList_SpikeList(SpikeListSpecPtr listSpec, double eventThreshold,
		  SignalDataPtr signal);

SpikeListSpecPtr	InitListSpec_SpikeList(int numChannels);

BOOLN	InsertSpikeSpec_SpikeList(SpikeListSpecPtr listSpec, uShort channel,
		  ChanLen timeIndex);

BOOLN	ResetListSpec_SpikeList(SpikeListSpecPtr listSpec);

__END_DECLS

#endif
