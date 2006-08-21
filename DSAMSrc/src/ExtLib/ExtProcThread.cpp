/******************
 *		
 * File:		ExtProcThread.h
 * Purpose: 	Process thread class module.
 * Comments:	
 * Author:		L. P. O'Mard
 * Created:		23 Sep 2004
 * Updated:		
 * Copyright:	(c) 2004, CNBH, University of Essex
 *
 ******************/

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG_H */

#include <stdio.h>
#include <stdlib.h>

#include "ExtCommon.h"

#include "GeCommon.h"
#include "GeSignalData.h"
#include "GeEarObject.h"
#include "GeUniParMgr.h"
#include "GeModuleMgr.h"

#include "ExtProcThread.h"

//#define DEBUG	1

#if DEBUG
#	include <time.h>
#endif

/******************************************************************************/
/****************************** Bitmaps ***************************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Methods ***************************************/
/******************************************************************************/

/****************************** Constructor ***********************************/

/*
 * The first thread uses the original process EarObject, and so some of
 * its values need to be reset on closing.
 */

ProcThread::ProcThread(int theIndex, int theOffset, int theNumChannels,
  wxMutex *mutex, wxCondition *condition, int *theThreadCount): wxThread()
{
#	if DEBUG
	printf("ProcThread::ProcThread: Debug: Index = %d, offset = %d\n", theIndex,
	  theOffset);
#	endif
	index = theIndex;
	offset = theOffset;
	numChannels = theNumChannels;
	myMutex = mutex;
	myCondition = condition;
	threadCount = theThreadCount;

}

/****************************** ConfigProcess *********************************/

/*
 * The first thread uses the original process EarObject, and so some of
 * its values need to be reset on closing.
 */

EarObjectPtr
ProcThread::ConfigProcess(EarObjectPtr theDataPtr)
{
	int		i;
	EarObjectPtr	process;
	SignalDataPtr	outSignal, subOutSignal;

	if (!theDataPtr->localOutSignalFlag)
		return(theDataPtr);
	process = (index)? &theDataPtr->threadProcs[index - 1]: theDataPtr;
	outSignal = _OutSig_EarObject(process);
#	ifdef DEBUG
	printf("ProcThread::ConfigProcess: outsignal %lx, index = %d\n",
	  (unsigned long) outSignal, index);
#	endif
	outSignal->offset = offset;
	outSignal->numChannels = offset + numChannels;
	for (i = 0; i < theDataPtr->numSubProcesses; i++) {
		subOutSignal = _OutSig_EarObject(process->subProcessList[i]);
		subOutSignal->offset = outSignal->offset;
		subOutSignal->numChannels = outSignal->numChannels;
	}
	return(process);

}

/****************************** OnExit ****************************************/

// called when the thread exits - whether it terminates normally or is
// stopped with Delete() (but not when it is Kill()ed!)
// The Mutex tells the other(s) thread(s) that we're about to terminate: we must
// lock the mutex first or we might signal the condition before the
// waiting threads start waiting on it!
// The first thread uses the original EarObject and its data, and so some
// parameters need to be reset.

void
ProcThread::OnExit(void)
{
	wxMutexLocker lock(*myMutex);
	myCondition->Signal();
	(*threadCount)--;
#	if DEBUG
	printf("ProcThread::OnExit: T[%d] signaled finished.\n", index);
#	endif

}

/******************************************************************************/
/****************************** General Routines ******************************/
/******************************************************************************/

