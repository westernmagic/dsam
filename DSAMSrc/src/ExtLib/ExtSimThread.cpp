/******************
 *		
 * File:		ExtSimThread.cpp
 * Purpose: 	Simulation thread class module.
 * Comments:	This was re-named from the RunMgr Class
 *				The 'RunMgr' class allows the MainSimulation program to send
 *				run information to the simulation manager.  The class contains
 *				classes that can be overridden.
 *				26-01-99 LPO: Introduced the 'ListSimParameters' function which
 *				prints the parameters for a simulation.
 *				Introduced the 'StautsChanged' function which indicates when
 *				the 'CheckProgInitialisation' routine needs to be run to
 *				re-initialise the simulation.
 *				29-01-99 LPO: Added a universal parameter list for the program
 *				parameters.
 *				05-05-99 LPO: Introduced the 'ListProgParameters' routine.
 *				14-05-99 LPO: Introduced the 'GetSimParFile' routine which
 *				returns a pointer to the simulation parameter file name.
 * Author:		L. P. O'Mard
 * Created:		20 Sep 1998
 * Updated:		14 May 1999
 * Copyright:	(c) 1999-2001, University of Essex
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

#include "ExtMainApp.h"
#include "ExtSimThread.h"

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

SimThread::SimThread(void): wxThread()
{
	SetTestDestroy_ModuleMgr(TestDestroy_SimThread);

}

/****************************** Entry *****************************************/

// The thread execution starts here.
// This is a virtual function used by the WxWin threading code.

void *
SimThread::Entry()
{
	SetInterruptRequestStatus_Common(FALSE);
	dSAMMainApp->RunSimulation();
	GetPtr_AppInterface()->simulationFinishedFlag = TRUE;

	return(NULL);

}


/****************************** OnExit ****************************************/

// called when the thread exits - whether it terminates normally or is
// stopped with Delete() (but not when it is Kill()ed!)

void
SimThread::OnExit()
{
	wxCriticalSectionLocker	locker(dSAMMainApp->mainCritSect);

	dSAMMainApp->simThread = NULL;
	SetTestDestroy_ModuleMgr(NULL);
	dSAMMainApp->SetRunIndicators(FALSE);
	SwitchGUILocking_Common(FALSE);
	SetInterruptRequestStatus_Common(FALSE);

}

/****************************** SuspendDiagnostics ****************************/

void
SimThread::SuspendDiagnostics(void)
{
	wxCriticalSectionLocker locker(diagsCritSect);
	SetDiagMode(COMMON_OFF_DIAG_MODE);

}

/******************************************************************************/
/****************************** General Routines ******************************/
/******************************************************************************/

/*************************** TestDestroy **************************************/

/*
 * This routine is a dummy routine for the TestDestroy function required in
 * thread programs to notify that threads need to end.
 */
 
BOOLN
TestDestroy_SimThread(void)
{
	return(dSAMMainApp->simThread->MyTestDestroy());

}

