/**********************
 *
 * File:		StFMTone.c
 * Purpose:		Frequency modulated pure tone signal generation paradigm.
 * Comments:	Written using ModuleProducer version 1.5.
 * Author:		Almudena
 * Created:		Nov 28 1995
 * Updated:		12 Mar 1997
 * Copyright:	(c) 1998, University of Essex
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
#include "StFMTone.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

FMTonePtr	fMTonePtr = NULL;

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
Free_PureTone_FM(void)
{
	/* static const char	*funcName = "Free_PureTone_FM"; */

	if (fMTonePtr == NULL)
		return(FALSE);
	if (fMTonePtr->parList)
		FreeList_UniParMgr(&fMTonePtr->parList);
	if (fMTonePtr->parSpec == GLOBAL) {
		free(fMTonePtr);
		fMTonePtr = NULL;
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
Init_PureTone_FM(ParameterSpecifier parSpec)
{
	static const char	*funcName = "Init_PureTone_FM";

	if (parSpec == GLOBAL) {
		if (fMTonePtr != NULL)
			Free_PureTone_FM();
		if ((fMTonePtr = (FMTonePtr) malloc(sizeof(FMTone))) == NULL) {
			NotifyError("%s: Out of memory for 'global' pointer", funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (fMTonePtr == NULL) {
			NotifyError("%s:  'local' pointer not set.", funcName);
			return(FALSE);
		}
	}
	fMTonePtr->parSpec = parSpec;
	fMTonePtr->frequencyFlag = FALSE;
	fMTonePtr->intensityFlag = FALSE;
	fMTonePtr->durationFlag = FALSE;
	fMTonePtr->dtFlag = FALSE;
	fMTonePtr->phaseFlag = FALSE;
	fMTonePtr->modulationDepthFlag = FALSE;
	fMTonePtr->modulationFrequencyFlag = FALSE;
	fMTonePtr->modulationPhaseFlag = FALSE;
	fMTonePtr->frequency = 0.0;
	fMTonePtr->intensity = 0.0;
	fMTonePtr->duration = 0.0;
	fMTonePtr->dt = 0.0;
	fMTonePtr->phase = 0.0;
	fMTonePtr->modulationDepth = 0.0;
	fMTonePtr->modulationFrequency = 0.0;
	fMTonePtr->modulationPhase = 0.0;

	if (!SetUniParList_PureTone_FM()) {
		NotifyError("%s: Could not initialise parameter list.", funcName);
		Free_PureTone_FM();
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
SetUniParList_PureTone_FM(void)
{
	static const char *funcName = "SetUniParList_PureTone_FM";
	UniParPtr	pars;

	if ((fMTonePtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  PURETONE_FM_NUM_PARS, NULL)) == NULL) {
		NotifyError("%s: Could not initialise parList.", funcName);
		return(FALSE);
	}
	pars = fMTonePtr->parList->pars;
	SetPar_UniParMgr(&pars[PURETONE_FM_FREQUENCY], "FREQUENCY",
	  "Frequency (Hz).",
	  UNIPAR_REAL,
	  &fMTonePtr->frequency, NULL,
	  (void * (*)) SetFrequency_PureTone_FM);
	SetPar_UniParMgr(&pars[PURETONE_FM_INTENSITY], "INTENSITY",
	  "Intensity (dB SPL).",
	  UNIPAR_REAL,
	  &fMTonePtr->intensity, NULL,
	  (void * (*)) SetIntensity_PureTone_FM);
	SetPar_UniParMgr(&pars[PURETONE_FM_PHASE], "PHASE",
	  "Phase (degrees).",
	  UNIPAR_REAL,
	  &fMTonePtr->phase, NULL,
	  (void * (*)) SetPhase_PureTone_FM);
	SetPar_UniParMgr(&pars[PURETONE_FM_MODULATIONDEPTH], "MOD_DEPTH",
	  "Modulation depth (%).",
	  UNIPAR_REAL,
	  &fMTonePtr->modulationDepth, NULL,
	  (void * (*)) SetModulationDepth_PureTone_FM);
	SetPar_UniParMgr(&pars[PURETONE_FM_MODULATIONFREQUENCY], "MOD_FREQ",
	  "Modulation frequency (Hz).",
	  UNIPAR_REAL,
	  &fMTonePtr->modulationFrequency, NULL,
	  (void * (*)) SetModulationFrequency_PureTone_FM);
	SetPar_UniParMgr(&pars[PURETONE_FM_MODULATIONPHASE], "MOD_PHASE",
	  "Modulation phase (degrees).",
	  UNIPAR_REAL,
	  &fMTonePtr->modulationPhase, NULL,
	  (void * (*)) SetModulationPhase_PureTone_FM);
	SetPar_UniParMgr(&pars[PURETONE_FM_DURATION], "DURATION",
	  "Duration (s).",
	  UNIPAR_REAL,
	  &fMTonePtr->duration, NULL,
	  (void * (*)) SetDuration_PureTone_FM);
	SetPar_UniParMgr(&pars[PURETONE_FM_SAMPLINGINTERVAL], "DT",
	  "Sampling interval, dt (s).",
	  UNIPAR_REAL,
	  &fMTonePtr->dt, NULL,
	  (void * (*)) SetSamplingInterval_PureTone_FM);
	return(TRUE);

}

/****************************** GetUniParListPtr ******************************/

/*
 * This function returns a pointer to the module's universal parameter
 * list.
 */

UniParListPtr
GetUniParListPtr_PureTone_FM(void)
{
	static const char	*funcName = "GetUniParListPtr_PureTone_FM";

	if (fMTonePtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (fMTonePtr->parList == NULL) {
		NotifyError("%s: UniParList data structure has not been initialised. "
		  "NULL returned.", funcName);
		return(NULL);
	}
	return(fMTonePtr->parList);

}

/****************************** SetPars ***************************************/

/*
 * This function sets all the module's parameters.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetPars_PureTone_FM(double frequency, double intensity,
  double duration, double samplingInterval, double phase, double modulationDepth,
  double modulationFrequency, double modulationPhase)
{
	static const char	*funcName = "SetPars_PureTone_FM";
	BOOLN	ok;

	ok = TRUE;
	if (!SetFrequency_PureTone_FM(frequency))
		ok = FALSE;
	if (!SetIntensity_PureTone_FM(intensity))
		ok = FALSE;
	if (!SetDuration_PureTone_FM(duration))
		ok = FALSE;
	if (!SetSamplingInterval_PureTone_FM(samplingInterval))
		ok = FALSE;
	if (!SetPhase_PureTone_FM(phase))
		ok = FALSE;
	if (!SetModulationDepth_PureTone_FM(modulationDepth))
		ok = FALSE;
	if (!SetModulationFrequency_PureTone_FM(modulationFrequency))
		ok = FALSE;
	if (!SetModulationPhase_PureTone_FM(modulationPhase))
		ok = FALSE;
	if (!ok)
		NotifyError("%s: Failed to set all module parameters." ,funcName);
	return(ok);

}

/****************************** SetFrequency **********************************/

/*
 * This function sets the module's frequency parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetFrequency_PureTone_FM(double theFrequency)
{
	static const char	*funcName = "SetFrequency_PureTone_FM";

	if (fMTonePtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any required checks here. ***/
	fMTonePtr->frequencyFlag = TRUE;
	fMTonePtr->frequency = theFrequency;
	return(TRUE);

}

/****************************** SetIntensity **********************************/

/*
 * This function sets the module's intensity parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetIntensity_PureTone_FM(double theIntensity)
{
	static const char	*funcName = "SetIntensity_PureTone_FM";

	if (fMTonePtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any required checks here. ***/
	fMTonePtr->intensityFlag = TRUE;
	fMTonePtr->intensity = theIntensity;
	return(TRUE);

}

/****************************** SetDuration ***********************************/

/*
 * This function sets the module's duration parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetDuration_PureTone_FM(double theDuration)
{
	static const char	*funcName = "SetDuration_PureTone_FM";

	if (fMTonePtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any required checks here. ***/
	fMTonePtr->durationFlag = TRUE;
	fMTonePtr->duration = theDuration;
	return(TRUE);

}

/****************************** SetSamplingInterval ***************************/

/*
 * This function sets the module's samplingInterval parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetSamplingInterval_PureTone_FM(double theSamplingInterval)
{
	static const char	*funcName = "SetSamplingInterval_PureTone_FM";

	if (fMTonePtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any required checks here. ***/
	fMTonePtr->dtFlag = TRUE;
	fMTonePtr->dt = theSamplingInterval;
	return(TRUE);

}

/****************************** SetPhase **************************************/

/*
 * This function sets the module's phase parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetPhase_PureTone_FM(double thePhase)
{
	static const char	*funcName = "SetPhase_PureTone_FM";

	if (fMTonePtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any required checks here. ***/
	fMTonePtr->phaseFlag = TRUE;
	fMTonePtr->phase = thePhase;
	return(TRUE);

}

/****************************** SetModulationDepth *****************************/

/*
 * This function sets the module's modulationDepth parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetModulationDepth_PureTone_FM(double theModulationDepth)
{
	static const char	*funcName = "SetModulationDepth_PureTone_FM";

	if (fMTonePtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any required checks here. ***/
	fMTonePtr->modulationDepthFlag = TRUE;
	fMTonePtr->modulationDepth = theModulationDepth;
	return(TRUE);

}

/****************************** SetModulationFrequency *************************/

/*
 * This function sets the module's modulationFrequency parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetModulationFrequency_PureTone_FM(double theModulationFrequency)
{
	static const char	*funcName = "SetModulationFrequency_PureTone_FM";

	if (fMTonePtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any required checks here. ***/
	fMTonePtr->modulationFrequencyFlag = TRUE;
	fMTonePtr->modulationFrequency = theModulationFrequency;
	return(TRUE);

}

/****************************** SetModulationPhase *****************************/

/*
 * This function sets the module's modulationPhase parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetModulationPhase_PureTone_FM(double theModulationPhase)
{
	static const char	*funcName = "SetModulationPhase_PureTone_FM";

	if (fMTonePtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any required checks here. ***/
	fMTonePtr->modulationPhaseFlag = TRUE;
	fMTonePtr->modulationPhase = theModulationPhase;
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
CheckPars_PureTone_FM(void)
{
	static const char	*funcName = "CheckPars_PureTone_FM";
	BOOLN	ok;

	ok = TRUE;
	if (fMTonePtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (!fMTonePtr->frequencyFlag) {
		NotifyError("%s: frequency variable not set.", funcName);
		ok = FALSE;
	}
	if (!fMTonePtr->intensityFlag) {
		NotifyError("%s: intensity variable not set.", funcName);
		ok = FALSE;
	}
	if (!fMTonePtr->durationFlag) {
		NotifyError("%s: duration variable not set.", funcName);
		ok = FALSE;
	}
	if (!fMTonePtr->dtFlag) {
		NotifyError("%s: dt variable not set.", funcName);
		ok = FALSE;
	}
	if (!fMTonePtr->phaseFlag) {
		NotifyError("%s: phase variable not set.", funcName);
		ok = FALSE;
	}
	if (!fMTonePtr->modulationDepthFlag) {
		NotifyError("%s: modulationDepth variable not set.", funcName);
		ok = FALSE;
	}
	if (!fMTonePtr->modulationFrequencyFlag) {
		NotifyError("%s: modulationFrequency variable not set.", funcName);
		ok = FALSE;
	}
	if (!fMTonePtr->modulationPhaseFlag) {
		NotifyError("%s: modulationPhase variable not set.", funcName);
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
PrintPars_PureTone_FM(void)
{
	static const char	*funcName = "PrintPars_PureTone_FM";

	if (!CheckPars_PureTone_FM()) {
		NotifyError("%s: Parameters have not been correctly set.", funcName);
		return(FALSE);
	}
	DPrint("Frequeny Modulated PureTone Module "\
	  "Parameters:-\n");
	DPrint("\tFrequency = %g Hz,", fMTonePtr->frequency);
	DPrint("\tIntensity = %g dB SPL\n", fMTonePtr->intensity);
	DPrint("\tDuration = %g ms,",
	  MSEC(fMTonePtr->duration));
	DPrint("\tsamplingInterval = %g ms\n",
	  MSEC(fMTonePtr->dt));
	DPrint("\tPhase = %g degrees,", fMTonePtr->phase);
	DPrint("\tModulation depth = %g %%\n",
	  fMTonePtr->modulationDepth);
	DPrint("\tModulation frequency/phase = %g Hz / %g "
	  "degrees\n", fMTonePtr->modulationFrequency, fMTonePtr->modulationPhase);
	return(TRUE);

}

/****************************** ReadPars **************************************/

/*
 * This program reads a specified number of parameters from a file.
 * It returns FALSE if it fails in any way.n */

BOOLN
ReadPars_PureTone_FM(char *fileName)
{
	static const char	*funcName = "ReadPars_PureTone_FM";
	BOOLN	ok;
	char	*filePath;
	double	frequency, intensity, duration, samplingInterval, phase;
	double	modulationDepth, modulationFrequency, modulationPhase;
	FILE	*fp;

	filePath = GetParsFileFPath_Common(fileName);
	if ((fp = fopen(filePath, "r")) == NULL) {
		NotifyError("%s: Cannot open data file '%s'.\n", funcName, filePath);
		return(FALSE);
	}
	DPrint("%s: Reading from '%s':\n", funcName, filePath);
	Init_ParFile();
	ok = TRUE;
	if (!GetPars_ParFile(fp, "%lf", &frequency))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &intensity))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &phase))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &modulationDepth))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &modulationFrequency))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &modulationPhase))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &duration))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &samplingInterval))
		ok = FALSE;
	fclose(fp);
	Free_ParFile();
	if (!ok) {
		NotifyError("%s: Not enough lines, or invalid parameters, in module "
		  "parameter file '%s'.", funcName, filePath);
		return(FALSE);
	}
	if (!SetPars_PureTone_FM(frequency, intensity, duration, samplingInterval,
	  phase, modulationDepth, modulationFrequency, modulationPhase)) {
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
SetParsPointer_PureTone_FM(ModulePtr theModule)
{
	static const char	*funcName = "SetParsPointer_PureTone_FM";

	if (!theModule) {
		NotifyError("%s: The module is not set.", funcName);
		return(FALSE);
	}
	fMTonePtr = (FMTonePtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 */

BOOLN
InitModule_PureTone_FM(ModulePtr theModule)
{
	static const char	*funcName = "InitModule_PureTone_FM";

	if (!SetParsPointer_PureTone_FM(theModule)) {
		NotifyError("%s: Cannot set parameters pointer.", funcName);
		return(FALSE);
	}
	if (!Init_PureTone_FM(GLOBAL)) {
		NotifyError("%s: Could not initialise process structure.", funcName);
		return(FALSE);
	}
	theModule->parsPtr = fMTonePtr;
	theModule->CheckPars = CheckPars_PureTone_FM;
	theModule->Free = Free_PureTone_FM;
	theModule->GetUniParListPtr = GetUniParListPtr_PureTone_FM;
	theModule->PrintPars = PrintPars_PureTone_FM;
	theModule->ReadPars = ReadPars_PureTone_FM;
	theModule->RunProcess = GenerateSignal_PureTone_FM;
	theModule->SetParsPointer = SetParsPointer_PureTone_FM;
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
CheckData_PureTone_FM(EarObjectPtr data)
{
	static const char	*funcName = "CheckData_PureTone_FM";

	if (data == NULL) {
		NotifyError("%s: EarObject not initialised.", funcName);
		return(FALSE);
	}
	/*** Put additional checks here. ***/
	return(TRUE);

}

/****************************** GenerateSignal ********************************/

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
GenerateSignal_PureTone_FM(EarObjectPtr data)
{
	static const char	*funcName = "GenerateSignal_PureTone_FM";
	ChanLen		i, t;
	register	double		amplitude, modulationIndex;
	register	ChanData	 *dataPtr;

	if (!CheckPars_PureTone_FM())
		return(FALSE);
	if (!CheckData_PureTone_FM(data)) {
		NotifyError("%s: Process data invalid.", funcName);
		return(FALSE);
	}
	SetProcessName_EarObject(data, "Frequency Modulated Pure Tone stimulus");
	if ( !InitOutSignal_EarObject(data, 1, (ChanLen) (fMTonePtr->duration /
	  fMTonePtr->dt + 1.5), fMTonePtr->dt) ) {
		NotifyError("%s: Cannot initialise output signal", funcName);
		return(FALSE);
	}
	modulationIndex = fMTonePtr->modulationDepth / 100.0 * fMTonePtr->frequency /
	  fMTonePtr->modulationFrequency;
	amplitude = RMS_AMP(fMTonePtr->intensity) * SQRT_2;
	dataPtr = data->outSignal->channel[0];
	for (i = 0, t = data->timeIndex; i < data->outSignal->length; i++, t++)
		*(dataPtr++) = amplitude * sin(PIx2 * fMTonePtr->frequency *
		  t * data->outSignal->dt - modulationIndex * cos(2 * PI *
		  fMTonePtr->modulationFrequency * t * data->outSignal->dt +
		  DEGREES_TO_RADS(fMTonePtr->modulationPhase)) +
		    DEGREES_TO_RADS(fMTonePtr->phase));

	SetProcessContinuity_EarObject(data);
	return(TRUE);

}

