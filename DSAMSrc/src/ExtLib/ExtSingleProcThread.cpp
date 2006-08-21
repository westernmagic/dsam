/******************
 *		
 * File:		ExtSingleProcThread.h
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
#include "ExtSingleProcThread.h"

//#define DEBUG 1

#if DEBUG
	extern clock_t startTime;
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

SingleProcThread::SingleProcThread(int theIndex, int offset, int numChannels,
  EarObjectPtr theDataPtr, wxMutex *mutex, wxCondition *condition,
  int *theThreadCount): ProcThread(theIndex, offset, numChannels, mutex,
  condition, theThreadCount)
{
	process = ConfigProcess(theDataPtr);

}

/****************************** Entry *****************************************/

// The thread execution starts here.
// This is a virtual function used by the WxWin threading code.

void *
SingleProcThread::Entry()
{
#	if DEBUG
	static const char *funcName = "SingleProcThread::Entry";
	SignalDataPtr outSignal = _OutSig_EarObject(process);
	printf("%s: Debug: Entered\n", funcName);
	printf("%s: Debug: Running with channels %d -> %d.\n", funcName, outSignal->
	  offset, outSignal->numChannels - 1);
	if (process->subProcessList) {
		SignalDataPtr subOutSignal = _OutSig_EarObject(process->subProcessList[
		  0]);
		printf("%s: Debug: subProcess [%lx], offset = %d, numChannels = %d\n",
		  funcName, (unsigned long) process->subProcessList[0], subOutSignal->
		  offset, subOutSignal->numChannels);
	}
	clock_t	processStart = clock();
	printf("%s: Debug: T[%d]: Starting main process at %g s\n", funcName,
	  GetIndex(), ELAPSED_TIME(startTime, processStart));
#	endif
	bool ok = CXX_BOOL(RunProcessStandard_ModuleMgr(process));
#	if DEBUG
	clock_t	processFinish = clock();
	printf("%s: T[%d]: Finished main process at %g s, %g s elapsed.\n",
	  funcName, GetIndex(), ELAPSED_TIME(startTime, processFinish),
	   ELAPSED_TIME(processStart, processFinish));
#	endif
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
SingleProcThread::OnExit()
{
	ProcThread::OnExit();

}

/******************************************************************************/
/****************************** General Routines ******************************/
/******************************************************************************/

