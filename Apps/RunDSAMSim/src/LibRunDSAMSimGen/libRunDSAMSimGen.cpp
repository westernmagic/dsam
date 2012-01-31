/**********************
 *
 * File:		LibRunDSAMSimGen.cpp
 * Purpose:		This is the main routine for the scripting language interface
 * Comments:
 * Author:		L. P. O'Mard
 * Created:		01 Nov 2011
 * Updated:
 * Copyright:	(c) 2011, L. P. O'Mard
 *
 *********************/

#if wxUSE_GUI
    #error "This sample can't be compiled in GUI mode."
#endif // wxUSE_GUI

#include <stdio.h>
#include <wchar.h>
#include <iostream>
using namespace std;

#include <wx/socket.h>
#include <wx/app.h>

#include <GeCommon.h>
#include <GeSignalData.h>
#include <GeEarObject.h>
#include <GeUniParMgr.h>
#include <UtString.h>
#include <ExtIPCUtils.h>
#include <ExtMainApp.h>
#include <ExtIPCClient.h>
#include <UtDatum.h>
#include <UtAppInterface.h>

#include "MatInSignal.h"
#include "MatMainApp.h"
#include "libRunDSAMSimGen.h"

/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

#define	PROGRAM_NAME				wxT("RunDSAMSim")
#define	USAGE_MESSAGE \
  "RunDSAMSim:\n" \
  "\n" \
  "[data info] = RunDSAMSim(<sim file>, [<parameter settings>, [<signal>," \
  "...\n" \
  "                         <signal info.>]])\n" \
  "\n" \
  "<sim file>           Simulation file name (string).\n" \
  "<parameter settings> Parameter '<name> <value> ...' pairs (string).\n" \
  "<signal>             Data signal ([chan, samples] real matrix).\n" \
  "<signal info.>       Signal information (structure).\n"

/******************************************************************************/
/*************************** Global Variables *********************************/
/******************************************************************************/

SignalDataPtr	outSignal = NULL;
EarObjectPtr	outProcess = NULL;

/******************************************************************************/
/*************************** Subroutines and functions ************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Main routine *************************************/
/******************************************************************************/

SignalDataPtr
RunDSAMSim(WChar *simFile, WChar *parameterOptions, SignalDataPtr inSignal)
{
	bool	staticTimeFlag = false;
	int		numChannels = 0, interleaveLevel = 1;
	ChanLen	length = 0;
	EarObjectPtr	audModel, outProcess;
	MatInSignal		*inputSignal = NULL;

	SetDiagMode(COMMON_DIALOG_DIAG_MODE);
	SetErrorsFile_Common(wxT("screen"), OVERWRITE);
	SetDPrintFunc(DPrint_MatMainApp);
	SetNotifyFunc(Notify_MatMainApp);
	if (inSignal && ((inputSignal = new MatInSignal(inSignal, (outSignal &&
	  (outSignal == inSignal)))) == NULL)) {
		NotifyError(wxT("%s: Failed to initialise input signal."), PROGRAM_NAME);
		return(NULL);
	}
	MatMainApp	mainApp((WChar *) PROGRAM_NAME, simFile, parameterOptions,
	  inputSignal);
	if (!mainApp) {
		NotifyError(wxT("%s: Could not initialise the MatMainApp module."),
		  PROGRAM_NAME);
		return(NULL);
	}
	if (!mainApp.Main()) {
		NotifyError(wxT("%s: Could not run simulation."), PROGRAM_NAME);
		return(NULL);
	}
	audModel = mainApp.GetSimProcess();

	if (audModel) {
		if ((outProcess == NULL) && ((outProcess = Init_EarObject(wxT("NULL"))) ==
		  NULL)) {
			NotifyError(wxT("%s: Could not initialise output process."), PROGRAM_NAME);
			return(NULL);
		}
		ConnectOutSignalToIn_EarObject(audModel, outProcess);
		if (!InitOutTypeFromInSignal_EarObject(outProcess, 0)) {
			NotifyError(wxT("%s: Cannot initialise output channel."), PROGRAM_NAME);
			return(NULL);
		}
		InitOutDataFromInSignal_EarObject(outProcess);
		DisconnectOutSignalFromIn_EarObject(audModel, outProcess);
	}
	outProcess->localOutSignalFlag = FALSE;
	outSignal = _OutSig_EarObject(outProcess);
	return(outSignal);

}
