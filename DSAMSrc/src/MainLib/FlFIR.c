/**********************
 *
 * File:		FlFIR.c
 * Purpose:		This routine uses the Remez exchange algorithm to calculate a
 *				Finite Impulse Response (FIR) filter which is then run as a
 *				process.
 * Comments:	Written using ModuleProducer version 1.2.9 (Aug 10 2000).
 * Author:		L. P. O'Mard.
 * Created:		04 Dec 2000
 * Updated:	
 * Copyright:	(c) 2000, CNBH University of Essex
 *
 *********************/

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "GeCommon.h"
#include "GeSignalData.h"
#include "GeEarObject.h"
#include "GeUniParMgr.h"
#include "GeModuleMgr.h"
#include "GeNSpecLists.h"
#include "FiParFile.h"
#include "UtRemez.h"
#include "UtFIRFilters.h"
#include "FlFIR.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

FIRPtr	fIRPtr = NULL;

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
Free_Filter_FIR(void)
{
	/* static const char	*funcName = "Free_Filter_FIR";  */

	if (fIRPtr == NULL)
		return(FALSE);
	FreeProcessVariables_Filter_FIR();
	if (fIRPtr->bandFreqs) {
		free(fIRPtr->bandFreqs);
		fIRPtr->bandFreqs = NULL;
	}
	if (fIRPtr->desired) {
		free(fIRPtr->desired);
		fIRPtr->desired = NULL;
	}
	if (fIRPtr->weights) {
		free(fIRPtr->weights);
		fIRPtr->weights = NULL;
	}
	if (fIRPtr->parList)
		FreeList_UniParMgr(&fIRPtr->parList);
	if (fIRPtr->parSpec == GLOBAL) {
		free(fIRPtr);
		fIRPtr = NULL;
	}
	return(TRUE);

}

/****************************** InitTypeList **********************************/

/*
 * This function initialises the 'type' list array
 */

BOOLN
InitTypeList_Filter_FIR(void)
{
	static NameSpecifier	modeList[] = {

			{ "ARBITRARY",		FILTER_FIR_ARBITRARY_TYPE },
			{ "BANDPASS",		FILTER_FIR_BANDPASS_TYPE },
			{ "DIFFERENTIATOR",	FILTER_FIR_DIFFERENTIATOR_TYPE },
			{ "HILBERT",		FILTER_FIR_HILBERT_TYPE },
			{ "USER",			FILTER_FIR_USER_TYPE },
			{ "",	FILTER_FIR_TYPE_NULL },
		};
	fIRPtr->typeList = modeList;
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
Init_Filter_FIR(ParameterSpecifier parSpec)
{
	static const char	*funcName = "Init_Filter_FIR";

	if (parSpec == GLOBAL) {
		if (fIRPtr != NULL)
			Free_Filter_FIR();
		if ((fIRPtr = (FIRPtr) malloc(sizeof(FIR))) == NULL) {
			NotifyError("%s: Out of memory for 'global' pointer", funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (fIRPtr == NULL) {
			NotifyError("%s:  'local' pointer not set.", funcName);
			return(FALSE);
		}
	}
	fIRPtr->parSpec = parSpec;
	fIRPtr->updateProcessVariablesFlag = TRUE;
	fIRPtr->diagnosticModeFlag = TRUE;
	fIRPtr->typeFlag = TRUE;
	fIRPtr->numTapsFlag = FALSE;
	fIRPtr->numBandsFlag = FALSE;
	fIRPtr->diagnosticMode = GENERAL_BOOLEAN_OFF;
	fIRPtr->type = FILTER_FIR_USER_TYPE;
	fIRPtr->numTaps = 0;
	fIRPtr->numBands = 0;
	fIRPtr->bandFreqs = NULL;
	fIRPtr->desired = NULL;
	fIRPtr->weights = NULL;

	InitTypeList_Filter_FIR();
	if (!SetUniParList_Filter_FIR()) {
		NotifyError("%s: Could not initialise parameter list.", funcName);
		Free_Filter_FIR();
		return(FALSE);
	}
	fIRPtr->coeffs = NULL;
	return(TRUE);

}

/****************************** SetUniParList *********************************/

/*
 * This function initialises and sets the module's universal parameter
 * list. This list provides universal access to the module's
 * parameters.  It expects to be called from the 'Init_' routine.
 */

BOOLN
SetUniParList_Filter_FIR(void)
{
	static const char *funcName = "SetUniParList_Filter_FIR";
	UniParPtr	pars;

	if ((fIRPtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  FILTER_FIR_NUM_PARS, NULL)) == NULL) {
		NotifyError("%s: Could not initialise parList.", funcName);
		return(FALSE);
	}
	pars = fIRPtr->parList->pars;
	SetPar_UniParMgr(&pars[FILTER_FIR_DIAGNOSTICMODE], "DIAG_MODE",
	  "Diagnostic mode ('off', 'screen' or <file name>).",
	  UNIPAR_BOOL,
	  &fIRPtr->diagnosticMode, NULL,
	  (void * (*)) SetDiagnosticMode_Filter_FIR);
	SetPar_UniParMgr(&pars[FILTER_FIR_TYPE], "TYPE",
	  "FIR filter type 'bandpass', 'differentiator' or 'Hilber').",
	  UNIPAR_NAME_SPEC,
	  &fIRPtr->type, fIRPtr->typeList,
	  (void * (*)) SetType_Filter_FIR);
	SetPar_UniParMgr(&pars[FILTER_FIR_NUMTAPS], "NUM_TAPS",
	  "Number of filter tapsl",
	  UNIPAR_INT,
	  &fIRPtr->numTaps, NULL,
	  (void * (*)) SetNumTaps_Filter_FIR);
	SetPar_UniParMgr(&pars[FILTER_FIR_NUMBANDS], "NUM_BANDS",
	  "No. of frequency bands.",
	  UNIPAR_INT,
	  &fIRPtr->numBands, NULL,
	  (void * (*)) SetNumBands_Filter_FIR);
	SetPar_UniParMgr(&pars[FILTER_FIR_BANDFREQS], "BAND_FREQ",
	  "Frequency response bands/Frequencies (Hz).",
	  UNIPAR_REAL_ARRAY,
	  &fIRPtr->bandFreqs, &fIRPtr->numBands,
	  (void * (*)) SetIndividualBand_Filter_FIR);
	SetPar_UniParMgr(&pars[FILTER_FIR_DESIRED], "FREQ_RESP",
	  "Desired frequency response (dB).",
	  UNIPAR_REAL_ARRAY,
	  &fIRPtr->desired, &fIRPtr->numBands,
	  (void * (*)) SetIndividualDesired_Filter_FIR);
	SetPar_UniParMgr(&pars[FILTER_FIR_WEIGHTS], "WEIGHTS",
	  "Positive weighting for each band",
	  UNIPAR_REAL_ARRAY,
	  &fIRPtr->weights, &fIRPtr->numBands,
	  (void * (*)) SetIndividualWeight_Filter_FIR);
	return(TRUE);

}

/****************************** GetUniParListPtr ******************************/

/*
 * This function returns a pointer to the module's universal parameter
 * list.
 */

UniParListPtr
GetUniParListPtr_Filter_FIR(void)
{
	static const char	*funcName = "GetUniParListPtr_Filter_FIR";

	if (fIRPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (fIRPtr->parList == NULL) {
		NotifyError("%s: UniParList data structure has not been initialised. "
		  "NULL returned.", funcName);
		return(NULL);
	}
	return(fIRPtr->parList);

}

/****************************** AllocNumBands *********************************/

/*
 * This function allocates the memory for the pure tone arrays.
 * It will assume that nothing needs to be done if the 'numBands' 
 * variable is the same as the current structure member value.
 * To make this work, the function needs to set the structure 'numBands'
 * parameter too.
 * It returns FALSE if it fails in any way.
 */

BOOLN
AllocNumBands_Filter_FIR(int numBands)
{
	static const char	*funcName = "AllocNumBands_Filter_FIR";

	if (numBands == fIRPtr->numBands)
		return(TRUE);
	if (fIRPtr->bandFreqs)
		free(fIRPtr->bandFreqs);
	if ((fIRPtr->bandFreqs = (double *) calloc(numBands, sizeof(double))) ==
	  NULL) {
		NotifyError("%s: Cannot allocate memory for '%d' bands/Freqs.",
		  funcName, numBands);
		return(FALSE);
	}
	if (fIRPtr->desired)
		free(fIRPtr->desired);
	if ((fIRPtr->desired = (double *) calloc(numBands, sizeof(double))) ==
	  NULL) {
		NotifyError("%s: Cannot allocate memory for '%d' desired.", funcName,
		  numBands);
		return(FALSE);
	}
	if (fIRPtr->weights)
		free(fIRPtr->weights);
	if ((fIRPtr->weights = (double *) calloc(numBands, sizeof(double))) ==
	  NULL) {
		NotifyError("%s: Cannot allocate memory for '%d' weights.", funcName,
		  numBands);
		return(FALSE);
	}
	fIRPtr->numBands = numBands;
	return(TRUE);

}

/****************************** SetPars ***************************************/

/*
 * This function sets all the module's parameters.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetPars_Filter_FIR(char *diagnosticMode, char * type, int numTaps, int numBands,
  double *bandFreqs, double *desired, double *weights)
{
	static const char	*funcName = "SetPars_Filter_FIR";
	BOOLN	ok;

	ok = TRUE;
	if (!SetDiagnosticMode_Filter_FIR(diagnosticMode))
		ok = FALSE;
	if (!SetType_Filter_FIR(type))
		ok = FALSE;
	if (!SetNumTaps_Filter_FIR(numTaps))
		ok = FALSE;
	if (!SetNumBands_Filter_FIR(numBands))
		ok = FALSE;
	if (!SetBandFreqs_Filter_FIR(bandFreqs))
		ok = FALSE;
	if (!SetDesired_Filter_FIR(desired))
		ok = FALSE;
	if (!SetWeights_Filter_FIR(weights))
		ok = FALSE;
	if (!ok)
		NotifyError("%s: Failed to set all module parameters." ,funcName);
	return(ok);

}

/****************************** SetDiagnosticMode *****************************/

/*
 * This function sets the module's diagnosticMode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetDiagnosticMode_Filter_FIR(char * theDiagnosticMode)
{
	static const char	*funcName = "SetDiagnosticMode_Filter_FIR";
	int		specifier;

	if (fIRPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theDiagnosticMode,
	  BooleanList_NSpecLists(0))) == GENERAL_BOOLEAN_NULL) {
		NotifyError("%s: Illegal switch state (%s).", funcName,
		  theDiagnosticMode);
		return(FALSE);
	}
	fIRPtr->diagnosticModeFlag = TRUE;
	fIRPtr->diagnosticMode = specifier;
	return(TRUE);

}

/****************************** SetType ***************************************/

/*
 * This function sets the module's type parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetType_Filter_FIR(char * theType)
{
	static const char	*funcName = "SetType_Filter_FIR";
	int		specifier;

	if (fIRPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theType,
		fIRPtr->typeList)) == FILTER_FIR_TYPE_NULL) {
		NotifyError("%s: Illegal name (%s).", funcName, theType);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	fIRPtr->typeFlag = TRUE;
	fIRPtr->type = specifier;
	return(TRUE);

}

/****************************** SetNumTaps ************************************/

/*
 * This function sets the module's numTaps parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetNumTaps_Filter_FIR(int theNumTaps)
{
	static const char	*funcName = "SetNumTaps_Filter_FIR";

	if (fIRPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	fIRPtr->numTapsFlag = TRUE;
	fIRPtr->numTaps = theNumTaps;
	return(TRUE);

}

/****************************** SetNumBands ***********************************/

/*
 * This function sets the module's numBands parameter.
 * It returns TRUE if the operation is successful.
 * The 'numBands' variable is set by the 'AllocNumBands_Filter_FIR' routine.
 * Additional checks should be added as required.
 */

BOOLN
SetNumBands_Filter_FIR(int theNumBands)
{
	static const char	*funcName = "SetNumBands_Filter_FIR";

	if (fIRPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (theNumBands < 1) {
		NotifyError("%s: Value must be greater then zero (%d).", funcName,
		  theNumBands);
		return(FALSE);
	}
	if (!AllocNumBands_Filter_FIR(theNumBands)) {
		NotifyError("%%s: Cannot allocate memory for the 'numBands' arrays.",
		  funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	fIRPtr->numBandsFlag = TRUE;
	return(TRUE);

}

/****************************** SetBandFreqs ********************************/

/*
 * This function sets the module's bandFreqs array.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetBandFreqs_Filter_FIR(double *theBandFreqs)
{
	static const char	*funcName = "SetBandFreqs_Filter_FIR";

	if (fIRPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	fIRPtr->bandFreqs = theBandFreqs;
	fIRPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/****************************** SetIndividualBand *****************************/

/*
 * This function sets the module's band array element.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetIndividualBand_Filter_FIR(int theIndex, double theBand)
{
	static const char	*funcName = "SetIndividualBand_Filter_FIR";

	if (fIRPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (fIRPtr->bandFreqs == NULL) {
		NotifyError("%s: BandFreqs not set.", funcName);
		return(FALSE);
	}
	if (theIndex > fIRPtr->numBands - 1) {
		NotifyError("%s: Index value must be in the range 0 - %d (%d).",
		  funcName, fIRPtr->numBands - 1, theIndex);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	fIRPtr->bandFreqs[theIndex] = theBand;
	fIRPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/****************************** SetDesired ************************************/

/*
 * This function sets the module's desired array.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetDesired_Filter_FIR(double *theDesired)
{
	static const char	*funcName = "SetDesired_Filter_FIR";

	if (fIRPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	fIRPtr->desired = theDesired;
	fIRPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/****************************** SetIndividualDesired **************************/

/*
 * This function sets the module's desired array element.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetIndividualDesired_Filter_FIR(int theIndex, double theDesired)
{
	static const char	*funcName = "SetIndividualDesired_Filter_FIR";

	if (fIRPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (fIRPtr->desired == NULL) {
		NotifyError("%s: Desired not set.", funcName);
		return(FALSE);
	}
	if (theIndex > fIRPtr->numBands - 1) {
		NotifyError("%s: Index value must be in the range 0 - %d (%d).",
		  funcName, fIRPtr->numBands - 1, theIndex);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	fIRPtr->desired[theIndex] = theDesired;
	fIRPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/****************************** SetWeights ************************************/

/*
 * This function sets the module's weights array.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetWeights_Filter_FIR(double *theWeights)
{
	static const char	*funcName = "SetWeights_Filter_FIR";

	if (fIRPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	fIRPtr->weights = theWeights;
	fIRPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/****************************** SetIndividualWeight ***************************/

/*
 * This function sets the module's weight array element.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetIndividualWeight_Filter_FIR(int theIndex, double theWeight)
{
	static const char	*funcName = "SetIndividualWeight_Filter_FIR";

	if (fIRPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (fIRPtr->weights == NULL) {
		NotifyError("%s: Weights not set.", funcName);
		return(FALSE);
	}
	if (theIndex > fIRPtr->numBands - 1) {
		NotifyError("%s: Index value must be in the range 0 - %d (%d).",
		  funcName, fIRPtr->numBands - 1, theIndex);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	fIRPtr->weights[theIndex] = theWeight;
	fIRPtr->updateProcessVariablesFlag = TRUE;
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
CheckPars_Filter_FIR(void)
{
	static const char	*funcName = "CheckPars_Filter_FIR";
	BOOLN	ok;
	int		i;

	ok = TRUE;
	if (fIRPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (!fIRPtr->diagnosticModeFlag) {
		NotifyError("%s: diagnosticMode variable not set.", funcName);
		ok = FALSE;
	}
	if (!fIRPtr->typeFlag) {
		NotifyError("%s: type variable not set.", funcName);
		ok = FALSE;
	}
	if (!fIRPtr->numTapsFlag) {
		NotifyError("%s: numTaps variable not set.", funcName);
		ok = FALSE;
	}
	if (!fIRPtr->numBandsFlag) {
		NotifyError("%s: numBands variable not set.", funcName);
		ok = FALSE;
	}
	if (fIRPtr->bandFreqs == NULL) {
		NotifyError("%s: bandFreqs array not set.", funcName);
		ok = FALSE;
	}
	if (fIRPtr->desired == NULL) {
		NotifyError("%s: desired array not set.", funcName);
		ok = FALSE;
	}
	if (fIRPtr->weights == NULL) {
		NotifyError("%s: weights array not set.", funcName);
		ok = FALSE;
	}
	if (fIRPtr->type == FILTER_FIR_USER_TYPE) {
		SetNumTaps_Filter_FIR(fIRPtr->numBands);
		for (i = 0; ok && (i < fIRPtr->numBands - 1); i++)
			if ((fIRPtr->bandFreqs[i] != 0.0) || (fIRPtr->weights[i] != 0.0)) {
				NotifyError("%s: the frequency bands and weights columns must "
				  "have zero values in USER type filters.", funcName);
				ok = FALSE;
			}
	} else {
		for (i = 0; ok && (i < fIRPtr->numBands - 1); i++)
			if (fIRPtr->bandFreqs[i] >= fIRPtr->bandFreqs[i + 1]) {
				NotifyError("%s: Band centres/Frequencies must be in ascending "
				  "order (No.s %d and %d).", funcName, i, i + 1);
				ok = FALSE;
			}
	}
	return(ok);

}

/****************************** PrintPars *************************************/

/*
 * This routine prints all the module's parameters to the file stream.
 * specified by the lUTEar.parsFile file pointer.
 */

BOOLN
PrintPars_Filter_FIR(void)
{
	static const char	*funcName = "PrintPars_Filter_FIR";
	int		i;

	if (!CheckPars_Filter_FIR()) {
		NotifyError("%s: Parameters have not been correctly set.", funcName);
		return(FALSE);
	}
	DPrint("FIR Filter Module Parameters:-\n");
	DPrint("\tDiagnostics mode: %s,\n", BooleanList_NSpecLists(fIRPtr->
	  diagnosticMode)->name);
	DPrint("\t%10s\t%10s\t%10s\n", "Band/Freqs", "desired", "weights");
	DPrint("\t%10s\t%10s\t%10s\n", "(Hz)", "(\?\?)", "(>0)");
	if (fIRPtr->diagnosticMode != GENERAL_DIAGNOSTIC_OFF_MODE) {
		for (i = 0; i < fIRPtr->numBands; i++)
			DPrint("\t%10g\t%10g\t%10g\n", fIRPtr->bandFreqs[i], fIRPtr->
			  desired[i], fIRPtr->weights[i]);
	} else
		DPrint("\tList of %d elements (off because of diagnostic mode).\n",
		  fIRPtr->numBands);
	DPrint("\tFilter type = %s \n", fIRPtr->typeList[fIRPtr->type].name);
	DPrint("\tnumTaps = %d ??\n", fIRPtr->numTaps);
	DPrint("\tnumBands = %d ??\n", fIRPtr->numBands);
	return(TRUE);

}

/****************************** ReadPars **************************************/

/*
 * This program reads a specified number of parameters from a file.
 * It returns FALSE if it fails in any way.n */

BOOLN
ReadPars_Filter_FIR(char *fileName)
{
	static const char	*funcName = "ReadPars_Filter_FIR";
	BOOLN	ok;
	char	*filePath, type[MAXLINE], diagnosticMode[MAX_FILE_PATH];
	int		i, numTaps, numBands = 0;
	FILE	*fp;

	filePath = GetParsFileFPath_Common(fileName);
	if ((fp = fopen(filePath, "r")) == NULL) {
		NotifyError("%s: Cannot open data file '%s'.\n", funcName, fileName);
		return(FALSE);
	}
	DPrint("%s: Reading from '%s':\n", funcName, fileName);
	Init_ParFile();
	ok = TRUE;
	if (!GetPars_ParFile(fp, "%s", diagnosticMode))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%s", type))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%d", &numTaps))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%d", &numBands))
		ok = FALSE;
	if (ok && !AllocNumBands_Filter_FIR(numBands)) {
		NotifyError("%%s: Cannot allocate memory for the 'numBands' arrays.",
		  funcName);
		return(FALSE);
	}
	for (i = 0; i < numBands; i++)
		if (!GetPars_ParFile(fp, "%lf %lf %lf", &fIRPtr->bandFreqs[i],
		  &fIRPtr->desired[i], &fIRPtr->weights[i]))
			ok = FALSE;
	fclose(fp);
	Free_ParFile();
	if (!ok) {
		NotifyError("%s: Not enough lines, or invalid parameters, in module "
		  "parameter file '%s'.", funcName, fileName);
		return(FALSE);
	}
	if (!SetPars_Filter_FIR(diagnosticMode, type, numTaps, numBands, fIRPtr->
	  bandFreqs, fIRPtr->desired, fIRPtr->weights)) {
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
SetParsPointer_Filter_FIR(ModulePtr theModule)
{
	static const char	*funcName = "SetParsPointer_Filter_FIR";

	if (!theModule) {
		NotifyError("%s: The module is not set.", funcName);
		return(FALSE);
	}
	fIRPtr = (FIRPtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 */

BOOLN
InitModule_Filter_FIR(ModulePtr theModule)
{
	static const char	*funcName = "InitModule_Filter_FIR";

	if (!SetParsPointer_Filter_FIR(theModule)) {
		NotifyError("%s: Cannot set parameters pointer.", funcName);
		return(FALSE);
	}
	if (!Init_Filter_FIR(GLOBAL)) {
		NotifyError("%s: Could not initialise process structure.", funcName);
		return(FALSE);
	}
	theModule->parsPtr = fIRPtr;
	theModule->CheckPars = CheckPars_Filter_FIR;
	theModule->Free = Free_Filter_FIR;
	theModule->GetUniParListPtr = GetUniParListPtr_Filter_FIR;
	theModule->PrintPars = PrintPars_Filter_FIR;
	theModule->ReadPars = ReadPars_Filter_FIR;
	theModule->RunProcess = RunProcess_Filter_FIR;
	theModule->SetParsPointer = SetParsPointer_Filter_FIR;
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
CheckData_Filter_FIR(EarObjectPtr data)
{
	static const char	*funcName = "CheckData_Filter_FIR";

	if (data == NULL) {
		NotifyError("%s: EarObject not initialised.", funcName);
		return(FALSE);
	}
	if (!CheckInit_SignalData(data->inSignal[0], funcName))
		return(FALSE);
	/*** Put additional checks here. ***/
	return(TRUE);

}

/****************************** GetType ***************************************/

/*
 * This function returns the type of a filter according to the UtRemez types.
 * This is used so that there is no assumed corresponence between the UtRemez
 * module's constants and this Module's specifiers.
 */

int
GetType_Filter_FIR(void)
{
	switch (fIRPtr->type) {
	case FILTER_FIR_ARBITRARY_TYPE:
		return(-BANDPASS);
	case FILTER_FIR_BANDPASS_TYPE:
		return(BANDPASS);
	case FILTER_FIR_DIFFERENTIATOR_TYPE:
		return(DIFFERENTIATOR);
	case FILTER_FIR_HILBERT_TYPE:
		return(HILBERT);
	default:
		return(-1);	/* straight user coeffs. */
	}
	
}

/**************************** InitProcessVariables ****************************/

/*
 * This function allocates the memory for the process variables.
 */

BOOLN
InitProcessVariables_Filter_FIR(EarObjectPtr data)
{
	static const char *funcName = "InitProcessVariables_Filter_FIR";
	int		i, totalStateLength;
	double	*statePtr;
	
	if (fIRPtr->updateProcessVariablesFlag || data->updateProcessFlag) {
		FreeProcessVariables_Filter_FIR();
		if ((fIRPtr->coeffs = InitFIRCoeffs_FIRFilters(
		  data->outSignal->numChannels, fIRPtr->numTaps, fIRPtr->numBands,
		  fIRPtr->bandFreqs, fIRPtr->desired, fIRPtr->weights,
		  GetType_Filter_FIR())) == NULL) {
			NotifyError("%s: Could not initialises FIR coefficients.",
			  funcName);
			FreeProcessVariables_Filter_FIR();
			return(FALSE);
		}
		
		fIRPtr->updateProcessVariablesFlag = FALSE;
	} else if (data->timeIndex == PROCESS_START_TIME) {
		totalStateLength = fIRPtr->coeffs->numChannels * fIRPtr->coeffs->m;
		statePtr = fIRPtr->coeffs->state;
		for (i = 0; i < totalStateLength; i++)
			*statePtr++ = 0.0;
	}
	return(TRUE);

}

/**************************** FreeProcessVariables ****************************/

/*
 * This routine releases the memory allocated for the process variables
 * if they have been initialised.
 */

void
FreeProcessVariables_Filter_FIR(void)
{

	if (fIRPtr->coeffs == NULL)
		return;
	FreeFIRCoeffs_FIRFilters(&fIRPtr->coeffs);

}

/****************************** RunProcess ************************************/

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
RunProcess_Filter_FIR(EarObjectPtr data)
{
	static const char	*funcName = "RunProcess_Filter_FIR";

	if (data == NULL) {
		NotifyError("%s: EarObject not initialised.", funcName);
		return(FALSE);
	}
	if (!CheckPars_Filter_FIR())
		return(FALSE);
	if (!CheckData_Filter_FIR(data)) {
		NotifyError("%s: Process data invalid.", funcName);
		return(FALSE);
	}
	SetProcessName_EarObject(data, "FIR filter module process");
	if (!InitOutSignal_EarObject(data, data->inSignal[0]->numChannels, 
	  data->inSignal[0]->length, data->inSignal[0]->dt)) {
		NotifyError("%s: Could not initialise the process output signal.",
		  funcName);
		return(FALSE);
	}
	if (!InitProcessVariables_Filter_FIR(data)) {
		NotifyError("%s: Could not initialise the process variables.",
		  funcName);
		return(FALSE);
	}
	FIR_FIRFilters(data, fIRPtr->coeffs);

	SetProcessContinuity_EarObject(data);
	return(TRUE);

}

