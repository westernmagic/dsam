/**********************
 *
 * File:		FlMultiBPass.c
 * Purpose:		This is an implementation of an outer-/middle-ear pre-emphasis
 *				filter using the filters.c module.
 * Comments:	Written using ModuleProducer version 1.2.2 (Nov 11 1998).
 *				A parallel, multiple band-pass filter is used to model the
 *				effects of the outer- and middle-ear on an input signal.
 *				The first data EarObject in the BPass is the process EarObject.
 *				12-11-98 LPO: I have changed this module to use arrays rather
 *				than a data structure.  This change makes it easier to do the
 *				universal parameter implementation.
 * Author:		L. P. O'Mard
 * Created:		20 Jul 1998
 * Updated:		12 Nov 1998
 * Copyright:	(c) 1998, University of Essex.
 *
 *********************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG */

#include "GeCommon.h"
#include "GeSignalData.h"
#include "GeEarObject.h"
#include "GeUniParMgr.h"
#include "GeModuleMgr.h"
#include "FiParFile.h"
#include "UtFilters.h"
#include "FlMultiBPass.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

MultiBPassFPtr	multiBPassFPtr = NULL;

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
Free_Filter_MultiBPass(void)
{
	/* static const char	*funcName = "Free_Filter_MultiBPass";  */

	if (multiBPassFPtr == NULL)
		return(FALSE);
	FreeProcessVariables_Filter_MultiBPass();
	if (multiBPassFPtr->cascade) {
		free(multiBPassFPtr->cascade);
		multiBPassFPtr->cascade = NULL;
	}
	if (multiBPassFPtr->preAttenuation) {
		free(multiBPassFPtr->preAttenuation);
		multiBPassFPtr->preAttenuation = NULL;
	}
	if (multiBPassFPtr->upperCutOffFreq) {
		free(multiBPassFPtr->upperCutOffFreq);
		multiBPassFPtr->upperCutOffFreq = NULL;
	}
	if (multiBPassFPtr->lowerCutOffFreq) {
		free(multiBPassFPtr->lowerCutOffFreq);
		multiBPassFPtr->lowerCutOffFreq = NULL;
	}
	if (multiBPassFPtr->parList)
		FreeList_UniParMgr(&multiBPassFPtr->parList);
	if (multiBPassFPtr->parSpec == GLOBAL) {
		free(multiBPassFPtr);
		multiBPassFPtr = NULL;
	}
	return(TRUE);

}

/********************************* SetDefaultNumFiltersArrays *****************/

/*
 * This routine sets the default arrays and array values.
 */

BOOLN
SetDefaultNumFiltersArrays_Filter_MultiBPass(void)
{
	static const char *funcName =
	  "SetDefaultNumFiltersArrays_Filter_MultiBPass";
	int		i;
	int		cascade[] = {2, 2, 2};
	double	preAttenuation[] = {1.5, 6.0, -11.0};
	double	lowerCutOffFreq[] = {330.0, 1900.0, 7500.0};
	double	upperCutOffFreq[] = {5500.0, 5000.0, 14000.0};

	if (!AllocNumFilters_Filter_MultiBPass(3)) {
		NotifyError("%s: Could not allocate default arrays.", funcName);
		return(FALSE);
	}
	for (i = 0; i < multiBPassFPtr->numFilters; i++) {
		multiBPassFPtr->cascade[i] = cascade[i];
		multiBPassFPtr->preAttenuation[i] = preAttenuation[i];
		multiBPassFPtr->upperCutOffFreq[i] = upperCutOffFreq[i];
		multiBPassFPtr->lowerCutOffFreq[i] = lowerCutOffFreq[i];
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
Init_Filter_MultiBPass(ParameterSpecifier parSpec)
{
	static const char	*funcName = "Init_Filter_MultiBPass";

	if (parSpec == GLOBAL) {
		if (multiBPassFPtr != NULL)
			Free_Filter_MultiBPass();
		if ((multiBPassFPtr = (MultiBPassFPtr) malloc(sizeof(MultiBPassF))) ==
		  NULL) {
			NotifyError("%s: Out of memory for 'global' pointer", funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (multiBPassFPtr == NULL) {
			NotifyError("%s:  'local' pointer not set.", funcName);
			return(FALSE);
		}
	}
	multiBPassFPtr->parSpec = parSpec;
	multiBPassFPtr->updateProcessVariablesFlag = TRUE;
	multiBPassFPtr->numFiltersFlag = FALSE;
	multiBPassFPtr->numFilters = 0;
	multiBPassFPtr->cascade = NULL;
	multiBPassFPtr->preAttenuation = NULL;
	multiBPassFPtr->upperCutOffFreq = NULL;
	multiBPassFPtr->lowerCutOffFreq = NULL;

	if (!SetDefaultNumFiltersArrays_Filter_MultiBPass()) {
		NotifyError("%s: Could not set the default 'numFilters' arrays.",
		  funcName);
		return(FALSE);
	}

	if (!SetUniParList_Filter_MultiBPass()) {
		NotifyError("%s: Could not initialise parameter list.", funcName);
		Free_Filter_MultiBPass();
		return(FALSE);
	}
	multiBPassFPtr->numChannels = 0;
	multiBPassFPtr->bPassPars = NULL;
	return(TRUE);

}

/****************************** SetUniParList *********************************/

/*
 * This function initialises and sets the module's universal parameter
 * list. This list provides universal access to the module's
 * parameters.  It expects to be called from the 'Init_' routine.
 */

BOOLN
SetUniParList_Filter_MultiBPass(void)
{
	static const char *funcName = "SetUniParList_Filter_MultiBPass";
	UniParPtr	pars;

	if ((multiBPassFPtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  MULTIBPASS_NUM_PARS, NULL)) == NULL) {
		NotifyError("%s: Could not initialise parList.", funcName);
		return(FALSE);
	}
	pars = multiBPassFPtr->parList->pars;
	SetPar_UniParMgr(&pars[MULTIBPASS_NUMFILTERS], "NUM_FILTERS",
	  "No. of parallel band pass filters.",
	  UNIPAR_INT,
	  &multiBPassFPtr->numFilters, NULL,
	  (void * (*)) SetNumFilters_Filter_MultiBPass);
	SetPar_UniParMgr(&pars[MULTIBPASS_CASCADE], "CASCADE",
	  "Filter cascades.",
	  UNIPAR_INT_ARRAY,
	  &multiBPassFPtr->cascade, &multiBPassFPtr->numFilters,
	  (void * (*)) SetIndividualCascade_Filter_MultiBPass);
	SetPar_UniParMgr(&pars[MULTIBPASS_PREATTENUATION],
	  "ATTENUATION",
	  "Filter pre-attentuation (dB SPL).",
	  UNIPAR_REAL_ARRAY,
	  &multiBPassFPtr->preAttenuation, &multiBPassFPtr->numFilters,
	  (void * (*)) SetIndividualPreAttenuation_Filter_MultiBPass);
	SetPar_UniParMgr(&pars[MULTIBPASS_LOWERCUTOFFFREQ],
	  "LOWER_FREQ",
	  "Filter lower cut-off frequencies (Hz).",
	  UNIPAR_REAL_ARRAY,
	  &multiBPassFPtr->lowerCutOffFreq, &multiBPassFPtr->numFilters,
	  (void * (*)) SetIndividualLowerCutOffFreq_Filter_MultiBPass);
	SetPar_UniParMgr(&pars[MULTIBPASS_UPPERCUFOFFFREQ],
	  "UPPER_FREQ",
	  "Filter upper cut-off frequencies (Hz).",
	  UNIPAR_REAL_ARRAY,
	  &multiBPassFPtr->upperCutOffFreq, &multiBPassFPtr->numFilters,
	  (void * (*)) SetIndividualUpperCutOffFreq_Filter_MultiBPass);
	return(TRUE);

}

/****************************** GetUniParListPtr ******************************/

/*
 * This function returns a pointer to the module's universal parameter
 * list.
 */

UniParListPtr
GetUniParListPtr_Filter_MultiBPass(void)
{
	static const char	*funcName = "GetUniParListPtr_Filter_MultiBPass";

	if (multiBPassFPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (multiBPassFPtr->parList == NULL) {
		NotifyError("%s: UniParList data structure has not been initialised. "
		  "NULL returned.", funcName);
		return(NULL);
	}
	return(multiBPassFPtr->parList);

}

/****************************** AllocNumFilters *******************************/

/*
 * This function allocates the memory for the pure tone arrays.
 * It will assume that nothing needs to be done if the 'numFilters' 
 * variable is the same as the current structure member value.
 * To make this work, the function needs to set the structure 'numFilters'
 * parameter too.
 * It returns FALSE if it fails in any way.
 */

BOOLN
AllocNumFilters_Filter_MultiBPass(int numFilters)
{
	static const char	*funcName = "AllocNumFilters_Filter_MultiBPass";

	if (numFilters == multiBPassFPtr->numFilters)
		return(TRUE);
	if (multiBPassFPtr->cascade)
		free(multiBPassFPtr->cascade);
	if ((multiBPassFPtr->cascade = (int *) calloc(numFilters, sizeof(int))) ==
	  NULL) {
		NotifyError("%s: Cannot allocate memory for '%d' cascade.", funcName,
		  numFilters);
		return(FALSE);
	}
	if (multiBPassFPtr->preAttenuation)
		free(multiBPassFPtr->preAttenuation);
	if ((multiBPassFPtr->preAttenuation = (double *) calloc(numFilters,
	  sizeof(double))) == NULL) {
		NotifyError("%s: Cannot allocate memory for '%d' preAttenuation.",
		  funcName, numFilters);
		return(FALSE);
	}
	if (multiBPassFPtr->upperCutOffFreq)
		free(multiBPassFPtr->upperCutOffFreq);
	if ((multiBPassFPtr->upperCutOffFreq = (double *) calloc(numFilters,
	  sizeof(double))) == NULL) {
		NotifyError("%s: Cannot allocate memory for '%d' upperCutOffFreq.",
		  funcName, numFilters);
		return(FALSE);
	}
	if (multiBPassFPtr->lowerCutOffFreq)
		free(multiBPassFPtr->lowerCutOffFreq);
	if ((multiBPassFPtr->lowerCutOffFreq = (double *) calloc(numFilters,
	  sizeof(double))) == NULL) {
		NotifyError("%s: Cannot allocate memory for '%d' lowerCutOffFreq.",
		  funcName, numFilters);
		return(FALSE);
	}
	multiBPassFPtr->numFilters = numFilters;
	multiBPassFPtr->numFiltersFlag = TRUE;
	return(TRUE);

}

/****************************** SetPars ***************************************/

/*
 * This function sets all the module's parameters.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetPars_Filter_MultiBPass(int numFilters, int *cascade,
  double *preAttenuation, double *upperCutOffFreq, double *lowerCutOffFreq)
{
	static const char	*funcName = "SetPars_Filter_MultiBPass";
	BOOLN	ok;

	ok = TRUE;
	if (!SetNumFilters_Filter_MultiBPass(numFilters))
		ok = FALSE;
	if (!SetCascade_Filter_MultiBPass(cascade))
		ok = FALSE;
	if (!SetPreAttenuation_Filter_MultiBPass(preAttenuation))
		ok = FALSE;
	if (!SetUpperCutOffFreq_Filter_MultiBPass(upperCutOffFreq))
		ok = FALSE;
	if (!SetLowerCutOffFreq_Filter_MultiBPass(lowerCutOffFreq))
		ok = FALSE;
	if (!ok)
		NotifyError("%s: Failed to set all module parameters." ,funcName);
	return(ok);

}

/****************************** SetNumFilters *********************************/

/*
 * This function sets the module's numFilters parameter.
 * It returns TRUE if the operation is successful.
 * The 'numFilters' variable is set by the 'AllocNumFilters_Filter_MultiBPass'
 * routine.
 * Additional checks should be added as required.
 */

BOOLN
SetNumFilters_Filter_MultiBPass(int theNumFilters)
{
	static const char	*funcName = "SetNumFilters_Filter_MultiBPass";

	if (multiBPassFPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (theNumFilters < 1) {
		NotifyError("%s: Value must be greater then zero (%d).", funcName,
		  theNumFilters);
		return(FALSE);
	}
	if (!AllocNumFilters_Filter_MultiBPass(theNumFilters)) {
		NotifyError("%s: Cannot allocate memory for the 'numFilters' arrays.",
		  funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	multiBPassFPtr->numFiltersFlag = TRUE;
	multiBPassFPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/****************************** SetCascade ************************************/

/*
 * This function sets the module's cascade array.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetCascade_Filter_MultiBPass(int *theCascade)
{
	static const char	*funcName = "SetCascade_Filter_MultiBPass";

	if (multiBPassFPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	multiBPassFPtr->cascade = theCascade;
	return(TRUE);

}

/****************************** SetIndividualCascade **************************/

/*
 * This function sets the module's cascade array element.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetIndividualCascade_Filter_MultiBPass(int theIndex, int theCascade)
{
	static const char	*funcName =
	  "SetIndividualCascade_Filter_MultiBPass";

	if (multiBPassFPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (multiBPassFPtr->cascade == NULL) {
		NotifyError("%s: Cascade not set.", funcName);
		return(FALSE);
	}
	if (theIndex > multiBPassFPtr->numFilters - 1) {
		NotifyError("%s: Index value must be in the range 0 - %d (%d).",
		  funcName, multiBPassFPtr->numFilters - 1, theIndex);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	multiBPassFPtr->cascade[theIndex] = theCascade;
	return(TRUE);

}

/****************************** SetPreAttenuation *****************************/

/*
 * This function sets the module's preAttenuation array.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetPreAttenuation_Filter_MultiBPass(double *thePreAttenuation)
{
	static const char	*funcName = "SetPreAttenuation_Filter_MultiBPass";

	if (multiBPassFPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	multiBPassFPtr->preAttenuation = thePreAttenuation;
	return(TRUE);

}

/****************************** SetIndividualPreAttenuation *******************/

/*
 * This function sets the module's preAttenuation array element.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetIndividualPreAttenuation_Filter_MultiBPass(int theIndex,
  double thePreAttenuation)
{
	static const char *funcName =
	  "SetIndividualPreAttenuation_Filter_MultiBPass";

	if (multiBPassFPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (multiBPassFPtr->preAttenuation == NULL) {
		NotifyError("%s: PreAttenuation not set.", funcName);
		return(FALSE);
	}
	if (theIndex > multiBPassFPtr->numFilters - 1) {
		NotifyError("%s: Index value must be in the range 0 - %d (%d).",
		  funcName, multiBPassFPtr->numFilters - 1, theIndex);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	multiBPassFPtr->preAttenuation[theIndex] = thePreAttenuation;
	return(TRUE);

}

/****************************** SetUpperCutOffFreq ****************************/

/*
 * This function sets the module's upperCutOffFreq array.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetUpperCutOffFreq_Filter_MultiBPass(double *theUpperCutOffFreq)
{
	static const char	*funcName =
	  "SetUpperCutOffFreq_Filter_MultiBPass";

	if (multiBPassFPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	multiBPassFPtr->upperCutOffFreq = theUpperCutOffFreq;
	return(TRUE);

}

/****************************** SetIndividualUpperCutOffFreq ******************/

/*
 * This function sets the module's upperCutOffFreq array element.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetIndividualUpperCutOffFreq_Filter_MultiBPass(int theIndex,
  double theUpperCutOffFreq)
{
	static const char	*funcName =
	  "SetIndividualUpperCutOffFreq_Filter_MultiBPass";

	if (multiBPassFPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (multiBPassFPtr->upperCutOffFreq == NULL) {
		NotifyError("%s: UpperCutOffFreq not set.", funcName);
		return(FALSE);
	}
	if (theIndex > multiBPassFPtr->numFilters - 1) {
		NotifyError("%s: Index value must be in the range 0 - %d (%d).",
		  funcName, multiBPassFPtr->numFilters - 1, theIndex);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	multiBPassFPtr->upperCutOffFreq[theIndex] = theUpperCutOffFreq;
	return(TRUE);

}

/****************************** SetLowerCutOffFreq ****************************/

/*
 * This function sets the module's lowerCutOffFreq array.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetLowerCutOffFreq_Filter_MultiBPass(double *theLowerCutOffFreq)
{
	static const char	*funcName =
	  "SetLowerCutOffFreq_Filter_MultiBPass";

	if (multiBPassFPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	multiBPassFPtr->lowerCutOffFreq = theLowerCutOffFreq;
	return(TRUE);

}

/****************************** SetIndividualLowerCutOffFreq ******************/

/*
 * This function sets the module's lowerCutOffFreq array element.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetIndividualLowerCutOffFreq_Filter_MultiBPass(int theIndex,
 double theLowerCutOffFreq)
{
	static const char	*funcName =
	  "SetIndividualLowerCutOffFreq_Filter_MultiBPass";

	if (multiBPassFPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (multiBPassFPtr->lowerCutOffFreq == NULL) {
		NotifyError("%s: LowerCutOffFreq not set.", funcName);
		return(FALSE);
	}
	if (theIndex > multiBPassFPtr->numFilters - 1) {
		NotifyError("%s: Index value must be in the range 0 - %d (%d).",
		  funcName, multiBPassFPtr->numFilters - 1, theIndex);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	multiBPassFPtr->lowerCutOffFreq[theIndex] = theLowerCutOffFreq;
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
CheckPars_Filter_MultiBPass(void)
{
	static const char	*funcName = "CheckPars_Filter_MultiBPass";
	BOOLN	ok;

	ok = TRUE;
	if (multiBPassFPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (!multiBPassFPtr->numFiltersFlag) {
		NotifyError("%s: numFilters variable not set.", funcName);
		ok = FALSE;
	}
	if (multiBPassFPtr->cascade == NULL) {
		NotifyError("%s: cascade array not set.", funcName);
		ok = FALSE;
	}
	if (multiBPassFPtr->preAttenuation == NULL) {
		NotifyError("%s: preAttenuation array not set.", funcName);
		ok = FALSE;
	}
	if (multiBPassFPtr->upperCutOffFreq == NULL) {
		NotifyError("%s: upperCutOffFreq array not set.", funcName);
		ok = FALSE;
	}
	if (multiBPassFPtr->lowerCutOffFreq == NULL) {
		NotifyError("%s: lowerCutOffFreq array not set.", funcName);
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
PrintPars_Filter_MultiBPass(void)
{
	static const char	*funcName = "PrintPars_Filter_MultiBPass";
	int		i;

	if (!CheckPars_Filter_MultiBPass()) {
		NotifyError("%s: Parameters have not been correctly set.", funcName);
		return(FALSE);
	}
	DPrint("Multiple Bandpass Filter Module Parameters:-\n");
	DPrint("\tNo. Filters = %d\n", multiBPassFPtr->numFilters);
	DPrint("\t%10s\t%10s\t%10s\t%10s\n", "Pre-atten. ","   Cascade  ",
	  " L. Cutoff", " H. Cutoff");
	DPrint("\t%10s\t%10s\t%10s\t%10s\n", "(dB)", "", "(Hz)", "(Hz)");
	for (i = 0; i < multiBPassFPtr->numFilters; i++)
		DPrint("\t%10g\t%10d\t%10g\t%10g\n", multiBPassFPtr->preAttenuation[i],
		  multiBPassFPtr->cascade[i], multiBPassFPtr->lowerCutOffFreq[i],
		  multiBPassFPtr->upperCutOffFreq[i]);
	return(TRUE);

}

/****************************** ReadPars **************************************/

/*
 * This program reads a specified number of parameters from a file.
 * It returns FALSE if it fails in any way.n */

BOOLN
ReadPars_Filter_MultiBPass(char *fileName)
{
	static const char	*funcName = "ReadPars_Filter_MultiBPass";
	BOOLN	ok;
	char	*filePath;
	int		i, numFilters;
	FILE	*fp;

	filePath = GetParsFileFPath_Common(fileName);
	if ((fp = fopen(filePath, "r")) == NULL) {
		NotifyError("%s: Cannot open data file '%s'.\n", funcName, fileName);
		return(FALSE);
	}
	DPrint("%s: Reading from '%s':\n", funcName, fileName);
	Init_ParFile();
	ok = TRUE;
	if (!GetPars_ParFile(fp, "%d", &numFilters))
		ok = FALSE;
	if (!AllocNumFilters_Filter_MultiBPass(numFilters)) {
		NotifyError("%s: Cannot allocate memory for the 'numFilters' arrays.",
		 funcName);
		return(FALSE);
	}
	for (i = 0; i < numFilters; i++)
		if (!GetPars_ParFile(fp, "%d %lf %lf %lf", &multiBPassFPtr->cascade[i],
		  &multiBPassFPtr->preAttenuation[i], &multiBPassFPtr->lowerCutOffFreq[
		  i], &multiBPassFPtr->upperCutOffFreq[i]))
			ok = FALSE;
	fclose(fp);
	Free_ParFile();
	if (!ok) {
		NotifyError("%s: Not enough lines, or invalid parameters, in module "
		  "parameter file '%s'.", funcName, fileName);
		return(FALSE);
	}
	if (!SetPars_Filter_MultiBPass(numFilters, multiBPassFPtr->cascade,
	  multiBPassFPtr->preAttenuation, multiBPassFPtr->upperCutOffFreq,
	  multiBPassFPtr->lowerCutOffFreq)) {
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
SetParsPointer_Filter_MultiBPass(ModulePtr theModule)
{
	static const char	*funcName = "SetParsPointer_Filter_MultiBPass";

	if (!theModule) {
		NotifyError("%s: The module is not set.", funcName);
		return(FALSE);
	}
	multiBPassFPtr = (MultiBPassFPtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 */

BOOLN
InitModule_Filter_MultiBPass(ModulePtr theModule)
{
	static const char	*funcName = "InitModule_Filter_MultiBPass";

	if (!SetParsPointer_Filter_MultiBPass(theModule)) {
		NotifyError("%s: Cannot set parameters pointer.", funcName);
		return(FALSE);
	}
	if (!Init_Filter_MultiBPass(GLOBAL)) {
		NotifyError("%s: Could not initialise process structure.", funcName);
		return(FALSE);
	}
	theModule->parsPtr = multiBPassFPtr;
	theModule->CheckPars = CheckPars_Filter_MultiBPass;
	theModule->Free = Free_Filter_MultiBPass;
	theModule->GetUniParListPtr = GetUniParListPtr_Filter_MultiBPass;
	theModule->PrintPars = PrintPars_Filter_MultiBPass;
	theModule->ReadPars = ReadPars_Filter_MultiBPass;
	theModule->RunProcess = RunModel_Filter_MultiBPass;
	theModule->SetParsPointer = SetParsPointer_Filter_MultiBPass;
	return(TRUE);

}

/****************************** CheckData *************************************/

/*
 * This routine checks that the 'data' EarObject and input signal are
 * correctly initialised.
 * It should also include checks that ensure that the module's
 * parameters are compatible with the signal parameters, i.e. dt is
 * not too small, etc...
 */

BOOLN
CheckData_Filter_MultiBPass(EarObjectPtr data)
{
	static const char	*funcName = "CheckData_Filter_MultiBPass";

	if (data == NULL) {
		NotifyError("%s: EarObject not initialised.", funcName);
		return(FALSE);
	}
	if (!CheckInSignal_EarObject(data, funcName))
		return(FALSE);
	if (!CheckRamp_SignalData(data->inSignal[0])) {
		NotifyError("%s: Input signal not correctly initialised.", funcName);
		return(FALSE);
	}
	/*** Put additional checks here. ***/
	return(TRUE);

}

/**************************** InitProcessVariables ****************************/

/*
 * This function allocates the memory for the process variables.
 */

BOOLN
InitProcessVariables_Filter_MultiBPass(EarObjectPtr data)
{
	static const char *funcName = "InitProcessVariables_Filter_MultiBPass";
	BOOLN	ok = TRUE;
	int		i, j, k;
	double	*statePtr;
	BPassParsPtr	bPParsPtr;
	
	if (multiBPassFPtr->updateProcessVariablesFlag || data->updateProcessFlag) {
		FreeProcessVariables_Filter_MultiBPass();
		if ((multiBPassFPtr->bPassPars = (BPassParsPtr) calloc(multiBPassFPtr->
		  numFilters, sizeof(BPassPars))) == NULL) {
			NotifyError("%s: Cannot allocate memory for bPassPars array.",
			  funcName);
			return(FALSE);
		}
		for (i = 0; i < multiBPassFPtr->numFilters; i++) {
			bPParsPtr = &multiBPassFPtr->bPassPars[i];
			if ((bPParsPtr->coefficients = (BandPassCoeffsPtr *) calloc(
			  data->outSignal->numChannels, sizeof(BandPassCoeffsPtr))) ==
			  NULL) {
		 		NotifyError("%s: Out of memory for filter %d's coefficients.",
				  funcName, i);
		 		return(FALSE);
			}
			if (i == 0)
				bPParsPtr->data = data;
			else {
				if ((bPParsPtr->data = Init_EarObject("NULL")) == NULL) {
		 			NotifyError("%s: Out of memory for filter %d's data.",
					  funcName, i);
		 			return(FALSE);
				}
			}
			
		}
		multiBPassFPtr->numChannels = data->outSignal->numChannels;
	 	for (i = 0; (i < multiBPassFPtr->numFilters) && ok; i++) {
			bPParsPtr = &multiBPassFPtr->bPassPars[i];
	 		for (j = 0; j < data->outSignal->numChannels; j++)
				if ((bPParsPtr->coefficients[j] = InitBandPassCoeffs_Filters(
				  multiBPassFPtr->cascade[i], multiBPassFPtr->lowerCutOffFreq[
				  i], multiBPassFPtr->upperCutOffFreq[i], data->inSignal[
				  0]->dt)) == NULL) {
				  	NotifyError("%s: Failed initialised filter %d, channel %d.",
					  funcName, i, j);
					ok = FALSE;
				}
		}
		if (!ok) {
			FreeProcessVariables_Filter_MultiBPass();
			return(FALSE);
		}
		multiBPassFPtr->updateProcessVariablesFlag = FALSE;
	} else if (data->timeIndex == PROCESS_START_TIME) {
		for (k = 0; k < multiBPassFPtr->numFilters; k++) {
			bPParsPtr = &multiBPassFPtr->bPassPars[k];
			for (i = 0; i < data->outSignal->numChannels; i++) {
				statePtr = bPParsPtr->coefficients[i]->state;
				for (j = 0; j < multiBPassFPtr->cascade[k] *
				  FILTERS_NUM_CONTBUTT2_STATE_VARS; j++)
					*statePtr++ = 0.0;
			}
		}
	}
	return(TRUE);

}

/**************************** FreeProcessVariables ****************************/

/*
 * This routine releases the memory allocated for the process variables
 * if they have been initialised.
 */

void
FreeProcessVariables_Filter_MultiBPass(void)
{
	int			i, j;
	BPassParsPtr	bPParsPtr;

	if (!multiBPassFPtr->bPassPars)
		return;
	for (i = 0; i < multiBPassFPtr->numFilters; i++) {
		bPParsPtr = &multiBPassFPtr->bPassPars[i];
		if (bPParsPtr->coefficients) {
			for (j = 0; j < multiBPassFPtr->numChannels; j++)
				FreeBandPassCoeffs_Filters(&bPParsPtr->coefficients[j]);
			free(bPParsPtr->coefficients);
			bPParsPtr->coefficients = NULL;
		}
		if ((i != 0) && bPParsPtr->data)
			Free_EarObject(&bPParsPtr->data);
	}
	free(multiBPassFPtr->bPassPars);
	multiBPassFPtr->bPassPars = NULL;

}

/****************************** RunModel **************************************/

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
RunModel_Filter_MultiBPass(EarObjectPtr data)
{
	static const char	*funcName = "RunModel_Filter_MultiBPass";
	register	ChanData	 *inPtr, *outPtr;
	int		i, chan;
	ChanLen	j;
	BPassParsPtr	bPParsPtr;

	if (data == NULL) {
		NotifyError("%s: EarObject not initialised.", funcName);
		return(FALSE);
	}
	if (!CheckPars_Filter_MultiBPass())
		return(FALSE);
	if (!CheckData_Filter_MultiBPass(data)) {
		NotifyError("%s: Process data invalid.", funcName);
		return(FALSE);
	}
	SetProcessName_EarObject(data, "Multiple Bandpass filter Module "
	  "process.");
	if (!InitOutSignal_EarObject(data, data->inSignal[0]->numChannels,
	  data->inSignal[0]->length, data->inSignal[0]->dt)) {
		NotifyError("%s: Could not initialise the process output signal.",
		  funcName);
		return(FALSE);
	}
	if (!InitProcessVariables_Filter_MultiBPass(data)) {
		NotifyError("%s: Could not initialise the process variables.",
		  funcName);
		return(FALSE);
	}
	for (i = 0; i < multiBPassFPtr->numFilters; i++) {
		bPParsPtr = &multiBPassFPtr->bPassPars[i];
		TempInputConnection_EarObject(data, bPParsPtr->data, 1);
		InitOutFromInSignal_EarObject(bPParsPtr->data, 0);
		if (fabs(multiBPassFPtr->preAttenuation[i]) > DBL_EPSILON)
			GaindB_SignalData(bPParsPtr->data->outSignal, multiBPassFPtr->
			  preAttenuation[i]);
		BandPass_Filters(bPParsPtr->data->outSignal, bPParsPtr->coefficients);
	}
	for (chan = 0; chan < data->outSignal->numChannels; chan++) {
		for (i = 1; i < multiBPassFPtr->numFilters; i++) {
			inPtr = multiBPassFPtr->bPassPars[i].data->outSignal->channel[chan];
			outPtr = data->outSignal->channel[chan];
			for (j = 0; j < data->outSignal->length; j++)
				*outPtr++ += *inPtr++;
		}
	}
	SetProcessContinuity_EarObject(data);
	return(TRUE);

}


