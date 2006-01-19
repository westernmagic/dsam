/******************
 *		
 * File:		ExtRunThreadedProc.cpp
 * Purpose: 	This module runs a processes using threads of possible.
 * Comments:	It was decided to create this module rather than creating
 *				a replacement ExecuteSimulation routine.
 *				21-10-05 LPO sub simulation scripts will not currently run
 *				threaded.
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
#include <wx/listimpl.cpp>

#include "GeCommon.h"
#include "GeSignalData.h"
#include "GeEarObject.h"
#include "GeUniParMgr.h"
#include "GeModuleMgr.h"
#include "UtAppInterface.h"
#include "UtSSSymbols.h"
#include "UtSSParser.h"
#include "UtDatum.h"
#include "UtDynaList.h"

#include "ExtProcThread.h"
#include "ExtSingleProcThread.h"
#include "ExtProcChainThread.h"
#include "ExtRunThreadedProc.h"

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

RunThreadedProc	*runThreadedProc = NULL;
#if DEBUG
	clock_t	startTime;
#endif

/******************************************************************************/
/****************************** Functions *************************************/
/******************************************************************************/

/****************************** Constructor ***********************************/

RunThreadedProc::RunThreadedProc(void)
{
#	if DEBUG
	printf("RunThreadedProc::RunThreadedProc: Debug: Entered\n");
	startTime = clock();
#	endif
	numThreads = wxThread::GetCPUCount();
	SetThreadMode((GetPtr_AppInterface())? GetPtr_AppInterface()->threadMode:
	  APP_INT_THREAD_MODE_PROCESS);
#	if DEBUG
	printf("RunThreadedProc::RunThreadedProc: Debug: %d CPU's available, "
	  "thread mode: %d\n", numThreads, threadMode);
#	endif
	runThreadedProc = this;

}

/****************************** SetNumThreads *********************************/

void
RunThreadedProc::SetNumThreads(int theNumThreads)
{
#	if DEBUG
	printf("RunThreadedProc::SetNumThreads: Debug: Entered\n");
#	endif
	numThreads = (theNumThreads < 1)? wxThread::GetCPUCount(): theNumThreads;

}

/****************************** SetThreadMode *********************************/

bool
RunThreadedProc::SetThreadMode(int mode)
{
	static const char *funcName = "RunThreadedProc::SetThreadMode";

	threadMode = mode;
	switch (threadMode) {
	case APP_INT_THREAD_MODE_CHANNEL_CHAIN:
		SetRunProcess_ModuleMgr(RunProcessStandard_ModuleMgr);
		SetExecute_Utility_Datum(Execute_RunThreadedProc);
		break;
	case APP_INT_THREAD_MODE_PROCESS:
		SetRunProcess_ModuleMgr(RunProcess_RunThreadedProc);
		SetExecute_Utility_Datum(ExecuteStandard_Utility_Datum);
		break;
	default:
		NotifyError("%s: Unknown thread mode (%d).", funcName, mode);
		return(false);
	} /* switch */
	return(true);
		
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
	printf("%s: Debug: Working on '%s'\n", funcName, data->processName);
#	endif
	data->initThreadRunFlag = true;
	if ((data->numThreads > 1) && ((data->numThreads != numThreads) || data->
	  updateProcessFlag))
		FreeThreadProcs_EarObject(data);
	data->numThreads = numThreads;
#	if DEBUG
	printf("%s: Debug: Main outsignal [1] = %lx.\n", funcName, (unsigned 
	  long) data->outSignal);
#	endif
	ok = CXX_BOOL(RunProcessStandard_ModuleMgr(data));
#	if DEBUG
	printf("%s: Debug: Main outsignal [2] = %lx.\n", funcName, (unsigned 
	  long) data->outSignal);
#	endif
	data->initThreadRunFlag = false;
	data->useThreadsFlag = (data->module->threadMode ==
	  MODULE_THREAD_MODE_NONE)? FALSE: (data->outSignal->numChannels > 1);
	if (!data->useThreadsFlag)
		data->numThreads = 1;
	if (!ok)
		NotifyError("%s: Could not do pre-thread process initialisation run.",
		  funcName);
	return(ok);

}

/****************************** InitThreadProcesses ***************************/

bool
RunThreadedProc::InitThreadProcesses(EarObjectPtr data)
{
	static const char *funcName = "RunThreadedProc::InitThreadProcesses";

#	if DEBUG
	printf("%s: Debug: Thread processes initialised for '%s'\n", funcName,
	  data->processName);
#	endif
	data->clientData = new ProcessSignalling;
	if (!InitThreadProcs_EarObject(data)) {
		wxLogFatalError("%s: Could not initialise thread EarObjects.",
		  funcName);
		return(false);
		delete (ProcessSignalling *) data->clientData;
	}
	return(true);

}

/****************************** SetThreadDistribution *************************/

void
RunThreadedProc::SetThreadDistribution(int numChannels)
{
	if (numChannels < numThreads)
		numThreads = numChannels;
	chansPerThread = numChannels / numThreads;
#	if DEBUG
	printf("RunThreadedProc::SetThreadDistribution: Debug: chansPerThread = "
	  "%d\n", chansPerThread);
#	endif
	remainderChans = numChannels % numThreads;
	threadCount = numThreads;

}

/****************************** RunProcess ************************************/

/*
 * Run a process which is enabled to use threading.
 * The Process 'threadRunFlag' must be set before the
 * 'InitThreadProcs_EarObject'routine is called, so that each thread has this
 * flag set correctly.
 */

bool
RunThreadedProc::RunProcess(EarObjectPtr data)
{
	static const char *funcName = "RunThreadedProc::RunProcess";

	if ((numThreads < 2) || !MODULE_THREAD_ENABLED(data))
		return(CXX_BOOL(RunProcessStandard_ModuleMgr(data)));

	int		i;
	wxMutex	mutex;
	wxCondition	condition(mutex);
	SingleProcThread	*procThread;

#	if DEBUG
	printf("%s: Debug: Entered.\n", funcName);
#	endif
	if (!PreThreadProcessInit(data))
		return(false);

	if (!data->useThreadsFlag)
		return(CXX_BOOL(RunProcessStandard_ModuleMgr(data)));
	mutex.Lock();
	SetThreadDistribution(data->outSignal->numChannels);

	if (!InitThreadProcesses(data)) {
		wxLogFatalError("%s: Could not initialise thread processes.", funcName);
		return(false);
	}
	for (i = 0; i < numThreads; i++) {
		procThread = new SingleProcThread(i, i * chansPerThread,
		  chansPerThread + ((i == numThreads - 1)? remainderChans: 0), data,
		  &mutex, &condition, &threadCount);
		if (procThread->Create() != wxTHREAD_NO_ERROR) {
			wxLogFatalError("%s: Can't create process thread!", funcName);
			procThread->Delete();
		}
		if (procThread->Run() != wxTHREAD_NO_ERROR) {
			wxLogError("%s: Cannot start process thread.", funcName);
			procThread->Delete();
		}
	}
	while (threadCount) {
#		if DEBUG
		printf("%s: Debug: waiting for %d threads to finish.\n", funcName,
		  threadCount);
#		endif
		condition.Wait();
#		if DEBUG
		printf("%s: Debug: Thread finished at %g s.\n", funcName,
		  ELAPSED_TIME(startTime, clock()));
#		endif
	}
	data->threadRunFlag = FALSE;
	RestoreProcess(data);
#	if DEBUG
	printf("%s: Debug: Finished\n", funcName);
#	endif
	return(true);

}

/****************************** PreThreadSimulationInit ***********************/

/*
 * This routine ranges through the entire simulation running the pre-thread
 * process initialisation.
 * It assumes that the simulation has been correctly initialised.
 */

bool
RunThreadedProc::PreThreadSimulationInit(DatumPtr start, bool *brokenChain)
{
	static const char *funcName = "RunThreadedProc::PreThreadSimulationInit";
	bool	linkBreak;
	int		chainCount = 0;
	DatumPtr	pc, pc1, pc2, threadStartPc = NULL;

	for (pc = start, threadStartPc = NULL; pc != NULL; pc = pc->next) {
		linkBreak = FALSE;
		switch (pc->type) {
		case PROCESS:
#			if DEBUG
			printf("%s: Debug: label '%s'\n", funcName, pc->label);
			printf("%s: Debug: Main outsignal = %lx.\n", funcName, (unsigned
			  long) pc->data->outSignal);
#			endif
			if (!PreThreadProcessInit(pc->data))
				return(false);
#			if DEBUG
			printf("%s: Debug: Main outsignal [1] = %lx.\n", funcName, (unsigned
			  long) pc->data->outSignal);
#			endif
			UpdateCustomers_EarObject(pc->data);
#			if DEBUG
			printf("%s: Debug: Main outsignal [2] = %lx.\n", funcName, (unsigned
			  long) pc->data->
			  outSignal);
#			endif
			if (pc->data->useThreadsFlag && !InitThreadProcesses(pc->data)) {
				wxLogFatalError("%s: Could not initialise thread processes.",
				  funcName);
				return(false);
			}
			linkBreak = (!pc->data->module->threadMode);
			SetUpdateProcessFlag_EarObject(pc->data, FALSE);
			pc2 = pc;
			break;
		case REPEAT:
			if (!PreThreadSimulationInit(pc->next, &linkBreak)) {
				NotifyError("%s: Could not do pre-thread simulation "
				  "initialisation.", funcName);
				return(false);
			}
			pc->threadSafe = !linkBreak;
			pc2 = GetFirstProcessInst_Utility_Datum(pc);
			break;
		case STOP:
			return(true);
		default:
			pc2 = pc;
		} /* switch */
		if (!threadStartPc) {
			threadStartPc = pc;
			chainCount = 1;
		} else {
			pc1 = (threadStartPc->type == PROCESS)? threadStartPc:
			  GetPreviousProcessInst_Utility_Datum(threadStartPc);
			if (!pc1 || !pc2 || (!linkBreak && ((pc2->type == RESET) ||
			  (pc1->data->outSignal->numChannels == pc2->data->outSignal->
			  numChannels))))
				chainCount++;
			else {
#				if DEBUG
				printf("%s: Debug: label '%s' marked as thread start, %d "
				  "processes\n", funcName, threadStartPc->label, chainCount);
#				endif
				threadStartPc->passedThreadEnd = pc;
				threadStartPc = pc;
				chainCount = 1;
				*brokenChain = true;
			}
		}
		if (pc->type == REPEAT)
			pc = pc->u.loop.stopPC;
	}
	return(true);

}

/****************************** RestoreProcess ********************************/

void
RunThreadedProc::RestoreProcess(EarObjectPtr process)
{
	if (!process->localOutSignalFlag)
		return;
#	if DEBUG
	printf("RunThreadedProc::RestoreProcess: Debug: '%s' offset %d, "
	  "threadIndex = %d.\n", (process->processName)? process->processName:
	  "unknown", process->outSignal->offset, process->threadIndex);
#	endif
	if (!process->useThreadsFlag)
		return;
	if (!process->threadIndex)	{
#		if DEBUG
		printf("RunThreadedProc::RestoreProcess: Debug: Restoring process '%s' "
		  "to offset '%d', numChannels '%d'.\n", process->processName, 0,
		  process->outSignal->origNumChannels);
#		endif
		int		i;
		process->outSignal->offset = 0;
		process->outSignal->numChannels = process->outSignal->origNumChannels;
		for (i = 0; i < process->numSubProcesses; i++) {
			process->subProcessList[i]->outSignal->offset = 0;
			process->subProcessList[i]->outSignal->numChannels =
			  process->outSignal->origNumChannels;
		}
	}

}

/****************************** RestoreSimulation *****************************/

void
RunThreadedProc::RestoreSimulation(DatumPtr simulation)
{
	DatumPtr	pc;

	for (pc = simulation; pc != simulation->passedThreadEnd; pc = pc->next)
		if (pc->type == PROCESS)
			RestoreProcess(pc->data);

}

/****************************** CleanUpThreadRuns *****************************/

/*
 */

DatumPtr
RunThreadedProc::CleanUpThreadRuns(DatumPtr start)
{
	DatumPtr	pc, lastInstruction = NULL;

	if (start == NULL)
		return NULL;
	for (pc = start; pc != start->passedThreadEnd; pc = pc->next) {
		if  (pc->type == PROCESS)
			pc->data->threadRunFlag = FALSE;
		lastInstruction = pc;
	}
	return(lastInstruction);

}

/****************************** ExecuteMultiThreadChain ***********************/

/*
 * This routine executes a threaded chain.
 */

DatumPtr
RunThreadedProc::ExecuteMultiThreadChain(DatumPtr start)
{
	static const char *funcName = "RunThreadedProc::ExecuteMultiThreadChain";
	int		i;
	DatumPtr	pc, lastInstruction = NULL;
	wxMutex		mutex;
	wxCondition	condition(mutex);
	ProcChainThread	*procChainThread;

	mutex.Lock();
	if ((pc = GetFirstProcessInst_Utility_Datum(start)) == NULL) {
		pc = start;
		SetThreadDistribution(1);
	} else
		SetThreadDistribution(pc->data->outSignal->numChannels);
#	if DEBUG
	printf("%s: Debug: numThreads = %d for %s\n", funcName, numThreads,
	  start->label);
	clock_t chainStart;
#	endif
	for (i = 0; i < numThreads; i++) {
		procChainThread = new ProcChainThread(i, i * chansPerThread,
		  chansPerThread + ((i == numThreads - 1)? remainderChans: 0),
		  start, &mutex, &condition, &threadCount);
		if (procChainThread->Create() != wxTHREAD_NO_ERROR) {
			wxLogFatalError("%s: Can't create process thread!",
			  funcName);
			procChainThread->Delete();
		}
#		if DEBUG
		chainStart = clock();
		printf("%s: Debug: T[%d]: Started for %s, time = %g s\n", funcName,
		  procChainThread->GetIndex(), start->label, ELAPSED_TIME(
		  startTime, chainStart));
#		endif
		if (procChainThread->Run() != wxTHREAD_NO_ERROR) {
			wxLogError("%s: Cannot start process thread.", funcName);
			procChainThread->Delete();
		}
	}
	while (threadCount) {
#		if DEBUG
		printf("%s: Debug: waiting for %d threads to finish.\n",
		  funcName, threadCount);
#		endif
		condition.Wait();
#		if DEBUG
		clock_t chainFinished = clock();
		printf("%s: Debug: Thread finished at %g s, %g s elapsed.\n", funcName,
		  ELAPSED_TIME(startTime, chainFinished),
		  ELAPSED_TIME(chainStart, chainFinished));
#		endif
	}
	lastInstruction = CleanUpThreadRuns(start);
	RestoreSimulation(start);
	return(lastInstruction);

}

/****************************** ExecuteStandardChain **************************/

/*
 * This routine executes a threaded chain.
 */

DatumPtr
RunThreadedProc::ExecuteStandardChain(DatumPtr start)
{
	/*static const char *funcName = "RunThreadedProc::ExecuteStandardChain";*/
	DatumPtr	lastInstruction;

	lastInstruction = ExecuteStandard_Utility_Datum(start, start->
	  passedThreadEnd, 0);
	CleanUpThreadRuns(start);
	return(lastInstruction);

}

/****************************** Execute ***************************************/

/*
 * This routine replaces the ExecuteStandard_Utility_Datum routine.
 */

DatumPtr
RunThreadedProc::Execute(DatumPtr start)
{
	static const char *funcName = "RunThreadedProc::Execute";
	bool	brokenChain = false;

	if (start == NULL)
		return NULL;
	if (numThreads < 2)
		return(ExecuteStandard_Utility_Datum(start, NULL, 0));

	DatumPtr	pc, lastInstruction = NULL;

#	if DEBUG
	printf("%s: Entered\n", funcName);
#	endif

#	if DEBUG
	printf("%s: start [0] = %lx\n", funcName, (unsigned long) start);
#	endif
	if (!PreThreadSimulationInit(start, &brokenChain)) {
		NotifyError("%s: Could not do pre-thread simulation initialisation.",
		  funcName);
		return(NULL);
	}
#	if DEBUG
	printf("%s: start [1] = %lx\n", funcName, (unsigned long) start);
#	endif
	SetExecute_Utility_Datum(ExecuteStandard_Utility_Datum);
	SetRunProcess_ModuleMgr(RunProcessStandard_ModuleMgr);
	SetResetProcess_EarObject(ResetProcess_ModuleMgr);
	for (pc = start; pc != NULL; pc = pc->passedThreadEnd) {
		switch (pc->type) {
		case PROCESS: {
			if ((pc->data->module->threadMode == MODULE_THREAD_MODE_NONE) ||
			  !pc->data->useThreadsFlag)
				lastInstruction = ExecuteStandardChain(pc);
			else
				lastInstruction = ExecuteMultiThreadChain(pc);
			break; }
		case RESET:
			ResetProcess_EarObject(pc->data);
			break;
		case STOP:
			return (pc);
		case REPEAT:
			if (!pc->threadSafe)
				lastInstruction = ExecuteStandardChain(pc);
			else
			  	ExecuteMultiThreadChain(pc);
			break;
		default:
			break;
		} /* switch */
		if (pc == NULL)
			break;
	}
	SetRunProcess_ModuleMgr(RunProcessStandard_ModuleMgr);
	SetExecute_Utility_Datum(Execute_RunThreadedProc);
	SetResetProcess_EarObject(ResetProcessStandard_EarObject);
	return(lastInstruction);

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

/*************************** Execute ******************************************/

/*
 */
 
DatumPtr
Execute_RunThreadedProc(DatumPtr start, DatumPtr passedEnd, int threadIndex)
{
	DatumPtr	pc; 
	passedEnd = NULL;		/* Stop compiler complaining. */
	threadIndex = 0;
	pc = runThreadedProc->Execute(start);
	
	return(pc);

}
