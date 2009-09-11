/******************
 *
 * File:		ExtRunThreadedProc.h
 * Purpose: 	This module runs a processes using threads of possible.
 * Comments:	It was deceided to create this module rather than creating
 *				a replacement ExecuteSimulation routine.
 * Author:		L. P. O'Mard
 * Created:		30 Sep 2004
 * Updated:
 * Copyright:	(c) 2004, CNBH, University of Essex
 *
 ******************/

#ifndef	_EXTRUNTHREADEDPROC_H
#define _EXTRUNTHREADEDPROC_H	1

#include "GeSignalData.h"
#include "GeEarObject.h"

/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Class definitions ********************************/
/******************************************************************************/

/*************************** RunThreadedProc **********************************/

class RunThreadedProc {

	int		numThreads, chansPerThread, remainderChans, threadCount;
	int		threadMode;

  public:
	RunThreadedProc(void);

	DatumPtr	CleanUpThreadRuns(DatumPtr start);
	bool	DetermineChannelChains(DatumPtr start, bool *brokenChain);
	DatumPtr	Execute(DatumPtr start);
	DatumPtr	ExecuteMultiThreadChain(DatumPtr start);
	DatumPtr	ExecuteStandardChain(DatumPtr start);
	bool	InitialiseProcesses(DatumPtr start);
	bool	InitThreadProcesses(EarObjectPtr data);
	bool	PreThreadSimulationInit(DatumPtr start, bool *brokenChain);
	bool	PreThreadProcessInit(EarObjectPtr data);
	void	RestoreProcess(EarObjectPtr process);
	void	RestoreSimulation(DatumPtr simulation);
	bool	RunProcess(EarObjectPtr data);
	void	SetNumThreads(int theNumThreads);
	void	SetThreadDistribution(int numChannels);
	bool	SetThreadMode(int mode);

};

/******************************************************************************/
/*************************** External variables *******************************/
/******************************************************************************/

extern RunThreadedProc	*runThreadedProc;

/******************************************************************************/
/*************************** Subroutine declarations **************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Call back prototypes *****************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

DatumPtr	Execute_RunThreadedProc(DatumPtr start, DatumPtr passedEnd,
			  int threadIndex);
DatumPtr	ExecuteSubSims_RunThreadedProc(DatumPtr start, DatumPtr passedEnd,
			  int threadIndex);

BOOLN	RunProcess_RunThreadedProc(EarObjectPtr data);

BOOLN	SetSubSimThreadFlag_RunThreadedProc(DatumPtr pc);

__END_DECLS

#endif
