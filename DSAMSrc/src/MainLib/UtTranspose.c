/**********************
 *
 * File:		UtTranspose.c
 * Purpose:		This module takes an input signal and converts the n channels
 *				of m samples to m channels of n samples.
 * Comments:	Written using ModuleProducer version 1.4.2 (Dec 19 2003).
 * Author:		L. P. O'Mard
 * Created:		13 Jul 2004
 * Updated:	
 * Copyright:	(c) 2004, CNBH, University of Essex
 *
 *********************/

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>

#include "GeCommon.h"
#include "GeSignalData.h"
#include "GeEarObject.h"
#include "GeUniParMgr.h"
#include "GeModuleMgr.h"
#include "FiParFile.h"
#include "UtTranspose.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

TransposePtr	transposePtr = NULL;

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
Free_Utility_Transpose(void)
{
	/* static const char	*funcName = "Free_Utility_Transpose";  */

	if (transposePtr == NULL)
		return(FALSE);
	if (transposePtr->parList)
		FreeList_UniParMgr(&transposePtr->parList);
	if (transposePtr->parSpec == GLOBAL) {
		free(transposePtr);
		transposePtr = NULL;
	}
	return(TRUE);

}

/****************************** InitModeList **********************************/

/*
 * This function initialises the 'mode' list array
 */

BOOLN
InitModeList_Utility_Transpose(void)
{
	static NameSpecifier	modeList[] = {

			{ "STANDARD",		UTILITY_TRANSPOSE_STANDARD_MODE },
			{ "FIRST_CHANNEL",	UTILITY_TRANSPOSE_FIRST_CHANNEL_MODE },
			{ "",				UTILITY_TRANSPOSE_MODE_NULL },
		};
	transposePtr->modeList = modeList;
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
Init_Utility_Transpose(ParameterSpecifier parSpec)
{
	static const char	*funcName = "Init_Utility_Transpose";

	if (parSpec == GLOBAL) {
		if (transposePtr != NULL)
			Free_Utility_Transpose();
		if ((transposePtr = (TransposePtr) malloc(sizeof(Transpose))) == NULL) {
			NotifyError("%s: Out of memory for 'global' pointer", funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (transposePtr == NULL) {
			NotifyError("%s:  'local' pointer not set.", funcName);
			return(FALSE);
		}
	}
	transposePtr->parSpec = parSpec;
	transposePtr->modeFlag = TRUE;
	transposePtr->mode = UTILITY_TRANSPOSE_STANDARD_MODE;

	InitModeList_Utility_Transpose();
	if (!SetUniParList_Utility_Transpose()) {
		NotifyError("%s: Could not initialise parameter list.", funcName);
		Free_Utility_Transpose();
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
SetUniParList_Utility_Transpose(void)
{
	static const char *funcName = "SetUniParList_Utility_Transpose";
	UniParPtr	pars;

	if ((transposePtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  UTILITY_TRANSPOSE_NUM_PARS, NULL)) == NULL) {
		NotifyError("%s: Could not initialise parList.", funcName);
		return(FALSE);
	}
	pars = transposePtr->parList->pars;
	SetPar_UniParMgr(&pars[UTILITY_TRANSPOSE_MODE], "MODE",
	  "Sample labelling mode ('standard' or 'first channel'.",
	  UNIPAR_NAME_SPEC,
	  &transposePtr->mode, transposePtr->modeList,
	  (void * (*)) SetMode_Utility_Transpose);
	return(TRUE);

}

/****************************** GetUniParListPtr ******************************/

/*
 * This function returns a pointer to the module's universal parameter
 * list.
 */

UniParListPtr
GetUniParListPtr_Utility_Transpose(void)
{
	static const char	*funcName = "GetUniParListPtr_Utility_Transpose";

	if (transposePtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (transposePtr->parList == NULL) {
		NotifyError("%s: UniParList data structure has not been initialised. "
		  "NULL returned.", funcName);
		return(NULL);
	}
	return(transposePtr->parList);

}

/****************************** SetMode ***************************************/

/*
 * This function sets the module's mode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetMode_Utility_Transpose(char * theMode)
{
	static const char	*funcName = "SetMode_Utility_Transpose";
	int		specifier;

	if (transposePtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theMode,
		transposePtr->modeList)) == UTILITY_TRANSPOSE_MODE_NULL) {
		NotifyError("%s: Illegal name (%s).", funcName, theMode);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	transposePtr->modeFlag = TRUE;
	transposePtr->mode = specifier;
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
CheckPars_Utility_Transpose(void)
{
	static const char	*funcName = "CheckPars_Utility_Transpose";
	BOOLN	ok;

	ok = TRUE;
	if (transposePtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (!transposePtr->modeFlag) {
		NotifyError("%s: mode variable not set.", funcName);
		ok = FALSE;
	}
	return(ok);

}

/****************************** PrintPars *************************************/

/*
 * This routine prints all the module's parameters to the file stream.
 * specified by the lUTEar.parsFile file pointer.
 */

BOOLN
PrintPars_Utility_Transpose(void)
{
	static const char	*funcName = "PrintPars_Utility_Transpose";

	if (!CheckPars_Utility_Transpose()) {
		NotifyError("%s: Parameters have not been correctly set.", funcName);
		return(FALSE);
	}
	DPrint("Transpose Utility Module Parameters:-\n");
	DPrint("\tSample labelling mode = %s\n", transposePtr->modeList[
	  transposePtr->mode].name);
	return(TRUE);

}

/****************************** SetParsPointer ********************************/

/*
 * This function sets the global parameter pointer for the module to
 * that associated with the module instance.
 */

BOOLN
SetParsPointer_Utility_Transpose(ModulePtr theModule)
{
	static const char	*funcName = "SetParsPointer_Utility_Transpose";

	if (!theModule) {
		NotifyError("%s: The module is not set.", funcName);
		return(FALSE);
	}
	transposePtr = (TransposePtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 * It also initialises the process structure.
 */

BOOLN
InitModule_Utility_Transpose(ModulePtr theModule)
{
	static const char	*funcName = "InitModule_Utility_Transpose";

	if (!SetParsPointer_Utility_Transpose(theModule)) {
		NotifyError("%s: Cannot set parameters pointer.", funcName);
		return(FALSE);
	}
	if (!Init_Utility_Transpose(GLOBAL)) {
		NotifyError("%s: Could not initialise process structure.", funcName);
		return(FALSE);
	}
	theModule->parsPtr = transposePtr;
	theModule->CheckPars = CheckPars_Utility_Transpose;
	theModule->Free = Free_Utility_Transpose;
	theModule->GetUniParListPtr = GetUniParListPtr_Utility_Transpose;
	theModule->PrintPars = PrintPars_Utility_Transpose;
	theModule->RunProcess = Process_Utility_Transpose;
	theModule->SetParsPointer = SetParsPointer_Utility_Transpose;
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
CheckData_Utility_Transpose(EarObjectPtr data)
{
	static const char	*funcName = "CheckData_Utility_Transpose";

	if (data == NULL) {
		NotifyError("%s: EarObject not initialised.", funcName);
		return(FALSE);
	}
	if (!CheckInSignal_EarObject(data, funcName))
		return(FALSE);
	/*** Put additional checks here. ***/
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
Process_Utility_Transpose(EarObjectPtr data)
{
	static const char	*funcName = "Process_Utility_Transpose";
	register ChanData	 *inPtr;
	BOOLN	regularLabels;
	int		i, chan;
	double	*chanLabel, delta1, newDt, timeOffset;

	if (!CheckPars_Utility_Transpose())
		return(FALSE);
	if (!CheckData_Utility_Transpose(data)) {
		NotifyError("%s: Process data invalid.", funcName);
		return(FALSE);
	}
	SetProcessName_EarObject(data, "Transpose Utility Module process");
	chanLabel = data->inSignal[0]->info.chanLabel;
	if ((data->inSignal[0]->numChannels > 2)) {
		delta1 = chanLabel[1] - chanLabel[0];
		regularLabels = (!DBL_GREATER(delta1, chanLabel[2] - chanLabel[1]));
	} else
		regularLabels = FALSE;
	if ((transposePtr->mode == UTILITY_TRANSPOSE_STANDARD_MODE) &&
	  regularLabels) {
		newDt = delta1;
		timeOffset = chanLabel[0];
	} else {
		newDt = 1.0;
		timeOffset = 0.0;
	}
	  
	if (!InitOutSignal_EarObject(data, data->inSignal[0]->length,
	  data->inSignal[0]->numChannels, newDt)) {
		NotifyError("%s: Cannot initialise output channels.", funcName);
		return(FALSE);
	}
	SetOutputTimeOffset_SignalData(data->outSignal, timeOffset);
	SetLocalInfoFlag_SignalData(data->outSignal, TRUE);
	SetInfoChanDataTitle_SignalData(data->outSignal, data->inSignal[0]->info.
	  sampleTitle);
	SetInfoSampleTitle_SignalData(data->outSignal, data->inSignal[0]->info.
	  chanDataTitle);
	for (chan = 0; chan < data->inSignal[0]->numChannels; chan++) {
		inPtr = data->inSignal[0]->channel[chan];
		for (i = 0; i < data->outSignal->numChannels; i++)
			data->outSignal->channel[i][chan] = *inPtr++;
	}
	for (chan = 0; chan < data->outSignal->numChannels; chan++)
		data->outSignal->info.chanLabel[chan] = chan * data->inSignal[0]->dt;

	SetProcessContinuity_EarObject(data);
	return(TRUE);

}