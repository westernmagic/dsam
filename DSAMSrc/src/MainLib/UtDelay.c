/**********************
 *
 * File:		UtDelay.c
 * Purpose:		This module introduces a delay in a monaural signal or an
 *				interaural time difference (ITD) in a binaural signal.
 * Comments:	Written using ModuleProducer version 1.10 (Jan  3 1997).
 *				For binaural signals if the delay is positive, then the right
 *				(second) channel is delayed relative to the left (first)
 *				channel, and vice versus for negative delays.
 *				Monaural signals always treat delays as positive values.
 * Author:		L. P. O'Mard
 * Created:		08 Apr 1997
 * Updated:	
 * Copyright:	(c) 1998, University of Essex
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
#include "UtDelay.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

Delay2Ptr	delay2Ptr = NULL;

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
Free_Utility_Delay(void)
{
	/* static const char	*funcName = "Free_Utility_Delay";  */

	if (delay2Ptr == NULL)
		return(FALSE);
	if (delay2Ptr->parList)
		FreeList_UniParMgr(&delay2Ptr->parList);
	if (delay2Ptr->parSpec == GLOBAL) {
		free(delay2Ptr);
		delay2Ptr = NULL;
	}
	return(TRUE);

}

/****************************** InitModeList **********************************/

/*
 * This routine intialises the Mode list array.
 */

BOOLN
InitModeList_Utility_Delay(void)
{
	static NameSpecifier	modeList[] = {

					{ "SINGLE",	DELAY_SINGLE_MODE },
					{ "LINEAR",	DELAY_LINEAR_MODE },
					{ "", 		DELAY_NULL }
				};
	delay2Ptr->modeList = modeList;
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
Init_Utility_Delay(ParameterSpecifier parSpec)
{
	static const char	*funcName = "Init_Utility_Delay";

	if (parSpec == GLOBAL) {
		if (delay2Ptr != NULL)
			Free_Utility_Delay();
		if ((delay2Ptr = (Delay2Ptr) malloc(sizeof(Delay2))) == NULL) {
			NotifyError("%s: Out of memory for 'global' pointer", funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (delay2Ptr == NULL) {
			NotifyError("%s:  'local' pointer not set.", funcName);
			return(FALSE);
		}
	}
	delay2Ptr->parSpec = parSpec;
	delay2Ptr->modeFlag = TRUE;
	delay2Ptr->initialDelayFlag = TRUE;
	delay2Ptr->finalDelayFlag = TRUE;
	delay2Ptr->mode = DELAY_SINGLE_MODE;
	delay2Ptr->initialDelay = 0.0;
	delay2Ptr->finalDelay = 0.0;

	InitModeList_Utility_Delay();
	if (!SetUniParList_Utility_Delay()) {
		NotifyError("%s: Could not initialise parameter list.", funcName);
		Free_Utility_Delay();
		return(FALSE);
	}
	SetEnabledPars_Utility_Delay();
	return(TRUE);

}

/****************************** SetUniParList *********************************/

/*
 * This function initialises and sets the module's universal parameter
 * list. This list provides universal access to the module's
 * parameters.  It expects to be called from the 'Init_' routine.
 */

BOOLN
SetUniParList_Utility_Delay(void)
{
	static const char *funcName = "SetUniParList_Utility_Delay";
	UniParPtr	pars;

	if ((delay2Ptr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  UTILITY_DELAY_NUM_PARS, NULL)) == NULL) {
		NotifyError("%s: Could not initialise parList.", funcName);
		return(FALSE);
	}
	pars = delay2Ptr->parList->pars;
	SetPar_UniParMgr(&pars[UTILITY_DELAY_MODE], "MODE",
	  "Operation mode ('single' or 'linear').",
	  UNIPAR_NAME_SPEC,
	  &delay2Ptr->mode, delay2Ptr->modeList,
	  (void * (*)) SetMode_Utility_Delay);
	SetPar_UniParMgr(&pars[UTILITY_DELAY_INITIALDELAY], "INITIAL_DELAY",
	  "Initial time delay (s).",
	  UNIPAR_REAL,
	  &delay2Ptr->initialDelay, NULL,
	  (void * (*)) SetInitialDelay_Utility_Delay);
	SetPar_UniParMgr(&pars[UTILITY_DELAY_FINALDELAY], "FINAL_DELAY",
	  "Final time delay (not used with 'single' mode).",
	  UNIPAR_REAL,
	  &delay2Ptr->finalDelay, NULL,
	  (void * (*)) SetFinalDelay_Utility_Delay);
	return(TRUE);

}

/****************************** SetEnabledPars *******************************/

/*
 * This routine sets the enabled parameters.
 */

void
SetEnabledPars_Utility_Delay(void)
{
	delay2Ptr->parList->pars[UTILITY_DELAY_FINALDELAY].enabled = (delay2Ptr->
	  mode == DELAY_LINEAR_MODE);
	
}

/****************************** GetUniParListPtr ******************************/

/*
 * This function returns a pointer to the module's universal parameter
 * list.
 */

UniParListPtr
GetUniParListPtr_Utility_Delay(void)
{
	static const char	*funcName = "GetUniParListPtr_Utility_Delay";

	if (delay2Ptr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (delay2Ptr->parList == NULL) {
		NotifyError("%s: UniParList data structure has not been initialised. "
		  "NULL returned.", funcName);
		return(NULL);
	}
	return(delay2Ptr->parList);

}

/****************************** SetPars ***************************************/

/*
 * This function sets all the module's parameters.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetPars_Utility_Delay(char *mode, double initialDelay, double finalDelay)
{
	static const char	*funcName = "SetPars_Utility_Delay";
	BOOLN	ok;

	ok = TRUE;
	if (!SetMode_Utility_Delay(mode))
		ok = FALSE;
	if (!SetInitialDelay_Utility_Delay(initialDelay))
		ok = FALSE;
	if (!SetFinalDelay_Utility_Delay(finalDelay))
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
SetMode_Utility_Delay(char *theMode)
{
	static const char	*funcName = "SetMode_Utility_Delay";
	int		specifier;

	if (delay2Ptr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theMode, delay2Ptr->modeList)) ==
	  DELAY_NULL) {
		NotifyError("%s: Illegal mode name (%s).", funcName, theMode);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	delay2Ptr->modeFlag = TRUE;
	delay2Ptr->mode = specifier;
	SetEnabledPars_Utility_Delay();
	return(TRUE);

}

/****************************** SetInitialDelay *******************************/

/*
 * This function sets the module's initialDelay parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetInitialDelay_Utility_Delay(double theInitialDelay)
{
	static const char	*funcName = "SetInitialDelay_Utility_Delay";

	if (delay2Ptr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	delay2Ptr->initialDelayFlag = TRUE;
	delay2Ptr->initialDelay = theInitialDelay;
	return(TRUE);

}

/****************************** SetFinalDelay *********************************/

/*
 * This function sets the module's finalDelay parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetFinalDelay_Utility_Delay(double theFinalDelay)
{
	static const char	*funcName = "SetFinalDelay_Utility_Delay";

	if (delay2Ptr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	delay2Ptr->finalDelayFlag = TRUE;
	delay2Ptr->finalDelay = theFinalDelay;
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
CheckPars_Utility_Delay(void)
{
	static const char	*funcName = "CheckPars_Utility_Delay";
	BOOLN	ok;

	ok = TRUE;
	if (delay2Ptr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (!delay2Ptr->modeFlag) {
		NotifyError("%s: mode variable not set.", funcName);
		ok = FALSE;
	}
	if (!delay2Ptr->initialDelayFlag) {
		NotifyError("%s: initialDelay variable not set.", funcName);
		ok = FALSE;
	}
	if (!delay2Ptr->finalDelayFlag) {
		NotifyError("%s: finalDelay variable not set.", funcName);
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
PrintPars_Utility_Delay(void)
{
	static const char	*funcName = "PrintPars_Utility_Delay";

	if (!CheckPars_Utility_Delay()) {
		NotifyError("%s: Parameters have not been correctly set.", funcName);
		return(FALSE);
	}
	DPrint("Create binaural ITD Utility Module Parameters:-\n");
	DPrint("\tMode = %s,\n", delay2Ptr->modeList[delay2Ptr->mode].name);
	DPrint("\tInitial delay = %g ms", MSEC(delay2Ptr->initialDelay));
	if (delay2Ptr->mode == DELAY_LINEAR_MODE)
		DPrint(",\tfinalDelay = %g ms", MSEC(delay2Ptr->finalDelay));
	DPrint(".\n");
	return(TRUE);

}

/****************************** ReadPars **************************************/

/*
 * This program reads a specified number of parameters from a file.
 * It returns FALSE if it fails in any way.n */

BOOLN
ReadPars_Utility_Delay(char *fileName)
{
	static const char	*funcName = "ReadPars_Utility_Delay";
	BOOLN	ok;
	char	*filePath;
	char	mode[MAXLINE];
	double	initialDelay, finalDelay;
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
	if (!GetPars_ParFile(fp, "%lf", &initialDelay))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &finalDelay))
		ok = FALSE;
	fclose(fp);
	Free_ParFile();
	if (!ok) {
		NotifyError("%s: Not enough lines, or invalid parameters, in module "
		  "parameter file '%s'.", funcName, filePath);
		return(FALSE);
	}
	if (!SetPars_Utility_Delay(mode, initialDelay, finalDelay)) {
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
SetParsPointer_Utility_Delay(ModulePtr theModule)
{
	static const char	*funcName = "SetParsPointer_Utility_Delay";

	if (!theModule) {
		NotifyError("%s: The module is not set.", funcName);
		return(FALSE);
	}
	delay2Ptr = (Delay2Ptr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 */

BOOLN
InitModule_Utility_Delay(ModulePtr theModule)
{
	static const char	*funcName = "InitModule_Utility_Delay";

	if (!SetParsPointer_Utility_Delay(theModule)) {
		NotifyError("%s: Cannot set parameters pointer.", funcName);
		return(FALSE);
	}
	if (!Init_Utility_Delay(GLOBAL)) {
		NotifyError("%s: Could not initialise process structure.", funcName);
		return(FALSE);
	}
	theModule->parsPtr = delay2Ptr;
	theModule->threadMode = MODULE_THREAD_MODE_SIMPLE;
	theModule->CheckPars = CheckPars_Utility_Delay;
	theModule->Free = Free_Utility_Delay;
	theModule->GetUniParListPtr = GetUniParListPtr_Utility_Delay;
	theModule->PrintPars = PrintPars_Utility_Delay;
	theModule->ReadPars = ReadPars_Utility_Delay;
	theModule->RunProcess = Process_Utility_Delay;
	theModule->SetParsPointer = SetParsPointer_Utility_Delay;
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
CheckData_Utility_Delay(EarObjectPtr data)
{
	static const char	*funcName = "CheckData_Utility_Delay";
	double	signalDuration;

	if (data == NULL) {
		NotifyError("%s: EarObject not initialised.", funcName);
		return(FALSE);
	}
	if (!CheckInSignal_EarObject(data, funcName))
		return(FALSE);
	signalDuration = _GetDuration_SignalData(data->inSignal[0]);
	if (fabs(delay2Ptr->initialDelay) > signalDuration) {
		NotifyError("%s: Delay (%g ms) is longer than signal.", funcName,
		  delay2Ptr->initialDelay, _GetDuration_SignalData(data->inSignal[0]));
		return(FALSE);
	}
	if (fabs(delay2Ptr->finalDelay) > signalDuration) {
		NotifyError("%s: Delay (%g ms) is longer than signal.", funcName,
		  delay2Ptr->initialDelay, _GetDuration_SignalData(data->inSignal[0]));
		return(FALSE);
	}
	switch (delay2Ptr->mode) {
	case DELAY_LINEAR_MODE:
		if ((delay2Ptr->finalDelay < delay2Ptr->initialDelay)) {
			NotifyError("%s: Illegal delay range (%g - %g ms).", funcName,
			  delay2Ptr->initialDelay, delay2Ptr->finalDelay);
			return(FALSE);
		}
		if ((data->inSignal[0]->numChannels /
		  data->inSignal[0]->interleaveLevel) < 2) {
			NotifyError("%s: LINEAR mode can only be used with multi-channel\n"
			  "monaural or binaural signals.", funcName);
			return(FALSE);
		}
		break;
	default:
		break;
	} /* switch */
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
Process_Utility_Delay(EarObjectPtr data)
{
	static const char	*funcName = "Process_Utility_Delay";
	register	ChanData	*inPtr, *outPtr;
	int		chan, delayedChan;
	double	delay = 0.0;
	ChanLen	i, delayIndex;
	Delay2Ptr	p = delay2Ptr;

	if (!data->threadRunFlag) {
		if (!CheckPars_Utility_Delay())
			return(FALSE);
		if (!CheckData_Utility_Delay(data)) {
			NotifyError("%s: Process data invalid.", funcName);
			return(FALSE);
		}
		SetProcessName_EarObject(data, "Introduce ITD into binaural signal");
		if (!InitOutFromInSignal_EarObject(data, 0)) {
			NotifyError("%s: Cannot initialise output channels.", funcName);
			return(FALSE);
		}
		SetLocalInfoFlag_SignalData(data->outSignal, TRUE);
		SetInfoChannelTitle_SignalData(data->outSignal, "Delay-ITD ms");
		p->delayPerChannel = (p->mode != DELAY_LINEAR_MODE)? 0.0: (p->
		  finalDelay - p->initialDelay) / (data->outSignal->numChannels / data->
		  outSignal->interleaveLevel - 1);
		if (data->initThreadRunFlag)
			return(TRUE);
	}
	for (chan = data->outSignal->offset; chan < data->outSignal->numChannels;
	  chan += data->outSignal->interleaveLevel) {
		switch (p->mode) {
		case DELAY_SINGLE_MODE:
			delay = p->initialDelay;
			break;
		case DELAY_LINEAR_MODE:
			delay = p->delayPerChannel * (chan /
			  data->outSignal->interleaveLevel) + p->initialDelay;
			break;
		} /* switch */
		delayIndex = (ChanLen) fabs(delay / data->inSignal[0]->dt + 0.5);
		SetInfoChannelLabel_SignalData(data->outSignal, chan, MSEC(delay));
		if (data->outSignal->interleaveLevel == 1)
			delayedChan = chan;
		else {
			delayedChan = (p->initialDelay > 0.0)? chan + 1: chan;
			SetInfoChannelLabel_SignalData(data->outSignal, chan + 1, delay);
		}
		inPtr = data->inSignal[0]->channel[delayedChan];
		outPtr = data->outSignal->channel[delayedChan];
		for (i = 0; i < delayIndex; i++)
				*outPtr++ = 0.0;
		for (i = delayIndex; i < data->outSignal->length; i++)
			*outPtr++ = *inPtr++;
	}
	SetProcessContinuity_EarObject(data);
	return(TRUE);

}

