/**********************
 *
 * File:		RunDSAMSim.cpp
 * Purpose:		This is the main routine for the scripting language interface
 * Comments:
 * Author:		L. P. O'Mard
 * Created:		16 Feb 2003
 * Updated:
 * Copyright:	(c) 2003, University of Essex
 *
 *********************/

#if wxUSE_GUI
    #error "This sample can't be compiled in GUI mode."
#endif // wxUSE_GUI

#include <wx/socket.h>
#include <wx/app.h>
#include <mex.h>

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

#define	STR_BUFFER_LEN(MX_STR)	(mxGetM(MX_STR) * mxGetN(MX_STR) * \
		  sizeof(mxChar) + 1)

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

/**************************** MyDPrint ****************************************/

/*
 * This prints an error message and allows greater flexibilty with error
 * messages.
 */

void
MyDPrint(const WChar *format, va_list args)
{
	WChar 	workStr[LONG_STRING];

	DSAM_vsnprintf(workStr, LONG_STRING, format, args);
	mexPrintf(ConvUTF8_Utility_String(workStr));

}

/**************************** MyNotify ****************************************/

/*
 * This prints an error message and allows greater flexibilty with error
 * messages.
 */

void
MyNotify(const WChar *message, CommonDiagSpecifier type)
{
	const WChar	*heading;
	WChar	workStr[LONG_STRING];

	if (!GetDSAMPtr_Common()->notificationCount) {
		SetDiagMode(COMMON_DIALOG_DIAG_MODE);
		heading = wxT("\nDiagnostics:-\n");
	} else
		heading = wxT("");
	Snprintf_Utility_String(workStr, LONG_STRING, wxT("%s%s: %s\n"), heading, DiagnosticTitle(
	  type), message);
	mexPrintf(ConvUTF8_Utility_String(workStr));

}

/*************************** PrintHelp ****************************************/

/*
 * Print the help message.
 */

void
PrintHelp(void)
{
	mexErrMsgTxt(USAGE_MESSAGE);

}

/**************************** CheckDoubleField ********************************/

bool
CheckDoubleField (const mxArray *info, const WChar *field)
{
	static const WChar *funcName = wxT("CheckDoubleField");
	const mxArray	*entry;

	if ((entry = mxGetField(info, 0, ConvUTF8_Utility_String(field))) == NULL) {
		NotifyError(wxT("%s: Structure '%s' element not present."), funcName,
		  field);
		return(false);
	}
	if (!mxIsDouble(entry)) {
		NotifyError(wxT("%s: Structure '%s' element must be a real value."),
		  funcName, field);
		return(false);
	}
	return(true);

}

/**************************** AnyBadArgument **********************************/

bool
AnyBadArgument(int nrhs, const mxArray *prhs[])
{
	static const WChar *funcName = wxT("AnyBadArgument");
	if (nrhs < 1)
		PrintHelp();

	if (!mxIsChar(prhs[SIM_FILE])) {
		NotifyError(wxT("%s: The <file name> argument (no. %d) must be a ")
		  wxT("string."), funcName, SIM_FILE);
		return(true);
	}
	if (nrhs > PARAMETER_OPTIONS) {
		if (!mxIsChar(prhs[PARAMETER_OPTIONS])) {
			NotifyError(wxT("%s: The <paramter options> argument (no. %d) must ")
			  wxT("be a string."),  funcName, PARAMETER_OPTIONS);
			return(true);
		}
	}
	if (nrhs > INPUT_SIGNAL) {
		if (!mxIsDouble(prhs[INPUT_SIGNAL])) {
			NotifyError(wxT("%s: The <signal> argument must be (no. %d) real ")
			  wxT("matrix type."),  funcName, INPUT_SIGNAL);
			return(true);
		}
	}
	if (nrhs > INFO_STRUCT) {
		const mxArray *info = prhs[INFO_STRUCT];

		CheckDoubleField(info, wxT("dt"));
	}
	return false;

}

/****************************** GetString *************************************/

/*
 * Returns a C string from a MX string.
 */

WChar *
GetString(const mxArray *str)
{
	static const WChar *funcName = wxT("GetString");
	char	*s;
	WChar	*s2;
	int		bufferLength;

	bufferLength = (int) STR_BUFFER_LEN(str) ;
	s = (char *) mxCalloc(bufferLength + 1, sizeof(mxChar));
	if (mxGetString(str, s, bufferLength) != 0) {
		NotifyError(wxT("%s: Not enough space for string. String is ")
		  wxT("truncated."), funcName);
		return(NULL);
	}
	if ((s2 = MBSToWCS2_Utility_String(s)) == NULL) {
		NotifyError(wxT("%s: Not enough space for string."), funcName);
		free(s);
		return(NULL);
	}
	mxFree(s);
	return(s2);

}

/****************************** GetOutputInfoStruct ***************************/

/*
 * This routine returns the info structure using the data EarObjects output
 * signal.
 */

mxArray *
GetOutputInfoStruct(SignalDataPtr signal)
{
	//static const char *funcName = "GetOutputInfoStruct";
	const char	*fieldNames[] = {"dt", "staticTimeFlag", "length",
					 "labels", "numChannels", "numWindowFrames",
					 "outputTimeOffset", "interleaveLevel",
					 "wordSize", NULL};
	const char	**p;
	int		i, numFields = 0;
	double	*lPtr;
	mxArray	*info, *labels;

	for (p = fieldNames; *p; p++)
		numFields++;
	numFields--;
	info = mxCreateStructMatrix(1, 1, numFields, fieldNames);

	mxSetField(info, 0, "dt", mxCreateScalarDouble(signal->dt));
	mxSetField(info, 0, "staticTimeFlag", mxCreateScalarDouble(signal->
	  staticTimeFlag));
	mxSetField(info, 0, "length", mxCreateScalarDouble(signal->length));
	labels = mxCreateDoubleMatrix(1, signal->numChannels, mxREAL);
	lPtr = mxGetPr(labels);
	for (i = 0; i < signal->numChannels; i++)
		*lPtr++ = signal->info.chanLabel[i];
	mxSetField(info, 0, "labels", labels);
	mxSetField(info, 0, "numChannels", mxCreateScalarDouble(signal->
	  numChannels));
	mxSetField(info, 0, "numWindowFrames", mxCreateScalarDouble(signal->
	  numWindowFrames));
	mxSetField(info, 0, "outputTimeOffset", mxCreateScalarDouble(signal->
	  outputTimeOffset));
	mxSetField(info, 0, "interleaveLevel", mxCreateScalarDouble(signal->
	  interleaveLevel));
	mxSetField(info, 0, "wordSize", mxCreateScalarDouble(2.0));

	return info;

}

/****************************** GetOutputSignalMatrix *************************/

/*
 * This routine returns the signal matrix using the data EarObjects output
 * signal.
 */

mxArray *
GetOutputSignalMatrix(SignalDataPtr signal)
{
	register double		*outPtr;
	register ChanData	*inPtr;
	int		chan;
	ChanLen	i;
	double	*mPtr;

	mxArray *m = mxCreateDoubleMatrix((int) signal->numChannels,
	  (int) signal->length, mxREAL);
	mPtr = mxGetPr(m);
	for (chan = 0; chan < signal->numChannels; chan++) {
		inPtr = signal->channel[chan];
		outPtr = mPtr + chan;
		for (i = 0; i < signal->length; i++, outPtr += signal->numChannels)
			*outPtr = *inPtr++;
	}
	return m;

}

/******************************************************************************/
/*************************** Main routine *************************************/
/******************************************************************************/

#define GET_INFO_PAR(NAME, DEF_VALUE) ((mxGetField(info, 0, (NAME)))? \
		  *mxGetPr(mxGetField(info, 0, (NAME))): (DEF_VALUE))
void
mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])

{
	BOOLN	staticTimeFlag = FALSE;
	WChar	*simFile, *parameterOptions;
	int		numChannels = 0, interleaveLevel = 1;
	ChanLen	length = 0;
	double	*inputMatrixPtr = NULL, dt = 0.0, outputTimeOffset = 0.0;
	EarObjectPtr	audModel;
	SignalDataPtr	outSignal;
	MatInSignal		*inputSignal = NULL;

//	if (!InitWxWidgets())
//		return;
	SetErrorsFile_Common(wxT("screen"), OVERWRITE);
	SetDPrintFunc(MyDPrint);
	SetNotifyFunc(MyNotify);
	if (AnyBadArgument(nrhs, prhs))
		return;

	simFile = GetString(prhs[SIM_FILE]);
	parameterOptions = (nrhs > PARAMETER_OPTIONS)? GetString(prhs[
	  PARAMETER_OPTIONS]): (WChar *) wxT("");

	if (nrhs > INFO_STRUCT) {
		const mxArray *info = prhs[INFO_STRUCT];
		inputMatrixPtr = mxGetPr(prhs[INPUT_SIGNAL]);
 		numChannels = (int) mxGetM(prhs[INPUT_SIGNAL]);
		length = (ChanLen) mxGetN(prhs[INPUT_SIGNAL]);
		dt =  *mxGetPr(mxGetField(info, 0, "dt"));
		staticTimeFlag = (BOOLN) GET_INFO_PAR("staticTimeFlag", FALSE);
		outputTimeOffset = GET_INFO_PAR("outputTimeOffset", 0.0);
		interleaveLevel = (int) GET_INFO_PAR("interleaveLevel",
		  DSAMMAT_AUTO_INTERLEAVE_LEVEL);
	}
	if (inputMatrixPtr) {
		if ((inputSignal = new MatInSignal((Float *) inputMatrixPtr, numChannels,
		  interleaveLevel, length, dt, staticTimeFlag, outputTimeOffset)) == NULL) {
			NotifyError(wxT("%s: Failed to initialise input signal."), PROGRAM_NAME);
			return;
		}
	}
	MatMainApp	mainApp((WChar *) PROGRAM_NAME, simFile, parameterOptions,
	  inputSignal);
	if (!mainApp) {
		NotifyError(wxT("%s: Could not initialise the MatMainApp module."),
		  PROGRAM_NAME);
		return;
	}
	if (!mainApp.Main()) {
		NotifyError(wxT("%s: Could not run simulation."), PROGRAM_NAME);
		return;
	}
	audModel = mainApp.GetSimProcess();

	free(simFile);
	if (nrhs > PARAMETER_OPTIONS)
		free(parameterOptions);

	if (audModel && ((outSignal = _OutSig_EarObject(audModel)) != NULL)) {
		plhs[0] = GetOutputSignalMatrix(audModel->outSignal);
		plhs[1] = GetOutputInfoStruct(audModel->outSignal);
	}
	if (inputSignal)
		delete inputSignal;
}
