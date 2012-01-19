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

/*************************** DPrintStandard ***********************************/

/*
 * This routine prints out a diagnostic message, preceded by a bell sound.
 * It is used in the same way as the vprintf statement.
 * This is the standard version for ANSI C.
 */

void
DPrint_LibRunDSAMSimGen(const WChar *format, va_list args)
{
	CheckInitParsFile_Common();
	if (GetDSAMPtr_Common()->parsFile == stdout) {
		wxString str, fmt;
		fmt = format;
		fmt.Replace(wxT("%S"), wxT("%s"));	/* This is very simplistic - requires revision */
		str.PrintfV(fmt, args);

		if (GetDSAMPtr_Common()->diagnosticsPrefix)
			str = GetDSAMPtr_Common()->diagnosticsPrefix + str;
		std::cout << str.utf8_str();
	} else {
		if (GetDSAMPtr_Common()->diagnosticsPrefix)
			DSAM_fprintf(GetDSAMPtr_Common()->parsFile, STR_FMT,
			  GetDSAMPtr_Common()->diagnosticsPrefix);
		DSAM_vfprintf(GetDSAMPtr_Common()->parsFile, format, args);
	}

}

/***************************** Notify *****************************************/

/*
 * All notification messages are stored in the notification list.
 * This list is reset when the noficiationCount is zero.
 */

void
Notify_LibRunDSAMSimGen(const wxChar *message, CommonDiagSpecifier type)
{
	FILE	*fp;

	switch (type) {
	case COMMON_ERROR_DIAGNOSTIC:
		fp = GetDSAMPtr_Common()->errorsFile;
		break;
	case COMMON_WARNING_DIAGNOSTIC:
		fp = GetDSAMPtr_Common()->warningsFile;
		break;
	default:
		fp = stdout;
		break;
	}
	if (fp == stdout) {
		cout << wxConvUTF8.cWX2MB(message) << endl;
	} else {
		fprintf(fp, wxConvUTF8.cWX2MB(message));
		fprintf(fp, "\n");
	}

} /* Notify_LibRunDSAMSimGen */

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
	SetDPrintFunc(DPrint_LibRunDSAMSimGen);
	SetNotifyFunc(Notify_LibRunDSAMSimGen);
//	if (AnyBadArgument(nrhs, prhs))
//		return;
//
//	if (nrhs > INFO_STRUCT) {
//		const mxArray *info = prhs[INFO_STRUCT];
//		inputMatrixPtr = mxGetPr(prhs[INPUT_SIGNAL]);
// 		numChannels = (int) mxGetM(prhs[INPUT_SIGNAL]);
//		length = (ChanLen) mxGetN(prhs[INPUT_SIGNAL]);
//		dt =  *mxGetPr(mxGetField(info, 0, "dt"));
//		staticTimeFlag = (bool) GET_INFO_PAR("staticTimeFlag", false);
//		outputTimeOffset = GET_INFO_PAR("outputTimeOffset", 0.0);
//		interleaveLevel = (int) GET_INFO_PAR("interleaveLevel",
//		  DSAMMAT_AUTO_INTERLEAVE_LEVEL);
//	}
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

//	free(simFile);
//	if (nrhs > PARAMETER_OPTIONS)
//		free(parameterOptions);
//
//	if (audModel && ((outSignal = _OutSig_EarObject(audModel)) != NULL)) {
//		plhs[0] = GetOutputSignalMatrix(audModel->outSignal);
//		plhs[1] = GetOutputInfoStruct(audModel->outSignal);
//	}
//	if (inputSignal)
//		delete inputSignal;
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
