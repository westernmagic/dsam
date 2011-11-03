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

static wxInitializer	*initializer = NULL;

/******************************************************************************/
/*************************** Subroutines and functions ************************/
/******************************************************************************/

/**************************** InitWxWidgets ***********************************/

/*
 * This routine initialises wxWidgets, but ensures that it is only done once
 * If it is done more than once, then an error will occur - contrary to the
 * wxWidgets documentation.
 */

bool
InitWxWidgets(void)
{
	if (initializer)
		return(true);
	initializer = new wxInitializer;

	if (!*initializer) {
		NotifyError(wxT("main: Failed to initialize the wxWidgets library, ")
		  wxT("aborting."));
		return(false);
	}
	return(true);

}

/******************************************************************************/
/*************************** Main routine *************************************/
/******************************************************************************/

SignalDataPtr
RunDSAMSim(WChar *simFile, WChar *parameterOptions, SignalDataPtr inSignal)
{
	bool	staticTimeFlag = false;
	int		numChannels = 0, interleaveLevel = 1;
	ChanLen	length = 0;
	double	*inputMatrixPtr = NULL, dt = 0.0, outputTimeOffset = 0.0;
	EarObjectPtr	audModel;
	SignalDataPtr	outSignal;
	MatInSignal		*inputSignal = NULL;

//	SetErrorsFile_Common(wxT("screen"), OVERWRITE);
//	SetDPrintFunc(MyDPrint);
//	SetNotifyFunc(MyNotify);
//	if (AnyBadArgument(nrhs, prhs))
//		return;
//
//	simFile = GetString(prhs[SIM_FILE]);
//	parameterOptions = (nrhs > PARAMETER_OPTIONS)? GetString(prhs[
//	  PARAMETER_OPTIONS]): (WChar *) wxT("");
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
//	if (inputMatrixPtr) {
//		if ((inputSignal = new MatInSignal((Float *) inputMatrixPtr, numChannels,
//		  interleaveLevel, length, dt, staticTimeFlag, outputTimeOffset)) == NULL) {
//			NotifyError(wxT("%s: Failed to initialise input signal."), PROGRAM_NAME);
//			return;
//		}
//	}
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
	return(_OutSig_EarObject(audModel));

}
