/**********************
 *
 * File:		UtAmpMod.c
 * Purpose:		Utility to amplitude modulate signal.
 * Comments:	Written using ModuleProducer version 1.8.
 * Author:		L. P. O'Mard
 * Created:		21 Dec 1995
 * Updated:	
 * Copyright:	(c) 1998, University of Essex
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
#include "UtAmpMod.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

AmpModPtr	ampModPtr = NULL;

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
Free_Utility_AmpMod(void)
{
	/* static const char	*funcName = "Free_Utility_AmpMod";  */

	if (ampModPtr == NULL)
		return(FALSE);
	if (ampModPtr->modulationDepths) {
		free(ampModPtr->modulationDepths);
		ampModPtr->modulationDepths = NULL;
	}
	if (ampModPtr->frequencies) {
		free(ampModPtr->frequencies);
		ampModPtr->frequencies = NULL;
	}
	if (ampModPtr->phases) {
		free(ampModPtr->phases);
		ampModPtr->phases = NULL;
	}
	if (ampModPtr->parList)
		FreeList_UniParMgr(&ampModPtr->parList);
	if (ampModPtr->parSpec == GLOBAL) {
		free(ampModPtr);
		ampModPtr = NULL;
	}
	return(TRUE);

}

/********************************* SetDefaultNumFrequenciesArrays *************/

/*
 * This routine sets the default arrays and array values.
 */

BOOLN
SetDefaultNumFrequenciesArrays_Utility_AmpMod(void)
{
	static const char *funcName =
	  "SetDefaultNumFrequenciesArrays_Utility_AmpMod";
	int		i;
	double	modulationDepths[] = {50.0};
	double	frequencies[] = {100.0};
	double	phases[] = {0.0};

	if (!AllocNumFrequencies_Utility_AmpMod(1)) {
		NotifyError("%s: Could not allocate default arrays.", funcName);
		return(FALSE);
	}
	for (i = 0; i < ampModPtr->numFrequencies; i++) {
		ampModPtr->modulationDepths[i] = modulationDepths[i];
		ampModPtr->frequencies[i] = frequencies[i];
		ampModPtr->phases[i] = phases[i];
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
Init_Utility_AmpMod(ParameterSpecifier parSpec)
{
	static const char	*funcName = "Init_Utility_AmpMod";

	if (parSpec == GLOBAL) {
		if (ampModPtr != NULL)
			Free_Utility_AmpMod();
		if ((ampModPtr = (AmpModPtr) malloc(sizeof(AmpMod))) == NULL) {
			NotifyError("%s: Out of memory for 'global' pointer", funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (ampModPtr == NULL) {
			NotifyError("%s:  'local' pointer not set.", funcName);
			return(FALSE);
		}
	}
	ampModPtr->parSpec = parSpec;
	ampModPtr->numFrequenciesFlag = FALSE;
	ampModPtr->numFrequencies = 0;
	ampModPtr->modulationDepths = NULL;
	ampModPtr->frequencies = NULL;
	ampModPtr->phases = NULL;

	if (!SetDefaultNumFrequenciesArrays_Utility_AmpMod()) {
		NotifyError("%s: Could not set the default 'numFrequencies' arrays.",
		  funcName);
		return(FALSE);
	}

	if (!SetUniParList_Utility_AmpMod()) {
		NotifyError("%s: Could not initialise parameter list.", funcName);
		Free_Utility_AmpMod();
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
SetUniParList_Utility_AmpMod(void)
{
	static const char *funcName = "SetUniParList_Utility_AmpMod";
	UniParPtr	pars;

	if ((ampModPtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  UTILITY_AMPMOD_NUM_PARS, NULL)) == NULL) {
		NotifyError("%s: Could not initialise parList.", funcName);
		return(FALSE);
	}
	pars = ampModPtr->parList->pars;
	SetPar_UniParMgr(&pars[UTILITY_AMPMOD_NUMFREQUENCIES], "NUM_FREQS",
	  "Number of modulation frequencies.",
	  UNIPAR_INT_AL,
	  &ampModPtr->numFrequencies, NULL,
	  (void * (*)) SetNumFrequencies_Utility_AmpMod);
	SetPar_UniParMgr(&pars[UTILITY_AMPMOD_MODULATIONDEPTHS], "DEPTH",
	  "Modulation depths (%).",
	  UNIPAR_REAL_ARRAY,
	  &ampModPtr->modulationDepths, &ampModPtr->numFrequencies,
	  (void * (*)) SetIndividualDepth_Utility_AmpMod);
	SetPar_UniParMgr(&pars[UTILITY_AMPMOD_FREQUENCIES], "FREQUENCY",
	  "Modulation frequencies (Hz).",
	  UNIPAR_REAL_ARRAY,
	  &ampModPtr->frequencies, &ampModPtr->numFrequencies,
	  (void * (*)) SetIndividualFreq_Utility_AmpMod);
	SetPar_UniParMgr(&pars[UTILITY_AMPMOD_PHASES], "PHASE",
	  "Modulation phases (degrees).",
	  UNIPAR_REAL_ARRAY,
	  &ampModPtr->phases, &ampModPtr->numFrequencies,
	  (void * (*)) SetIndividualPhase_Utility_AmpMod);
	return(TRUE);

}

/****************************** GetUniParListPtr ******************************/

/*
 * This function returns a pointer to the module's universal parameter
 * list.
 */

UniParListPtr
GetUniParListPtr_Utility_AmpMod(void)
{
	static const char	*funcName = "GetUniParListPtr_Utility_AmpMod";

	if (ampModPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (ampModPtr->parList == NULL) {
		NotifyError("%s: UniParList data structure has not been initialised. "
		  "NULL returned.", funcName);
		return(NULL);
	}
	return(ampModPtr->parList);

}

/****************************** AllocNumFrequencies ***************************/

/*
 * This function allocates the memory for the pure tone arrays.
 * It will assume that nothing needs to be done if the 'numFrequencies' 
 * variable is the same as the current structure member value.
 * To make this work, the function needs to set the structure 'numFrequencies'
 * parameter too.
 * It returns FALSE if it fails in any way.
 */

BOOLN
AllocNumFrequencies_Utility_AmpMod(int numFrequencies)
{
	static const char	*funcName = "AllocNumFrequencies_Utility_AmpMod";

	if (numFrequencies == ampModPtr->numFrequencies)
		return(TRUE);
	if (ampModPtr->modulationDepths)
		free(ampModPtr->modulationDepths);
	if ((ampModPtr->modulationDepths = (double *) calloc(numFrequencies,
	  sizeof(double))) == NULL) {
		NotifyError("%s: Cannot allocate memory for '%d' modulationDepths.",
		  funcName, numFrequencies);
		return(FALSE);
	}
	if (ampModPtr->frequencies)
		free(ampModPtr->frequencies);
	if ((ampModPtr->frequencies = (double *) calloc(numFrequencies, sizeof(
	  double))) == NULL) {
		NotifyError("%s: Cannot allocate memory for '%d' frequencies.",
		  funcName, numFrequencies);
		return(FALSE);
	}
	if (ampModPtr->phases)
		free(ampModPtr->phases);
	if ((ampModPtr->phases = (double *) calloc(numFrequencies, sizeof(
	  double))) == NULL) {
		NotifyError("%s: Cannot allocate memory for '%d' phases.", funcName,
		  numFrequencies);
		return(FALSE);
	}
	ampModPtr->numFrequencies = numFrequencies;
	ampModPtr->numFrequenciesFlag = TRUE;
	return(TRUE);

}

/****************************** SetPars ***************************************/

/*
 * This function sets all the module's parameters.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetPars_Utility_AmpMod(int numFrequencies, double *modulationDepths,
  double *frequencies, double *phases)
{
	static const char	*funcName = "SetPars_Utility_AmpMod";
	BOOLN	ok;

	ok = TRUE;
	if (!SetNumFrequencies_Utility_AmpMod(numFrequencies))
		ok = FALSE;
	if (!SetModulationDepths_Utility_AmpMod(modulationDepths))
		ok = FALSE;
	if (!SetFrequencies_Utility_AmpMod(frequencies))
		ok = FALSE;
	if (!SetPhases_Utility_AmpMod(phases))
		ok = FALSE;
	if (!ok)
		NotifyError("%s: Failed to set all module parameters." ,funcName);
	return(ok);

}

/****************************** SetNumFrequencies *****************************/

/*
 * This function sets the module's numFrequencies parameter.
 * It returns TRUE if the operation is successful.
 * The 'numFrequencies' variable is set by the
 * 'AllocNumFrequencies_Utility_AmpMod' routine.
 * Additional checks should be added as required.
 */

BOOLN
SetNumFrequencies_Utility_AmpMod(int theNumFrequencies)
{
	static const char	*funcName = "SetNumFrequencies_Utility_AmpMod";

	if (ampModPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (theNumFrequencies < 1) {
		NotifyError("%s: Value must be greater then zero (%d).", funcName,
		  theNumFrequencies);
		return(FALSE);
	}
	if (!AllocNumFrequencies_Utility_AmpMod(theNumFrequencies)) {
		NotifyError(
		  "%%s: Cannot allocate memory for the 'numFrequencies' arrays.",
		  funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	ampModPtr->numFrequenciesFlag = TRUE;
	return(TRUE);

}

/****************************** SetModulationDepths ***************************/

/*
 * This function sets the module's modulationDepths array.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetModulationDepths_Utility_AmpMod(double *theModulationDepths)
{
	static const char	*funcName = "SetModulationDepths_Utility_AmpMod";

	if (ampModPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	ampModPtr->modulationDepths = theModulationDepths;
	return(TRUE);

}

/****************************** SetFrequencies ********************************/

/*
 * This function sets the module's frequencies array.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetFrequencies_Utility_AmpMod(double *theFrequencies)
{
	static const char	*funcName = "SetFrequencies_Utility_AmpMod";

	if (ampModPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	ampModPtr->frequencies = theFrequencies;
	return(TRUE);

}

/****************************** SetPhases *************************************/

/*
 * This function sets the module's phases array.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetPhases_Utility_AmpMod(double *thePhases)
{
	static const char	*funcName = "SetPhases_Utility_AmpMod";

	if (ampModPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	ampModPtr->phases = thePhases;
	return(TRUE);

}

/********************************* SetIndividualFreq **************************/

/*
 * This function sets the individual frequency values.
 * It first checks if the frequencies have been set.
 */

BOOLN
SetIndividualFreq_Utility_AmpMod(int theIndex, double theFrequency)
{
	static const char *funcName = "SetIndividualFreq_Utility_AmpMod";

	if (ampModPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (ampModPtr->frequencies == NULL) {
		NotifyError("%s: Frequencies not set.", funcName);
		return(FALSE);
	}
	if (theIndex > ampModPtr->numFrequencies - 1) {
		NotifyError("%s: Index value must be in the\nrange 0 - %d (%d).\n",
		  funcName, ampModPtr->numFrequencies - 1, theIndex);
		return(FALSE);
	}
	ampModPtr->frequencies[theIndex] = theFrequency;
	return(TRUE);

}

/********************************* SetIndividualPhase *************************/

/*
 * This function sets the individual phase values.
 * It first checks if the phases have been set.
 */

BOOLN
SetIndividualPhase_Utility_AmpMod(int theIndex, double thePhase)
{
	static const char *funcName = "SetIndividualPhase_Utility_AmpMod";

	if (ampModPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (ampModPtr->phases == NULL) {
		NotifyError("%s: Phases not set.", funcName);
		return(FALSE);
	}
	if (theIndex > ampModPtr->numFrequencies - 1) {
		NotifyError("%s: Index value must be in the\nrange 0 - %d (%d).\n",
		  funcName, ampModPtr->numFrequencies - 1, theIndex);
		return(FALSE);
	}
	ampModPtr->phases[theIndex] = thePhase;
	return(TRUE);

}

/********************************* SetIndividualIndex *************************/

/*
 * This function sets the individual depth values.
 * It first checks if the phases have been set.
 */

BOOLN
SetIndividualDepth_Utility_AmpMod(int theIndex, double theModDepth)
{
	static const char *funcName = "SetIndividualIndex_Utility_AmpMod";

	if (ampModPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (ampModPtr->modulationDepths == NULL) {
		NotifyError("%s: Modulation depths not set.", funcName);
		return(FALSE);
	}
	if (theIndex > ampModPtr->numFrequencies - 1) {
		NotifyError("%s: Index value must be in the\nrange 0 - %d (%d).\n",
		  funcName, ampModPtr->numFrequencies - 1, theIndex);
		return(FALSE);
	}
	ampModPtr->modulationDepths[theIndex] = theModDepth;
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
CheckPars_Utility_AmpMod(void)
{
	static const char	*funcName = "CheckPars_Utility_AmpMod";
	BOOLN	ok;
	int		i;

	ok = TRUE;
	if (ampModPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (ampModPtr->modulationDepths == NULL) {
		NotifyError("%s: modulationDepths array not set.", funcName);
		ok = FALSE;
	}
	if (ampModPtr->frequencies == NULL) {
		NotifyError("%s: frequencies array not set.", funcName);
		ok = FALSE;
	}
	if (ampModPtr->phases == NULL) {
		NotifyError("%s: phases array not set.", funcName);
		ok = FALSE;
	}
	if (!ampModPtr->numFrequenciesFlag) {
		NotifyError("%s: numFrequencies variable not set.", funcName);
		ok = FALSE;
	}
	for (i = 0; i < ampModPtr->numFrequencies; i++)
		if (ampModPtr->modulationDepths[i] / 100.0 > (1.0 /
		  ampModPtr->numFrequencies)) {
			NotifyError("%s: Illegal modulation index, [%d] = %g)", funcName,
			  i, ampModPtr->modulationDepths[i]);
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
PrintPars_Utility_AmpMod(void)
{
	static const char	*funcName = "PrintPars_Utility_AmpMod";
	int		i;

	if (!CheckPars_Utility_AmpMod()) {
		NotifyError("%s: Parameters have not been correctly set.", funcName);
		return(FALSE);
	}
	DPrint("Amplitude Modulation Module Parameters:-\n");
	DPrint("\t%10s\t%10s\t%10s\n", "Mod. Depth",
	  "Frequencies", "Phases");
	DPrint("\t%10s\t%10s\t%10s\n", "(%)", "(Hz)",
	  "(degrees)");
	for (i = 0; i < ampModPtr->numFrequencies; i++)
		DPrint("\t%10g\t%10g\t%10g\n",
		  ampModPtr->modulationDepths[i], ampModPtr->frequencies[i],
		  ampModPtr->phases[i]);
	DPrint("\tNo. of frequencies = %d\n",
	  ampModPtr->numFrequencies);
	return(TRUE);

}

/****************************** ReadPars **************************************/

/*
 * This program reads a specified number of parameters from a file.
 * It returns FALSE if it fails in any way.n */

BOOLN
ReadPars_Utility_AmpMod(char *fileName)
{
	static const char	*funcName = "ReadPars_Utility_AmpMod";
	BOOLN	ok;
	char	*filePath;
	int		i, numFrequencies;
	FILE	*fp;

	filePath = GetParsFileFPath_Common(fileName);
	if ((fp = fopen(filePath, "r")) == NULL) {
		NotifyError("%s: Cannot open data file '%s'.\n", funcName, fileName);
		return(FALSE);
	}
	DPrint("%s: Reading from '%s':\n", funcName, fileName);
	Init_ParFile();
	ok = TRUE;
	if (!GetPars_ParFile(fp, "%d", &numFrequencies))
		ok = FALSE;
	if (!AllocNumFrequencies_Utility_AmpMod(numFrequencies)) {
		NotifyError(
		  "%%s: Cannot allocate memory for the 'numFrequencies' arrays.",
		  funcName);
		return(FALSE);
	}
	for (i = 0; i < numFrequencies; i++)
		if (!GetPars_ParFile(fp, "%lf %lf %lf", &ampModPtr->modulationDepths[
		  i], &ampModPtr->frequencies[i], &ampModPtr->phases[i]))
			ok = FALSE;
	fclose(fp);
	Free_ParFile();
	if (!ok) {
		NotifyError("%s: Not enough lines, or invalid parameters, in module "
		  "parameter file '%s'.", funcName, fileName);
		return(FALSE);
	}
	if (!SetPars_Utility_AmpMod(numFrequencies, ampModPtr->modulationDepths,
	  ampModPtr->frequencies, ampModPtr->phases)) {
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
SetParsPointer_Utility_AmpMod(ModulePtr theModule)
{
	static const char	*funcName = "SetParsPointer_Utility_AmpMod";

	if (!theModule) {
		NotifyError("%s: The module is not set.", funcName);
		return(FALSE);
	}
	ampModPtr = (AmpModPtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 */

BOOLN
InitModule_Utility_AmpMod(ModulePtr theModule)
{
	static const char	*funcName = "InitModule_Utility_AmpMod";

	if (!SetParsPointer_Utility_AmpMod(theModule)) {
		NotifyError("%s: Cannot set parameters pointer.", funcName);
		return(FALSE);
	}
	if (!Init_Utility_AmpMod(GLOBAL)) {
		NotifyError("%s: Could not initialise process structure.", funcName);
		return(FALSE);
	}
	theModule->parsPtr = ampModPtr;
	theModule->CheckPars = CheckPars_Utility_AmpMod;
	theModule->Free = Free_Utility_AmpMod;
	theModule->GetUniParListPtr = GetUniParListPtr_Utility_AmpMod;
	theModule->PrintPars = PrintPars_Utility_AmpMod;
	theModule->ReadPars = ReadPars_Utility_AmpMod;
	theModule->RunProcess = Process_Utility_AmpMod;
	theModule->SetParsPointer = SetParsPointer_Utility_AmpMod;
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
CheckData_Utility_AmpMod(EarObjectPtr data)
{
	static const char	*funcName = "CheckData_Utility_AmpMod";

	if (data == NULL) {
		NotifyError("%s: EarObject not initialised.", funcName);
		return(FALSE);
	}
	if (!CheckInSignal_EarObject(data, funcName))
		return(FALSE);
	/*** Put additional checks here. ***/
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
Process_Utility_AmpMod(EarObjectPtr data)
{
	static const char	*funcName = "Process_Utility_AmpMod";
	register	ChanData	 *inPtr, *outPtr;
	int		chan, j;
	double	sum, time;
	ChanLen	i, t;

	if (!CheckPars_Utility_AmpMod())
		return(FALSE);
	if (!CheckData_Utility_AmpMod(data)) {
		NotifyError("%s: Process data invalid.", funcName);
		return(FALSE);
	}
	SetProcessName_EarObject(data, "Amplitude modulation utility module.");
	if (!InitOutSignal_EarObject(data, data->inSignal[0]->numChannels, 
	  data->inSignal[0]->length, data->inSignal[0]->dt)) {
		NotifyError("%s: Cannot initialise output channels.", funcName);
		return(FALSE);
	}
	for (chan = 0; chan < data->inSignal[0]->numChannels; chan++) {
		inPtr = data->inSignal[0]->channel[chan];
		outPtr = data->outSignal->channel[chan];
		for (i = 0, t = data->timeIndex; i < data->outSignal->length; i++,
		  t++) {
			time = t * data->outSignal->dt;
			for (j = 0, sum = 0.0; j < ampModPtr->numFrequencies; j++)
				sum += ampModPtr->modulationDepths[j] / 100.0 * sin(PIx2 *
				  ampModPtr->frequencies[j] * time +
				  DEGREES_TO_RADS(ampModPtr->phases[j]));
			*outPtr++ = (1.0 + sum) * *inPtr++;
		}
	}
	SetProcessContinuity_EarObject(data);
	return(TRUE);

}

