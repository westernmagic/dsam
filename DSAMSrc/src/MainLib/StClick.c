/**********************
 *
 * File:		StClick.c
 * Purpose:		This module contains the methods for the Click stimulus
 *				generation paradigm. 
 * Comments:	
 * Author:		L. P. O'Mard
 * Created:		18 Feb 1993
 * Updated:		12 Mar 1997
 * Copyright:	(c) 1997,  University of Essex
 *
 **********************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG_H */

#include "GeCommon.h"
#include "GeSignalData.h"
#include "GeEarObject.h"
#include "GeUniParMgr.h"
#include "GeModuleMgr.h"
#include "StClick.h"
#include "FiParFile.h"

/******************************************************************************/
/********************************* Global variables ***************************/
/******************************************************************************/

ClickPtr		clickPtr = NULL;

/******************************************************************************/
/********************************* Subroutines and functions ******************/
/******************************************************************************/

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
Init_Click(ParameterSpecifier parSpec)
{
	static const char *funcName = "Init_Click";

	if (parSpec == GLOBAL) {
		if (clickPtr != NULL)
			Free_Click();
		if ((clickPtr = (ClickPtr) malloc(sizeof(Click))) == NULL) {
			NotifyError("%s: Out of memory for 'global' pointer", funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (clickPtr == NULL) { 
			NotifyError("%s:  'local' pointer not set.", funcName);
			return(FALSE);
		}
	}
	clickPtr->parSpec = parSpec;
	clickPtr->clickTimeFlag = FALSE;
	clickPtr->amplitudeFlag = FALSE;
	clickPtr->durationFlag = FALSE;
	clickPtr->dtFlag = FALSE;
	clickPtr->clickTime = 0.0;
	clickPtr->amplitude = 0.0;
	clickPtr->duration = 0.0;
	clickPtr->dt = 0.0;

	if (!SetUniParList_Click()) {
		NotifyError("%s: Could not initialise parameter list.", funcName);
		Free_Click();
		return(FALSE);
	}
	return(TRUE);

}

/********************************* Free ***************************************/

/*
 * This function releases of the memory allocated for the process variables.
 * It should be called when the module is no longer in use.
 * It is defined as returning a BOOLN value because the generic module
 * interface requires that a non-void value be returned.
 */

BOOLN
Free_Click(void)
{
	if (clickPtr == NULL)
		return(TRUE);
	if (clickPtr->parList)
		FreeList_UniParMgr(&clickPtr->parList);
	if (clickPtr->parSpec == GLOBAL) {
		free(clickPtr);
		clickPtr = NULL;
	}
	return(TRUE);

}

/********************************* SetUniParList ******************************/

/*
 * This routine initialises and sets the module's universal parameter list.
 * This list provides universal access to the module's parameters.
 */
 
BOOLN
SetUniParList_Click(void)
{
	static const char *funcName = "SetUniParList_Click";
	UniParPtr	pars;

	if ((clickPtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  CLICK_NUM_PARS, NULL)) == NULL) {
		NotifyError("%s: Could not initialise parList.", funcName);
		return(FALSE);
	}
	pars = clickPtr->parList->pars;
	SetPar_UniParMgr(&pars[CLICK_CLICKTIME], "TIME",
	  "Time for the delta-function click (s).",
	  UNIPAR_REAL,
	  &clickPtr->clickTime, NULL,
	  (void * (*)) SetClickTime_Click);
	SetPar_UniParMgr(&pars[CLICK_AMPLITUDE], "AMPLITUDE",
	  "Amplitude (uPa).",
	  UNIPAR_REAL,
	  &clickPtr->amplitude, NULL,
	  (void * (*)) SetAmplitude_Click);
	SetPar_UniParMgr(&pars[CLICK_DURATION], "DURATION",
	  "Duration (s).",
	  UNIPAR_REAL,
	  &clickPtr->duration, NULL,
	  (void * (*)) SetDuration_Click);
	SetPar_UniParMgr(&pars[CLICK_SAMPLINGINTERVAL], "DT",
	  "Sampling interval, dt (s).",
	  UNIPAR_REAL,
	  &clickPtr->dt, NULL,
	  (void * (*)) SetSamplingInterval_Click);
	return(TRUE);

}

/********************************* GetUniParListPtr ***************************/

/*
 * This routine returns a pointer to the module's universal parameter list.
 */

UniParListPtr
GetUniParListPtr_Click(void)
{
	static const char *funcName = "GetUniParListPtr_Click";

	if (clickPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (clickPtr->parList == NULL) {
		NotifyError("%s: UniParList data structure has not been initialised. "
		  "NULL returned.", funcName);
		return(NULL);
	}
	return(clickPtr->parList);

}

/********************************* CheckPars **********************************/

/*
 * This routine checks that the necessary parameters for the module have been
 * correctly initialised.
 * It returns TRUE if there are no problems.
 */
 
BOOLN
CheckPars_Click(void)
{
	static const char *funcName = "CheckPars_Click";
	BOOLN	ok;
	double	remainder;
	
	ok = TRUE;
	if (clickPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (!clickPtr->clickTimeFlag) {
		NotifyError("%s: clickTime variable not set.", funcName);
		ok = FALSE;
	}
	if (!clickPtr->amplitudeFlag) {
		NotifyError("%s: amplitude variable not set.", funcName);
		ok = FALSE;
	}
	if (!clickPtr->durationFlag) {
		NotifyError("%s: duration variable not set.", funcName);
		ok = FALSE;
	}
	if (!clickPtr->dtFlag) {
		NotifyError("%s: dt (sampling interval) variable not set.", funcName);
		ok = FALSE;
	}
	if (clickPtr->clickTime >= clickPtr->duration) {
		NotifyError("%s: Click time must be less then the signal duration.",
		  funcName);
		ok = FALSE;
	}
	remainder = fmod(clickPtr->clickTime, clickPtr->dt);
	if ((remainder > DBL_EPSILON) && (fabs(remainder - clickPtr->dt) >
	  DBL_EPSILON)) {
		NotifyError("%s: The click time should be a multiple of the sampling "
		  "interval (%g ms)", funcName, MILLI(clickPtr->dt));
		ok = FALSE;
	}
	return(ok);
}	

/********************************* SetClickTime *******************************/

/*
 * This function sets the module's clickTime parameter.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetClickTime_Click(double theClickTime)
{
	static const char *funcName = "SetClickTime_Click";

	if (clickPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	clickPtr->clickTimeFlag = TRUE;
	clickPtr->clickTime = theClickTime;
	return(TRUE);

}

/********************************* SetAmplitude *******************************/

/*
 * This function sets the module's amplitude parameter.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetAmplitude_Click(double theAmplitude)
{
	static const char *funcName = "SetAmplitude_Click";

	if (clickPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	clickPtr->amplitudeFlag = TRUE;
	clickPtr->amplitude = theAmplitude;
	return(TRUE);

}

/********************************* SetDuration ********************************/

/*
 * This function sets the module's duration parameter.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetDuration_Click(double theDuration)
{
	static const char *funcName = "SetDuration_Click";

	if (clickPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	clickPtr->durationFlag = TRUE;
	clickPtr->duration = theDuration;
	return(TRUE);

}

/********************************* SetSamplingInterval ************************/

/*
 * This function sets the module's samplingInterval parameter.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetSamplingInterval_Click(double theSamplingInterval)
{
	static const char *funcName = "SetSamplingInterval_Click";

	if (clickPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if ( theSamplingInterval <= 0.0 ) {
		NotifyError("%s: Illegal sampling interval value = %g!", funcName,
		  theSamplingInterval);
		return(FALSE);
	}
	clickPtr->dtFlag = TRUE;
	clickPtr->dt = theSamplingInterval;
	return(TRUE);

}

/********************************* SetPars ************************************/

/*
 * This function sets all the module's parameters.
 * It returns TRUE if the operation is successful.
 */
 
BOOLN
SetPars_Click(double theClickTime, double theAmplitude, double theDuration,
  double theSamplingInterval)
{
	static const char *funcName = "SetPars_Click";
	BOOLN	ok;
	
	ok = TRUE;
	if (!SetClickTime_Click(theClickTime))
		ok = FALSE;
	if (!SetAmplitude_Click(theAmplitude))
		ok = FALSE;
	if (!SetDuration_Click(theDuration))
		ok = FALSE;
	if (!SetSamplingInterval_Click(theSamplingInterval))
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
PrintPars_Click(void)
{
	static const char *funcName = "PrintPars_Click";

	if (!CheckPars_Click()) {
		NotifyError("%s: Parameters have not been correctly set.", funcName);
		return(FALSE);
	}
	DPrint("Click Module Parameters:-\n");
	DPrint("\tClick time = %g ms,\tAmplitude = %g uPa,\n",
	  MSEC(clickPtr->clickTime), clickPtr->amplitude);
	DPrint("\tDuration = %g ms,\tSampling interval = %g ms\n",
	  MSEC(clickPtr->duration), MSEC(clickPtr->dt));
	return(TRUE);

}

/****************************** ReadPars **************************************/

/*
 * This program reads a specified number of parameters from a file.
 * It returns FALSE if it fails in any way.
 */
 
BOOLN
ReadPars_Click(char *fileName)
{
	static const char *funcName = "ReadPars_Click";
	BOOLN	ok;
	char	*filePath;
	double  amplitude, duration, samplingInterval, clickTime;
    FILE    *fp;
    
	filePath = GetParsFileFPath_Common(fileName);
    if ((fp = fopen(filePath, "r")) == NULL) {
        NotifyError("%s: Cannot open data file '%s'.\n", funcName, filePath);
		return(FALSE);
    }
    DPrint("%s: Reading from '%s':\n", funcName, filePath);
    Init_ParFile();
	ok = TRUE;
    if (!GetPars_ParFile(fp, "%lf", &clickTime))
		ok = FALSE;
    if (!GetPars_ParFile(fp, "%lf", &amplitude))
		ok = FALSE;
    if (!GetPars_ParFile(fp, "%lf", &duration))
		ok = FALSE;
    if (!GetPars_ParFile(fp, "%lf", &samplingInterval))
		ok = FALSE;
    fclose(fp);
    Free_ParFile();
	if (!ok) {
		NotifyError("%s:  Not enough lines, or invalid parameters, in "\
		  "module parameter file '%s'.", funcName, filePath);
		return(FALSE);
	}
	if (!SetPars_Click(clickTime, amplitude, duration,
	  samplingInterval)) {
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
SetParsPointer_Click(ModulePtr theModule)
{
	static const char	*funcName = "SetParsPointer_Click";

	if (!theModule) {
		NotifyError("%s: The module is not set.", funcName);
		return(FALSE);
	}
	clickPtr = (ClickPtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 */

BOOLN
InitModule_Click(ModulePtr theModule)
{
	static const char	*funcName = "InitModule_Click";

	if (!SetParsPointer_Click(theModule)) {
		NotifyError("%s: Cannot set parameters pointer.", funcName);
		return(FALSE);
	}
	if (!Init_Click(GLOBAL)) {
		NotifyError("%s: Could not initialise process structure.", funcName);
		return(FALSE);
	}
	theModule->parsPtr = clickPtr;
	theModule->CheckPars = CheckPars_Click;
	theModule->Free = Free_Click;
	theModule->GetUniParListPtr = GetUniParListPtr_Click;
	theModule->PrintPars = PrintPars_Click;
	theModule->ReadPars = ReadPars_Click;
	theModule->RunProcess = GenerateSignal_Click;
	theModule->SetParsPointer = SetParsPointer_Click;
	return(TRUE);

}

/********************************* GenerateSignal *****************************/

/*
 * This routine allocates memory for the output signal, if necessary, and
 * generates the signal into channel[0] of the signal data-set.
 * It checks that all initialisation has been correctly carried out by calling
 * the appropriate checking routines.
 * It can be called repeatedly with different parameter values if required.
 * Stimulus generation only sets the output signal, the input signal is not
 * used.
 */

BOOLN
GenerateSignal_Click(EarObjectPtr data)
{
	static const char *funcName = "GenerateSignal_Click";
	ChanLen		timeIndex;
	ChanData	*dataPtr;

	if (data == NULL) {
		NotifyError("%s: EarObject not initialised.", funcName);
		return(FALSE);
	}	
	if (!CheckPars_Click())
		return(FALSE);
	SetProcessName_EarObject(data, "Click (delta-function) stimulus");
	if (!InitOutSignal_EarObject(data, 1, (ChanLen) (clickPtr->duration /
		clickPtr->dt + 0.5), clickPtr->dt)) {
		NotifyError("%s: Cannot initialise output channels.", funcName);
		return(FALSE);
	}
	dataPtr = data->outSignal->channel[0];
	/* The channel is initialised to zero by InitOutSignal_EarObject. */
	timeIndex = (ChanLen) floor(clickPtr->clickTime / clickPtr->dt + 0.5) - 1;
	dataPtr[timeIndex] = clickPtr->amplitude;
	data->outSignal->rampFlag = TRUE;	/* Clicks cannot really be ramped. */
	SetProcessContinuity_EarObject(data);
	return(TRUE);

} /* GenerateSignal_Click */

