/**********************
 *
 * File:		AnIntensity.c
 * Purpose:		This module calculates the intensity for a signal, starting
 *				from an offset position.
 * Comments:	Written using ModuleProducer version 1.9 (May 27 1996).
 * Author:		L. P. O'Mard
 * Created:		12 Jun 1996
 * Updated:	
 * Copyright:	(c) 1998, University of Essex.
 *
 *********************/

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG_H */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "GeCommon.h"
#include "GeSignalData.h"
#include "GeEarObject.h"
#include "GeUniParMgr.h"
#include "GeModuleMgr.h"
#include "FiParFile.h"
#include "AnIntensity.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

IntensityPtr	intensityPtr = NULL;

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
Free_Analysis_Intensity(void)
{
	/* static const char	*funcName = "Free_Analysis_Intensity";  */

	if (intensityPtr == NULL)
		return(FALSE);
	if (intensityPtr->parList)
		FreeList_UniParMgr(&intensityPtr->parList);
	if (intensityPtr->parSpec == GLOBAL) {
		free(intensityPtr);
		intensityPtr = NULL;
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
Init_Analysis_Intensity(ParameterSpecifier parSpec)
{
	static const char	*funcName = "Init_Analysis_Intensity";

	if (parSpec == GLOBAL) {
		if (intensityPtr != NULL)
			Free_Analysis_Intensity();
		if ((intensityPtr = (IntensityPtr) malloc(sizeof(Intensity))) == NULL) {
			NotifyError("%s: Out of memory for 'global' pointer", funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (intensityPtr == NULL) {
			NotifyError("%s:  'local' pointer not set.", funcName);
			return(FALSE);
		}
	}
	intensityPtr->parSpec = parSpec;
	intensityPtr->timeOffsetFlag = TRUE;
	intensityPtr->extentFlag = TRUE;
	intensityPtr->timeOffset = 2.5e-3;
	intensityPtr->extent = -1.0;

	if (!SetUniParList_Analysis_Intensity()) {
		NotifyError("%s: Could not initialise parameter list.", funcName);
		Free_Analysis_Intensity();
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
SetUniParList_Analysis_Intensity(void)
{
	static const char *funcName = "SetUniParList_Analysis_Intensity";
	UniParPtr	pars;

	if ((intensityPtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  ANALYSIS_INTENSITY_NUM_PARS, NULL)) == NULL) {
		NotifyError("%s: Could not initialise parList.", funcName);
		return(FALSE);
	}
	pars = intensityPtr->parList->pars;
	SetPar_UniParMgr(&pars[ANALYSIS_INTENSITY_TIMEOFFSET], "OFFSET",
	  "Time from which to start calculation (s).",
	  UNIPAR_REAL,
	  &intensityPtr->timeOffset, NULL,
	  (void * (*)) SetTimeOffset_Analysis_Intensity);
	SetPar_UniParMgr(&pars[ANALYSIS_INTENSITY_EXTENT], "EXTENT",
	  "Time over which calculation is performed: -ve value assumes end of "
	  "signal (s).",
	  UNIPAR_REAL,
	  &intensityPtr->extent, NULL,
	  (void * (*)) SetExtent_Analysis_Intensity);
	return(TRUE);

}

/****************************** GetUniParListPtr ******************************/

/*
 * This function returns a pointer to the module's universal parameter
 * list.
 */

UniParListPtr
GetUniParListPtr_Analysis_Intensity(void)
{
	static const char	*funcName = "GetUniParListPtr_Analysis_Intensity";

	if (intensityPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (intensityPtr->parList == NULL) {
		NotifyError("%s: UniParList data structure has not been initialised. "
		  "NULL returned.", funcName);
		return(NULL);
	}
	return(intensityPtr->parList);

}

/****************************** SetPars ***************************************/

/*
 * This function sets all the module's parameters.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetPars_Analysis_Intensity(double timeOffset)
{
	static const char	*funcName = "SetPars_Analysis_Intensity";
	BOOLN	ok;

	ok = TRUE;
	if (!SetTimeOffset_Analysis_Intensity(timeOffset))
		ok = FALSE;
	if (!ok)
		NotifyError("%s: Failed to set all module parameters." ,funcName);
	return(ok);

}

/****************************** SetTimeOffset *********************************/

/*
 * This function sets the module's timeOffset parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetTimeOffset_Analysis_Intensity(double theTimeOffset)
{
	static const char	*funcName = "SetTimeOffset_Analysis_Intensity";

	if (intensityPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (theTimeOffset < 0.0) {
		NotifyError("%s: Illegal offset (%g ms).", funcName, MSEC(
		  theTimeOffset));
		return(FALSE);
	}
	intensityPtr->timeOffsetFlag = TRUE;
	intensityPtr->timeOffset = theTimeOffset;
	return(TRUE);

}

/****************************** SetExtent *************************************/

/*
 * This function sets the module's extent parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetExtent_Analysis_Intensity(double theExtent)
{
	static const char	*funcName = "SetExtent_Analysis_Intensity";

	if (intensityPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	intensityPtr->extentFlag = TRUE;
	intensityPtr->extent = theExtent;
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
CheckPars_Analysis_Intensity(void)
{
	static const char	*funcName = "CheckPars_Analysis_Intensity";
	BOOLN	ok;

	ok = TRUE;
	if (intensityPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (!intensityPtr->timeOffsetFlag) {
		NotifyError("%s: timeOffset variable not set.", funcName);
		ok = FALSE;
	}
	if (!intensityPtr->extentFlag) {
		NotifyError("%s: extent variable not set.", funcName);
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
PrintPars_Analysis_Intensity(void)
{
	static const char	*funcName = "PrintPars_Analysis_Intensity";

	if (!CheckPars_Analysis_Intensity()) {
		NotifyError("%s: Parameters have not been correctly set.", funcName);
		return(FALSE);
	}
	DPrint("Intensity Analysis Module Parameters:-\n");
	DPrint("\tTime offset = %g ms,", MSEC(intensityPtr->timeOffset));
	DPrint("\tTime extent = ");
	if (intensityPtr->extent < 0.0)
		DPrint("<end of signal>");
	else
		DPrint("%g ms\n", MSEC(intensityPtr->extent));
	DPrint(".\n");
	return(TRUE);

}

/****************************** ReadPars **************************************/

/*
 * This program reads a specified number of parameters from a file.
 * It returns FALSE if it fails in any way.n */

BOOLN
ReadPars_Analysis_Intensity(char *fileName)
{
	static const char	*funcName = "ReadPars_Analysis_Intensity";
	BOOLN	ok;
	char	*filePath;
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
	if (!GetPars_ParFile(fp, "%lf", &timeOffset))
		ok = FALSE;
	fclose(fp);
	Free_ParFile();
	if (!ok) {
		NotifyError("%s: Not enough lines, or invalid parameters, in module "
		  "parameter file '%s'.", funcName, filePath);
		return(FALSE);
	}
	if (!SetPars_Analysis_Intensity(timeOffset)) {
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
SetParsPointer_Analysis_Intensity(ModulePtr theModule)
{
	static const char	*funcName = "SetParsPointer_Analysis_Intensity";

	if (!theModule) {
		NotifyError("%s: The module is not set.", funcName);
		return(FALSE);
	}
	intensityPtr = (IntensityPtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 */

BOOLN
InitModule_Analysis_Intensity(ModulePtr theModule)
{
	static const char	*funcName = "InitModule_Analysis_Intensity";

	if (!SetParsPointer_Analysis_Intensity(theModule)) {
		NotifyError("%s: Cannot set parameters pointer.", funcName);
		return(FALSE);
	}
	if (!Init_Analysis_Intensity(GLOBAL)) {
		NotifyError("%s: Could not initialise process structure.", funcName);
		return(FALSE);
	}
	theModule->parsPtr = intensityPtr;
	theModule->CheckPars = CheckPars_Analysis_Intensity;
	theModule->Free = Free_Analysis_Intensity;
	theModule->GetUniParListPtr = GetUniParListPtr_Analysis_Intensity;
	theModule->PrintPars = PrintPars_Analysis_Intensity;
	theModule->ReadPars = ReadPars_Analysis_Intensity;
	theModule->RunProcess = Calc_Analysis_Intensity;
	theModule->SetParsPointer = SetParsPointer_Analysis_Intensity;
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
CheckData_Analysis_Intensity(EarObjectPtr data)
{
	static const char	*funcName = "CheckData_Analysis_Intensity";
	double	duration;

	if (data == NULL) {
		NotifyError("%s: EarObject not initialised.", funcName);
		return(FALSE);
	}
	if (!CheckInSignal_EarObject(data, funcName))
		return(FALSE);
	duration =  _GetDuration_SignalData(data->inSignal[0]);
	if (intensityPtr->timeOffset >= duration) {
		NotifyError("%s: Time offset (%g ms) is longer than signal duration "
		  "(%g ms).", funcName, MSEC(intensityPtr->timeOffset), MSEC(duration));
		return(FALSE);
	}
	if (intensityPtr->extent > 0.0) {
		if ((intensityPtr->timeOffset + intensityPtr->extent) > duration) {
			NotifyError("%s: Time offset (%g ms) + extent (%g ms) is longer "
			  "than signal duration (%g).", funcName,  MSEC(
			  intensityPtr->timeOffset), MSEC(intensityPtr->extent), MSEC(
			  duration));
			return(FALSE);
		}
		if (intensityPtr->extent < data->inSignal[0]->dt) {
			NotifyError("%s: Time extent is too small (%g ms).  It should be "
			  "greater than the sampling interval (%g ms).", funcName, MSEC(
			  intensityPtr->extent), MSEC(data->inSignal[0]->dt));
			return(FALSE);
		}
	}
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
Calc_Analysis_Intensity(EarObjectPtr data)
{
	static const char	*funcName = "Calc_Analysis_Intensity";
	register	ChanData	 *inPtr, sum;
	int		chan;
	ChanLen	i, timeOffsetIndex, extent;

	if (!CheckPars_Analysis_Intensity())
		return(FALSE);
	if (!CheckData_Analysis_Intensity(data)) {
		NotifyError("%s: Process data invalid.", funcName);
		return(FALSE);
	}
	SetProcessName_EarObject(data, "Intensity Analysis Module");
	if (!InitOutSignal_EarObject(data, data->inSignal[0]->numChannels, 1,
	  1.0)) {
		NotifyError("%s: Cannot initialise output channels.", funcName);
		return(FALSE);
	}
	timeOffsetIndex = (ChanLen) (intensityPtr->timeOffset /
	  data->inSignal[0]->dt + 0.5);
	extent = (intensityPtr->extent < 0.0)? data->inSignal[0]->length -
	  timeOffsetIndex: (ChanLen) (intensityPtr->extent / data->inSignal[0]->dt +
	  0.5);
	for (chan = 0; chan < data->inSignal[0]->numChannels; chan++) {
		inPtr = data->inSignal[0]->channel[chan] + timeOffsetIndex;
		for (i = 0, sum = 0.0; i < extent; i++, inPtr++)
			sum += *inPtr * *inPtr;
		data->outSignal->channel[chan][0] = (ChanData) DB_SPL(sqrt(sum /
		  extent));
	}
	SetProcessContinuity_EarObject(data);
	return(TRUE);

}

