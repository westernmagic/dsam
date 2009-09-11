/*******
 *
 * UtDebug.c
 *
 * This module contains the debug routines used in testing.
 *
 * by Lowel O'Mard 26-10-93.
 *
 ********/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG_H */

#include "GeCommon.h"
#include "GeSignalData.h"
#include "GeEarObject.h"
#include "UtString.h"
#include "UtDebug.h"

/******************************************************************************/
/*************************** Global Variables *********************************/
/******************************************************************************/

EarObjectPtr	debug = NULL;	/* This is used for debugging modules. */

/******************************************************************************/
/************************ Subroutines and functions ***************************/
/******************************************************************************/

/********************************* CheckInit **********************************/

/*
 * This routine checks that the gobal variable debug (hence the module) has
 * been initialised.  It prevents attempts to use uninitailised pointers which
 * will cause the module to crash the system.
 * It returns FALSE if the gobal variable has not been initialised.
 */

BOOLN
CheckInit_Debug(void)
{
	static const WChar *funcName = wxT("CheckInit_Debug");

	if (debug == NULL) {
		NotifyError(wxT("%s: Debug module not initialised."), funcName);
		return(FALSE);
	}
	return(TRUE);

}

/*************************** Init *********************************************/

/*
 * This routine initialises the debug EarObject.
 */

void
Init_Debug(void)
{
	if (debug != NULL)
		Free_EarObject(&debug);
	debug = Init_EarObject(wxT("null"));

}

/*************************** Init *********************************************/

/*
 * This routine frees the memory used by the debug EarObject.
 */

void
Free_Debug(void)
{
	if (debug != NULL)
		Free_EarObject(&debug);

}

/*************************** SetPars ******************************************/

/*
 * This function sets the debug EarObjects parameters.
 */

BOOLN
SetPars_Debug(uShort numChannels, ChanLen length, Float samplingInterval)
{
	static const WChar *funcName = wxT("SetPars_Debug");

	if (!CheckInit_Debug())
		return(FALSE);
	if (!InitOutSignal_EarObject(debug, numChannels, length,
	  samplingInterval)) {
		NotifyError(wxT("%s: Could not initialise output signal."), funcName);
		return(FALSE);
	}
	SetProcessName_EarObject(debug, wxT("Debug module"));
	return(TRUE);

}

/*************************** SetChannelValue **********************************/

/*
 * This function sets the specified value of a channel of the output signal.
 * It checks that the channel and sample numbers are correct.
 * It returns FALSE if it fails in any way.
 */

BOOLN
SetChannelValue_Debug(int channel, ChanLen sample, Float value)
{
	static const WChar *funcName = wxT("SetChannelValue_Debug");

	if (!CheckInit_Debug()) {
		NotifyError(wxT("%s: Debug module not initialised."), funcName);
		return(FALSE);
	}
	if ((channel < 0) || (channel >= debug->outSignal->numChannels)) {
		NotifyError(wxT("%s: Channel must be in range %d - %d."), funcName,
		  0, debug->outSignal->numChannels);
		return(FALSE);
	}
	if (sample >= debug->outSignal->length) {
		NotifyError(wxT("%s: Sample must be in range %u - %u."), funcName,
		  0, debug->outSignal->length);
		return(FALSE);
	}
	_OutSig_EarObject(debug)->channel[channel][sample] = value;
	return(TRUE);

}

/*************************** WriteOutSignal_Debug *****************************/

/*
 * This routine writes out the debug signal to file, using the
 * OutputToFile_SignalData routine.
 */

BOOLN
WriteOutSignal_Debug(WChar *fileName)
{

	return(OutputToFile_SignalData(fileName, debug->outSignal));

}

/*************************** WriteOutSignal_Debug *****************************/

/*
 * This routine writes out the contents of an array for debug purposes.
 * The full array length should be given as this is used in conjunction with the
 * increment value to set the array dimensions.
 */

void
WriteArray_Debug(WChar *fileName, Float *p, ChanLen length, int increment)
{
	static const char *funcName = "WriteArray_Debug";
	ChanLen	i;
	FILE	*fp;

	if ((fp = DSAM_fopen(fileName, "w")) == NULL) {
		NotifyError(wxT("%s: Could not open file '%s' for writing."), funcName,
		  fileName);
		return;
	}
	fprintf(fp, "X\tY\n");
	for (i = 0; i < length / increment; i++, p += increment)
		fprintf(fp, "%lu\t%g\n", i, *p);
	fprintf(fp, "\n");
	fclose(fp);

}

