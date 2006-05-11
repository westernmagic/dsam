/**********************
 *
 * File:		UtPause.c
 * Purpose:		This module allows the processing of a simulation to be
 *				interrupted, producing a specified message.
 * Comments:	Written using ModuleProducer version 1.2.4 (May  7 1999).
 *				27-05-99 LPO: Added the 'alertMode' option.
 *				02-03-05 LPO: There is no point in thread enabling this module.
 * Author:		L. P. O'Mard
 * Created:		07 May 1999
 * Updated:		27 May 1999
 * Copyright:	(c) 2005, CNBH, University of Essex.
 *
 *********************/

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "GeCommon.h"
#include "GeSignalData.h"
#include "GeEarObject.h"
#include "GeNSpecLists.h"
#include "GeUniParMgr.h"
#include "GeModuleMgr.h"
#include "FiParFile.h"
#include "UtString.h"
#include "UtPause.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

PausePtr	pausePtr = NULL;

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
Free_Utility_Pause(void)
{
	if (pausePtr == NULL)
		return(FALSE);
	if (pausePtr->parList)
		FreeList_UniParMgr(&pausePtr->parList);
	if (pausePtr->parSpec == GLOBAL) {
		free(pausePtr);
		pausePtr = NULL;
	}
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
Init_Utility_Pause(ParameterSpecifier parSpec)
{
	static const WChar	*funcName = wxT("Init_Utility_Pause");

	if (parSpec == GLOBAL) {
		if (pausePtr != NULL)
			Free_Utility_Pause();
		if ((pausePtr = (PausePtr) malloc(sizeof(Pause))) == NULL) {
			NotifyError(wxT("%s: Out of memory for 'global' pointer"),
			  funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (pausePtr == NULL) {
			NotifyError(wxT("%s:  'local' pointer not set."), funcName);
			return(FALSE);
		}
	}
	pausePtr->parSpec = parSpec;
	pausePtr->alertModeFlag = TRUE;
	pausePtr->delayFlag = TRUE;
	pausePtr->messageFlag = TRUE;
	pausePtr->alertMode = GENERAL_BOOLEAN_ON;
	pausePtr->delay = -1;
	DSAM_snprintf(pausePtr->message, MAXLINE, wxT("Processing paused"));

	if (!SetUniParList_Utility_Pause()) {
		NotifyError(wxT("%s: Could not initialise parameter list."), funcName);
		Free_Utility_Pause();
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
SetUniParList_Utility_Pause(void)
{
	static const WChar *funcName = wxT("SetUniParList_Utility_Pause");
	UniParPtr	pars;

	if ((pausePtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  UTILITY_PAUSE_NUM_PARS, NULL)) == NULL) {
		NotifyError(wxT("%s: Could not initialise parList."), funcName);
		return(FALSE);
	}
	pars = pausePtr->parList->pars;
	SetPar_UniParMgr(&pars[UTILITY_PAUSE_ALERTMODE], wxT("ALERT_MODE"),
	  wxT("Bell mode ('on' or 'off')."),
	  UNIPAR_BOOL,
	  &pausePtr->alertMode, NULL,
	  (void * (*)) SetAlertMode_Utility_Pause);
	SetPar_UniParMgr(&pars[UTILITY_PAUSE_DELAY], wxT("DELAY"),
	  wxT("Delay time: negative values mean indefinite period (s)."),
	  UNIPAR_INT,
	  &pausePtr->delay, NULL,
	  (void * (*)) SetDelay_Utility_Pause);
	SetPar_UniParMgr(&pars[UTILITY_PAUSE_MESSAGE], wxT("MESSAGE"),
	  wxT("Pause diagnostic message."),
	  UNIPAR_STRING,
	  &pausePtr->message, NULL,
	  (void * (*)) SetMessage_Utility_Pause);
	return(TRUE);

}

/****************************** GetUniParListPtr ******************************/

/*
 * This function returns a pointer to the module's universal parameter
 * list.
 */

UniParListPtr
GetUniParListPtr_Utility_Pause(void)
{
	static const WChar	*funcName = wxT("GetUniParListPtr_Utility_Pause");

	if (pausePtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (pausePtr->parList == NULL) {
		NotifyError(wxT("%s: UniParList data structure has not been "
		  "initialised.  NULL returned."), funcName);
		return(NULL);
	}
	return(pausePtr->parList);

}

/****************************** SetPars ***************************************/

/*
 * This function sets all the module's parameters.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetPars_Utility_Pause(WChar * alertMode, int delay, WChar *message)
{
	static const WChar	*funcName = wxT("SetPars_Utility_Pause");
	BOOLN	ok;

	ok = TRUE;
	if (!SetAlertMode_Utility_Pause(alertMode))
		ok = FALSE;
	if (!SetDelay_Utility_Pause(delay))
		ok = FALSE;
	if (!SetMessage_Utility_Pause(message))
		ok = FALSE;
	if (!ok)
		NotifyError(wxT("%s: Failed to set all module parameters.") ,funcName);
	return(ok);

}

/****************************** SetAlertMode **********************************/

/*
 * This function sets the module's alertMode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetAlertMode_Utility_Pause(WChar * theAlertMode)
{
	static const WChar	*funcName = wxT("SetAlertMode_Utility_Pause");
	int		specifier;

	if (pausePtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theAlertMode,
		BooleanList_NSpecLists(0))) == GENERAL_BOOLEAN_NULL) {
		NotifyError(wxT("%s: Illegal name (%s)."), funcName, theAlertMode);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	pausePtr->alertModeFlag = TRUE;
	pausePtr->alertMode = specifier;
	return(TRUE);

}

/****************************** SetDelay **************************************/

/*
 * This function sets the module's delay parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetDelay_Utility_Pause(int theDelay)
{
	static const WChar	*funcName = wxT("SetDelay_Utility_Pause");

	if (pausePtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	pausePtr->delayFlag = TRUE;
	pausePtr->delay = theDelay;
	return(TRUE);

}

/****************************** SetMessage ************************************/

/*
 * This function sets the module's message array.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetMessage_Utility_Pause(WChar *theMessage)
{
	static const WChar	*funcName = wxT("SetMessage_Utility_Pause");

	if (pausePtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	pausePtr->messageFlag = TRUE;
	DSAM_snprintf(pausePtr->message, LONG_STRING, wxT("%s"), theMessage);
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
CheckPars_Utility_Pause(void)
{
	static const WChar	*funcName = wxT("CheckPars_Utility_Pause");
	BOOLN	ok;

	ok = TRUE;
	if (pausePtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (!pausePtr->alertModeFlag) {
		NotifyError(wxT("%s: alertMode variable not set."), funcName);
		ok = FALSE;
	}
	if (!pausePtr->delayFlag) {
		NotifyError(wxT("%s: delay variable not set."), funcName);
		ok = FALSE;
	}
	if (!pausePtr->messageFlag) {
		NotifyError(wxT("%s: message array not set."), funcName);
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
PrintPars_Utility_Pause(void)
{
	static const WChar	*funcName = wxT("PrintPars_Utility_Pause");

	if (!CheckPars_Utility_Pause()) {
		NotifyError(wxT("%s: Parameters have not been correctly set."),
		  funcName);
		return(FALSE);
	}
	DPrint(wxT("Pause Utility Module Parameters:-\n"));
	DPrint(wxT("\tBell mode = %s,"), BooleanList_NSpecLists(
	  pausePtr->alertMode)->name);
	DPrint(wxT("\tdelay = "));
	if (pausePtr->delay < 0)
		DPrint(wxT("indefinite.\n"));
	else
		DPrint(wxT("%d (s)\n"), pausePtr->delay);
	DPrint(wxT("\tmessage: %s\n"), pausePtr->message);
	return(TRUE);

}

/****************************** ReadPars **************************************/

/*
 * This program reads a specified number of parameters from a file.
 * It returns FALSE if it fails in any way.n */

BOOLN
ReadPars_Utility_Pause(WChar *fileName)
{
	static const WChar	*funcName = wxT("ReadPars_Utility_Pause");
	BOOLN	ok;
	WChar	*filePath, alertMode[MAXLINE], message[LONG_STRING];
	int		delay;
	FILE	*fp;

	filePath = GetParsFileFPath_Common(fileName);
	if ((fp = fopen(ConvUTF8_Utility_String(filePath), "r")) == NULL) {
		NotifyError(wxT("%s: Cannot open data file '%s'.\n"), funcName,
		  filePath);
		return(FALSE);
	}
	DPrint(wxT("%s: Reading from '%s':\n"), funcName, filePath);
	Init_ParFile();
	ok = TRUE;
	if (!GetPars_ParFile(fp, wxT("%s"), alertMode))
		ok = FALSE;
	if (!GetPars_ParFile(fp, wxT("%d"), &delay))
		ok = FALSE;
	if (!GetPars_ParFile(fp, wxT("%s"), message))
		ok = FALSE;
	fclose(fp);
	Free_ParFile();
	if (!ok) {
		NotifyError(wxT("%s: Not enough lines, or invalid parameters, in "
		  "module parameter file '%s'."), funcName, filePath);
		return(FALSE);
	}
	if (!SetPars_Utility_Pause(alertMode, delay, message)) {
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
SetParsPointer_Utility_Pause(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("SetParsPointer_Utility_Pause");

	if (!theModule) {
		NotifyError(wxT("%s: The module is not set."), funcName);
		return(FALSE);
	}
	pausePtr = (PausePtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 */

BOOLN
InitModule_Utility_Pause(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("InitModule_Utility_Pause");

	if (!SetParsPointer_Utility_Pause(theModule)) {
		NotifyError(wxT("%s: Cannot set parameters pointer."), funcName);
		return(FALSE);
	}
	if (!Init_Utility_Pause(GLOBAL)) {
		NotifyError(wxT("%s: Could not initialise process structure."),
		  funcName);
		return(FALSE);
	}
	theModule->parsPtr = pausePtr;
	theModule->CheckPars = CheckPars_Utility_Pause;
	theModule->Free = Free_Utility_Pause;
	theModule->GetUniParListPtr = GetUniParListPtr_Utility_Pause;
	theModule->PrintPars = PrintPars_Utility_Pause;
	theModule->ReadPars = ReadPars_Utility_Pause;
	theModule->RunProcess = Process_Utility_Pause;
	theModule->SetParsPointer = SetParsPointer_Utility_Pause;
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
CheckData_Utility_Pause(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("CheckData_Utility_Pause");

	if (data == NULL) {
		NotifyError(wxT("%s: EarObject not initialised."), funcName);
		return(FALSE);
	}
	if (!CheckInSignal_EarObject(data, funcName))
		return(FALSE);
	/*** Put additional checks here. ***/
	return(TRUE);

}

/***************************** Notify *****************************************/

/*
 * This routine prints out an alert message.
 * Output is always to 'stdou' or the GUI.
 * It uses the same format as 'NotifyError' etc. so that it can use the
 * 'Notify_Message' routine.
 * The 'oldDialogStatusFlag' is needed because the GUI version of the
 * 'Notify_Message' changes GetDSAMPtr_Common()->dialogOutputFlag.
 * It expects a user response before returning.
 */

void
Notify_Utility_Pause(WChar *format, ...)
{
	DiagModeSpecifier	oldDiagMode = GetDSAMPtr_Common()->diagMode;
	va_list	args;

	va_start(args, format);
	(GetDSAMPtr_Common()->Notify)(format, args,
	  COMMON_GENERAL_DIAGNOSTIC_WITH_CANCEL);
	SetDiagMode(oldDiagMode);
	if (GetDSAMPtr_Common()->interruptRequestedFlag) {
		GetDSAMPtr_Common()->notificationCount++; /* no more dialogs */
		return;
	}
	if (GetDSAMPtr_Common()->diagMode == COMMON_CONSOLE_DIAG_MODE) {
		DSAM_printf(wxT("Press <return> to continue..."));
		getchar();
	}
	va_end(args);

} /* NotifyError */

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
Process_Utility_Pause(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("Process_Utility_Pause");
	time_t	startTime;
	FILE	*oldParsFile;

	if (!data->threadRunFlag) {
		if (!CheckPars_Utility_Pause())
			return(FALSE);
		if (!CheckData_Utility_Pause(data)) {
			NotifyError(wxT("%s: Process data invalid."), funcName);
			return(FALSE);
		}
		SetProcessName_EarObject(data, wxT("Pause utility module process"));
		data->updateCustomersFlag = (data->inSignal[0] != data->outSignal);
		data->outSignal = data->inSignal[0];
		if (data->initThreadRunFlag)
			return(TRUE);
	}

	if (pausePtr->alertMode == GENERAL_BOOLEAN_ON)
		DSAM_fprintf(stderr, wxT("\07"));
 	if (pausePtr->delay > 0.0) {
		oldParsFile = GetDSAMPtr_Common()->parsFile;
		GetDSAMPtr_Common()->parsFile = stdout;
		DPrint(wxT("\n%s\n"), pausePtr->message);
		DPrint(wxT("\n>> Pausing for %d seconds...\n"), pausePtr->delay);
		for (startTime = time(NULL); (difftime(time(NULL), startTime) <
		  pausePtr->delay); )
 			if (GetDSAMPtr_Common()->interruptRequestedFlag)
				break;
		GetDSAMPtr_Common()->parsFile = oldParsFile;
	} else if (pausePtr->alertMode == GENERAL_BOOLEAN_ON) {
		Notify_Utility_Pause(wxT("%s"), pausePtr->message);
	}
	SetProcessContinuity_EarObject(data);
	return(TRUE);

}

