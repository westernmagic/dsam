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


#include "mex.h"

#include "GeCommon.h"
#include "GeSignalData.h"
#include "GeEarObject.h"
#include "DSAMMatrix.h"

/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

#define	PROGRAM_NAME				"RunDSAMSim"
#define	NULL_FIELD		-1000

/******************************************************************************/
/*************************** Macro Definitions ********************************/
/******************************************************************************/

#define	STR_BUFFER_LEN(MX_STR)	(mxGetM(MX_STR) * mxGetN(MX_STR) * \
		  sizeof(mxChar) + 1)

/******************************************************************************/
/*************************** Global Variables *********************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Subroutines and functions ************************/
/******************************************************************************/

/**************************** MyErrMsgTxt ************************************/

/*
 * This prints an error message and allows greater flexibilty with error
 * messages.
 */

void
MyErrMsgTxt(const char *format, ...)
{
	char 	workStr[LONG_STRING];
	va_list	args;

	Free_DSAMMatrix();
	va_start(args, format);
	vsnprintf(workStr, LONG_STRING, format, args);
	mexErrMsgTxt(workStr);
	va_end(args);

}

/*************************** PrintHelp ****************************************/

/*
 * Print the help message.
 */

void
PrintHelp(void)
{
	mexErrMsgTxt(
	  "RunDSAMSim:\n"
	  "\n"
	  "[data info] = RunDSAMSim(<sim file>, [<diag. mode>, [<parameter "
	  "settings>,...\n"
	  "                         [<signal>, [<signal info.>]]]])\n"
	  "\n"
	  "<sim file>           Simulation file name (string).\n"
	  "<diag. mode>         Dagnostic mode 'screen', 'off' or '<file name>'.\n"
	  "<parameter settings> Parameter '<name> <value> ...' pairs (string).\n"
	  "<signal>             Data signal ([chan, samples] real matrix).\n"
	  "<signal info.>       Signal information (structure).\n"
	);

}

/**************************** CheckDoubleField ********************************/

BOOLN
CheckDoubleField (const mxArray *info, char *field)
{
	static const char *funcName = "CheckDoubleField";
	const mxArray	*entry;

	if ((entry = mxGetField(info, 0, "dt")) == NULL)
		MyErrMsgTxt("%s: Structure '%s' element not present.", funcName, field);
	if (!mxIsDouble(entry))
		MyErrMsgTxt("%s: Structure '%s' element must be a real value.",
		  funcName, field);
	return(TRUE);

}

/**************************** AnyBadArgument **********************************/

BOOLN
AnyBadArgument(int nrhs, const mxArray *prhs[])
{
	static const char *funcName = "AnyBadArgument";
	if (nrhs < 1)
		PrintHelp();

	if (!mxIsChar(prhs[SIM_FILE]))
		MyErrMsgTxt("%s: The <file name> argument (no. %d) must be a string.",
		  funcName, SIM_FILE);
	if (nrhs > PARAMETER_OPTIONS) {
		if (!mxIsChar(prhs[PARAMETER_OPTIONS]))
			MyErrMsgTxt("%s: The <paramter options> argument (no. %d) must be "
			  "a string.",  funcName, PARAMETER_OPTIONS);
	}
	if (nrhs > DIAG_MODE) 
		if (!mxIsChar(prhs[DIAG_MODE])) {
			MyErrMsgTxt("%s: The <diag. mode> argument (no. %d) must be a "
			  "string.", funcName, DIAG_MODE);
	}
	if (nrhs > INPUT_SIGNAL) {
		if (!mxIsDouble(prhs[INPUT_SIGNAL]))
			MyErrMsgTxt("%s: The <signal> argument must be (no. %d) real "
			  "matrix type.",  funcName, INPUT_SIGNAL);
	}
	if (nrhs > INFO_STRUCT) {
		const mxArray *info = prhs[INFO_STRUCT];

		CheckDoubleField(info, "dt");
	}
	return FALSE;

}

/****************************** GetString *************************************/

/*
 * Returns a C string from a MX string.
 */

char *
GetString(const mxArray *str)
{
	char	*s;
	int		bufferLength;

	bufferLength = STR_BUFFER_LEN(str);
	s = mxCalloc(bufferLength, sizeof(mxChar));
	if (mxGetString(str, s, bufferLength) != 0)
		mexWarnMsgTxt("GetString: Not enough space for string. String is "
		  "truncated.");
	return(s);

}

/****************************** GetOutputInfoStruct ***************************/

/*
 * This routine returns the info structure using the data EarObjects output
 * signal.
 */

mxArray *
GetOutputInfoStruct(SignalDataPtr signal)
{
	static const char *funcName = "GetOutputInfoStruct";
	const char	*fieldNames[] = {"dt", "staticTimeFlag", "length", "labels",
				  "numChannels", "numWindowFrames", "outputTimeOffset",
				  "interleaveLevel", "wordSize", ""};
	const char	**p;
	int		i, numFields = 0;
	double	*lPtr;
	mxArray	*info, *labels;

	for (p = fieldNames; *p != '\0'; p++)
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
	register ChanData	*inPtr, *outPtr;
	int		chan;
	ChanLen	i;
	double	*mPtr;

	mxArray	*m= mxCreateDoubleMatrix((int) signal->numChannels,
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
	char	*simFile, *diagMode, *parameterOptions;
	uShort	numChannels = 0;
	ChanLen	length = 0;
	double	*inputMatrixPtr = NULL, dt = 0.0, outputTimeOffset= 0.0;
	EarObjectPtr	audModel;

	SetErrorsFile_Common("screen", OVERWRITE);
	if (AnyBadArgument(nrhs, prhs))
		return;

	simFile = GetString(prhs[SIM_FILE]);
	diagMode = (nrhs > DIAG_MODE)? GetString(prhs[DIAG_MODE]): "";
	parameterOptions = (nrhs > PARAMETER_OPTIONS)? GetString(prhs[
	  PARAMETER_OPTIONS]): (char *) "";

	if (nrhs > INFO_STRUCT) {
		const mxArray *info = prhs[INFO_STRUCT];
		inputMatrixPtr = mxGetPr(prhs[INPUT_SIGNAL]);
 		numChannels = (uShort) mxGetM(prhs[INPUT_SIGNAL]);
		length = (ChanLen) mxGetN(prhs[INPUT_SIGNAL]);
		dt =  *mxGetPr(mxGetField(info, 0, "dt"));
		staticTimeFlag = (BOOLN) GET_INFO_PAR("staticTimeFlag", FALSE);
		outputTimeOffset = GET_INFO_PAR("outputTimeOffset", 0.0);
	}
	if ((audModel = RunSimulation_DSAMMatrix(PROGRAM_NAME, simFile, diagMode,
	  parameterOptions, inputMatrixPtr, numChannels, length, dt,
	  staticTimeFlag, outputTimeOffset)) == NULL) {
		Free_DSAMMatrix();
		return;
	}

	mxFree(simFile);
	if (nrhs > DIAG_MODE)
		mxFree(diagMode);
	if (nrhs > PARAMETER_OPTIONS)
		mxFree(parameterOptions);

	plhs[0] = GetOutputSignalMatrix(audModel->outSignal);
	plhs[1] = GetOutputInfoStruct(audModel->outSignal);
	Free_DSAMMatrix();

}
