/**********************
 *
 * File:		UtBinSignal.c
 * Purpose:		This routine generates a binned histogram from the EarObject's
 *				input signal.
 * Comments:	Written using ModuleProducer version 1.9 (Feb 29 1996).
 *				It adds to previous data if the output signal has already been
 *				initialised and the data->updateProcessFlag is set to FALSE.
 *				Otherwise it will overwrite the old signal or create a new
 *				signal as required.
 *				The data->updateProcessFlag facility is useful for repeated
 *				runs.
 * Author:		L. P. O'Mard
 * Created:		21 Mar 1996
 * Updated:		23 Jan 1997
 * Copyright:	(c) 1998, University of Essex
 *
 *********************/

#include <stdio.h>
#include <stdlib.h>
#include <float.h>
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
#include "UtBinSignal.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

BinSignalPtr	binSignalPtr = NULL;

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
Free_Utility_BinSignal(void)
{
	/* static const char	*funcName = "Free_Utility_BinSignal"; */

	if (binSignalPtr == NULL)
		return(FALSE);
	if (binSignalPtr->parList)
		FreeList_UniParMgr(&binSignalPtr->parList);
	if (binSignalPtr->parSpec == GLOBAL) {
		free(binSignalPtr);
		binSignalPtr = NULL;
	}
	return(TRUE);

}

/****************************** InitModeList **********************************/

/*
 * This function initialises the 'mode' list array
 */

BOOLN
InitModeList_Utility_BinSignal(void)
{
	static NameSpecifier	modeList[] = {

			{ "AVERAGE",	UTILITY_BINSIGNAL_AVERAGE_MODE },
			{ "SUM",		UTILITY_BINSIGNAL_SUM_MODE },
			{ "",	UTILITY_BINSIGNAL_MODE_NULL },
		};
	binSignalPtr->modeList = modeList;
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
Init_Utility_BinSignal(ParameterSpecifier parSpec)
{
	static const char	*funcName = "Init_Utility_BinSignal";

	if (parSpec == GLOBAL) {
		if (binSignalPtr != NULL)
			Free_Utility_BinSignal();
		if ((binSignalPtr = (BinSignalPtr) malloc(sizeof(BinSignal))) == NULL) {
			NotifyError("%s: Out of memory for 'global' pointer", funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (binSignalPtr == NULL) {
			NotifyError("%s:  'local' pointer not set.", funcName);
			return(FALSE);
		}
	}
	binSignalPtr->parSpec = parSpec;
	binSignalPtr->modeFlag = FALSE;
	binSignalPtr->binWidthFlag = FALSE;
	binSignalPtr->mode = 0;
	binSignalPtr->binWidth = 0.0;

	InitModeList_Utility_BinSignal();
	if (!SetUniParList_Utility_BinSignal()) {
		NotifyError("%s: Could not initialise parameter list.", funcName);
		Free_Utility_BinSignal();
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
SetUniParList_Utility_BinSignal(void)
{
	static const char *funcName = "SetUniParList_Utility_BinSignal";
	UniParPtr	pars;

	if ((binSignalPtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  UTILITY_BINSIGNAL_NUM_PARS, NULL)) == NULL) {
		NotifyError("%s: Could not initialise parList.", funcName);
		return(FALSE);
	}
	pars = binSignalPtr->parList->pars;
	SetPar_UniParMgr(&pars[UTILITY_BINSIGNAL_MODE], "MODE",
	  "Bining mode ('average' or 'sum').",
	  UNIPAR_NAME_SPEC,
	  &binSignalPtr->mode, binSignalPtr->modeList,
	  (void * (*)) SetMode_Utility_BinSignal);
	SetPar_UniParMgr(&pars[UTILITY_BINSIGNAL_BINWIDTH], "BIN_WIDTH",
	  "Bin width for binned signal (s).",
	  UNIPAR_REAL,
	  &binSignalPtr->binWidth, NULL,
	  (void * (*)) SetBinWidth_Utility_BinSignal);
	return(TRUE);

}

/****************************** GetUniParListPtr ******************************/

/*
 * This function returns a pointer to the module's universal parameter
 * list.
 */

UniParListPtr
GetUniParListPtr_Utility_BinSignal(void)
{
	static const char	*funcName = "GetUniParListPtr_Utility_BinSignal";

	if (binSignalPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (binSignalPtr->parList == NULL) {
		NotifyError("%s: UniParList data structure has not been initialised. "
		  "NULL returned.", funcName);
		return(NULL);
	}
	return(binSignalPtr->parList);

}

/****************************** SetPars ***************************************/

/*
 * This function sets all the module's parameters.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetPars_Utility_BinSignal(char * mode, double binWidth)
{
	static const char	*funcName = "SetPars_Utility_BinSignal";
	BOOLN	ok;

	ok = TRUE;
	if (!SetMode_Utility_BinSignal(mode))
		ok = FALSE;
	if (!SetBinWidth_Utility_BinSignal(binWidth))
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
SetMode_Utility_BinSignal(char * theMode)
{
	static const char	*funcName = "SetMode_Utility_BinSignal";
	int		specifier;

	if (binSignalPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theMode, binSignalPtr->modeList)) ==
	   UTILITY_BINSIGNAL_MODE_NULL) {
		NotifyError("%s: Illegal name (%s).", funcName, theMode);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	binSignalPtr->modeFlag = TRUE;
	binSignalPtr->mode = specifier;
	return(TRUE);

}

/****************************** SetBinWidth ***********************************/

/*
 * This function sets the module's binWidth parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetBinWidth_Utility_BinSignal(double theBinWidth)
{
	static const char	*funcName = "SetBinWidth_Utility_BinSignal";

	if (binSignalPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (theBinWidth == 0.0) {
		NotifyError("%s: Illegal bin width (%g ms).", funcName,
		  MSEC(theBinWidth));
		return(FALSE);
	}
	binSignalPtr->binWidthFlag = TRUE;
	binSignalPtr->binWidth = theBinWidth;
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
CheckPars_Utility_BinSignal(void)
{
	static const char	*funcName = "CheckPars_Utility_BinSignal";
	BOOLN	ok;

	ok = TRUE;
	if (binSignalPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (!binSignalPtr->modeFlag) {
		NotifyError("%s: mode variable not set.", funcName);
		ok = FALSE;
	}
	if (!binSignalPtr->binWidthFlag) {
		NotifyError("%s: binWidth variable not set.", funcName);
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
PrintPars_Utility_BinSignal(void)
{
	static const char	*funcName = "PrintPars_Utility_BinSignal";

	if (!CheckPars_Utility_BinSignal()) {
		NotifyError("%s: Parameters have not been correctly set.", funcName);
		return(FALSE);
	}
	DPrint("Bin Signal Utility Module Parameters:-\n");
	DPrint("\tBining mode = %s, \t", binSignalPtr->modeList[
	  binSignalPtr->mode].name);
	DPrint("\tBin width = ");
	if (binSignalPtr->binWidth < 0.0)
		DPrint("signal/segment duration.\n");
	else
		DPrint("%g (ms).\n", MSEC(binSignalPtr->binWidth));
	return(TRUE);

}

/****************************** ReadPars **************************************/

/*
 * This program reads a specified number of parameters from a file.
 * It returns FALSE if it fails in any way.n */

BOOLN
ReadPars_Utility_BinSignal(char *fileName)
{
	static const char	*funcName = "ReadPars_Utility_BinSignal";
	BOOLN	ok;
	char	*filePath, mode[MAXLINE];
	double	binWidth;
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
	if (!GetPars_ParFile(fp, "%lf", &binWidth))
		ok = FALSE;
	fclose(fp);
	Free_ParFile();
	if (!ok) {
		NotifyError("%s: Not enough lines, or invalid parameters, in module "
		  "parameter file '%s'.", funcName, filePath);
		return(FALSE);
	}
	if (!SetPars_Utility_BinSignal(mode, binWidth)) {
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
SetParsPointer_Utility_BinSignal(ModulePtr theModule)
{
	static const char	*funcName = "SetParsPointer_Utility_BinSignal";

	if (!theModule) {
		NotifyError("%s: The module is not set.", funcName);
		return(FALSE);
	}
	binSignalPtr = (BinSignalPtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 */

BOOLN
InitModule_Utility_BinSignal(ModulePtr theModule)
{
	static const char	*funcName = "InitModule_Utility_BinSignal";

	if (!SetParsPointer_Utility_BinSignal(theModule)) {
		NotifyError("%s: Cannot set parameters pointer.", funcName);
		return(FALSE);
	}
	if (!Init_Utility_BinSignal(GLOBAL)) {
		NotifyError("%s: Could not initialise process structure.", funcName);
		return(FALSE);
	}
	theModule->parsPtr = binSignalPtr;
	theModule->CheckPars = CheckPars_Utility_BinSignal;
	theModule->Free = Free_Utility_BinSignal;
	theModule->GetUniParListPtr = GetUniParListPtr_Utility_BinSignal;
	theModule->PrintPars = PrintPars_Utility_BinSignal;
	theModule->ReadPars = ReadPars_Utility_BinSignal;
	theModule->RunProcess = Process_Utility_BinSignal;
	theModule->SetParsPointer = SetParsPointer_Utility_BinSignal;
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
CheckData_Utility_BinSignal(EarObjectPtr data)
{
	static const char	*funcName = "CheckData_Utility_BinSignal";
	double	dt;

	if (data == NULL) {
		NotifyError("%s: EarObject not initialised.", funcName);
		return(FALSE);
	}
	if (!CheckInit_SignalData(data->inSignal[0], "CheckData_Utility_BinSignal"))
		return(FALSE);
	dt = data->inSignal[0]->dt;
	if (((binSignalPtr->binWidth > 0.0) && (binSignalPtr->binWidth < dt)) ||
	  (binSignalPtr->binWidth > _GetDuration_SignalData(data->inSignal[0]))) {
		NotifyError("%s: Invalid binWidth (%g ms).", funcName,
		  MSEC(binSignalPtr->binWidth));
		return(FALSE);
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
Process_Utility_BinSignal(EarObjectPtr data)
{
	static const char	*funcName = "Process_Utility_BinSignal";
	register	ChanData	 *inPtr, *outPtr, binSum, dt, nextBinCutOff;
	int		chan, numBins;
	double duration, binWidth;
	ChanLen	i;

	if (!CheckPars_Utility_BinSignal())
		return(FALSE);
	if (!CheckData_Utility_BinSignal(data)) {
		NotifyError("%s: Process data invalid.", funcName);
		return(FALSE);
	}
	SetProcessName_EarObject(data, "Signal binning routine");
	duration = _GetDuration_SignalData(data->inSignal[0]);
	binWidth = (binSignalPtr->binWidth < 0.0)? duration: binSignalPtr->binWidth;
	if (!InitOutSignal_EarObject(data, data->inSignal[0]->numChannels,
	  (ChanLen) floor(duration / binWidth + 0.5), binWidth)) {
		NotifyError("%s: Cannot initialise output channels.", funcName);
		return(FALSE);
	}
	dt = data->inSignal[0]->dt;
	numBins = (int) floor(binWidth / dt + 0.5);
	for (chan = 0; chan < data->inSignal[0]->numChannels; chan++) {
		inPtr = data->inSignal[0]->channel[chan];
		outPtr = data->outSignal->channel[chan];
		nextBinCutOff = binWidth;
		for (i = 0, binSum = 0.0; i < data->inSignal[0]->length; i++) {
			binSum += *(inPtr++);
			if (DBL_GREATER_OR_EQUAL((i + 1) * dt, nextBinCutOff)) {
				if ((ChanLen) (outPtr - data->outSignal->channel[chan]) <
				  data->outSignal->length)
					*outPtr++ += (binSignalPtr->mode ==
					  UTILITY_BINSIGNAL_AVERAGE_MODE)? binSum / numBins: binSum;
				binSum = 0.0;
				nextBinCutOff += binWidth;
			}
		}
	}
	SetUtilityProcessContinuity_EarObject(data);
	SetProcessContinuity_EarObject(data);
	return(TRUE);

}
