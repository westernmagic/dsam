/**********************
 *
 * File:		StMPPTone.c
 * Purpose:		The module generates a signal which contains multiple puretone
 *				pulses at different frequencies.
 * Comments:	
 * Author:		L. P. O'Mard
 * Created:		18 Feb 1993
 * Updated:		12 Mar 1997
 * Copyright:	(c) 1997, University of Essex
 *
 *********************/


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
#include "StMPPTone.h"

/********************************* Global variables ***************************/

PureTone4Ptr	pureTone4Ptr = NULL;

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
Init_PureTone_MultiPulse(ParameterSpecifier parSpec)
{
	static const char *funcName = "Init_PureTone_MultiPulse";

	if (parSpec == GLOBAL) {
		if (pureTone4Ptr != NULL)
			Free_PureTone_MultiPulse();
		if ((pureTone4Ptr = (PureTone4Ptr) malloc(sizeof(PureTone4))) == NULL) {
			NotifyError("%s: Out of memory for 'global'pointer", funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (pureTone4Ptr == NULL) { 
			NotifyError("%s:  'local' pointer not set.", funcName);
			return(FALSE);
		}
	}
	pureTone4Ptr->parSpec = parSpec;
	pureTone4Ptr->intensityFlag = FALSE;
	pureTone4Ptr->durationFlag = FALSE;
	pureTone4Ptr->dtFlag = FALSE;
	pureTone4Ptr->beginPeriodDurationFlag = FALSE;
	pureTone4Ptr->pulseDurationFlag = FALSE;
	pureTone4Ptr->repetitionPeriodFlag = FALSE;
	pureTone4Ptr->numPulsesFlag = FALSE;
	pureTone4Ptr->numPulses = 0;
	pureTone4Ptr->intensity = 0.0;
	pureTone4Ptr->duration = 0.0;
	pureTone4Ptr->dt = 0.0;
	pureTone4Ptr->beginPeriodDuration = 0.0;
	pureTone4Ptr->pulseDuration = 0.0;
	pureTone4Ptr->repetitionPeriod = 0.0;
	pureTone4Ptr->frequencies = NULL;

	if (!SetUniParList_PureTone_MultiPulse()) {
		NotifyError("%s: Could not initialise parameter list.", funcName);
		Free_PureTone_MultiPulse();
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
Free_PureTone_MultiPulse(void)
{
	if (pureTone4Ptr == NULL)
		return(TRUE);
	free(pureTone4Ptr->frequencies);
	pureTone4Ptr->frequencies = NULL;
	if (pureTone4Ptr->parList)
		FreeList_UniParMgr(&pureTone4Ptr->parList);
	if (pureTone4Ptr->parSpec == GLOBAL) {
		free(pureTone4Ptr);
		pureTone4Ptr = NULL;
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
SetUniParList_PureTone_MultiPulse(void)
{
	static const char *funcName = "SetUniParList_PureTone_Multi";
	UniParPtr	pars;

	if ((pureTone4Ptr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  PURETONE_MULTIPULSE_NUM_PARS, NULL)) == NULL) {
		NotifyError("%s: Could not initialise parList.", funcName);
		return(FALSE);
	}
	pars = pureTone4Ptr->parList->pars;
	SetPar_UniParMgr(&pars[PURETONE_MULTIPULSE_NUMPULSES], "NUM_PULSES",
	  "Number of Pulse Frequencies",
	  UNIPAR_INT,
	  &pureTone4Ptr->numPulses, NULL,
	  (void * (*)) SetNumPulses_PureTone_MultiPulse);
	SetPar_UniParMgr(&pars[PURETONE_MULTIPULSE_FREQUENCIES], "PULSE_FREQ",
	  "Pulse frequencies (Hz).",
	  UNIPAR_REAL_ARRAY,
	  &pureTone4Ptr->frequencies, &pureTone4Ptr->numPulses,
	  (void * (*)) SetIndividualFreq_PureTone_MultiPulse);
	SetPar_UniParMgr(&pars[PURETONE_MULTIPULSE_INTENSITY], "INTENSITY",
	  "Intensity (dB SPL).",
	  UNIPAR_REAL,
	  &pureTone4Ptr->intensity, NULL,
	  (void * (*)) SetIntensity_PureTone_MultiPulse);
	SetPar_UniParMgr(&pars[PURETONE_MULTIPULSE_BEGINPERIODDURATION], "SILENCE",
	  "Silent period, before first pulse (s).",
	  UNIPAR_REAL,
	  &pureTone4Ptr->beginPeriodDuration, NULL,
	  (void * (*)) SetBeginPeriodDuration_PureTone_MultiPulse);
	SetPar_UniParMgr(&pars[PURETONE_MULTIPULSE_PULSEDURATION], "PULSE_DURATION",
	  "Pulse duration (s).",
	  UNIPAR_REAL,
	  &pureTone4Ptr->pulseDuration, NULL,
	  (void * (*)) SetPulseDuration_PureTone_MultiPulse);
	SetPar_UniParMgr(&pars[PURETONE_MULTIPULSE_REPETITIONPERIOD], "REP_PERIOD",
	  "Repetition period (s).",
	  UNIPAR_REAL,
	  &pureTone4Ptr->repetitionPeriod, NULL,
	  (void * (*)) SetRepetitionPeriod_PureTone_MultiPulse);
	SetPar_UniParMgr(&pars[PURETONE_MULTIPULSE_DURATION], "DURATION",
	  "Duration (s).",
	  UNIPAR_REAL,
	  &pureTone4Ptr->duration, NULL,
	  (void * (*)) SetDuration_PureTone_MultiPulse);
	SetPar_UniParMgr(&pars[PURETONE_MULTIPULSE_SAMPLINGINTERVAL], "DT",
	  "Stimulus sampling interval, dt (s).",
	  UNIPAR_REAL,
	  &pureTone4Ptr->dt, NULL,
	  (void * (*)) SetSamplingInterval_PureTone_MultiPulse);
	return(TRUE);

}

/****************************** GetUniParListPtr ******************************/

/*
 * This function returns a pointer to the module's universal parameter
 * list.
 */

UniParListPtr
GetUniParListPtr_PureTone_MultiPulse(void)
{
	static const char	*funcName = "GetUniParListPtr_PureTone_Multi";

	if (pureTone4Ptr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (pureTone4Ptr->parList == NULL) {
		NotifyError("%s: UniParList data structure has not been initialised. "
		  "NULL returned.", funcName);
		return(NULL);
	}
	return(pureTone4Ptr->parList);

}

/****************************** AllocNumPulses ********************************/

/*
 * This function allocates the memory for the pure tone arrays.
 * It will assume that nothing needs to be done if the 'numPulses' 
 * variable is the same as the current structure member value.
 * To make this work, the function needs to set the structure 'numPulses'
 * parameter too.
 * It returns FALSE if it fails in any way.
 */

BOOLN
AllocNumPulses_PureTone_MultiPulse(int numPulses)
{
	static const char	*funcName = "AllocNumPulses_PureTone_MultiPulse";

	if (numPulses == pureTone4Ptr->numPulses)
		return(TRUE);
	if (pureTone4Ptr->frequencies)
		free(pureTone4Ptr->frequencies);
	if ((pureTone4Ptr->frequencies = (double *) calloc(numPulses, sizeof(
	  double))) == NULL) {
		NotifyError("%s: Cannot allocate memory for '%d' frequencies.",
		  funcName, numPulses);
		return(FALSE);
	}
	pureTone4Ptr->numPulses = numPulses;
	return(TRUE);

}

/********************************* CheckPars **********************************/

/*
 * This routine checks that the necessary parameters for the module have been
 * correctly initialised.
 * It checks that the Nyquist critical intensity is not exceeded.
 * It returns TRUE if there are no problems.
 */
 
BOOLN
CheckPars_PureTone_MultiPulse(void)
{
	static const char *funcName = "CheckPars_PureTone_MultiPulse";
	BOOLN	ok;
	int		i;
	double	criticalFrequency;
	
	ok = TRUE;
	if (pureTone4Ptr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (!pureTone4Ptr->intensityFlag) {
		NotifyError("%s: Intensity variable not set.", funcName);
		ok = FALSE;
	}
	if (pureTone4Ptr->frequencies == NULL) {
		NotifyError("%s: intensites array not set.", funcName);
		ok = FALSE;
	}
	if (!pureTone4Ptr->durationFlag) {
		NotifyError("%s: duration variable not set.", funcName);
		ok = FALSE;
	}
	if (!pureTone4Ptr->beginPeriodDurationFlag) {
		NotifyError("%s: beginPeriodDuration variable not "\
		  "set.", funcName);
		ok = FALSE;
	}
	if (!pureTone4Ptr->pulseDurationFlag) {
		NotifyError("%s: pulseDuration variable not set.", funcName);
		ok = FALSE;
	}
	if (!pureTone4Ptr->dtFlag) {
		NotifyError("%s: dt variable not set.", funcName);
		ok = FALSE;
	}
	if (!pureTone4Ptr->repetitionPeriodFlag) {
		NotifyError("%s: repetitionPeriod variable not set.", funcName);
		ok = FALSE;
	}
	if (!pureTone4Ptr->numPulsesFlag) {
		NotifyError("%s: numPulses variable not set.", funcName);
		ok = FALSE;
	}
	if ( (pureTone4Ptr->pulseDuration > pureTone4Ptr->duration) ) {
	  	NotifyError("%s: The pulse duration  is longer than the signal "\
		  "duration.", funcName);
	  	ok = FALSE;
	}
	if ( (pureTone4Ptr->pulseDuration > pureTone4Ptr->repetitionPeriod) ) {
	  	NotifyError("%s: The pulse duration  is longer than the repetition "\
		  "period.", funcName);
	  	ok = FALSE;
	}
	criticalFrequency = 1.0 / (2.0 * pureTone4Ptr->dt);
	if (ok)
		for (i = 0; i < pureTone4Ptr->numPulses; i++)
			if (criticalFrequency <= pureTone4Ptr->frequencies[i]) {
				NotifyError("%s: Sampling rate (dt = %g ms) is too low for "\
				  "frequency No. %d.", funcName, MSEC(pureTone4Ptr->dt), i);
				ok = FALSE;
			}
	return(ok);

}	

/********************************* SetIntensity *******************************/

/*
 * This function sets the module's intensity parameter.  It first checks that
 * the module has been initialised.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetIntensity_PureTone_MultiPulse(double theIntensity)
{
	static const char *funcName = "SetIntensity_PureTone_MultiPulse";

	if (pureTone4Ptr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	pureTone4Ptr->intensityFlag = TRUE;
	pureTone4Ptr->intensity = theIntensity;
	return(TRUE);

}

/********************************* SetFrequencies *****************************/

/*
 * This function sets up the module's frequencies array.
 * The pointer to the frequencies lists is passed to the module.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetFrequencies_PureTone_MultiPulse(double *theFrequencies)
{
	static const char *funcName = "SetFrequencies_PureTone_MultiPulse";

	if (pureTone4Ptr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	pureTone4Ptr->frequencies = theFrequencies;
	return(TRUE);

}

/****************************** SetNumPulses **********************************/

/*
 * This function sets the module's numPulses parameter.
 * It returns TRUE if the operation is successful.
 * The 'numPulses' variable is set by the 'AllocNumPulses_PureTone_MultiPulse'
 * routine.
 * Additional checks should be added as required.
 */

BOOLN
SetNumPulses_PureTone_MultiPulse(int theNumPulses)
{
	static const char	*funcName = "SetNumPulses_PureTone_MultiPulse";

	if (pureTone4Ptr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (theNumPulses < 1) {
		NotifyError("%s: Value must be greater then zero (%d).", funcName,
		  theNumPulses);
		return(FALSE);
	}
	if (!AllocNumPulses_PureTone_MultiPulse(theNumPulses)) {
		NotifyError("%%s: Cannot allocate memory for the 'numPulses' arrays.",
		  funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	pureTone4Ptr->numPulsesFlag = TRUE;
	return(TRUE);

}

/********************************* SetIndividualFreq **************************/

/*
 * This function sets the individual frequency values.
 * It first checks if the frequencies have been set.
 */

BOOLN
SetIndividualFreq_PureTone_MultiPulse(int theIndex, double theFrequency)
{
	static const char *funcName = "SetIndividualFreq_PureTone_MultiPulse";

	if (pureTone4Ptr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (pureTone4Ptr->frequencies == NULL) {
		NotifyError("%s: Frequencies not set.", funcName);
		return(FALSE);
	}
	if (theIndex > pureTone4Ptr->numPulses - 1) {
		NotifyError("%s: Index value must be in the\nrange 0 - %d (%d).\n",
		  funcName, pureTone4Ptr->numPulses - 1, theIndex);
		return(FALSE);
	}
	pureTone4Ptr->frequencies[theIndex] = theFrequency;
	return(TRUE);

}

/********************************* SetDuration ********************************/

/*
 * This function sets the module's duration parameter.  It first checks that
 * the module has been initialised.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetDuration_PureTone_MultiPulse(double theDuration)
{
	static const char *funcName = "SetDuration_PureTone_MultiPulse";

	if (pureTone4Ptr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	pureTone4Ptr->durationFlag = TRUE;
	pureTone4Ptr->duration = theDuration;
	return(TRUE);

}

/********************************* SetBeginPeriodDuration *********************/

/*
 * This function sets the module's beginPeriodDuration parameter.  It first
 * checks that the module has been initialised.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetBeginPeriodDuration_PureTone_MultiPulse(double theBeginPeriodDuration)
{
	static const char *funcName = "SetBeginPeriodDuration_PureTone_MultiPulse";

	if (pureTone4Ptr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	pureTone4Ptr->beginPeriodDurationFlag = TRUE;
	pureTone4Ptr->beginPeriodDuration = theBeginPeriodDuration;
	return(TRUE);

}

/********************************* SetPulseDuration ***************************/

/*
 * This function sets the module's pulseDuration parameter.  It first checks
 * that the module has been initialised.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetPulseDuration_PureTone_MultiPulse(double thePulseDuration)
{
	static const char *funcName = "SetPulseDuration_PureTone_MultiPulse";

	if (pureTone4Ptr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (thePulseDuration < 0.0) {
		NotifyError("%s: Illegal negative time!", funcName);
		return(FALSE);
	}
	pureTone4Ptr->pulseDurationFlag = TRUE;
	pureTone4Ptr->pulseDuration = thePulseDuration;
	return(TRUE);

}

/********************************* SetRepetitionPeriod ************************/

/*
 * This function sets the module's repetitionPeriod parameter.  It first checks
 * that the module has been initialised.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetRepetitionPeriod_PureTone_MultiPulse(double theRepetitionPeriod)
{
	static const char *funcName = "SetRepetitionPeriod_PureTone_MultiPulse";

	if (pureTone4Ptr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (theRepetitionPeriod < 0.0) {
		NotifyError("%s: Illegal negative time!", funcName);
		return(FALSE);
	}
	pureTone4Ptr->repetitionPeriodFlag = TRUE;
	pureTone4Ptr->repetitionPeriod = theRepetitionPeriod;
	return(TRUE);

}

/********************************* SetSamplingInterval ************************/

/*
 * This function sets the module's timeStep parameter.  It first checks that
 * the module has been initialised.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetSamplingInterval_PureTone_MultiPulse(double theSamplingInterval)
{
	static const char *funcName = "SetSamplingInterval_PureTone_MultiPulse";

	if (pureTone4Ptr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if ( theSamplingInterval <= 0.0 ) {
		NotifyError("%s: Illegal sampling interval value = %g!", funcName,
		  theSamplingInterval);
		return(FALSE);
	}
	pureTone4Ptr->dtFlag = TRUE;
	pureTone4Ptr->dt = theSamplingInterval;
	return(TRUE);

}

/****************************** SetPars ***************************************/

/*
 * This function sets all the module's parameters.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetPars_PureTone_MultiPulse(int numPulses, double *frequencies,
  double intensity, double beginPeriodDuration, double pulseDuration,
  double repetitionPeriod, double duration, double samplingInterval)
{
	static const char	*funcName = "SetPars_PureTone_MultiPulse";
	BOOLN	ok;

	ok = TRUE;
	if (!SetNumPulses_PureTone_MultiPulse(numPulses))
		ok = FALSE;
	if (!SetFrequencies_PureTone_MultiPulse(frequencies))
		ok = FALSE;
	if (!SetIntensity_PureTone_MultiPulse(intensity))
		ok = FALSE;
	if (!SetBeginPeriodDuration_PureTone_MultiPulse(beginPeriodDuration))
		ok = FALSE;
	if (!SetPulseDuration_PureTone_MultiPulse(pulseDuration))
		ok = FALSE;
	if (!SetRepetitionPeriod_PureTone_MultiPulse(repetitionPeriod))
		ok = FALSE;
	if (!SetDuration_PureTone_MultiPulse(duration))
		ok = FALSE;
	if (!SetSamplingInterval_PureTone_MultiPulse(samplingInterval))
		ok = FALSE;
	if (!ok)
		NotifyError("%s: Failed to set all module parameters." ,funcName);
	return(ok);

}

/****************************** GetIndividualFreq *****************************/

/*
 * This routine returns an individual frequency from the frequency list.
 */

double
GetIndividualFreq_PureTone_MultiPulse(int index)
{
	static const char *funcName = "GetIndividualFreq_PureTone_MultiPulse";

	if (pureTone4Ptr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (!CheckPars_PureTone_MultiPulse()) {
		NotifyError("%s: Parameters have not been correctly\nset.  Returning "\
		  "zero.", funcName);
		return(0.0);
	}
	if (index < 0 || index >= pureTone4Ptr->numPulses) {
		NotifyError("%s: The valid index range is 0 to %d.  Returning zero.\n",
		  funcName, pureTone4Ptr->numPulses - 1);
		return(0.0);
	}
	return(pureTone4Ptr->frequencies[index]);
	
}

/****************************** PrintPars *************************************/

/*
 * This program prints the parameters of the module to the standard output.
 */
 
BOOLN
PrintPars_PureTone_MultiPulse(void)
{
	static const char *funcName = "PrintPars_PureTone_MultiPulse";
	int	i;
	
	if (!CheckPars_PureTone_MultiPulse()) {
		NotifyError("%s: Parameters have not been correctly set.", funcName);
		return(FALSE);
	}
	DPrint("Multi-Frequency Pulsed Pure Tone Module "\
	  "Parameters:-\n");
	DPrint("\tIntensity = %g (Hz)\n", pureTone4Ptr->intensity);
	DPrint("\tDuration = %g ms,\tSampling interval = %g ms\n",
	  MSEC(pureTone4Ptr->duration), MSEC(pureTone4Ptr->dt));
	DPrint("\t%10s\t%s\n", "Pulse No.", "Frequency (Hz)");
	for (i = 0; i < pureTone4Ptr->numPulses; i++)
		DPrint("\t%10d\t%10g\n", i,
		  pureTone4Ptr->frequencies[i]);
	DPrint("\tBegin silence = %g ms,\tPulse duration = %g "\
	  "ms\n", MSEC(pureTone4Ptr->beginPeriodDuration),
	  MSEC(pureTone4Ptr->pulseDuration));
	DPrint("\tRepetition period = %g ms.\n",
	  MSEC(pureTone4Ptr->repetitionPeriod));
	return(TRUE);

}

/****************************** ReadPars **************************************/

/*
 * This program reads a specified number of parameters from a file.
 * It returns FALSE if it fails in any way.n */

BOOLN
ReadPars_PureTone_MultiPulse(char *fileName)
{
	static const char	*funcName = "ReadPars_PureTone_MultiPulse";
	BOOLN	ok;
	char	*filePath;
	int		i, numPulses;
	double	intensity, beginPeriodDuration, pulseDuration, repetitionPeriod;
	double	duration, samplingInterval;
	FILE	*fp;

	filePath = GetParsFileFPath_Common(fileName);
	if ((fp = fopen(filePath, "r")) == NULL) {
		NotifyError("%s: Cannot open data file '%s'.\n", funcName, fileName);
		return(FALSE);
	}
	DPrint("%s: Reading from '%s':\n", funcName, fileName);
	Init_ParFile();
	ok = TRUE;
	if (!GetPars_ParFile(fp, "%d", &numPulses))
		ok = FALSE;
	if (!AllocNumPulses_PureTone_MultiPulse(numPulses)) {
		NotifyError("%%s: Cannot allocate memory for the 'numPulses' arrays.",
		  funcName);
		return(FALSE);
	}
	for (i = 0; i < numPulses; i++)
		if (!GetPars_ParFile(fp, "%lf", &pureTone4Ptr->frequencies[i]))
			ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &intensity))
		ok = FALSE;
    if (!GetPars_ParFile(fp, "%lf", &duration))
		ok = FALSE;
    if (!GetPars_ParFile(fp, "%lf", &beginPeriodDuration))
		ok = FALSE;
    if (!GetPars_ParFile(fp, "%lf", &pulseDuration))
		ok = FALSE;
    if (!GetPars_ParFile(fp, "%lf", &repetitionPeriod))
		ok = FALSE;
    if (!GetPars_ParFile(fp, "%lf", &samplingInterval))
		ok = FALSE;
	fclose(fp);
	Free_ParFile();
	if (!ok) {
		NotifyError("%s: Not enough lines, or invalid parameters, in module "
		  "parameter file '%s'.", funcName, fileName);
		return(FALSE);
	}
	if (!SetPars_PureTone_MultiPulse(numPulses, pureTone4Ptr->frequencies,
	  intensity, beginPeriodDuration, pulseDuration, repetitionPeriod,
	  duration, samplingInterval)) {
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
SetParsPointer_PureTone_MultiPulse(ModulePtr theModule)
{
	static const char	*funcName = "SetParsPointer_PureTone_MultiPulse";

	if (!theModule) {
		NotifyError("%s: The module is not set.", funcName);
		return(FALSE);
	}
	pureTone4Ptr = (PureTone4Ptr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 */

BOOLN
InitModule_PureTone_MultiPulse(ModulePtr theModule)
{
	static const char	*funcName = "InitModule_PureTone_MultiPulse";

	if (!SetParsPointer_PureTone_MultiPulse(theModule)) {
		NotifyError("%s: Cannot set parameters pointer.", funcName);
		return(FALSE);
	}
	if (!Init_PureTone_MultiPulse(GLOBAL)) {
		NotifyError("%s: Could not initialise process structure.", funcName);
		return(FALSE);
	}
	theModule->parsPtr = pureTone4Ptr;
	theModule->CheckPars = CheckPars_PureTone_MultiPulse;
	theModule->Free = Free_PureTone_MultiPulse;
	theModule->GetUniParListPtr = GetUniParListPtr_PureTone_MultiPulse;
	theModule->PrintPars = PrintPars_PureTone_MultiPulse;
	theModule->ReadPars = ReadPars_PureTone_MultiPulse;
	theModule->RunProcess = GenerateSignal_PureTone_MultiPulse;
	theModule->SetParsPointer = SetParsPointer_PureTone_MultiPulse;
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
GenerateSignal_PureTone_MultiPulse(EarObjectPtr data)
{
	static const char *funcName = "GenerateSignal_PureTone_MultiPulse";
	double		amplitude;
	register	ChanData	*dataPtr;
	ChanLen		i, t, pulseDurationIndex, repetitionPeriodIndex;

	if (data == NULL) {
		NotifyError("%s: EarObject not initialised.", funcName);
		return(FALSE);
	}	
	if (!CheckPars_PureTone_MultiPulse())
		return(FALSE);
	SetProcessName_EarObject(data, "Multi-frequency pulsed pure-tone stimulus");
	if ( !InitOutSignal_EarObject(data, PURE_TONE_3_NUM_CHANNELS,
	  (ChanLen) (pureTone4Ptr->duration / pureTone4Ptr->dt + 1.5),
	  pureTone4Ptr->dt) ) {
		NotifyError("%s: Cannot initialise output signal", funcName);
		return(FALSE);
	}
	amplitude = RMS_AMP(pureTone4Ptr->intensity) * SQRT_2;
	pulseDurationIndex = (ChanLen) floor(pureTone4Ptr->pulseDuration /
	  pureTone4Ptr->dt + 0.5);
	repetitionPeriodIndex = (ChanLen) floor(pureTone4Ptr->repetitionPeriod /
	  pureTone4Ptr->dt + 0.5);
	if (data->timeIndex == PROCESS_START_TIME) {
		pureTone4Ptr->beginIndex = (ChanLen) floor(pureTone4Ptr->
		  beginPeriodDuration / data->outSignal->dt + 0.5);
		pureTone4Ptr->pulseOn = TRUE;
		pureTone4Ptr->pulseNumber = 0;
		pureTone4Ptr->pulseCount = 0;
	}
	dataPtr = data->outSignal->channel[0];
	for (i = 0, t = data->timeIndex; i < data->outSignal->length; i++, t++,
	  dataPtr++) {
	  	if (t < pureTone4Ptr->beginIndex)
	  		continue;
		if (pureTone4Ptr->pulseOn) {
			*dataPtr = amplitude * sin(PIx2 *
			  pureTone4Ptr->frequencies[pureTone4Ptr->pulseNumber] *
			  (pureTone4Ptr->pulseCount++ * data->outSignal->dt));
			pureTone4Ptr->pulseOn = (pureTone4Ptr->pulseCount <
			  pulseDurationIndex);
		} else {
			*dataPtr = 0.0;
			if (((t - pureTone4Ptr->beginIndex) % repetitionPeriodIndex) == 1) {
				pureTone4Ptr->pulseOn = TRUE;
				pureTone4Ptr->pulseCount = 0;
				pureTone4Ptr->pulseNumber++;
				pureTone4Ptr->pulseNumber %= pureTone4Ptr->numPulses;
			}
		}
	}
	SetProcessContinuity_EarObject(data);
	return(TRUE);

} /* GenerateSignal_PureTone_MultiPulse */

