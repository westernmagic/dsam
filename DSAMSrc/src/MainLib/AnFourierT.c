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
	/* static const char	*funcName = "Free_Analysis_FourierT"; */

	if (fourierTPtr == NULL)
		return(FALSE);
	FreeProcessVariables_Analysis_FourierT();
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

			{ "MODULUS",	ANALYSIS_FOURIERT_MODULUS_OUTPUTMODE },
			{ "PHASE",		ANALYSIS_FOURIERT_PHASE_OUTPUTMODE },
			{ "",	ANALYSIS_FOURIERT_OUTPUTMODE_NULL },
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
	static const char	*funcName = "Init_Analysis_FourierT";

	if (parSpec == GLOBAL) {
		if (fourierTPtr != NULL)
			free(fourierTPtr);
		if ((fourierTPtr = (FourierTPtr) malloc(sizeof(FourierT))) == NULL) {
			NotifyError("%s: Out of memory for 'global' pointer", funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (fourierTPtr == NULL) {
			NotifyError("%s:  'local' pointer not set.", funcName);
			return(FALSE);
		}
	}
	fourierTPtr->parSpec = parSpec;
	fourierTPtr->updateProcessVariablesFlag = TRUE;
	fourierTPtr->outputModeFlag = TRUE;
	fourierTPtr->outputMode = ANALYSIS_FOURIERT_MODULUS_OUTPUTMODE;

	InitOutputModeList_Analysis_FourierT();
	if (!SetUniParList_Analysis_FourierT()) {
		NotifyError("%s: Could not initialise parameter list.", funcName);
		Free_Analysis_FourierT();
		return(FALSE);
	}
	fourierTPtr->fTLength = 0;
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
	static const char *funcName = "SetUniParList_Analysis_FourierT";
	UniParPtr	pars;

	if ((fourierTPtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  ANALYSIS_FOURIERT_NUM_PARS, NULL)) == NULL) {
		NotifyError("%s: Could not initialise parList.", funcName);
		return(FALSE);
	}
	pars = fourierTPtr->parList->pars;
	SetPar_UniParMgr(&pars[ANALYSIS_FOURIERT_OUTPUTMODE], "OUTPUT_MODE",
	  "Output mode ('modulus' or 'phase').",
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
	static const char	*funcName = "GetUniParListPtr_Analysis_FourierT";

	if (fourierTPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (fourierTPtr->parList == NULL) {
		NotifyError("%s: UniParList data structure has not been initialised. "
		  "NULL returned.", funcName);
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
SetPars_Analysis_FourierT(char * outputMode)
{
	static const char	*funcName = "SetPars_Analysis_FourierT";
	BOOLN	ok;

	ok = TRUE;
	if (!SetOutputMode_Analysis_FourierT(outputMode))
		ok = FALSE;
	if (!ok)
		NotifyError("%s: Failed to set all module parameters." ,funcName);
	return(ok);

}

/****************************** SetOutputMode *********************************/

/*
 * This function sets the module's outputMode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetOutputMode_Analysis_FourierT(char * theOutputMode)
{
	static const char	*funcName = "SetOutputMode_Analysis_FourierT";
	int		specifier;

	if (fourierTPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theOutputMode,
		fourierTPtr->outputModeList)) == ANALYSIS_FOURIERT_OUTPUTMODE_NULL) {
		NotifyError("%s: Illegal name (%s).", funcName, theOutputMode);
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
	static const char	*funcName = "CheckPars_Analysis_FourierT";
	BOOLN	ok;

	ok = TRUE;
	if (fourierTPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (!fourierTPtr->outputModeFlag) {
		NotifyError("%s: outputMode variable not set.", funcName);
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
	static const char	*funcName = "PrintPars_Analysis_FourierT";

	if (!CheckPars_Analysis_FourierT()) {
		NotifyError("%s: Parameters have not been correctly set.", funcName);
		return(FALSE);
	}
	DPrint("Fourier Transform Analysis Module Parameters:-\n");
	DPrint("\tOutput mode = %s \n",
	  fourierTPtr->outputModeList[fourierTPtr->outputMode].name);
	return(TRUE);

}

/****************************** ReadPars **************************************/

/*
 * This program reads a specified number of parameters from a file.
 * It returns FALSE if it fails in any way.n */

BOOLN
ReadPars_Analysis_FourierT(char *fileName)
{
	static const char	*funcName = "ReadPars_Analysis_FourierT";
	BOOLN	ok;
	char	*filePath;
	char	outputMode[MAXLINE];
	FILE	*fp;

	filePath = GetParsFileFPath_Common(fileName);
	if ((fp = fopen(filePath, "r")) == NULL) {
		NotifyError("%s: Cannot open data file '%s'.\n", funcName, fileName);
		return(FALSE);
	}
	DPrint("%s: Reading from '%s':\n", funcName, fileName);
	Init_ParFile();
	ok = TRUE;
	if (!GetPars_ParFile(fp, "%s", outputMode))
		ok = FALSE;
	fclose(fp);
	Free_ParFile();
	if (!ok) {
		NotifyError("%s: Not enough lines, or invalid parameters, in module "
		  "parameter file '%s'.", funcName, fileName);
		return(FALSE);
	}
	if (!SetPars_Analysis_FourierT(outputMode)) {
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
SetParsPointer_Analysis_FourierT(ModulePtr theModule)
{
	static const char	*funcName = "SetParsPointer_Analysis_FourierT";

	if (!theModule) {
		NotifyError("%s: The module is not set.", funcName);
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
	static const char	*funcName = "InitModule_Analysis_FourierT";

	if (!SetParsPointer_Analysis_FourierT(theModule)) {
		NotifyError("%s: Cannot set parameters pointer.", funcName);
		return(FALSE);
	}
	if (!Init_Analysis_FourierT(GLOBAL)) {
		NotifyError("%s: Could not initialise process structure.", funcName);
		return(FALSE);
	}
	theModule->parsPtr = fourierTPtr;
	theModule->CheckPars = CheckPars_Analysis_FourierT;
	theModule->Free = Free_Analysis_FourierT;
	theModule->GetUniParListPtr = GetUniParListPtr_Analysis_FourierT;
	theModule->PrintPars = PrintPars_Analysis_FourierT;
	theModule->ReadPars = ReadPars_Analysis_FourierT;
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
	static const char	*funcName = "CheckData_Analysis_FourierT";

	if (data == NULL) {
		NotifyError("%s: EarObject not initialised.", funcName);
		return(FALSE);
	}
	if (!CheckInit_SignalData(data->inSignal[0], funcName))
		return(FALSE);
	/*** Put additional checks here. ***/
	return(TRUE);

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
	static const char *funcName = "InitProcessVariables_Analysis_FourierT";

	if (fourierTPtr->updateProcessVariablesFlag || data->updateProcessFlag) {
		FreeProcessVariables_Analysis_FourierT();
		fourierTPtr->fTLength = Length_FFT(data->inSignal[0]->length);
		if ((fourierTPtr->fT = (Complex *) calloc(fourierTPtr->fTLength,
		  sizeof(Complex))) == NULL) {
			NotifyError("%s: Couldn't allocate memory for complex data array.",
			  funcName);
			return(FALSE);
		}
		fourierTPtr->updateProcessVariablesFlag = FALSE;
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
	if (fourierTPtr->fT) {
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
 */

BOOLN
Calc_Analysis_FourierT(EarObjectPtr data)
{
	static const char	*funcName = "Calc_Analysis_FourierT";
	register	ChanData	 *inPtr, *outPtr;
	int		chan;
	double	dF;
	ChanLen	i;
	Complex	*fT;

	if (!CheckPars_Analysis_FourierT())
		return(FALSE);
	if (!CheckData_Analysis_FourierT(data)) {
		NotifyError("%s: Process data invalid.", funcName);
		return(FALSE);
	}
	SetProcessName_EarObject(data, "Fourier Transform: modulus");
	if (!InitOutSignal_EarObject(data, data->inSignal[0]->numChannels,
	  data->inSignal[0]->length, data->inSignal[0]->dt)) {
		NotifyError("%s: Couldn't initialse output signal.", funcName);
		return(FALSE);
	}
	if (!InitProcessVariables_Analysis_FourierT(data)) {
		NotifyError("%s: Could not initialise the process variables.",
		  funcName);
		return(FALSE);
	}
	dF = 1.0 / (data->inSignal[0]->dt * fourierTPtr->fTLength);
	SetSamplingInterval_SignalData(data->outSignal, dF);
	SetLocalInfoFlag_SignalData(data->outSignal, TRUE);
	SetInfoSampleTitle_SignalData(data->outSignal, "Frequency (Hz)");
	SetInfoChannelTitle_SignalData(data->outSignal, "Arbitrary Amplitude");
	SetStaticTimeFlag_SignalData(data->outSignal, TRUE);
	SetOutputTimeOffset_SignalData(data->outSignal, 0.0);
	/* Set up data and apply Lorch Window. */
	for (chan = 0; chan < data->inSignal[0]->numChannels; chan++) {
		fT = fourierTPtr->fT;
		fT->re = fT->im = 0.0;
		inPtr = data->inSignal[0]->channel[chan] + 1;
		for (i = 1, fT++; i < data->outSignal->length; i++, fT++) {
			fT->im = 0.0;
			fT->re = *inPtr++;
		}
		for (i = i; i < fourierTPtr->fTLength; i++, fT++) {
			fT->re = 0.0;
			fT->im = 0.0;
		}
		CalcComplex_FFT(fourierTPtr->fT, fourierTPtr->fTLength, FORWARD_FT);
		outPtr = data->outSignal->channel[chan];
		fT = fourierTPtr->fT;
		switch (fourierTPtr->outputMode) {
		case ANALYSIS_FOURIERT_MODULUS_OUTPUTMODE:
			for (i = 0; i < data->outSignal->length; i++, fT++)
				*outPtr++ = (ChanData) Modulus_CmplxM(fT);
			break;
		case ANALYSIS_FOURIERT_PHASE_OUTPUTMODE:
			for (i = 0; i < data->outSignal->length; i++, fT++)
				*outPtr++ = (ChanData) atan2(fT->im, fT->re);
			break;
		default:
			;
		}		
		
	}
	SetProcessContinuity_EarObject(data);
	return(TRUE);

}

