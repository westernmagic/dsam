/******************
 *
 * File:		FiASCII.c
 * Purpose:		This Filing module deals with the writing reading of raw binary
 *				data files.
 * Comments:	17-04-98 LPO: This module has now been separated from the main
 *				FiDataFile module.
 * Authors:		L. P. O'Mard
 * Created:		17 Apr 1998
 * Updated:		
 * Copyright:	(c) 1998, University of Essex
 * 
 ******************/

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG_H */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "GeCommon.h"
#include "GeSignalData.h"
#include "GeEarObject.h"
#include "GeUniParMgr.h"
#include "UtUIEEEFloat.h"
#include "UtUPortableIO.h"
#include "UtString.h"
#include "FiParFile.h"
#include "FiDataFile.h"
#include "FiASCII.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Subroutines & functions ***********************/
/******************************************************************************/

/**************************** ReadFile ****************************************/

/*
 * This function reads a file in ASCII stream format.
 * The data is stored in the output signal of an EarObject.
 * This is the default file, and is assumed if no suffix is given
 * If a dash, '-.suffix', is given as the file name, then the data will be 
 * read from the standard input.
 * It returns TRUE if successful.
 * In segmented mode, this routine returns FALSE when it gets to the end of
 * the signal.
 */

BOOLN
ReadFile_ASCII(char *fileName, EarObjectPtr data)
{
	static const char *funcName = "ReadFile_ASCII";
	BOOLN	endOfSignal;
	char	line[MAXLINE_LARGE];
	int		chan, numColumns;
	double	dt;
	ChanLen	i, length, numSamples;
	FILE	*fp;

	if (!CheckPars_DataFile()) {
		NotifyError("%s: Parameters have not been correctly set.", funcName);
		return(FALSE);
	}
	if ((fp = OpenFile_DataFile(fileName, FOR_ASCII_READING)) == NULL) {
		NotifyError("%s: Couldn't open file.", funcName);
		return(FALSE);
	}
	if ((numColumns = NumberOfColumns_DataFile(fp)) == 0) {
		NotifyError("%s: Illegal no. of columns: '%s'.", funcName, fileName);
		return(FALSE);
	}
	SetProcessName_EarObject(data, "'%s' ASCII (dat) file",
	  GetFileNameFPath_Utility_String(fileName));
	numSamples = (int32) floor(dataFilePtr->duration *
	  dataFilePtr->defaultSampleRate + 1.5);
	if (!InitProcessVariables_DataFile(data, numSamples,
	  dataFilePtr->defaultSampleRate)) {
		NotifyError("%s: Could not initialise process variables.", funcName);
		return(FALSE);
	}
	dt = 1.0 / dataFilePtr->defaultSampleRate;
	length = dataFilePtr->maxSamples;
	if (!InitOutSignal_EarObject(data, numColumns, length, dt) ) {
		NotifyError("%s: Cannot initialise output signal", funcName);
		return(FALSE);
	}
	for (i = 0; i < data->timeIndex; i++)
		if (fgets(line, MAXLINE_LARGE, fp) == NULL)
			return(FALSE);
	for (i = 0, endOfSignal = FALSE; !endOfSignal && (i < length); i++)
		for (chan = 0; (chan < data->outSignal->numChannels); chan++)
			if (fscanf(fp, "%lf", &data->outSignal->channel[chan][i]) == EOF) {
				endOfSignal = TRUE;
				break;
			}
	if (endOfSignal) {
		if ((data->outSignal->length = i - 1) == 0) {
			if (!GetDSAMPtr_Common()->segmentedMode)
				NotifyError("%s: Couldn't read samples from the file '%s'.",
				  funcName ,fileName);
			return(FALSE);
		}
	}
	if (fp != stdin)
		fclose(fp);
	return(TRUE);

}

/**************************** WriteFile ***************************************/

/*
 * This routine sends the output signal of an EarObject to file.
 * It assumes that the GeEarObject.has been initialised.
 */
 
BOOLN
WriteFile_ASCII(char *fileName, EarObjectPtr data)
{
	static const char *funcName = "WriteFile_ASCII";

	if (!CheckPars_SignalData(data->outSignal)) {
		NotifyError("%s: Output signal not initialised.", funcName);
		return(FALSE);
	}
	SetTimeIndex_SignalData(data->outSignal, _WorldTime_EarObject(data));
	if (!OutputToFile_SignalData(fileName, data->outSignal)) {
		if (data->processName != NULL)
			NotifyWarning("%s: Data from EarObject: %s, has not been output "
			  "to file.", funcName, data->processName);
		else
			NotifyWarning("%s: Data from EarObject: <no name>, has not been "
			  "output to file.", funcName);
		return(FALSE);
	} else {
		return(TRUE);
	}
		
}


