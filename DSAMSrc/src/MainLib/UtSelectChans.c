/**********************
 *
 * File:		UtSelectChans.c
 * Purpose:		This routine copies the input signal to the output signal,
 *				but copies only the specified input channels to the output
 *				signal.
 *				The channels are marked as zero in the array supplied as an
 *				argument.
 * Comments:	Written using ModuleProducer version 1.9 (May 27 1996).
 *				N. B. the array setting is uncontrolled, but this cannot be
 *				helped right now.
 *				There are no PrintPars nor ReadPars routines, as these are not
 *				appropriate for this type of module.
 *				19-03-97 LPO: Added "mode" parameter.
 *				02-03-05 LPO: This module is not being thread enabled as the
 *				number of input channels defines the channel processing.
 * Author:		L. P. O'Mard
 * Created:		11 Jun 1996
 * Updated:		19 Mar 1997
 * Copyright:	(c) 2005, CNBH, University of Essex
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
#include "GeNSpecLists.h"
#include "FiParFile.h"
#include "UtSelectChans.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

SelectChanPtr	selectChanPtr = NULL;

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
Free_Utility_SelectChannels(void)
{
	/* static const char	*funcName = "Free_Utility_SelectChannels";  */

	if (selectChanPtr == NULL)
		return(FALSE);
	if (selectChanPtr->selectionArray) {
		free(selectChanPtr->selectionArray);
		selectChanPtr->selectionArray = NULL;
	}
	if (selectChanPtr->parList)
		FreeList_UniParMgr(&selectChanPtr->parList);
	if (selectChanPtr->parSpec == GLOBAL) {
		free(selectChanPtr);
		selectChanPtr = NULL;
	}
	return(TRUE);

}

/****************************** InitModeList **********************************/

/*
 * This routine intialises the Mode list array.
 */

BOOLN
InitModeList_Utility_SelectChannels(void)
{
	static NameSpecifier	modeList[] = {

					{ "ZERO",	SELECT_CHANS_ZERO_MODE },
					{ "REMOVE",	SELECT_CHANS_REMOVE_MODE },
					{ "EXPAND",	SELECT_CHANS_EXPAND_MODE },
					{ "", 		SELECT_CHANS_NULL }
				};
	selectChanPtr->modeList = modeList;
	return(TRUE);

}

/****************************** InitSelectionModeList *************************/

/*
 * This function initialises the 'selectionMode' list array
 */

BOOLN
InitSelectionModeList_Utility_SelectChannels(void)
{
	static NameSpecifier	modeList[] = {

			{ "ALL",		UTILITY_SELECTCHANNELS_SELECTIONMODE_ALL },
			{ "MIDDLE",		UTILITY_SELECTCHANNELS_SELECTIONMODE_MIDDLE },
			{ "LOWEST",		UTILITY_SELECTCHANNELS_SELECTIONMODE_LOWEST },
			{ "HIGHEST",	UTILITY_SELECTCHANNELS_SELECTIONMODE_HIGHEST },
			{ "USER",		UTILITY_SELECTCHANNELS_SELECTIONMODE_USER },
			{ "",			UTILITY_SELECTCHANNELS_SELECTIONMODE_NULL },
		};
	selectChanPtr->selectionModeList = modeList;
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
Init_Utility_SelectChannels(ParameterSpecifier parSpec)
{
	static const char	*funcName = "Init_Utility_SelectChannels";

	if (parSpec == GLOBAL) {
		if (selectChanPtr != NULL)
			Free_Utility_SelectChannels();
		if ((selectChanPtr = (SelectChanPtr) malloc(sizeof(SelectChan))) ==
		  NULL) {
			NotifyError("%s: Out of memory for 'global' pointer", funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (selectChanPtr == NULL) {
			NotifyError("%s:  'local' pointer not set.", funcName);
			return(FALSE);
		}
	}
	selectChanPtr->parSpec = parSpec;
	selectChanPtr->updateProcessVariablesFlag = TRUE;
	selectChanPtr->modeFlag = TRUE;
	selectChanPtr->selectionModeFlag = TRUE;
	selectChanPtr->numChannelsFlag = TRUE;
	selectChanPtr->mode = SELECT_CHANS_REMOVE_MODE;
	selectChanPtr->selectionMode = UTILITY_SELECTCHANNELS_SELECTIONMODE_ALL;
	selectChanPtr->selectionArray = NULL;

	InitModeList_Utility_SelectChannels();
	InitSelectionModeList_Utility_SelectChannels();
	if (!SetUniParList_Utility_SelectChannels()) {
		NotifyError("%s: Could not initialise parameter list.", funcName);
		Free_Utility_SelectChannels();
		return(FALSE);
	}
	selectChanPtr->numChannels = 0;
	return(TRUE);

}

/****************************** SetUniParList *********************************/

/*
 * This function initialises and sets the module's universal parameter
 * list. This list provides universal access to the module's
 * parameters.  It expects to be called from the 'Init_' routine.
 */

BOOLN
SetUniParList_Utility_SelectChannels(void)
{
	static const char *funcName = "SetUniParList_Utility_SelectChannels";
	UniParPtr	pars;

	if ((selectChanPtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  UTILITY_SELECTCHANNELS_NUM_PARS, NULL)) == NULL) {
		NotifyError("%s: Could not initialise parList.", funcName);
		return(FALSE);
	}
	pars = selectChanPtr->parList->pars;
	SetPar_UniParMgr(&pars[UTILITY_SELECTCHANNELS_MODE], "MODE",
	  "Selection mode - 'zero', 'remove' or 'expand'.",
	  UNIPAR_NAME_SPEC,
	  &selectChanPtr->mode, selectChanPtr->modeList,
	  (void * (*)) SetMode_Utility_SelectChannels);
	SetPar_UniParMgr(&pars[UTILITY_SELECTCHANNELS_SELECTIONMODE],
	  "SELECTION_MODE",
	  "Channel selection mode ('all', 'middle', 'top', 'botton' or 'user'.",
	  UNIPAR_NAME_SPEC,
	  &selectChanPtr->selectionMode, selectChanPtr->selectionModeList,
	  (void * (*)) SetSelectionMode_Utility_SelectChannels);
	SetPar_UniParMgr(&pars[UTILITY_SELECTCHANNELS_NUMCHANNELS], "NUM_CHANNELS",
	  "No. of channels in selection field (This is no longer).",
	  UNIPAR_INT_AL,
	  &selectChanPtr->numChannels, NULL,
	  (void * (*)) SetNumChannels_Utility_SelectChannels);
	SetPar_UniParMgr(&pars[UTILITY_SELECTCHANNELS_SELECTIONARRAY], "ARRAY",
	  "Selection array 0 = off, 1 = on.",
	  UNIPAR_REAL_DYN_ARRAY,
	  &selectChanPtr->selectionArray, &selectChanPtr->numChannels,
	  (void * (*)) SetIndividualSelection_Utility_SelectChannels);

	SetEnabledState_Utility_SelectChannels();
	return(TRUE);

}

/****************************** SetEnabledState *******************************/

/*
 * This routine sets the enabled states.
 */

void
SetEnabledState_Utility_SelectChannels(void)
{
	selectChanPtr->parList->pars[UTILITY_SELECTCHANNELS_NUMCHANNELS].enabled =
	  FALSE;
	selectChanPtr->parList->pars[UTILITY_SELECTCHANNELS_SELECTIONARRAY].
	  enabled = (selectChanPtr->selectionMode ==
	  UTILITY_SELECTCHANNELS_SELECTIONMODE_USER);

}

/****************************** GetUniParListPtr ******************************/

/*
 * This function returns a pointer to the module's universal parameter
 * list.
 */

UniParListPtr
GetUniParListPtr_Utility_SelectChannels(void)
{
	static const char	*funcName = "GetUniParListPtr_Utility_SelectChannels";

	if (selectChanPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (selectChanPtr->parList == NULL) {
		NotifyError("%s: UniParList data structure has not been initialised. "
		  "NULL returned.", funcName);
		return(NULL);
	}
	return(selectChanPtr->parList);

}

/****************************** AllocNumChannels ******************************/

/*
 * This function allocates the memory for the pure tone arrays.
 * It will assume that nothing needs to be done if the 'numChannels' 
 * variable is the same as the current structure member value.
 * To make this work, the function needs to set the structure 'numChannels'
 * parameter too.
 * It returns FALSE if it fails in any way.
 */

BOOLN
AllocNumChannels_Utility_SelectChannels(int numChannels)
{
	static const char	*funcName = "AllocNumChannels_Utility_SelectChannels";

	if (numChannels == selectChanPtr->numChannels)
		return(TRUE);
	if (selectChanPtr->selectionArray)
		free(selectChanPtr->selectionArray);
	if ((selectChanPtr->selectionArray = (double *) calloc(numChannels,
	  sizeof(double))) == NULL) {
		NotifyError("%s: Cannot allocate memory for '%d' selectionArray.",
		  funcName, numChannels);
		return(FALSE);
	}
	selectChanPtr->numChannels = numChannels;
	selectChanPtr->numChannelsFlag = TRUE;
	return(TRUE);

}

/****************************** SetPars ***************************************/

/*
 * This function sets all the module's parameters.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetPars_Utility_SelectChannels(char *mode, int numChannels,
  double *selectionArray)
{
	static const char	*funcName = "SetPars_Utility_SelectChannels";
	BOOLN	ok;

	ok = TRUE;
	if (!SetMode_Utility_SelectChannels(mode))
		ok = FALSE;
	if (!SetNumChannels_Utility_SelectChannels(numChannels))
		ok = FALSE;
	if (!SetSelectionArray_Utility_SelectChannels(selectionArray))
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
SetMode_Utility_SelectChannels(char *theMode)
{
	static const char	*funcName = "SetMode_Utility_SelectChannels";
	int		specifier;

	if (selectChanPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theMode,
	  selectChanPtr->modeList)) == SELECT_CHANS_NULL) {
		NotifyError("%s: Illegal mode name (%s).", funcName, theMode);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	selectChanPtr->modeFlag = TRUE;
	selectChanPtr->mode = specifier;
	return(TRUE);

}

/****************************** SetSelectionMode ******************************/

/*
 * This function sets the module's selectionMode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetSelectionMode_Utility_SelectChannels(char * theSelectionMode)
{
	static const char	*funcName = "SetSelectionMode_Utility_SelectChannels";
	int		specifier;

	if (selectChanPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theSelectionMode,
	  selectChanPtr->selectionModeList)) ==
	  UTILITY_SELECTCHANNELS_SELECTIONMODE_NULL) {
		NotifyError("%s: Illegal name (%s).", funcName, theSelectionMode);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	selectChanPtr->selectionModeFlag = TRUE;
	selectChanPtr->updateProcessVariablesFlag = TRUE;
	selectChanPtr->selectionMode = specifier;
	SetEnabledState_Utility_SelectChannels();
	return(TRUE);

}

/****************************** SetNumChannels ********************************/

/*
 * This function sets the module's numChannels parameter.
 * It returns TRUE if the operation is successful.
 * The 'numChannels' variable is set by the
 * 'AllocNumChannels_Utility_SelectChannels' routine.
 * Additional checks should be added as required.
 */

BOOLN
SetNumChannels_Utility_SelectChannels(int theNumChannels)
{
	static const char	*funcName = "SetNumChannels_Utility_SelectChannels";

    if (selectChanPtr == NULL) {
        NotifyError("%s: Module not initialised.", funcName);
        return(FALSE);
    }
    if (theNumChannels < 1) {
        NotifyError("%s: Illegal number of selection channels.\n", funcName,
          theNumChannels);
        return(FALSE);
    }
    if (!AllocNumChannels_Utility_SelectChannels(theNumChannels)) {
        NotifyError("%s: Cannot allocate memory for the 'numChannels' arrays.",
         funcName);
        return(FALSE);
    }
    /*** Put any other required checks here. ***/
    selectChanPtr->numChannelsFlag = TRUE;
    selectChanPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/****************************** SetSelectionArray *****************************/

/*
 * This function sets the module's selectionArray array.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetSelectionArray_Utility_SelectChannels(double *theSelectionArray)
{
	static const char	*funcName = "SetSelectionArray_Utility_SelectChannels";

	if (selectChanPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	selectChanPtr->selectionArray = theSelectionArray;
	return(TRUE);

}

/****************************** SetIndividualSelection ************************/

/*
 * This function sets the module's selectionArray array element.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetIndividualSelection_Utility_SelectChannels(int theIndex, double theSelection)
{
	static const char *funcName =
	  "SetIndividualSelectionArray_Utility_SelectChannels";

	if (selectChanPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (selectChanPtr->selectionMode !=
	  UTILITY_SELECTCHANNELS_SELECTIONMODE_USER) {
		NotifyError("%s: Channels can only be selected in 'user' selection "
		  "mode.", funcName);
		return(FALSE);
	}
	if (theSelection < 0.0) {
		NotifyError("%s: The selection values must be greater than zero ('%g'",
		  funcName, theSelection);
		return(FALSE);
	}
	if ((theIndex > selectChanPtr->numChannels - 1) &&
	  !ResizeDoubleArray_UniParMgr(&selectChanPtr->selectionArray,
	    &selectChanPtr->numChannels, theIndex + 1)) {
		NotifyError("%s: Could not resize the selection array.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	selectChanPtr->selectionArray[theIndex] = theSelection;
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
CheckPars_Utility_SelectChannels(void)
{
	static const char	*funcName = "CheckPars_Utility_SelectChannels";
	BOOLN	ok;

	ok = TRUE;
	if (selectChanPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (!selectChanPtr->selectionModeFlag) {
		NotifyError("%s: selectionMode variable not set.", funcName);
		ok = FALSE;
	}
	if ((selectChanPtr->selectionMode == 
	  UTILITY_SELECTCHANNELS_SELECTIONMODE_USER) &&
	  selectChanPtr->selectionArray == NULL) {
		NotifyError("%s: selectionArray array not set.", funcName);
		ok = FALSE;
	}
	if (!selectChanPtr->modeFlag) {
		NotifyError("%s: mode variable not set.", funcName);
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
PrintPars_Utility_SelectChannels(void)
{
	static const char	*funcName = "PrintPars_Utility_SelectChannels";
	int		i;

	if (!CheckPars_Utility_SelectChannels()) {
		NotifyError("%s: Parameters have not been correctly set.", funcName);
		return(FALSE);
	}
	DPrint("Channel Selection Utility Module Parameters:-\n");
	DPrint("\t%10s\n", "Selection Array");
	DPrint("\t%10s\n", "(state)");
	for (i = 0; i < selectChanPtr->numChannels; i++)
		if (selectChanPtr->selectionArray[i] > 0.0)
			DPrint("\t    on * %g\n", selectChanPtr->selectionArray[i]);
		else
			DPrint("\t    off\n");
	DPrint("\tMode = %s,", selectChanPtr->modeList[selectChanPtr->mode].name);
	DPrint("\tChannel selection mode = %s.\n", selectChanPtr->selectionModeList[
	  selectChanPtr->selectionMode].name);
	return(TRUE);

}

/****************************** ReadPars **************************************/

/*
 * This program reads a specified number of parameters from a file.
 * It returns FALSE if it fails in any way.n */

BOOLN
ReadPars_Utility_SelectChannels(char *fileName)
{
	static const char	*funcName = "ReadPars_Utility_SelectChannels";
	BOOLN	ok;
	char	*filePath, mode[MAXLINE];
	int		i, numChannels;
	FILE	*fp;

	filePath = GetParsFileFPath_Common(fileName);
	if ((fp = fopen(filePath, "r")) == NULL) {
		NotifyError("%s: Cannot open data file '%s'.\n", funcName, fileName);
		return(FALSE);
	}
	DPrint("%s: Reading from '%s':\n", funcName, fileName);
	Init_ParFile();
	ok = TRUE;
	if (!GetPars_ParFile(fp, "%s", mode))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%d", &numChannels))
		ok = FALSE;
	if (!AllocNumChannels_Utility_SelectChannels(numChannels)) {
		NotifyError("%s: Cannot allocate memory for the 'numChannels' arrays.",
		  funcName);
		return(FALSE);
	}
	for (i = 0; i < selectChanPtr->numChannels; i++)
		if (!GetPars_ParFile(fp, "%lf", &selectChanPtr->selectionArray[i]))
			ok = FALSE;
	fclose(fp);
	Free_ParFile();
	if (!ok) {
		NotifyError("%s: Not enough lines, or invalid parameters, in module "
		  "parameter file '%s'.", funcName, fileName);
		return(FALSE);
	}
	selectChanPtr->selectionMode = UTILITY_SELECTCHANNELS_SELECTIONMODE_USER;
	if (!SetPars_Utility_SelectChannels(mode, numChannels,
	  selectChanPtr->selectionArray)) {
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
SetParsPointer_Utility_SelectChannels(ModulePtr theModule)
{
	static const char	*funcName = "SetParsPointer_Utility_SelectChannels";

	if (!theModule) {
		NotifyError("%s: The module is not set.", funcName);
		return(FALSE);
	}
	selectChanPtr = (SelectChanPtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 */

BOOLN
InitModule_Utility_SelectChannels(ModulePtr theModule)
{
	static const char	*funcName = "InitModule_Utility_SelectChannels";

	if (!SetParsPointer_Utility_SelectChannels(theModule)) {
		NotifyError("%s: Cannot set parameters pointer.", funcName);
		return(FALSE);
	}
	if (!Init_Utility_SelectChannels(GLOBAL)) {
		NotifyError("%s: Could not initialise process structure.", funcName);
		return(FALSE);
	}
	theModule->parsPtr = selectChanPtr;
	theModule->CheckPars = CheckPars_Utility_SelectChannels;
	theModule->Free = Free_Utility_SelectChannels;
	theModule->GetUniParListPtr = GetUniParListPtr_Utility_SelectChannels;
	theModule->PrintPars = PrintPars_Utility_SelectChannels;
	theModule->ReadPars = ReadPars_Utility_SelectChannels;
	theModule->RunProcess = Process_Utility_SelectChannels;
	theModule->SetParsPointer = SetParsPointer_Utility_SelectChannels;
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
CheckData_Utility_SelectChannels(EarObjectPtr data)
{
	static const char	*funcName = "CheckData_Utility_SelectChannels";
	int		numInChans;

	if (data == NULL) {
		NotifyError("%s: EarObject not initialised.", funcName);
		return(FALSE);
	}
	if (!CheckInSignal_EarObject(data, funcName))
		return(FALSE);
	numInChans = data->inSignal[0]->numChannels / data->inSignal[0]->
	  interleaveLevel;
	if (selectChanPtr->numChannels > numInChans) {
		NotifyError("%s: The specified channel field (%d) is greater than\n"
		  "as the input signal channels (%d, interleave level %d).", funcName,
		  selectChanPtr->numChannels, data->inSignal[0]->numChannels,
		  data->inSignal[0]->interleaveLevel);
			return(FALSE);
	}
	return(TRUE);

}

/****************************** InitProcessVariables **************************/

/*
 * This function allocates the memory for the process variables.
 * It assumes that all of the parameters for the module have been
 * correctly initialised.
 */

BOOLN
InitProcessVariables_Utility_SelectChannels(EarObjectPtr data)
{
	static const char *funcName = "InitProcessVariables_Utility_SelectChannels";
	int		i, numInChans;
	SelectChanPtr	p = selectChanPtr;

	if (p->updateProcessVariablesFlag || data->updateProcessFlag) {
		/*** Additional update flags can be added to above line ***/
		numInChans = data->inSignal[0]->numChannels / data->inSignal[0]->
		  interleaveLevel;
		if ((p->numChannels < numInChans) && !ResizeDoubleArray_UniParMgr(
		  &p->selectionArray, &p->numChannels, numInChans)) {
			NotifyError("%s: Could not set selection array.", funcName);
			return(FALSE);
		}
		switch (p->selectionMode) {
		case UTILITY_SELECTCHANNELS_SELECTIONMODE_ALL:
			for (i = 0; i < p->numChannels; i++)
				p->selectionArray[i] = 1.0;
			break;
		case UTILITY_SELECTCHANNELS_SELECTIONMODE_MIDDLE:
			for (i = 0; i < p->numChannels; i++)
				p->selectionArray[i] = 0.0;
			p->selectionArray[p->numChannels / 2] = 1.0;
			break;
		case UTILITY_SELECTCHANNELS_SELECTIONMODE_LOWEST:
			for (i = 0; i < p->numChannels; i++)
				p->selectionArray[i] = 0.0;
			p->selectionArray[0] = 1.0;
			break;
		case UTILITY_SELECTCHANNELS_SELECTIONMODE_HIGHEST:
			for (i = 0; i < p->numChannels; i++)
				p->selectionArray[i] = 0.0;
			p->selectionArray[p->numChannels - 1] = 1.0;
			break;
		default:
			;
		}
		p->updateProcessVariablesFlag = FALSE;
	}
	return(TRUE);

}

/****************************** Process ***************************************/

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
Process_Utility_SelectChannels(EarObjectPtr data)
{
	static const char	*funcName = "Process_Utility_SelectChannels";
	register	ChanData	 *inPtr, *outPtr, multiplier;
	uShort	numChannels = 0;
	int		i, k, l, chan, inChanIndex;
	ChanLen	j;
	SelectChanPtr	p = selectChanPtr;

	if (!CheckPars_Utility_SelectChannels())
		return(FALSE);
	if (!CheckData_Utility_SelectChannels(data)) {
		NotifyError("%s: Process data invalid.", funcName);
		return(FALSE);
	}
	SetProcessName_EarObject(data, "Select Channels Utility Module process");
	if (!InitProcessVariables_Utility_SelectChannels(data)) {
		NotifyError("%s: Could not initialise the process variables.",
		  funcName);
		return(FALSE);
	}
	switch (p->mode) {
	case SELECT_CHANS_ZERO_MODE:
		numChannels = data->inSignal[0]->numChannels;
		break;
	case SELECT_CHANS_REMOVE_MODE:
		for (i = 0, numChannels = 0; i < p->numChannels; i++)
			if (p->selectionArray[i] > 0.0)
				numChannels++;
			numChannels *= data->inSignal[0]->interleaveLevel;
			break;
	case SELECT_CHANS_EXPAND_MODE:
		for (i = 0, numChannels = 0; i < p->numChannels; i++)
			if (p->selectionArray[i] > 0.0)
				numChannels += (uShort) p->selectionArray[i];
			numChannels *= data->inSignal[0]->interleaveLevel;
		break;
	} /* switch */
	if (!InitOutSignal_EarObject(data, numChannels, data->inSignal[0]->length,
	  data->inSignal[0]->dt)) {
		NotifyError("%s: Cannot initialise output channels.", funcName);
		return(FALSE);
	}
	SetInterleaveLevel_SignalData(data->outSignal, data->inSignal[0]->
	  interleaveLevel);
	SetLocalInfoFlag_SignalData(data->outSignal, TRUE);
	switch (p->mode) {
	case SELECT_CHANS_ZERO_MODE:
		for (i = 0; i < data->inSignal[0]->numChannels; i++) {
			outPtr = data->outSignal->channel[i];
			if (p->selectionArray[i] > 0.0) {
			inChanIndex = i / data->inSignal[0]->interleaveLevel;
				inPtr = data->inSignal[0]->channel[i];
				multiplier = p->selectionArray[inChanIndex];
				for (j = 0; j < data->outSignal->length; j++)
					*outPtr++ = *inPtr++ * multiplier;
			} else
				for (j = 0; j < data->outSignal->length; j++)
					*outPtr++ = 0.0;
		}
		break;
	case SELECT_CHANS_REMOVE_MODE:
		for (i = 0, chan = 0; i < data->inSignal[0]->numChannels; i++) {
			inChanIndex = i / data->inSignal[0]->interleaveLevel;
			if (p->selectionArray[inChanIndex] > 0.0) {
				SetInfoChannelLabel_SignalData(data->outSignal, chan, data->
				  inSignal[0]->info.chanLabel[i]);
				SetInfoCF_SignalData(data->outSignal, chan, data->inSignal[0]->
				  info.cFArray[i]);
				inPtr = data->inSignal[0]->channel[i];
				outPtr = data->outSignal->channel[chan];
				multiplier = p->selectionArray[inChanIndex];
				for (j = 0; j < data->outSignal->length; j++)
					*outPtr++ = *inPtr++ * multiplier;
				chan++;
			}
		}
		break;
	case SELECT_CHANS_EXPAND_MODE:
		for (i = 0, chan = 0; i < data->inSignal[0]->numChannels; i += data->
		  inSignal[0]->interleaveLevel) {
			inChanIndex = i / data->inSignal[0]->interleaveLevel;
			if (p->selectionArray[inChanIndex] > 0.0)
				for (k = 0; k < p->selectionArray[inChanIndex]; k++)
					for (l = 0; l < data->outSignal->interleaveLevel; l++) {
						SetInfoChannelLabel_SignalData(data->outSignal, chan,
						  data->inSignal[0]->info.chanLabel[i + l]);
						SetInfoCF_SignalData(data->outSignal, chan, data->
						  inSignal[0]->info.cFArray[i + l]);
						inPtr = data->inSignal[0]->channel[i + l];
						outPtr = data->outSignal->channel[chan];
						for (j = 0; j < data->outSignal->length; j++)
							*outPtr++ = *inPtr++;
						chan++;
					}
		}
		break;
	} /* switch */
	SetUtilityProcessContinuity_EarObject(data);
	SetProcessContinuity_EarObject(data);
	return(TRUE);

}

