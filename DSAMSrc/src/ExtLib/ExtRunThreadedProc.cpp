/******************
 *		
 * File:		ExtRunThreadedProc.cpp
 * Purpose: 	This module runs a processes using threads of possible.
 * Comments:	It was deceided to create this module rather than creating
 *				a replacement ExecuteSimulation routine.
 * Author:		L. P. O'Mard
 * Created:		30 Sep 2004
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
#include "UtAppInterface.h"
#include "UtSSSymbols.h"
#include "UtSSParser.h"

#include "ExtProcThread.h"
#include "ExtRunThreadedProc.h"

/******************************************************************************/
/****************************** Bitmaps ***************************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

RunThreadedProc	*runThreadedProc = NULL;

/******************************************************************************/
/****************************** Functions *************************************/
/******************************************************************************/

/****************************** Constructor ***********************************/

RunThreadedProc::RunThreadedProc(void)
{
#	if DEBUG
	printf("RunThreadedProc::RunThreadedProc: Debug: Entered\n");
#	endif
	numThreads = wxThread::GetCPUCount();
#	if DEBUG
	printf("RunThreadedProc::RunThreadedProc: Debug: %d CPU's available\n",
	  numThreads);
#	endif
	runThreadedProc = this;
	SetRunProcess_ModuleMgr(RunProcess_RunThreadedProc);

}

/****************************** Constructor ***********************************/

void
RunThreadedProc::SetNumThreads(int theNumThreads)
{
#	if DEBUG
	printf("RunThreadedProc::SetNumThreads: Debug: Entered\n");
#	endif
	numThreads = (theNumThreads < 1)? wxThread::GetCPUCount(): theNumThreads;

}

/****************************** PreThreadProcessInit **************************/

/*
 * This function does the pre-thread process initialisation run.
 */

bool
RunThreadedProc::PreThreadProcessInit(EarObjectPtr data)
{
	static const char *funcName = "RunThreadedProc::PreThreadProcessInit";
	bool	ok = true;

#	if DEBUG
	printf("%s: Debug: Entered.\n", funcName);
#	endif
	data->initThreadRunFlag = true;
	data->numThreads = numThreads;
	ok = CXX_BOOL(RunProcessStandard_ModuleMgr(data));
	data->initThreadRunFlag = false;
	if (!ok)
		NotifyError("%s: Could not do pre-thread process initialisation run.",
		  funcName);
	return(ok);

}


/****************************** RunThreadEnabled ******************************/

/*
 * Run a process which is enabled to use threading.
 * The Process 'threadRunFlag' must be set before the
 * 'InitThreadProcs_EarObject'routine is called, so that each thread has this
 * flag set correctly.
 */

bool
RunThreadedProc::RunThreadEnabled(EarObjectPtr data)
{
	static const char *funcName = "RunThreadedProc::RunThreadEnabled";
	int		i, chansPerThread, remainderChans, threadCount;
	wxMutex	mutex;
	wxCondition	condition(mutex);
	ProcThread	*procThread;

#	if DEBUG
	printf("%s: Debug: Entered.\n", funcName);
#	endif
	if (!PreThreadProcessInit(data))
		return(false);

	if (data->outSignal->numChannels < 2)
		return(CXX_BOOL(RunProcessStandard_ModuleMgr(data)));
	mutex.Lock();
	if (data->outSignal->numChannels < numThreads)
		numThreads = data->outSignal->numChannels;
	chansPerThread = data->outSignal->numChannels / numThreads;
#	if DEBUG
	printf("%s: Debug: chansPerThread = %d\n", funcName, chansPerThread);
#	endif
	remainderChans = data->outSignal->numChannels % numThreads;

	data->threadRunFlag = TRUE;
	if (!InitThreadProcs_EarObject(data, numThreads)) {
		wxLogFatalError("%s: Could not initialise thread EarObjects.",
		  funcName);
		return(false);
	}
	threadCount = numThreads;
	for (i = 0; i < numThreads; i++) {
		procThread = new ProcThread(i, i * chansPerThread, chansPerThread +
		  ((i == numThreads - 1)? remainderChans: 0), data, &mutex, &condition,
		  &threadCount);
		if (procThread->Create() != wxTHREAD_NO_ERROR)
			wxLogFatalError("%s: Can't create process thread!", funcName);
		if (procThread->Run() != wxTHREAD_NO_ERROR)
			wxLogError("%s: Cannot start process thread.", funcName);
	}
	while (threadCount) {
#		if DEBUG
		printf("%s: Debug: waiting for %d threads to finish.\n", funcName,
		  threadCount);
#		endif
		condition.Wait();
	}
	data->threadRunFlag = FALSE;
#	if DEBUG
	printf("%s: Debug: Finished\n", funcName);
#	endif
	return(true);

}

/****************************** RunProcess ************************************/

/*
 * This routine replaces the RunProcessStandard_ModuleMgr routine.
 */

bool
RunThreadedProc::RunProcess(EarObjectPtr data)
{
	if ((numThreads < 2) || !MODULE_THREAD_ENABLED(data))
		return(CXX_BOOL(RunProcessStandard_ModuleMgr(data)));
	return(RunThreadEnabled(data));

}

/******************************************************************************/
/****************************** General Routines ******************************/
/******************************************************************************/

/*************************** RunProcess ***************************************/

/*
 */
 
BOOLN
RunProcess_RunThreadedProc(EarObjectPtr data)
{
	return(runThreadedProc->RunProcess(data));

}

