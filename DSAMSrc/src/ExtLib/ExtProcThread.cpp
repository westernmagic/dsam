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

ProcThread::ProcThread(int theIndex, int offset, int numChannels,
  EarObjectPtr theDataPtr, wxMutex *mutex, wxCondition *condition,
  int *theThreadCount): wxThread()
{
	int		i;

	index = theIndex;
	if (index)
		process = &theDataPtr->threadProcs[index - 1];
	else {
		process = theDataPtr;
		origNumChannels = process->outSignal->numChannels;
	}
	process->outSignal->offset = offset;
	process->outSignal->numChannels = offset + numChannels;
	for (i = 0; i < theDataPtr->numSubProcesses; i++) {
		process->subProcessList[i]->outSignal->offset = process->outSignal->
		  offset;
		process->subProcessList[i]->outSignal->numChannels = process->
		  outSignal->numChannels;
	}
	myMutex = mutex;
	myCondition = condition;
	threadCount = theThreadCount;

}

/****************************** Entry *****************************************/

// The thread execution starts here.
// This is a virtual function used by the WxWin threading code.

void *
ProcThread::Entry()
{
#	if DEBUG
	printf("ProcThread::Entry: Debug: Entered\n");
	printf("ProcThread::Entry: Debug: Running with channels %d -> %d.\n",
	  process->outSignal->offset, process->outSignal->numChannels - 1);
#	endif
	bool ok = CXX_BOOL(RunProcessStandard_ModuleMgr(process));
	return((void *) ok);

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
ProcThread::OnExit()
{
	wxMutexLocker lock(*myMutex);
	myCondition->Signal();
	(*threadCount)--;
#	if DEBUG
	printf("ProcThread::OnExit: offset %d signaled finished.\n",
	  process->outSignal->offset);
#	endif
	if (!index)	{
		int		i;
		process->outSignal->offset = 0;
		process->outSignal->numChannels = origNumChannels;
		for (i = 0; i < process->numSubProcesses; i++) {
			process->subProcessList[i]->outSignal->offset = 0;
			process->subProcessList[i]->outSignal->numChannels =
			  origNumChannels;
		}
	}

}

/******************************************************************************/
/****************************** General Routines ******************************/
/******************************************************************************/

