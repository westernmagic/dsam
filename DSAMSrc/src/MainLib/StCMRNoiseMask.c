/**********************
 *
 * File:		StCMRNoiseMask.c
 * Purpose:		
 * Comments:	Written using ModuleProducer version 1.6.1 (Sep 24 2008).
 * Author:		
 * Created:		30 Sep 2008
 * Updated:	
 * Copyright:	(c) 2008, 
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
#include "GeNSpecLists.h"
#include "GeUniParMgr.h"
#include "GeModuleMgr.h"
#include "UtString.h"
#include "UtFFT.h"
#include "UtRandom.h"
#include "FiParFile.h"
#include "StCMRNoiseMask.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

CMRNoiseMPtr	cMRNoiseMPtr = NULL;

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
Free_CMR_NoiseMasker(void)
{
	/* static const WChar	*funcName = wxT("Free_CMR_NoiseMasker"); */

	if (cMRNoiseMPtr == NULL)
		return(FALSE);
	if (cMRNoiseMPtr->parList)
		FreeList_UniParMgr(&cMRNoiseMPtr->parList);
	if (cMRNoiseMPtr->parSpec == GLOBAL) {
		free(cMRNoiseMPtr);
		cMRNoiseMPtr = NULL;
	}
	return(TRUE);

}

/****************************** InitSpacingTypeList ***************************/

/*
 * This function initialises the 'p->spacingType' list array
 */

BOOLN
InitSpacingTypeList_CMR_NoiseMasker(void)
{
	static NameSpecifier	modeList[] = {

			{ wxT("LINEAR"),	CMR_NOISEMASKER_SPACINGTYPE_LINEAR },
			{ wxT("OCTAVE"),	CMR_NOISEMASKER_SPACINGTYPE_OCTAVE },
			{ 0, 0 },
		};
	cMRNoiseMPtr->spacingTypeList = modeList;
	return(TRUE);

}

/****************************** InitConditionList *****************************/

/*
 * This function initialises the 'p->condition' list array
 */

BOOLN
InitConditionList_CMR_NoiseMasker(void)
{
	static NameSpecifier	modeList[] = {

			{ wxT("CD"),	CMR_NOISEMASKER_CONDITION_CD },
			{ wxT("CM"),	CMR_NOISEMASKER_CONDITION_CM },
			{ wxT("DV"),	CMR_NOISEMASKER_CONDITION_DV },
			{ wxT("RF"),	CMR_NOISEMASKER_CONDITION_RF },
			{ wxT("SO"),	CMR_NOISEMASKER_CONDITION_SO },
			{ 0, 0 },
		};
	cMRNoiseMPtr->conditionList = modeList;
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
Init_CMR_NoiseMasker(ParameterSpecifier parSpec)
{
	static const WChar	*funcName = wxT("Init_CMR_NoiseMasker");

	if (parSpec == GLOBAL) {
		if (cMRNoiseMPtr != NULL)
			Free_CMR_NoiseMasker();
		if ((cMRNoiseMPtr = (CMRNoiseMPtr) malloc(sizeof(CMRNoiseM))) == NULL) {
			NotifyError(wxT("%s: Out of memory for 'global' pointer"), funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (cMRNoiseMPtr == NULL) {
			NotifyError(wxT("%s:  'local' pointer not set."), funcName);
			return(FALSE);
		}
	}
	cMRNoiseMPtr->parSpec = parSpec;
	cMRNoiseMPtr->updateProcessVariablesFlag = TRUE;
	cMRNoiseMPtr->binauralMode = GENERAL_BOOLEAN_OFF;
	cMRNoiseMPtr->spacingType = CMR_NOISEMASKER_SPACINGTYPE_LINEAR;
	cMRNoiseMPtr->condition = CMR_NOISEMASKER_CONDITION_RF;
	cMRNoiseMPtr->onFreqEar = GENERAL_EAR_LEFT;
	cMRNoiseMPtr->sigEar = GENERAL_EAR_LEFT;
	DSAM_strcpy(cMRNoiseMPtr->flankEar, wxT("L"));
	cMRNoiseMPtr->nlow = 3;
	cMRNoiseMPtr->nupp = 3;
	cMRNoiseMPtr->nGapLow = 1;
	cMRNoiseMPtr->nGapUpp = 1;
	cMRNoiseMPtr->ranSeed = 0;
	cMRNoiseMPtr->lowFBLevel = 0.0;
	cMRNoiseMPtr->uppFBLevel = 0.0;
	cMRNoiseMPtr->oFMLevel = 60.0;
	cMRNoiseMPtr->spacing = 100.0;
	cMRNoiseMPtr->maskerModFreq = 20.0;
	cMRNoiseMPtr->bandwidth = 20.0;
	cMRNoiseMPtr->sigLevel = 0.0;
	cMRNoiseMPtr->sigFreq = 700.0;
	cMRNoiseMPtr->gateTime = 10.0e-3;
	cMRNoiseMPtr->duration = 0.5;
	cMRNoiseMPtr->dt = DEFAULT_DT;

	InitSpacingTypeList_CMR_NoiseMasker();
	InitConditionList_CMR_NoiseMasker();
	if (!SetUniParList_CMR_NoiseMasker()) {
		NotifyError(wxT("%s: Could not initialise parameter list."), funcName);
		Free_CMR_NoiseMasker();
		return(FALSE);
	}
	cMRNoiseMPtr->fTInv = NULL;
	return(TRUE);

}

/****************************** SetUniParList *********************************/

/*
 * This function initialises and sets the module's universal parameter
 * list. This list provides universal access to the module's
 * parameters.  It expects to be called from the 'Init_' routine.
 */

BOOLN
SetUniParList_CMR_NoiseMasker(void)
{
	static const WChar	*funcName = wxT("SetUniParList_CMR_NoiseMasker");
	UniParPtr	pars;

	if ((cMRNoiseMPtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  CMR_NOISEMASKER_NUM_PARS, NULL)) == NULL) {
		NotifyError(wxT("%s: Could not initialise parList."), funcName);
		return(FALSE);
	}
	pars = cMRNoiseMPtr->parList->pars;
	SetPar_UniParMgr(&pars[CMR_NOISEMASKER_BINAURALMODE], wxT("BINAURAL_MODE"),
	  wxT("Binaural mode ('on' or 'off ')."),
	  UNIPAR_BOOL,
	  &cMRNoiseMPtr->binauralMode, NULL,
	  (void * (*)) SetBinauralMode_CMR_NoiseMasker);
	SetPar_UniParMgr(&pars[CMR_NOISEMASKER_SPACINGTYPE], wxT("SPACING_TYPE"),
	  wxT("Spacing type ('linear' or 'octave')."),
	  UNIPAR_NAME_SPEC,
	  &cMRNoiseMPtr->spacingType, cMRNoiseMPtr->spacingTypeList,
	  (void * (*)) SetSpacingType_CMR_NoiseMasker);
	SetPar_UniParMgr(&pars[CMR_NOISEMASKER_CONDITION], wxT("CONDITION"),
	  wxT("Stimulus p->condition mode: ('cd', 'cm', 'dv', 'rf' or 'so'."),
	  UNIPAR_NAME_SPEC,
	  &cMRNoiseMPtr->condition, cMRNoiseMPtr->conditionList,
	  (void * (*)) SetCondition_CMR_NoiseMasker);
	SetPar_UniParMgr(&pars[CMR_NOISEMASKER_ONFREQEAR], wxT("ON_FREQ_EAR"),
	  wxT("On-frequency ear ('left' or 'right')."),
	  UNIPAR_NAME_SPEC,
	  &cMRNoiseMPtr->onFreqEar, EarModeList_NSpecLists(0),
	  (void * (*)) SetOnFreqEar_CMR_NoiseMasker);
	SetPar_UniParMgr(&pars[CMR_NOISEMASKER_SIGEAR], wxT("SIGNAL_EAR"),
	  wxT("Signal presentation ear ('left' or 'right')."),
	  UNIPAR_NAME_SPEC,
	  &cMRNoiseMPtr->sigEar, EarModeList_NSpecLists(0),
	  (void * (*)) SetSigEar_CMR_NoiseMasker);
	SetPar_UniParMgr(&pars[CMR_NOISEMASKER_FLANKEAR], wxT("FLANK_EAR"),
	  wxT("Flanker ear string ??"),
	  UNIPAR_STRING,
	  &cMRNoiseMPtr->flankEar, NULL,
	  (void * (*)) SetFlankEar_CMR_NoiseMasker);
	SetPar_UniParMgr(&pars[CMR_NOISEMASKER_NLOW], wxT("NUM_LOW_BANDS"),
	  wxT("No. lower frequency bands."),
	  UNIPAR_INT,
	  &cMRNoiseMPtr->nlow, NULL,
	  (void * (*)) SetNlow_CMR_NoiseMasker);
	SetPar_UniParMgr(&pars[CMR_NOISEMASKER_NUPP], wxT("NUM_UPPER_BANDS"),
	  wxT("No. upper frequency bands."),
	  UNIPAR_INT,
	  &cMRNoiseMPtr->nupp, NULL,
	  (void * (*)) SetNupp_CMR_NoiseMasker);
	SetPar_UniParMgr(&pars[CMR_NOISEMASKER_NGAPLOW], wxT("NUM_LOWER_GAP"),
	  wxT("No. of lower Gaps."),
	  UNIPAR_INT,
	  &cMRNoiseMPtr->nGapLow, NULL,
	  (void * (*)) SetNGapLow_CMR_NoiseMasker);
	SetPar_UniParMgr(&pars[CMR_NOISEMASKER_NGAPUPP], wxT("NUM_UPPER_GAP"),
	  wxT("No. of upper Gaps."),
	  UNIPAR_INT,
	  &cMRNoiseMPtr->nGapUpp, NULL,
	  (void * (*)) SetNGapUpp_CMR_NoiseMasker);
	SetPar_UniParMgr(&pars[CMR_NOISEMASKER_RANSEED], wxT("RAN_SEED"),
	  wxT("Random Number Seed."),
	  UNIPAR_LONG,
	  &cMRNoiseMPtr->ranSeed, NULL,
	  (void * (*)) SetRanSeed_CMR_NoiseMasker);
	SetPar_UniParMgr(&pars[CMR_NOISEMASKER_LOWFBLEVEL], wxT("LOWER_FB_LEVEL"),
	  wxT("Lower FB level (dB re OFM)."),
	  UNIPAR_REAL,
	  &cMRNoiseMPtr->lowFBLevel, NULL,
	  (void * (*)) SetLowFBLevel_CMR_NoiseMasker);
	SetPar_UniParMgr(&pars[CMR_NOISEMASKER_UPPFBLEVEL], wxT("UPPER_FB_LEVEL"),
	  wxT("Upper FB level (dB re OFM)."),
	  UNIPAR_REAL,
	  &cMRNoiseMPtr->uppFBLevel, NULL,
	  (void * (*)) SetUppFBLevel_CMR_NoiseMasker);
	SetPar_UniParMgr(&pars[CMR_NOISEMASKER_BATTEN], wxT("OFM_LEVEL"),
	  wxT("On-frequency masker level (dB SPL)."),
	  UNIPAR_REAL,
	  &cMRNoiseMPtr->oFMLevel, NULL,
	  (void * (*)) SetOFMLevel_CMR_NoiseMasker);
	SetPar_UniParMgr(&pars[CMR_NOISEMASKER_SPACING], wxT("SPACING"),
	  wxT("Spacing (Hz or octaves)."),
	  UNIPAR_REAL,
	  &cMRNoiseMPtr->spacing, NULL,
	  (void * (*)) SetSpacing_CMR_NoiseMasker);
	SetPar_UniParMgr(&pars[CMR_NOISEMASKER_MASKERMODFREQ], wxT("MASKER_MOD_FREQ"),
	  wxT("Masker modulation frequency (Hz)."),
	  UNIPAR_REAL,
	  &cMRNoiseMPtr->maskerModFreq, NULL,
	  (void * (*)) SetMaskerModFreq_CMR_NoiseMasker);
	SetPar_UniParMgr(&pars[CMR_NOISEMASKER_BANDWIDTH], wxT("BANDWIDTH"),
	  wxT("Bandwidth (Hz)."),
	  UNIPAR_REAL,
	  &cMRNoiseMPtr->bandwidth, NULL,
	  (void * (*)) SetBandwidth_CMR_NoiseMasker);
	SetPar_UniParMgr(&pars[CMR_NOISEMASKER_SIGLEVEL], wxT("SIGNAL_LEVEL"),
	  wxT("Signal level (dB SPL)."),
	  UNIPAR_REAL,
	  &cMRNoiseMPtr->sigLevel, NULL,
	  (void * (*)) SetSigLevel_CMR_NoiseMasker);
	SetPar_UniParMgr(&pars[CMR_NOISEMASKER_SIGFREQ], wxT("SIGNAL_FREQ"),
	  wxT("Signal frequency (Hz)."),
	  UNIPAR_REAL,
	  &cMRNoiseMPtr->sigFreq, NULL,
	  (void * (*)) SetSigFreq_CMR_NoiseMasker);
	SetPar_UniParMgr(&pars[CMR_NOISEMASKER_GATETIME], wxT("SIGNAL_GATE_TIME"),
	  wxT("Signal gate time (s)."),
	  UNIPAR_REAL,
	  &cMRNoiseMPtr->gateTime, NULL,
	  (void * (*)) SetGateTime_CMR_NoiseMasker);
	SetPar_UniParMgr(&pars[CMR_NOISEMASKER_DURATION], wxT("DURATION"),
	  wxT("Duration (s)."),
	  UNIPAR_REAL,
	  &cMRNoiseMPtr->duration, NULL,
	  (void * (*)) SetDuration_CMR_NoiseMasker);
	SetPar_UniParMgr(&pars[CMR_NOISEMASKER_SAMPLINGINTERVAL], wxT("DT"),
	  wxT("Sampling interval, dt (s)."),
	  UNIPAR_REAL,
	  &cMRNoiseMPtr->dt, NULL,
	  (void * (*)) SetSamplingInterval_CMR_NoiseMasker);
	return(TRUE);

}

/****************************** GetUniParListPtr ******************************/

/*
 * This function returns a pointer to the module's universal parameter
 * list.
 */

UniParListPtr
GetUniParListPtr_CMR_NoiseMasker(void)
{
	static const WChar	*funcName = wxT("GetUniParListPtr_CMR_NoiseMasker");

	if (cMRNoiseMPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (cMRNoiseMPtr->parList == NULL) {
		NotifyError(wxT("%s: UniParList data structure has not been initialised. "
		  "NULL returned."), funcName);
		return(NULL);
	}
	return(cMRNoiseMPtr->parList);

}

/****************************** SetBinauralMode *******************************/

/*
 * This function sets the module's binauralMode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetBinauralMode_CMR_NoiseMasker(WChar * theBinauralMode)
{
	static const WChar	*funcName = wxT("SetBinauralMode_CMR_NoiseMasker");
	int		specifier;

	if (cMRNoiseMPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theBinauralMode,
		BooleanList_NSpecLists(0))) == GENERAL_BOOLEAN_NULL) {
		NotifyError(wxT("%s: Illegal switch state (%s)."), funcName, theBinauralMode);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	cMRNoiseMPtr->binauralMode = specifier;
	return(TRUE);

}

/****************************** SetSpacingType ********************************/

/*
 * This function sets the module's p->spacingType parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetSpacingType_CMR_NoiseMasker(WChar * theSpacingType)
{
	static const WChar	*funcName = wxT("SetSpacingType_CMR_NoiseMasker");
	int		specifier;

	if (cMRNoiseMPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theSpacingType,
		cMRNoiseMPtr->spacingTypeList)) == CMR_NOISEMASKER_SPACINGTYPE_NULL) {
		NotifyError(wxT("%s: Illegal name (%s)."), funcName, theSpacingType);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	cMRNoiseMPtr->spacingType = specifier;
	return(TRUE);

}

/****************************** SetCondition **********************************/

/*
 * This function sets the module's p->condition parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetCondition_CMR_NoiseMasker(WChar * theCondition)
{
	static const WChar	*funcName = wxT("SetCondition_CMR_NoiseMasker");
	int		specifier;

	if (cMRNoiseMPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theCondition,
		cMRNoiseMPtr->conditionList)) == CMR_NOISEMASKER_CONDITION_NULL) {
		NotifyError(wxT("%s: Illegal name (%s)."), funcName, theCondition);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	cMRNoiseMPtr->condition = specifier;
	return(TRUE);

}

/****************************** SetOnFreqEar **********************************/

/*
 * This function sets the module's onFreqEar parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetOnFreqEar_CMR_NoiseMasker(WChar * theOnFreqEar)
{
	static const WChar	*funcName = wxT("SetOnFreqEar_CMR_NoiseMasker");
	int		specifier;

	if (cMRNoiseMPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theOnFreqEar,
	  EarModeList_NSpecLists(0))) == GENERAL_EAR_NULL) {
		NotifyError(wxT("%s: Illegal name (%s)."), funcName, theOnFreqEar);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	cMRNoiseMPtr->onFreqEar = specifier;
	return(TRUE);

}

/****************************** SetSigEar *************************************/

/*
 * This function sets the module's sigEar parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetSigEar_CMR_NoiseMasker(WChar * theSigEar)
{
	static const WChar	*funcName = wxT("SetSigEar_CMR_NoiseMasker");
	int		specifier;

	if (cMRNoiseMPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theSigEar,
			EarModeList_NSpecLists(0))) == GENERAL_EAR_NULL) {
		NotifyError(wxT("%s: Illegal name (%s)."), funcName, theSigEar);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	cMRNoiseMPtr->sigEar = specifier;
	return(TRUE);

}

/****************************** SetFlankEar ***********************************/

/*
 * This function sets the module's flankEar parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetFlankEar_CMR_NoiseMasker(WChar *theFlankEar)
{
	static const WChar	*funcName = wxT("SetFlankEar_CMR_NoiseMasker");

	if (cMRNoiseMPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	DSAM_strcpy(cMRNoiseMPtr->flankEar, theFlankEar);
	return(TRUE);

}

/****************************** SetNlow ***************************************/

/*
 * This function sets the module's p->nlow parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetNlow_CMR_NoiseMasker(int theNlow)
{
	static const WChar	*funcName = wxT("SetNlow_CMR_NoiseMasker");

	if (cMRNoiseMPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	cMRNoiseMPtr->nlow = theNlow;
	return(TRUE);

}

/****************************** SetNupp ***************************************/

/*
 * This function sets the module's p->nupp parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetNupp_CMR_NoiseMasker(int theNupp)
{
	static const WChar	*funcName = wxT("SetNupp_CMR_NoiseMasker");

	if (cMRNoiseMPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	cMRNoiseMPtr->nupp = theNupp;
	return(TRUE);

}

/****************************** SetNGapLow ************************************/

/*
 * This function sets the module's nGapLow parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetNGapLow_CMR_NoiseMasker(int theNGapLow)
{
	static const WChar	*funcName = wxT("SetNGapLow_CMR_NoiseMasker");

	if (cMRNoiseMPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	cMRNoiseMPtr->nGapLow = theNGapLow;
	return(TRUE);

}

/****************************** SetNGapUpp ************************************/

/*
 * This function sets the module's nGapUpp parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetNGapUpp_CMR_NoiseMasker(int theNGapUpp)
{
	static const WChar	*funcName = wxT("SetNGapUpp_CMR_NoiseMasker");

	if (cMRNoiseMPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	cMRNoiseMPtr->nGapUpp = theNGapUpp;
	return(TRUE);

}

/****************************** SetRanSeed ************************************/

/*
 * This function sets the module's ranSeed parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetRanSeed_CMR_NoiseMasker(long theRanSeed)
{
	static const WChar	*funcName = wxT("SetRanSeed_CMR_NoiseMasker");

	if (cMRNoiseMPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	cMRNoiseMPtr->updateProcessVariablesFlag = TRUE;
	cMRNoiseMPtr->ranSeed = theRanSeed;
	return(TRUE);

}

/****************************** SetLowFBLevel ***********************************/

/*
 * This function sets the module's lowFBLevel parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetLowFBLevel_CMR_NoiseMasker(double theLowFBLevel)
{
	static const WChar	*funcName = wxT("SetLowFBLevel_CMR_NoiseMasker");

	if (cMRNoiseMPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	cMRNoiseMPtr->lowFBLevel = theLowFBLevel;
	return(TRUE);

}

/****************************** SetUppFBLevel ***********************************/

/*
 * This function sets the module's uppFBLevel parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetUppFBLevel_CMR_NoiseMasker(double theUppFBLevel)
{
	static const WChar	*funcName = wxT("SetUppFBLevel_CMR_NoiseMasker");

	if (cMRNoiseMPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	cMRNoiseMPtr->uppFBLevel = theUppFBLevel;
	return(TRUE);

}

/****************************** SetOFMLevel *************************************/

/*
 * This function sets the module's p->oFMLevel parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetOFMLevel_CMR_NoiseMasker(double theOFMLevel)
{
	static const WChar	*funcName = wxT("SetOFMLevel_CMR_NoiseMasker");

	if (cMRNoiseMPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	cMRNoiseMPtr->oFMLevel = theOFMLevel;
	return(TRUE);

}

/****************************** SetSpacing ************************************/

/*
 * This function sets the module's p->spacing parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetSpacing_CMR_NoiseMasker(double theSpacing)
{
	static const WChar	*funcName = wxT("SetSpacing_CMR_NoiseMasker");

	if (cMRNoiseMPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	cMRNoiseMPtr->spacing = theSpacing;
	return(TRUE);

}

/****************************** SetMaskerModFreq ******************************/

/*
 * This function sets the module's maskerModFreq parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetMaskerModFreq_CMR_NoiseMasker(double theMaskerModFreq)
{
	static const WChar	*funcName = wxT("SetMaskerModFreq_CMR_NoiseMasker");

	if (cMRNoiseMPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	cMRNoiseMPtr->maskerModFreq = theMaskerModFreq;
	return(TRUE);

}

/****************************** SetBandwidth ******************************/

/*
 * This function sets the module's bandwidth parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetBandwidth_CMR_NoiseMasker(double theBandwidth)
{
	static const WChar	*funcName = wxT("SetBandwidth_CMR_NoiseMasker");

	if (cMRNoiseMPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	cMRNoiseMPtr->bandwidth = theBandwidth;
	return(TRUE);

}

/****************************** SetSigLevel ***********************************/

/*
 * This function sets the module's sigLevel parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetSigLevel_CMR_NoiseMasker(double theSigLevel)
{
	static const WChar	*funcName = wxT("SetSigLevel_CMR_NoiseMasker");

	if (cMRNoiseMPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	cMRNoiseMPtr->sigLevel = theSigLevel;
	return(TRUE);

}

/****************************** SetSigFreq ************************************/

/*
 * This function sets the module's sigFreq parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetSigFreq_CMR_NoiseMasker(double theSigFreq)
{
	static const WChar	*funcName = wxT("SetSigFreq_CMR_NoiseMasker");

	if (cMRNoiseMPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	cMRNoiseMPtr->sigFreq = theSigFreq;
	return(TRUE);

}

/****************************** SetGateTime ***********************************/

/*
 * This function sets the module's gateTime parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetGateTime_CMR_NoiseMasker(double theGateTime)
{
	static const WChar	*funcName = wxT("SetGateTime_CMR_NoiseMasker");

	if (cMRNoiseMPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	cMRNoiseMPtr->gateTime = theGateTime;
	return(TRUE);

}

/****************************** SetDuration ***********************************/

/*
 * This function sets the module's duration parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetDuration_CMR_NoiseMasker(double theDuration)
{
	static const WChar	*funcName = wxT("SetDuration_CMR_NoiseMasker");

	if (cMRNoiseMPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	cMRNoiseMPtr->duration = theDuration;
	return(TRUE);

}

/****************************** SetSamplingInterval ***************************/

/*
 * This function sets the module's samplingInterval parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetSamplingInterval_CMR_NoiseMasker(double theSamplingInterval)
{
	static const WChar	*funcName = wxT("SetSamplingInterval_CMR_NoiseMasker");

	if (cMRNoiseMPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	cMRNoiseMPtr->dt = theSamplingInterval;
	return(TRUE);

}

/****************************** PrintPars *************************************/

/*
 * This routine prints all the module's parameters to the file stream.
 * specified by the lUTEar.parsFile file pointer.
 */

BOOLN
PrintPars_CMR_NoiseMasker(void)
{
	/*static const WChar	*funcName = wxT("PrintPars_CMR_NoiseMasker");*/

	DPrint(wxT("?? CMR Module Parameters:-\n"));
	DPrint(wxT("\tbinauralMode = %s\n"), BooleanList_NSpecLists(cMRNoiseMPtr->
	  binauralMode)->name);
	DPrint(wxT("\tpacingType = %s \n"), cMRNoiseMPtr->spacingTypeList[
	  cMRNoiseMPtr->spacingType].name);
	DPrint(wxT("\tcondition = %s \n"), cMRNoiseMPtr->conditionList[
	  cMRNoiseMPtr->condition].name);
	DPrint(wxT("\tonFreqEar = %s \n"), EarModeList_NSpecLists(
	  cMRNoiseMPtr->onFreqEar)->name);
	DPrint(wxT("\tsigEar = %s \n"), EarModeList_NSpecLists(cMRNoiseMPtr->
	  sigEar)->name);
	DPrint(wxT("\tflankEar = %s ??\n"), cMRNoiseMPtr->flankEar);
	DPrint(wxT("\tnlow = %d ??\n"), cMRNoiseMPtr->nlow);
	DPrint(wxT("\tnupp = %d ??\n"), cMRNoiseMPtr->nupp);
	DPrint(wxT("\tnGapLow = %d ??\n"), cMRNoiseMPtr->nGapLow);
	DPrint(wxT("\tnGapUpp = %d ??\n"), cMRNoiseMPtr->nGapUpp);
	DPrint(wxT("\tranSeed = %ld ??\n"), cMRNoiseMPtr->ranSeed);
	DPrint(wxT("\tlowFBLevel = %g ??\n"), cMRNoiseMPtr->lowFBLevel);
	DPrint(wxT("\tuppFBLevel = %g ??\n"), cMRNoiseMPtr->uppFBLevel);
	DPrint(wxT("\toFMLevel = %g ??\n"), cMRNoiseMPtr->oFMLevel);
	DPrint(wxT("\tspacing = %g ??\n"), cMRNoiseMPtr->spacing);
	DPrint(wxT("\tmaskerModFreq = %g ??\n"), cMRNoiseMPtr->maskerModFreq);
	DPrint(wxT("\tbandwidth = %g ??\n"), cMRNoiseMPtr->bandwidth);
	DPrint(wxT("\tsigLevel = %g ??\n"), cMRNoiseMPtr->sigLevel);
	DPrint(wxT("\tsigFreq = %g ??\n"), cMRNoiseMPtr->sigFreq);
	DPrint(wxT("\tgateTime = %g ??\n"), cMRNoiseMPtr->gateTime);
	DPrint(wxT("\tduration = %g ??\n"), cMRNoiseMPtr->duration);
	DPrint(wxT("\tsamplingInterval = %g ??\n"), cMRNoiseMPtr->dt);
	return(TRUE);

}

/****************************** SetParsPointer ********************************/

/*
 * This function sets the global parameter pointer for the module to
 * that associated with the module instance.
 */

BOOLN
SetParsPointer_CMR_NoiseMasker(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("SetParsPointer_CMR_NoiseMasker");

	if (!theModule) {
		NotifyError(wxT("%s: The module is not set."), funcName);
		return(FALSE);
	}
	cMRNoiseMPtr = (CMRNoiseMPtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 * It also initialises the process structure.
 */

BOOLN
InitModule_CMR_NoiseMasker(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("InitModule_CMR_NoiseMasker");

	if (!SetParsPointer_CMR_NoiseMasker(theModule)) {
		NotifyError(wxT("%s: Cannot set parameters pointer."), funcName);
		return(FALSE);
	}
	if (!Init_CMR_NoiseMasker(GLOBAL)) {
		NotifyError(wxT("%s: Could not initialise process structure."), funcName);
		return(FALSE);
	}
	theModule->parsPtr = cMRNoiseMPtr;
	theModule->Free = Free_CMR_NoiseMasker;
	theModule->GetUniParListPtr = GetUniParListPtr_CMR_NoiseMasker;
	theModule->PrintPars = PrintPars_CMR_NoiseMasker;
	theModule->RunProcess = GenerateSignal_CMR_NoiseMasker;
	theModule->SetParsPointer = SetParsPointer_CMR_NoiseMasker;
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
CheckData_CMR_NoiseMasker(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("CheckData_CMR_NoiseMasker");

	if (data == NULL) {
		NotifyError(wxT("%s: EarObject not initialised."), funcName);
		return(FALSE);
	}
	/*** Put additional checks here. ***/
	return(TRUE);

}

/****************************** InitProcessVariables **************************/

/*
 * This function allocates the memory for the process variables.
 * It assumes that all of the parameters for the module have been
 * correctly initialised.
 */

BOOLN
InitProcessVariables_CMR_NoiseMasker(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("InitProcessVariables_CMR_NoiseMasker");
	SignalDataPtr	outSignal = _OutSig_EarObject(data);
	CMRNoiseMPtr	p = cMRNoiseMPtr;

	if (p->updateProcessVariablesFlag || data->updateProcessFlag) {
		FreeProcessVariables_CMR_NoiseMasker();
		if (!SetRandPars_EarObject(data, p->ranSeed, funcName))
			return(FALSE);
		if ((p->fTInv = InitArray_FFT(outSignal->length, TRUE, 1)) == NULL) {
			NotifyError(wxT("%s: Out of memory for fT fft structure."), funcName);
			return(FALSE);
		}
		p->fTInv->plan[0] = fftw_plan_dft_c2r_1d(p->fTInv->fftLen, (fftw_complex *) p->fTInv->data,
		  p->fTInv->data, FFTW_ESTIMATE);
		p->updateProcessVariablesFlag = FALSE;
	}
	if (data->timeIndex == PROCESS_START_TIME) {
		/*** Put reset (to zero ?) code here ***/
	}
	return(TRUE);

}

/****************************** FreeProcessVariables **************************/

/*
 * This routine releases the memory allocated for the process variables
 * if they have been initialised.
 */

BOOLN
FreeProcessVariables_CMR_NoiseMasker(void)
{
	/* static const WChar	*funcName = wxT("FreeProcessVariables_CMR_NoiseMasker");*/

	FreeArray_FFT(&cMRNoiseMPtr->fTInv);
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
GenerateSignal_CMR_NoiseMasker(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("GenerateSignal_CMR_NoiseMasker");
	register ChanData	 *outPtr;
	int		i, k, t, chan, sample;
	double	phase, gatefactor, value, sigAtten, lowAtten, uppAtten;
	CMRNoiseMPtr	p = cMRNoiseMPtr;
	double	batten, sigLevel, srate = 1.0 / p->dt, onfreqmodphase = 0.75 * PIx2;
	double	flankmodphase = 0.75 * PIx2, half_bandw = 0.5 * p->bandwidth;
	fftw_complex	*c1;
	SignalDataPtr	outSignal;

	if (!data->threadRunFlag) {
		if (!CheckData_CMR_NoiseMasker(data)) {
			NotifyError(wxT("%s: Process data invalid."), funcName);
			return(FALSE);
		}
		SetProcessName_EarObject(data, wxT("CMR with noise maskers stimulus."));
		if (!InitOutSignal_EarObject(data, (p->binauralMode)? 2: 1,
		  (ChanLen) floor(p->duration / p->dt + 0.5), p->dt)) {
			NotifyError(wxT("%s: Cannot initialise output channels."), funcName);
			return(FALSE);
		}
		SetInterleaveLevel_SignalData(_OutSig_EarObject(data), (uShort) _OutSig_EarObject(
		  data)->numChannels);
		if (!InitProcessVariables_CMR_NoiseMasker(data)) {
			NotifyError(wxT("%s: Could not initialise the process variables."),
		  funcName);
			return(FALSE);
		}
		if (data->initThreadRunFlag)
			return(TRUE);
	}

	outSignal = _OutSig_EarObject(data);
	batten = RMS_AMP(p->oFMLevel);
	sigAtten = RMS_AMP(p->sigLevel);
	lowAtten = RMS_AMP(p->lowFBLevel + p->oFMLevel);
	uppAtten = RMS_AMP(p->uppFBLevel + p->oFMLevel);
	/* SET PARAMETERS FOR MASKER AND SIGNAL */
	/* ------------------ other useful parameters -----------------------------
	// ------------------ masker parameters from command window --------------
	 */
	
	if (p->condition == CMR_NOISEMASKER_CONDITION_SO)	{	//SO -> signal only
		batten = 0;
	}

	if (p->condition == CMR_NOISEMASKER_CONDITION_CD)	{	//codeviant -> out of phase
		flankmodphase = 0.25 * PIx2;
	}
	// ------------------ signal parameters -----------------------------------
	double sigphase = 0;
	int  gatesample = (int) (srate * p->gateTime); 
	int silence = (int) (outSignal->length / 4) ;

	// -------- precalculations for the masker ---------------------------------
	if (p->condition == CMR_NOISEMASKER_CONDITION_RF)	{ // no flanking bands for ref -> overwrite p->nlow,p->nupp
	p->nlow = 0;
	p->nupp = 0;
	}
	// now calculate center freq of flanking components
	const ntotal = p->nupp + p->nlow;
	//special case. Only one R,L,or D given -> set all flanking band s to that ear
	if (DSAM_strlen(p->flankEar) == 1)	{		
			for (k=1; k < CMR_NOISEMASKER_MAXNTOTAL; k++)	{
				p->flankEar[k] = p->flankEar[0];
			}
	}
	if (DSAM_strlen(p->flankEar) < ntotal){
		NotifyError(wxT("%s: Not all flanking band know which ear they have to go too!"), funcName);
		return (FALSE);
	}
	
	double freqcomp = 0; 
	double freqcomps[CMR_NOISEMASKER_MAXNTOTAL] = {0};
	if (p->spacingType != CMR_NOISEMASKER_SPACINGTYPE_LINEAR)	{  //for log
		for (i=0;i<=(p->nlow-1);i++)	{ // lower components
			freqcomp = p->sigFreq/(double)pow(2.0,(p->nlow - i +p->nGapLow)*(double)p->spacing);
			freqcomps[i] = freqcomp;
		}
		for (i=1;i<=p->nupp;i++)	{	  // upper components
			freqcomp=p->sigFreq*(double)pow(2.0,(i+p->nGapUpp)*(double)p->spacing);
			freqcomps[i + p->nlow -1] = freqcomp;
		}
	}
	else	{				// for lin
		for (i=0;i<=(p->nlow-1);i++)	{ // lower components
				freqcomp = p->sigFreq-p->spacing*(p->nlow - i +p->nGapLow);
				freqcomps[i] = freqcomp;
			}
		for (i=1;i<=p->nupp;i++)	{     // upper components
				freqcomp = p->sigFreq+p->spacing*(i+p->nGapUpp);
				freqcomps[i + p->nlow -1] = freqcomp;
			}
	}
	/* needs some error checks (at the moment still missing!)
	 specials for the multiplied noise */
	int klow = 1; //we dont want a DC 
	int kupp = (half_bandw / srate) * outSignal->length; // hbw -> samples 
	int N = outSignal->length;
	double normfactor = 1.0 / sqrt((double) kupp); // after amultiplying with normfactor each noiseband has 0dB output level 

	/*////////////////////////////////////////////////////////////////////////
	// now calculate the low-pass noise for the on-frequency band on one
	// side (it depends on the p->condition if we use the same or a different 
	// one in the other ear 
	////////////////////////////////////////////////////////////////////////
	 */

	c1 = (fftw_complex *) p->fTInv->data;
	for (k = 0; k < klow; k++)
		CMPLX_RE(c1[k]) = CMPLX_IM(c1[k]) = 0.0;
	for (k = klow; k < (kupp + 1); k++) {
		phase = Ran01_Random(data->randPars) * PIx2;
		CMPLX_RE(c1[k]) = cos(phase);
		CMPLX_IM(c1[k]) = sin(phase);
	}
	for (k = (kupp + 1); k < p->fTInv->fftLen / 2 + 1; k++)
		CMPLX_RE(c1[k]) = CMPLX_IM(c1[k]) = 0.0;
	
	// 2.) do inverse fourier trafo 
	fftw_execute(p->fTInv->plan[0]);

	outSignal = _OutSig_EarObject(data);
	for (chan = 0; chan < outSignal->numChannels; chan++) {
		outPtr = outSignal->channel[chan];
		/* STIMULUS GENERATION
		// --------------- generate stimulus LEFT ear ----------------------------- */
		if (((chan == 0) && (p->sigEar != GENERAL_EAR_RIGHT )) ||
		  (chan == 1) && (p->sigEar != GENERAL_EAR_LEFT)) {		// signal L(eft) or D(iotic)
			for (sample=0; sample<outSignal->length; sample++)
				outPtr[sample] = sin(sigphase + (sample / srate) * p->sigFreq * PIx2) *
				 sigAtten * SQRT_2;
			for (sample=0; sample<silence; sample++) { // silence before and after signal
				outPtr[sample] = 0;
				outPtr[outSignal->length-1-sample]= 0;
			}
			for (sample=silence; sample<(silence+gatesample); sample++) {// apply gate windows
					double gatefactor = (1.0 - cos(((sample-silence) / (double) gatesample) * PI))/2.0;
					outPtr[sample] *= gatefactor;
					outPtr[outSignal->length-sample-1] *= gatefactor;
			}
		}
		for (sample=0; sample<outSignal->length; sample++) {
			if (((chan == 0) && (p->onFreqEar != GENERAL_EAR_RIGHT)) ||
			  ((chan == 1) && (p->onFreqEar != GENERAL_EAR_LEFT)))	{	// on frequency band  L(eft) or D(iotic) 
				value = sin(sigphase + (sample/srate) * p->sigFreq * PIx2) * p->fTInv->data[sample] *
				  normfactor * batten;
				outPtr[sample] = outPtr[sample] + value;
			}
			if (p->condition == CMR_NOISEMASKER_CONDITION_CM)	{
				for (i=0;i<p->nlow;i++)	{//lower bands
					if (p->flankEar[i] != 'R')	{ //flankingband i L(eft) or D(iotic)
						value = sin(sigphase + (sample/srate) * freqcomps[i] * PIx2) * p->fTInv->data[sample] *
						  normfactor * lowAtten;
						outPtr[sample] = outPtr[sample] + value;
					}
				}
				for (i=p->nlow;i<ntotal;i++)	{//upper bands
					if (p->flankEar[i] != 'R')	{ //flankingband i L(eft) or D(iotic)
						value = sin(sigphase + (sample/srate) * freqcomps[i] * PIx2) * p->fTInv->data[sample] *
						  normfactor * uppAtten;
						outPtr[sample] = outPtr[sample] + value;
					}
				}
			}
		}
		if (p->condition == CMR_NOISEMASKER_CONDITION_DV)	{
			for (i=0;i<p->nlow;i++)	{//lower bands
				if (((chan == 0) && (p->flankEar[i] != 'R')) ||
				  ((chan == 1) && (p->flankEar[i] != 'L'))	)	{ //flankingband i  L(eft) or D(iotic)
					////////////////////////////////////////////////////////////
					// deviant means for each band a new lp-noise 
					////////////////////////////////////////////////////////////
					// 1.) set fourier components to 0 outside the desired passband 
					c1 = (fftw_complex *) p->fTInv->data;
					for (k = 0; k < klow; k++)
						CMPLX_RE(c1[k]) = CMPLX_IM(c1[k]) = 0.0;
					for (k = klow; k < (kupp + 1); k++) {
						phase = Ran01_Random(data->randPars) * PIx2;
						CMPLX_RE(c1[k]) = cos(phase);
						CMPLX_IM(c1[k]) = sin(phase);
					}
					for (k = (kupp + 1); k < p->fTInv->fftLen / 2 + 1; k++)
						CMPLX_RE(c1[k]) = CMPLX_IM(c1[k]) = 0.0;
					for (sample=0; sample<outSignal->length; sample++) {
						value = sin(sigphase + (sample/srate) * freqcomps[i] * PIx2) *
						  p->fTInv->data[sample] * normfactor * lowAtten;
						outPtr[sample] = outPtr[sample] + value;
					}
				}
			}
			for (i=p->nlow;i<ntotal;i++)	{//upper bands
				if (((chan == 0) && (p->flankEar[i] != 'R')) ||
				  ((chan == 1) && (p->flankEar[i] != 'L')))	{ //flankingband i  L(eft) or D(iotic)
					/* deviant means for each band a new lp-noise */
					////////////////////////////////////////////////////////////
					// 1.) set fourier components to 0 outside the desired passband 
					c1 = (fftw_complex *) p->fTInv->data;
					for (k = 0; k < klow; k++)
						CMPLX_RE(c1[k]) = CMPLX_IM(c1[k]) = 0.0;
					for (k = klow; k < (kupp + 1); k++) {
						phase = Ran01_Random(data->randPars) * PIx2;
						CMPLX_RE(c1[k]) = cos(phase);
						CMPLX_IM(c1[k]) = sin(phase);
					}
					for (k = (kupp + 1); k < p->fTInv->fftLen / 2 + 1; k++)
						CMPLX_RE(c1[k]) = CMPLX_IM(c1[k]) = 0.0;
					// 2.) do inverse fourier trafo 
					fftw_execute(p->fTInv->plan[0]);
					for (sample=0; sample<outSignal->length; sample++) {
						value = sin(sigphase + (sample/srate) * freqcomps[i] * PIx2) *
						p->fTInv->data[sample] * normfactor * uppAtten;
						outPtr[sample] = outPtr[sample] + value;
					}
				}
			}
		}
	}


	SetProcessContinuity_EarObject(data);
	return(TRUE);

}
