/**********************
 *
 * File:		AnFindNextIndex.c
 * Purpose:		This module finds the next minimum after a specified offset in
 *				each channel of a signal, and returns the respective index for
 *				each channel.
 * Comments:	Written using ModuleProducer version 1.9 (May 27 1996).
 *				The agorithm looks for the first minimum after a negative
 *				gradient. A negative gradient is where cPtr(i + 1) - cPtr(i)
 *				< 0.
 * Author:		L. P. O'Mard
 * Created:		6 Jun 1996
 * Updated:	
 * Copyright:	(c) 1998, University Of Essex.
 *
 *********************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG_H */

#include "GeCommon.h"
#include "GeSignalData.h"
#include "GeEarObject.h"
#include "GeUniParMgr.h"
#include "GeModuleMgr.h"
#include "FiParFile.h"
#include "AnFindNextIndex.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

FindIndexPtr	findIndexPtr = NULL;

/******************************************************************************/
/****************************** Subroutines and functions *********************/
/******************************************************************************/

/****************************** Free ******************************************/

/*
 * This function releases of the memory allocated for the process
 * variables. It should be called when the module is no longer in use.
 * It is defined as returning a BOOLN value because the generic
 * module interface requires that a non-void value be returned.
 */

BOOLN
Free_Analysis_FindNextIndex(void)
{
	/* static const char	*funcName = "Free_Analysis_FindNextIndex";  */

	if (findIndexPtr == NULL)
		return(FALSE);
	if (findIndexPtr->parList)
		FreeList_UniParMgr(&findIndexPtr->parList);
	if (findIndexPtr->parSpec == GLOBAL) {
		free(findIndexPtr);
		findIndexPtr = NULL;
	}
	return(TRUE);

}

/****************************** InitModeList **********************************/

/*
 * This routine intialises the Mode list array.
 */

BOOLN
InitModeList_Analysis_FindNextIndex(void)
{
	static NameSpecifier	modeList[] = {

					{ "MINIMUM", FIND_INDEX_MINIMUM},
					{ "MAXIMUM", FIND_INDEX_MAXIMUM},
					{ "", FIND_INDEX_NULL }
				
				};
	findIndexPtr->modeList = modeList;
	return(TRUE);

}

/****************************** Init ******************************************/

/*
 * This function initialises the module by setting module's parameter
 * pointer structure.
 * The GLOBAL option is for hard programming - it sets the module's
 * pointer to the global parameter structure and initialises the
 * parameters. The LOCAL option is for generic programming - it
 * initialises the parameter structure currently pointed to by the
 * module's parameter pointer.
 */

BOOLN
Init_Analysis_FindNextIndex(ParameterSpecifier parSpec)
{
	static const char	*funcName = "Init_Analysis_FindNextIndex";

	if (parSpec == GLOBAL) {
		if (findIndexPtr != NULL)
			Free_Analysis_FindNextIndex();
		if ((findIndexPtr = (FindIndexPtr) malloc(sizeof(FindIndex))) == NULL) {
			NotifyError("%s: Out of memory for 'global' pointer", funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (findIndexPtr == NULL) {
			NotifyError("%s:  'local' pointer not set.", funcName);
			return(FALSE);
		}
	}
	findIndexPtr->parSpec = parSpec;
	findIndexPtr->modeFlag = FALSE;
	findIndexPtr->timeOffsetFlag = FALSE;
	findIndexPtr->mode = 0;
	findIndexPtr->timeOffset = 0.0;

	InitModeList_Analysis_FindNextIndex();
	if (!SetUniParList_Analysis_FindNextIndex()) {
		NotifyError("%s: Could not initialise parameter list.", funcName);
		Free_Analysis_FindNextIndex();
		return(FALSE);
	}
	return(TRUE);

}

/****************************** SetUniParList *********************************/

/*
 * This function initialises and sets the module's universal parameter
 * list. This list provides universal access to the module's
 * parameters.  It expects to be called from the 'Init_' routine.
 */

BOOLN
SetUniParList_Analysis_FindNextIndex(void)
{
	static const char *funcName = "SetUniParList_Analysis_FindNextIndex";
	UniParPtr	pars;

	if ((findIndexPtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  ANALYSIS_FINDNEXTINDEX_NUM_PARS, NULL)) == NULL) {
		NotifyError("%s: Could not initialise parList.", funcName);
		return(FALSE);
	}
	pars = findIndexPtr->parList->pars;
	SetPar_UniParMgr(&pars[ANALYSIS_FINDNEXTINDEX_MODE], "MODE",
	  "Index search mode ('minimum' or 'maximum').",
	  UNIPAR_NAME_SPEC,
	  &findIndexPtr->mode, findIndexPtr->modeList,
	  (void * (*)) SetMode_Analysis_FindNextIndex);
	SetPar_UniParMgr(&pars[ANALYSIS_FINDNEXTINDEX_TIMEOFFSET], "OFFSET",
	  "Time offset for start of search (s).",
	  UNIPAR_REAL,
	  &findIndexPtr->timeOffset, NULL,
	  (void * (*)) SetTimeOffset_Analysis_FindNextIndex);
	return(TRUE);

}

/****************************** GetUniParListPtr ******************************/

/*
 * This function returns a pointer to the module's universal parameter
 * list.
 */

UniParListPtr
GetUniParListPtr_Analysis_FindNextIndex(void)
{
	static const char	*funcName = "GetUniParListPtr_Analysis_FindNextIndex";

	if (findIndexPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (findIndexPtr->parList == NULL) {
		NotifyError("%s: UniParList data structure has not been initialised. "
		  "NULL returned.", funcName);
		return(NULL);
	}
	return(findIndexPtr->parList);

}

/****************************** SetPars ***************************************/

/*
 * This function sets all the module's parameters.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetPars_Analysis_FindNextIndex(char *mode, double timeOffset)
{
	static const char	*funcName = "SetPars_Analysis_FindNextIndex";
	BOOLN	ok;

	ok = TRUE;
	if (!SetMode_Analysis_FindNextIndex(mode))
		ok = FALSE;
	if (!SetTimeOffset_Analysis_FindNextIndex(timeOffset))
		ok = FALSE;
	if (!ok)
		NotifyError("%s: Failed to set all module parameters." ,funcName);
	return(ok);

}

/****************************** SetMode ***************************************/

/*
 * This function sets the module's mode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetMode_Analysis_FindNextIndex(char *theMode)
{
	static const char	*funcName = "SetMode_Analysis_FindNextIndex";
	int		specifier;

	if (findIndexPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theMode, findIndexPtr->modeList)) ==
	  FIND_INDEX_NULL) {
		NotifyError("%s: Illegal mode name (%s).", funcName, theMode);
		return(FALSE);
	}
	findIndexPtr->modeFlag = TRUE;
	findIndexPtr->mode = specifier;
	return(TRUE);

}

/****************************** SetTimeOffset *********************************/

/*
 * This function sets the module's timeOffset parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetTimeOffset_Analysis_FindNextIndex(double theTimeOffset)
{
	static const char	*funcName =
	  "SetTimeOffset_Analysis_FindNextIndex";

	if (findIndexPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	findIndexPtr->timeOffsetFlag = TRUE;
	findIndexPtr->timeOffset = theTimeOffset;
	return(TRUE);

}

/****************************** CheckPars *************************************/

/*
 * This routine checks that the necessary parameters for the module
 * have been correctly initialised.
 * Other 'operational' tests which can only be done when all
 * parameters are present, should also be carried out here.
 * It returns TRUE if there are no problems.
 */

BOOLN
CheckPars_Analysis_FindNextIndex(void)
{
	static const char	*funcName = "CheckPars_Analysis_FindNextIndex";
	BOOLN	ok;

	ok = TRUE;
	if (findIndexPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (!findIndexPtr->modeFlag) {
		NotifyError("%s: mode variable not set.", funcName);
		ok = FALSE;
	}
	if (!findIndexPtr->timeOffsetFlag) {
		NotifyError("%s: timeOffset variable not set.", funcName);
		ok = FALSE;
	}
	return(ok);

}

/****************************** PrintPars *************************************/

/*
 * This routine prints all the module's parameters to the file stream.
 * specified by the dSAM.parsFile file pointer.
 */

BOOLN
PrintPars_Analysis_FindNextIndex(void)
{
	static const char	*funcName = "PrintPars_Analysis_FindNextIndex";

	if (!CheckPars_Analysis_FindNextIndex()) {
		NotifyError("%s: Parameters have not been correctly set.", funcName);
		return(FALSE);
	}
	DPrint("Find Indexes Analysis Module "\
	  "Parameters:-\n");
	DPrint("\tSearch mode = %s,",
	  findIndexPtr->modeList[findIndexPtr->mode].name);
	DPrint("\tTime offset = %g ms\n",
	  MSEC(findIndexPtr->timeOffset));
	return(TRUE);

}

/****************************** ReadPars **************************************/

/*
 * This program reads a specified number of parameters from a file.
 * It returns FALSE if it fails in any way.n */

BOOLN
ReadPars_Analysis_FindNextIndex(char *fileName)
{
	static const char	*funcName = "ReadPars_Analysis_FindNextIndex";
	BOOLN	ok;
	char	*filePath;
	char	mode[MAXLINE];
	double	timeOffset;
	FILE	*fp;

	filePath = GetParsFileFPath_Common(fileName);
	if ((fp = fopen(filePath, "r")) == NULL) {
		NotifyError("%s: Cannot open data file '%s'.\n", funcName, filePath);
		return(FALSE);
	}
	DPrint("%s: Reading from '%s':\n", funcName, filePath);
	Init_ParFile();
	ok = TRUE;
	if (!GetPars_ParFile(fp, "%s", mode))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &timeOffset))
		ok = FALSE;
	fclose(fp);
	Free_ParFile();
	if (!ok) {
		NotifyError("%s: Not enough lines, or invalid parameters, in module "
		  "parameter file '%s'.", funcName, filePath);
		return(FALSE);
	}
	if (!SetPars_Analysis_FindNextIndex(mode, timeOffset)) {
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
SetParsPointer_Analysis_FindNextIndex(ModulePtr theModule)
{
	static const char	*funcName = "SetParsPointer_Analysis_FindNextIndex";

	if (!theModule) {
		NotifyError("%s: The module is not set.", funcName);
		return(FALSE);
	}
	findIndexPtr = (FindIndexPtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 */

BOOLN
InitModule_Analysis_FindNextIndex(ModulePtr theModule)
{
	static const char	*funcName = "InitModule_Analysis_FindNextIndex";

	if (!SetParsPointer_Analysis_FindNextIndex(theModule)) {
		NotifyError("%s: Cannot set parameters pointer.", funcName);
		return(FALSE);
	}
	if (!Init_Analysis_FindNextIndex(GLOBAL)) {
		NotifyError("%s: Could not initialise process structure.", funcName);
		return(FALSE);
	}
	theModule->parsPtr = findIndexPtr;
	theModule->CheckPars = CheckPars_Analysis_FindNextIndex;
	theModule->Free = Free_Analysis_FindNextIndex;
	theModule->GetUniParListPtr = GetUniParListPtr_Analysis_FindNextIndex;
	theModule->PrintPars = PrintPars_Analysis_FindNextIndex;
	theModule->ReadPars = ReadPars_Analysis_FindNextIndex;
	theModule->RunProcess = Calc_Analysis_FindNextIndex;
	theModule->SetParsPointer = SetParsPointer_Analysis_FindNextIndex;
	return(TRUE);

}

/****************************** CheckData *************************************/

/*
 * This routine checks that the 'data' EarObject and input signal are
 * correctly initialised.
 * It should also include checks that ensure that the module's
 * parameters are compatible with the signal parameters, i.e. dt is
 * not too small, etc...
 * The 'CheckRamp_SignalData()' can be used instead of the
 * 'CheckInit_SignalData()' routine if the signal must be ramped for
 * the process.
 */

BOOLN
CheckData_Analysis_FindNextIndex(EarObjectPtr data)
{
	static const char	*funcName = "CheckData_Analysis_FindNextIndex";
	double	signalDuration;

	if (data == NULL) {
		NotifyError("%s: EarObject not initialised.", funcName);
		return(FALSE);
	}
	if (!CheckInit_SignalData(data->inSignal[0], funcName))
		return(FALSE);
	signalDuration = _GetDuration_SignalData(data->inSignal[0]);
	if (findIndexPtr->timeOffset > signalDuration) {
		NotifyError("%s: Offset value (%g ms)is longer than the signal "
		  "duration (%g ms).", funcName, MSEC(findIndexPtr->timeOffset),
		  MSEC(signalDuration));
		return(FALSE);
	}
	/*** Put additional checks here. ***/
	return(TRUE);

}

/****************************** Calc ******************************************/

/*
 * This routine allocates memory for the output signal, if necessary,
 * and generates the signal into channel[0] of the signal data-set.
 * It checks that all initialisation has been correctly carried out by
 * calling the appropriate checking routines.
 * It can be called repeatedly with different parameter values if
 * required.
 * Stimulus generation only sets the output signal, the input signal
 * is not used.
 * With repeated calls the Signal memory is only allocated once, then
 * re-used.
 */

BOOLN
Calc_Analysis_FindNextIndex(EarObjectPtr data)
{
	static const char	*funcName = "Calc_Analysis_FindNextIndex";
	register	ChanData	 *inPtr, lastValue;
	BOOLN	found, gradient, findMinimum;
	int		chan;
	ChanLen	i, offsetIndex, widthIndex, index;

	if (!CheckPars_Analysis_FindNextIndex())
		return(FALSE);
	if (!CheckData_Analysis_FindNextIndex(data)) {
		NotifyError("%s: Process data invalid.", funcName);
		return(FALSE);
	}
	SetProcessName_EarObject(data, "Find Value Analysis");
	if (!InitOutSignal_EarObject(data, data->inSignal[0]->numChannels, 1,
	  1.0)) {
		NotifyError("%s: Cannot initialise output channels.", funcName);
		return(FALSE);
	}
	offsetIndex = (ChanLen) (findIndexPtr->timeOffset /
	  data->inSignal[0]->dt + 0.5);
	findMinimum = (findIndexPtr->mode == FIND_INDEX_MINIMUM);
	for (chan = 0; chan < data->outSignal->numChannels; chan++) {
		inPtr = data->inSignal[0]->channel[chan] + offsetIndex;
		gradient = FALSE;
		for (i = offsetIndex + 1, lastValue = *inPtr++, found = FALSE,
		  widthIndex = 0; (i < data->inSignal[0]->length - 1) && !found; i++,
		    inPtr++) {
			if (!gradient)
				gradient = (findMinimum)? (*inPtr < lastValue): (*inPtr >
				  lastValue);
			if (gradient) {
				if ((findMinimum)? (*inPtr < *(inPtr + 1)): (*inPtr > *(inPtr +
				  1)))  {
				  	index = i - widthIndex / 2;
					data->outSignal->channel[chan][0] = (ChanData) index;
					found = TRUE;
					break;
				} else if (*inPtr == *(inPtr + 1)) /* check for flats troughs.*/
					widthIndex++;
			}
			lastValue = *inPtr;
		}
		if (!found) {
			NotifyWarning("%s: %s not found. Returning end of channel "\
			  "index = %u.", funcName, (findMinimum)? "Minimum": "Maximum",
			  data->inSignal[0]->length - 1);
			data->outSignal->channel[chan][0] = (ChanData) (data->inSignal[
			  0]->length - 1);
		}
	}

	SetProcessContinuity_EarObject(data);
	return(TRUE);

}
