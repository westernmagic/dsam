/**********************
 *
 * File:		AnSAImage.c
 * Purpose:		This module implements the AIM stabilised auditory image (SAI)
 *				process.
 *				See Patterson R. D. and Allerhand M. H. (1995) "Time-domain
 *				modeling of peripheral auditory processing: A modular
 *				Architecture and a software platform", J. Acoust. Soc. Am. 98,
 *				1890-1894.
 * Comments:	Written using ModuleProducer version 1.12 (Oct 12 1997).
 *				This module uses the UtStrobe utility module.
 *				29-10-97 LPO: Introduced 'dataBuffer' EarObject for the
 *				correct treatment during segment processing.  The previous data
 *				is assumed to be the negative width plus zero.
 *				04-11-97 LPO: Introduced the correct 'strobe is time zero'
 *				behaviour.
 *				04-11-97 LPO: Improved buffer algorithm so the signal segments
 *				less than the SAI length can be used.
 *				18-11-97 LPO: Introduced the inputDecayRate parameter.
 *				11-12-97 LPO: Allowed the "delayed" strobe modes to be processed
 *				correctly, by interpreting delays as an additional shift using
 *				the postive and negative widths.
 *				28-05-98 LPO: Put in check for zero image decay half-life.
 *				09-06-98 LPO: The strobe information is now output as part of
 *				the diagnostics.
 *				12-06-98 LPO: Corrected numLastSamples problem, which was
 *				causing strobes to be processed passed the end of the strobe
 *				data.
 *				30-06-98 LPO: Introduced use of the SignalData structure's
 *				'numWindowFrames' field.
 *				08-10-98 LPO: The initialisation and free'ing of the 'Strobe'
 *				utility module is now done at the same time as the SAI module.
 *				This was necessary for the UniPar implementation.
 * Author:		L. P. O'Mard
 * Created:		12 Oct 1997
 * Updated:		08 Oct 1998
 * Copyright:	(c) 1998, University of Essex
 *
 *********************/

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "GeCommon.h"
#include "GeSignalData.h"
#include "GeEarObject.h"
#include "GeUniParMgr.h"
#include "GeModuleMgr.h"
#include "GeNSpecLists.h"
#include "FiParFile.h"
#include "UtString.h"
#include "AnSAImage.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

SAImagePtr	sAImagePtr = NULL;

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
Free_Analysis_SAI(void)
{
	/* static const char	*funcName = "Free_Analysis_SAI";  */

	if (sAImagePtr == NULL)
		return(FALSE);
	FreeProcessVariables_Analysis_SAI();
	Free_EarObject(&sAImagePtr->strobeData);
	if (sAImagePtr->diagnosticModeList)
		free(sAImagePtr->diagnosticModeList);
	if (sAImagePtr->parList)
		FreeList_UniParMgr(&sAImagePtr->parList);
	if (sAImagePtr->parSpec == GLOBAL) {
		free(sAImagePtr);
		sAImagePtr = NULL;
	}
	return(TRUE);

}

/****************************** InitIntegrationModeList ***********************/

/*
 * This function initialises the 'integrationMode' list array
 */

BOOLN
InitIntegrationModeList_Analysis_SAI(void)
{
	static NameSpecifier	modeList[] = {

			{ "STI",	SAI_INTEGRATION_MODE_STI },
			{ "AC",		SAI_INTEGRATION_MODE_AC },
			{ "",		SAI_INTEGRATION_MODE_NULL },
		};
	sAImagePtr->integrationModeList = modeList;
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
Init_Analysis_SAI(ParameterSpecifier parSpec)
{
	static const char	*funcName = "Init_Analysis_SAI";

	if (parSpec == GLOBAL) {
		if (sAImagePtr != NULL)
			Free_Analysis_SAI();
		if ((sAImagePtr = (SAImagePtr) malloc(sizeof(SAImage))) == NULL) {
			NotifyError("%s: Out of memory for 'global' pointer", funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (sAImagePtr == NULL) {
			NotifyError("%s:  'local' pointer not set.", funcName);
			return(FALSE);
		}
	}
	sAImagePtr->parSpec = parSpec;
	sAImagePtr->updateProcessVariablesFlag = TRUE;
	sAImagePtr->diagnosticModeFlag = TRUE;
	sAImagePtr->integrationModeFlag = TRUE;
	sAImagePtr->strobeSpecificationFlag = FALSE;
	sAImagePtr->positiveWidthFlag = TRUE;
	sAImagePtr->negativeWidthFlag = TRUE;
	sAImagePtr->inputDecayRateFlag = TRUE;
	sAImagePtr->imageDecayHalfLifeFlag = TRUE;
	sAImagePtr->diagnosticMode = GENERAL_BOOLEAN_OFF;
	sAImagePtr->integrationMode = SAI_INTEGRATION_MODE_STI;
	sAImagePtr->strobeSpecification[0] = '\0';
	sAImagePtr->positiveWidth = 5.0e-3;
	sAImagePtr->negativeWidth = -35.0e-3;
	sAImagePtr->inputDecayRate = 2.5e+3;
	sAImagePtr->imageDecayHalfLife = 30e-3;

	if ((sAImagePtr->diagnosticModeList = InitNameList_NSpecLists(
	  DiagModeList_NSpecLists(0), sAImagePtr->diagnosticString)) == NULL)
		return(FALSE);
	InitIntegrationModeList_Analysis_SAI();
	if ((sAImagePtr->strobeData = Init_EarObject("Util_Strobe")) == NULL) {
		NotifyError("%s: Could not initialise strobe data EarObject", funcName);
		return(FALSE);
	}
	if (!SetUniParList_Analysis_SAI()) {
		NotifyError("%s: Could not initialise parameter list.", funcName);
		Free_Analysis_SAI();
		return(FALSE);
	}
	strcpy(sAImagePtr->diagnosticString, DEFAULT_FILE_NAME);
	sAImagePtr->strobeInSignalIndex = -1;
	sAImagePtr->inputDecay = NULL;
	sAImagePtr->fp = NULL;
	sAImagePtr->decayCount = NULL;
	sAImagePtr->inputCount = NULL;
	sAImagePtr->dataBuffer = NULL;
	sAImagePtr->strobeDataBuffer = NULL;
	return(TRUE);

}

/********************************* SetUniParList ******************************/

/*
 * This routine initialises and sets the module's universal parameter list.
 * This list provides universal access to the module's parameters.
 */
 
BOOLN
SetUniParList_Analysis_SAI(void)
{
	static const char *funcName = "SetUniParList_Analysis_SAI";
	UniParPtr	pars;

	if ((sAImagePtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  SAI_NUM_PARS, NULL)) == NULL) {
		NotifyError("%s: Could not initialise parList.", funcName);
		return(FALSE);
	}
	pars = sAImagePtr->parList->pars;
	SetPar_UniParMgr(&pars[SAI_DIAGNOSTIC_MODE], "DIAGNOSTICS",
	  "Diagnostic mode ('off', 'screen', 'error' or <file name>).",
	  UNIPAR_NAME_SPEC_WITH_FILE,
	  &sAImagePtr->diagnosticMode, sAImagePtr->diagnosticModeList,
	  (void * (*)) SetDiagnosticMode_Analysis_SAI);
	SetPar_UniParMgr(&pars[SAI_INTEGRATION_MODE], "INT_MODE",
	  "Integration mode: 'STI' - stabilised temporal integration, 'AC' - "
	  "autocorrelation.",
	  UNIPAR_NAME_SPEC,
	  &sAImagePtr->integrationMode, sAImagePtr->integrationModeList,
	  (void * (*)) SetIntegrationMode_Analysis_SAI);
	SetPar_UniParMgr(&pars[SAI_STROBE_SPECIFICATION], "STROBE_PAR_FILE",
	  "Strobe module parameter file name.",
	  UNIPAR_MODULE,
	  &sAImagePtr->strobeSpecification, GetUniParListPtr_ModuleMgr(sAImagePtr->
	  strobeData),
	  (void * (*)) SetStrobeSpecification_Analysis_SAI);
	SetPar_UniParMgr(&pars[SAI_NEGATIVE_WIDTH], "NWIDTH",
	  "Negative width of auditory image (s).",
	  UNIPAR_REAL,
	  &sAImagePtr->negativeWidth, NULL,
	  (void * (*)) SetNegativeWidth_Analysis_SAI);
	SetPar_UniParMgr(&pars[SAI_POSITIVE_WIDTH], "PWIDTH",
	  "Positive width of auditory image (s).",
	  UNIPAR_REAL,
	  &sAImagePtr->positiveWidth, NULL,
	  (void * (*)) SetPositiveWidth_Analysis_SAI);
	SetPar_UniParMgr(&pars[SAI_INPUT_DECAY_RATE], "NAP_DECAY",
	  "Neural activity pattern (input) decay rate (%/s)",
	  UNIPAR_REAL,
	  &sAImagePtr->inputDecayRate, NULL,
	  (void * (*)) SetInputDecayRate_Analysis_SAI);
	SetPar_UniParMgr(&pars[SAI_IMAGE_DECAY_HALF_LIFE], "IMAGE_DECAY",
	  "Auditory image decay half-life (s).",
	  UNIPAR_REAL,
	  &sAImagePtr->imageDecayHalfLife, NULL,
	  (void * (*)) SetImageDecayHalfLife_Analysis_SAI);

	return(TRUE);

}

/********************************* GetUniParListPtr ***************************/

/*
 * This routine returns a pointer to the module's universal parameter list.
 */

UniParListPtr
GetUniParListPtr_Analysis_SAI(void)
{
	static const char *funcName = "GetUniParListPtr_Analysis_SAI";

	if (sAImagePtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (sAImagePtr->parList == NULL) {
		NotifyError("%s: UniParList data structure has not been initialised. "
		  "NULL returned.", funcName);
		return(NULL);
	}
	return(sAImagePtr->parList);

}

/****************************** SetPars ***************************************/

/*
 * This function sets all the module's parameters.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetPars_Analysis_SAI(char * diagnosticMode, char * integrationMode,
  char *strobeSpecification, double negativeWidth, double positiveWidth,
  double inputDecayRate, double imageDecayHalfLife)
{
	static const char	*funcName = "SetPars_Analysis_SAI";
	BOOLN	ok;

	ok = TRUE;
	if (!SetDiagnosticMode_Analysis_SAI(diagnosticMode))
		ok = FALSE;
	if (!SetIntegrationMode_Analysis_SAI(integrationMode))
		ok = FALSE;
	if (!SetStrobeSpecification_Analysis_SAI(strobeSpecification))
		ok = FALSE;
	if (!SetNegativeWidth_Analysis_SAI(negativeWidth))
		ok = FALSE;
	if (!SetPositiveWidth_Analysis_SAI(positiveWidth))
		ok = FALSE;
	if (!SetInputDecayRate_Analysis_SAI(inputDecayRate))
		ok = FALSE;
	if (!SetImageDecayHalfLife_Analysis_SAI(imageDecayHalfLife))
		ok = FALSE;
	if (!ok)
		NotifyError("%s: Failed to set all module parameters." ,funcName);
	return(ok);

}

/****************************** SetDiagnosticMode ****************************/

/*
 * This function sets the module's diagnosticMode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetDiagnosticMode_Analysis_SAI(char *theDiagnosticMode)
{
	static const char	*funcName = "SetDiagnosticMode_Analysis_SAI";

	if (sAImagePtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	sAImagePtr->diagnosticModeFlag = TRUE;
	sAImagePtr->diagnosticMode = IdentifyDiag_NSpecLists(theDiagnosticMode,
	  sAImagePtr->diagnosticModeList);
	return(TRUE);

}

/****************************** SetIntegrationMode ****************************/

/*
 * This function sets the module's integrationMode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetIntegrationMode_Analysis_SAI(char * theIntegrationMode)
{
	static const char	*funcName = "SetIntegrationMode_Analysis_SAI";
	int		specifier;

	if (sAImagePtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theIntegrationMode,
		sAImagePtr->integrationModeList)) == SAI_INTEGRATION_MODE_NULL) {
		NotifyError("%s: Illegal name (%s).", funcName, theIntegrationMode);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	sAImagePtr->integrationModeFlag = TRUE;
	sAImagePtr->integrationMode = specifier;
	return(TRUE);

}

/****************************** SetStrobeSpecification ************************/

/*
 * This function sets the module's strobeSpecification parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetStrobeSpecification_Analysis_SAI(char *theStrobeSpecification)
{
	static const char	*funcName = "SetStrobeSpecification_Analysis_SAI";
	BOOLN	ok;
	ParFilePtr	oldPtr = parFile;

	if (sAImagePtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (strcmp(theStrobeSpecification, NO_FILE) != 0) {
		parFile = NULL;
		ok = ReadPars_ModuleMgr(sAImagePtr->strobeData, theStrobeSpecification);
		parFile = oldPtr;
		if (!ok) {
			NotifyError("%s: Could not read strobe utility module parameters.",
			  funcName);
			return(FALSE);
		}
	}
	sAImagePtr->strobeSpecificationFlag = TRUE;
	snprintf(sAImagePtr->strobeSpecification, MAX_FILE_PATH, "%s",
	  theStrobeSpecification);
	return(TRUE);

}

/****************************** SetPositiveWidth ******************************/

/*
 * This function sets the module's positiveWidth parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetPositiveWidth_Analysis_SAI(double thePositiveWidth)
{
	static const char	*funcName = "SetPositiveWidth_Analysis_SAI";

	if (sAImagePtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	sAImagePtr->positiveWidthFlag = TRUE;
	sAImagePtr->positiveWidth = thePositiveWidth;
	sAImagePtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/****************************** SetNegativeWidth ******************************/

/*
 * This function sets the module's negativeWidth parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetNegativeWidth_Analysis_SAI(double theNegativeWidth)
{
	static const char	*funcName = "SetNegativeWidth_Analysis_SAI";

	if (sAImagePtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (theNegativeWidth > 0.0) {
		NotifyError("%s: Illegal negative width (%g ms).", funcName,
		  MSEC(theNegativeWidth));
		return(FALSE);
	}
	sAImagePtr->negativeWidthFlag = TRUE;
	sAImagePtr->negativeWidth = theNegativeWidth;
	sAImagePtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/****************************** SetInputDecayRate *****************************/

/*
 * This function sets the module's inputDecayRate parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetInputDecayRate_Analysis_SAI(double theInputDecayRate)
{
	static const char	*funcName = "SetInputDecayRate_Analysis_SAI";

	if (sAImagePtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	sAImagePtr->inputDecayRateFlag = TRUE;
	sAImagePtr->inputDecayRate = theInputDecayRate;
	sAImagePtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/****************************** SetImageDecayHalfLife *************************/

/*
 * This function sets the module's imageDecayHalfLife parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetImageDecayHalfLife_Analysis_SAI(double theImageDecayHalfLife)
{
	static const char	*funcName = "SetImageDecayHalfLife_Analysis_SAI";

	if (sAImagePtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (theImageDecayHalfLife <= 0.0) {
		NotifyError("%s: Value must be greater than zero (%g).", funcName,
		  theImageDecayHalfLife);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	sAImagePtr->imageDecayHalfLifeFlag = TRUE;
	sAImagePtr->imageDecayHalfLife = theImageDecayHalfLife;
	return(TRUE);

}

/****************************** SetDelay **************************************/

/*
 * This function sets the delay for the modules local instance of the
 * strobe utility module.
 */

BOOLN
SetDelay_Analysis_SAI(double theDelay)
{
	return(SetRealPar_ModuleMgr(sAImagePtr->strobeData, "strobe_lag",
	  theDelay));

}

/****************************** SetDelayTimeout *******************************/

/*
 * This function sets the delay timeout for the modules local instance of the
 * strobe utility module.
 */

BOOLN
SetDelayTimeout_Analysis_SAI(double theDelayTimeout)
{
	return(SetRealPar_ModuleMgr(sAImagePtr->strobeData, "timeout",
	  theDelayTimeout));

}

/****************************** SetThresholdDecayRate *************************/

/*
 * This function sets the threshold decay rate for the modules local instance
 * of the strobe utility module.
 */

BOOLN
SetThresholdDecayRate_Analysis_SAI(double theThresholdDecayRate)
{
	return(SetRealPar_ModuleMgr(sAImagePtr->strobeData, "threshold_decay",
	  theThresholdDecayRate));

}

/****************************** SetThreshold **********************************/

/*
 * This function sets thethreshold for the modules local instance of the
 * strobe utility module.
 */

BOOLN
SetThreshold_Analysis_SAI(double theThreshold)
{
	return(SetRealPar_ModuleMgr(sAImagePtr->strobeData, "threshold",
	  theThreshold));

}

/****************************** SetTypeMode ***********************************/

/*
 * This function sets the type mode for the modules local instance of the
 * strobe utility module.
 */

BOOLN
SetTypeMode_Analysis_SAI(char *theTypeMode)
{
	return(SetPar_ModuleMgr(sAImagePtr->strobeData, "criterion", theTypeMode));

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
CheckPars_Analysis_SAI(void)
{
	static const char	*funcName = "CheckPars_Analysis_SAI";
	BOOLN	ok;

	ok = TRUE;
	if (sAImagePtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (!sAImagePtr->diagnosticModeFlag) {
		NotifyError("%s: diagnosticMode variable not set.", funcName);
		ok = FALSE;
	}
	if (!sAImagePtr->integrationModeFlag) {
		NotifyError("%s: integrationMode variable not set.", funcName);
		ok = FALSE;
	}
	if (!sAImagePtr->strobeSpecificationFlag) {
		NotifyError("%s: strobeSpecification variable not set.", funcName);
		ok = FALSE;
	}
	if (!sAImagePtr->positiveWidthFlag) {
		NotifyError("%s: positiveWidth variable not set.", funcName);
		ok = FALSE;
	}
	if (!sAImagePtr->negativeWidthFlag) {
		NotifyError("%s: negativeWidth variable not set.", funcName);
		ok = FALSE;
	}
	if (!sAImagePtr->inputDecayRateFlag) {
		NotifyError("%s: inputDecayRate variable not set.", funcName);
		ok = FALSE;
	}
	if (!sAImagePtr->imageDecayHalfLifeFlag) {
		NotifyError("%s: imageDecayHalfLife variable not set.", funcName);
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
PrintPars_Analysis_SAI(void)
{
	static const char	*funcName = "PrintPars_Analysis_SAI";

	if (!CheckPars_Analysis_SAI()) {
		NotifyError("%s: Parameters have not been correctly set.", funcName);
		return(FALSE);
	}
	DPrint("Stabilised Auditory Image Analysis Module Parameters:-\n");
	DPrint("\tStrobeSpecification (%s):\n", sAImagePtr->strobeSpecification);
	SetDiagnosticsPrefix("\t");
	PrintPars_ModuleMgr(sAImagePtr->strobeData);
	SetDiagnosticsPrefix(NULL);
	DPrint("\tIntegration mode = %s,\n", sAImagePtr->integrationModeList[
	  sAImagePtr->integrationMode].name);
	DPrint("\tNegative width = %g ms,", MSEC(sAImagePtr->negativeWidth));
	DPrint("\tPositive width = %g ms,\n", MSEC(sAImagePtr->positiveWidth));
	DPrint("\tInput decay rate = %g %/ms,", sAImagePtr->inputDecayRate /
	  MSEC(1.0));
	DPrint("\tImage decay half-life = %g ms,\n",
	  MSEC(sAImagePtr->imageDecayHalfLife));
	DPrint("\tDiagnostic mode: %s.\n", sAImagePtr->diagnosticModeList[
	  sAImagePtr->diagnosticMode].name);
	return(TRUE);

}

/****************************** ReadPars **************************************/

/*
 * This program reads a specified number of parameters from a file.
 * It returns FALSE if it fails in any way.n */

BOOLN
ReadPars_Analysis_SAI(char *fileName)
{
	static const char	*funcName = "ReadPars_Analysis_SAI";
	BOOLN	ok;
	char	*filePath, diagnosticMode[MAXLINE], strobeSpecification[MAXLINE];
	char	integrationMode[MAXLINE];
	double	positiveWidth, negativeWidth, inputDecayRate, imageDecayHalfLife;
	FILE	*fp;

	filePath = GetParsFileFPath_Common(fileName);
	if ((fp = fopen(filePath, "r")) == NULL) {
		NotifyError("%s: Cannot open data file '%s'.\n", funcName, filePath);
		return(FALSE);
	}
	DPrint("%s: Reading from '%s':\n", funcName, filePath);
	Init_ParFile();
	ok = TRUE;
	if (!GetPars_ParFile(fp, "%s", diagnosticMode))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%s", integrationMode))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%s", strobeSpecification))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &negativeWidth))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &positiveWidth))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &inputDecayRate))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &imageDecayHalfLife))
		ok = FALSE;
	fclose(fp);
	Free_ParFile();
	if (!ok) {
		NotifyError("%s: Not enough lines, or invalid parameters, in module "
		  "parameter file '%s'.", funcName, filePath);
		return(FALSE);
	}
	if (!SetPars_Analysis_SAI(diagnosticMode, integrationMode,
	  strobeSpecification, negativeWidth, positiveWidth, inputDecayRate,
	  imageDecayHalfLife)) {
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
SetParsPointer_Analysis_SAI(ModulePtr theModule)
{
	static const char	*funcName = "SetParsPointer_Analysis_SAI";

	if (!theModule) {
		NotifyError("%s: The module is not set.", funcName);
		return(FALSE);
	}
	sAImagePtr = (SAImagePtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 */

BOOLN
InitModule_Analysis_SAI(ModulePtr theModule)
{
	static const char	*funcName = "InitModule_Analysis_SAI";

	if (!SetParsPointer_Analysis_SAI(theModule)) {
		NotifyError("%s: Cannot set parameters pointer.", funcName);
		return(FALSE);
	}
	if (!Init_Analysis_SAI(GLOBAL)) {
		NotifyError("%s: Could not initialise process structure.", funcName);
		return(FALSE);
	}
	theModule->parsPtr = sAImagePtr;
	theModule->threadMode = MODULE_THREAD_MODE_SIMPLE;
	theModule->CheckPars = CheckPars_Analysis_SAI;
	theModule->Free = Free_Analysis_SAI;
	theModule->GetUniParListPtr = GetUniParListPtr_Analysis_SAI;
	theModule->PrintPars = PrintPars_Analysis_SAI;
	theModule->ReadPars = ReadPars_Analysis_SAI;
	theModule->ResetProcess = ResetProcess_Analysis_SAI;
	theModule->RunProcess = Process_Analysis_SAI;
	theModule->SetParsPointer = SetParsPointer_Analysis_SAI;
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
CheckData_Analysis_SAI(EarObjectPtr data)
{
	static const char	*funcName = "CheckData_Analysis_SAI";
	double	width, strobeDelay;
	int		strobeType;

	if (data == NULL) {
		NotifyError("%s: EarObject not initialised.", funcName);
		return(FALSE);
	}
	if (!CheckInSignal_EarObject(data, funcName))
		return(FALSE);
	strobeType = *GetUniParPtr_ModuleMgr(sAImagePtr->strobeData, "criterion")->
	  valuePtr.nameList.specifier;
	if (strobeType == STROBE_USER_MODE) {
		if (data->numInSignals < 2) {
			NotifyError("%s: In strobe 'USER' mode two input EarObjects are\n"
			  "required (%d)", funcName, data->numInSignals);
			return(FALSE);
		}
		if (!CheckInit_SignalData(data->inSignal[1],
		  "CheckData_Analysis_SAI")) {
			NotifyError("%s: Second EarObject required for USER mode not "
			  "initialised.", funcName);
			return(FALSE);
		}
		if (!SameType_SignalData(data->inSignal[0], data->inSignal[1])) {
			NotifyError("%s: Input signals are not the same.", funcName);		
			return(FALSE);
		}
	}
	width = sAImagePtr->positiveWidth - sAImagePtr->negativeWidth +
	  data->inSignal[0]->dt;
	if (width <= 0.0) {
		NotifyError("%s: Illegal frame width (%g -> %g ms).", funcName,
		  MSEC(sAImagePtr->negativeWidth), MSEC(sAImagePtr->positiveWidth));
		return(FALSE);
	}
	strobeDelay = *GetUniParPtr_ModuleMgr(sAImagePtr->strobeData,
	  "strobe_lag")->valuePtr.r;
	if ((strobeType == STROBE_PEAK_SHADOW_POSITIVE_MODE) && (sAImagePtr->
	  positiveWidth < strobeDelay)) {
		NotifyError("%s: The positive width (%g ms) must be less than strobe "
		  "delay (%g ms).", funcName, MSEC(sAImagePtr->positiveWidth),
		  MSEC(strobeDelay));
		return(FALSE);
	}
	return(TRUE);

}

/****************************OutputStrobeData *********************************/

/*
 * This routine outputs the strobe data to the specified file stream.
 * It expects the strobe data to have been initialised correctly.
 */

void
OutputStrobeData_Analysis_SAI(void)
{
	/* static const char *funcName = "OutputStrobeData_Analysis_SAI"; */
	int		chan;
	ChanLen	i, t;
	SignalDataPtr	signal;

	signal = sAImagePtr->strobeData->outSignal;
	fprintf(sAImagePtr->fp, "Time (s)");
	for (chan = signal->offset; chan < signal->numChannels; chan++)
		fprintf(sAImagePtr->fp, "\t[%d]", chan);
	fprintf(sAImagePtr->fp, "\n");
	for (i = 0, t = signal->timeIndex; i < signal->length; i++, t++) {
		fprintf(sAImagePtr->fp, "%g", t * signal->dt +
		  signal->outputTimeOffset);
		for (chan = signal->offset; chan < signal->numChannels; chan++)
			fprintf(sAImagePtr->fp, "\t%g", signal->channel[chan][i]);
		fprintf(sAImagePtr->fp, "\n");
	}

}

/**************************** InitInputDecayArray *****************************/

/*
 * This function initialises the input decay array for the process variables.
 * It assumes that all of the parameters for the module have been correctly
 * initialised.
 * This function assumes that the inputDecayRate is the percentage decay per
 * second.
 * The number of samples per second = 1 / dt.
 * The decay values scale the input by 1.0 at the strobe point, e.g. in the SAI
 * the values fall to the left and rise to the right of 0 s, when the
 * inputDecayRate > 0.0.
 * This is the opposite sign to that in AIM because in AIM the decay array is
 * applied from the end of the channel, backwards.
 */

BOOLN
InitInputDecayArray_Analysis_SAI(EarObjectPtr data)
{
	static const char *funcName = "InitInputDecayArray_Analysis_SAI";
	double	decayPerSample, *decayPtr, totalDecay;
	ChanLen	i;

	if ((sAImagePtr->inputDecay = (double *) calloc(data->outSignal->length,
	  sizeof(double))) == NULL) {
		NotifyError("%s: Could not initialise input decay array", funcName);
		return(FALSE);
	}
	decayPerSample = -sAImagePtr->inputDecayRate / 100.0 * data->outSignal->dt;
	totalDecay = 1.0 + sAImagePtr->zeroIndex * decayPerSample;
	decayPtr = sAImagePtr->inputDecay;
	for (i = 0; i < data->outSignal->length; i++) {
		if (totalDecay >= 1.0)
			*decayPtr++ = 1.0;
		else if (totalDecay <= 0.0)
			*decayPtr++ = 0.0;
		else
			*decayPtr++ = totalDecay;
		totalDecay -= decayPerSample;
	}
	return(TRUE);

}

/**************************** ResetProcess ************************************/

/*
 * This routine resets the process variables.
 */

void
ResetProcess_Analysis_SAI(EarObjectPtr data)
{
	int		i;
	SAImagePtr	p = sAImagePtr;

	ResetOutSignal_EarObject(data);
	SetNumWindowFrames_SignalData(data->outSignal, 0);
	ResetOutSignal_EarObject(p->dataBuffer);
	ResetOutSignal_EarObject(p->strobeDataBuffer);
	if (data->threadRunFlag)
		p->inputCount[data->threadIndex] = 0;
	else {
		for (i = 0; i < data->numThreads; i++)
			p->inputCount[i] = 0;
	}
	for (i = data->outSignal->offset; i < data->outSignal->numChannels; i++)
		p->decayCount[i] = 0;

}

/**************************** InitProcessVariables ****************************/

/*
 * This function allocates the memory for the process variables.
 * It assumes that all of the parameters for the module have been correctly
 * initialised.
 */

BOOLN
InitProcessVariables_Analysis_SAI(EarObjectPtr data)
{
	static const char *funcName = "InitProcessVariables_Analysis_SAI";
	SAImagePtr	p = sAImagePtr;

	if (p->updateProcessVariablesFlag || data->updateProcessFlag) {
		FreeProcessVariables_Analysis_SAI();
		OpenDiagnostics_NSpecLists(&p->fp, p->diagnosticModeList,
		  p->diagnosticMode);
		if (!InitSubProcessList_EarObject(data, SAI_NUM_SUB_PROCESSES)) {
			NotifyError("%s: Could not initialise %d sub-process list for "
			  "process.", funcName, SAI_NUM_SUB_PROCESSES);
			return(FALSE);
		}
		data->subProcessList[SAI_STROBEDATA] = p->strobeData;
		if ((p->dataBuffer = Init_EarObject("NULL")) == NULL) {
			NotifyError("%s: Could not initialise previous data EarObject",
			  funcName);
			return(FALSE);
		}
		if ((p->strobeDataBuffer = Init_EarObject("NULL")) == NULL) {
			NotifyError("%s: Could not initialise previous strobe data "
			  "EarObject", funcName);
			return(FALSE);
		}
		p->zeroIndex = (ChanLen) floor(-p->negativeWidth / data->inSignal[0]->
		  dt + 0.5);
		p->positiveWidthIndex = data->outSignal->length - p->zeroIndex;
		if (!InitInputDecayArray_Analysis_SAI(data)) {
			NotifyError("%s: failed in to initialise input decay array",
			  funcName);
			return(FALSE);
		}
		if ((p->decayCount = (ChanLen *) calloc(data->outSignal->numChannels,
		  sizeof(ChanLen))) == NULL) {
			NotifyError("%s: Could not initialise decayCount array",
			  funcName);
			return(FALSE);
		}
		if ((p->inputCount = (ChanLen *) calloc(data->numThreads, sizeof(
		  ChanLen))) == NULL) {
			NotifyError("%s: Out of memory for inputCount array.",
			  funcName);
			return(FALSE);
		}
		p->strobeInSignalIndex = (*GetUniParPtr_ModuleMgr(p->strobeData,
		  "criterion")->valuePtr.nameList.specifier == STROBE_USER_MODE)? 1: 0;
		if (!InitOutSignal_EarObject(p->dataBuffer, data->outSignal->
		  numChannels, data->outSignal->length, data->outSignal->dt)) {
			NotifyError("%s: Cannot initialise channels for previous data.",
			  funcName);
			return(FALSE);
		}
		if (!InitOutSignal_EarObject(p->strobeDataBuffer, data->outSignal->
		  numChannels, data->outSignal->length, data->outSignal->dt)) {
			NotifyError("%s: Cannot initialise channels for previous strobe "
			  "data.", funcName);
			return(FALSE);
		}
		p->updateProcessVariablesFlag = FALSE;
	}
	if (data->timeIndex == PROCESS_START_TIME) {
		ResetProcess_Analysis_SAI(data);
	}
	return(TRUE);

}

/**************************** FreeProcessVariables ****************************/

/*
 * This routine releases the memory allocated for the process variables
 * if they have been initialised.
 */

BOOLN
FreeProcessVariables_Analysis_SAI(void)
{
	Free_EarObject(&sAImagePtr->dataBuffer);
	Free_EarObject(&sAImagePtr->strobeDataBuffer);
	if (sAImagePtr->inputDecay) {
		free(sAImagePtr->inputDecay);
		sAImagePtr->inputDecay = NULL;
	}
	if (sAImagePtr->fp) {
		fclose(sAImagePtr->fp);
		sAImagePtr->fp = NULL;
	}
	if (sAImagePtr->decayCount) {
		free(sAImagePtr->decayCount);
		sAImagePtr->decayCount = NULL;
	}
	if (sAImagePtr->inputCount) {
		free(sAImagePtr->inputCount);
		sAImagePtr->inputCount = NULL;
	}
	sAImagePtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/**************************** PushBufferData **********************************/

/*
 * This routine pushes the input and strobe data into the previous data
 * buffers.
 */

void
PushBufferData_Analysis_SAI(EarObjectPtr data, ChanLen frameLength)
{
	/* static const char	*funcName = "PushBufferData_Analysis_SAI"; */
	register ChanData	*inPtr, *outPtr, *inStrobePtr, *outStrobePtr;
	int		chan;
	ChanLen	i, shiftLength, inputCount;
	SAImagePtr	p = sAImagePtr;

	inputCount = *(p->inputCount + data->threadIndex);
	for (chan = data->outSignal->offset; chan < data->outSignal->numChannels;
	  chan++) {
		if (frameLength == data->outSignal->length) {
			inPtr = data->inSignal[0]->channel[chan] + inputCount;
			inStrobePtr = p->strobeData->outSignal->channel[chan] + inputCount;
			shiftLength = frameLength;
		} else {
			inPtr = p->dataBuffer->outSignal->channel[chan] + frameLength;
			inStrobePtr = p->strobeDataBuffer->outSignal->channel[
			  chan] + frameLength;
			shiftLength = data->outSignal->length - frameLength;
		}
		outPtr = p->dataBuffer->outSignal->channel[chan];
		outStrobePtr = p->strobeDataBuffer->outSignal->channel[chan];
		for (i = 0; i < shiftLength; i++) {
			*outPtr++ = *inPtr++;
			*outStrobePtr++ = *inStrobePtr++;
		}
		if (frameLength < data->outSignal->length) {
			inPtr = data->inSignal[0]->channel[chan] + inputCount;
			inStrobePtr = p->strobeData->outSignal->channel[chan] + inputCount;
			for (i = 0; i < frameLength; i++) {
				*outPtr++ = *inPtr++;
				*outStrobePtr++ = *inStrobePtr++;
			}
		}
	}

}

/**************************** DecayImage **************************************/

/*
 * This routine applies a decay to the image buffer, dependent upon the
 * 'decayCount' for each channel.
 */

void
DecayImage_Analysis_SAI(EarObjectPtr data, int chan)
{
	register ChanData	*dataPtr, scale;
	ChanLen		i;
	
	if (!sAImagePtr->decayCount[chan])
		return;
	scale = exp(-LN_2 / sAImagePtr->imageDecayHalfLife * sAImagePtr->decayCount[
	  chan] * data->inSignal[0]->dt);
	dataPtr = data->outSignal->channel[chan];
	for (i = 0; i < data->outSignal->length; i++)
		*dataPtr++ *= scale;
	sAImagePtr->decayCount[chan] = 0;

}

/**************************** ProcessFrameSection *****************************/

/*
 * This routine carries out the SAI image process.
 * The values in the data buffer have already been scaled, but all values
 * used from the input signal must be scaled.
 * The routine decays the SAI data according to the duration of the input
 * signal duration.
 * It only applies the scale when a full frame has been processed.
 */

void
ProcessFrameSection_Analysis_SAI(EarObjectPtr data, ChanData **strobeStatePtrs,
  ChanData **dataPtrs, ChanLen strobeOffset, ChanLen frameOffset,
  ChanLen sectionLength)
{
	/* static const char	*funcName = "ProcessFrameSection_Analysis_SAI"; */
	register ChanData	*inPtr, *outPtr, *strobeStatePtr, scaler;
	int		chan;
	double	*inputDecayPtr;
	ChanLen	i, j, sectionEnd, inputCount;
	SAImagePtr	p = sAImagePtr;

	if (sectionLength == 0)
		return;
	inputCount = *(p->inputCount + data->threadIndex);
	sectionEnd = frameOffset + sectionLength;
	for (chan = data->outSignal->offset; chan < data->outSignal->numChannels;
	  chan++) {
		strobeStatePtr = strobeStatePtrs[chan] + strobeOffset;
		for (i = frameOffset; i < sectionEnd; i++, strobeStatePtr++) {
			if (*strobeStatePtr > 0.0) {
				DecayImage_Analysis_SAI(data, chan);
				inPtr = p->dataBuffer->outSignal->channel[chan] + i;
				outPtr = data->outSignal->channel[chan];
				inputDecayPtr = p->inputDecay;
				switch (p->integrationMode) {
				case SAI_INTEGRATION_MODE_STI:
					for (j = i; j < data->outSignal->length; j++)
						*outPtr++ += *inPtr++ * *inputDecayPtr++;
					inPtr = data->inSignal[0]->channel[chan] + inputCount;
					for (j = 0; j < i; j++)
						*outPtr++ += *inPtr++ * *inputDecayPtr++;
					break;
				case SAI_INTEGRATION_MODE_AC:
					scaler = *(dataPtrs[chan] + strobeOffset + i - frameOffset);
					for (j = i; j < data->outSignal->length; j++)
						*outPtr++ += *inPtr++ * *inputDecayPtr++ * scaler;
					inPtr = data->inSignal[0]->channel[chan] + inputCount;
					for (j = 0; j < i; j++)
						*outPtr++ += *inPtr++ * *inputDecayPtr++ * scaler;
					break;
				default:
					;
				}
			}
			p->decayCount[chan]++;
		}
	}

}

/****************************** Process ***************************************/

/*
 * This routine allocates memory for the output signal, if necessary,
 * and generates the signal into channel[0] of the signal data-set.
 * It checks that all initialisation has been correctly carried out by
 * calling the appropriate checking routines.
 * It can be called repeatedly with different parameter values if required.
 * Stimulus generation only sets the output signal, the input signal
 * is not used.
 * With repeated calls the Signal memory is only allocated once, then re-used.
 * The signalData process is "Manually" connected to the appropriate input
 * signal.
 * The width of the frame must be "(positive - negative) / dt + 1".  The
 * addtional 1 is necessary because of the zero.  Thank you God, for helping me
 * to find that.
 */

BOOLN
Process_Analysis_SAI(EarObjectPtr data)
{
	static const char	*funcName = "Process_Analysis_SAI";
	BOOLN	endOfData;
	int		chan;
	double	dt;
	ChanLen	frameLength, positiveWidthIndex, negativeWidthIndex, *inputCountPtr;
	SAImagePtr	p = sAImagePtr;
	EarObjectPtr	strobeData;

	if (!data->threadRunFlag) {
		if (!CheckPars_Analysis_SAI())
			return(FALSE);
		if (!CheckData_Analysis_SAI(data)) {
			NotifyError("%s: Process data invalid.", funcName);
			return(FALSE);
		}
		SetProcessName_EarObject(data, "Stabilised Auditory Image Module "
		  "process");
		dt = data->inSignal[0]->dt;
		if (!InitOutSignal_EarObject(data, data->inSignal[0]->numChannels, 
		  (ChanLen) floor((p->positiveWidth - p->negativeWidth) / dt + 1.5),
		  dt)) {
			NotifyError("%s: Cannot initialise output channels.", funcName);
			return(FALSE);
		}
		SetStaticTimeFlag_SignalData(data->outSignal, TRUE);
		SetOutputTimeOffset_SignalData(data->outSignal, p->negativeWidth);
		if (!InitProcessVariables_Analysis_SAI(data)) {
			NotifyError("%s: Could not initialise the process variables.",
			  funcName);
			return(FALSE);
		}
		TempInputConnection_EarObject(data, p->strobeData, 1);
		p->strobeData->inSignal[0] = data->inSignal[p->strobeInSignalIndex];
		if (p->diagnosticMode)
			OutputStrobeData_Analysis_SAI();
		if (data->initThreadRunFlag)
			return(TRUE);
	}
	strobeData = data->subProcessList[SAI_STROBEDATA];
	if (!RunProcessStandard_ModuleMgr(strobeData)) {
		NotifyError("%s: Could not process strobe data .", funcName);
		return(FALSE);
	}
	inputCountPtr = p->inputCount + data->threadIndex;
	positiveWidthIndex = p->positiveWidthIndex - strobePtr->numLastSamples;
	negativeWidthIndex = p->zeroIndex + strobePtr->numLastSamples;
	for (frameLength = data->outSignal->length, endOfData = FALSE;
	  !endOfData; ) {
		if (*inputCountPtr + data->outSignal->length > data->inSignal[0]->
		  length) {
			endOfData = TRUE;
			frameLength = data->inSignal[0]->length - *inputCountPtr;
			if (frameLength < positiveWidthIndex) {
				positiveWidthIndex = frameLength;
				negativeWidthIndex = 0;
			} else if ((frameLength - positiveWidthIndex) < negativeWidthIndex)
				negativeWidthIndex = frameLength - positiveWidthIndex;
		}
		ProcessFrameSection_Analysis_SAI(data, p->strobeDataBuffer->outSignal->
		  channel, p->dataBuffer->outSignal->channel, p->zeroIndex +
		  strobePtr->numLastSamples, 0, positiveWidthIndex);
		ProcessFrameSection_Analysis_SAI(data, strobeData->outSignal->
		  channel, data->inSignal[0]->channel, *inputCountPtr,
		  positiveWidthIndex, negativeWidthIndex);
		PushBufferData_Analysis_SAI(data, frameLength);
		*inputCountPtr = (endOfData)? 0: *inputCountPtr + frameLength;
	}
	 for (chan = data->outSignal->offset; chan < data->outSignal->numChannels;
	  chan++)
		DecayImage_Analysis_SAI(data, chan);
	data->outSignal->numWindowFrames++;
	SetProcessContinuity_EarObject(data);
	return(TRUE);

}
