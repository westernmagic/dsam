/**********************
 *
 * File:		StMPTone.c
 * Purpose:		This module contains the methods for the multiple pure-tone
 *				signal generation paradigm.
 * Comments:	
 * Author:		L. P. O'Mard
 * Created:		12 Jul 1993
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
#include "StMPTone.h"
#include "FiParFile.h"

/******************************************************************************/
/********************************* Global variables ***************************/
/******************************************************************************/

MPureTonePtr	mPureTonePtr = NULL;

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
Init_PureTone_Multi(ParameterSpecifier parSpec)
{
	static const char	*funcName = "Init_PureTone_Multi";

	if (parSpec == GLOBAL) {
		if (mPureTonePtr != NULL)
			free(mPureTonePtr);
		if ((mPureTonePtr = (MPureTonePtr) malloc(sizeof(MPureTone))) == NULL) {
			NotifyError("%s: Out of memory for 'global' pointer", funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (mPureTonePtr == NULL) { 
			NotifyError("%s:  'local' pointer not set.", funcName);
			return(FALSE);
		}
	}
	mPureTonePtr->parSpec = parSpec;
	mPureTonePtr->numPTonesFlag = FALSE;
	mPureTonePtr->durationFlag = FALSE;
	mPureTonePtr->dtFlag = FALSE;
	mPureTonePtr->numPTones = 0;
	mPureTonePtr->duration = 0.0;
	mPureTonePtr->dt = 0.0;
	mPureTonePtr->intensities = NULL;
	mPureTonePtr->frequencies = NULL;
	mPureTonePtr->phases = NULL;

	if (!SetUniParList_PureTone_Multi()) {
		NotifyError("%s: Could not initialise parameter list.", funcName);
		Free_PureTone_Multi();
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
Free_PureTone_Multi(void)
{
	if (mPureTonePtr == NULL)
		return(TRUE);
	free(mPureTonePtr->intensities);
	mPureTonePtr->intensities = NULL;
	free(mPureTonePtr->frequencies);
	mPureTonePtr->frequencies = NULL;
	free(mPureTonePtr->phases);
	mPureTonePtr->phases = NULL;
	if (mPureTonePtr->parList)
		FreeList_UniParMgr(&mPureTonePtr->parList);
	if (mPureTonePtr->parSpec == GLOBAL) {
		free(mPureTonePtr);
		mPureTonePtr = NULL;
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
SetUniParList_PureTone_Multi(void)
{
	static const char *funcName = "SetUniParList_PureTone_Multi";
	UniParPtr	pars;

	if ((mPureTonePtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  PURETONE_MULTI_NUM_PARS, NULL)) == NULL) {
		NotifyError("%s: Could not initialise parList.", funcName);
		return(FALSE);
	}
	pars = mPureTonePtr->parList->pars;
	SetPar_UniParMgr(&pars[PURETONE_MULTI_NUMPTONES], "NUM",
	  "Number of pure tones.",
	  UNIPAR_INT,
	  &mPureTonePtr->numPTones, NULL,
	  (void * (*)) SetNumPTones_PureTone_Multi);
	SetPar_UniParMgr(&pars[PURETONE_MULTI_FREQUENCIES], "FREQUENCY",
	  "Pure tone frequencies (Hz).",
	  UNIPAR_REAL_ARRAY,
	  &mPureTonePtr->frequencies, &mPureTonePtr->numPTones,
	  (void * (*)) SetIndividualFreq_PureTone_Multi);
	SetPar_UniParMgr(&pars[PURETONE_MULTI_INTENSITIES], "INTENSITY",
	  "Pure tone intensities (dB SPL).",
	  UNIPAR_REAL_ARRAY,
	  &mPureTonePtr->intensities, &mPureTonePtr->numPTones,
	  (void * (*)) SetIndividualIntensity_PureTone_Multi);
	SetPar_UniParMgr(&pars[PURETONE_MULTI_PHASES], "PHASE",
	  "Pure tone phases (degrees).",
	  UNIPAR_REAL_ARRAY,
	  &mPureTonePtr->phases, &mPureTonePtr->numPTones,
	  (void * (*)) SetIndividualPhase_PureTone_Multi);
	SetPar_UniParMgr(&pars[PURETONE_MULTI_DURATION], "DURATION",
	  "Duration (s).",
	  UNIPAR_REAL,
	  &mPureTonePtr->duration, NULL,
	  (void * (*)) SetDuration_PureTone_Multi);
	SetPar_UniParMgr(&pars[PURETONE_MULTI_SAMPLINGINTERVAL], "DT",
	  "Sampling interval, dt (s).",
	  UNIPAR_REAL,
	  &mPureTonePtr->dt, NULL,
	  (void * (*)) SetSamplingInterval_PureTone_Multi);
	return(TRUE);

}

/****************************** GetUniParListPtr ******************************/

/*
 * This function returns a pointer to the module's universal parameter
 * list.
 */

UniParListPtr
GetUniParListPtr_PureTone_Multi(void)
{
	static const char	*funcName = "GetUniParListPtr_PureTone_Multi";

	if (mPureTonePtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (mPureTonePtr->parList == NULL) {
		NotifyError("%s: UniParList data structure has not been initialised. "
		  "NULL returned.", funcName);
		return(NULL);
	}
	return(mPureTonePtr->parList);

}

/********************************* CheckPars **********************************/

/*
 * This routine checks that the necessary parameters for the module have been
 * correctly initialised.
 * It also checks that the Nyquist critical frequency is not exceeded.
 * It returns TRUE if there are no problems.
 */
 
BOOLN
CheckPars_PureTone_Multi(void)
{
	static const char *funcName = "CheckPars_PureTone_Multi";
	BOOLN	ok;
	int		i;
	double	criticalFrequency;
	
	ok = TRUE;
	if (mPureTonePtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (mPureTonePtr->frequencies == NULL) {
		NotifyError("%s: Frequencies array not set.", funcName);
		ok = FALSE;
	}
	if (mPureTonePtr->intensities == NULL) {
		NotifyError("%s: intensities array not set.", funcName);
		ok = FALSE;
	}
	if (mPureTonePtr->phases == NULL) {
		NotifyError("%s: phases array not set.", funcName);
		ok = FALSE;
	}
	if (!mPureTonePtr->numPTones) {
		NotifyError("%s: numPTones variable not set.", funcName);
		ok = FALSE;
	}
	if (!mPureTonePtr->durationFlag) {
		NotifyError("%s: duration variable not set.", funcName);
		ok = FALSE;
	}
	if (!mPureTonePtr->dtFlag) {
		NotifyError("%s: dtFlag variable not set.", funcName);
		ok = FALSE;
	}
	criticalFrequency = 1.0 / (2.0 * mPureTonePtr->dt);
	if (ok)
		for (i = 0; i < mPureTonePtr->numPTones; i++)
			if (criticalFrequency <= mPureTonePtr->frequencies[i]) {
				NotifyError("%s: Sampling rate (dt = %g ms) is too low for "\
				  "one of the stimulus\nfrequencies (frequency[%d] = %g).",
				  funcName, MSEC(mPureTonePtr->dt), i,
				  mPureTonePtr->frequencies[i]);
				ok = FALSE;
			} 
	return(ok);
	
}	

/********************************* SetNumPTones *******************************/

/*
 * This function sets the module's numPTones parameter.  It first checks that
 * the module has been initialised.
 * The 'numPTones' variable is set by the 'AllocNumPTones_PureTone_Multi'
 * routine.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetNumPTones_PureTone_Multi(int theNumPTones)
{
	static const char *funcName = "SetNumPTones_PureTone_Multi";

	if (mPureTonePtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (theNumPTones < 1) {
		NotifyError("%s: Value must be greater then zero (%d).", funcName,
		  theNumPTones);
		return(FALSE);
	}
	if (!AllocNumPTones_PureTone_Multi(theNumPTones)) {
		NotifyError("%s: Cannot allocate memory for numPTone arrays.",
		  funcName);
		return(FALSE);
 	}
	mPureTonePtr->numPTonesFlag = TRUE;
	return(TRUE);

}

/********************************* SetFrequencies *****************************/

/*
 * This function sets up the module's frequencies array.
 * The pointer to the frequencies lists is passed to the module.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetFrequencies_PureTone_Multi(double *theFrequencies)
{
	static const char *funcName = "SetFrequencies_PureTone_Multi";

	if (mPureTonePtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	mPureTonePtr->frequencies = theFrequencies;
	return(TRUE);

}

/********************************* SetIntensities *****************************/

/*
 * This function sets up the module's intensities array.
 * The pointer to the intensities lists is passed to the module.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetIntensities_PureTone_Multi(double *theIntensities)
{
	static const char *funcName = "SetIntensities_PureTone_Multi";

	if (mPureTonePtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	mPureTonePtr->intensities = theIntensities;
	return(TRUE);

}

/********************************* SetPhases **********************************/

/*
 * This function sets up the module's phases array.
 * The pointer to the phases lists is passed to the module.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetPhases_PureTone_Multi(double *thePhases)
{
	static const char *funcName = "SetPhases_PureTone_Multi";

	if (mPureTonePtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	mPureTonePtr->phases = thePhases;
	return(TRUE);

}

/********************************* SetDuration ********************************/

/*
 * This function sets the module's duration parameter.  It first checks that
 * the module has been initialised.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetDuration_PureTone_Multi(double theDuration)
{
	static const char *funcName = "SetDuration_PureTone_Multi";

	if (mPureTonePtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	mPureTonePtr->durationFlag = TRUE;
	mPureTonePtr->duration = theDuration;
	return(TRUE);

}

/********************************* SetSamplingInterval ************************/

/*
 * This function sets the module's samplingInterval parameter.  It first checks
 * that the module has been initialised.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetSamplingInterval_PureTone_Multi(double theSamplingInterval)
{
	static const char *funcName = "SetSamplingInterval_PureTone_Multi";

	if (mPureTonePtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if ( theSamplingInterval <= 0.0 ) {
		NotifyError("%s: Illegal sampling interval value = %g!", funcName,
		  theSamplingInterval);
		return(FALSE);
	}
	mPureTonePtr->dtFlag = TRUE;
	mPureTonePtr->dt = theSamplingInterval;
	return(TRUE);

}

/********************************* SetIndividualFreq **************************/

/*
 * This function sets the individual frequency values.
 * It first checks if the frequencies have been set.
 */

BOOLN
SetIndividualFreq_PureTone_Multi(int theIndex, double theFrequency)
{
	static const char *funcName = "SetIndividualFreq_PureTone_Multi";

	if (mPureTonePtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (mPureTonePtr->frequencies == NULL) {
		NotifyError("%s: Frequencies not set.", funcName);
		return(FALSE);
	}
	if (theIndex > mPureTonePtr->numPTones - 1) {
		NotifyError("%s: Index value must be in the\nrange 0 - %d (%d).\n",
		  funcName, mPureTonePtr->numPTones - 1, theIndex);
		return(FALSE);
	}
	mPureTonePtr->frequencies[theIndex] = theFrequency;
	return(TRUE);

}

/********************************* SetIndividualIntensity *********************/

/*
 * This function sets the individual intensity values.
 * It first checks if the intensities have been set.
 */

BOOLN
SetIndividualIntensity_PureTone_Multi(int theIndex, double theIntensity)
{
	static const char *funcName = "SetIndividualIntensity_PureTone_Multi";

	if (mPureTonePtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (mPureTonePtr->intensities == NULL) {
		NotifyError("%s: Intensities not set.", funcName);
		return(FALSE);
	}
	if (theIndex > mPureTonePtr->numPTones - 1) {
		NotifyError("%s: Index value must be in the\nrange 0 - %d (%d).\n",
		  funcName, mPureTonePtr->numPTones - 1,
		  theIndex);
		return(FALSE);
	}
	mPureTonePtr->intensities[theIndex] = theIntensity;
	return(TRUE);

}

/********************************* SetIndividualPhase *************************/

/*
 * This function sets the individual phase values.
 * It first checks if the phases have been set.
 */

BOOLN
SetIndividualPhase_PureTone_Multi(int theIndex, double thePhase)
{
	static const char *funcName = "SetIndividualPhase_PureTone_Multi";

	if (mPureTonePtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (mPureTonePtr->phases == NULL) {
		NotifyError("%s: Intensities not set.", funcName);
		return(FALSE);
	}
	if (theIndex > mPureTonePtr->numPTones - 1) {
		NotifyError("%s: Index value must be in the\nrange 0 - %d (%d).\n",
		  funcName, mPureTonePtr->numPTones - 1, theIndex);
		return(FALSE);
	}
	mPureTonePtr->phases[theIndex] = thePhase;
	return(TRUE);

}

/********************************* SetPars ************************************/

/*
 * This function sets all the module's parameters.
 * It returns TRUE if the operation is successful.
 */
 
BOOLN
SetPars_PureTone_Multi(int theNumPTones, double *theFrequencies,
  double *theIntensities, double *thePhases, double theDuration,
  double theSamplingInterval)
{
	static const char *funcName = "SetPars_PureTone_Multi";
	BOOLN	ok;
	
	ok = TRUE;
	if (!SetNumPTones_PureTone_Multi(theNumPTones))
		ok = FALSE;
	if (!SetFrequencies_PureTone_Multi(theFrequencies))
		ok = FALSE;
	if (!SetIntensities_PureTone_Multi(theIntensities))
		ok = FALSE;
	if (!SetPhases_PureTone_Multi(thePhases))
		ok = FALSE;
	if (!SetDuration_PureTone_Multi(theDuration))
		ok = FALSE;
	if (!SetSamplingInterval_PureTone_Multi(theSamplingInterval))
		ok = FALSE;
	if (!ok)
		NotifyError("%s: Failed to set all module parameters.", funcName);
	return(ok);
	
}

/****************************** GetIndividualFreq *****************************/

/*
 * This routine returns an individual frequency from the frequency list.
 */

double
GetIndividualFreq_PureTone_Multi(int index)
{
	static const char *funcName = "GetIndividualFreq_PureTone_Multi";

	if (mPureTonePtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (!CheckPars_PureTone_Multi()) {
		NotifyError("%s: Parameters have not been correctly\nset.  Returning "\
		  "zero.", funcName);
		return(0.0);
	}
	if (index < 0 || index >= mPureTonePtr->numPTones) {
		NotifyError("%s: The valid index range is 0 to %d.  Returning zero.\n",
		  funcName, mPureTonePtr->numPTones - 1);
		return(0.0);
	}
	return(mPureTonePtr->frequencies[index]);
	
}

/****************************** GetIndividualIntensity ************************/

/*
 * This routine returns an individual intensity from the intensity list.
 */

double
GetIndividualIntensity_PureTone_Multi(int index)
{
	static const char *funcName = "GetIndividualIntensity_PureTone_Multi";

	if (mPureTonePtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (!CheckPars_PureTone_Multi()) {
		NotifyError("%s: Parameters have not been correctly\nset.  Returning "\
		  "zero.", funcName);
		return(0.0);
	}
	if (index < 0 || index >= mPureTonePtr->numPTones) {
		NotifyError("%s: The valid index range is 0 to %d.  Returning zero.\n",
		  funcName, mPureTonePtr->numPTones - 1);
		return(0.0);
	}
	return(mPureTonePtr->intensities[index]);
	
}

/****************************** PrintPars *************************************/

/*
 * This program prints the parameters of the module to the standard output.
 */
 
BOOLN
PrintPars_PureTone_Multi(void)
{
	static const char *funcName = "PrintPars_PureTone_Multi";
	int		i;
	
	if (!CheckPars_PureTone_Multi()) {
		NotifyError("%s: Parameters have not been correctly set.", funcName);
		return(FALSE);
	}
	DPrint("Multiple Pure Tone Module Parameters:-\n");
	DPrint("\t%10s\t%10s\t%10s\n", "Frequency ",
	  "Intensity", "   Phase  ");
	DPrint("\t%10s\t%10s\t%10s\n", "   (Hz)   ",
	  " (dB SPL)", " (degrees)");
	for (i = 0; i < mPureTonePtr->numPTones; i++)
		DPrint("\t%10g\t%10g\t%10g\n",
		  mPureTonePtr->frequencies[i], mPureTonePtr->intensities[i],
		  mPureTonePtr->phases[i]);
	DPrint("\tDuration = %g ms,\tSampling interval = %g ms\n",
	  MSEC(mPureTonePtr->duration), MSEC(mPureTonePtr->dt));
	return(TRUE);

}

/****************************** AllocNumPTones ********************************/

/*
 * This function allocates the memory for the pure tone arrays.
 * It will assume that nothing needs to be done if the specified numPTones is
 * the same as the current structure member value.
 * To make this work, it needs to set the numPTones parameter too.
 * It returns FALSE if it fails in any way.
 */
 
BOOLN
AllocNumPTones_PureTone_Multi(int numPTones)
{
	static const char *funcName = "AllocNumPTones_PureTone_Multi";

	if (numPTones == mPureTonePtr->numPTones)
		return(TRUE);
	if (mPureTonePtr->frequencies)
		free(mPureTonePtr->frequencies);
	if ((mPureTonePtr->frequencies = (double *) calloc(numPTones, sizeof(
	  double))) == NULL) {
		NotifyError("%s: Cannot allocate memory for %d frequencies.", funcName,
		  numPTones);
		return(FALSE);
 	}
	if (mPureTonePtr->intensities)
		free(mPureTonePtr->intensities);
	if ((mPureTonePtr->intensities = (double *) calloc(numPTones, sizeof(
	  double))) == NULL) {
		NotifyError("%s: Cannot allocate memory for '%d' intensities.",
		  funcName, numPTones);
		return(FALSE);
 	}
	if (mPureTonePtr->phases)
		free(mPureTonePtr->phases);
	if ((mPureTonePtr->phases = (double *) calloc(numPTones, sizeof(
	  double))) == NULL) {
		NotifyError("%s: Cannot allocate memory for '%d' phases.", funcName,
		  numPTones);
		return(FALSE);
 	}
	mPureTonePtr->numPTones = numPTones;
	return(TRUE);

}

/****************************** ReadPars **************************************/

/*
 * This program reads a specified number of parameters from a file.
 * It returns FALSE if it fails in any way.
 */
 
BOOLN
ReadPars_PureTone_Multi(char *fileName)
{
	static const char *funcName = "ReadPars_PureTone_Multi";
	BOOLN	ok;
	char	*filePath;
    int		i, numberOfPTones;
	double  duration, samplingInterval;
	FILE    *fp;
    
	filePath = GetParsFileFPath_Common(fileName);
    if ((fp = fopen(filePath, "r")) == NULL) {
        NotifyError("%s: Cannot open data file '%s'.\n", funcName, filePath);
		return(FALSE);
    }
    DPrint("%s: Reading from '%s':\n", funcName, filePath);
    Init_ParFile();
	ok = TRUE;
    if (!GetPars_ParFile(fp, "%d", &numberOfPTones))
		ok = FALSE;
	if (!AllocNumPTones_PureTone_Multi(numberOfPTones)) {
		NotifyError("%s: Cannot allocate memory for numPTone arrays.",
		funcName);
		ok = FALSE;
 	}
	for (i = 0; ok && (i < numberOfPTones); i++)
		if (!GetPars_ParFile(fp, "%lf %lf %lf", &mPureTonePtr->frequencies[i],
		  &mPureTonePtr->intensities[i], &mPureTonePtr->phases[i]))
			ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &duration))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &samplingInterval))
		ok = FALSE;
    fclose(fp);
    Free_ParFile();
	if (!ok) {
		NotifyError("%s: Not enough lines, or invalid parameters, in module "\
		  "parameter file '%s'.", funcName, filePath);
		return(FALSE);
	}
	if (!SetPars_PureTone_Multi(numberOfPTones, mPureTonePtr->frequencies,
	  mPureTonePtr->intensities, mPureTonePtr->phases, duration,
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
SetParsPointer_PureTone_Multi(ModulePtr theModule)
{
	static const char	*funcName = "SetParsPointer_PureTone_Multi";

	if (!theModule) {
		NotifyError("%s: The module is not set.", funcName);
		return(FALSE);
	}
	mPureTonePtr = (MPureTonePtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 */

BOOLN
InitModule_PureTone_Multi(ModulePtr theModule)
{
	static const char	*funcName = "InitModule_PureTone_Multi";

	if (!SetParsPointer_PureTone_Multi(theModule)) {
		NotifyError("%s: Cannot set parameters pointer.", funcName);
		return(FALSE);
	}
	if (!Init_PureTone_Multi(GLOBAL)) {
		NotifyError("%s: Could not initialise process structure.", funcName);
		return(FALSE);
	}
	theModule->parsPtr = mPureTonePtr;
	theModule->CheckPars = CheckPars_PureTone_Multi;
	theModule->Free = Free_PureTone_Multi;
	theModule->GetUniParListPtr = GetUniParListPtr_PureTone_Multi;
	theModule->PrintPars = PrintPars_PureTone_Multi;
	theModule->ReadPars = ReadPars_PureTone_Multi;
	theModule->RunProcess = GenerateSignal_PureTone_Multi;
	theModule->SetParsPointer = SetParsPointer_PureTone_Multi;
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
 * With repeated calls the Signal memory is only allocated once, then re-used.
 */

BOOLN
GenerateSignal_PureTone_Multi(EarObjectPtr data)
{
	static const char *funcName = "GenerateSignal_PureTone_Multi";
	int			j;
	ChanLen		i,t;
	register	double		amplitude, phase;
	register	ChanData	*dataPtr;

	if (data == NULL) {
		NotifyError("%s: EarObject not initialised.", funcName);
		return(FALSE);
	}	
	if (!CheckPars_PureTone_Multi())
		return(FALSE);
	SetProcessName_EarObject(data, "Multiple pure tone stimulus");
	data->updateProcessFlag = TRUE;	/* Ensure signal is set to zero. */
	if ( !InitOutSignal_EarObject(data, PURE_TONE_NUM_CHANNELS,
	  (ChanLen) (mPureTonePtr->duration / mPureTonePtr->dt + 1.5),
	  mPureTonePtr->dt) ) {
		NotifyError("%s: Cannot initialise output signal.", funcName);
		return(FALSE);
	}
	for (j = 0; j < mPureTonePtr->numPTones; j++) {
		amplitude = RMS_AMP(mPureTonePtr->intensities[j]) * SQRT_2;
		phase = DEGREES_TO_RADS(mPureTonePtr->phases[j]);
		dataPtr = data->outSignal->channel[0];
		for (i = 0, t = data->timeIndex; i < data->outSignal->length; i++, t++)
			*dataPtr++ += (ChanData) (amplitude * sin(PIx2 *
			   mPureTonePtr->frequencies[j] * (t * data->outSignal->dt) +
			   phase));
	}
	SetProcessContinuity_EarObject(data);
	return(TRUE);

} /* GenerateSignal_PureTone_Multi */

