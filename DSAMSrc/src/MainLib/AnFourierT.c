/**********************
 *
 * File:		AnFourierT.c
 * Purpose:		This module runs a basic Fourier Analysis.
 * Comments:	
 * Author:		L. P. O'Mard
 * Created:		18-01-94
 * Updated:		
 * Copyright:	(c) 2000, University of Essex
 *
 **********************/

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
#include "GeModuleMgr.h"
#include "UtCmplxM.h"
#include "UtFFT.h"
#include "UtString.h"
#include "AnFourierT.h"

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

FourierTPtr	fourierTPtr = NULL;

/******************************************************************************/
/****************************** Functions and subroutines *********************/
/******************************************************************************/

/****************************** Free ******************************************/

/*
 * This function releases of the memory allocated for the process
 * variables. It should be called when the module is no longer in use.
 * It is defined as returning a BOOLN value because the generic
 * module interface requires that a non-void value be returned.
 */

BOOLN
Free_Analysis_FourierT(void)
{
	if (fourierTPtr == NULL)
		return(FALSE);
	FreeProcessVariables_Analysis_FourierT();
	if (fourierTPtr->parList)
		FreeList_UniParMgr(&fourierTPtr->parList);
	if (fourierTPtr->parSpec == GLOBAL) {
		free(fourierTPtr);
		fourierTPtr = NULL;
	}
	return(TRUE);

}

/****************************** InitOutputModeList ****************************/

/*
 * This function initialises the 'outputMode' list array
 */

BOOLN
InitOutputModeList_Analysis_FourierT(void)
{
	static NameSpecifier	modeList[] = {

			{ wxT("MODULUS"),	ANALYSIS_FOURIERT_MODULUS_OUTPUTMODE },
			{ wxT("PHASE"),		ANALYSIS_FOURIERT_PHASE_OUTPUTMODE },
			{ wxT("COMPLEX"),	ANALYSIS_FOURIERT_COMPLEX_OUTPUTMODE },
			{ wxT("DB_SPL"),	ANALYSIS_FOURIERT_DB_SPL_OUTPUTMODE },
			{ wxT(""),			ANALYSIS_FOURIERT_OUTPUTMODE_NULL },
		};
	fourierTPtr->outputModeList = modeList;
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
Init_Analysis_FourierT(ParameterSpecifier parSpec)
{
	static const WChar	*funcName = wxT("Init_Analysis_FourierT");

	if (parSpec == GLOBAL) {
		if (fourierTPtr != NULL)
			free(fourierTPtr);
		if ((fourierTPtr = (FourierTPtr) malloc(sizeof(FourierT))) == NULL) {
			NotifyError(wxT("%s: Out of memory for 'global' pointer"),
			  funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (fourierTPtr == NULL) {
			NotifyError(wxT("%s:  'local' pointer not set."), funcName);
			return(FALSE);
		}
	}
	fourierTPtr->parSpec = parSpec;
	fourierTPtr->updateProcessVariablesFlag = TRUE;
	fourierTPtr->outputModeFlag = TRUE;
	fourierTPtr->outputMode = ANALYSIS_FOURIERT_MODULUS_OUTPUTMODE;

	InitOutputModeList_Analysis_FourierT();
	if (!SetUniParList_Analysis_FourierT()) {
		NotifyError(wxT("%s: Could not initialise parameter list."), funcName);
		Free_Analysis_FourierT();
		return(FALSE);
	}
	fourierTPtr->fTLength = 0;
	fourierTPtr->numThreads = 0;
	fourierTPtr->fT = NULL;
	return(TRUE);

}

/****************************** SetUniParList *********************************/

/*
 * This function initialises and sets the module's universal parameter
 * list. This list provides universal access to the module's
 * parameters.  It expects to be called from the 'Init_' routine.
 */

BOOLN
SetUniParList_Analysis_FourierT(void)
{
	static const WChar *funcName = wxT("SetUniParList_Analysis_FourierT");
	UniParPtr	pars;

	if ((fourierTPtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  ANALYSIS_FOURIERT_NUM_PARS, NULL)) == NULL) {
		NotifyError(wxT("%s: Could not initialise parList."), funcName);
		return(FALSE);
	}
	pars = fourierTPtr->parList->pars;
	SetPar_UniParMgr(&pars[ANALYSIS_FOURIERT_OUTPUTMODE], wxT("OUTPUT_MODE"),
	  wxT("Output mode: 'modulus', 'phase', 'complex' or 'dB_SPL' "
	  "(approximation)."),
	  UNIPAR_NAME_SPEC,
	  &fourierTPtr->outputMode, fourierTPtr->outputModeList,
	  (void * (*)) SetOutputMode_Analysis_FourierT);
	return(TRUE);

}

/****************************** GetUniParListPtr ******************************/

/*
 * This function returns a pointer to the module's universal parameter
 * list.
 */

UniParListPtr
GetUniParListPtr_Analysis_FourierT(void)
{
	static const WChar	*funcName = wxT("GetUniParListPtr_Analysis_FourierT");

	if (fourierTPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (fourierTPtr->parList == NULL) {
		NotifyError(wxT("%s: UniParList data structure has not been "
		  "initialised. NULL returned."), funcName);
		return(NULL);
	}
	return(fourierTPtr->parList);

}

/****************************** SetPars ***************************************/

/*
 * This function sets all the module's parameters.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetPars_Analysis_FourierT(WChar * outputMode)
{
	static const WChar	*funcName = wxT("SetPars_Analysis_FourierT");
	BOOLN	ok;

	ok = TRUE;
	if (!SetOutputMode_Analysis_FourierT(outputMode))
		ok = FALSE;
	if (!ok)
		NotifyError(wxT("%s: Failed to set all module parameters.") ,funcName);
	return(ok);

}

/****************************** SetOutputMode *********************************/

/*
 * This function sets the module's outputMode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetOutputMode_Analysis_FourierT(WChar * theOutputMode)
{
	static const WChar	*funcName = wxT("SetOutputMode_Analysis_FourierT");
	int		specifier;

	if (fourierTPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theOutputMode,
		fourierTPtr->outputModeList)) == ANALYSIS_FOURIERT_OUTPUTMODE_NULL) {
		NotifyError(wxT("%s: Illegal name (%s)."), funcName, theOutputMode);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	fourierTPtr->outputModeFlag = TRUE;
	fourierTPtr->outputMode = specifier;
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
CheckPars_Analysis_FourierT(void)
{
	static const WChar	*funcName = wxT("CheckPars_Analysis_FourierT");
	BOOLN	ok;

	ok = TRUE;
	if (fourierTPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (!fourierTPtr->outputModeFlag) {
		NotifyError(wxT("%s: outputMode variable not set."), funcName);
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
PrintPars_Analysis_FourierT(void)
{
	static const WChar	*funcName = wxT("PrintPars_Analysis_FourierT");

	if (!CheckPars_Analysis_FourierT()) {
		NotifyError(wxT("%s: Parameters have not been correctly set."),
		  funcName);
		return(FALSE);
	}
	DPrint(wxT("Fourier Transform Analysis Module Parameters:-\n"));
	DPrint(wxT("\tOutput mode = %s \n"),
	  fourierTPtr->outputModeList[fourierTPtr->outputMode].name);
	return(TRUE);

}

/****************************** ReadPars **************************************/

/*
 * This program reads a specified number of parameters from a file.
 * It returns FALSE if it fails in any way.n */

BOOLN
ReadPars_Analysis_FourierT(WChar *fileName)
{
	static const WChar	*funcName = wxT("ReadPars_Analysis_FourierT");
	BOOLN	ok;
	WChar	*filePath;
	WChar	outputMode[MAXLINE];
	FILE	*fp;

	filePath = GetParsFileFPath_Common(fileName);
	if ((fp = fopen(ConvUTF8_Utility_String(filePath), "r")) == NULL) {
		NotifyError(wxT("%s: Cannot open data file '%s'.\n"), funcName,
		  fileName);
		return(FALSE);
	}
	DPrint(wxT("%s: Reading from '%s':\n"), funcName, fileName);
	Init_ParFile();
	ok = TRUE;
	if (!GetPars_ParFile(fp, wxT("%s"), outputMode))
		ok = FALSE;
	fclose(fp);
	Free_ParFile();
	if (!ok) {
		NotifyError(wxT("%s: Not enough lines, or invalid parameters, in "
		  "module parameter file '%s'."), funcName, fileName);
		return(FALSE);
	}
	if (!SetPars_Analysis_FourierT(outputMode)) {
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
SetParsPointer_Analysis_FourierT(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("SetParsPointer_Analysis_FourierT");

	if (!theModule) {
		NotifyError(wxT("%s: The module is not set."), funcName);
		return(FALSE);
	}
	fourierTPtr = (FourierTPtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 */

BOOLN
InitModule_Analysis_FourierT(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("InitModule_Analysis_FourierT");

	if (!SetParsPointer_Analysis_FourierT(theModule)) {
		NotifyError(wxT("%s: Cannot set parameters pointer."), funcName);
		return(FALSE);
	}
	if (!Init_Analysis_FourierT(GLOBAL)) {
		NotifyError(wxT("%s: Could not initialise process structure."),
		  funcName);
		return(FALSE);
	}
	theModule->parsPtr = fourierTPtr;
	theModule->threadMode = MODULE_THREAD_MODE_SIMPLE;
	theModule->CheckPars = CheckPars_Analysis_FourierT;
	theModule->Free = Free_Analysis_FourierT;
	theModule->GetUniParListPtr = GetUniParListPtr_Analysis_FourierT;
	theModule->PrintPars = PrintPars_Analysis_FourierT;
	theModule->ReadPars = ReadPars_Analysis_FourierT;
	theModule->ResetProcess = ResetProcess_Analysis_FourierT;
	theModule->RunProcess = Calc_Analysis_FourierT;
	theModule->SetParsPointer = SetParsPointer_Analysis_FourierT;
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
CheckData_Analysis_FourierT(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("CheckData_Analysis_FourierT");

	if (data == NULL) {
		NotifyError(wxT("%s: EarObject not initialised."), funcName);
		return(FALSE);
	}
	if (!CheckInSignal_EarObject(data, funcName))
		return(FALSE);
	/*** Put additional checks here. ***/
	return(TRUE);

}

/**************************** ResetProcess ************************************/

/*
 * This routine resets the process variables.
 */

void
ResetProcess_Analysis_FourierT(EarObjectPtr data)
{
	ResetOutSignal_EarObject(data);

}

/**************************** InitProcessVariables ****************************/

/*
 * This routine initialises the exponential table.
 * It assumes that all of the parameters for the module have been correctly
 * initialised.
 * This routine assumes that calloc sets all of the InterSIHSpikeSpecPtr
 * pointers to NULL.
 */

BOOLN
InitProcessVariables_Analysis_FourierT(EarObjectPtr data)
{
	static const WChar *funcName = wxT(
	  "InitProcessVariables_Analysis_FourierT");
	int		i;
	FourierTPtr	p = fourierTPtr;

	if (p->updateProcessVariablesFlag || data->updateProcessFlag) {
		FreeProcessVariables_Analysis_FourierT();
		p->fTLength = Length_FFT(data->inSignal[0]->length);
		p->numThreads = data->numThreads;
		if ((p->fT = (ComplexPtr *) calloc(p->numThreads, sizeof(
		  ComplexPtr))) == NULL) {
			NotifyError(wxT("%s: Couldn't allocate memory for complex data "
			  "pointer array."), funcName);
			return(FALSE);
		}
		for (i = 0; i < data->numThreads; i++)
			if ((p->fT[i] = (Complex *) calloc(p->fTLength, sizeof(
			  Complex))) == NULL) {
				NotifyError(wxT("%s: Couldn't allocate memory for complex data "
				  "array (%d)."), funcName, i);
				return(FALSE);
			}
		p->updateProcessVariablesFlag = FALSE;
	}
	return(TRUE);

}

/**************************** FreeProcessVariables ****************************/

/*
 * This routine releases the memory allocated for the process variables
 * if they have been initialised.
 */

void
FreeProcessVariables_Analysis_FourierT(void)
{
	int		i;

	if (fourierTPtr->fT) {
		for (i = 0; i < fourierTPtr->numThreads; i++)
			free(fourierTPtr->fT[i]);
		free(fourierTPtr->fT);
		fourierTPtr->fT = NULL;
	}

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
 * The outSignal->dt field is set to the dF value, however to calculate the
 * length of the channels, this needs to be the same as the input signal dt.
 * the call to 'SetSamplingInterval_SignalData' before 'InitOutSignal_EarObject'
 * ensures that the signal is not needlessly recreated.
 */

BOOLN
Calc_Analysis_FourierT(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("Calc_Analysis_FourierT");
	register	ChanData	 *inPtr, *outPtr;
	int		chan, outChan;
	double	dF;
	ChanLen	i;
	Complex	*fT;
	FourierTPtr	p = fourierTPtr;

	if (!data->threadRunFlag) {
		if (!CheckPars_Analysis_FourierT())
			return(FALSE);
		if (!CheckData_Analysis_FourierT(data)) {
			NotifyError(wxT("%s: Process data invalid."), funcName);
			return(FALSE);
		}
		SetProcessName_EarObject(data, wxT("Fourier Transform: modulus"));
		p->numOutChans = (p->outputMode ==
		  ANALYSIS_FOURIERT_COMPLEX_OUTPUTMODE)? 2: 1;
		if (data->outSignal)
			SetSamplingInterval_SignalData(data->outSignal, data->inSignal[
			  0]->dt);
		if (!InitOutSignal_EarObject(data, (uShort) (data->inSignal[0]->
		  numChannels * p->numOutChans), data->inSignal[0]->length, data->
		  inSignal[0]->dt)) {
			NotifyError(wxT("%s: Couldn't initialse output signal."), funcName);
			return(FALSE);
		}
		ResetProcess_Analysis_FourierT(data);
		if (!InitProcessVariables_Analysis_FourierT(data)) {
			NotifyError(wxT("%s: Could not initialise the process variables."),
			  funcName);
			return(FALSE);
		}
		dF = 1.0 / (data->inSignal[0]->dt * p->fTLength);
		SetSamplingInterval_SignalData(data->outSignal, dF);
		SetLocalInfoFlag_SignalData(data->outSignal, TRUE);
		SetInfoSampleTitle_SignalData(data->outSignal, wxT("Frequency (Hz)"));
		SetInfoChannelTitle_SignalData(data->outSignal, wxT("Arbitrary "
		  "Amplitude"));
		SetStaticTimeFlag_SignalData(data->outSignal, TRUE);
		SetOutputTimeOffset_SignalData(data->outSignal, 0.0);
		SetInterleaveLevel_SignalData(data->outSignal, (uShort) (
		  data->inSignal[0]->interleaveLevel * p->numOutChans));
		p->dBSPLFactor = SQRT_2 / data->inSignal[0]->length;
		if (data->initThreadRunFlag)
			return(TRUE);
	}
	for (chan = data->outSignal->offset; chan < data->outSignal->numChannels;
	  chan++) {
		outChan = chan * p->numOutChans;
		fT = p->fT[data->threadIndex];
		fT->re = fT->im = 0.0;
		inPtr = data->inSignal[0]->channel[outChan] + 1;
		for (i = 1, fT++; i < data->outSignal->length; i++, fT++) {
			fT->im = 0.0;
			fT->re = *inPtr++;
		}
		for (i = i; i < p->fTLength; i++, fT++) {
			fT->re = 0.0;
			fT->im = 0.0;
		}
		fT = p->fT[data->threadIndex];
		outPtr = data->outSignal->channel[outChan];
		CalcComplex_FFT(fT, p->fTLength, FORWARD_FT);
		switch (p->outputMode) {
		case ANALYSIS_FOURIERT_MODULUS_OUTPUTMODE:
			for (i = 0; i < data->outSignal->length; i++, fT++)
				*outPtr++ = (ChanData) MODULUS_CMPLX(*fT);
			break;
		case ANALYSIS_FOURIERT_PHASE_OUTPUTMODE:
			for (i = 0; i < data->outSignal->length; i++, fT++)
				*outPtr++ = (ChanData) atan2(fT->im, fT->re);
			break;
		case ANALYSIS_FOURIERT_COMPLEX_OUTPUTMODE:
			for (i = 0; i < data->outSignal->length; i++, fT++)
				*outPtr++ = (ChanData) fT->re;
			fT = p->fT[data->threadIndex];
			outPtr = data->outSignal->channel[outChan + 1];
			for (i = 0; i < data->outSignal->length; i++, fT++)
				*outPtr++ = (ChanData) fT->im;
			break;
		case ANALYSIS_FOURIERT_DB_SPL_OUTPUTMODE:
			for (i = 0; i < data->outSignal->length; i++, fT++)
				*outPtr++ = (ChanData) DB_SPL(MODULUS_CMPLX(*fT) *
				  p->dBSPLFactor);
			break;
		default:
			;
		}		
		
	}
	SetProcessContinuity_EarObject(data);
	return(TRUE);

}

