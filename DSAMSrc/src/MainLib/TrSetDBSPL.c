/**********************
 *
 * File:		TrSetDBSPL.c
 * Purpose:		This program calculates a signal's current intensity then
 *				adjusts it to a specified intensity.
 * Comments:	Written using ModuleProducer version 1.12 (Oct 12 1997).
 *				The intensity is calculated starting from an offset position.
 *				23-04-98 LPO: This routine will calculate the normalisation
 *				gain for the first segment only.
 * Author:		L. P. O'Mard
 * Created:		06 Nov 1997
 * Updated:		23 Apr 1998
 * Copyright:	(c) 1998, University of Essex
 *
 *********************/

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "GeCommon.h"
#include "GeSignalData.h"
#include "GeEarObject.h"
#include "GeUniParMgr.h"
#include "GeModuleMgr.h"
#include "FiParFile.h"
#include "UtString.h"
#include "TrSetDBSPL.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

SetDBSPLPtr	setDBSPLPtr = NULL;

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
Free_Transform_SetDBSPL(void)
{
	if (setDBSPLPtr == NULL)
		return(TRUE);
	FreeProcessVariables_Transform_SetDBSPL();
	if (setDBSPLPtr->parList)
		FreeList_UniParMgr(&setDBSPLPtr->parList);
	if (setDBSPLPtr->parSpec == GLOBAL) {
		free(setDBSPLPtr);
		setDBSPLPtr = NULL;
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
Init_Transform_SetDBSPL(ParameterSpecifier parSpec)
{
	static const WChar	*funcName = wxT("Init_Transform_SetDBSPL");

	if (parSpec == GLOBAL) {
		if (setDBSPLPtr != NULL)
			Free_Transform_SetDBSPL();
		if ((setDBSPLPtr = (SetDBSPLPtr) malloc(sizeof(SetDBSPL))) == NULL) {
			NotifyError(wxT("%s: Out of memory for 'global' pointer"),
			  funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (setDBSPLPtr == NULL) {
			NotifyError(wxT("%s:  'local' pointer not set."), funcName);
			return(FALSE);
		}
	}
	setDBSPLPtr->parSpec = parSpec;
	setDBSPLPtr->updateProcessVariablesFlag = TRUE;
	setDBSPLPtr->timeOffsetFlag = TRUE;
	setDBSPLPtr->intensityFlag = TRUE;
	setDBSPLPtr->timeOffset = 0.0;
	setDBSPLPtr->intensity = 0.0;

	if (!SetUniParList_Transform_SetDBSPL()) {
		NotifyError(wxT("%s: Could not initialise parameter list."), funcName);
		Free_Transform_SetDBSPL();
		return(FALSE);
	}
	setDBSPLPtr->scale = 0.0;
	return(TRUE);

}

/********************************* SetUniParList ******************************/

/*
 * This routine initialises and sets the module's universal parameter list.
 * This list provides universal access to the module's parameters.
 */
 
BOOLN
SetUniParList_Transform_SetDBSPL(void)
{
	static const WChar *funcName = wxT("SetUniParList_Transform_SetDBSPL");
	UniParPtr	pars;

	if ((setDBSPLPtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  SETDBSPL_NUM_PARS, NULL)) == NULL) {
		NotifyError(wxT("%s: Could not initialise parList."), funcName);
		return(FALSE);
	}
	pars = setDBSPLPtr->parList->pars;
	SetPar_UniParMgr(&pars[SETDBSPL_TIMEOFFSET], wxT("OFFSET"),
	  wxT("Time offset for intensity calculation (s)."),
	  UNIPAR_REAL,
	  &setDBSPLPtr->timeOffset, NULL,
	  (void * (*)) SetTimeOffset_Transform_SetDBSPL);
	SetPar_UniParMgr(&pars[SETDBSPL_INTENSITY], wxT("DBSPL"),
	  wxT("Required intensity setting (dB SPL)."),
	  UNIPAR_REAL,
	  &setDBSPLPtr->intensity, NULL,
	  (void * (*)) SetIntensity_Transform_SetDBSPL);

	return(TRUE);

}

/********************************* GetUniParListPtr ***************************/

/*
 * This routine returns a pointer to the module's universal parameter list.
 */

UniParListPtr
GetUniParListPtr_Transform_SetDBSPL(void)
{
	static const WChar *funcName = wxT("GetUniParListPtr_Transform_SetDBSPL");

	if (setDBSPLPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (setDBSPLPtr->parList == NULL) {
		NotifyError(wxT("%s: UniParList data structure has not been "
		  "initialised. NULL returned."), funcName);
		return(NULL);
	}
	return(setDBSPLPtr->parList);

}

/****************************** SetPars ***************************************/

/*
 * This function sets all the module's parameters.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetPars_Transform_SetDBSPL(double timeOffset, double intensity)
{
	static const WChar	*funcName = wxT("SetPars_Transform_SetDBSPL");
	BOOLN	ok;

	ok = TRUE;
	if (!SetTimeOffset_Transform_SetDBSPL(timeOffset))
		ok = FALSE;
	if (!SetIntensity_Transform_SetDBSPL(intensity))
		ok = FALSE;
	if (!ok)
		NotifyError(wxT("%s: Failed to set all module parameters.") ,funcName);
	return(ok);

}

/****************************** SetTimeOffset *********************************/

/*
 * This function sets the module's timeOffset parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetTimeOffset_Transform_SetDBSPL(double theTimeOffset)
{
	static const WChar	*funcName = wxT("SetTimeOffset_Transform_SetDBSPL");

	if (setDBSPLPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (theTimeOffset < 0.0) {
		NotifyError(wxT("%s: Illegal offset (%g ms)."), funcName,
		  MSEC(theTimeOffset));
		return(FALSE);
	}
	setDBSPLPtr->timeOffsetFlag = TRUE;
	setDBSPLPtr->timeOffset = theTimeOffset;
	return(TRUE);

}

/****************************** SetDBSPL **********************************/

/*
 * This function sets the module's intensity parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetIntensity_Transform_SetDBSPL(double theIntensity)
{
	static const WChar	*funcName = wxT("SetIntensity_Transform_SetDBSPL");

	if (setDBSPLPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	setDBSPLPtr->intensityFlag = TRUE;
	setDBSPLPtr->intensity = theIntensity;
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
CheckPars_Transform_SetDBSPL(void)
{
	static const WChar	*funcName = wxT("CheckPars_Transform_SetDBSPL");
	BOOLN	ok;

	ok = TRUE;
	if (setDBSPLPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (!setDBSPLPtr->timeOffsetFlag) {
		NotifyError(wxT("%s: timeOffset variable not set."), funcName);
		ok = FALSE;
	}
	if (!setDBSPLPtr->intensityFlag) {
		NotifyError(wxT("%s: intensity variable not set."), funcName);
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
PrintPars_Transform_SetDBSPL(void)
{
	static const WChar	*funcName = wxT("PrintPars_Transform_SetDBSPL");

	if (!CheckPars_Transform_SetDBSPL()) {
		NotifyError(wxT("%s: Parameters have not been correctly set."),
		  funcName);
		return(FALSE);
	}
	DPrint(wxT("Set Intensity Transform Module Parameters:-\n"));
	DPrint(wxT("\tTime offset = %g ms,"), MSEC(setDBSPLPtr->timeOffset));
	DPrint(wxT("\tIntensity = %g dB SPL\n"), setDBSPLPtr->intensity);
	return(TRUE);

}

/****************************** ReadPars **************************************/

/*
 * This program reads a specified number of parameters from a file.
 * It returns FALSE if it fails in any way.n */

BOOLN
ReadPars_Transform_SetDBSPL(WChar *fileName)
{
	static const WChar	*funcName = wxT("ReadPars_Transform_SetDBSPL");
	BOOLN	ok;
	WChar	*filePath;
	double	timeOffset, intensity;
	FILE	*fp;

	filePath = GetParsFileFPath_Common(fileName);
	if ((fp = DSAM_fopen(filePath, "r")) == NULL) {
		NotifyError(wxT("%s: Cannot open data file '%s'.\n"), funcName,
		  filePath);
		return(FALSE);
	}
	DPrint(wxT("%s: Reading from '%s':\n"), funcName, filePath);
	Init_ParFile();
	ok = TRUE;
	if (!GetPars_ParFile(fp, wxT("%lf"), &timeOffset))
		ok = FALSE;
	if (!GetPars_ParFile(fp, wxT("%lf"), &intensity))
		ok = FALSE;
	fclose(fp);
	Free_ParFile();
	if (!ok) {
		NotifyError(wxT("%s: Not enough lines, or invalid parameters, in "
		  "module parameter file '%s'."), funcName, filePath);
		return(FALSE);
	}
	if (!SetPars_Transform_SetDBSPL(timeOffset, intensity)) {
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
SetParsPointer_Transform_SetDBSPL(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("SetParsPointer_Transform_SetDBSPL");

	if (!theModule) {
		NotifyError(wxT("%s: The module is not set."), funcName);
		return(FALSE);
	}
	setDBSPLPtr = (SetDBSPLPtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 */

BOOLN
InitModule_Transform_SetDBSPL(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("InitModule_Transform_SetDBSPL");

	if (!SetParsPointer_Transform_SetDBSPL(theModule)) {
		NotifyError(wxT("%s: Cannot set parameters pointer."), funcName);
		return(FALSE);
	}
	if (!Init_Transform_SetDBSPL(GLOBAL)) {
		NotifyError(wxT("%s: Could not initialise process structure."),
		  funcName);
		return(FALSE);
	}
	theModule->parsPtr = setDBSPLPtr;
	theModule->CheckPars = CheckPars_Transform_SetDBSPL;
	theModule->Free = Free_Transform_SetDBSPL;
	theModule->GetUniParListPtr = GetUniParListPtr_Transform_SetDBSPL;
	theModule->PrintPars = PrintPars_Transform_SetDBSPL;
	theModule->ReadPars = ReadPars_Transform_SetDBSPL;
	theModule->RunProcess = Process_Transform_SetDBSPL;
	theModule->SetParsPointer = SetParsPointer_Transform_SetDBSPL;
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
CheckData_Transform_SetDBSPL(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("CheckData_Transform_SetDBSPL");

	if (data == NULL) {
		NotifyError(wxT("%s: EarObject not initialised."), funcName);
		return(FALSE);
	}
	if (!CheckInSignal_EarObject(data, funcName))
		return(FALSE);
	if (setDBSPLPtr->timeOffset >=
	  _GetDuration_SignalData(data->inSignal[0])) {
		NotifyError(wxT("%s: Time offset (%g ms) is longer than signal "
		  "duration (%g ms)."), funcName, MSEC(setDBSPLPtr->timeOffset),
		  MSEC(_GetDuration_SignalData(data->inSignal[0])));
		return(FALSE);
	}
	/*** Put additional checks here. ***/
	return(TRUE);

}

/**************************** InitProcessVariables ****************************/

/*
 * This function allocates the memory for the process variables if necessary
 * at sets the initial process values as required.
 * It assumes that all of the parameters for the module have been correctly
 * initialised.
 */

BOOLN
InitProcessVariables_Transform_SetDBSPL(EarObjectPtr data)
{
	register ChanData	 *inPtr, sum;
	int		chan;
	double	gaindB;
	ChanLen	i, timeOffsetIndex;

	if (setDBSPLPtr->updateProcessVariablesFlag || data->updateProcessFlag ||
	  (data->timeIndex == PROCESS_START_TIME)) {
		setDBSPLPtr->updateProcessVariablesFlag = FALSE;
		timeOffsetIndex = (ChanLen) (setDBSPLPtr->timeOffset /
		  data->inSignal[0]->dt + 0.5);
		for (chan = 0, sum = 0.0; chan < data->inSignal[0]->numChannels;
		  chan++) {
			inPtr = data->inSignal[0]->channel[chan] + timeOffsetIndex;
			for (i = timeOffsetIndex; i < data->inSignal[0]->length; i++,
			  inPtr++)
				sum += *inPtr * *inPtr;
		}
		gaindB = setDBSPLPtr->intensity - DB_SPL(sqrt(sum /
		  (data->inSignal[0]->length - timeOffsetIndex) /
		  data->inSignal[0]->numChannels));
		setDBSPLPtr->scale = pow(10.0, gaindB / 20.0);
	}
	return(TRUE);

}

/**************************** FreeProcessVariables ****************************/

/*
 * This routine releases the memory allocated for the process variables
 * if they have been initialised.
 */

BOOLN
FreeProcessVariables_Transform_SetDBSPL(void)
{
	setDBSPLPtr->updateProcessVariablesFlag = TRUE;
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
Process_Transform_SetDBSPL(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("Process_Transform_SetDBSPL");
	register	ChanData	 *inPtr;
	int		chan;
	ChanLen	i;

	if (!data->threadRunFlag) {
		if (!CheckPars_Transform_SetDBSPL())
			return(FALSE);
		if (!CheckData_Transform_SetDBSPL(data)) {
			NotifyError(wxT("%s: Process data invalid."), funcName);
			return(FALSE);
		}
		SetProcessName_EarObject(data, wxT("Set Intensity Module process"));
		if (_OutSig_EarObject(data) != data->inSignal[0]) {
			_OutSig_EarObject(data) = data->inSignal[0];
			data->updateCustomersFlag = TRUE;
		}
		if (!InitProcessVariables_Transform_SetDBSPL(data)) {
			NotifyError(wxT("%s: Could not initialise the process variables."),
			  funcName);
			return(FALSE);
		}
		if (data->initThreadRunFlag)
			return(TRUE);
	}
	for (chan = 0; chan < data->inSignal[0]->numChannels; chan++) {
		inPtr = data->inSignal[0]->channel[chan];
		for (i = 0; i < data->inSignal[0]->length; i++)
			*inPtr++ *= setDBSPLPtr->scale;
	}
	SetProcessContinuity_EarObject(data);
	return(TRUE);

}

