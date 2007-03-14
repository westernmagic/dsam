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
 * Authors:		L. P. O'Mard.
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
#include <sndfile.h>

#include "GeCommon.h"
#include "GeSignalData.h"
#include "GeEarObject.h"
#include "GeUniParMgr.h"
#include "GeModuleMgr.h"
#include "UtString.h"
#include "FiParFile.h"
#include "FiDataFile.h"
#include "FiASCII.h"
#include "FiSndFile.h"

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

DataFilePtr dataFilePtr = NULL;

struct {

	int		*formats;
	int		*subFormats;
	NameSpecifier	*formatList;
	NameSpecifier	*subFormatList;

} dataFileSndFormatInfo = {

	NULL,
	NULL,
	NULL,
	NULL

};

/******************************************************************************/
/****************************** Subroutines & functions ***********************/
/******************************************************************************/

/****************************** InitEndianModeList ****************************/

/*
 * This function initialises the 'endianMode' list array
 */

BOOLN
InitEndianModeList_DataFile(void)
{
	static NameSpecifier	modeList[] = {

			{ wxT("DEFAULT"),			DATA_FILE_DEFAULT_ENDIAN },
			{ wxT("LITTLE"),			DATA_FILE_LITTLE_ENDIAN },
			{ wxT("BIG"),				DATA_FILE_BIG_ENDIAN },
			{ wxT("CPU"),				DATA_FILE_CPU_ENDIAN },
			{ NULL,						DATA_FILE_ENDIAN_NULL }
		};
	dataFilePtr->endianModeList = modeList;
	return(TRUE);

}

/****************************** InitDSAMFormatList ***************************/

/*
 * This function initialises the 'DSAM Format' list array
 */

NameSpecifier *
DSAMFormatList_DataFile(int index)
{
	static NameSpecifier	modeList[] = {

			{ wxT("IL"),	DATA_FILE_INTERLEAVELEVEL },
			{ wxT("NWF"),	DATA_FILE_NUMWINDOWFRAMES },
			{ wxT("STF"),	DATA_FILE_STATICTIMEFLAG },
			{ wxT("OTO"),	DATA_FILE_OUTPUTTIMEOFFSET },
			{ wxT("NORM"),	DATA_FILE_NORMALISATION },
			{ wxT("VER"),	DATA_FILE_DSAMVERSION },
			{ NULL,						-1 }
		};
	return (&modeList[index]);

}

/****************************** FreeSoundFormatLists **************************/

/*
 * Free's the global sound file format lists.
 */

void
FreeSoundFormatLists_DataFile(void)
{
	if (dataFileSndFormatInfo.formats) {
		free(dataFileSndFormatInfo.formats);
		dataFileSndFormatInfo.formats = NULL;
	}
	if (dataFileSndFormatInfo.subFormats) {
		free(dataFileSndFormatInfo.subFormats);
		dataFileSndFormatInfo.subFormats = NULL;
	}
	FreeNameAllocatedList_NameSpecifier(&dataFileSndFormatInfo.formatList);
	FreeNameAllocatedList_NameSpecifier(&dataFileSndFormatInfo.subFormatList);

}

/****************************** InitSoundFormatList ***************************/

/*
 * This function initialises the 'soundFormatList' global list array.
 * It retrieves the information from the 'libsndFile' library routines.
 */

BOOLN
InitSoundFormatList_DataFile(void)
{
	static const WChar *funcName = wxT("InitSoundFormatList_DataFile");
	int		i, majorFormatCount, localFormatCount = 0, count, *iP;
	NameSpecifierPtr	p;
	SF_FORMAT_INFO	info ;
	NameSpecifier localSoundFormatList[] = {
		{wxT("DAT"),	ASCII_DATA_FILE},
		{NULL,			NULL_DATA_FILE}
	};

	if (dataFileSndFormatInfo.formatList)
		return(TRUE);
	for (p = localSoundFormatList; p->name; p++)
		localFormatCount++;
	sf_command (NULL, SFC_GET_FORMAT_MAJOR_COUNT, &majorFormatCount,
	  sizeof (int)) ;
	count = majorFormatCount + localFormatCount + 1;
	if ((dataFileSndFormatInfo.formatList = (NameSpecifier *) calloc(count,
	  sizeof(NameSpecifier))) == NULL) {
		NotifyError(wxT("%s: Out of memory for sound format list (%d)"),
		  funcName, count);
		return(FALSE);
	}
	if ((dataFileSndFormatInfo.formats = (int *) calloc(count, sizeof(int))) == NULL) {
		NotifyError(wxT("%s: Out of memory for sound format array (%d)"),
		  funcName, count);
		return(FALSE);
	}
	for (i = 0, p = dataFileSndFormatInfo.formatList, iP = dataFileSndFormatInfo.formats;
	  i < localFormatCount; i++, p++, iP++) {
		p->name = InitString_Utility_String(localSoundFormatList[i].name);
		p->specifier = localSoundFormatList[i].specifier;
		*iP = -1;
	}
	for (i = 0;  i < majorFormatCount; i++, p++, iP++) {
		info.format = i;
		sf_command(NULL, SFC_GET_FORMAT_MAJOR, &info, sizeof (info));
		p->name = InitString_Utility_String(MBSToWCS_Utility_String(
		  info.extension));
		ToUpper_Utility_String(p->name, p->name);
		p->specifier = localSoundFormatList[localFormatCount - 1].specifier + i + 1;
		*iP = info.format;
	}
	p->name = NULL;
	p->specifier = NULL_DATA_FILE;
	*iP = -1;
	return(TRUE);

}

/****************************** InitSoundSubFormatList *************************/

/*
 * This function initialises the 'InitSoundSubFormatList' global list array.
 * It retrieves the information from the 'libsndFile' library routines.
 */

BOOLN
InitSoundSubFormatList_DataFile(void)
{
	static const WChar *funcName = wxT("InitSoundSubFormatList_DataFile");
	int		i, subTypeFormatCount, localSubFormatCount = 0, count, *iP;
	SF_FORMAT_INFO	info ;
	NameSpecifierPtr	p;
	NameSpecifier localSoundSubFormatList[] = {
		{wxT("ASCII"),	ASCII_DATA_FILE},
		{NULL,			NULL_DATA_FILE}
	};

	if (dataFileSndFormatInfo.subFormatList)
		return(TRUE);
	for (p = localSoundSubFormatList; p->name; p++)
		localSubFormatCount++;
	sf_command (NULL, SFC_GET_FORMAT_SUBTYPE_COUNT, &subTypeFormatCount,
	  sizeof (int)) ;
	count = subTypeFormatCount + localSubFormatCount + 1;
	if ((dataFileSndFormatInfo.subFormatList = (NameSpecifier *) calloc(count, sizeof(
	  NameSpecifier))) == NULL) {
		NotifyError(wxT("%s: Out of memory for sound sub-type format list (%d)"),
		  funcName, subTypeFormatCount);
		return(FALSE);
	}
	if ((dataFileSndFormatInfo.subFormats = (int *) calloc(count, sizeof(int))) == NULL) {
		NotifyError(wxT("%s: Out of memory for sound sub-format array (%d)"),
		  funcName, count);
		return(FALSE);
	}
	for (i = 0, p = dataFileSndFormatInfo.subFormatList, iP = dataFileSndFormatInfo.subFormats;
	  i < localSubFormatCount; i++, p++, iP++) {
		p->name = InitString_Utility_String(localSoundSubFormatList[i].name);
		p->specifier = localSoundSubFormatList[i].specifier;
		*iP = -1;
	}
	for (i = 0; i < subTypeFormatCount; i++, p++, iP++) {
		info.format = i;
		sf_command(NULL, SFC_GET_FORMAT_SUBTYPE, &info, sizeof (info));
		p->name = InitString_Utility_String(MBSToWCS_Utility_String(
		  info.name));
		ToUpper_Utility_String(p->name, p->name);
		p->specifier = localSoundSubFormatList[localSubFormatCount - 1].specifier + i + 1;
		*iP = info.format;
	}
	p->name = NULL;
	p->specifier = NULL_DATA_FILE;
	*iP = -1;
	return(TRUE);

}

/****************************** GetSndFormat ***********************************/

/*
 * This function returns the corresponding sound format.
 */

int
GetSndFormat_DataFile(int specifier)
{
	return(dataFileSndFormatInfo.formats[specifier]);

}

/****************************** GetSndSubFormat *******************************/

/*
 * This function returns the corresponding sound sub-format.
 */

int
GetSndSubFormat_DataFile(int specifier)
{
	return(dataFileSndFormatInfo.subFormats[specifier]);

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
	static const WChar *funcName = wxT("GetDuration_DataFile");
	double	duration;

	switch (Format_DataFile(GetSuffix_Utility_String(dataFilePtr->name))) {
	case	ASCII_DATA_FILE:
		duration = GetDuration_ASCII(dataFilePtr->name);
		break;
	default:
		duration = -1.0;
	} /* switch */
	if (duration < 0.0)
		NotifyError(wxT("%s: Could not calculate the total duration of the ")
		  wxT("signal for '%s'"), funcName, dataFilePtr->name);
	return(duration);

}

/********************************* ResetSFInfo ********************************/

/*
 * Reset the SFInfo structure.
 */

void
ResetSFInfo_DataFile(SF_INFO *p)
{
	p->frames = 0;
	p->samplerate = 0;
	p->channels = 0;
	p->format = 0;
	p->sections = 0;
	p->seekable = 0;	

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
	static const WChar *funcName = wxT("Init_DataFile");

	if (parSpec == GLOBAL) {
		if (dataFilePtr != NULL)
			Free_DataFile();
		if ((dataFilePtr = (DataFilePtr) malloc(sizeof(DataFile))) == NULL) {
			NotifyError(wxT("%s: Out of memory for 'global' pointer"),
			  funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (dataFilePtr == NULL) { 
			NotifyError(wxT("%s:  'local' pointer not set."), funcName);
			return(FALSE);
		}
	}
	dataFilePtr->parSpec = parSpec;
	dataFilePtr->updateProcessVariablesFlag = TRUE;
	DSAM_strncpy(dataFilePtr->name, wxT("output.dat"), MAXLINE);
	dataFilePtr->subFormatType = 1 + ASCII_DATA_FILE;
	dataFilePtr->wordSize = 2;
	dataFilePtr->endian = DATA_FILE_DEFAULT_ENDIAN;
	dataFilePtr->numChannels = 1;
	dataFilePtr->defaultSampleRate = 8000.0;
	dataFilePtr->duration = -1.0;
	dataFilePtr->timeOffset = 0.0;
	dataFilePtr->gain = 0.0;
	dataFilePtr->normalisation = -1.0;

	dataFilePtr->GetDuration = GetDuration_DataFile;
	InitEndianModeList_DataFile();
	if (!InitSoundFormatList_DataFile()) {
		NotifyError(wxT("%s: Could not initialise sound format list."),
		  funcName);
		Free_DataFile();
		return(FALSE);
	}
	if (!InitSoundSubFormatList_DataFile()) {
		NotifyError(wxT("%s: Could not initialise sound sub-type format list."),
		  funcName);
		Free_DataFile();
		return(FALSE);
	}
	if (!SetUniParList_DataFile()) {
		NotifyError(wxT("%s: Could not initialise parameter list."), funcName);
		Free_DataFile();
		return(FALSE);
	}
	dataFilePtr->titleString = NULL;
	dataFilePtr->outputTimeOffset = 0.0;
	dataFilePtr->timeOffsetIndex = 0;
	dataFilePtr->timeOffsetCount = 0;
	dataFilePtr->maxSamples = 0;
	dataFilePtr->type = NULL_DATA_FILE;
	ResetSFInfo_DataFile(&dataFilePtr->sFInfo);
	dataFilePtr->sndFile = NULL;
	dataFilePtr->vIOFuncs = NULL;
	dataFilePtr->vIOPtr = NULL;
	dataFilePtr->buffer = NULL;
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
	if (dataFilePtr->vIOFuncs)
		free(dataFilePtr->vIOFuncs);
	if (dataFilePtr->vIOPtr && !dataFilePtr->inputMode)
		FreeVirtualIOMemory_SndFile(&dataFilePtr->vIOPtr);
	Free_SndFile();
	FreeProcessVariables_DataFile();
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
	static const WChar *funcName = wxT("SetUniParList_DataFile");
	UniParPtr	pars;

	if ((dataFilePtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  DATAFILE_NUM_PARS, NULL)) == NULL) {
		NotifyError(wxT("%s: Could not initialise parList."), funcName);
		return(FALSE);
	}
	pars = dataFilePtr->parList->pars;
	SetPar_UniParMgr(&pars[DATAFILE_FILENAME], wxT("FILENAME"),
	  wxT("Stimulus file name."),
	  UNIPAR_FILE_NAME,
	  &dataFilePtr->name, (WChar *) wxT("*.*"),
	  (void * (*)) SetFileName_DataFile);
	SetPar_UniParMgr(&pars[DATAFILE_SUBFORMATTYPE], wxT("SUB_FORMAT_TYPE"),
	  wxT("Sound file format sub-type."),
	  UNIPAR_NAME_SPEC,
	  &dataFilePtr->subFormatType, dataFileSndFormatInfo.subFormatList,
	  (void * (*)) SetSubFormatType_DataFile);
	SetPar_UniParMgr(&pars[DATAFILE_WORDSIZE], wxT("WORDSIZE"),
	  wxT("Default word size for sound data (1,2 or 4 bytes)"),
	  UNIPAR_INT,
	  &dataFilePtr->wordSize, NULL,
	  (void * (*)) SetWordSize_DataFile);
	SetPar_UniParMgr(&pars[DATAFILE_ENDIAN], wxT("ENDIAN_MODE"),
	  wxT("Default (for format), 'little' or 'big'-endian."),
	  UNIPAR_NAME_SPEC,
	  &dataFilePtr->endian, dataFilePtr->endianModeList,
	  (void * (*)) SetEndian_DataFile);
	SetPar_UniParMgr(&pars[DATAFILE_NUMCHANNELS], wxT("CHANNELS"),
	  wxT("No. of Channels: for raw binary and ASCII files.)"),
	  UNIPAR_INT,
	  &dataFilePtr->numChannels, NULL,
	  (void * (*)) SetNumChannels_DataFile);
	SetPar_UniParMgr(&pars[DATAFILE_NORMALISE], wxT("NORM_MODE"),
	  wxT("Normalisation factor for writing (either 0 or -ve: automatic)."),
	  UNIPAR_REAL,
	  &dataFilePtr->normalisation, NULL,
	  (void * (*)) SetNormalisation_DataFile);
	SetPar_UniParMgr(&pars[DATAFILE_DEFAULTSAMPLERATE], wxT("SAMPLERATE"),
	  wxT("Default sample rate: for raw binary and ASCII files (Hz)."),
	  UNIPAR_REAL,
	  &dataFilePtr->defaultSampleRate, NULL,
	  (void * (*)) SetDefaultSampleRate_DataFile);
	SetPar_UniParMgr(&pars[DATAFILE_DURATION], wxT("DURATION"),
	  wxT("Duration of read signal: -ve = unlimited (s)"),
	  UNIPAR_REAL,
	  &dataFilePtr->duration, NULL,
	  (void * (*)) SetDuration_DataFile);
	SetPar_UniParMgr(&pars[DATAFILE_TIMEOFFSET], wxT("STARTTIME"),
	  wxT("Start time (offset) for signal (s)."),
	  UNIPAR_REAL,
	  &dataFilePtr->timeOffset, NULL,
	  (void * (*)) SetTimeOffset_DataFile);
	SetPar_UniParMgr(&pars[DATAFILE_GAIN], wxT("GAIN"),
	  wxT("Relative signal gain (dB)."),
	  UNIPAR_REAL,
	  &dataFilePtr->gain, NULL,
	  (void * (*)) SetGain_DataFile);

	dataFilePtr->parList->pars[DATAFILE_WORDSIZE].enabled = FALSE;
	return(TRUE);

}

/********************************* GetUniParListPtr ***************************/

/*
 * This routine returns a pointer to the module's universal parameter list.
 */

UniParListPtr
GetUniParListPtr_DataFile(void)
{
	static const WChar *funcName = wxT("GetUniParListPtr_DataFile");

	if (dataFilePtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (dataFilePtr->parList == NULL) {
		NotifyError(wxT("%s: UniParList data structure has not been ")
		  wxT("initialised. NULL returned."), funcName);
		return(NULL);
	}
	return(dataFilePtr->parList);

}

/****************************** SetSubFormatType ******************************/

/*
 * This function sets the module's subFormatType parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetSubFormatType_DataFile(WChar * theSubFormatType)
{
	static const WChar	*funcName = wxT("SetSubFormatType_DataFile");
	int		specifier;

	if (dataFilePtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theSubFormatType,
		dataFileSndFormatInfo.subFormatList)) == NULL_DATA_FILE) {
		NotifyError(wxT("%s: Illegal name (%s)."), funcName, theSubFormatType);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	dataFilePtr->subFormatType = specifier;
	dataFilePtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/**************************** SetWordSize *************************************/

/*
 * This routine sets the word size for the binary file support.
 * Only the values 1, 2 or 4 are valid.
 */

BOOLN
SetWordSize_DataFile(int wordSize)
{
	static const WChar *funcName = wxT("SetWordSize_DataFile");

	if (dataFilePtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if ((wordSize < 1) || (wordSize > 4)) {
		NotifyError(wxT("%s: Illegal value - must be an integer 1 - 4 (%d)."),
		  funcName, wordSize);
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
	static const WChar *funcName = wxT("SetNormalisation_DataFile");

	if (dataFilePtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	dataFilePtr->normalisation = normalisation;
	return(TRUE);
	
}

/**************************** SetNumChannels **********************************/

/*
 * This routine sets the number of channels for the binary file support.
 */

BOOLN
SetNumChannels_DataFile(int numChannels)
{
	static const WChar *funcName = wxT("SetNumChannels_DataFile");

	if (dataFilePtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (numChannels < 1) {
		NotifyError(wxT("%s: Value - must be > 1 (%d)."), funcName,
		  numChannels);
		return(FALSE);
	}
	dataFilePtr->numChannels = numChannels;
	dataFilePtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);
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
SetEndian_DataFile(WChar *endian)
{
	static const WChar *funcName = wxT("SetEndian_DataFile");
	int		specifier;

	if (dataFilePtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(endian,
		dataFilePtr->endianModeList)) == DATA_FILE_ENDIAN_NULL) {
		NotifyError(wxT("%s: Illegal name (%s)."), funcName, endian);
		return(FALSE);
	}
	dataFilePtr->endian = specifier;
	return(TRUE);

}

/**************************** Format ******************************************/

/*
 * It sets the dataFilePtr->type field.
 */

FileFormatSpecifier
Format_DataFile(WChar *suffix)
{

	return(dataFilePtr->type = Identify_NameSpecifier(suffix, dataFileSndFormatInfo.
	  formatList));

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

sf_count_t
SetIOSectionLength_DataFile(EarObjectPtr data)
{
	if (!GetDSAMPtr_Common()->segmentedMode || (_OutSig_EarObject(data) ==
	  NULL))
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
OpenFile_DataFile(WChar *fileName, char *mode)
{
	static const WChar *funcName = wxT("OpenFile_DataFile");
	WChar	*parFilePath;
	FILE	*fp, *dummy = stdout;

	switch (*fileName) {
	case STDIN_STDOUT_FILE_DIRN:
		return((mode[0] == 'r')? stdin: stdout);
	case MEMORY_FILE_DIRN: /* Memory pointer */
		return(dummy);
	default:
		Free_SndFile();
		parFilePath = GetParsFileFPath_Common(fileName);
		if ((fp = DSAM_fopen(parFilePath, mode)) == NULL) {
			NotifyError(wxT("%s: Couldn't open '%s' ('%s')."), funcName,
			  fileName, MBSToWCS_Utility_String(mode));
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
	WChar	*c, line[MAXLINE_LARGE];
	int		numcols;

	if (DSAM_fgets(line, MAXLINE_LARGE, fp) == NULL)
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
	/*SetPosition_UPortableIO(fp, 0L, SEEK_SET);*/
	return(numcols);
	
} /* NumberOfColumns_DataFile */

/********************************* SetDefaultSampleRate ***********************/

/*
 * This function sets the module's DefaultSampleRate parameter.
 * It returns TRUE if the operation is successful.
 */
 
BOOLN
SetDefaultSampleRate_DataFile(double theDefaultSampleRate)
{
	static const WChar *funcName = wxT("SetDefaultSampleRate_DataFile");

	if (dataFilePtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
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
	static const WChar *funcName = wxT("SetDuration_DataFile");

	if (dataFilePtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
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
	static const WChar *funcName = wxT("SetTimeOffset_DataFile");

	if (dataFilePtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (theTimeOffset < 0.0) {
		NotifyError(wxT("%s: Illegal value (%g ms)."), MILLI(theTimeOffset));
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
	static const WChar *funcName = wxT("SetGain_DataFile");

	if (dataFilePtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
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
SetFileName_DataFile(WChar *fileName)
{
	static const WChar *funcName = wxT("SetFileName_DataFile");

	if (dataFilePtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (*fileName == (char) NULL) {
		NotifyError(wxT("%s: Illegal file name."), funcName);
		return(FALSE);
	}
	DSAM_strncpy(dataFilePtr->name, fileName, MAX_FILE_PATH);
	Format_DataFile(GetSuffix_Utility_String(fileName));
	Free_SndFile();
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
	static const WChar *funcName = wxT("CheckPars_DataFile");
	BOOLN	ok;
	
	ok = TRUE;
	if (dataFilePtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (dataFilePtr->name[0] == (char) NULL) {
		NotifyError(wxT("%s: File name not set."), funcName);
		ok = FALSE;
	}
	if (dataFilePtr->name[0] == (char) NULL) {
		NotifyError(wxT("%s: File name not set."), funcName);
		ok = FALSE;
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
	static const WChar *funcName = wxT("CheckParsRead_DataFile");
	BOOLN	ok;
	
	ok = TRUE;
	if (dataFilePtr->type == ASCII_DATA_FILE) {
		if ((dataFilePtr->name[0] == STDIN_STDOUT_FILE_DIRN) &&
		  (dataFilePtr->duration <= 0.0)) {
			NotifyError(wxT("%s: Duration must be set, i.e.greater than zero")
			  wxT(",\nfor pipes (%g s)."), funcName, dataFilePtr->duration);
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
SetPars_DataFile(WChar *theFileName, int theWordSize, int theNumChannels,
  double theDefaultSampleRate, double theDuration, double theTimeOffset,
  double theGain)
{
	static const WChar *funcName = wxT("SetPars_DataFile");
	BOOLN	ok;
	
	ok = TRUE;
	if (!SetFileName_DataFile(theFileName))
		ok = FALSE;
	if (!SetWordSize_DataFile(theWordSize))
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
	if (!ok)
		NotifyError(wxT("%s: Failed to set all module parameters."), funcName);
	return(ok);
	
}

/****************************** PrintPars *************************************/

/*
 * This program prints the parameters of the module to the standard output.
 */
 
BOOLN
PrintPars_DataFile(void)
{
	static const WChar *funcName = wxT("PrintPars_DataFile");
	DataFilePtr	p = dataFilePtr;

	if (!CheckPars_DataFile()) {
		NotifyError(wxT("%s: Parameters have not been correctly set."),
		  funcName);
		return(FALSE);
	}
	DPrint(wxT("DataFile Module Parameters:-\n"));
	DPrint(wxT("\tFile name = %s,"), p->name);
	DPrint(wxT("\tSub-format type = %s,\n"), dataFileSndFormatInfo.subFormatList[
	  p->subFormatType].name);
	DPrint(wxT("\tNumber of channels = %d,"), p->numChannels);
	DPrint(wxT("\tGain = %g dB\n"), p->gain);
	DPrint(wxT("\tNormalisation mode: %g (last normalisation: %g),\n"),
	  p->normalisation, p->normalise);
	DPrint(wxT("\tDefault sample rate = %g,\tDuration = "), p->defaultSampleRate);
	if (p->duration < 0.0)
		DPrint(wxT("No limit,\n"));
	else
		DPrint(wxT("%g ms,\n"), MILLI(p->duration));
	DPrint(wxT("\tTime offset = %g ms.\n"), MILLI(p->timeOffset));
	return(TRUE);

}

/****************************** ReadPars **************************************/

/*
 * This program reads a specified number of parameters from a file.
 * The "endian" parameter is no longer used.
 * It returns FALSE if it fails in any way.
 */
 
BOOLN
ReadPars_DataFile(WChar *parFileName)
{
	static const WChar *funcName = wxT("ReadPars_DataFile");
	BOOLN	ok;
	WChar	fileName[MAX_FILE_PATH], *parFilePath, endian[MAXLINE];
	int		wordSize, numChannels;
	double  defaultSampleRate, duration, timeOffset, gain, normalisation;
	FILE	*fp;
	
    if (DSAM_strcmp(parFileName, NO_FILE) == 0)
    	return(TRUE);
	parFilePath = GetParsFileFPath_Common(parFileName);
	if ((fp = DSAM_fopen(parFilePath, "r")) == NULL) {
		NotifyError(wxT("%s: Cannot open data file '%s'.\n"), funcName,
		  parFilePath);
		return(FALSE);
	}
	DPrint(wxT("%s: Reading from '%s':\n"), funcName, parFilePath);
	Init_ParFile();
	ok = TRUE;
	if (!GetPars_ParFile(fp, wxT("%s"), fileName))
		ok = FALSE;
	if (!GetPars_ParFile(fp, wxT("%d"), &wordSize))
		ok = FALSE;
	if (!GetPars_ParFile(fp, wxT("%s"), endian))
		ok = FALSE;
	if (!GetPars_ParFile(fp, wxT("%d"), &numChannels))
		ok = FALSE;
	if (!GetPars_ParFile(fp, wxT("%lf"), &normalisation))
		ok = FALSE;
	if (!GetPars_ParFile(fp, wxT("%lf"), &defaultSampleRate))
		ok = FALSE;
	if (!GetPars_ParFile(fp, wxT("%lf"), &duration))
		ok = FALSE;
	if (!GetPars_ParFile(fp, wxT("%lf"), &timeOffset))
		ok = FALSE;
	if (!GetPars_ParFile(fp, wxT("%lf"), &gain))
		ok = FALSE;
	fclose(fp);
	Free_ParFile();
	if (!ok) {
		NotifyError(wxT("%s:  Not enough, lines or invalid parameters, in ")
		  wxT("module parameter file '%s'."), funcName, parFilePath);
		return(FALSE);
	}
	if (!SetPars_DataFile(fileName, wordSize, numChannels, defaultSampleRate,
	  duration, timeOffset, gain)) {
		NotifyError(wxT("%s: Could not set parameters."), funcName);
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
	static const WChar	*funcName = wxT("SetParsPointer_DataFile");

	if (!theModule) {
		NotifyError(wxT("%s: The module is not set."), funcName);
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
	static const WChar	*funcName = wxT("InitModule_DataFile");

	if (!SetParsPointer_DataFile(theModule)) {
		NotifyError(wxT("%s: Cannot set parameters pointer."), funcName);
		return(FALSE);
	}
	if (!Init_DataFile(GLOBAL)) {
		NotifyError(wxT("%s: Could not initialise process structure."),
		  funcName);
		return(FALSE);
	}
	dataFilePtr->inputMode = (DSAM_strcmp(theModule->name, wxT(
	  "DATAFILE_IN")) == 0);
	if (!dataFilePtr->inputMode) {
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
InitProcessVariables_DataFile(EarObjectPtr data, sf_count_t length,
  double sampleRate)
{
	static WChar *funcName = wxT("InitProcessVariables_DataFile");
	
	if (dataFilePtr->updateProcessVariablesFlag || data->updateProcessFlag) {
		FreeProcessVariables_DataFile();
		if ((dataFilePtr->timeOffsetIndex = (ChanLen) floor(
		  dataFilePtr->timeOffset * sampleRate + 0.5)) >= length) {
			NotifyError(wxT("%s: Time offset (%g ms) must be shorter than ")
			  wxT("signal."), funcName, MILLI(dataFilePtr->timeOffset));
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
				NotifyError(wxT("%s: Duration (%g ms) is longer than signal."),
				  funcName, MILLI(dataFilePtr->duration));
				return(FALSE);
			}
		}
		dataFilePtr->normalise = 1.0;
		dataFilePtr->updateProcessVariablesFlag = FALSE;
	}
	return(TRUE);

}

/**************************** InitBuffer **************************************/

/*
 * This routine initialised the data buffer.  It is used for sound file
 * processing.
 */

BOOLN
InitBuffer_DataFile(SignalDataPtr signal, const WChar *callingFunction)
{
	static const WChar *funcName = wxT("InitBuffer_DataFile");
	DataFilePtr	p = dataFilePtr;

	if (p->buffer)
		return(TRUE);
	if ((p->buffer = (double *) calloc(signal->numChannels * signal->
	  interleaveLevel * DATAFILE_BUFFER_FRAMES, sizeof(double))) == NULL) {
		NotifyError(wxT("%s (%s): Out of memory for dataBuffer."),
		  callingFunction, funcName);
		return(FALSE);
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
	if (dataFilePtr->buffer) {
		free(dataFilePtr->buffer);
		dataFilePtr->buffer = NULL;
	}
	dataFilePtr->updateProcessVariablesFlag = TRUE;

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
ReadSignal_DataFile(WChar *fileName, EarObjectPtr data)
{
	/* static const WChar *funcName = wxT("ReadSignal_DataFile"); */
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
ReadSignalMain_DataFile(WChar *fileName, EarObjectPtr data)
{
	static const WChar *funcName = wxT("ReadSignalMain_DataFile");
	BOOLN	ok;
	SignalDataPtr	outSignal;

	if (dataFilePtr == NULL) {
		if (!Init_DataFile(GLOBAL)) {
			NotifyError(wxT("%s: Could not initialise parameters."), funcName);
			return(FALSE);
		}
	} else if (!CheckPars_DataFile() || !CheckParsRead_DataFile()) {
		NotifyError(wxT("%s: Parameters have not been correctly set."),
		  funcName);
		return(FALSE);
	}
	Format_DataFile(GetSuffix_Utility_String(fileName));
	switch (dataFilePtr->type) {
	case	ASCII_DATA_FILE:
		ok = ReadFile_ASCII(fileName, data);
		break;
	default	:
		ok = ReadFile_SndFile(fileName, data);
		break;
	} /* switch */
	outSignal = _OutSig_EarObject(data);
	if (!ok && !GetDSAMPtr_Common()->segmentedMode)
		NotifyError(wxT("%s: Could not read file '%s'."), funcName, fileName);
	if (ok) {
		if (fabs(dataFilePtr->gain) > DBL_EPSILON)
			GaindB_SignalData(outSignal, dataFilePtr->gain);
		if (!outSignal->staticTimeFlag)
			SetOutputTimeOffset_SignalData(outSignal, dataFilePtr->
			  timeOffsetIndex * outSignal->dt + dataFilePtr->outputTimeOffset);
		outSignal->rampFlag = TRUE;	/* Let user sort out ramps */
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
WriteOutSignal_DataFile(WChar *fileName, EarObjectPtr data)
{
	/* static const WChar *funcName = wxT("WriteOutSignal_DataFile"); */
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
WriteOutSignalMain_DataFile(WChar *fileName, EarObjectPtr data)
{
	static const WChar *funcName = wxT("WriteOutSignalMain_DataFile");
	BOOLN	ok = FALSE;
	
	if (!data || !_OutSig_EarObject(data)) {
		NotifyError(wxT("%s: EarObject not initialised."), funcName);
		return(FALSE);
	}
	if (dataFilePtr == NULL) {
		if (!Init_DataFile(GLOBAL)) {
			NotifyError(wxT("%s: Could not initialise parameters."), funcName);
			return(FALSE);
		}
		dataFilePtr->inputMode = FALSE;
	} else if (!CheckPars_DataFile()) {
		NotifyError(wxT("%s: Parameters have not been correctly set."),
		  funcName);
		return(FALSE);
	}
	switch (Format_DataFile(GetSuffix_Utility_String(fileName))) {
	case	ASCII_DATA_FILE:
		ok = WriteFile_ASCII(fileName, data);
		break;
	default	:
		ok = WriteFile_SndFile(fileName, data);
		break;
	}
	if (!ok)
		NotifyError(wxT("%s: Could not write file '%s'."), funcName, fileName);
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
	if (!data->threadRunFlag) {
		if (InLineProcess_ModuleMgr(data, WriteOutSignal_DataFile_Named))
			_OutSig_EarObject(data) = _InSig_EarObject(data, 0);
		if (data->initThreadRunFlag)
			return(TRUE);
	}
	SetProcessContinuity_EarObject(data);
	return(WriteOutSignalMain_DataFile(dataFilePtr->name, data));

}

