/**********************
 *
 * File:		TrCollectSignals.c
 * Purpose:		This transform produces and output signal with a number of
 *				channels equal to the sum of the input signal channels.
 * Comments:	Written using ModuleProducer version 1.4.2 (Dec 19 2003).
 * Author:		L. P. O'Mard
 * Created:		22 Apr 2004
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
#include "TrCollectSignals.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

CollectSigsPtr	collectSigsPtr = NULL;

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
Free_Transform_CollectSignals(void)
{
	/* static const char	*funcName = "Free_Transform_CollectSignals";  */

	if (collectSigsPtr == NULL)
		return(FALSE);
	if (collectSigsPtr->labels) {
		free(collectSigsPtr->labels);
		collectSigsPtr->labels = NULL;
	}
	if (collectSigsPtr->parList)
		FreeList_UniParMgr(&collectSigsPtr->parList);
	if (collectSigsPtr->parSpec == GLOBAL) {
		free(collectSigsPtr);
		collectSigsPtr = NULL;
	}
	return(TRUE);

}

/****************************** InitLabelModeList *****************************/

/*
 * This function initialises the 'labelMode' list array
 */

BOOLN
InitLabelModeList_Transform_CollectSignals(void)
{
	static NameSpecifier	modeList[] = {

			{ "CHAN_INDEX",	TRANSFORM_COLLECTSIGNALS_LABELMODE_CHAN_INDEX },
			{ "INPUT_LABELS", TRANSFORM_COLLECTSIGNALS_LABELMODE_INPUT_LABELS },
			{ "USER",		TRANSFORM_COLLECTSIGNALS_LABELMODE_USER },
			{ "",			TRANSFORM_COLLECTSIGNALS_LABELMODE_NULL },
		};
	collectSigsPtr->labelModeList = modeList;
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
Init_Transform_CollectSignals(ParameterSpecifier parSpec)
{
	static const char	*funcName = "Init_Transform_CollectSignals";

	if (parSpec == GLOBAL) {
		if (collectSigsPtr != NULL)
			Free_Transform_CollectSignals();
		if ((collectSigsPtr = (CollectSigsPtr) malloc(sizeof(CollectSigs))) ==
		  NULL) {
			NotifyError("%s: Out of memory for 'global' pointer", funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (collectSigsPtr == NULL) {
			NotifyError("%s:  'local' pointer not set.", funcName);
			return(FALSE);
		}
	}
	collectSigsPtr->parSpec = parSpec;
	collectSigsPtr->labelModeFlag = TRUE;
	collectSigsPtr->labelMode = TRANSFORM_COLLECTSIGNALS_LABELMODE_CHAN_INDEX;
	collectSigsPtr->labels = NULL;

	InitLabelModeList_Transform_CollectSignals();
	if (!SetUniParList_Transform_CollectSignals()) {
		NotifyError("%s: Could not initialise parameter list.", funcName);
		Free_Transform_CollectSignals();
		return(FALSE);
	}
	collectSigsPtr->numChannels = 0;
	return(TRUE);

}

/****************************** SetUniParList *********************************/

/*
 * This function initialises and sets the module's universal parameter
 * list. This list provides universal access to the module's
 * parameters.  It expects to be called from the 'Init_' routine.
 */

BOOLN
SetUniParList_Transform_CollectSignals(void)
{
	static const char *funcName = "SetUniParList_Transform_CollectSignals";
	UniParPtr	pars;

	if ((collectSigsPtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  TRANSFORM_COLLECTSIGNALS_NUM_PARS, NULL)) == NULL) {
		NotifyError("%s: Could not initialise parList.", funcName);
		return(FALSE);
	}
	pars = collectSigsPtr->parList->pars;
	SetPar_UniParMgr(&pars[TRANSFORM_COLLECTSIGNALS_LABELMODE], "MODE",
	  "Channel labelling mode ('chan_index', 'input_labels' or 'user').",
	  UNIPAR_NAME_SPEC,
	  &collectSigsPtr->labelMode, collectSigsPtr->labelModeList,
	  (void * (*)) SetLabelMode_Transform_CollectSignals);
	SetPar_UniParMgr(&pars[TRANSFORM_COLLECTSIGNALS_LABELS], "LABEL",
	  "Channel label array (floating point values).",
	  UNIPAR_REAL_DYN_ARRAY,
	  &collectSigsPtr->labels, &collectSigsPtr->numChannels,
	  (void * (*)) SetIndividualLabel_Transform_CollectSignals);

	SetEnabledState_Transform_CollectSignals();
	return(TRUE);

}

/****************************** SetEnabledState *******************************/

/*
 * This routine sets the enabled states.
 */

void
SetEnabledState_Transform_CollectSignals(void)
{
	collectSigsPtr->parList->pars[TRANSFORM_COLLECTSIGNALS_LABELS].enabled =
	  (collectSigsPtr->labelMode == TRANSFORM_COLLECTSIGNALS_LABELMODE_USER);

}

/****************************** GetUniParListPtr ******************************/

/*
 * This function returns a pointer to the module's universal parameter
 * list.
 */

UniParListPtr
GetUniParListPtr_Transform_CollectSignals(void)
{
	static const char	*funcName = "GetUniParListPtr_Transform_CollectSignals";

	if (collectSigsPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (collectSigsPtr->parList == NULL) {
		NotifyError("%s: UniParList data structure has not been initialised. "
		  "NULL returned.", funcName);
		return(NULL);
	}
	return(collectSigsPtr->parList);

}

/****************************** SetLabelMode **********************************/

/*
 * This function sets the module's labelMode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetLabelMode_Transform_CollectSignals(char * theLabelMode)
{
	static const char	*funcName = "SetLabelMode_Transform_CollectSignals";
	int		specifier;

	if (collectSigsPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theLabelMode,
		collectSigsPtr->labelModeList)) ==
		  TRANSFORM_COLLECTSIGNALS_LABELMODE_NULL) {
		NotifyError("%s: Illegal name (%s).", funcName, theLabelMode);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	collectSigsPtr->labelModeFlag = TRUE;
	collectSigsPtr->labelMode = specifier;
	return(TRUE);

}

/****************************** SetLabels *************************************/

/*
 * This function sets the module's labels array.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetLabels_Transform_CollectSignals(double *theLabels)
{
	static const char	*funcName = "SetLabels_Transform_CollectSignals";

	if (collectSigsPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	collectSigsPtr->labels = theLabels;
	return(TRUE);

}

/****************************** SetIndividualLabel ****************************/

/*
 * This function sets the module's 'labels' array element.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetIndividualLabel_Transform_CollectSignals(int theIndex, double theLabel)
{
	static const char *funcName =
	  "SetIndividualLabelArray_Utility_CollectSignals";

	if (collectSigsPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (collectSigsPtr->labelMode != TRANSFORM_COLLECTSIGNALS_LABELMODE_USER) {
		NotifyError("%s: Channels can only be selected in 'user' Label "
		  "mode.", funcName);
		return(FALSE);
	}
	if ((theIndex > collectSigsPtr->numChannels - 1) &&
	  !ResizeDoubleArray_UniParMgr(&collectSigsPtr->labels,
	    &collectSigsPtr->numChannels, theIndex + 1)) {
		NotifyError("%s: Could not resize the 'labels' array.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	collectSigsPtr->labels[theIndex] = theLabel;
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
CheckPars_Transform_CollectSignals(void)
{
	static const char	*funcName = "CheckPars_Transform_CollectSignals";
	BOOLN	ok;

	ok = TRUE;
	if (collectSigsPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (!collectSigsPtr->labelModeFlag) {
		NotifyError("%s: labelMode variable not set.", funcName);
		ok = FALSE;
	}
	if ((collectSigsPtr->labelMode ==
	  TRANSFORM_COLLECTSIGNALS_LABELMODE_USER) && collectSigsPtr->labels ==
	  NULL) {
		NotifyError("%s: 'Labels' array not set.", funcName);
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
PrintPars_Transform_CollectSignals(void)
{
	static const char	*funcName = "PrintPars_Transform_CollectSignals";
	int		i;

	if (!CheckPars_Transform_CollectSignals()) {
		NotifyError("%s: Parameters have not been correctly set.", funcName);
		return(FALSE);
	}
	DPrint("Collect Signal Transform Module Parameters:-\n");
	DPrint("\tLabel mode = %s.\n", collectSigsPtr->labelModeList[
	  collectSigsPtr->labelMode].name);
	if (collectSigsPtr->labelMode == TRANSFORM_COLLECTSIGNALS_LABELMODE_USER) {
		DPrint("\tLabel Array:\n");
		for (i = 0; i < collectSigsPtr->numChannels; i++)
			DPrint("\tLabel[%2d]: %g\n", i, collectSigsPtr->labels[i]);
	}
	return(TRUE);

}

/****************************** SetParsPointer ********************************/

/*
 * This function sets the global parameter pointer for the module to
 * that associated with the module instance.
 */

BOOLN
SetParsPointer_Transform_CollectSignals(ModulePtr theModule)
{
	static const char	*funcName = "SetParsPointer_Transform_CollectSignals";

	if (!theModule) {
		NotifyError("%s: The module is not set.", funcName);
		return(FALSE);
	}
	collectSigsPtr = (CollectSigsPtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 * It also initialises the process structure.
 */

BOOLN
InitModule_Transform_CollectSignals(ModulePtr theModule)
{
	static const char	*funcName = "InitModule_Transform_CollectSignals";

	if (!SetParsPointer_Transform_CollectSignals(theModule)) {
		NotifyError("%s: Cannot set parameters pointer.", funcName);
		return(FALSE);
	}
	if (!Init_Transform_CollectSignals(GLOBAL)) {
		NotifyError("%s: Could not initialise process structure.", funcName);
		return(FALSE);
	}
	theModule->parsPtr = collectSigsPtr;
	theModule->CheckPars = CheckPars_Transform_CollectSignals;
	theModule->Free = Free_Transform_CollectSignals;
	theModule->GetUniParListPtr = GetUniParListPtr_Transform_CollectSignals;
	theModule->PrintPars = PrintPars_Transform_CollectSignals;
	theModule->RunProcess = Process_Transform_CollectSignals;
	theModule->SetParsPointer = SetParsPointer_Transform_CollectSignals;
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
CheckData_Transform_CollectSignals(EarObjectPtr data)
{
	static const char	*funcName = "CheckData_Transform_CollectSignals";
	BOOLN	ok = TRUE;
	int		i;

	if (data == NULL) {
		NotifyError("%s: EarObject not initialised.", funcName);
		return(FALSE);
	}
	if (!CheckInSignal_EarObject(data, funcName))
		return(FALSE);
	for (i = 1; ok && (i < data->numInSignals); i++)
		if ((data->inSignal[0]->length != data->inSignal[i]->length) ||
		  (fabs(data->inSignal[0]->dt - data->inSignal[i]->dt) > DBL_EPSILON) ||
		  (data->inSignal[0]->interleaveLevel != data->inSignal[i]->
		  interleaveLevel)) {
			NotifyError("%s: Input signal [%d] does not have the same length "
			  "and sampling interval as the first, [0].", funcName, i);
			ok = FALSE;
		}
	if (!ok)
		return(FALSE);
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
Process_Transform_CollectSignals(EarObjectPtr data)
{
	static const char	*funcName = "Process_Transform_CollectSignals";
	register ChanData	 **outChannels;
	int		i, chan, numChannels;
	double	*chanLabels, *userLabels;

	if (data == NULL) {
		NotifyError("%s: EarObject not initialised.", funcName);
		return(FALSE);
	}
	if (!CheckPars_Transform_CollectSignals())
		return(FALSE);
	if (!CheckData_Transform_CollectSignals(data)) {
		NotifyError("%s: Process data invalid.", funcName);
		return(FALSE);
	}
	SetProcessName_EarObject(data, "Collect channels transform module process");
	for (i = 0, numChannels = 0; i < data->numInSignals; i++)
		numChannels += data->inSignal[i]->numChannels;
	if ((collectSigsPtr->labelMode == TRANSFORM_COLLECTSIGNALS_LABELMODE_USER)
	  && (collectSigsPtr->numChannels != numChannels)) {
		NotifyError("%s: The number of labels (%d) be equal to the number of "
		  "channels in the output signal (%d).", funcName,
		  collectSigsPtr->numChannels, numChannels);
		return(FALSE);
	}
	data->externalDataFlag = TRUE;
	if (!InitOutSignal_EarObject(data, numChannels, data->inSignal[0]->length,
	  data->inSignal[0]->dt)) {
		NotifyError("%s: Cannot initialise output channels.", funcName);
		return(FALSE);
	}
	SetInterleaveLevel_SignalData(data->outSignal, data->inSignal[0]->
	  interleaveLevel);
	SetLocalInfoFlag_SignalData(data->outSignal, TRUE);
	outChannels = data->outSignal->channel;
	chanLabels = data->outSignal->info.chanLabel;
	userLabels = collectSigsPtr->labels;
	for (i = 0; i < data->numInSignals; i++)
		for (chan = 0; chan < data->inSignal[i]->numChannels; chan++) {
			*outChannels++ = data->inSignal[i]->channel[chan];
			switch (collectSigsPtr->labelMode) {
			case TRANSFORM_COLLECTSIGNALS_LABELMODE_INPUT_LABELS:
				*chanLabels++ = data->inSignal[i]->info.chanLabel[chan];
				break;
			case TRANSFORM_COLLECTSIGNALS_LABELMODE_USER:
				*chanLabels++ = *userLabels++;
				break;
			default:
				*chanLabels++ = (double) (outChannels -
				  data->outSignal->channel - 1);
			} /* switch */
		}

	SetProcessContinuity_EarObject(data);
	return(TRUE);

}

