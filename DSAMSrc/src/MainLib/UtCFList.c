/**********************
 *
 * File:		UtCFList.c
 * Purpose:		This file contains the centre frequency management routines.
 * Comments:	The "bandWidth" field is set using the UtBandWidth module.
 *				06-10-98 LPO: The CFList type is now saved as the specifier,
 *				rather than as a string.  The 'minCf', 'maxCF' and 'eRBDensity'
 *				values are now saved with the CFList, so that the parameter list
 *				can always know these values.
 *				12-10-98 LPO: Introduced the 'SetParentCFList_CFList' and the
 *				'parentPtr' so that a CFList can always set the pointer which is
 *				pointing to it.
 *				19-11-98 LPO: Changed code so that a NULL array for the user
 *				modes causes the frequency and bandwidth arrays to have space
 *				allocated for them.  This was needed for the universal
 *				parameters implementation.
 *				26-11-98 LPO: Added the 'oldNumChannels' private parameter so
 *				that the user mode can use the parameters of the old frequency
 *				array if it exists.
 *				17-06-99 LPO: The 'SetIndividualFreq' routine now sets the
 *				CFList's 'updateFlag' field when a frequency is set.  This is
 *				needed because modules, such as 'BM_Carney' need to know if the
 *				bandwidths need recalculating.
 *				06-11-00 LPO: Implented the Greenwood function with the
 *				functions and values provided by Steve D. Holmes.
 * Author:		L. P. O'Mard
 * Created:		12 Jul 1993
 * Updated:		17 Jun 1999
 * Copyright:	(c) 1999, University of Essex
 *
 *********************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG_H */

#include "GeCommon.h"
#include "GeUniParMgr.h"
#include "FiParFile.h"

#include "UtBandwidth.h"
#include "UtCFList.h"

/******************************************************************************/
/****************************** Global Variables ******************************/
/******************************************************************************/

static NameSpecifier	cFListDiagModeList[] = {

							{ "LIST", 		CFLIST_DIAG_MODE },
							{ "PARAMETERS", CFLIST_PARAMETERS_DIAG_MODE },
							{ "",			CFLIST_DIAG_NULL }
						};

/******************************************************************************/
/****************************** Subroutines & functions ***********************/
/******************************************************************************/

/****************************** Init ******************************************/

/*
 * This function allocates memory for the centre frequency data structure.
 * The function returns a pointer to the prepared structure.
 */
 
CFListPtr
Init_CFList(const char *callingFunctionName)
{
	static const char *funcName = "Init_CFList";
	CFListPtr theCFs;
	
	if ((theCFs = (CFListPtr) malloc(sizeof(CFList))) == NULL ) {
		NotifyError("%s: Out of Memory (called by %s).", funcName,
		  callingFunctionName);
		return(NULL);
	}
	theCFs->minCFFlag = FALSE;
	theCFs->maxCFFlag = FALSE;
	theCFs->updateFlag = TRUE;
	theCFs->numChannels = 0;
	theCFs->minCF = 0.0;
	theCFs->maxCF = 0.0;
	theCFs->focalCF = 0.0;
	theCFs->eRBDensity = 0.0;
	theCFs->frequency = NULL;
	theCFs->bandwidth = NULL;
	theCFs->bandwidthMode.specifier = BANDWIDTH_NULL;
	theCFs->diagnosticMode = CFLIST_DIAG_NULL;
	theCFs->centreFreqMode = CFLIST_NULL;

	theCFs->cFParList = NULL;
	theCFs->bParList = NULL;
	theCFs->oldNumChannels = 0;
	return(theCFs);
	
}

/****************************** Free ******************************************/

/*
 * This function de-allocates memory for the centre frequency data structure.
 */
 
void
Free_CFList(CFListPtr *theCFs)
{
	if (*theCFs == NULL)
		return;
	if ((*theCFs)->frequency != NULL)
		free((*theCFs)->frequency);
	if ((*theCFs)->bandwidth != NULL)
		free((*theCFs)->bandwidth);
	FreeList_UniParMgr(&(*theCFs)->cFParList);
	FreeList_UniParMgr(&(*theCFs)->bParList);
	free((*theCFs));
	*theCFs = NULL;

}

/********************************* CFModeList *********************************/

/*
 * This function returns a CF mode list entry.
 */

NameSpecifier *
CFModeList_CFList(int index)
{
	static NameSpecifier	modeList[] = {

			{ "SINGLE", 	CFLIST_SINGLE_MODE },
			{ "USER", 		CFLIST_USER_MODE },
			{ "ERB", 		CFLIST_ERB_MODE },
			{ "ERB_N", 		CFLIST_ERBN_MODE },
			{ "LOG", 		CFLIST_LOG_MODE },
			{ "FOCAL_LOG",	CFLIST_FOCAL_LOG_MODE },
			{ "LINEAR",		CFLIST_LINEAR_MODE },
			{ "CAT",		CFLIST_CAT_MODE },
			{ "CHINCHILLA",	CFLIST_CHINCHILLA_MODE },
			{ "GUINEA_PIG",	CFLIST_GPIG_MODE },
			{ "HUMAN",		CFLIST_HUMAN_MODE },
			{ "MACAQUE",	CFLIST_MACAQUEM_MODE },
			{ "",			CFLIST_NULL }
		};
	return (&modeList[index]);

}

/********************************* RegenerateList *****************************/

/*
 * This routine regenerates a list from an existing CFList structure with the
 * appropriate parameters and arrays set for the current parameters.
 * It returns FALSE if it fails in any way.
 */

BOOLN
RegenerateList_CFList(CFListPtr theCFs)
{
	static const char *funcName = "RegenerateList_CFList";
	int		i, numChannels;
	double	*frequencies, *bandwidths;

	if (((theCFs->centreFreqMode == CFLIST_USER_MODE) ||
	  (theCFs->centreFreqMode == CFLIST_SINGLE_MODE)) &&
	  (theCFs->oldNumChannels != theCFs->numChannels)) {
		if ((frequencies = (double *) calloc(theCFs->numChannels,
		  sizeof(double))) == NULL) {
			NotifyError("%s: Out of memory for frequency array (%d).",
			  funcName, theCFs->numChannels);
			return(FALSE);
		}
		numChannels = MINIMUM(theCFs->oldNumChannels, theCFs->numChannels);
		for (i = 0; i < numChannels; i++)
			frequencies[i] = theCFs->frequency[i];
		free(theCFs->frequency);
		theCFs->frequency = frequencies;
	}
	if ((theCFs->bandwidthMode.specifier == BANDWIDTH_USER) &&
	  (theCFs->oldNumChannels != theCFs->numChannels)) {
		if ((bandwidths = (double *) calloc(theCFs->numChannels,
		  sizeof(double))) == NULL) {
			NotifyError("%s: Out of memory for bandwidth array (%d).",
			  funcName, theCFs->numChannels);
			return(FALSE);
		}
		numChannels = MINIMUM(theCFs->oldNumChannels, theCFs->numChannels);
		for (i = 0; i < numChannels; i++)
			bandwidths[i] = theCFs->bandwidth[i];
		free(theCFs->bandwidth);
		theCFs->bandwidth = bandwidths;
	}
	if (!SetGeneratedPars_CFList(theCFs)) {
		NotifyError("%s: Could not regenerate CF list.", funcName);
		return(FALSE);
	}
	if (!SetBandwidthArray_CFList(theCFs, theCFs->bandwidth)) {
		NotifyError("%s: Could not regenerate CF list.", funcName);
		return(FALSE);
	}
	return(TRUE);

}

/********************************* SetDiagnosticMode **************************/

/*
 * This routine sets the diagnostic mode of a CFList.
 */

BOOLN
SetDiagnosticMode_CFList(CFListPtr theCFs, char *modeName)
{
	static const char *funcName = "SetDiagnosticMode_CFList";
	int		mode;

	if ((mode = Identify_NameSpecifier(modeName, cFListDiagModeList)) == 
	  CFLIST_DIAG_NULL) {
		NotifyError("%s: Unknown diagnostic mode (%s).", funcName, modeName);
		return(FALSE);
	}
	theCFs->diagnosticMode = (CFListDiagModeSpecifier) mode;
	return(TRUE);

}

/********************************* SetCFMode **********************************/

/*
 * This routine sets the diagnostic mode of a CFList.
 */

BOOLN
SetCFMode_CFList(CFListPtr theCFs, char *modeName)
{
	static const char *funcName = "SetCFMode_CFList";
	int		mode;

	if (!CheckInit_CFList(theCFs, funcName))
		return(FALSE);
	if ((mode = Identify_NameSpecifier(modeName, CFModeList_CFList(0))) ==
	  CFLIST_NULL) {
		NotifyError("%s: Unknown CF mode (%s).", funcName, modeName);
		return(FALSE);
	}
	theCFs->centreFreqMode = (CFListSpecifier) mode;
	switch (mode) {
	case CFLIST_SINGLE_MODE:
		theCFs->numChannels = 1;
		break;
	case CFLIST_ERB_MODE:
	case CFLIST_ERBN_MODE:
	case CFLIST_LOG_MODE:
	case CFLIST_FOCAL_LOG_MODE:
	case CFLIST_LINEAR_MODE:
	case CFLIST_CAT_MODE:
	case CFLIST_CHINCHILLA_MODE:
	case CFLIST_GPIG_MODE:
	case CFLIST_HUMAN_MODE:
	case CFLIST_MACAQUEM_MODE:
		if (theCFs->numChannels < 2)
			theCFs->numChannels = 2;
		break;
	default:
		;
	}
	if (theCFs->cFParList) {
		SetCFUniParListMode_CFList(theCFs);
		theCFs->cFParList->updateFlag = TRUE;
	}
	return(TRUE);

}

/********************************* SetMinCF ***********************************/

/*
 * This routine sets the minimum frequency of a CFList.
 */

BOOLN
SetMinCF_CFList(CFListPtr theCFs, double minCF)
{
	static const char *funcName = "SetMinCF_CFList";

	if (!CheckInit_CFList(theCFs, funcName))
		return(FALSE);
	if (minCF <= 0.0) {
		NotifyError("%s: Illegal frequency (%g Hz).", funcName, minCF);
		return(FALSE);
	}
	theCFs->minCF = minCF;
	theCFs->minCFFlag = TRUE;
	if (!theCFs->maxCFFlag && (theCFs->minCF > theCFs->maxCF))
		theCFs->maxCF = theCFs->minCF * 2.0;
	if (theCFs->cFParList)
		theCFs->cFParList->updateFlag = TRUE;
	return(TRUE);

}

/********************************* SetMaxCF ***********************************/

/*
 * This routine sets the maximum frequency of a CFList.
 */

BOOLN
SetMaxCF_CFList(CFListPtr theCFs, double maxCF)
{
	static const char *funcName = "SetMaxCF_CFList";

	if (!CheckInit_CFList(theCFs, funcName))
		return(FALSE);
	if (maxCF <= 0.0) {
		NotifyError("%s: Illegal frequency (%g Hz).", funcName, maxCF);
		return(FALSE);
	}
	theCFs->maxCF = maxCF;
	theCFs->maxCFFlag = TRUE;
	if (!theCFs->minCFFlag && (theCFs->maxCF < theCFs->minCF))
		theCFs->minCF = theCFs->maxCF / 2.0;
	if (theCFs->cFParList)
		theCFs->cFParList->updateFlag = TRUE;
	return(TRUE);

}

/********************************* SetFocalCF *********************************/

/*
 * This routine sets the maximum frequency of a CFList.
 */

BOOLN
SetFocalCF_CFList(CFListPtr theCFs, double focalCF)
{
	static const char *funcName = "SetFocalCF_CFList";

	if (!CheckInit_CFList(theCFs, funcName))
		return(FALSE);
	if (focalCF <= 0.0) {
		NotifyError("%s: Illegal frequency (%g Hz).", funcName, focalCF);
		return(FALSE);
	}
	theCFs->focalCF = focalCF;
	if (theCFs->cFParList)
		theCFs->cFParList->updateFlag = TRUE;
	return(TRUE);

}

/********************************* SetNumChannels *****************************/

/*
 * This routine sets the numChannels field of a CFList.
 */

BOOLN
SetNumChannels_CFList(CFListPtr theCFs, int numChannels)
{
	static const char *funcName = "SetNumChannels_CFList";

	if (!CheckInit_CFList(theCFs, funcName))
		return(FALSE);
	if (numChannels <= 0) {
		NotifyError("%s: Illegal number of channels (%d).", funcName,
		  numChannels);
		return(FALSE);
	}
	if (theCFs->centreFreqMode == CFLIST_SINGLE_MODE) {
		NotifyError("%s: You cannot set the number of channels when single "
		  "mode is set.  First change the mode.", funcName);
		return(FALSE);
	}
	theCFs->numChannels = numChannels;
	if (theCFs->cFParList)
		theCFs->cFParList->updateFlag = TRUE;
	return(TRUE);

}

/********************************* SetERBDensity ******************************/

/*
 * This routine sets the maximum frequency of a CFList.
 */

BOOLN
SetERBDensity_CFList(CFListPtr theCFs, double eRBDensity)
{
	static const char *funcName = "SetERBDensity_CFList";

	if (!CheckInit_CFList(theCFs, funcName))
		return(FALSE);
	if (eRBDensity <= 0.0) {
		NotifyError("%s: Illegal ERB density (%g filters/critical band).",
		  funcName, eRBDensity);
		return(FALSE);
	}
	theCFs->eRBDensity = eRBDensity;
	if (theCFs->cFParList)
		theCFs->cFParList->updateFlag = TRUE;
	return(TRUE);

}

/********************************* SetSingleFrequency *************************/

/*
 * This function sets a single frequency value of a CFList.
 * It first checks if the frequencies have been set.
 */

BOOLN
SetSingleFrequency_CFList(CFListPtr theCFs, double theFrequency)
{
	static const char *funcName = "SetSingleFrequency_CFList";

	if (!SetIndividualFreq_CFList(theCFs, 0, theFrequency)) {
		NotifyError("%s: Could not set frequency.", funcName);
		return(FALSE);
	}
	return(TRUE);

}

/********************************* SetIndividualFreq **************************/

/*
 * This function sets the individual frequency values of a CFList.
 * It first checks if the frequencies have been set.
 */

BOOLN
SetIndividualFreq_CFList(CFListPtr theCFs, int theIndex, double theFrequency)
{
	static const char *funcName = "SetIndividualFreq_CFList";

	if (!CheckInit_CFList(theCFs, funcName))
		return(FALSE);
	if (theCFs->frequency == NULL) {
		NotifyError("%s: Frequencies not set.", funcName);
		return(FALSE);
	}
	if (theIndex > theCFs->numChannels - 1) {
		NotifyError("%s: Index value must be in the\nrange 0 - %d (%d).\n",
		  funcName, theCFs->numChannels - 1, theIndex);
		return(FALSE);
	}
	theCFs->frequency[theIndex] = theFrequency;
	theCFs->updateFlag = TRUE;
	return(TRUE);

}

/********************************* SetIndividualBandwidth *********************/

/*
 * This function sets the individual bandwidth values of a CFList.
 * It first checks if the frequencies have been set.
 */

BOOLN
SetIndividualBandwidth_CFList(CFListPtr theCFs, int theIndex,
  double theBandwidth)
{
	static const char *funcName = "SetIndividualBandwidth_CFList";

	if (!CheckInit_CFList(theCFs, funcName))
		return(FALSE);
	if (theCFs->bandwidth == NULL) {
		NotifyError("%s: Frequencies not set.", funcName);
		return(FALSE);
	}
	if (theIndex > theCFs->numChannels - 1) {
		NotifyError("%s: Index value must be in the\nrange 0 - %d (%d).\n",
		  funcName, theCFs->numChannels - 1, theIndex);
		return(FALSE);
	}
	theCFs->bandwidth[theIndex] = theBandwidth;
	return(TRUE);

}

/********************************* SetCFUniParListMode ************************/

/*
 * This routine enables and disables the respective parameters for each CF mode.
 * It assumes that the parameter list has been correctly initialised.
 */
 
BOOLN
SetCFUniParListMode_CFList(CFListPtr theCFs)
{
	static const char *funcName = "SetCFUniParListMode_CFList";
	int		i;

	if (!CheckInit_CFList(theCFs, funcName))
		return(FALSE);
	for (i = CFLIST_NUM_CONSTANT_PARS; i < CFLIST_NUM_PARS; i++)
		theCFs->cFParList->pars[i].enabled = FALSE;
	switch (theCFs->centreFreqMode) {
	case CFLIST_ERBN_MODE:
	case CFLIST_LOG_MODE:
	case CFLIST_LINEAR_MODE:
	case CFLIST_CAT_MODE:
	case CFLIST_CHINCHILLA_MODE:
	case CFLIST_GPIG_MODE:
	case CFLIST_HUMAN_MODE:
	case CFLIST_MACAQUEM_MODE:
		theCFs->cFParList->pars[CFLIST_CF_MIN_FREQ].enabled = TRUE;
		theCFs->cFParList->pars[CFLIST_CF_MAX_FREQ].enabled = TRUE;
		theCFs->cFParList->pars[CFLIST_CF_NUM_CHANNELS].enabled = TRUE;
		break;
	case CFLIST_ERB_MODE:
		theCFs->cFParList->pars[CFLIST_CF_MIN_FREQ].enabled = TRUE;
		theCFs->cFParList->pars[CFLIST_CF_MAX_FREQ].enabled = TRUE;
		theCFs->cFParList->pars[CFLIST_CF_ERB_DENSITY].enabled = TRUE;
		break;
	case CFLIST_FOCAL_LOG_MODE:
		theCFs->cFParList->pars[CFLIST_CF_FOCAL_FREQ].enabled = TRUE;
		theCFs->cFParList->pars[CFLIST_CF_MIN_FREQ].enabled = TRUE;
		theCFs->cFParList->pars[CFLIST_CF_MAX_FREQ].enabled = TRUE;
		theCFs->cFParList->pars[CFLIST_CF_NUM_CHANNELS].enabled = TRUE;
		break;
	case CFLIST_SINGLE_MODE:
		theCFs->cFParList->pars[CFLIST_CF_SINGLE_FREQ].enabled = TRUE;
		break;
	case CFLIST_USER_MODE:
		theCFs->cFParList->pars[CFLIST_CF_NUM_CHANNELS].enabled = TRUE;
		theCFs->cFParList->pars[CFLIST_CF_FREQUENCIES].enabled = TRUE;
		break;
	default:
		NotifyError("%s: CF mode (%d) not implemented.", funcName,
		  theCFs->centreFreqMode);
		return(FALSE);
	}
	return(TRUE);

}

/********************************* SetCFUniParList ****************************/

/*
 * This routine initialises and sets the CFList's universal parameter list.
 * This list provides universal access to the CFList's parameters.
 */
 
BOOLN
SetCFUniParList_CFList(CFListPtr theCFs)
{
	static const char *funcName = "SetCFUniParList_CFList";
	UniParPtr	pars;

	if (!CheckInit_CFList(theCFs, funcName))
		return(FALSE);
	if (!theCFs->cFParList && ((theCFs->cFParList = InitList_UniParMgr(
	  UNIPAR_SET_CFLIST, CFLIST_NUM_PARS, theCFs)) == NULL)) {
		NotifyError("%s: Could not initialise CF parList.", funcName);
		return(FALSE);
	}
	pars = theCFs->cFParList->pars;
	SetPar_UniParMgr(&pars[CFLIST_CF_DIAGNOSTIC_MODE], "DIAG_MODE",
	 "Diagnostic mode ('list' or 'parameters').",
	  UNIPAR_NAME_SPEC,
	  &theCFs->diagnosticMode, cFListDiagModeList,
	  (void * (*)) SetDiagnosticMode_CFList);
	SetPar_UniParMgr(&pars[CFLIST_CF_MODE], "CF_MODE",
	 "Centre frequency mode ('single', 'ERB', 'ERB_n', 'log', 'linear', "
	 "'focal_log', 'user', 'human', 'cat', 'chinchilla', 'guinea-pig' or "
	 "'macaque').",
	  UNIPAR_NAME_SPEC,
	  &theCFs->centreFreqMode, CFModeList_CFList(0),
	  (void * (*)) SetCFMode_CFList);
	SetPar_UniParMgr(&pars[CFLIST_CF_SINGLE_FREQ], "SINGLE_CF",
	  "Centre frequency (Hz).",
	  UNIPAR_REAL,
	  &theCFs->frequency[0], NULL,
	  (void * (*)) SetSingleFrequency_CFList);
	SetPar_UniParMgr(&pars[CFLIST_CF_FOCAL_FREQ], "FOCAL_CF",
	  "Focal centre frequency (Hz).",
	  UNIPAR_REAL,
	  &theCFs->focalCF, NULL,
	  (void * (*)) SetFocalCF_CFList);
	SetPar_UniParMgr(&pars[CFLIST_CF_MIN_FREQ], "MIN_CF",
	  "Minimum centre frequency (Hz).",
	  UNIPAR_REAL,
	  &theCFs->minCF, NULL,
	  (void * (*)) SetMinCF_CFList);
	SetPar_UniParMgr(&pars[CFLIST_CF_MAX_FREQ], "MAX_CF",
	  "Maximum centre frequency (Hz).",
	  UNIPAR_REAL,
	  &theCFs->maxCF, NULL,
	  (void * (*)) SetMaxCF_CFList);
	SetPar_UniParMgr(&pars[CFLIST_CF_NUM_CHANNELS], "CHANNELS",
	  "No. of centre frequencies.",
	  UNIPAR_INT,
	  &theCFs->numChannels, NULL,
	  (void *(*)) SetNumChannels_CFList);
	SetPar_UniParMgr(&pars[CFLIST_CF_ERB_DENSITY], "ERB_DENSITY",
	  "ERB density (filters/critical band).",
	  UNIPAR_REAL,
	  &theCFs->eRBDensity, NULL,
	 (void *(*)) SetERBDensity_CFList);
	SetPar_UniParMgr(&pars[CFLIST_CF_FREQUENCIES], "CENTRE_FREQ",
	  "Centre frequencies (Hz).",
	  UNIPAR_REAL_ARRAY,
	  &theCFs->frequency, &theCFs->numChannels,
	  (void * (*)) SetIndividualFreq_CFList);
	return(TRUE);

}

/********************************* SetBandwidthSpecifier **********************/

/*
 * This routine sets the bandwidth specifier.
 * It also initialises the CFList data structure, if it has not already been
 * initialised.
 */

BOOLN
SetBandwidthSpecifier_CFList(CFListPtr theCFs, char *modeName)
{
	static const char *funcName = "SetBandwidthSpecifier_CFList(";

	if (!CheckInit_CFList(theCFs, funcName))
		return(FALSE);
	if (!SetMode_Bandwidth(&theCFs->bandwidthMode, modeName)) {
		NotifyError("%s: Could not set bandwidth mode.", funcName);
		return(FALSE);
	}
	SetBandwidthArray_CFList(theCFs, theCFs->bandwidth);
	theCFs->updateFlag = TRUE;
	if (theCFs->bParList)
		SetBandwidthUniParListMode_CFList(theCFs);
	return(TRUE);

}

/********************************* SetBandwidthMin ****************************/

/*
 * This routine sets the bandwidth bwMin parameter.
 * At present the 'USER' mode is not implemented.
 */

BOOLN
SetBandwidthMin_CFList(CFListPtr theCFs, double bwMin)
{
	static const char *funcName = "SetBandwidthMin_CFList";

	if (!CheckInit_CFList(theCFs, funcName))
		return(FALSE);
	if (bwMin <= 0.0) {
		NotifyError("%s: Illegal minimum bandwith (%g Hz).", funcName, bwMin);
		return(FALSE);
	}
	theCFs->bandwidthMode.bwMin = bwMin;
	return(TRUE);

}

/********************************* SetBandwidthQuality ************************/

/*
 * This routine sets the bandwidth Quality parameter.
 * At present the 'USER' mode is not implemented.
 */

BOOLN
SetBandwidthQuality_CFList(CFListPtr theCFs, double quality)
{
	static const char *funcName = "SetBandwidthQuality_CFList";

	if (!CheckInit_CFList(theCFs, funcName))
		return(FALSE);
	if (quality <= 0.0) {
		NotifyError("%s: Illegal minimum bandwith (%g Hz).", funcName, quality);
		return(FALSE);
	}
	theCFs->bandwidthMode.quality = quality;
	return(TRUE);

}

/********************************* SetBandwidthUniParListMode *****************/

/*
 * This routine enables and disables the respective parameters for each 
 * bandwidth mode.
 * It assumes that the parameter list has been correctly initialised.
 */
 
BOOLN
SetBandwidthUniParListMode_CFList(CFListPtr theCFs)
{
	static const char *funcName = "SetBandwidthUniParListMode_CFList";
	int		i;

	if (!CheckInit_CFList(theCFs, funcName))
		return(FALSE);
	for (i = BANDWIDTH_NUM_CONSTANT_PARS; i < BANDWIDTH_NUM_PARS; i++)
		theCFs->bParList->pars[i].enabled = FALSE;

	switch (theCFs->bandwidthMode.specifier) {
	case BANDWIDTH_ERB:
	case BANDWIDTH_CAT:
	case BANDWIDTH_GUINEA_PIG:
	case BANDWIDTH_NONLINEAR:
	case BANDWIDTH_DISABLED:
	case BANDWIDTH_INTERNAL_DYNAMIC:
	case BANDWIDTH_INTERNAL_STATIC:
		break;
	case BANDWIDTH_USER:
		theCFs->bParList->pars[BANDWIDTH_PAR_BANDWIDTH].enabled = TRUE;
		break;
	case BANDWIDTH_CUSTOM_ERB:
		theCFs->bParList->pars[BANDWIDTH_PAR_MIN].enabled = TRUE;
		theCFs->bParList->pars[BANDWIDTH_PAR_QUALITY].enabled = TRUE;
		break;
	default:
		NotifyError("%s: Bandwidth mode (%d) not implemented.", funcName,
		  theCFs->bandwidthMode.specifier);
		return(FALSE);
	}
	return(TRUE);

}

/********************************* SetBandwidthUniParList *********************/

/*
 * This routine initialises and sets the CFlist's universal parameter list.
 * This list provides universal access to the CFList's parameters.
 */
 
BOOLN
SetBandwidthUniParList_CFList(CFListPtr theCFs)
{
	static const char *funcName = "SetBandwidthUniParList_CFList";
	
	UniParPtr	pars;

	if (!CheckInit_CFList(theCFs, funcName))
		return(FALSE);
	if (!theCFs->bParList && ((theCFs->bParList = InitList_UniParMgr(
	  UNIPAR_SET_CFLIST, BANDWIDTH_NUM_PARS, theCFs))) == NULL) {
		NotifyError("%s: Could not initialise bandwidth parList.", funcName);
		return(FALSE);
	}
	pars = theCFs->bParList->pars;
	SetPar_UniParMgr(&pars[BANDWIDTH_PAR_MODE], "B_MODE",
	 "Bandwidth mode ('ERB', 'Custom_ERB', 'Guinea_Pig', 'user' or "
	 "'Nonlinear').",
	  UNIPAR_NAME_SPEC,
	  &theCFs->bandwidthMode.specifier, ModeList_Bandwidth(0),
	  (void *(*)) SetBandwidthSpecifier_CFList);
	SetPar_UniParMgr(&pars[BANDWIDTH_PAR_MIN], "BW_MIN",
	  "Minimum filter bandwidth (Hz).",
	  UNIPAR_REAL,
	  &theCFs->bandwidthMode.bwMin, NULL,
	  (void *(*)) SetBandwidthMin_CFList);
	SetPar_UniParMgr(&pars[BANDWIDTH_PAR_QUALITY], "QUALITY",
	  "Ultimate quality factor of filters.",
	  UNIPAR_REAL,
	  &theCFs->bandwidthMode.quality, NULL,
	  (void *(*)) SetBandwidthQuality_CFList);
	SetPar_UniParMgr(&pars[BANDWIDTH_PAR_BANDWIDTH], "BANDWIDTH",
	  "Filter bandwidths (Hz).",
	  UNIPAR_REAL_ARRAY,
	  &theCFs->bandwidth, &theCFs->numChannels,
	  (void *(*)) SetIndividualBandwidth_CFList);
	return(TRUE);

}

/****************************** CheckInit *************************************/

/*
 * This routine checks whether or not a CFList.has been initialised.
 *
 */
 
BOOLN
CheckInit_CFList(CFListPtr theCFs, const char *callingFunction)
{
	static const char *funcName = "CheckInit_CFList";

	if (theCFs == NULL) {
		NotifyError("%s: Signal not set in %s.", funcName, callingFunction);
		return(FALSE);
	}
	return(TRUE);
	
}

/**************************** CheckPars ***************************************/

/*
 * This routine checks that the necessary parameters for a CFList.have been
 * correctly initialised.
 * It returns TRUE if there are no problems.
 */

BOOLN
CheckPars_CFList(CFListPtr theCFs)
{
	static const char *funcName = "CheckPars_CFList";
	BOOLN	ok;
	int	i;
	
	if (!CheckInit_CFList(theCFs, "CheckPars_CFList"))
		return(FALSE);
	if (theCFs->numChannels <= 0) {
		NotifyError("%s: Number of Channels not correctly set.", funcName);
		return(FALSE);
	}
	if (theCFs->frequency == NULL) {
		NotifyError("%s: No frequencies set.", funcName);
		return(FALSE);
	}
	ok = TRUE;
	for (i = 0; i < theCFs->numChannels; i++)
		if (theCFs->frequency[i] < 0.0) {
			NotifyError("%s: Invalid frequency[%d] = %g Hz.", funcName, i,
			  theCFs->frequency[i]);
			ok = FALSE;
		}
	if (theCFs->bandwidth) {
		for (i = 0; i < theCFs->numChannels; i++)
			if (theCFs->bandwidth[i] < 0.0) {
				NotifyError("%s: Invalid bandwidth[%d] = %g Hz.", funcName,
				 i, theCFs->bandwidth[i]);
				ok = FALSE;
			}
	}
	return(ok);
	
}

/****************************** AllocateFrequencies ***************************/

/*
 * This function allocates the memory for the CFList frequencies.
 * If the 'frequency' field is not NULL, then it will check that the number of
 * channels has not changed, in which case it will do nothing.
 */

BOOLN
AllocateFrequencies_CFList(CFListPtr theCFs)
{
	static const char *funcName = "AllocateFrequencies_CFList(";

	if (!CheckInit_CFList(theCFs, funcName))
		return(FALSE);
	if ((theCFs->numChannels != theCFs->oldNumChannels) && theCFs->frequency) {
		free(theCFs->frequency);
		theCFs->frequency = NULL;
	}
	if (!theCFs->frequency) {
		if ((theCFs->frequency = (double *) calloc(theCFs->numChannels,
		  sizeof(double))) == NULL) {
			NotifyError("%s: Out of memory for frequencies.", funcName);
			return(FALSE);
		}
	}
	return(TRUE);

}

/********************************* GenerateDefault ****************************/

/*
 * This routine generates a default CFList.
 * If the number of channels is one, then it is assumed that 'single' or 'user'
 * mode is being used.
 */
 
CFListPtr
GenerateDefault_CFList(char *modeName, int numberOfCFs, double minCF,
  double maxCF, char *bwModeName, double (* BWidthFunc)(struct BandwidthMode *,
  double))
{
	static const char *funcName = "GenerateDefault_CFList";
	double	*frequencies;
	CFListPtr theCFs;

	if (numberOfCFs < 1) {
		NotifyError("%s: Insufficient CF's (%d).", funcName, numberOfCFs);
		return(NULL);
	}
	if (Identify_NameSpecifier(modeName, CFModeList_CFList(0)) ==
	  CFLIST_SINGLE_MODE) {
		if ((frequencies = (double *) calloc(numberOfCFs, sizeof(double))) ==
		  NULL) {
			NotifyError("%s: Out of memory for frequencies (%d)", funcName,
			  numberOfCFs);
			return(NULL);
		}
		frequencies[0] = minCF;
	} else
		frequencies = NULL;
	if ((theCFs = GenerateList_CFList(modeName, "parameters", numberOfCFs,
	  minCF, maxCF, 0.0, 0.0, frequencies)) == NULL) {
		NotifyError("%s: Could not generate default CF list.", funcName);
		Free_CFList(&theCFs);
		return(NULL);
	}
	theCFs->minCFFlag = FALSE;
	theCFs->maxCFFlag = FALSE;
	if (BWidthFunc)
		theCFs->bandwidthMode.Func = BWidthFunc;
	if (!SetBandwidths_CFList(theCFs, bwModeName, NULL)) {
		NotifyError("%s: Could not set CF bandwidths.",
		  funcName);
		Free_CFList(&theCFs);
		return(NULL);
	}
	return(theCFs);

}

/****************************** GenerateERB ***********************************/

/*
 * This function generates the centre frequency list from the frequency
 * range and ERB density.
 * It assumes that the general parameter checks have been done on the CFList
 * structure previously.
 */

BOOLN
GenerateERB_CFList(CFListPtr theCFs)
{
	static const char *funcName = "GenerateERB_CFList";
	int		i;
	double	theERBRate;

	if (!CheckInit_CFList(theCFs, funcName))
		return(FALSE);
	theCFs->numChannels = (int) ceil((ERBRateFromF_Bandwidth(theCFs->maxCF) - 
	  ERBRateFromF_Bandwidth(theCFs->minCF)) * theCFs->eRBDensity);
	if (theCFs->numChannels < 1) {
		NotifyError("%s: Illegal frequency range %g - %g, or ERB density, %g.",
		  funcName, theCFs->minCF, theCFs->maxCF, theCFs->eRBDensity);
		return(FALSE);
	}
	if (!AllocateFrequencies_CFList(theCFs)) {
		NotifyError("%s: Could not allocate frequencies.", funcName);
		return(FALSE);
	}
	for (i = 0, theERBRate = ERBRateFromF_Bandwidth(theCFs->minCF); i <
	  theCFs->numChannels; i++) {
		theCFs->frequency[i] = FFromERBRate_Bandwidth(theERBRate);
		theERBRate += 1.0 / theCFs->eRBDensity;
	}
	return(TRUE);

}

/****************************** GenerateERBn **********************************/

/*
 * This function generates the centre frequency list from the frequency
 * range and no. of CF's.
 * It assumes that the general parameter checks have been done on the CFList
 * structure previously.
 */

BOOLN
GenerateERBn_CFList(CFListPtr theCFs)
{
	static const char *funcName = "GenerateERBn_CFList";
	int		i;
	double	theERBRate;

	if (!CheckInit_CFList(theCFs, funcName))
		return(FALSE);
	if (!AllocateFrequencies_CFList(theCFs)) {
		NotifyError("%s: Could not allocate frequencies.", funcName);
		return(FALSE);
	}
	theCFs->eRBDensity = theCFs->numChannels / (ERBRateFromF_Bandwidth(
	  theCFs->maxCF) - ERBRateFromF_Bandwidth(theCFs->minCF));
	for (i = 0, theERBRate = ERBRateFromF_Bandwidth(theCFs->minCF); i <
	  theCFs->numChannels; i++) {
		theCFs->frequency[i] = FFromERBRate_Bandwidth(theERBRate);
		theERBRate += 1.0 / theCFs->eRBDensity;
	}
	return(TRUE);

}

/********************************* GenerateUser *******************************/

/*
 * This function generates a user defined list of centre frequencies in a
 * CFList data structure.
 * It assumes that the general parameter checks have been done on the CFList
 * structure previously.
 * No checks are made that the array is the correct length.
 * This routine also sets default values for the CFList structure's 'minCF' and
 * 'maxCF' fields.
 */
 
BOOLN
GenerateUser_CFList(CFListPtr theCFs)
{
	static const char *funcName = "GenerateUser_CFList";
	int		i;
	
	if (!CheckInit_CFList(theCFs, funcName))
		return(FALSE);
	if (theCFs->frequency == NULL) {
		NotifyError("%s: Frequency array not initialised.", funcName);
		return(FALSE);
	}
	for (i = 0; i < theCFs->numChannels; i++) {
		if (theCFs->frequency[i] < 0.0) {
			NotifyError("%s: Illegal frequency[%d] = %g Hz.", funcName, i,
			  theCFs->frequency[i]);
			return(FALSE);
		}
	}
	theCFs->minCF = theCFs->frequency[0];
	theCFs->maxCF = (theCFs->centreFreqMode == CFLIST_SINGLE_MODE)?
	  theCFs->frequency[0] * 10.0: theCFs->frequency[theCFs->numChannels - 1];
	return(TRUE);

}

/****************************** GenerateLog ***********************************/

/*
 * This function generates a logarithmic centre frequency list from the
 * frequency range and the number of filters.
 * It assumes that the general parameter checks have been done on the CFList
 * structure previously.
 */

BOOLN
GenerateLog_CFList(CFListPtr theCFs)
{
	static const char *funcName = "GenerateLog_CFList";
	int		i;
	double	theLogRate, logMinCF;

	if (!CheckInit_CFList(theCFs, funcName))
		return(FALSE);
	logMinCF = log10(theCFs->minCF);
	theLogRate = (log10(theCFs->maxCF) - logMinCF) / (theCFs->numChannels - 1);
	if (!AllocateFrequencies_CFList(theCFs)) {
		NotifyError("%s: Could not allocate frequencies.", funcName);
		return(FALSE);
	}
	for (i = 0; i < theCFs->numChannels; i++)
		theCFs->frequency[i] = pow(10.0, logMinCF + theLogRate * i);
	return(TRUE);

}

/****************************** GenerateFocalLog ******************************/

/*
 * This function generates a logarithmic centre frequency list from the
 * frequency range and the number of filters.
 * It ensures that the specified frequency is the focus of the centre frequency
 * list.
 * It assumes that the general parameter checks have been done on the CFList
 * structure previously.
 */

BOOLN
GenerateFocalLog_CFList(CFListPtr theCFs)
{
	static const char *funcName = "GenerateFocalLog_CFList";
	int		i, focalIndex;
	double	theLogRate, logMinCF, logMaxCF, logFocalCF;

	if (!CheckInit_CFList(theCFs, funcName))
		return(FALSE);
	if ((theCFs->focalCF <= theCFs->minCF) || (theCFs->focalCF >=
	  theCFs->maxCF)) {
		NotifyError("%s: Focal frequency %g is not within the frequency "
		  "range %g - %g.", funcName, theCFs->focalCF, theCFs->minCF,
		  theCFs->maxCF);
		return(FALSE);
	}
	if (!AllocateFrequencies_CFList(theCFs)) {
		NotifyError("%s: Could not allocate frequencies.", funcName);
		return(FALSE);
	}
	logFocalCF = log10(theCFs->focalCF);
	logMinCF = log10(theCFs->minCF);
	logMaxCF = log10(theCFs->maxCF);
	theLogRate = (logMaxCF - logMinCF) / (theCFs->numChannels - 1);
	focalIndex = (int) ((logFocalCF - logMinCF) / (logMaxCF - logMinCF) *
	  theCFs->numChannels);
	for (i = 0; i < focalIndex + 1; i++)
		theCFs->frequency[focalIndex - i] = pow(10.0, logFocalCF - theLogRate *
		  i);
	for (i = focalIndex + 1; i < theCFs->numChannels; i++)
		theCFs->frequency[i] = pow(10.0, logFocalCF + theLogRate * (i -
		  focalIndex));
	return(TRUE);

}

/****************************** GenerateLinear ********************************/

/*
 * This function generates a linear centre frequency list from the
 * frequency range and the number of filters.
 * It assumes that the general parameter checks have been done on the CFList
 * structure previously.
 */

BOOLN
GenerateLinear_CFList(CFListPtr theCFs)
{
	static const char *funcName = "GenerateLinear_CFList";
	int		i;
	double	scale;

	if (!CheckInit_CFList(theCFs, funcName))
		return(FALSE);
	scale = (theCFs->maxCF - theCFs->minCF) / (theCFs->numChannels - 1);
	if (!AllocateFrequencies_CFList(theCFs)) {
		NotifyError("%s: Could not allocate frequencies.", funcName);
		return(FALSE);
	}
	for (i = 0; i < theCFs->numChannels; i++)
		theCFs->frequency[i] = scale * i + theCFs->minCF;
	return(TRUE);

}

/****************************** GetGreenwoodPars ******************************/

/*
 * This function returns a pointer to the Greenwood pointer structure specified
 * by the species type.
 */

GreenwoodParsPtr
GetGreenwoodPars_CFList(int	species)
{
	static const char *funcName = "GetGreenwoodPars_CFList";
	static GreenwoodPars	greenwoodPars[] = {

			{ CFLIST_CAT_MODE,			456.0,	0.8,	2.1 },
			{ CFLIST_CHINCHILLA_MODE,	163.5,	0.85,	2.1 },
			{ CFLIST_GPIG_MODE,			350.0,	0.85,	2.1 },
			{ CFLIST_HUMAN_MODE,		165.4,	0.88,	2.1 },
			{ CFLIST_MACAQUEM_MODE,		360.0,	0.85,	2.1 },
			{ -1,						0.0,	0.0,	0.0 }
		};
	int		i;

	for (i = 0; greenwoodPars[i].species >= 0; i++)
		if (greenwoodPars[i].species == species)
			return (&greenwoodPars[i]);
	NotifyError("%s: Unknown species. (%d).", funcName, species);
	return(NULL);

}

/****************************** GenerateGreenwood *****************************/

/*
 * This function generates a Greenwood species centre frequency list from the
 * global 'greenwoodPars' lookup table.
 * It assumes that the general parameter checks have been done on the CFList
 * structure previously.
 */

#define GREENWOOD_X(cF, PARS) (log10((cF) / ((PARS)->aA) + ((PARS)->k)) / \
		  (PARS)->a)

BOOLN
GenerateGreenwood_CFList(CFListPtr theCFs)
{
	static const char *funcName = "GenerateGreenwood_CFList";
	int		i;
	double	xMin, xMax, scale;
	GreenwoodParsPtr	gPtr;

	if (!CheckInit_CFList(theCFs, funcName))
		return(FALSE);
	if ((gPtr = GetGreenwoodPars_CFList(theCFs->centreFreqMode)) == NULL) {
		NotifyError("%s: Could not set Greenwood parameters.", funcName);
		return(FALSE);
	}
	xMin = GREENWOOD_X(theCFs->minCF, gPtr);
	xMax = GREENWOOD_X(theCFs->maxCF, gPtr);
	scale = (xMax - xMin) / (theCFs->numChannels - 1);
	if (!AllocateFrequencies_CFList(theCFs)) {
		NotifyError("%s: Could not allocate frequencies.", funcName);
		return(FALSE);
	}
	for (i = 0; i < theCFs->numChannels; i++)
		theCFs->frequency[i] = gPtr->aA * (pow(10.0, gPtr->a * (xMin + i *
		  scale)) - gPtr->k);
	return(TRUE);

}

#undef GREENWOOD_X

/****************************** RatifyPars ************************************/

/*
 * This routine ensures that the various parameters have the correct relation-
 * ship to each other, even if they are not enabled, i.e., even when the
 * 'eRBDensity' field is not used it is nice to keep it up to date (for use by
 * the universal parameter lists implentation).
 * It assumes that the frequency list has been correctly initialised.
 */

BOOLN
RatifyPars_CFList(CFListPtr theCFs)
{
	static const char *funcName = "RatifyPars_CFList";

	if (!CheckInit_CFList(theCFs, funcName))
		return(FALSE);
	if ((theCFs->centreFreqMode != CFLIST_ERB_MODE) &&
	  (theCFs->centreFreqMode != CFLIST_ERBN_MODE))
		theCFs->eRBDensity = theCFs->numChannels / (ERBRateFromF_Bandwidth(
		  theCFs->maxCF) - ERBRateFromF_Bandwidth(theCFs->minCF));
	if (theCFs->centreFreqMode != CFLIST_CF_FOCAL_FREQ)
		theCFs->focalCF = (theCFs->frequency[0] + theCFs->frequency[
		  theCFs->numChannels - 1]) / 2.0;
	return(TRUE);

}

/****************************** SetGeneratedPars ******************************/

/*
 * This function takes all the required parameters and sets the generated
 * parameters of existing CFList structure.
 * It returns FALSE if it fails in any way.
 * The 'oldNumChannels' is remembered so that if there is no change to the
 * number of channels, then the old frequency array can be used.
 */

BOOLN
SetGeneratedPars_CFList(CFListPtr theCFs)
{
	static const char *funcName = "SetGeneratedPars_CFList";
	BOOLN	ok = TRUE;

	if (!CheckInit_CFList(theCFs, funcName))
		return(FALSE);
	switch (theCFs->centreFreqMode) {
	case CFLIST_ERB_MODE:
	case CFLIST_ERBN_MODE:
	case CFLIST_LOG_MODE:
	case CFLIST_FOCAL_LOG_MODE:
	case CFLIST_LINEAR_MODE:
	case CFLIST_CAT_MODE:
	case CFLIST_CHINCHILLA_MODE:
	case CFLIST_GPIG_MODE:
	case CFLIST_HUMAN_MODE:
	case CFLIST_MACAQUEM_MODE:
		if (theCFs->maxCF <= theCFs->minCF) {
			NotifyError("%s: Illegal frequency range %g - %g.", funcName,
			  theCFs->minCF, theCFs->maxCF);
			return(FALSE);
		}
	default:
		;
	};
	if ((theCFs->centreFreqMode == CFLIST_SINGLE_MODE) && (theCFs->numChannels <
	  1)) {
		NotifyError("%s: Illegal no. of centre frequencies (%d).", funcName,
		  theCFs->numChannels);
		return(FALSE);
	}
	if (((theCFs->centreFreqMode == CFLIST_ERBN_MODE) ||
	  (theCFs->centreFreqMode == CFLIST_LOG_MODE) || (theCFs->centreFreqMode ==
	  CFLIST_ERBN_MODE) || (theCFs->centreFreqMode == CFLIST_LOG_MODE) ||
	  (theCFs->centreFreqMode == CFLIST_LINEAR_MODE)) && (theCFs->numChannels <
	  2)) {
		NotifyError("%s: Illegal no. of centre frequencies (%d).", funcName,
		  theCFs->numChannels);
		return(FALSE);
	}
	switch (theCFs->centreFreqMode) {
	case CFLIST_SINGLE_MODE:
		ok = GenerateUser_CFList(theCFs);
		break;
	case CFLIST_USER_MODE:
		ok = GenerateUser_CFList(theCFs);
		break;
	case CFLIST_ERB_MODE:
		ok = GenerateERB_CFList(theCFs);
		break;
	case CFLIST_ERBN_MODE:
		ok = GenerateERBn_CFList(theCFs);
		break;
	case CFLIST_LOG_MODE:
		ok = GenerateLog_CFList(theCFs);
		break;
	case CFLIST_LINEAR_MODE:
		ok = GenerateLinear_CFList(theCFs);
		break;
	case CFLIST_FOCAL_LOG_MODE:
		ok = GenerateFocalLog_CFList(theCFs);
		break;
	case CFLIST_CAT_MODE:
	case CFLIST_CHINCHILLA_MODE:
	case CFLIST_GPIG_MODE:
	case CFLIST_HUMAN_MODE:
	case CFLIST_MACAQUEM_MODE:
		ok = GenerateGreenwood_CFList(theCFs);
		break;
	default:
		;
	} /* Switch */
	if (!ok) {
		NotifyError("%s: Could not generate CFList for '%s' frequency "
		  "mode.", funcName, CFModeList_CFList(theCFs->centreFreqMode)->name);
		return(FALSE);
	}
	theCFs->oldNumChannels = theCFs->numChannels;
	theCFs->updateFlag = TRUE;
	RatifyPars_CFList(theCFs);
	if (!SetCFUniParList_CFList(theCFs)) {
		NotifyError("%s: Could not initialise parameter list.", funcName);
		return(FALSE);
	}
	SetCFUniParListMode_CFList(theCFs);
	return(TRUE);

}

/****************************** GenerateList **********************************/

/*
 * This function takes all the required parameters and returns a pointer to a
 * CFList structure.
 * It returns with the address of the CFList, or NULL if it fails.
 * Even though when the 'eRBDensity' field is not used it is nice to keep it up
 * to date (for use by the universal parameter lists implentation).
 * The 'oldNumChannels' is remembered so that if a change to 'iser' mode is
 * made, then the old frequency array can be used.
 */

CFListPtr
GenerateList_CFList(char *modeName, char *diagModeName, int numberOfCFs,
  double minCF, double maxCF, double focalCF, double eRBDensity,
  double *frequencies)
{
	static const char *funcName = "GenerateList_CFList";
	BOOLN	ok = TRUE;
	CFListPtr	theCFs = NULL;

	if ((theCFs = Init_CFList(funcName)) == NULL) {
		NotifyError("%s: Out of memory.", funcName);
		return(NULL);
	}
	if (!SetCFMode_CFList(theCFs, modeName)) {
		Free_CFList(&theCFs);
		return(NULL);
	}
	if (!SetDiagnosticMode_CFList(theCFs, diagModeName))
		ok = FALSE;

	switch (theCFs->centreFreqMode ) {
	case CFLIST_USER_MODE:
		if (!SetNumChannels_CFList(theCFs, numberOfCFs))
			ok = FALSE;
		break;
	case CFLIST_ERB_MODE:
		if (!SetMinCF_CFList(theCFs, minCF) || !SetMaxCF_CFList(theCFs, maxCF))
			ok = FALSE;
		break;
	case CFLIST_ERBN_MODE:
	case CFLIST_LOG_MODE:
	case CFLIST_LINEAR_MODE:
	case CFLIST_FOCAL_LOG_MODE:
	case CFLIST_CAT_MODE:
	case CFLIST_CHINCHILLA_MODE:
	case CFLIST_GPIG_MODE:
	case CFLIST_HUMAN_MODE:
	case CFLIST_MACAQUEM_MODE:
		if (!SetNumChannels_CFList(theCFs, numberOfCFs))
			ok = FALSE;
		if (!SetMinCF_CFList(theCFs, minCF) || !SetMaxCF_CFList(theCFs, maxCF))
			ok = FALSE;
		break;
	default:
		;
	}

	switch (theCFs->centreFreqMode) {
	case CFLIST_FOCAL_LOG_MODE:
		if (!SetFocalCF_CFList(theCFs, focalCF))
			ok = FALSE;
		break;
	case CFLIST_ERB_MODE:
		if (!SetERBDensity_CFList(theCFs, eRBDensity))
			ok = FALSE;
		break;
	case CFLIST_USER_MODE:
	case CFLIST_SINGLE_MODE:
		if (!frequencies)
			ok = FALSE;
	default:
		theCFs->frequency = frequencies;
	}

	if (!ok) {
		NotifyError("%s: Could not set the CF list parameters.", funcName);
		Free_CFList(&theCFs);
		return(NULL);
	}
	if (!SetGeneratedPars_CFList(theCFs)) {
		NotifyError("%s: Could not set CF list generated parameters '%s'"
		  "frequency mode.", funcName, modeName);
		Free_CFList(&theCFs);
		return(NULL);
	}
	return(theCFs);

}

/****************************** ReadPars **************************************/

/*
 * This routine reads the CFList parameters from a file stream.
 * It returns with the address of the CFList, or NULL if it fails.
 */

CFListPtr
ReadPars_CFList(FILE *fp)
{
	static const char *funcName = "ReadPars_CFList";
	BOOLN	ok = TRUE;
	char	modeName[MAXLINE], diagModeName[MAXLINE];
	int		i, mode, numberOfCFs;
	double	*frequencies = NULL, focalCFFrequency, lowestCFFrequency;
	double	highestCFFrequency, eRBDensity;
	CFListPtr	theCFs = NULL;
	
	if (!GetPars_ParFile(fp, "%s", diagModeName)) {
		NotifyError("%s: Could not read diagnostic mode.", funcName);
		return(NULL);
	}
	if (!GetPars_ParFile(fp, "%s", modeName)) {
		NotifyError("%s: Could not read centre frequency mode.", funcName);
		return(NULL);
	}
	switch (mode = Identify_NameSpecifier(modeName, CFModeList_CFList(0))) {
	case	CFLIST_SINGLE_MODE:
	case	CFLIST_USER_MODE:
		if (mode == CFLIST_SINGLE_MODE)
			numberOfCFs = 1;
		else {
			if (!GetPars_ParFile(fp, "%d", &numberOfCFs))
				ok = FALSE;
		}
		if (numberOfCFs < 1) {
			NotifyError("%s: Insufficient CF's (%d).", funcName, numberOfCFs);
			ok = FALSE;
		} else {
			if ((frequencies = (double *) calloc(numberOfCFs,
			  sizeof(double))) == NULL) {
				NotifyError("%s: Out of memory for frequencies (%d)", funcName,
				  numberOfCFs);
				ok = FALSE;
			}
			for (i = 0; (i < numberOfCFs) && ok; i++)
				if (!GetPars_ParFile(fp, "%lf", &frequencies[i]))
					ok = FALSE;
		}
		break;
	case	CFLIST_ERB_MODE:
		if (!GetPars_ParFile(fp, "%lf", &lowestCFFrequency))
			ok = FALSE;
		if (!GetPars_ParFile(fp, "%lf", &highestCFFrequency))
			ok = FALSE;
		if (!GetPars_ParFile(fp, "%lf", &eRBDensity))
			ok = FALSE;
		break;
	case	CFLIST_ERBN_MODE:
	case	CFLIST_LOG_MODE:
	case	CFLIST_LINEAR_MODE:
	case	CFLIST_CAT_MODE:
	case	CFLIST_CHINCHILLA_MODE:
	case	CFLIST_GPIG_MODE:
	case	CFLIST_HUMAN_MODE:
	case	CFLIST_MACAQUEM_MODE:
		if (!GetPars_ParFile(fp, "%lf", &lowestCFFrequency))
			ok = FALSE;
		if (!GetPars_ParFile(fp, "%lf", &highestCFFrequency))
			ok = FALSE;
		if (!GetPars_ParFile(fp, "%d", &numberOfCFs))
			ok = FALSE;
		break;
	case	CFLIST_FOCAL_LOG_MODE:
		if (!GetPars_ParFile(fp, "%lf", &focalCFFrequency))
			ok = FALSE;
		if (!GetPars_ParFile(fp, "%lf", &lowestCFFrequency))
			ok = FALSE;
		if (!GetPars_ParFile(fp, "%lf", &highestCFFrequency))
			ok = FALSE;
		if (!GetPars_ParFile(fp, "%d", &numberOfCFs))
			ok = FALSE;
		break;
	default	:
		NotifyError("%s: Unknown frequency mode (%s).", funcName, modeName);
		ok = FALSE;
		break;
	} /* Switch */
	if (!ok) {
		NotifyError("%s: Failed to read centre frequency list parameters.",
		  funcName);
		return(NULL);
	}
	if ((theCFs = GenerateList_CFList(modeName, diagModeName, numberOfCFs,
	  lowestCFFrequency, highestCFFrequency, focalCFFrequency, eRBDensity,
	  frequencies)) == NULL) {
		NotifyError("%s: Could not generate CF list.", funcName);
		return(NULL);
	}
	return(theCFs);

}

/********************************* SetBandwidthArray **************************/

/*
 * This void sets the array of bandwidths for a CFList structure.
 * It expects all parameters to be correctly set, though checks are made upon
 * 'USER' bandwidths, if they already exist, otherwise they are created.
 */

BOOLN
SetBandwidthArray_CFList(CFListPtr theCFs, double *theBandwidths)
{
	static const char *funcName = "SetBandwidthArray_CFList";
	int		i;
	
	switch (theCFs->bandwidthMode.specifier) {
	case BANDWIDTH_NULL:
		break;
	case BANDWIDTH_USER:
		if (theBandwidths == NULL) {
			NotifyError("%s: Bandwidth array not initialised.", funcName);
			return(FALSE);
		}
		for (i = 0; i < theCFs->numChannels; i++)
			if (theBandwidths[i] <= 0.0) {
				NotifyError("%s: Illegal bandwidth[%d] = %g Hz.", funcName,
				  i, theBandwidths[i]);
				return(FALSE);
			}
		theCFs->bandwidth = theBandwidths;
		break;
	case BANDWIDTH_INTERNAL_DYNAMIC:
	case BANDWIDTH_DISABLED:
		if (theCFs->bandwidth) {
			free(theCFs->bandwidth);
			theCFs->bandwidth = NULL;
		}
		break;
	default:
		if (theCFs->bandwidth)
			free(theCFs->bandwidth);
		if ((theCFs->bandwidth = (double *) calloc(theCFs->numChannels,
		  sizeof(double))) == NULL) {
			NotifyError("%s: Out of memory for bandwidths (%d).", funcName,
			  theCFs->numChannels);
			return(FALSE);
		}
		for (i = 0; i < theCFs->numChannels; i++)
			theCFs->bandwidth[i] = (* theCFs->bandwidthMode.Func)(
			  &theCFs->bandwidthMode, theCFs->frequency[i]);
	} /* switch */
	return(TRUE);

}

/********************************* SetBandwidths ******************************/

/*
 * This function sets the  bandwidths for a CFList data structure, used
 * by the basilar membrane filter banks.
 * It returns TRUE if the operation is successful.
 * If the 'modeName' is set to NULL, then the mode will not be reset.
 */
 
BOOLN
SetBandwidths_CFList(CFListPtr theCFs, char *modeName, double *theBandwidths)
{
	static const char *funcName = "SetBandwidths_CFList";
	
	if (!CheckPars_CFList(theCFs)) {
		NotifyError("%s: Centre frequency list  parameters not correctly "
		  "set, cannot add bandwidths.", funcName);
		return(FALSE);
	}
	if (modeName && !SetMode_Bandwidth(&theCFs->bandwidthMode, modeName)) {
		NotifyError("%s: Could not set bandwidth mode.", funcName);
		return(FALSE);
	}
	if (!SetBandwidthArray_CFList(theCFs, theBandwidths)) {
		NotifyError("%s: Could not set bandwidths.", funcName);
		return(FALSE);
	}
	theCFs->updateFlag = TRUE;
	if (!SetBandwidthUniParList_CFList(theCFs)) {
		NotifyError("%s: Could not initialise parameter list for bandwidths.",
		  funcName);
		return(FALSE);
	}
	SetBandwidthUniParListMode_CFList(theCFs);
	return(TRUE);

}

/****************************** ReadBandwidths ********************************/

/*
 * This routine sets the bandwidths of a CFList, using a bandwidth mode
 * read from file.
 * The use of pointers in assigning the bandwidths to the array was employed
 * because of some bizarre quirk of the Symantec C++ compiler.
 * It returns TRUE if successful.
 */

BOOLN
ReadBandwidths_CFList(FILE *fp, CFListPtr theCFs)
{
	static const char *funcName = "ReadBandwidths_CFList";
	char	modeName[MAXLINE];
	int		i;
		
	if (!CheckPars_CFList(theCFs)) {
		NotifyError("%s: Centre frequency list parameters not correctly "
		  "set, cannot add bandwidths.", funcName);
		return(FALSE);
	}
	if (!GetPars_ParFile(fp, "%s", modeName)) {
		NotifyError("%s: Could not read bandwidth mode.", funcName);
		return(FALSE);
	}
	if (!SetMode_Bandwidth(&theCFs->bandwidthMode, modeName)) {
		NotifyError("%s: Could not set bandwidth mode.", funcName);
		return(FALSE);
	}
	switch (theCFs->bandwidthMode.specifier) {
	case BANDWIDTH_USER:
		if ((theCFs->bandwidth = (double *) calloc(theCFs->numChannels,
		  sizeof(double))) == NULL) {
			NotifyError("%s: Out of memory for bandwidths.", funcName);
			return(FALSE);
		}
		for (i = 0; i < theCFs->numChannels; i++)
			if (!GetPars_ParFile(fp, "%lf", &theCFs->bandwidth[i])) {
				NotifyError("%s: Failed to read %d user bandwidths.", funcName,
				  theCFs->numChannels);
				return(FALSE);
			}
	case BANDWIDTH_DISABLED:
	case BANDWIDTH_INTERNAL_DYNAMIC:
	case BANDWIDTH_INTERNAL_STATIC:
		break;
	case BANDWIDTH_CUSTOM_ERB:
		if (!GetPars_ParFile(fp, "%lf", &theCFs->bandwidthMode.bwMin)) {
			NotifyError("%s: Could not read 'bwMin' for Custom ERB mode.",
			  funcName);
			return(FALSE);
		}
		if (!GetPars_ParFile(fp, "%lf", &theCFs->bandwidthMode.quality)) {
			NotifyError("%s: Could not read 'quality' for Custom ERB mode.",
			  funcName);
			return(FALSE);
		}
	default:
		;
	} /* switch */
	if (!SetBandwidths_CFList(theCFs, NULL, theCFs->bandwidth)) {
		NotifyError("%s: Could initialise bandwidth parameters.",
		  funcName);
		return(FALSE);
	}
	return(TRUE);

}

/****************************** GetCF *****************************************/

/*
 * This routine returns the respective centre frequency value for a CFList
 * structure.
 *
 */

double
GetCF_CFList(CFListPtr theCFs, int channel)
{
	static const char *funcName = "GetCF_CFList";

	if (!CheckPars_CFList(theCFs)) {
		NotifyError("%s: Centre frequency list parameters not correctly "
		  "set.  Zero returned.", funcName);
		return(0.0);
	}
	if (channel < 0 || channel >= theCFs->numChannels) {
		NotifyError("%s: Channel number must be in the range 0 to %d.  Zero "
		  "returned.", funcName, theCFs->numChannels - 1);
		return(0.0);
	}
	return(theCFs->frequency[channel]);

}

/****************************** ResetCF ***************************************/

/*
 * This routine resets the respective centre frequency value for a CFList
 * structure.
 * If the bandwidth mode is set, and it is not a user bandwidth, then the
 * bandwidth is automatically reset.
 *
 */

BOOLN
ResetCF_CFList(CFListPtr theCFs, int channel, double theFrequency)
{
	static const char *funcName = "ResetCF_CFList";

	if (theFrequency < 0.0) {
		NotifyError("%s: Illegal frequency value = %g.", funcName,
		  theFrequency);
		return(FALSE);
	}
	if (!CheckPars_CFList(theCFs)) {
		NotifyError("%s: Centre frequency list parameters not correctly set.",
		  funcName);
		return(FALSE);
	}
	if (channel < 0 || channel >= theCFs->numChannels) {
		NotifyError("%s: Channel number must be in the range 0 to %d.",
		  funcName, theCFs->numChannels - 1);
		return(FALSE);
	}
	if (theFrequency < 0.0) {
		NotifyError("%s: Illegal frequency value = %g.", funcName,
		  theFrequency);
		return(FALSE);
	}
	theCFs->frequency[channel] = theFrequency;
	if ((theCFs->bandwidthMode.specifier != BANDWIDTH_USER) &&
	  (theCFs->bandwidthMode.specifier != BANDWIDTH_INTERNAL_DYNAMIC) &&
	  (theCFs->bandwidthMode.specifier != BANDWIDTH_DISABLED))
		theCFs->bandwidth[channel] = (* theCFs->bandwidthMode.Func)(&theCFs->
		  bandwidthMode, theFrequency);
	theCFs->updateFlag = TRUE;
	return(TRUE);

}

/****************************** ResetBandwidth ********************************/

/*
 * This routine resets the respective bandwidth value for a CFList
 * structure.
 *
 */

BOOLN
ResetBandwidth_CFList(CFListPtr theCFs, int channel, double theBandwidth)
{
	static const char *funcName = "ResetBandwidth_CFList";
	if (theBandwidth < 0.0) {
		NotifyError("%s: Illegal bandwidth value = %g.", funcName,
		  theBandwidth);
		return(FALSE);
	}
	if (!CheckPars_CFList(theCFs)) {
		NotifyError("%s: Centre frequency list parameters  not correctly set",
		  funcName);
		return(FALSE);
	}
	if (theCFs->bandwidthMode.specifier != BANDWIDTH_USER) {
		NotifyError("%s: Individual bandwidths can only be set in USER "
		  "bandwidth mode.", funcName);
		return(FALSE);
	}
	if (channel < 0 || channel >= theCFs->numChannels) {
		NotifyError("%s: Channel number must be in the range 0 to %d.",
		  funcName, theCFs->numChannels - 1);
		return(FALSE);
	}
	theCFs->bandwidth[channel] = theBandwidth;
	theCFs->updateFlag = TRUE;
	return(TRUE);

}

/****************************** GetBandwidth **********************************/

/*
 * This routine returns the respective bandwidth value for a CFList
 * structure.
 *
 */

double
GetBandwidth_CFList(CFListPtr theCFs, int channel)
{
	static const char *funcName = "GetBandwidth_CFList";

	if (!CheckPars_CFList(theCFs)) {
		NotifyError("%s: Centre frequency list parameters not correctly "
		  "set.  Zero returned.", funcName);
		return(0.0);
	}
	if ((theCFs->bandwidthMode.specifier == BANDWIDTH_INTERNAL_DYNAMIC) ||
	  (theCFs->bandwidthMode.specifier == BANDWIDTH_DISABLED)) {
		NotifyError("%s: Internal/disabled bandwidths cannot be read.  Zero "
		  "returned", funcName);
		return(0.0);
	}
	if (channel < 0 || channel >= theCFs->numChannels) {
		NotifyError("%s: Channel number must be in the range 0 to %d, zero "
		  "returned.", funcName, theCFs->numChannels - 1);
		return(0.0);
	}
	return(theCFs->bandwidth[channel]);

}

/****************************** PrintList *************************************/

/*
 * This routine prints a list of the parameters of the CFList structure.
 */

void
PrintList_CFList(CFListPtr theCFs)
{
	static const char *funcName = "PrintList_CFList(";
	int		i;
	
	if (!CheckPars_CFList(theCFs)) {
		NotifyError("%s: Centre frequency list parameters not correctly set.",
		  funcName);
		return;
	}
	DPrint("\t\t%10s\t%10s\t%10s\n", "Filter No.", "Frequency", "Bandwidths");
	DPrint("\t\t%10s\t%10s\t%10s\n", "          ", "   (Hz)  ", "   (Hz)  ");
	for (i = 0; i < theCFs->numChannels; i++) {
		DPrint("\t\t%10d\t%10g", i, theCFs->frequency[i]);
		if (theCFs->bandwidthMode.specifier == BANDWIDTH_INTERNAL_DYNAMIC)
			DPrint("\t%10s\n", "<internal>");
		else if (theCFs->bandwidthMode.specifier == BANDWIDTH_DISABLED)
			DPrint("\t%10s\n", "<disabled>");
		else if (theCFs->bandwidth) {
				DPrint("\t%10g\n", theCFs->bandwidth[i]);
		} else
			DPrint("\t%10s\n", "<unset>");
	}
	DPrint("\t\tCF Spacing mode: %s, Bandwidth mode: %s\n",
	  CFModeList_CFList(theCFs->centreFreqMode)->name, ModeList_Bandwidth(
	    theCFs->bandwidthMode.specifier)->name);

}

/****************************** PrintPars *************************************/

/*
 * This routine prints the parameters associated with a CFList data structure.
 */

void
PrintPars_CFList(CFListPtr theCFs)
{
	static const char *funcName = "PrintPars_CFList";
	
	if (!CheckPars_CFList(theCFs)) {
		NotifyError("%s: Centre frequency list parameters not correctly set.",
		  funcName);
		return;
	}
	DPrint("\tCentre Frequency structure parameters:-\n");
	DPrint("\t\tDiagnostic mode: %s:\n", cFListDiagModeList[
	  theCFs->diagnosticMode].name);
	switch (theCFs->diagnosticMode) {
	case CFLIST_PARAMETERS_DIAG_MODE:
		if ((theCFs->centreFreqMode == CFLIST_USER_MODE) ||
		  (theCFs->centreFreqMode == CFLIST_SINGLE_MODE) ||
		  (theCFs->bandwidthMode.specifier == BANDWIDTH_USER)) {
			PrintList_CFList(theCFs);
			return;
		}
		DPrint("\t\tCF Spacing mode: %s\n",
		  CFModeList_CFList(theCFs->centreFreqMode)->name);
		switch (theCFs->centreFreqMode) {
		case CFLIST_ERB_MODE:
			DPrint("\t\tMinimum/maximum frequency: %g / %g Hz,\n",
			  theCFs->minCF, theCFs->maxCF);
			DPrint("\t\tERB density: %g filters/critical band.\n",
			  theCFs->eRBDensity);
			break;
		case CFLIST_ERBN_MODE:
		case CFLIST_LOG_MODE:
		case CFLIST_LINEAR_MODE:
		case CFLIST_CAT_MODE:
		case CFLIST_CHINCHILLA_MODE:
		case CFLIST_GPIG_MODE:
		case CFLIST_HUMAN_MODE:
		case CFLIST_MACAQUEM_MODE:
			DPrint("\t\tMinimum/maximum frequency: %g / %g Hz,\n",
			  theCFs->minCF, theCFs->maxCF);
			DPrint("\t\tNumber of CF's: %d.\n", theCFs->numChannels);
			break;
		case CFLIST_FOCAL_LOG_MODE:
			DPrint("\t\tMinimum/maximum frequency: %g / %g Hz,\n",
			  theCFs->minCF, theCFs->maxCF);
			DPrint("\t\tFocal CF: %g (Hz),\tNumber of CF's: %d.\n",
			  theCFs->focalCF, theCFs->numChannels);
			break;
		default:
			;
		} /* Switch */
		DPrint("\t\tBandwidth mode: %s\n", ModeList_Bandwidth(
		  theCFs->bandwidthMode.specifier)->name);
		switch (theCFs->bandwidthMode.specifier) {
		case BANDWIDTH_CUSTOM_ERB:
			DPrint("\t\tMinimum bandwidth: %g (Hz),",
			  theCFs->bandwidthMode.bwMin);
			DPrint("\tUltimate quality factor: %g.\n",
			  theCFs->bandwidthMode.quality);
			break;
		default:
			;
		}
		break;
	default:
		PrintList_CFList(theCFs);
	}

}
