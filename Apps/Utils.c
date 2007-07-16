/**********************
 *
 * File:		Utils.c
 * Purpose:		This module runs a tuning curve test on a simulation.
 * Comments:	It was created to be used with the feval program.
 *				Routines were copied from the AutoTest.c program
 * Author:		L. P. O'Mard
 * Created:		28 Jun 2000
 * Updated:		
 * Copyright:	(c) 2000, University of Essex
 *
 *********************/

#include "DSAM.h"
#include "Utils.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Functions and subroutines *********************/
/******************************************************************************/

/****************************** RunSimulation *********************************/

/*
 * This routine runs the 'simProcess' simulation.
 * It returns NULL if it fails in any way, otherwise it returns a pointer
 * to the results EarObjectPtr.
 */

EarObjectPtr
RunSimulation_Utils(void)
{
	static const WChar * funcName = wxT("RunSimulation_Utils");

	if (!RunProcess_ModuleMgr(appInterfacePtr->audModel)) {
		NotifyError(wxT("%s: Failed to run simulation."), funcName);
		return(NULL);
	}
	return(appInterfacePtr->audModel);

}

/****************************** InitResultEarObject ***************************/

/*
 * This function returns a pointer to an Earobject for holding the results of
 * a series of runs.
 */

EarObjectPtr
InitResultEarObject_Utils(int numChannels, ChanLen length, double initialValue,
 double increment, const WChar *processName)
{
	static WChar	*funcName = wxT("InitResultEarObject");
	EarObjectPtr	data;

	if ((data = Init_EarObject(wxT("NULL"))) == NULL) {
		NotifyError(wxT("%s: Could not initialise data results earObject."),
		  funcName);
		return(NULL);
	}
	if (!InitOutSignal_EarObject(data, numChannels, length, increment)) {
		NotifyError(wxT("%s: Cannot initialise output channels."), funcName);
		return(FALSE);
	}
	SetOutputTimeOffset_SignalData(data->outSignal, initialValue);
	SetStaticTimeFlag_SignalData(data->outSignal, TRUE);
	SetLocalInfoFlag_SignalData(data->outSignal, TRUE);
	return(data);

}

/****************************** OutputResults *********************************/

/*
 * This routine outputs the data according to the diagnostic mode.
 */

BOOLN
OutputResults_Utils(EarObjectPtr data, NameSpecifier *outputMode,
  OutputStyleSpecifier style)
{
	static const WChar * funcName = wxT("OutputResults_Utils");
	WChar	parFilePath[MAX_FILE_PATH];
	int		chan;
	ChanLen	i;
	FILE	*fp;

	if (!data) {
		NotifyError(wxT("%s: Output process not set."), funcName);
		return(FALSE);
	}
	if (!CheckPars_SignalData(data->outSignal)) {
		NotifyError(wxT("%s: Output signal not initialised."), funcName);
		return(FALSE);
	}
	if (style == UTILS_NORMAL_SIGNAL) {
		switch (outputMode->specifier) {
		case GENERAL_DIAGNOSTIC_SCREEN_MODE:
			WriteOutSignal_DataFile(wxT("-.dat"), data);
			break;
		case GENERAL_DIAGNOSTIC_FILE_MODE:
			WriteOutSignal_DataFile(outputMode->name, data);
			break;
		default:
			;
		}
		return(TRUE);
	}
	switch (outputMode->specifier) {
	case GENERAL_DIAGNOSTIC_SCREEN_MODE:
		fp = stdout;
		break;
	case GENERAL_DIAGNOSTIC_FILE_MODE:
		DSAM_strcpy(parFilePath, GetParsFileFPath_Common(outputMode->name));
		if ((fp = fopen(ConvUTF8_Utility_String(parFilePath), "w")) == NULL) {
			NotifyError(wxT("%s: Could not open file '%s'"), funcName,
			  parFilePath);
			return(FALSE);
		}
		break;
	default:
		return(TRUE);
	}
	fprintf_Utility_String(fp, wxT("%s"), data->outSignal->info.sampleTitle);
	for (i = 1; i < data->outSignal->numChannels; i++) {
		fprintf_Utility_String(fp, wxT("\t%g"), data->outSignal->info.chanLabel[
		  i]);
	}
	fprintf_Utility_String(fp, wxT("\n"));
		
	for (i = 0; i < data->outSignal->length; i++) {
		for (chan = 0; chan < data->outSignal->numChannels; chan++) {
			if (chan != 0)
				fprintf_Utility_String(fp, wxT("\t"));
			fprintf_Utility_String(fp, wxT("%g"), data->outSignal->channel[
			  chan][i]);
		}
		fprintf_Utility_String(fp, wxT("\n"));
	}
	if (outputMode->specifier == GENERAL_DIAGNOSTIC_FILE_MODE) {
		fclose(fp);
		DPrint(wxT("Output sent to file '%s'.\n"), parFilePath);
	}
	return(TRUE);

}

/****************************** GetFreqIndexSample ****************************/

/*
 * This function calculates the indexed result from the Ana_FourierT analysis
 * process.
 */

double
GetFreqIndexSample_Utils(EarObjectPtr data, double frequency)
{
	static const WChar	*funcName = wxT("GetFreqIndexSample_Utils");
	ChanLen	index;

	if (!data) {
		NotifyError(wxT("%s: EarObject not set."), funcName);
		return(0.0);
	}
	if ((index = (ChanLen) (frequency / data->outSignal->dt + 0.5)) >=
	  data->outSignal->length) {
		NotifyError(wxT("%s: Frequency (%g Hz) is outside data bounds."), funcName,
		  frequency);
		return(0.0);
	}
	return(GetSample_EarObject(data, 0, index));

}

/****************************** InitAnalysisOutputModeList ********************/

/*
 * This function initialises the 'analysisOutputMode' list array
 */

NameSpecifierPtr
InitPhaseOutputModeList_Utils(void)
{
	static NameSpecifier	modeList[] = {

			{ wxT("DEGREES"),	UTILS_PHASE_ANALYSISOUTPUT_DEGREES_MODE },
			{ wxT("RADIANS"),	UTILS_PHASE_ANALYSISOUTPUT_RADIANS_MODE },
			{ wxT("CYCLES"),	UTILS_PHASE_ANALYSISOUTPUT_CYCLES_MODE },
			{ NULL,				UTILS_PHASE_ANALYSISOUTPUTMODE_NULL },
		};
	return(modeList);

}

/****************************** GetPhase **************************************/

/*
 * This function returns the output phase, according to the specified output
 * mode.
 */

double
GetPhase_Utils(double phase, PhaseOutputModeSpecifier outputMode)
{
	static const WChar	*funcName = wxT("GetPhase_Utils");
	switch (outputMode) {
	case UTILS_PHASE_ANALYSISOUTPUT_DEGREES_MODE:
		return(RADS_TO_DEGREES(phase));
	case UTILS_PHASE_ANALYSISOUTPUT_CYCLES_MODE:
		return(phase / PIx2);
	case UTILS_PHASE_ANALYSISOUTPUT_RADIANS_MODE:
		return(phase);
	default:
		NotifyError(wxT("%s: Unknown phase output mode, retuning zero."), funcName);
		return(0.0);
	}

}

/****************************** CalcVectorAngle *******************************/

/*
 * This function calculates the angle between two two-dimensional vectors,
 * using the dot or vector product relation.
 */

double
CalcVectorAngle_Utils(double a1, double a2, double b1, double b2)
{
	double	cosTheta, theta;

	cosTheta = (a1 * b1 + a2 * b2) / sqrt((SQR(a1) + SQR(a2)) * (SQR(b1) + SQR(
	  b2)));
	theta = acos(cosTheta);
	/*printf(wxT("a(%g, %g), b(%g, %g), theta = %g\n"), a1, a2, b1, b2,
	  RADS_TO_DEGREES((cosTheta < 0.0)? -theta: theta)); */
	return ((cosTheta < 0.0)? -theta: theta);

}

/****************************** CalcPhase *************************************/

/*
 * This function calculates the phase from the Ana_FourierT analysis process.
 * dF is the normalised frequency, i.e. 1 / (No. of Frequencies).
 */

#define	MODULUS(A, B)	(sqrt(SQR(A) + SQR(B)))

double
CalcPhase_Utils(double phase, double dF, PhaseOutputModeSpecifier outputMode,
  BOOLN reset)
{
	/* static const WChar	*funcName = wxT("CalcPhase_Utils"); */
	static int		callCount;
	static double	lastPhaseChange, lastPhase;
	double	deltaPhase, angle, newAngle, phaseChange;

	if (reset) {
		callCount = 1;
		lastPhaseChange = phase;
		lastPhase = phase;
		/* printf(wxT("[%2d] Phase = %8.5g.\n"), callCount - 1, phase); */
	} else {
		callCount++;
		phaseChange = phase - lastPhase;
		/* printf(wxT("[%2d] Phase = %8.5g, dF = %8.5g: "), callCount - 1, phase,
		  dF); */
		angle = fabs(CalcVectorAngle_Utils(dF, lastPhaseChange, dF,
		  phaseChange));
		/* printf(wxT("angle = %6.3g\n"), RADS_TO_DEGREES(angle)); */
		deltaPhase = (phase > lastPhase)? -PIx2: PIx2;
		while ((newAngle = fabs(CalcVectorAngle_Utils(dF, lastPhaseChange, dF,
		  phaseChange + deltaPhase))) < angle) {
			angle = newAngle;
			phase += deltaPhase;
			phaseChange = phase - lastPhase;
			deltaPhase = (phase > lastPhase)? -PIx2: PIx2;
			/* printf(wxT("changed to %6.3g (%6.3g), angle = %6.3f (next = %6.3f / ")
			  wxT("%6.3f)\n"), phase, deltaPhase, RADS_TO_DEGREES(angle),
			  RADS_TO_DEGREES(fabs(CalcVectorAngle_Utils(dF,
			  lastPhaseChange, dF, phaseChange + deltaPhase))), RADS_TO_DEGREES(
			  CalcVectorAngle_Utils(dF, lastPhaseChange, dF, phaseChange +
			  deltaPhase))); */
		}
	}
	lastPhaseChange = phase - lastPhase;
	lastPhase = phase;
	return(GetPhase_Utils(phase, outputMode));

}

#undef	MODULUS
