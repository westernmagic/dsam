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
#include "UtString.h"
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

			{ wxT("AVERAGE"),	UTILITY_BINSIGNAL_AVERAGE_MODE },
			{ wxT("SUM"),		UTILITY_BINSIGNAL_SUM_MODE },
			{ wxT(""),			UTILITY_BINSIGNAL_MODE_NULL },
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
	static const WChar	*funcName = wxT("Init_Utility_BinSignal");

	if (parSpec == GLOBAL) {
		if (binSignalPtr != NULL)
			Free_Utility_BinSignal();
		if ((binSignalPtr = (BinSignalPtr) malloc(sizeof(BinSignal))) == NULL) {
			NotifyError(wxT("%s: Out of memory for 'global' pointer"),
			  funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (binSignalPtr == NULL) {
			NotifyError(wxT("%s:  'local' pointer not set."), funcName);
			return(FALSE);
		}
	}
	binSignalPtr->parSpec = parSpec;
	binSignalPtr->modeFlag = TRUE;
	binSignalPtr->binWidthFlag = TRUE;
	binSignalPtr->mode = UTILITY_BINSIGNAL_SUM_MODE;
	binSignalPtr->binWidth = -1.0;

	InitModeList_Utility_BinSignal();
	if (!SetUniParList_Utility_BinSignal()) {
		NotifyError(wxT("%s: Could not initialise parameter list."), funcName);
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
	static const WChar *funcName = wxT("SetUniParList_Utility_BinSignal");
	UniParPtr	pars;

	if ((binSignalPtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  UTILITY_BINSIGNAL_NUM_PARS, NULL)) == NULL) {
		NotifyError(wxT("%s: Could not initialise parList."), funcName);
		return(FALSE);
	}
	pars = binSignalPtr->parList->pars;
	SetPar_UniParMgr(&pars[UTILITY_BINSIGNAL_MODE], wxT("MODE"),
	  wxT("Bining mode ('average' or 'sum')."),
	  UNIPAR_NAME_SPEC,
	  &binSignalPtr->mode, binSignalPtr->modeList,
	  (void * (*)) SetMode_Utility_BinSignal);
	SetPar_UniParMgr(&pars[UTILITY_BINSIGNAL_BINWIDTH], wxT("BIN_WIDTH"),
	  wxT("Bin width for binned signal (s) (-ve: prev. signal duration)."),
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
	static const WChar	*funcName = wxT("GetUniParListPtr_Utility_BinSignal");

	if (binSignalPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (binSignalPtr->parList == NULL) {
		NotifyError(wxT("%s: UniParList data structure has not been ")
		  wxT("initialised. NULL returned."), funcName);
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
SetPars_Utility_BinSignal(WChar * mode, double binWidth)
{
	static const WChar	*funcName = wxT("SetPars_Utility_BinSignal");
	BOOLN	ok;

	ok = TRUE;
	if (!SetMode_Utility_BinSignal(mode))
		ok = FALSE;
	if (!SetBinWidth_Utility_BinSignal(binWidth))
		ok = FALSE;
	if (!ok)
		NotifyError(wxT("%s: Failed to set all module parameters.") ,funcName);
	return(ok);

}

/****************************** SetMode ***************************************/

/*
 * This function sets the module's mode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetMode_Utility_BinSignal(WChar * theMode)
{
	static const WChar	*funcName = wxT("SetMode_Utility_BinSignal");
	int		specifier;

	if (binSignalPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theMode, binSignalPtr->modeList)) ==
	   UTILITY_BINSIGNAL_MODE_NULL) {
		NotifyError(wxT("%s: Illegal name (%s)."), funcName, theMode);
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
	static const WChar	*funcName = wxT("SetBinWidth_Utility_BinSignal");

	if (binSignalPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
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
	static const WChar	*funcName = wxT("CheckPars_Utility_BinSignal");
	BOOLN	ok;

	ok = TRUE;
	if (binSignalPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (!binSignalPtr->modeFlag) {
		NotifyError(wxT("%s: mode variable not set."), funcName);
		ok = FALSE;
	}
	if (!binSignalPtr->binWidthFlag) {
		NotifyError(wxT("%s: binWidth variable not set."), funcName);
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
	static const WChar	*funcName = wxT("PrintPars_Utility_BinSignal");

	if (!CheckPars_Utility_BinSignal()) {
		NotifyError(wxT("%s: Parameters have not been correctly set."),
		  funcName);
		return(FALSE);
	}
	DPrint(wxT("Bin Signal Utility Module Parameters:-\n"));
	DPrint(wxT("\tBining mode = %s, \t"), binSignalPtr->modeList[
	  binSignalPtr->mode].name);
	DPrint(wxT("\tBin width = "));
	if (binSignalPtr->binWidth < 0.0)
		DPrint(wxT("signal/segment duration.\n"));
	else
		DPrint(wxT("%g (ms).\n"), MSEC(binSignalPtr->binWidth));
	return(TRUE);

}

/****************************** ReadPars **************************************/

/*
 * This program reads a specified number of parameters from a file.
 * It returns FALSE if it fails in any way.n */

BOOLN
ReadPars_Utility_BinSignal(WChar *fileName)
{
	static const WChar	*funcName = wxT("ReadPars_Utility_BinSignal");
	BOOLN	ok;
	WChar	*filePath, mode[MAXLINE];
	double	binWidth;
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
	if (!GetPars_ParFile(fp, wxT("%s"), mode))
		ok = FALSE;
	if (!GetPars_ParFile(fp, wxT("%lf"), &binWidth))
		ok = FALSE;
	fclose(fp);
	Free_ParFile();
	if (!ok) {
		NotifyError(wxT("%s: Not enough lines, or invalid parameters, in ")
		  wxT("module parameter file '%s'."), funcName, filePath);
		return(FALSE);
	}
	if (!SetPars_Utility_BinSignal(mode, binWidth)) {
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
SetParsPointer_Utility_BinSignal(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("SetParsPointer_Utility_BinSignal");

	if (!theModule) {
		NotifyError(wxT("%s: The module is not set."), funcName);
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
	static const WChar	*funcName = wxT("InitModule_Utility_BinSignal");

	if (!SetParsPointer_Utility_BinSignal(theModule)) {
		NotifyError(wxT("%s: Cannot set parameters pointer."), funcName);
		return(FALSE);
	}
	if (!Init_Utility_BinSignal(GLOBAL)) {
		NotifyError(wxT("%s: Could not initialise process structure."),
		  funcName);
		return(FALSE);
	}
	theModule->parsPtr = binSignalPtr;
	theModule->threadMode = MODULE_THREAD_MODE_SIMPLE;
	theModule->CheckPars = CheckPars_Utility_BinSignal;
	theModule->Free = Free_Utility_BinSignal;
	theModule->GetUniParListPtr = GetUniParListPtr_Utility_BinSignal;
	theModule->PrintPars = PrintPars_Utility_BinSignal;
	theModule->ReadPars = ReadPars_Utility_BinSignal;
	theModule->ResetProcess = ResetProcess_Utility_BinSignal;
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
	static const WChar	*funcName = wxT("CheckData_Utility_BinSignal");
	double	dt;

	if (data == NULL) {
		NotifyError(wxT("%s: EarObject not initialised."), funcName);
		return(FALSE);
	}
	if (!CheckInSignal_EarObject(data, funcName))
		return(FALSE);
	dt = _InSig_EarObject(data, 0)->dt;
	if (((binSignalPtr->binWidth > 0.0) && (binSignalPtr->binWidth < dt)) ||
	  (binSignalPtr->binWidth > _GetDuration_SignalData(_InSig_EarObject(data, 0)))) {
		NotifyError(wxT("%s: Invalid binWidth (%g ms)."), funcName,
		  MSEC(binSignalPtr->binWidth));
		return(FALSE);
	}
	return(TRUE);

}

/**************************** ResetProcess ************************************/

/*
 * This routine resets the process variables.
 */

void
ResetProcess_Utility_BinSignal(EarObjectPtr data)
{
	ResetOutSignal_EarObject(data);

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
	static const WChar	*funcName = wxT("Process_Utility_BinSignal");
	register	ChanData	 *inPtr, *outPtr, binSum, nextBinCutOff;
	int		chan;
	double duration;
	ChanLen	i;
	SignalDataPtr	outSignal;
	BinSignalPtr	p = binSignalPtr;

	if (!data->threadRunFlag) {
		if (!CheckPars_Utility_BinSignal())
			return(FALSE);
		if (!CheckData_Utility_BinSignal(data)) {
			NotifyError(wxT("%s: Process data invalid."), funcName);
			return(FALSE);
		}
		SetProcessName_EarObject(data, wxT("Signal binning routine"));
		duration = _GetDuration_SignalData(_InSig_EarObject(data, 0));
		p->dt = _InSig_EarObject(data, 0)->dt;
		if (p->binWidth < 0.0)
			p->wBinWidth = duration;
		else if (p->binWidth == 0.0)
			p->wBinWidth = p->dt;
		else
			p->wBinWidth = p->binWidth;
		if (!InitOutSignal_EarObject(data, _InSig_EarObject(data, 0)->numChannels,
		  (ChanLen) floor(duration / p->wBinWidth + 0.5), p->wBinWidth)) {
			NotifyError(wxT("%s: Cannot initialise output channels."),
			  funcName);
			return(FALSE);
		}
		ResetProcess_Utility_BinSignal(data);
		p->numBins = (int) floor(p->wBinWidth / p->dt + 0.5);
		if (data->initThreadRunFlag)
			return(TRUE);
	}
	outSignal = _OutSig_EarObject(data);
	for (chan = outSignal->offset; chan < outSignal->numChannels; chan++) {
		inPtr = _InSig_EarObject(data, 0)->channel[chan];
		outPtr = outSignal->channel[chan];
		nextBinCutOff = p->wBinWidth - p->dt;
		for (i = 0, binSum = 0.0; i < _InSig_EarObject(data, 0)->length; i++) {
			binSum += *(inPtr++);
			if (DBL_GREATER((i + 1) * p->dt, nextBinCutOff)) {
				if ((ChanLen) (outPtr - outSignal->channel[chan]) < outSignal->
				  length)
					*outPtr++ += (p->mode == UTILITY_BINSIGNAL_AVERAGE_MODE)?
					  binSum / p->numBins: binSum;
				binSum = 0.0;
				nextBinCutOff += p->wBinWidth;
			}
		}
	}
	SetUtilityProcessContinuity_EarObject(data);
	SetProcessContinuity_EarObject(data);
	return(TRUE);

}
