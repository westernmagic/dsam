/**********************
 *
 * File:		StWhiteNoise.c
 * Purpose:		This module contains the methods for the simple white-noise
 *				signal generation paradigm.
 * Comments:	
 * Author:		E. A. Lopez-Poveda
 * Created:		17 Jan 1995
 * Updated:		12 Mar 1997
 * Copyright:	(c) 1997,  University of Essex
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
#include "UtRandom.h"
#include "StWhiteNoise.h"

/********************************* Global variables ***************************/

WhiteNoisePtr	whiteNoisePtr = NULL;

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
Init_WhiteNoise(ParameterSpecifier parSpec)
{
	static const char *funcName = "Init_WhiteNoise";

	if (parSpec == GLOBAL) {
		if (whiteNoisePtr != NULL)
			Free_WhiteNoise();
		if ((whiteNoisePtr = (WhiteNoisePtr) malloc(sizeof(WhiteNoise))) ==
		  NULL) {
			NotifyError("%s: Out of memory for 'global' pointer", funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (whiteNoisePtr == NULL) { 
			NotifyError("%s:  'local' pointer not set.", funcName);
			return(FALSE);
		}
	}
	whiteNoisePtr->parSpec = parSpec;
	whiteNoisePtr->numChannelsFlag = TRUE;
	whiteNoisePtr->durationFlag = TRUE;
	whiteNoisePtr->dtFlag = TRUE;
	whiteNoisePtr->intensityFlag = TRUE;
	whiteNoisePtr->correlationDegreeFlag = TRUE;
	whiteNoisePtr->randomizationIndexFlag = TRUE;
	whiteNoisePtr->ranSeedFlag = TRUE;
	whiteNoisePtr->numChannels = 1;
	whiteNoisePtr->intensity = 30.0;
	whiteNoisePtr->duration = 0.1;
	whiteNoisePtr->dt = DEFAULT_DT;
	whiteNoisePtr->correlationDegree = 1;
	whiteNoisePtr->randomizationIndex = 36;
	whiteNoisePtr->ranSeed = 0;

	if (!SetUniParList_WhiteNoise()) {
		NotifyError("%s: Could not initialise parameter list.", funcName);
		Free_WhiteNoise();
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
Free_WhiteNoise(void)
{
	if (whiteNoisePtr == NULL)
		return(TRUE);
	if (whiteNoisePtr->parList)
		FreeList_UniParMgr(&whiteNoisePtr->parList);
	if (whiteNoisePtr->parSpec == GLOBAL) {
		free(whiteNoisePtr);
		whiteNoisePtr = NULL;
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
SetUniParList_WhiteNoise(void)
{
	static const char *funcName = "SetUniParList_WhiteNoise";
	UniParPtr	pars;

	if ((whiteNoisePtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  WHITENOISE_NUM_PARS, NULL)) == NULL) {
		NotifyError("%s: Could not initialise parList.", funcName);
		return(FALSE);
	}
	pars = whiteNoisePtr->parList->pars;
	SetPar_UniParMgr(&pars[WHITENOISE_NUMCHANNELS], "NUM_CHANNELS",
	  "Number of Sound Channels (Binaural = 2; Monaural = 1).",
	  UNIPAR_INT,
	  &whiteNoisePtr->numChannels, NULL,
	  (void * (*)) SetNumChannels_WhiteNoise);
	SetPar_UniParMgr(&pars[WHITENOISE_CORRELATIONDEGREE], "DEGREE_CORR",
	  "Degree of Binaural noise correlation (Correlated = 1; "
	  "Uncorrelated = -1).",
	  UNIPAR_INT,
	  &whiteNoisePtr->correlationDegree, NULL,
	  (void * (*)) SetCorrelationDegree_WhiteNoise);
	SetPar_UniParMgr(&pars[WHITENOISE_RANDOMIZATIONINDEX], "RAN_INDEX",
	  "Noise Randomization Index (greater than 12).",
	  UNIPAR_INT,
	  &whiteNoisePtr->randomizationIndex, NULL,
	  (void * (*)) SetRandomizationIndex_WhiteNoise);
	SetPar_UniParMgr(&pars[WHITENOISE_RANSEED], "RAN_SEED",
	  "Random Number Seed.",
	  UNIPAR_LONG,
	  &whiteNoisePtr->ranSeed, NULL,
	  (void * (*)) SetRanSeed_WhiteNoise);
	SetPar_UniParMgr(&pars[WHITENOISE_INTENSITY], "INTENSITY",
	  "Intensity (dB SPL).",
	  UNIPAR_REAL,
	  &whiteNoisePtr->intensity, NULL,
	  (void * (*)) SetIntensity_WhiteNoise);
	SetPar_UniParMgr(&pars[WHITENOISE_DURATION], "DURATION",
	  "Duration (s).",
	  UNIPAR_REAL,
	  &whiteNoisePtr->duration, NULL,
	  (void * (*)) SetDuration_WhiteNoise);
	SetPar_UniParMgr(&pars[WHITENOISE_SAMPLINGINTERVAL], "DT",
	  "Sampling Interval, dt (s).",
	  UNIPAR_REAL,
	  &whiteNoisePtr->dt, NULL,
	  (void * (*)) SetSamplingInterval_WhiteNoise);
	return(TRUE);

}

/****************************** GetUniParListPtr ******************************/

/*
 * This function returns a pointer to the module's universal parameter
 * list.
 */

UniParListPtr
GetUniParListPtr_WhiteNoise(void)
{
	static const char	*funcName = "GetUniParListPtr_WhiteNoise";

	if (whiteNoisePtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (whiteNoisePtr->parList == NULL) {
		NotifyError("%s: UniParList data structure has not been initialised. "
		  "NULL returned.", funcName);
		return(NULL);
	}
	return(whiteNoisePtr->parList);

}

/********************************* CheckPars **********************************/

/*
 * This routine checks that the necessary parameters for the module have been
 * correctly initialised.
 * It returns TRUE if there are no problems.
 */
 
BOOLN
CheckPars_WhiteNoise(void)
{
	static const char *funcName = "CheckPars_WhiteNoise";
	BOOLN	ok;
	
	ok = TRUE;
	if (whiteNoisePtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (!whiteNoisePtr->numChannelsFlag) {
		NotifyError("%s: binaural variable not set.", funcName);
		ok = FALSE;
	}
	if (!whiteNoisePtr->intensityFlag) {
		NotifyError("%s: intensity variable not set.", funcName);
		ok = FALSE;
	}
	if (!whiteNoisePtr->durationFlag) {
		NotifyError("%s: duration variable not set.", funcName);
		ok = FALSE;
	}
	if (!whiteNoisePtr->dtFlag) {
		NotifyError("%s: dt variable not set.", funcName);
		ok = FALSE;
	}
	if (!whiteNoisePtr->correlationDegreeFlag) {
		NotifyError("%s: Correlation degree variable not set.", funcName);
		ok = FALSE;
	}
	if (!whiteNoisePtr->randomizationIndexFlag) {
		NotifyError("%s: Randomization index variable not set.", funcName);
		ok = FALSE;
	}
	if (!whiteNoisePtr->ranSeedFlag) {
		NotifyError("%s: Random seed variable not set.", funcName);
		ok = FALSE;
	}
	return(ok);
	
}	

/***************************** SetNumChannels *********************************/

/*
 * This function sets the module's numChannels parameter. It first checks
 * that the module has been initialised.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetNumChannels_WhiteNoise(int theNumChannels)
{
	static const char *funcName = "SetNumChannels_WhiteNoise";

	if (whiteNoisePtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if ( (theNumChannels != 1) && (theNumChannels != 2) ) {
		NotifyError("%s: Illegal numChannels value = %d!", funcName,
		  theNumChannels);
		return(FALSE);
	}
	whiteNoisePtr->numChannelsFlag = TRUE;
	whiteNoisePtr->numChannels = theNumChannels;
	return(TRUE);

}

/********************************* SetIntensity *******************************/

/*
 * This function sets the module's intensity parameter.  It first checks that
 * the module has been initialised.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetIntensity_WhiteNoise(double theIntensity)
{
	static const char *funcName = "SetIntensity_WhiteNoise";

	if (whiteNoisePtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	whiteNoisePtr->intensityFlag = TRUE;
	whiteNoisePtr->intensity = theIntensity;
	return(TRUE);

}

/********************************* SetDuration ********************************/

/*
 * This function sets the module's duration parameter.  It first checks that
 * the module has been initialised.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetDuration_WhiteNoise(double theDuration)
{
	static const char *funcName = "SetDuration_WhiteNoise";

	if (whiteNoisePtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	whiteNoisePtr->durationFlag = TRUE;
	whiteNoisePtr->duration = theDuration;
	return(TRUE);

}

/********************************* SetSamplingInterval ************************/

/*
 * This function sets the module's samplingInterval parameter.  It first checks
 * that the module has been initialised.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetSamplingInterval_WhiteNoise(double theSamplingInterval)
{
	static const char *funcName = "SetSamplingInterval_WhiteNoise";

	if (whiteNoisePtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if ( theSamplingInterval <= 0.0 ) {
		NotifyError("%s: Illegal sampling interval value = %g!", funcName,
		  theSamplingInterval);
		return(FALSE);
	}
	whiteNoisePtr->dtFlag = TRUE;
	whiteNoisePtr->dt = theSamplingInterval;
	return(TRUE);

}

/****************************** SetCorrelationDegree **************************/

/*
 * This function sets the module's correlationDegree parameter. It first checks
 * that the module has been initialised.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetCorrelationDegree_WhiteNoise(int theCorrelationDegree)
{
	static const char *funcName = "SetCorrelationDegree_WhiteNoise";

	if (whiteNoisePtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if ( (theCorrelationDegree != 1) && (theCorrelationDegree != -1) ) {
		NotifyError("%s: Illegal correlationDegree value = %d!", funcName,
		  theCorrelationDegree);
		return(FALSE);
	}
	whiteNoisePtr->correlationDegreeFlag = TRUE;
	whiteNoisePtr->correlationDegree = theCorrelationDegree;
	return(TRUE);

}

/****************************** SetRandomizationIndex *************************/

/*
 * This function sets the module's random amplitude-generator maximum  
 * summation index parameter.It first checks that the module has been 
 * initialised. 
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetRandomizationIndex_WhiteNoise(int theRandomizationIndex)
{
	static const char *funcName = "SetRandomizationIndex_WhiteNoise";

	if (whiteNoisePtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (theRandomizationIndex < 12) {
		NotifyError("%s: The value of randomizationIndex (= %d) should not "\
		  "be greater than 12!", funcName, theRandomizationIndex);
		return(FALSE);
	}
	whiteNoisePtr->randomizationIndexFlag = TRUE;
	whiteNoisePtr->randomizationIndex = theRandomizationIndex;
	return(TRUE);

}

/****************************** SetRanSeed ************************************/

/*
 * This function sets the module's seed for randomization.
 * It first checks that the module has been initialised. 
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetRanSeed_WhiteNoise(long theRanSeed)
{
	static const char *funcName = "SetRanSeed_WhiteNoise";

	if (whiteNoisePtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	whiteNoisePtr->ranSeedFlag = TRUE;
	whiteNoisePtr->ranSeed = theRanSeed;
	return(TRUE);

}

/********************************* SetPars ************************************/

/*
 * This function sets all the module's parameters.
 * It returns TRUE if the operation is successful.
 */
 
BOOLN
SetPars_WhiteNoise(int theNumChannels, double theIntensity,
  double theDuration, double theSamplingInterval, int theCorrelationDegree,
  int theRandomizationIndex, long theRanSeed)
{
	static const char *funcName = "SetPars_WhiteNoise";
	BOOLN	ok;
	
	ok = TRUE;
	if (!SetNumChannels_WhiteNoise(theNumChannels))
		ok = FALSE;
	if (!SetIntensity_WhiteNoise(theIntensity))
		ok = FALSE;
	if (!SetDuration_WhiteNoise(theDuration))
		ok = FALSE;
	if (!SetSamplingInterval_WhiteNoise(theSamplingInterval))
		ok = FALSE;
	if (!SetCorrelationDegree_WhiteNoise(theCorrelationDegree))
		ok = FALSE;
	if (!SetRandomizationIndex_WhiteNoise(theRandomizationIndex))
		ok = FALSE;
	if (!SetRanSeed_WhiteNoise(theRanSeed))
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
PrintPars_WhiteNoise(void)
{
	static const char *funcName = "PrintPars_WhiteNoise";

	if (!CheckPars_WhiteNoise()) {
		NotifyError("%s: Parameters have not been correctly set.", funcName);
		return(FALSE);
	}
	DPrint("White Noise Module Parameters:-\n");
	DPrint("\tNumber of Sound Channels = %d,\tCorrelation "\
	  "Degree = %d,\n", whiteNoisePtr->numChannels,
	  whiteNoisePtr->correlationDegree);
	DPrint("\tRandomization Index = %d,\tRandomization "\
	  "Seed = %ld.\n", whiteNoisePtr->randomizationIndex,
	  whiteNoisePtr->ranSeed);
	DPrint("\tIntensity = %g dB SPL,\n",
	  whiteNoisePtr->intensity);
	DPrint("\tDuration = %g ms,\tSampling interval = %g ms\n",
	  MSEC(whiteNoisePtr->duration), MSEC(whiteNoisePtr->dt));
	return(TRUE);

}

/****************************** ReadPars **************************************/

/*
 * This program reads a specified number of parameters from a file.
 * It returns FALSE if it fails in any way.
 */
 
BOOLN
ReadPars_WhiteNoise(char *fileName)
{
	static const char *funcName = "ReadPars_WhiteNoise";
	BOOLN	ok;
	char	*filePath;
	double  intensity;
	double  duration, samplingInterval;
    int		numChannels, correlationDegree, randomizationIndex;
    long	ranSeed;
    FILE    *fp;
    
	filePath = GetParsFileFPath_Common(fileName);
    if ((fp = fopen(filePath, "r")) == NULL) {
        NotifyError("%s: Cannot open data file '%s'.\n", funcName, filePath);
		return(FALSE);
    }
    DPrint("%s: Reading from '%s':\n", funcName, filePath);
    Init_ParFile();
	ok = TRUE;
	if (!GetPars_ParFile(fp, "%d", &numChannels))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%d", &correlationDegree))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%d", &randomizationIndex))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%ld", &ranSeed))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &intensity))
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
	if (!SetPars_WhiteNoise(numChannels, intensity, duration,
	  samplingInterval, correlationDegree, randomizationIndex, ranSeed)) {
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
SetParsPointer_WhiteNoise(ModulePtr theModule)
{
	static const char	*funcName = "SetParsPointer_WhiteNoise";

	if (!theModule) {
		NotifyError("%s: The module is not set.", funcName);
		return(FALSE);
	}
	whiteNoisePtr = (WhiteNoisePtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 */

BOOLN
InitModule_WhiteNoise(ModulePtr theModule)
{
	static const char	*funcName = "InitModule_WhiteNoise";

	if (!SetParsPointer_WhiteNoise(theModule)) {
		NotifyError("%s: Cannot set parameters pointer.", funcName);
		return(FALSE);
	}
	if (!Init_WhiteNoise(GLOBAL)) {
		NotifyError("%s: Could not initialise process structure.", funcName);
		return(FALSE);
	}
	theModule->parsPtr = whiteNoisePtr;
	theModule->CheckPars = CheckPars_WhiteNoise;
	theModule->Free = Free_WhiteNoise;
	theModule->GetUniParListPtr = GetUniParListPtr_WhiteNoise;
	theModule->PrintPars = PrintPars_WhiteNoise;
	theModule->ReadPars = ReadPars_WhiteNoise;
	theModule->RunProcess = GenerateSignal_WhiteNoise;
	theModule->SetParsPointer = SetParsPointer_WhiteNoise;
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
GenerateSignal_WhiteNoise(EarObjectPtr data)
{
	static const char *funcName = "GenerateSignal_WhiteNoise";
	
	int			j;
	ChanLen		i;
	register	double		amplitude, sum;
	register	ChanData	*dataPtr, *dataPtrA, *dataPtrB;

	if (data == NULL) {
		NotifyError("%s: EarObject not initialised.", funcName);
		return(FALSE);
	}	
	if (!CheckPars_WhiteNoise())
		return(FALSE);
	SetProcessName_EarObject(data, "White Noise stimulus");
	if ( !InitOutSignal_EarObject(data, (uShort) whiteNoisePtr->numChannels,
	  (ChanLen) floor(whiteNoisePtr->duration / whiteNoisePtr->dt + 0.5),
	  whiteNoisePtr->dt) ) {
		NotifyError("%s: Cannot initialise output signal", funcName);
		return(FALSE);
	}
	if (data->updateProcessFlag)
		SetGlobalSeed_Random(whiteNoisePtr->ranSeed);
	SetInterleaveLevel_SignalData(data->outSignal, (uShort) whiteNoisePtr->
	  numChannels);
	amplitude = RMS_AMP(whiteNoisePtr->intensity) * SQRT_2;
	dataPtr = data->outSignal->channel[0];
	for (i = 0; i < data->outSignal->length; i++) {
		for (j = 0, sum = 0.0; j < whiteNoisePtr->randomizationIndex; j++)
			sum += Ran01_Random(&randomNumSeed);
		sum = sum - whiteNoisePtr->randomizationIndex / 2; 
		*(dataPtr++) = amplitude * 
		  (sum / sqrt(whiteNoisePtr->randomizationIndex / 12));
	}
	if (whiteNoisePtr->numChannels == 2) {
		dataPtrA = data->outSignal->channel[0];
		dataPtrB = data->outSignal->channel[1];
		for (i = 0; i < data->outSignal->length; i++)
		  *(dataPtrB++) = whiteNoisePtr->correlationDegree * *(dataPtrA++);
	}
	SetProcessContinuity_EarObject(data);
	return(TRUE);

} /* GenerateSignal_WhiteNoise */

