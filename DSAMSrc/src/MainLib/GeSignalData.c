/******************
 *
 * File:		SignalData.c
 * Purpose:		This module contains the methods for the SignalData class.
 * Comments:	02-04-95 LPO: changed the data structure so that channels are
 *				allocated dynamically - without the MAX_CHANNELS restriction -
 *				as suggested by Trevor Shackleton.
 *				10-04-97 LPO: Introduced info.cFArray - to only hold CF for
 *				modules which need this information, e.g Carney.
 *				10-11-97 LPO: Introduced the staticTimeFlag parameter for the
 *				SignalDaata structure. When set the _WorldTime_EarObject macro
 *				always sets the time to PROCESS_START_TIME.
 *				30-06-98 LPO: Introduced the numWindowFrames parameter.
 *				This will define the number of windows in the signal.
 *				23-09-98 LPO: Removed the InitCopy_SignalData routine as it
 *				was not used and contained a serious bug.
 * Authors:		L. P. O'Mard
 * Created:		17 Feb 1993
 * Updated:		23 Sep 1998
 * Copyright:	(c) 1998, University of Essex
 * 
 ******************/

#include <stdio.h> 
#include <stdlib.h>
#include <math.h>

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG_H */

#include "GeCommon.h"
#include "UtString.h"
#include "GeSignalData.h"

/******************************************************************************/
/*************************** Subroutines and functions ************************/
/******************************************************************************/

/**************************** InitInfo ****************************************/

/*
 * This routine initialises the signal info structure to the default values,
 * setting the channel label pointer to null (numChannels is not set yet).
 * Space is allocated for the channel labels when the channels are initialised.
 */

BOOLN
InitInfo_SignalData(SignalInfoPtr info)
{
	info->chanLabel = NULL;
	info->cFArray = NULL;
	snprintf(info->channelTitle, SIGNALDATA_MAX_TITLE, "Channel Data");
	snprintf(info->chanDataTitle, SIGNALDATA_MAX_TITLE, "Amplitude (uPa)");
	snprintf(info->sampleTitle, SIGNALDATA_MAX_TITLE, "Time (s)");
	return(TRUE);

}

/**************************** Init ********************************************/

/*
 * This function allocates memory for the signal data-set and sets the
 * default values.  The numChannels field should never be changed by the user.
 * It is only changed when the channels are initialised.
 * The function returns a pointer to the prepared structure.
 * The "info" pointer is a handle to information about the signals information.
 */

SignalDataPtr
Init_SignalData(const char *callingFunctionName)
{
	static const char *funcName = "Init_SignalData";
	SignalData	*theData;
	
	if ((theData = (SignalData *) malloc(sizeof(SignalData))) == NULL ) {
		NotifyError("%s: Out of Memory (called by %s).", funcName,
		  callingFunctionName);
		exit(1);
	}
	theData->numChannels = 0;
	theData->interleaveLevel = SIGNALDATA_DEFAULT_INTERLEAVE_LEVEL;
	theData->numWindowFrames = SIGNALDATA_DEFAULT_NUM_WINDOW_FRAMES;
	theData->length = 0;
	theData->timeIndex = PROCESS_START_TIME;
	theData->dt = 0.0;
	theData->outputTimeOffset = 0.0;
	theData->dtFlag = FALSE;
	theData->lengthFlag = FALSE;
	theData->rampFlag = FALSE;
	theData->localInfoFlag = FALSE;
	theData->staticTimeFlag = FALSE;
	InitInfo_SignalData(&theData->info);
	theData->channel = NULL;
	return(theData);
	
} /* Init_SignalData */

/**************************** FreeChannels ************************************/

/*
 * This routine frees the memory allocated to a SignalData's channels.
 * it also free's the memory for the channel labels info if set.
 * It also sets the respective pointers to NULL.
 */
 
void
FreeChannels_SignalData(SignalDataPtr theData)
{
	int		i;

	if (theData != NULL) {
		if (theData->channel != NULL) {
			if (!theData->externalDataFlag)
				for (i = 0; i < theData->numChannels; i++)
					free(theData->channel[i]);
			free(theData->channel);
			theData->channel = NULL;
		}
		if (theData->info.chanLabel != NULL) {
			free(theData->info.chanLabel);
			theData->info.chanLabel = NULL;
		}
		if (theData->info.cFArray != NULL) {
			free(theData->info.cFArray);
			theData->info.cFArray = NULL;
		}
	}
	
}

/**************************** Free ********************************************/

/*
 * This routine frees the memory allocated to a SignalData pointer.
 * It also sets the pointer to NULL, hence the address of the pointer is passed
 * to this function.
 */
 
void
Free_SignalData(SignalDataPtr *theData)
{
	if (*theData != NULL) {
		FreeChannels_SignalData(*theData);
		free(*theData);
		*theData = NULL;
	}
	
} /* Free_SignalData */

/**************************** ResetInfo ***************************************/

/*
 * This routine resets the signal info structure to the default values.
 */

BOOLN
ResetInfo_SignalData(SignalDataPtr signal)
{
	if (!CheckInit_SignalData(signal, "ResetInfo_SignalData"))
		return(FALSE);
	SetInfoChannelLabels_SignalData(signal, NULL);
	SetInfoCFArray_SignalData(signal, NULL);
	snprintf(signal->info.channelTitle, SIGNALDATA_MAX_TITLE, "Channel Data");
	snprintf(signal->info.chanDataTitle, SIGNALDATA_MAX_TITLE, "Amplitude "
	  "(uPa)");
	snprintf(signal->info.sampleTitle, SIGNALDATA_MAX_TITLE, "Time (s)");
	return(TRUE);

}

/**************************** CheckInit ***************************************/

/*
 * This routine checks whether or not a signal has been initialised.
 *
 */
 
BOOLN
CheckInit_SignalData(SignalDataPtr theSignal, const char *callingFunction)
{
	static const char *funcName = "CheckInit_SignalData";
	if (theSignal != NULL)
		return(TRUE);
	else {
		NotifyError("%s: Signal not set in %s.", funcName, callingFunction);
		return(FALSE);
	}
	
}

/**************************** InitChannels ************************************/

/*
 * This method allocates memory space for all of a signal's channels.
 * It updates the numChannel counter, and returns TRUE if
 * successful.
 * If the 'externalDataFlag' is set, then the pointers for the channels will be
 * set, but the memory for each channel will be assumed to have come from
 * elsewhere.
 * It also allocates space for the info channel labels.
 *
 */

BOOLN
InitChannels_SignalData(SignalDataPtr theData, uShort numChannels,
  BOOLN externalDataFlag)
{
	static const char *funcName = "InitChannels_SignalData";
	int			i, j;
	ChanData	**p;
	
	if (!CheckInit_SignalData(theData, "InitChannels_SignalData"))
		return(FALSE);
	if (theData->length < 1) {
		NotifyError("%s: Invalid signal length = %u.", funcName,
		  theData->length);
		return(FALSE);
	}
	if ((p = (ChanData **) calloc(numChannels, sizeof(ChanData *))) == NULL) {
		NotifyError("%s: Out of memory for channel pointers.", funcName);
		return(FALSE);
	}
	if (!externalDataFlag) {
		for (i = 0; i < numChannels; i++)
			if ((p[i] = (ChanData *) calloc(theData->length, sizeof(
			  ChanData))) == NULL) {
				NotifyError("%s: Out of memory for channel[%d] data (length = "
				  "%u).", funcName, i, theData->length);
				for (j = 0; j < i - 1; j++)
					free(p[j]);
				free(p);
				return(FALSE);
			}
	}
	theData->channel = p;
	theData->numChannels = numChannels;
	theData->externalDataFlag = externalDataFlag;
	if ((theData->info.chanLabel = (double *) calloc(numChannels,
	  sizeof(double))) == NULL) {
		NotifyError("%s: Out of memory for channel info labels.", funcName);
		FreeChannels_SignalData(theData);
		return(FALSE);
	}
	if ((theData->info.cFArray = (double *) calloc(numChannels,
	  sizeof(double))) == NULL) {
		NotifyError("%s: Out of memory for CF Array.", funcName);
		FreeChannels_SignalData(theData);
		return(FALSE);
	}
	for (i = 0; i < numChannels; i++)
		theData->info.chanLabel[i] = theData->info.cFArray[i] = (double) i /
		  theData->interleaveLevel;
	return(TRUE);

} /* InitChannels_SignalData */

/**************************** CheckPars ***************************************/

/*
 * This routine checks that the necessary parameters for a signal have been
 * correctly initialised.
 * It returns TRUE if there are no problems.
 */

BOOLN
CheckPars_SignalData(SignalDataPtr theSignal)
{
	static const char *funcName = "CheckPars_SignalData";

	if (!CheckInit_SignalData(theSignal, funcName))
		return(FALSE);
	if (theSignal->numChannels == 0) {
		NotifyError("%s: No Channels have been set.", funcName);
		return(FALSE);
	}
	if (theSignal->length == 0) {
		NotifyError("%s: Invalid zero signal length.", funcName);
		return(FALSE);
	}
	if (theSignal->dt <= 0.0) {
		NotifyError("%s: Invalid signal sampling interval = %lg.", funcName,
		  theSignal->dt);
		return(FALSE);
	}
	return(TRUE);
	
}

/**************************** CheckRamp ***************************************/

/*
 * This function tests if the ramp flag of a signal has been set.  Some
 * processes require that signals should be ramped, so this facility allows a
 * warning to be given if a signal has not been ramped.  The flag is set in
 * the UtRamp module.
 * It assumes that the signal has been correctly initialised.
 * It returns TRUE if the signal has been ramped.
 */

BOOLN
CheckRamp_SignalData(SignalDataPtr theSignal)
{
	static const char *funcName = "CheckRamp_SignalData";

	if (!theSignal->rampFlag)
		NotifyWarning("%s: Signal is not ramped (see Trans_Gate).", funcName);
	return(TRUE);
	
}

/**************************** SetChannelsFromSignal ***************************/

/*
 * This routine copies the data from the orignal channel into all of the
 * channels of the supplied signal.  If the supplier signal is
 * multi-channelled, then the output signal will be an interleave of the
 * supplier channels.
 * It sets the interleaveLevel field for 'theSignal'.
 * It returns true if the operation was successful.
 */

BOOLN
SetChannelsFromSignal_SignalData(SignalDataPtr theSignal,
  SignalDataPtr supplier)
{
	int		i;
	ChanLen	j;
	register	ChanData	*copyChannel, *supplierChannel;
	
	if (!CheckInit_SignalData(theSignal, "SetChannelsFromSignal_SignalData"))
		return(FALSE);
	if (!CheckInit_SignalData(supplier, "SetChannelsFromSignal_SignalData "\
	  "(supplier)"))
		return(FALSE);
	if (theSignal->numChannels != supplier->numChannels)
		SetInterleaveLevel_SignalData(theSignal, supplier->numChannels);
	else
		SetInterleaveLevel_SignalData(theSignal, supplier->interleaveLevel);
	for (i = 0; i < theSignal->numChannels; i++) {
		copyChannel = theSignal->channel[i];
		supplierChannel = supplier->channel[i % supplier->numChannels];
		for (j = 0; j < theSignal->length; j++)
			*(copyChannel++) = *(supplierChannel++);
	}
	return(TRUE);
	
}

/**************************** SameType ****************************************/

/*
 * Checks that two signal data sets have the same dimensions, i.e. length,
 * sampling interval etc.
 * It returns TRUE if they have the same dimensions.
 */

BOOLN
SameType_SignalData(SignalDataPtr a, SignalDataPtr b)
{
	static const char *funcName = "SameType_SignalData";

	if (!a->lengthFlag || !b->lengthFlag) {
		NotifyError("%s: Both data set length values must be set! (lengths:- "\
		  "a = %u, b = %u).", funcName, a->length, b->length);
		return(FALSE);
	}
	if  ((a->length != b->length) || (a->numChannels != b->numChannels)) {
		NotifyError("%s: Both data sets must have the same length and number "\
		  "of channels! (lengths:- a = %lu, b = %lu; channels:-  a = %d, b = "\
		  "%d.)", funcName, a->length, b->length, a->numChannels,
		  b->numChannels);
		return(FALSE);
	}
    return(TRUE);

}

/**************************** SameType...NoDiagnostics ************************/

/*
 * Checks that two signal data sets have the same dimensions, i.e. length,
 * sampling interval etc.
 * It returns TRUE if they have the same dimensions.
 */

BOOLN
SameType_SignalData_NoDiagnostics(SignalDataPtr a, SignalDataPtr b)
{
	if (!a->lengthFlag || !b->lengthFlag)
		return(FALSE);
	if  ((a->length != b->length) || (a->numChannels != b->numChannels) ||
	  (a->dt != b->dt))
		return(FALSE);
    return(TRUE);

}

/**************************** Scale *******************************************/

/*
 * This routine scales data values by a specified multiplier.
 *
 */

BOOLN
Scale_SignalData(SignalDataPtr d, double multiplier)
{
	int		i;
	ChanLen	j;
	ChanData	*dataPtr;
	
	if (!CheckInit_SignalData(d, "Scale_SignalData (theSignal)"))
		return(FALSE);
	for (i = 0; i < d->numChannels; i++)
		for (j = 0, dataPtr = d->channel[i]; j < d->length; j++)
			*(dataPtr++) *= multiplier;
	return(TRUE);
	
}

/**************************** Add *********************************************/

/*
 * Adds two data structures together.
 * The result is put into the first of the data sets: a = a + b;
 */

BOOLN
Add_SignalData(SignalDataPtr a, SignalDataPtr b)
{
	int		i;
	ChanLen	j;
	ChanData	*aPtr, *bPtr;
	
	if (!CheckInit_SignalData(a, "AddScaledData_SignalData (a)") ||
	  !CheckInit_SignalData(b, "AddScaledData_SignalData (b)"))
		return(FALSE);
	if (!SameType_SignalData(a, b))
		return(FALSE);
	for (i = 0; i < a->numChannels; i++) {
		aPtr = a->channel[i];
		bPtr = b->channel[i];
		for (j = 0; j < a->length; j++)
			*(aPtr++) += *(bPtr++);
	}
	return(TRUE);
	
}

/**************************** SetSamplingInterval *****************************/

/*
 * The sampling interval can be set any number of times.
 */
 
void
SetSamplingInterval_SignalData(SignalDataPtr theData,
  double theSamplingInterval)
{
	if (!CheckInit_SignalData(theData, "SetSamplingInterval_SignalData"))
		exit(1);
	theData->dt = theSamplingInterval;
	theData->dtFlag = TRUE;

}

/**************************** SetOutputTimeOffset *****************************/

/*
 * This routine sets the time offset field for the SignalData structure.
 * By default it is set to zero, but it can be set to anything.
 */
 
void
SetOutputTimeOffset_SignalData(SignalDataPtr theData,
  double theOutputTimeOffset)
{
	if (!CheckInit_SignalData(theData, "SetOutputTimeOffset_SignalData"))
		exit(1);
	theData->outputTimeOffset = theOutputTimeOffset;

}

/**************************** SetTimeIndex ************************************/

/*
 * This routine sets the time offset field for the SignalData structure.
 * It can only be set in segmented mode at present, because the default
 * value of PROCESS_START_TIME must be maintained otherwise.
 */
 
void
SetTimeIndex_SignalData(SignalDataPtr theData, ChanLen theTimeIndex)
{
	if (!GetDSAMPtr_Common()->segmentedMode)
		return;
	if (!CheckInit_SignalData(theData,"SetTimeIndex_SignalData" ))
		exit(1);
	theData->timeIndex = theTimeIndex;

}

/**************************** SetLength ***************************************/

/*
 * The length of a data set can only be set for a particular data set once.
 */

void
SetLength_SignalData(SignalDataPtr theData, ChanLen theLength)
{
	static const char *funcName = "SetLength_SignalData";

	if (!CheckInit_SignalData(theData, funcName))
		exit(1);
	if (!theData->lengthFlag) {
		theData->length = theLength;
		theData->lengthFlag = TRUE;
	} else
		NotifyError("%s: Data set length cannot be changed.", funcName);
		
}

/**************************** SetInterleaveLevel ******************************/

/*
 * This routine sets the interleave level field for the SignalData structure.
 * A monaural signal has an interleave level of 1 (the default), and
 * a binaural signal has an interleave level of 2.
 */
 
void
SetInterleaveLevel_SignalData(SignalDataPtr theData, uShort theInterleaveLevel)
{
	static const char *funcName = "SetInterleaveLevel_SignalData";

	if (!CheckInit_SignalData(theData, funcName))
		exit(1);
	if (theInterleaveLevel < SIGNALDATA_DEFAULT_INTERLEAVE_LEVEL) {
		NotifyError("%s: Illegal interleave level (%d).", funcName,
		  theInterleaveLevel);
		exit(1);
	}
	theData->interleaveLevel = theInterleaveLevel;

}

/**************************** SetNumWindowFrames ******************************/

/*
 * This routine sets the number of window frames field for the SignalData
 * structure.
 */
 
void
SetNumWindowFrames_SignalData(SignalDataPtr theData, uShort theNumWindowFrames)
{
	static const char *funcName = "SetNumWindowFrames_SignalData";

	if (!CheckInit_SignalData(theData, funcName))
		exit(1);
	theData->numWindowFrames = theNumWindowFrames;

}

/**************************** SetLocalInfoFlag ********************************/

/*
 * This routine sets the local info flag for the signal's info structure.
 */

void
SetLocalInfoFlag_SignalData(SignalDataPtr theData, BOOLN flag)
{
	if (!CheckInit_SignalData(theData, "SetLocalInfoFlag_SignalData"))
		exit(1);
	theData->localInfoFlag = flag;

}

/**************************** SetInfoChannelLabels ****************************/

/*
 * This routine sets the channel labels for the signal's info structure,
 * but only if the information is local, and not just a copy.
 * It does not verify that the array of value's provided is the correct length.
 * If the "labels" array is null, then the values will be set to the channel
 * index divided by the interleave level.
 */

void
SetInfoChannelLabels_SignalData(SignalDataPtr signal, double *labels)
{
	int		i;

	if (!CheckInit_SignalData(signal, "SetInfoChannelLabels_SignalData"))
		exit(1);
	if (!signal->localInfoFlag)
		return;
	if (labels)
		for (i = 0; i < signal->numChannels; i++)
			signal->info.chanLabel[i] = labels[i / signal->interleaveLevel];
	else
		for (i = 0; i < signal->numChannels; i++)
			signal->info.chanLabel[i] = (double) (i / signal->interleaveLevel);

}

/**************************** SetInfoChannelLabel *****************************/

/*
 * This routine sets a single channel label in the signal's info structure,
 * but only if the information is local, and not just a copy.
 * It returns an error if an attempt is made to use an illegal index.
 */

BOOLN
SetInfoChannelLabel_SignalData(SignalDataPtr theData, int index, double label)
{
	static char *funcName = "SetInfoChannelLabel_SignalData";

	if (!CheckInit_SignalData(theData, funcName))
		return(FALSE);
	if (!theData->localInfoFlag)
		return(TRUE);
	if ((index < 0) || (index >= theData->numChannels)) {
		NotifyError("%s: Illegal label index (%d), must be in range 0 - %d.",
		  funcName, index, theData->numChannels - 1);
		return(FALSE);
	}
	theData->info.chanLabel[index] = label;
	return(TRUE);

}

/**************************** SetInfoCFArray **********************************/

/*
 * This routine sets the CF array for the signal's info structure,
 * but only if the information is local, and not just a copy.
 * It does not verify that the array of value's provided is the correct length.
 * If the cFs array is null, then the values will be set to the channel index.
 */

void
SetInfoCFArray_SignalData(SignalDataPtr theData, double *cFs)
{
	int		i;

	if (!CheckInit_SignalData(theData, "SetInfoCFArray_SignalData"))
		exit(1);
	if (!theData->localInfoFlag)
		return;
	if (cFs)
		for (i = 0; i < theData->numChannels; i++)
			theData->info.cFArray[i] = cFs[i / theData->interleaveLevel];
	else
		for (i = 0; i < theData->numChannels / theData->interleaveLevel; i++)
			theData->info.cFArray[i] = (double) (i / theData->interleaveLevel);

}

/**************************** SetInfoCF ***************************************/

/*
 * This routine sets a single channel cf in the signal's info structure,
 * but only if the information is local, and not just a copy.
 * It returns an error if an attempt is made to use an illegal index.
 */

BOOLN
SetInfoCF_SignalData(SignalDataPtr theData, int index, double cF)
{
	static char *funcName = "SetInfoCF_SignalData";

	if (!CheckInit_SignalData(theData, funcName))
		return(FALSE);
	if (!theData->localInfoFlag)
		return(TRUE);
	if ((index < 0) || (index >= theData->numChannels)) {
		NotifyError("%s: Illegal CF array index (%d), must be in range 0 - %d.",
		  funcName, index, theData->numChannels - 1);
		return(FALSE);
	}
	theData->info.cFArray[index] = cF;
	return(TRUE);

}

/**************************** SetInfoChannelTitle *****************************/

/*
 * This routine sets the channel title for the signal's info structure,
 * but only if the information is local, and not just a copy.
 */

void
SetInfoChannelTitle_SignalData(SignalDataPtr theData, char *title)
{
	if (!CheckInit_SignalData(theData, "SetInfoChannelTitle_SignalData"))
		exit(1);
	if (!theData->localInfoFlag)
		return;
	snprintf(theData->info.channelTitle, SIGNALDATA_MAX_TITLE, "%s", title);

}

/**************************** SetInfoChanDataTitle ****************************/

/*
 * This routine sets the channel data title for the signal's info structure,
 * but only if the information is local, and not just a copy.
 */

void
SetInfoChanDataTitle_SignalData(SignalDataPtr theData, char *title)
{
	if (!CheckInit_SignalData(theData, "SetInfoChanDataTitle_SignalData"))
		exit(1);
	if (!theData->localInfoFlag)
		return;
	snprintf(theData->info.chanDataTitle, SIGNALDATA_MAX_TITLE, "%s", title);

}

/**************************** SetInfoSampleTitle ******************************/

/*
 * This routine sets the sample or dt title for the signal's info structure,
 * but only if the information is local, and not just a copy.
 */

void
SetInfoSampleTitle_SignalData(SignalDataPtr theData, char *title)
{
	if (!CheckInit_SignalData(theData, "SetInfoSampleTitle_SignalData"))
		exit(1);
	if (!theData->localInfoFlag)
		return;
	snprintf(theData->info.sampleTitle, SIGNALDATA_MAX_TITLE, "%s", title);

}

/**************************** CopyInfo ****************************************/

/*
 * This routine copies the info values from one signal to another: a = b.
 * The channel labels must be copied implicitly as space for them is
 * dynamicallly allocated.
 * the structure-structure copy is used, but as 'chanLabel' and 'cFArray' can
 * pointers to memory space, then they must be preserved.
 * The labels will only be copied if there are an equal number of labels,
 * or if the respective number of channels corresponds to the interleavel level.
 * Otherwise, if the labels have been initialised, they will be reset.
 */

BOOLN
CopyInfo_SignalData(SignalDataPtr a, SignalDataPtr b)
{
	int			i, interleaveLevel;
	double		*tempChanLabel, *tempCFArray;

	if (!CheckInit_SignalData(a, "CopyInfo_SignalData: signal a"))
		return(FALSE);
	if (!CheckInit_SignalData(b, "CopyInfo_SignalData: signal b"))
		return(FALSE);
	tempChanLabel = a->info.chanLabel;
	tempCFArray = a->info.cFArray;
	a->info = b->info;
	a->info.chanLabel = tempChanLabel;
	a->info.cFArray = tempCFArray;
	if ((a->numChannels == b->numChannels) || (b->numChannels %
	  a->numChannels == 0)) {
		interleaveLevel = b->numChannels / a->numChannels;
		for (i = 0; i < a->numChannels; i++) {
			a->info.chanLabel[i] = b->info.chanLabel[i * interleaveLevel];
			a->info.cFArray[i] = b->info.cFArray[i * interleaveLevel];
		}
	} else
		ResetInfo_SignalData(a);
	return(TRUE);

}

/**************************** OutputToFile ************************************/

/*
 * This routine outputs a signal to a specied file, using the simple DSAM
 * format.
 */

BOOLN
OutputToFile_SignalData(char *fileName, SignalDataPtr theData)
{
	static const char *funcName = "OutputToFile_SignalData";
	char	*parFilePath;
	int		j, k;
    ChanLen	i, t;
	FILE	*fp;
	
	if (!CheckPars_SignalData(theData)) {
		NotifyError("%s: Parameters not set for data-set.", funcName);
		return(FALSE);
	}
	if(!GetDSAMPtr_Common()->segmentedMode)
		DPrint("Output sent to file '%s'.\n", fileName);
	if (*fileName == '-')
		fp = stdout;
	else {
		parFilePath = GetParsFileFPath_Common(fileName);
		fp = (GetDSAMPtr_Common()->segmentedMode && (theData->timeIndex !=
		  PROCESS_START_TIME))? fopen(parFilePath, "a"): fopen(parFilePath,
		  "w");
	}
	if (fp == NULL) {
		NotifyError("%s: Cannot open file '%s'", funcName, fileName);
		return(FALSE);
	}
	if (theData->timeIndex == PROCESS_START_TIME) {
		fprintf(fp,"%s", theData->info.sampleTitle);
		for (k = 0; k < theData->numChannels; k++)
			fprintf(fp, "\t%g", theData->info.chanLabel[k]);
		fprintf(fp, "\n");
	}
	for (i = 0, t = theData->timeIndex; i < theData->length; i++, t++) {
		fprintf(fp, "%8.5e", t * theData->dt + theData->outputTimeOffset);
		for (j = 0; j < theData->numChannels; j++)
			fprintf(fp, "\t %g", theData->channel[j][i]);
		fprintf(fp, "\n");
	}
	CloseFile(fp);
	return(TRUE);

}

/**************************** GetDuration *************************************/

/*
 * This function returns the duration of a signal.
 */

double
GetDuration_SignalData(SignalDataPtr theSignal)
{
	static const char *funcName = "GetDuration_SignalData";

	if (!CheckPars_SignalData(theSignal)) {
		NotifyError("%s: Parameters not set for data-set, returned zero.",
		  funcName);
		return(0.0);
	}
	return((theSignal->length) * theSignal->dt);
	
}

/**************************** GetOutputTime ***********************************/

/*
 * This function returns the output time for a sample of a signal.
 * It uses the 'outputTimeOffset' field.
 */

double
GetOutputTime_SignalData(SignalDataPtr theSignal, ChanLen sample)
{
	static const char *funcName = "GetDuration_SignalData";

	if (!CheckPars_SignalData(theSignal)) {
		NotifyError("%s: Parameters not set for data-set, returned zero.",
		  funcName);
		return(0.0);
	}
	return((sample + 1) * theSignal->dt + theSignal->outputTimeOffset);

}

/**************************** GaindBIndividual ********************************/

/*
 * This routine increases the signal intensity for each channel by a
 * specified number of dB's (using the dB 1 scale, so that 0 dB gives an
 * attenuation factor of 1.0).
 * No special checks are made upon the gaindB array.
 *
 */

BOOLN
GaindBIndividual_SignalData(SignalDataPtr d, double gaindB[])
{
	static const char *funcName = "GaindBIndividual_SignalData";
	int		i;
	double	*scale;
	ChanLen	j;
	ChanData	*dataPtr;
	
	if (!CheckInit_SignalData(d, funcName))
		return(FALSE);
	if ((scale = (double *) calloc(d->numChannels, sizeof(double))) == NULL) {
		NotifyError("%s: Out of memory.", funcName);
		return(FALSE);
	}
	for (i = 0; i < d->numChannels; i++)
		scale[i] = pow(10.0, gaindB[i] / 20.0);
	for (i = 0; i < d->numChannels; i++)
		for (j = 0, dataPtr = d->channel[i]; j < d->length; j++)
			*(dataPtr++) *= scale[i];
	free(scale);
	return(TRUE);
	
}

/**************************** GaindB ******************************************/

/*
 * This routine increases the signal intensity for each channel by a
 * specified number of dB's (using the dB 1 scale, so that 0 dB gives an
 * attenuation factor of 1.0).
 * A single gain is used for all channels.
 */

BOOLN
GaindB_SignalData(SignalDataPtr d, double gaindB)
{
	int		i;
	double	scale;
	ChanLen	j;
	ChanData	*dataPtr;
	
	if (!CheckInit_SignalData(d, "GaindBIndividual_SignalData (theSignal)"))
		return(FALSE);
	scale = pow(10.0, gaindB / 20.0);
	for (i = 0; i < d->numChannels; i++)
		for (j = 0, dataPtr = d->channel[i]; j < d->length; j++)
			*(dataPtr++) *= scale;
	return(TRUE);
	
}

/**************************** Divide ******************************************/

/*
 * This function divides two data structures.
 * The result is put into the first of the data sets: a = a / b;
 */

BOOLN
Divide_SignalData(SignalDataPtr a, SignalDataPtr b)
{
	int		i;
	ChanLen	j;
	ChanData	*aPtr, *bPtr;
	
	if (!CheckInit_SignalData(a, "Divide_SignalData (a)") ||
	  !CheckInit_SignalData(b, "Divide_SignalData (b)"))
		return(FALSE);
	if (!SameType_SignalData(a, b))
		return(FALSE);
	for (i = 0; i < a->numChannels; i++) {
		aPtr = a->channel[i];
		bPtr = b->channel[i];
		for (j = 0; j < a->length; j++)
			*(aPtr++) /= *(bPtr++);
	}
	return(TRUE);
	
}

/**************************** Delay *******************************************/

/*
 * This routine delays a signal by a specified delay.  It adds the appropriate
 * number of zeroes at the beginning of the signal, and loses data of the end
 * of the signal.
 * 'delay' indicates the seconds that the signal is going to be delayed.
 * It returns FALSE if it fails in any way.
 */

BOOLN
Delay_SignalData(SignalDataPtr signal, double delay)
{
	static char *funcName = "Delay_SignalData";
	int			chan;
	ChanLen		i, samplesDelay;
	ChanData	*dataPtr;

	samplesDelay = (ChanLen) ( fabs(delay) / signal->dt);
	if (samplesDelay > signal->length)	{
		NotifyError("%s: Delay (%g ms) is longer than the signal duration "
		  "(%g ms)", funcName, MSEC(delay), MSEC(_GetDuration_SignalData(
		  signal)));
		return(FALSE);
	}
	if (delay > 0.0) {
		for (chan = 0; chan < signal->numChannels; chan++) {
			dataPtr = signal->channel[chan] + signal->length - samplesDelay - 1;
			for (i = 0; i < signal->length - samplesDelay; i++, dataPtr--)
				*(dataPtr + samplesDelay) = *dataPtr;
			for (i = 0; i < samplesDelay; i++)
				*++dataPtr = 0.0;

		}
	} else {
		for (chan = 0; chan < signal->numChannels; chan++) {
			dataPtr = signal->channel[chan];
			for (i = 0; i < signal->length - samplesDelay; i++, dataPtr++)
				*dataPtr = *(dataPtr + samplesDelay);
			for (i = 0; i < samplesDelay; i++)
				*dataPtr++ = 0.0;

		}
	}
	return(TRUE);

}

/**************************** SetStaticTimeFlag *******************************/

/*
 * This routine sets the static time flag to tell functions, such as the
 * _WorldTime_EarObject macro, that the time should not advance in segmented
 * mode..
 */

void
SetStaticTimeFlag_SignalData(SignalDataPtr signal, BOOLN flag)
{
	signal->staticTimeFlag = flag;

}

/****************************** InitLimitModeList *****************************/

/*
 * This function initialises the 'limitMode' list array
 */

NameSpecifier *
LimitModeList_SignalData(int index)
{
	static NameSpecifier	modeList[] = {

			{ "OCTAVE",		SIGNALDATA_LIMIT_MODE_OCTAVE },
			{ "CHANNEL",	SIGNALDATA_LIMIT_MODE_CHANNEL },
			{ "",			SIGNALDATA_LIMIT_MODE_NULL },
		};
	return (&modeList[index]);

}

/****************************** GetChannelLimits ******************************/

/*
 * This routine calculates the signal channel limits, according to the
 * limitMode.
 * The minimum and maximum channels are returned using the respective pointer
 * arguments.
 */

BOOLN
GetChannelLimits_SignalData(SignalDataPtr signal, int *minChan, int *maxChan,
  double lowerLimit, double upperLimit, SignalDataLimitModeSpecifier mode)
{
	static const char	*funcName = "GetChannelLimits_SignalData";

	switch (mode) {
	case SIGNALDATA_LIMIT_MODE_OCTAVE:
		*minChan = 0;
		while (signal->info.cFArray[0] > OCTAVE(signal->info.cFArray[*minChan],
		  lowerLimit))
			(*minChan)++;
		*maxChan = signal->numChannels - 1;
		while (signal->info.cFArray[signal->numChannels - 1] < OCTAVE(signal->
		  info.cFArray[*maxChan], upperLimit))
			(*maxChan)--;
		break;
	case SIGNALDATA_LIMIT_MODE_CHANNEL:
		*minChan = (int) -lowerLimit;
		*maxChan = signal->numChannels - (int) upperLimit - 1;
		break;
	default:
		;
	}
	if (*maxChan < *minChan) {
		NotifyError("%s: Invalid channel limits calculated from averaging "
		  "limits (%d -> %d).", funcName, *minChan, *maxChan);
		return(FALSE);
	}
	return(TRUE);

}

/****************************** GetWindowLimits *******************************/

/*
 * This routine calculates the window limits around a specified frequency,
 * according to the limit mode.
 * The minimum and maximum channels are returned using the respective pointer
 * arguments.
 * It expects the signal channels limits to have been correctly determined.
 */

void
GetWindowLimits_SignalData(SignalDataPtr signal, int *minChan, int *maxChan,
  double frequency, double lowerLimit, double upperLimit,
  SignalDataLimitModeSpecifier mode)
{
	/* static const char	*funcName = "GetWindowLimits_SignalData"; */
	int		chan;
	double	requiredFreq;

	chan = FindCFIndex_SignalData(signal, 0, signal->numChannels - 1,
	  frequency);
	switch (mode) {
	case SIGNALDATA_LIMIT_MODE_OCTAVE:
		requiredFreq = OCTAVE(frequency, lowerLimit);
		*minChan = chan;
		while (signal->info.cFArray[*minChan] > requiredFreq)
			(*minChan)--;
		(*minChan)++;
		requiredFreq = OCTAVE(frequency, upperLimit);
		*maxChan = chan;
		while (signal->info.cFArray[*maxChan] < requiredFreq)
			(*maxChan)++;
		(*maxChan)--;
		break;
	case SIGNALDATA_LIMIT_MODE_CHANNEL:
		*minChan = chan + (int) lowerLimit;
		*maxChan = chan + (int) upperLimit;
		break;
	default:
		;
	}

}

/****************************** FindCFIndex ***********************************/

/*
 * This routine routine returns the number index an an array of frequencies
 * of the closet to a specified frequency.
 * It works recursively.
 * This function assumes that all the arguments are correctly set, i.e. no
 * argument validity checks are made here.
 * A special check is made when there are two CF's to be checked, in which case
 * the one that is closest to the specified frequency is chosen.
 * The value -1 is returned if there is an error.
 */

int
FindCFIndex_SignalData(SignalDataPtr signal, int minIndex, int maxIndex,
  double frequency)
{
	int		midPoint;

	/*if (minIndex == maxIndex)
		return(-1); */
	if ((maxIndex - minIndex) == 1) {
		if (fabs(frequency - signal->info.cFArray[minIndex]) < fabs(frequency -
		   signal->info.cFArray[maxIndex]))
			return(minIndex);
		else
			return(maxIndex);
	}
	midPoint = minIndex + (maxIndex - minIndex) / 2;
	if (frequency < signal->info.cFArray[midPoint])
		return(FindCFIndex_SignalData(signal, minIndex, midPoint, frequency));
	else
		return(FindCFIndex_SignalData(signal, midPoint, maxIndex, frequency));

}
