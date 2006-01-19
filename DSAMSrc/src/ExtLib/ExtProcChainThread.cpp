/******************
 *		
 * File:		ExtProcChainThread.h
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
#include "UtSSSymbols.h"
#include "UtSSParser.h"
#include "UtDatum.h"

#include "ExtProcThread.h"
#include "ExtProcChainThread.h"

//#define DEBUG	1

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

ProcChainThread::ProcChainThread(int theIndex, int offset, int numChannels,
  DatumPtr start, wxMutex *mutex, wxCondition *condition,
  int *theThreadCount): ProcThread(theIndex, offset, numChannels, mutex,
  condition, theThreadCount)
{
	DatumPtr	pc;

	simulation = start;

	for (pc = simulation; pc != simulation->passedThreadEnd; pc = pc->next) {
		if (pc->type == PROCESS) {
#			if DEBUG
			printf("ProcChainThread::ProcChainThread: Debug: Process '%s' "
			  "listed: Main outsignal = %lx.\n", pc->label, (unsigned long) pc->
			  data->outSignal);
#			endif
			ConfigProcess(pc->data);
		}
	}

}

/****************************** Entry *****************************************/

// The thread execution starts here.
// This is a virtual function used by the WxWin threading code.

void *
ProcChainThread::Entry()
{
#	if DEBUG
	static const char *funcName = "ProcChainThread::Entry";
	printf("%s: Debug: T[%d]: labels '%s' -> '%s'\n", funcName,
	  GetIndex(), simulation->label, (simulation->passedThreadEnd)? simulation->
	  passedThreadEnd->label: "?");
	clock_t	processStart = clock();
	printf("%s: Debug: T[%d]: Starting simulation chain at %g s\n", funcName,
	  GetIndex(), ELAPSED_TIME(startTime, processStart));
#	endif
	bool ok = CXX_BOOL(ExecuteStandard_Utility_Datum(simulation, simulation->
	  passedThreadEnd, GetIndex()));
#	if DEBUG
	clock_t	processFinish = clock();
	printf("%s: T[%d]: Finished simulation chain at %g s, %g s elapsed.\n",
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

//void
//ProcChainThread::OnExit()
//{
//	ProcThread::OnExit();
//
//}

/******************************************************************************/
/****************************** General Routines ******************************/
/******************************************************************************/

