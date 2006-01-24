/******************
 *
 * File:		FiDataFile.c
 * Purpose:		This Filing module deals with the writing reading of data files.
 * Comments:	The sound format file routines were revised from sources from
 *				Lyon's Cochlear Model, The Program, Malcolm Slaney, Advanced
 *				Technology Group, Apple Computer, Inc., malcom@apple.com 1988.
 *				(Not the MicroSoft Wave file format routines.)
 *				10-11-97 LPO:  I have removed the SetTimeContinuity_EarObject()
 *				call from the individual routines.  The
 *				SetProcessContinuity_EarObject is now called only from the
 *				main ReadSignal_DataFile routine as it will reflect any
 *				internl changes in that routine.
 *				12-11-97 LPO: Put in dataFilePtr == NULL tests for setting
 *				routines.
 *				23-11-97 LPO: Added WriteOutSignal_DataFile_Named so that output
 *				can be specified from simulation specifications.
 *				14-04-98 LPO: Moved the sound file format specific routines into
 *				individual module files, i.e. FiRaw.[ch], FiAIFF.[ch] and
 *				FiMSWave.[ch].
 *				20-04-98 LPO: The dataFilePtr->maxSamples field is now set from
 *				the duration parameter which is read as a parameter.
 *				09-09-98 LPO: In ReadSignal_.. the SetContinuity_EarObject
 *				routine is only called if there is a successful read.
 *				03-02-99 LPO: The 'SetFileName_' routine now also sets the
 *				'type' specifier, It is still redundantly checked in the
 *				'ReadDataFile_' routine but this is kept to ensure that this
 *				part of the code can be used without the rest of DSAM.
 *				21-04-99 LPO: Changed the name of the 'WriteOutSignal' routine
 *				to 'WriteOutSignalMain'.  The new 'WriteOutSignal' now calls
 *				this routine, first setting 'dataFilePtr' to a temporary local
 *				pointer which is free'd after use.
 *				The same has been done for 'ReadSignal_'.
 *				Set the 'normalisation' initialisation to -1.0 so that 
 *				automatic normalisation is employed.
 *				27-04-99 LPO: Implemented 32 bit file support.
 *				04-08-99 LPO: The 'InitProcessVariables_' routine now correctly
 *				implements the auto-normalisation for calculating the
 *				'datafilePtr->normalise' parameter.
 * Authors:		L. P. O'Mard revised from Malcolm Slaney's code.
 * Created:		12 Jul 1993
 * Updated:		04 Aug 1999
 * Copyright:	(c) 1999, University of Essex
 * 
 ******************/

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG_H */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <float.h>
#include <limits.h>


#include "GeCommon.h"
#include "GeSignalData.h"
#include "GeEarObject.h"
#include "GeUniParMgr.h"
#include "GeModuleMgr.h"
#include "UtUIEEEFloat.h"
#include "UtUPortableIO.h"
#include "UtString.h"
#include "FiParFile.h"
#include "FiDataFile.h"
#include "FiRaw.h"
#include "FiAIFF.h"
#include "FiASCII.h"
#include "FiMSWave.h"

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

DataFilePtr dataFilePtr = NULL;

/******************************************************************************/
/****************************** Subroutines & functions ***********************/
/******************************************************************************/

/****************************** InitEndianModeList ****************************/

/*
 * This function initialises the 'normalisationMode' list array
 */

BOOLN
InitEndianModeList_DataFile(void)
{
	static NameSpecifier	modeList[] = {

			{ "DEFAULT",			DATA_FILE_DEFAULT_ENDIAN },
			{ "LITTLE",				DATA_FILE_LITTLE_ENDIAN },
			{ "LITTLE_UNSIGNED",	DATA_FILE_LITTLE_ENDIAN_UNSIGNED },
			{ "BIG",				DATA_FILE_BIG_ENDIAN },
			{ "BIG_UNSIGNED",		DATA_FILE_BIG_ENDIAN_UNSIGNED },
			{ "",					DATA_FILE_ENDIAN_NULL }
		};
	dataFilePtr->endianModeList = modeList;
	return(TRUE);

}

/**************************** GetDuration *************************************/

/*
 * This function returns the duration of signal stored in a file.
 * It is expected that it will only be used as a 'callback' routine for the
 * module, i.e. the module must be initialised to use it
 */

double
GetDuration_DataFile(void)
{
	static const char *funcName = "GetDuration_DataFile";
	double	duration;

	switch (Format_DataFile(GetSuffix_Utility_String(dataFilePtr->name))) {
	case	AIFF_DATA_FILE:
		duration = GetDuration_AIFF(dataFilePtr->name);
		break;
	case	ASCII_DATA_FILE:
		duration = GetDuration_ASCII(dataFilePtr->name);
		break;
	case	WAVE_DATA_FILE:
		duration = GetDuration_Wave(dataFilePtr->name);
		break;
	case	RAW_DATA_FILE:
		duration = GetDuration_Raw(dataFilePtr->name);
		break;
	default:
		duration = -1.0;
	} /* switch */
	if (duration < 0.0)
		NotifyError("%s: Could not calculate the total duration of the signal "
		  "for '%s'", funcName, dataFilePtr->name);
	return(duration);

}

/********************************* Init ***************************************/

/*
 * This function initialises the module by setting module's parameter pointer
 * structure.
 * The GLOBAL option is for hard programming - it sets the module's pointer to
 * the global parameter structure and initialises the parameters.
 * The LOCAL option is for generic programming - it initialises the parameter
 * structure currently pointed to by the module's parameter pointer.
 */

BOOLN
Init_DataFile(ParameterSpecifier parSpec)
{
	static const char *funcName = "Init_DataFile";

	if (parSpec == GLOBAL) {
		if (dataFilePtr != NULL)
			Free_DataFile();
		if ((dataFilePtr = (DataFilePtr) malloc(sizeof(DataFile))) == NULL) {
			NotifyError("%s: Out of memory for 'global' pointer", funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (dataFilePtr == NULL) { 
			NotifyError("%s:  'local' pointer not set.", funcName);
			return(FALSE);
		}
	}
	dataFilePtr->parSpec = parSpec;
	dataFilePtr->updateProcessVariablesFlag = TRUE;
	sprintf(dataFilePtr->name, "output.dat");
	dataFilePtr->wordSize = 2;
	dataFilePtr->endian = DATA_FILE_DEFAULT_ENDIAN;
	dataFilePtr->numChannels = 1;
	dataFilePtr->defaultSampleRate = 8000.0;
	dataFilePtr->duration = -1.0;
	dataFilePtr->timeOffset = 0.0;
	dataFilePtr->gain = 0.0;
	dataFilePtr->normalisation = -1.0;
	dataFilePtr->Write8Bits = Write8Bits;
	dataFilePtr->Write16Bits = Write16BitsHighLow;
	dataFilePtr->Write32Bits = Write32BitsHighLow;
	dataFilePtr->WriteIEEEExtended = WriteIEEEExtendedHighLow;
	dataFilePtr->Read16Bits = Read16BitsHighLow;
	dataFilePtr->Read32Bits = Read32BitsHighLow;
	dataFilePtr->ReadIEEEExtended = ReadIEEEExtendedHighLow;

	dataFilePtr->GetDuration = GetDuration_DataFile;
	InitEndianModeList_DataFile();
	if (!SetUniParList_DataFile()) {
		NotifyError("%s: Could not initialise parameter list.", funcName);
		Free_DataFile();
		return(FALSE);
	}
	dataFilePtr->normalise = 0.0;
	dataFilePtr->normOffset = 0.0;
	dataFilePtr->outputTimeOffset = 0.0;
	dataFilePtr->timeOffsetIndex = 0;
	dataFilePtr->timeOffsetCount = 0;
	dataFilePtr->maxSamples = 0;
	dataFilePtr->type = NULL_DATA_FILE;
	dataFilePtr->uIOPtr = NULL;
	return(TRUE);

}

/********************************* Free ***************************************/

/*
 * This function releases of the memory allocated for the process variables.
 * It should be called when the module is no longer in use.
 * It is defined as returning a BOOLN value because the generic module
 * interface requires that a non-void value be returned.
 * If the uIOPtr filed is set. it is expected that the assoiciated memory be
 * free'd elsewhere.
 */

BOOLN
Free_DataFile(void)
{
	if (dataFilePtr == NULL)
		return(TRUE);
	if (dataFilePtr->parList)
		FreeList_UniParMgr(&dataFilePtr->parList);

	if (dataFilePtr->parSpec == GLOBAL) {
		free(dataFilePtr);
		dataFilePtr = NULL;
	}
	return(TRUE);

}

/********************************* SetUniParList ******************************/

/*
 * This routine initialises and sets the module's universal parameter list.
 * This list provides universal access to the module's parameters.
 */
 
BOOLN
SetUniParList_DataFile(void)
{
	static const char *funcName = "SetUniParList_DataFile";
	UniParPtr	pars;

	if ((dataFilePtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  DATAFILE_NUM_PARS, NULL)) == NULL) {
		NotifyError("%s: Could not initialise parList.", funcName);
		return(FALSE);
	}
	pars = dataFilePtr->parList->pars;
	SetPar_UniParMgr(&pars[DATAFILE_FILENAME], "FILENAME",
	  "Stimulus file name.",
	  UNIPAR_FILE_NAME,
	  &dataFilePtr->name, (char *) "*.*",
	  (void * (*)) SetFileName_DataFile);
	SetPar_UniParMgr(&pars[DATAFILE_WORDSIZE], "WORDSIZE",
	  "Default word size for sound data (1,2 or 4 bytes)",
	  UNIPAR_INT,
	  &dataFilePtr->wordSize, NULL,
	  (void * (*)) SetWordSize_DataFile);
	SetPar_UniParMgr(&pars[DATAFILE_ENDIAN], "ENDIAN_MODE",
	  "Default (for format), 'little' or 'big'-endian.",
	  UNIPAR_NAME_SPEC,
	  &dataFilePtr->endian, dataFilePtr->endianModeList,
	  (void * (*)) SetEndian_DataFile);
	SetPar_UniParMgr(&pars[DATAFILE_NUMCHANNELS], "CHANNELS",
	  "No. of Channels: for raw binary and ASCII files.)",
	  UNIPAR_INT,
	  &dataFilePtr->numChannels, NULL,
	  (void * (*)) SetNumChannels_DataFile);
	SetPar_UniParMgr(&pars[DATAFILE_NORMALISE], "NORM_MODE",
	  "Normalisation factor for writing (either 0 or -ve: automatic).",
	  UNIPAR_REAL,
	  &dataFilePtr->normalisation, NULL,
	  (void * (*)) SetNormalisation_DataFile);
	SetPar_UniParMgr(&pars[DATAFILE_DEFAULTSAMPLERATE], "SAMPLERATE",
	  "Default sample rate: for raw binary and ASCII files (Hz).",
	  UNIPAR_REAL,
	  &dataFilePtr->defaultSampleRate, NULL,
	  (void * (*)) SetDefaultSampleRate_DataFile);
	SetPar_UniParMgr(&pars[DATAFILE_DURATION], "DURATION",
	  "Duration of read signal: -ve = unlimited (s)",
	  UNIPAR_REAL,
	  &dataFilePtr->duration, NULL,
	  (void * (*)) SetDuration_DataFile);
	SetPar_UniParMgr(&pars[DATAFILE_TIMEOFFSET], "STARTTIME",
	  "Start time (offset) for signal (s).",
	  UNIPAR_REAL,
	  &dataFilePtr->timeOffset, NULL,
	  (void * (*)) SetTimeOffset_DataFile);
	SetPar_UniParMgr(&pars[DATAFILE_GAIN], "GAIN",
	  "Relative signal gain (dB).",
	  UNIPAR_REAL,
	  &dataFilePtr->gain, NULL,
	  (void * (*)) SetGain_DataFile);

	return(TRUE);

}

/********************************* GetUniParListPtr ***************************/

/*
 * This routine returns a pointer to the module's universal parameter list.
 */

UniParListPtr
GetUniParListPtr_DataFile(void)
{
	static const char *funcName = "GetUniParListPtr_DataFile";

	if (dataFilePtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (dataFilePtr->parList == NULL) {
		NotifyError("%s: UniParList data structure has not been initialised. "
		  "NULL returned.", funcName);
		return(NULL);
	}
	return(dataFilePtr->parList);

}

/**************************** SetWordSize *************************************/

/*
 * This routine sets the word size for the binary file support.
 * Only the values 1, 2 or 4 are valid.
 */

BOOLN
SetWordSize_DataFile(int wordSize)
{
	static const char *funcName = "SetWordSize_DataFile";

	if (dataFilePtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if ((wordSize != 1) && (wordSize != 2) && (wordSize != 4)) {
		NotifyError("%s: Illegal value - must be 1, 2 or 4 (%d).", funcName,
		  wordSize);
		return(FALSE);
	}
	dataFilePtr->wordSize = wordSize;
	dataFilePtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);
	
}

/**************************** SetNormalisation ********************************/

/*
 * This routine sets the normalisation parameter for the binary file support.
 */

BOOLN
SetNormalisation_DataFile(double normalisation)
{
	static const char *funcName = "SetNormalisation_DataFile";

	if (dataFilePtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	dataFilePtr->normalisation = normalisation;
	dataFilePtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);
	
}

/**************************** SetNumChannels **********************************/

/*
 * This routine sets the number of channels for the binary file support.
 */

BOOLN
SetNumChannels_DataFile(int numChannels)
{
	static const char *funcName = "SetNumChannels_DataFile";

	if (dataFilePtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (numChannels < 1) {
		NotifyError("%s: Value - must be > 1 (%d).", funcName,
		  numChannels);
		return(FALSE);
	}
	dataFilePtr->numChannels = numChannels;
	dataFilePtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);
}



/**************************** GetMachineByteOrder *****************************/

/*
 * Sets the byte order according to the machine.
 */

DataFileEndianModeSpecifier
GetMachineByteOrder_DataFile(void)
{
	short int	word = 0x0001;
	char		*byte = (char *) &word;

	return((byte[0])? DATA_FILE_LITTLE_ENDIAN: DATA_FILE_BIG_ENDIAN);

}

/**************************** SetRWFormat *************************************/

/*
 * Sets the reading and writing routines of the 'dataFile' structure.
 * It also sets the DataFile structure's endian field, in case the default
 * endian was set and the calling routine has changed it.
 */

void
SetRWFormat_DataFile(int dataFormat)
{
	static const char *funcName = "SetRWFormat_DataFile";

	if (dataFilePtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return;
	}
	if (dataFormat == DATA_FILE_DEFAULT_ENDIAN)
		dataFormat = GetMachineByteOrder_DataFile();
	switch (dataFormat) {
	case DATA_FILE_LITTLE_ENDIAN:
	case DATA_FILE_LITTLE_ENDIAN_UNSIGNED:
		dataFilePtr->Write8Bits = Write8Bits;
		dataFilePtr->Write16Bits = Write16BitsLowHigh;
		dataFilePtr->Write32Bits = Write32BitsLowHigh;
		dataFilePtr->WriteIEEEExtended = WriteIEEEExtendedLowHigh;
		dataFilePtr->Read16Bits = Read16BitsLowHigh;
		dataFilePtr->Read32Bits = Read32BitsLowHigh;
		dataFilePtr->ReadIEEEExtended = ReadIEEEExtendedLowHigh;
		break;
	default:
		dataFilePtr->Write8Bits = Write8Bits;
		dataFilePtr->Write16Bits = Write16BitsHighLow;
		dataFilePtr->Write32Bits = Write32BitsHighLow;
		dataFilePtr->WriteIEEEExtended = WriteIEEEExtendedHighLow;
		dataFilePtr->Read16Bits = Read16BitsHighLow;
		dataFilePtr->Read32Bits = Read32BitsHighLow;
		dataFilePtr->ReadIEEEExtended = ReadIEEEExtendedHighLow;
		break;
	} /* Switch */

}

/**************************** SetEndian ***************************************/

/*
 * This routine sets the endedness of binary data file reading/writing to
 * big- or little-endian format.
 * It assigns the appropriate routine names to the 'dataFile' global
 * structure.
 * Can be set to default (big-), big- or little-endian.
 *
 */

BOOLN
SetEndian_DataFile(char *endian)
{
	static const char *funcName = "SetEndian_DataFile";
	int		specifier;

	if (dataFilePtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(endian,
		dataFilePtr->endianModeList)) == DATA_FILE_ENDIAN_NULL) {
		NotifyError("%s: Illegal name (%s).", funcName, endian);
		return(FALSE);
	}
	dataFilePtr->endian = specifier;
	SetRWFormat_DataFile(specifier);
	return(TRUE);
}

/**************************** ReadFileIdentifier ******************************/

/*
 * This routine attempts to read the specified identifier in one format then
 * changes to the other format if it fails.
 * It returns zero if it fails.
 */

int32
ReadFileIdentifier_DataFile(FILE *fp, int32 target, char *filetype)
{
	static const char *funcName = "ReadFileIdentifier_DataFile";
	int32	identifier;

	SetPosition_UPortableIO(fp, 0L, SEEK_SET);
	if ((identifier = dataFilePtr->Read32Bits(fp)) == target)
		return(identifier);
	dataFilePtr->endian = (dataFilePtr->endian == DATA_FILE_BIG_ENDIAN)?
	  DATA_FILE_LITTLE_ENDIAN: DATA_FILE_BIG_ENDIAN;
	SetRWFormat_DataFile(dataFilePtr->endian);
	SetPosition_UPortableIO(fp, 0L, SEEK_SET);
	if ((identifier = dataFilePtr->Read32Bits(fp)) == target)
		return(identifier);
	NotifyError("%s: Could not find %s identifier.", funcName, filetype);
	return(0);

}
	
/**************************** Format_DataFile *********************************/

/*
 * This routine returns the pointer to a data file format.
 * It also sets the dataFilePtr->type field.
 */

FileFormatSpecifier
Format_DataFile(char *suffix)
{
	static NameSpecifier soundFormatList[] = {

		{"AIF", AIFF_DATA_FILE},
		{"AIFF", AIFF_DATA_FILE},
		{"DAT", ASCII_DATA_FILE},
		{"RAW", RAW_DATA_FILE},
		{"WAV", WAVE_DATA_FILE},
		{"WAVE", WAVE_DATA_FILE},
		{"", NULL_DATA_FILE}

	};

	return(dataFilePtr->type = Identify_NameSpecifier(suffix, soundFormatList));

}

/**************************** FileLength **************************************/

/*
 * This function returns with the length of a file.
 * It assumes that the file has alread been opened.
 * It will not work with memory files.
 */

ChanLen
FileLength_DataFile(FILE *fp)
{
	ChanLen	pos, end;

	uPortableIOPtr = dataFilePtr->uIOPtr;
	pos = (ChanLen) GetPosition_UPortableIO(fp);
	SetPosition_UPortableIO(fp, 0L, SEEK_END);
	end = (ChanLen) GetPosition_UPortableIO(fp);
	SetPosition_UPortableIO(fp, (int32) pos, 0);
	return(end);
	
}

/**************************** SetIOSectionLength ******************************/

/*
 * This routine checks the size of sections read in if the segmented mode is
 * used.
 * It returns 0 if an attempt to read past the end of a recorded signal
 * length is made, otherwise it returns the section length to be read.
 * The section length is either the maxSamples field from the DataFile data
 * structure, or if less than that number of samples is left, then the number
 * of samples left is returned.
 * If it runs past the end of the signal, then a number <= 0 is returned,
 * otherwise it returns the I/O section length to be read/written.
 */

ChanLen
SetIOSectionLength_DataFile(EarObjectPtr data)
{
	if (!GetDSAMPtr_Common()->segmentedMode || (data->outSignal == NULL))
		return((ChanLen) ((dataFilePtr->numSamples > dataFilePtr->maxSamples)?
		 dataFilePtr->maxSamples: dataFilePtr->numSamples));

	if ( (dataFilePtr->numSamples - (int32) data->timeIndex) >
	  dataFilePtr->maxSamples)
		return(dataFilePtr->maxSamples);
	else
		return((ChanLen) dataFilePtr->numSamples - data->timeIndex);

}

/**************************** OpenFile ****************************************/

/*
 * This routine opens a file for reading, if required.
 * If the file name is "-" then "stdin" is returned.
 * The "stdin" file pointer is also returned if "+" is specfied, for 
 * transfer to memory using a file pointer.
 * If working with "memory" files, the memory pointer is set to the beginning
 * of the initialised memory.
 */

FILE *
OpenFile_DataFile(char *fileName, char *mode)
{
	static const char *funcName = "OpenFile_DataFile";
	char	*parFilePath;
	FILE	*fp, *dummy = stdout;

	uPortableIOPtr = dataFilePtr->uIOPtr;
	switch (*fileName) {
	case STDIN_STDOUT_FILE_DIRN:
		return((mode[0] == 'r')? stdin: stdout);
	case MEMORY_FILE_DIRN: /* Memory pointer */
		if (uPortableIOPtr == NULL)
			return((FILE *) NULL);
		uPortableIOPtr->memPtr = uPortableIOPtr->memStart;
		return(dummy);
	default:
		FreeMemory_UPortableIO(&dataFilePtr->uIOPtr);
		parFilePath = GetParsFileFPath_Common(fileName);
		if ((fp = fopen(parFilePath, mode)) == NULL) {
			NotifyError("%s: Couldn't open '%s' ('%s').", funcName, fileName,
			  mode);
			return((FILE *) NULL);
		}
		else
			return(fp);
	} /* switch */

}

/*************************** NumberOfColumns **********************************/

/*
 * This function determines the number of data columns in a file.
 */

int
NumberOfColumns_DataFile(FILE *fp)
{
	BOOLN	number;
	char	*c, line[MAXLINE_LARGE];
	int		numcols;

	if (fgets(line, MAXLINE_LARGE, fp) == NULL)
		return(0);
	/* Find the start of the first number */
	for(c = line; !isdigit(*c) && (*c != '.') && (*c != '-'); c++)
		;
	for (numcols = 0, number = FALSE; *c != '\0'; c++) {
		if (isdigit(*c) || (*c == '.') || (*c == '-') ||
		  (*c == '+') || (*c == 'E') || (*c == 'e')) {
			if (!number) {
				numcols++;
				number = TRUE;
			} 
		} else
			number = FALSE;
	}
	SetPosition_UPortableIO(fp, 0L, SEEK_SET);
	return(numcols);
	
} /* NumberOfColumns_DataFile */

/**************************** CalculateNormalisation **************************/

/*
 * This routine calculates the normalisation factor for a signal.
 * It expects the signal to be correctly initialised.
 * It returns zero if the maximum value is 0.0.
 * It returns the normalisation argument value, if it is greater than zero in
 * non-auto mode.
 */

double
CalculateNormalisation_DataFile(SignalDataPtr signal)
{
	int		chan;
	double 	maxValue;
	ChanLen	i;
	ChanData	*dataPtr;

	if (dataFilePtr->normalisation > DBL_EPSILON)
		maxValue = dataFilePtr->normalisation;
	else
		for (chan = 0, maxValue = -DBL_MAX; chan < signal->numChannels;
		  chan++) {
			dataPtr = signal->channel[chan];
			for (i = 0; i < signal->length; i++, dataPtr++)
				if (fabs(*dataPtr) > maxValue)
					maxValue = fabs(*dataPtr);
		}
	if (maxValue < DBL_EPSILON)
		return(1.0);
	dataFilePtr->normOffset = ((dataFilePtr->endian !=
	  DATA_FILE_LITTLE_ENDIAN_UNSIGNED) && (dataFilePtr->endian !=
	  DATA_FILE_BIG_ENDIAN_UNSIGNED))? 0.0: maxValue;
	return(DATAFILE_NORMALISE32(dataFilePtr->wordSize, maxValue));

}

/**************************** WriteSignal *************************************/

/*
 * This function writes out the channel data, dependent upon the
 * wordSize setting.
 * It assumes that the signal has been initialised.
 * It also assumes that the dataFilePtr->normalise field has been set.
 */

BOOLN
WriteSignal_DataFile(FILE *fp, SignalDataPtr signal)
{
	static const char *funcName = "WriteSignal_DataFile";
	int		j;
	ChanLen	i;
	
	switch (dataFilePtr->wordSize) {
	case	1:
		for (i = 0; i < signal->length; i++)
			for (j = 0; j < signal->numChannels; j++)
				dataFilePtr->Write8Bits(fp, (int) ((signal->channel[j][i] + 
				  dataFilePtr->normOffset) * dataFilePtr->normalise + 0.5));
		break;
	case	2:
		for (i = 0; i < signal->length; i++)
			for (j = 0; j < signal->numChannels; j++)
				dataFilePtr->Write16Bits(fp, (int16) ((signal->channel[j][i] + 
				  dataFilePtr->normOffset) * dataFilePtr->normalise + 0.5));
		break;
	case	4:
		for (i = 0; i < signal->length; i++)
			for (j = 0; j < signal->numChannels; j++)
				dataFilePtr->Write32Bits(fp, (int32) ((signal->channel[j][i] + 
				  dataFilePtr->normOffset) *  dataFilePtr->normalise + 0.5));
		break;
	default:
		NotifyError("%s: Unsupported sample size.", funcName);
		return(FALSE);
	} /* Switch */
	return(TRUE);

}

/********************************* SetDefaultSampleRate ***********************/

/*
 * This function sets the module's DefaultSampleRate parameter.
 * It returns TRUE if the operation is successful.
 */
 
BOOLN
SetDefaultSampleRate_DataFile(double theDefaultSampleRate)
{
	static const char *funcName = "SetDefaultSampleRate_DataFile";

	if (dataFilePtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	dataFilePtr->defaultSampleRate = theDefaultSampleRate;
	dataFilePtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/********************************* SetDuration ********************************/

/*
 * This function sets the module's duration parameter.
 * It returns TRUE if the operation is successful.
 */
 
BOOLN
SetDuration_DataFile(double theDuration)
{
	static const char *funcName = "SetDuration_DataFile";

	if (dataFilePtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	dataFilePtr->duration = theDuration;
	dataFilePtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/********************************* SetTimeOffset ******************************/

/*
 * This function sets the module's timeOffset parameter.
 * It returns TRUE if the operation is successful.
 */
 
BOOLN
SetTimeOffset_DataFile(double theTimeOffset)
{
	static const char *funcName = "SetTimeOffset_DataFile";

	if (dataFilePtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (theTimeOffset < 0.0) {
		NotifyError("%s: Illegal value (%g ms).", MILLI(theTimeOffset));
		return(FALSE);
	}
	dataFilePtr->timeOffset = theTimeOffset;
	dataFilePtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/********************************* SetGain ************************************/

/*
 * This function sets the module's gain parameter.
 * It returns TRUE if the operation is successful.
 */
 
BOOLN
SetGain_DataFile(double theGain)
{
	static const char *funcName = "SetGain_DataFile";

	if (dataFilePtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	dataFilePtr->gain = theGain;
	return(TRUE);

}

/**************************** SetFileName *************************************/

/*
 * This routine sets the 'name' field of the dataFile structure.
 * It is called as the ReadPars routine from the module manager.
 * The fileName field is used by the ReadSignal_DataFile_Named routine.
 */

BOOLN
SetFileName_DataFile(char *fileName)
{
	static const char *funcName = "SetFileName_DataFile";

	if (dataFilePtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (*fileName == (char) NULL) {
		NotifyError("%s: Illegal file name.", funcName);
		return(FALSE);
	}
	snprintf(dataFilePtr->name, MAX_FILE_PATH, "%s", fileName);
	Format_DataFile(GetSuffix_Utility_String(fileName));
	dataFilePtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/********************************* CheckPars **********************************/

/*
 * This routine checks that the necessary parameters for the module have been
 * correctly initialised.
 * It returns TRUE if there are no problems.
 */
 
BOOLN
CheckPars_DataFile(void)
{
	static const char *funcName = "CheckPars_DataFile";
	BOOLN	ok;
	
	ok = TRUE;
	if (dataFilePtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (dataFilePtr->name[0] == (char) NULL) {
		NotifyError("%s: File name not set.", funcName);
		ok = FALSE;
	}
	if (dataFilePtr->name[0] == (char) NULL) {
		NotifyError("%s: File name not set.", funcName);
		ok = FALSE;
	}
	if ((dataFilePtr->type == RAW_DATA_FILE) || (dataFilePtr->type ==
	  ASCII_DATA_FILE)) {
		if (dataFilePtr->numChannels < 1) {
			NotifyError("%s: Number of channels parameter incorrectly set.",
			  funcName);
			ok = FALSE;
		}
	}
	return(ok);
	
}	

/********************************* CheckParsRead ******************************/

/*
 * This routine checks that the necessary parameters for the module have been
 * correctly initialised for the specific case of reading files.
 * It expects the main CheckPars routine to have already confirmed that the
 * module has been correctly set. 
 * It returns TRUE if there are no problems.
 */
 
BOOLN
CheckParsRead_DataFile(void)
{
	static const char *funcName = "CheckParsRead_DataFile";
	BOOLN	ok;
	
	ok = TRUE;
	if ((dataFilePtr->type == RAW_DATA_FILE) || (dataFilePtr->type ==
	  ASCII_DATA_FILE)) {
		if ((dataFilePtr->name[0] == STDIN_STDOUT_FILE_DIRN) &&
		  (dataFilePtr->duration <= 0.0)) {
			NotifyError("%s: Duration must be set, i.e.greater than zero,\n"
			  "for pipes (%g s).", funcName, dataFilePtr->duration);
			ok = FALSE;
		}
	}
	return(ok);
	
}	

/********************************* SetPars ************************************/

/*
 * This function sets all the module's parameters.
 * It returns TRUE if the operation is successful.
 */
 
BOOLN
SetPars_DataFile(char *theFileName, int theWordSize, char *theEndian,
  int theNumChannels, double theDefaultSampleRate, double theDuration,
  double theTimeOffset, double theGain, double theNormalisation)
{
	static const char *funcName = "SetPars_DataFile";
	BOOLN	ok;
	
	ok = TRUE;
	if (!SetFileName_DataFile(theFileName))
		ok = FALSE;
	if (!SetWordSize_DataFile(theWordSize))
		ok = FALSE;
	if (!SetEndian_DataFile(theEndian))
		ok = FALSE;
	if (!SetNumChannels_DataFile(theNumChannels))
		ok = FALSE;
	if (!SetDefaultSampleRate_DataFile(theDefaultSampleRate))
		ok = FALSE;
	if (!SetDuration_DataFile(theDuration))
		ok = FALSE;
	if (!SetTimeOffset_DataFile(theTimeOffset))
		ok = FALSE;
	if (!SetGain_DataFile(theGain))
		ok = FALSE;
	if (!SetNormalisation_DataFile(theNormalisation))
		ok = FALSE;
	if (!ok)
		NotifyError("%s: Failed to set all module parameters.", funcName);
	return(ok);
	
}

/****************************** PrintPars *************************************/

/*
 * This program prints the parameters of the module to the standard output.
 */
 
BOOLN
PrintPars_DataFile(void)
{
	static const char *funcName = "PrintPars_DataFile";
	if (!CheckPars_DataFile()) {
		NotifyError("%s: Parameters have not been correctly set.", funcName);
		return(FALSE);
	}
	DPrint("DataFile Module Parameters:-\n");
	DPrint("\tFile name = %s\n", dataFilePtr->name);
	DPrint("\t%s endian format,\tDefault word size = %d\n",
	  dataFilePtr->endianModeList[dataFilePtr->endian].name,
	  dataFilePtr->wordSize);
	DPrint("\tNumber of channels = %d,", dataFilePtr->numChannels);
	DPrint("\tGain = %g dB\n", dataFilePtr->gain);
	DPrint("\tDefault sample rate = %g,\tDuration = ",
	  dataFilePtr->defaultSampleRate);
	if (dataFilePtr->duration < 0.0)
		DPrint("No limit,\n");
	else
		DPrint("%g ms,\n", MILLI(dataFilePtr->duration));
	DPrint("\tTime offset = %g ms.\n", MILLI(dataFilePtr->timeOffset));
	DPrint("\tNormalisation const: ");
	if (dataFilePtr->normalisation < DBL_EPSILON)
		DPrint("automatic.\n");
	else
		DPrint("%g\n", dataFilePtr->normalisation);
	return(TRUE);

}

/****************************** ReadPars **************************************/

/*
 * This program reads a specified number of parameters from a file.
 * It returns FALSE if it fails in any way.
 */
 
BOOLN
ReadPars_DataFile(char *parFileName)
{
	static const char *funcName = "ReadPars_DataFile";
	BOOLN	ok;
	char	fileName[MAX_FILE_PATH], *parFilePath, endian[MAXLINE];
	int		wordSize, numChannels;
	double  defaultSampleRate, duration, timeOffset, gain, normalisation;
	FILE	*fp;
	
    if (strcmp(parFileName, NO_FILE) == 0)
    	return(TRUE);
	parFilePath = GetParsFileFPath_Common(parFileName);
	if ((fp = fopen(parFilePath, "r")) == NULL) {
		NotifyError("%s: Cannot open data file '%s'.\n", funcName, parFilePath);
		return(FALSE);
	}
	DPrint("%s: Reading from '%s':\n", funcName, parFilePath);
	Init_ParFile();
	ok = TRUE;
	if (!GetPars_ParFile(fp, "%s", fileName))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%d", &wordSize))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%s", endian))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%d", &numChannels))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &normalisation))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &defaultSampleRate))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &duration))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &timeOffset))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &gain))
		ok = FALSE;
	fclose(fp);
	Free_ParFile();
	if (!ok) {
		NotifyError("%s:  Not enough, lines or invalid parameters, in module "\
		  "parameter file '%s'.", funcName, parFilePath);
		return(FALSE);
	}
	if (!SetPars_DataFile(fileName, wordSize, endian, numChannels,
	  defaultSampleRate, duration, timeOffset, gain, normalisation)) {
		NotifyError("%s: Could not set parameters.", funcName);
		return(FALSE);
	}
	return(TRUE);
	
}

/****************************** SetParsPointer ********************************/

/*
 * This function sets the global parameter pointer for the module to
 * that associated with the module instance.
 */

BOOLN
SetParsPointer_DataFile(ModulePtr theModule)
{
	static const char	*funcName = "SetParsPointer_DataFile";

	if (!theModule) {
		NotifyError("%s: The module is not set.", funcName);
		return(FALSE);
	}
	dataFilePtr = (DataFilePtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 */

BOOLN
InitModule_DataFile(ModulePtr theModule)
{
	static const char	*funcName = "InitModule_DataFile";

	if (!SetParsPointer_DataFile(theModule)) {
		NotifyError("%s: Cannot set parameters pointer.", funcName);
		return(FALSE);
	}
	if (!Init_DataFile(GLOBAL)) {
		NotifyError("%s: Could not initialise process structure.", funcName);
		return(FALSE);
	}
	dataFilePtr->inputMode = (strcmp(theModule->name, "DATAFILE_IN") == 0);
	if (dataFilePtr->inputMode)
		dataFilePtr->parList->pars[DATAFILE_NORMALISE].enabled = FALSE;
	else {
		dataFilePtr->parList->pars[DATAFILE_NUMCHANNELS].enabled = FALSE;
		dataFilePtr->parList->pars[DATAFILE_DEFAULTSAMPLERATE].enabled = FALSE;
		dataFilePtr->parList->pars[DATAFILE_DURATION].enabled = FALSE;
		dataFilePtr->parList->pars[DATAFILE_TIMEOFFSET].enabled = FALSE;
		dataFilePtr->parList->pars[DATAFILE_GAIN].enabled = FALSE;
	}
	theModule->parsPtr = dataFilePtr;
	theModule->CheckPars = CheckPars_DataFile;
	theModule->Free = Free_DataFile;
	theModule->GetUniParListPtr = GetUniParListPtr_DataFile;
	theModule->PrintPars = PrintPars_DataFile;
	theModule->ReadPars = ReadPars_DataFile;
	theModule->RunProcess = (dataFilePtr->inputMode)? ReadSignal_DataFile_Named:
	  WriteOutSignal_DataFile_Named;
	theModule->SetParsPointer = SetParsPointer_DataFile;
	return(TRUE);

}

/**************************** InitProcessVariables ****************************/

/*
 * This routine initialises the process variables for this module.
 * It assumes that all of the parameters for the module have been correctly
 * initialised.
 * It returns FALSE if it fails in anyway.
 */

BOOLN
InitProcessVariables_DataFile(EarObjectPtr data, ChanLen length,
  double sampleRate)
{
	static char *funcName = "InitProcessVariables_DataFile";
	
	if (dataFilePtr->updateProcessVariablesFlag || data->updateProcessFlag) {
		FreeProcessVariables_DataFile();
		if ((dataFilePtr->timeOffsetIndex = (ChanLen) floor(
		  dataFilePtr->timeOffset * sampleRate + 0.5)) >= length) {
			NotifyError("%s: Time offset (%g ms) must be shorter than signal.",
			  funcName, MILLI(dataFilePtr->timeOffset));
			return(FALSE);
		}
		dataFilePtr->timeOffsetCount = dataFilePtr->timeOffsetIndex;
		dataFilePtr->numSamples = (int32) (length -
		  dataFilePtr->timeOffsetCount);
		if (dataFilePtr->duration < 0.0)
			dataFilePtr->maxSamples = (int32) dataFilePtr->numSamples;
		else {
			if ((dataFilePtr->maxSamples = (int32) floor(dataFilePtr->duration *
			  sampleRate + 0.5)) > (int32) dataFilePtr->numSamples) {
				NotifyError("%s: Duration (%g ms) is longer than signal.",
				  funcName, MILLI(dataFilePtr->duration));
				return(FALSE);
			}
		}
		switch (dataFilePtr->wordSize) {
		case 1:
			dataFilePtr->normalise = (double) (0xff >> 1);
			break;
		case 2:
			dataFilePtr->normalise = (double) (0xffff >> 1);
			break;
		case 4:
			dataFilePtr->normalise = (double) (0xffffffff >> 1);
			break;
		default:
			NotifyError("%s: Unsupported sample bit size = %d.", funcName,
			  dataFilePtr->wordSize);
			return(FALSE);
		} /* switch */		
		if (dataFilePtr->normalisation > DBL_EPSILON)
		 	dataFilePtr->normalise /= dataFilePtr->normalisation;
		dataFilePtr->updateProcessVariablesFlag = FALSE;
	}
	return(TRUE);

}

/**************************** FreeProcessVariables ****************************/

/*
 * This routine releases any memory allocated for the process variables
 * if they have been initialised.
 */

void
FreeProcessVariables_DataFile(void)
{
	dataFilePtr->updateProcessVariablesFlag = TRUE;

}

/**************************** ReadSample **************************************/

/*
 * This function reads a sample from a specified file pointer.
 * It assumes that the file pointer has been correctly initialised, and that
 * no other checking, such as end of file, is required.
 * This routine is required because the Windows platform disliked the use of the
 * ReadSample pointer.
 */

ChanData
ReadSample_DataFile(FILE *fp)
{
	static const char *funcName = "ReadSample_DataFile";
	ChanData	sample;

	switch (dataFilePtr->wordSize) {
	case 1:
		sample = (ChanData) Read8Bits(fp);
		break;
	case 2:
		sample = (ChanData) dataFilePtr->Read16Bits(fp);
		break;
	case 4:
		sample = (ChanData) dataFilePtr->Read32Bits(fp);
		break;
	default:
		NotifyError("%s: Unsupported sample bit size = %d.", funcName,
		  dataFilePtr->wordSize);
		return(0.0);
	} /* switch */		
	return(sample / dataFilePtr->normalise);

}

/**************************** ReadSignal **************************************/

/*
 * This function reads a file in one of the supported file types.
 * It distinguishes sound file formats by the file suffix used.
 * The gain is only applied if the absolute value is greater than DBL_EPSILON.
 * It is the version which can be called without implicitly initialising the
 * data file module, i.e. it can be used in user programs.
 */

BOOLN
ReadSignal_DataFile(char *fileName, EarObjectPtr data)
{
	/* static const char *funcName = "ReadSignal_DataFile"; */
	BOOLN	ok;
	DataFile	tempDataFile, *oldPtr;

	oldPtr = dataFilePtr;
	dataFilePtr = &tempDataFile;
	Init_DataFile(LOCAL);
	dataFilePtr->inputMode = TRUE;
	ok = ReadSignalMain_DataFile(fileName, data);
	Free_DataFile();
	dataFilePtr = oldPtr;
	return(ok);

}

/**************************** ReadSignalMain **********************************/

/*
 * This function reads a file in one of the supported file types.
 * It distinguishes sound file formats by the file suffix used.
 * The gain is only applied if the absolute value is greater than DBL_EPSILON.
 */

BOOLN
ReadSignalMain_DataFile(char *fileName, EarObjectPtr data)
{
	static const char *funcName = "ReadSignalMain_DataFile";
	BOOLN	ok;

	if (dataFilePtr == NULL) {
		if (!Init_DataFile(GLOBAL)) {
			NotifyError("%s: Could not initialise parameters.", funcName);
			return(FALSE);
		}
	} else if (!CheckPars_DataFile() || !CheckParsRead_DataFile()) {
		NotifyError("%s: Parameters have not been correctly set.", funcName);
		return(FALSE);
	}
	Format_DataFile(GetSuffix_Utility_String(fileName));
	uPortableIOPtr = dataFilePtr->uIOPtr;
	switch (dataFilePtr->type) {
	case	AIFF_DATA_FILE:
		ok = ReadFile_AIFF(fileName, data);
		break;
	case	ASCII_DATA_FILE:
		ok = ReadFile_ASCII(fileName, data);
		break;
	case	WAVE_DATA_FILE:
		ok = ReadFile_Wave(fileName, data);
		break;
	case	RAW_DATA_FILE:
	default	:
		ok = ReadFile_Raw(fileName, data);
		break;
	} /* switch */
	if (!ok && !GetDSAMPtr_Common()->segmentedMode)
		NotifyError("%s: Could not read file '%s'.", funcName, fileName);
	if (ok) {
		if (fabs(dataFilePtr->gain) > DBL_EPSILON)
			GaindB_SignalData(data->outSignal, dataFilePtr->gain);
		if (!data->outSignal->staticTimeFlag)
			SetOutputTimeOffset_SignalData(data->outSignal,
			  dataFilePtr->timeOffsetIndex * data->outSignal->dt +
			  dataFilePtr->outputTimeOffset);
		data->outSignal->rampFlag = TRUE;	/* Let user sort out ramps */
		SetProcessContinuity_EarObject(data);
	}
	return(ok);
	
}

/**************************** WriteOutSignal **********************************/

/*
 * This function writes a file in one of the supported file types.
 * It distinguishes sound file formats by the file suffix used.
 * It is the version which can be called without implicitly initialising the
 * data file module, i.e. it can be used in user programs.
 */

BOOLN
WriteOutSignal_DataFile(char *fileName, EarObjectPtr data)
{
	/* static const char *funcName = "WriteOutSignal_DataFile"; */
	BOOLN	ok;
	DataFile	tempDataFile, *oldPtr;

	oldPtr = dataFilePtr;
	dataFilePtr = &tempDataFile;
	Init_DataFile(LOCAL);
	dataFilePtr->inputMode = FALSE;
	ok = WriteOutSignalMain_DataFile(fileName, data);
	Free_DataFile();
	dataFilePtr = oldPtr;
	return(ok);

}

/**************************** WriteOutSignalMain ******************************/

/*
 * This function writes a file in one of the supported file types.
 * It distinguishes sound file formats by the file suffix used.
 */

BOOLN
WriteOutSignalMain_DataFile(char *fileName, EarObjectPtr data)
{
	static const char *funcName = "WriteOutSignalMain_DataFile";
	BOOLN	ok = FALSE;
	
	if (!data || !data->outSignal) {
		NotifyError("%s: EarObject not initialised.", funcName);
		return(FALSE);
	}
	if (dataFilePtr == NULL) {
		if (!Init_DataFile(GLOBAL)) {
			NotifyError("%s: Could not initialise parameters.", funcName);
			return(FALSE);
		}
		dataFilePtr->inputMode = FALSE;
	} else if (!CheckPars_DataFile()) {
		NotifyError("%s: Parameters have not been correctly set.", funcName);
		return(FALSE);
	}
	uPortableIOPtr = dataFilePtr->uIOPtr;
	switch (Format_DataFile(GetSuffix_Utility_String(fileName))) {
	case	AIFF_DATA_FILE:
		ok = WriteFile_AIFF(fileName, data);
		break;
	case	ASCII_DATA_FILE:
		ok = WriteFile_ASCII(fileName, data);
		break;
	case	WAVE_DATA_FILE:
		ok = WriteFile_Wave(fileName, data);
		break;
	case	RAW_DATA_FILE:
	default	:
		ok = WriteFile_Raw(fileName, data);
		break;
	}
	if (!ok)
		NotifyError("%s: Could not write file '%s'.", funcName, fileName);
	return(ok);

}

/**************************** ReadSignal...Named ******************************/

/*
 * This routine calls the ReadSignal_DataFile routine using the named file
 * in the dataFile structure.
 */

BOOLN
ReadSignal_DataFile_Named(EarObjectPtr data)
{
	return(ReadSignalMain_DataFile(dataFilePtr->name, data));

}

/**************************** WriteOutSignal...Named **************************/

/*
 * This routine calls the WriteOutSignal_DataFile routine using the named file
 * in the dataFile structure.
 * The output signal is manually connected to the input signal.
 */

BOOLN
WriteOutSignal_DataFile_Named(EarObjectPtr data)
{
	/* static const char *funcName = "WriteOutSignal_DataFile_Named"; */

	if (!data->threadRunFlag) {
		data->outSignal = data->inSignal[0];
		if (data->initThreadRunFlag)
			return(TRUE);
	}
	SetProcessContinuity_EarObject(data);
	return(WriteOutSignalMain_DataFile(dataFilePtr->name, data));

}

