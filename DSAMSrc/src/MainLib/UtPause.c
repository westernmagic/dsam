/**********************
 *
 * File:		UtPause.c
 * Purpose:		This module allows the processing of a simulation to be
 *				interrupted, producing a specified message.
 * Comments:	Written using ModuleProducer version 1.2.4 (May  7 1999).
 *				27-05-99 LPO: Added the 'bellMode' option.
 * Author:		L. P. O'Mard
 * Created:		07 May 1999
 * Updated:		27 May 1999
 * Copyright:	(c) 1999, University of Essex.
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
#include "StdMessage.h"
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
	/* static const char	*funcName = "Free_Utility_Pause";  */

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
	static const char	*funcName = "Init_Utility_Pause";

	if (parSpec == GLOBAL) {
		if (pausePtr != NULL)
			Free_Utility_Pause();
		if ((pausePtr = (PausePtr) malloc(sizeof(Pause))) == NULL) {
			NotifyError("%s: Out of memory for 'global' pointer", funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (pausePtr == NULL) {
			NotifyError("%s:  'local' pointer not set.", funcName);
			return(FALSE);
		}
	}
	pausePtr->parSpec = parSpec;
	pausePtr->bellModeFlag = FALSE;
	pausePtr->delayFlag = FALSE;
	pausePtr->messageFlag = FALSE;
	pausePtr->bellMode = 0;
	pausePtr->delay = 0;
	pausePtr->message[0] = '\0';

	if (!SetUniParList_Utility_Pause()) {
		NotifyError("%s: Could not initialise parameter list.", funcName);
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
	static const char *funcName = "SetUniParList_Utility_Pause";
	UniParPtr	pars;

	if ((pausePtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  UTILITY_PAUSE_NUM_PARS, NULL)) == NULL) {
		NotifyError("%s: Could not initialise parList.", funcName);
		return(FALSE);
	}
	pars = pausePtr->parList->pars;
	SetPar_UniParMgr(&pars[UTILITY_PAUSE_BELLMODE], "BELL_MODE",
	  "Bell mode ('on' or 'off').",
	  UNIPAR_BOOL,
	  &pausePtr->bellMode, NULL,
	  (void * (*)) SetBellMode_Utility_Pause);
	SetPar_UniParMgr(&pars[UTILITY_PAUSE_DELAY], "DELAY",
	  "Delay time: negative values mean indefinite period (s).",
	  UNIPAR_INT,
	  &pausePtr->delay, NULL,
	  (void * (*)) SetDelay_Utility_Pause);
	SetPar_UniParMgr(&pars[UTILITY_PAUSE_MESSAGE], "MESSAGE",
	  "Pause diagnostic message.",
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
	static const char	*funcName = "GetUniParListPtr_Utility_Pause";

	if (pausePtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (pausePtr->parList == NULL) {
		NotifyError("%s: UniParList data structure has not been initialised. "
		  "NULL returned.", funcName);
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
SetPars_Utility_Pause(char * bellMode, int delay, char *message)
{
	static const char	*funcName = "SetPars_Utility_Pause";
	BOOLN	ok;

	ok = TRUE;
	if (!SetBellMode_Utility_Pause(bellMode))
		ok = FALSE;
	if (!SetDelay_Utility_Pause(delay))
		ok = FALSE;
	if (!SetMessage_Utility_Pause(message))
		ok = FALSE;
	if (!ok)
		NotifyError("%s: Failed to set all module parameters." ,funcName);
	return(ok);

}

/****************************** SetBellMode ***********************************/

/*
 * This function sets the module's bellMode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetBellMode_Utility_Pause(char * theBellMode)
{
	static const char	*funcName = "SetBellMode_Utility_Pause";
	int		specifier;

	if (pausePtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theBellMode,
		BooleanList_NSpecLists(0))) == GENERAL_BOOLEAN_NULL) {
		NotifyError("%s: Illegal name (%s).", funcName, theBellMode);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	pausePtr->bellModeFlag = TRUE;
	pausePtr->bellMode = specifier;
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
	static const char	*funcName = "SetDelay_Utility_Pause";

	if (pausePtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
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
SetMessage_Utility_Pause(char *theMessage)
{
	static const char	*funcName = "SetMessage_Utility_Pause";

	if (pausePtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	pausePtr->messageFlag = TRUE;
	snprintf(pausePtr->message, LONG_STRING, "%s", theMessage);
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
	static const char	*funcName = "CheckPars_Utility_Pause";
	BOOLN	ok;

	ok = TRUE;
	if (pausePtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (!pausePtr->bellModeFlag) {
		NotifyError("%s: bellMode variable not set.", funcName);
		ok = FALSE;
	}
	if (!pausePtr->delayFlag) {
		NotifyError("%s: delay variable not set.", funcName);
		ok = FALSE;
	}
	if (!pausePtr->messageFlag) {
		NotifyError("%s: message array not set.", funcName);
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
	static const char	*funcName = "PrintPars_Utility_Pause";

	if (!CheckPars_Utility_Pause()) {
		NotifyError("%s: Parameters have not been correctly set.", funcName);
		return(FALSE);
	}
	DPrint("Pause Utility Module Parameters:-\n");
	DPrint("\tBell mode = %s,", BooleanList_NSpecLists(
	  pausePtr->bellMode)->name);
	DPrint("\tdelay = ");
	if (pausePtr->delay < 0)
		DPrint("indefinite.\n");
	else
		DPrint("%d (s)\n", pausePtr->delay);
	DPrint("\tmessage: %s\n", pausePtr->message);
	return(TRUE);

}

/****************************** ReadPars **************************************/

/*
 * This program reads a specified number of parameters from a file.
 * It returns FALSE if it fails in any way.n */

BOOLN
ReadPars_Utility_Pause(char *fileName)
{
	static const char	*funcName = "ReadPars_Utility_Pause";
	BOOLN	ok;
	char	*filePath, bellMode[MAXLINE], message[LONG_STRING];
	int		delay;
	FILE	*fp;

	filePath = GetParsFileFPath_Common(fileName);
	if ((fp = fopen(filePath, "r")) == NULL) {
		NotifyError("%s: Cannot open data file '%s'.\n", funcName, filePath);
		return(FALSE);
	}
	DPrint("%s: Reading from '%s':\n", funcName, filePath);
	Init_ParFile();
	ok = TRUE;
	if (!GetPars_ParFile(fp, "%s", bellMode))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%d", &delay))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%s", message))
		ok = FALSE;
	fclose(fp);
	Free_ParFile();
	if (!ok) {
		NotifyError("%s: Not enough lines, or invalid parameters, in module "
		  "parameter file '%s'.", funcName, filePath);
		return(FALSE);
	}
	if (!SetPars_Utility_Pause(bellMode, delay, message)) {
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
SetParsPointer_Utility_Pause(ModulePtr theModule)
{
	static const char	*funcName = "SetParsPointer_Utility_Pause";

	if (!theModule) {
		NotifyError("%s: The module is not set.", funcName);
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
	static const char	*funcName = "InitModule_Utility_Pause";

	if (!SetParsPointer_Utility_Pause(theModule)) {
		NotifyError("%s: Cannot set parameters pointer.", funcName);
		return(FALSE);
	}
	if (!Init_Utility_Pause(GLOBAL)) {
		NotifyError("%s: Could not initialise process structure.", funcName);
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
	static const char	*funcName = "CheckData_Utility_Pause";

	if (data == NULL) {
		NotifyError("%s: EarObject not initialised.", funcName);
		return(FALSE);
	}
	if (!CheckInit_SignalData(data->inSignal[0], funcName))
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
 * The 'oldDontUseGUIFlag' is needed because the GUI version of the
 * 'Notify_Message' changes GetDSAMPtr_Common()->noGUIOutputFlag.
 * It expects a user response before returning.
 */

void
Notify_Utility_Pause(char *format, ...)
{
	BOOLN	oldDontUseGUIFlag;
	va_list	args;

	va_start(args, format);
	oldDontUseGUIFlag = GetDSAMPtr_Common()->noGUIOutputFlag;
	Notify_Message(format, args, COMMON_GENERAL_DIAGNOSTIC);
	GetDSAMPtr_Common()->noGUIOutputFlag = oldDontUseGUIFlag;
	if (GetDSAMPtr_Common()->noGUIOutputFlag) {
		printf("Press <return> to continue...");
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
	static const char	*funcName = "Process_Utility_Pause";
	time_t	startTime;
	FILE	*oldParsFile;

	if (data == NULL) {
		NotifyError("%s: EarObject not initialised.", funcName);
		return(FALSE);
	}
	if (!CheckPars_Utility_Pause())
		return(FALSE);
	if (!CheckData_Utility_Pause(data)) {
		NotifyError("%s: Process data invalid.", funcName);
		return(FALSE);
	}
	SetProcessName_EarObject(data, "Pause utility module process");
	data->updateCustomersFlag = (data->inSignal[0] != data->outSignal);
	data->outSignal = data->inSignal[0];

	if (pausePtr->bellMode == GENERAL_BOOLEAN_ON)
		fprintf(stderr, "\07");
 	if (pausePtr->delay > 0.0) {
		oldParsFile = GetDSAMPtr_Common()->parsFile;
		GetDSAMPtr_Common()->parsFile = stdout;
		DPrint("\n%s\n", pausePtr->message);
		DPrint("\n>> Pausing for %d seconds...\n", pausePtr->delay);
		for (startTime = time(NULL); (difftime(time(NULL), startTime) <
		  pausePtr->delay); )
 			;
		GetDSAMPtr_Common()->parsFile = oldParsFile;
	} else {
		Notify_Utility_Pause("%s", pausePtr->message);
	}
	SetProcessContinuity_EarObject(data);
	return(TRUE);

}

