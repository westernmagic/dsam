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

#include <iostream.h>
#include <octave/oct.h>

#include <octave/oct-map.h>

#include "GeCommon.h"
#include "GeSignalData.h"
#include "GeEarObject.h"
#include "DSAMMatrix.h"

/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

#define	PROGRAM_NAME				"RunDSAMSim"

/******************************************************************************/
/*************************** Global Variables *********************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Subroutines and functions ************************/
/******************************************************************************/

/**************************** AnyBadArgument **********************************/

static bool
AnyBadArgument(const octave_value_list& args)
{
	int		nArgIn = args.length();

	if (nArgIn < 1) {
		error("A simulation file name must be given as the first argument.");
		return true;
	}

	if (!args(SIM_FILE).is_string()) {
		error("File name (argument %d) must be a string.", SIM_FILE);
		return true;
	}
	if (nArgIn > PARAMETER_OPTIONS) {
		if (!args(PARAMETER_OPTIONS).is_string()) {
			error("Parameter options (argument %d) must be a string.",
			  PARAMETER_OPTIONS);
			return true;
		}
	}
	if (nArgIn > DIAG_MODE) {
		if (!args(DIAG_MODE).is_string()) {
			error("Diagnostic mode (argument %d) must be a string.",
			  DIAG_MODE);
			return true;
		}
	}
	if (nArgIn > INPUT_SIGNAL) {
		if (!args(INPUT_SIGNAL).is_matrix_type()) {
			error("Data signal (argument %d) must be matrix type.",
			  INPUT_SIGNAL);
			return true;
		}
	}
	if (nArgIn > INFO_STRUCT) {
		if (!args(INFO_STRUCT).is_map()) {
			error("Argument %d must be a structure (map) type.", INFO_STRUCT);
			return true;
		} else {
			Octave_map map = args(INFO_STRUCT).map_value();
			if (!map["dt"].is_real_scalar()) {
				error("Structure 'dt' element not present.");
				return true;
			}
		}
	}
	return false;

}

/****************************** GetOutputInfoStruct ***************************/

/*
 * This routine returns the info structure using the data EarObjects output
 * signal.
 */

static Octave_map
GetOutputInfoStruct(SignalDataPtr signal)
{
	int		i;
	Octave_map	info;

	info["dt"] = octave_value(signal->dt);
	info["staticTimeFlag"] = octave_value((bool) signal->staticTimeFlag);
	info["length"] = octave_value((double) signal->length);
	ColumnVector	labels(signal->numChannels);
	for (i = 0; i < signal->numChannels; i++)
		labels(i) = signal->info.chanLabel[i];
	info["labels"] = octave_value(labels);
	info["numChannels"] = octave_value((double) signal->numChannels);
	info["numWindowFrames"] = octave_value((double) signal->numWindowFrames);
	info["outputTimeOffset"] = octave_value(signal->outputTimeOffset);
	info["interleaveLevel"] = octave_value((double) signal->interleaveLevel);
	info["wordSize"] = octave_value((double) 2);
	return info;

}

/****************************** GetOutputSignalMatrix *************************/

/*
 * This routine returns the signal matrix using the data EarObjects output
 * signal.
 */

static Matrix
GetOutputSignalMatrix(SignalDataPtr signal)
{
	register ChanData	*inPtr;
	int		chan, i;
	Matrix	m((int) signal->numChannels, (int) signal->length);

	for (chan = 0; chan < signal->numChannels; chan++) {
		inPtr = signal->channel[chan];
		for (i = 0; i < signal->length; i++)
			m(chan, i) = *inPtr++;
	}
	return m;

}

/******************************************************************************/
/*************************** Main routine *************************************/
/******************************************************************************/

#define GET_INFO_PAR(NAME, DEF_VALUE) ((info[(NAME)].is_real_scalar())? \
		  info[(NAME)].double_value(): (DEF_VALUE))
// Note that the third parameter (nargout) is not used, so it is
// omitted from the list of arguments to DEFUN_DLD in order to avoid
// the warning from gcc about an unused function parameter. 

DEFUN_DLD (RunDSAMSim, args, ,
  "RunDSAMSim:\n"
  "\n"
  "[data info] = RunDSAMSim(<sim file>, [<parameter settings>, [<diag. mode>,"
  "...\n"
  "                         [<signal>, [<signal info.>]]]])\n"
  "\n"
  "<sim file>           Simulation file name (string).\n"
  "<parameter settings> Parameter '<name> <value> ...' pairs (string).\n"
  "<diag. mode>         Diagnostic mode 'screen', 'off' or '<file name>'.\n"
  "<signal>             Data signal ([chan, samples] real matrix).\n"
  "<signal info.>       Signal information (structure).\n"
  )
{
	BOOLN	staticTimeFlag = FALSE;
	uShort	numChannels = 0;
	ChanLen	length = 0;
	double	*inputMatrixPtr = NULL, dt = 0.0, outputTimeOffset= 0.0;
	std::string	diagMode, parameterOptions;
	EarObjectPtr	audModel;
	octave_value_list retVal;

	if (AnyBadArgument(args))
		return octave_value_list();

	int nArgsIn = args.length();
	diagMode = (nArgsIn > DIAG_MODE)? &args(DIAG_MODE).string_value()[0]: "";
	parameterOptions = (nArgsIn > PARAMETER_OPTIONS)? &args(PARAMETER_OPTIONS).
	  string_value()[0]: (char *) "";

	Matrix	inputMatrix;
	
	if (nArgsIn > INFO_STRUCT) {
		Octave_map info = args(INFO_STRUCT).map_value();
		inputMatrix = args(INPUT_SIGNAL).matrix_value();
		inputMatrixPtr = &inputMatrix(0, 0);
 		numChannels = (uShort) inputMatrix.rows();
		length = (ChanLen) inputMatrix.columns();
		dt = info["dt"].double_value();
		staticTimeFlag = (BOOLN) GET_INFO_PAR("staticTimeFlag", FALSE);
		outputTimeOffset = GET_INFO_PAR("outputTimeOffset", 0.0);
	}

	if ((audModel = RunSimulation_DSAMMatrix(PROGRAM_NAME, &args(SIM_FILE).
	  string_value()[0], &diagMode[0], &parameterOptions[0], inputMatrixPtr,
	  numChannels, length, dt, staticTimeFlag, outputTimeOffset)) == NULL) {
		Free_DSAMMatrix();
		return octave_value_list();
	}

	retVal(0) = GetOutputSignalMatrix(audModel->outSignal);
	retVal(1) = GetOutputInfoStruct(audModel->outSignal);

	Free_DSAMMatrix();
	return retVal;

}
