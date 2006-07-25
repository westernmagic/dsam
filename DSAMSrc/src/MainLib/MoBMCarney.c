/**********************
 *
 * File:		MoBMCarney.c
 * Purpose:		Laurel H. Carney basilar membrane module: Carney L. H. (1993)
 *				"A model for the responses of low-frequency auditory-nerve
 *				fibers in cat", JASA, 93, pp 401-417.
 * Comments:	Written using ModuleProducer version 1.9 (Feb 29 1996).
 *				21-03-97 LPO: Corrected binaural processing bug.
 * Authors:		L. P. O'Mard modified from L. H. Carney's code
 * Created:		12 Mar 1996
 * Updated:		21 Mar 1997
 * Copyright:	(c) 1997, University of Essex.
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
#include "UtString.h"
#include "MoBMCarney.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

BMCarneyPtr	bMCarneyPtr = NULL;

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
Free_BasilarM_Carney(void)
{
	if (bMCarneyPtr == NULL)
		return(FALSE);
	FreeProcessVariables_BasilarM_Carney();
	Free_CFList(&bMCarneyPtr->cFList);
	if (bMCarneyPtr->parList)
		FreeList_UniParMgr(&bMCarneyPtr->parList);
	if (bMCarneyPtr->parSpec == GLOBAL) {
		free(bMCarneyPtr);
		bMCarneyPtr = NULL;
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
Init_BasilarM_Carney(ParameterSpecifier parSpec)
{
	static const WChar	*funcName = wxT("Init_BasilarM_Carney");

	if (parSpec == GLOBAL) {
		if (bMCarneyPtr != NULL)
			Free_BasilarM_Carney();
		if ((bMCarneyPtr = (BMCarneyPtr) malloc(sizeof(BMCarney))) == NULL) {
			NotifyError(wxT("%s: Out of memory for 'global' pointer"), funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (bMCarneyPtr == NULL) {
			NotifyError(wxT("%s:  'local' pointer not set."), funcName);
			return(FALSE);
		}
	}
	bMCarneyPtr->parSpec = parSpec;
	bMCarneyPtr->updateProcessVariablesFlag = TRUE;
	bMCarneyPtr->cascadeFlag = TRUE;
	bMCarneyPtr->cutOffFrequencyFlag = TRUE;
	bMCarneyPtr->hCOperatingPointFlag = TRUE;
	bMCarneyPtr->asymmetricalBiasFlag = TRUE;
	bMCarneyPtr->maxHCVoltageFlag = TRUE;
	bMCarneyPtr->cascade = 4;
	bMCarneyPtr->cutOffFrequency = 1100.0;
	bMCarneyPtr->hCOperatingPoint = 5000.0;
	bMCarneyPtr->asymmetricalBias = 0.462;
	bMCarneyPtr->maxHCVoltage = 10.0;
	if ((bMCarneyPtr->cFList = GenerateDefault_CFList(
	  CFLIST_DEFAULT_MODE_NAME, CFLIST_DEFAULT_CHANNELS, 200.0, 5000.0,
	  wxT("internal_dynamic"), CFLIST_DEFAULT_BW_MODE_FUNC)) == NULL) {
		NotifyError(wxT("%s: could not set default CFList."), funcName);
		return(FALSE);
	}

	if (!SetUniParList_BasilarM_Carney()) {
		NotifyError(wxT("%s: Could not initialise parameter list."), funcName);
		Free_BasilarM_Carney();
		return(FALSE);
	}
	bMCarneyPtr->coefficients = NULL;
	bMCarneyPtr->f = NULL;
	return(TRUE);

}

/****************************** SetUniParList *********************************/

/*
 * This function initialises and sets the module's universal parameter
 * list. This list provides universal access to the module's
 * parameters.  It expects to be called from the 'Init_' routine.
 */

BOOLN
SetUniParList_BasilarM_Carney(void)
{
	static const WChar *funcName = wxT("SetUniParList_BasilarM_Carney");
	UniParPtr	pars;

	if ((bMCarneyPtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  BASILARM_CARNEY_NUM_PARS, NULL)) == NULL) {
		NotifyError(wxT("%s: Could not initialise parList."), funcName);
		return(FALSE);
	}
	pars = bMCarneyPtr->parList->pars;
	SetPar_UniParMgr(&pars[BASILARM_CARNEY_CASCADE], wxT("CASCADE"),
	  wxT("Filter cascade."),
	  UNIPAR_INT,
	  &bMCarneyPtr->cascade, NULL,
	  (void * (*)) SetCascade_BasilarM_Carney);
	SetPar_UniParMgr(&pars[BASILARM_CARNEY_CUTOFFFREQUENCY], wxT("FC"),
	  wxT("Cut-off frequency for OHC low-pass filter, Fc (Hz)."),
	  UNIPAR_REAL,
	  &bMCarneyPtr->cutOffFrequency, NULL,
	  (void * (*)) SetCutOffFrequency_BasilarM_Carney);
	SetPar_UniParMgr(&pars[BASILARM_CARNEY_HCOPERATINGPOINT], wxT("P_DFB"),
	  wxT("Operating point of OHC (feedback) non-linearity, P_Dfb (Pa)."),
	  UNIPAR_REAL,
	  &bMCarneyPtr->hCOperatingPoint, NULL,
	  (void * (*)) SetHCOperatingPoint_BasilarM_Carney);
	SetPar_UniParMgr(&pars[BASILARM_CARNEY_ASYMMETRICALBIAS], wxT("P0"),
	  wxT("Asymmetrical bias for OHC non-linearity, P0 (rad)."),
	  UNIPAR_REAL,
	  &bMCarneyPtr->asymmetricalBias, NULL,
	  (void * (*)) SetAsymmetricalBias_BasilarM_Carney);
	SetPar_UniParMgr(&pars[BASILARM_CARNEY_MAXHCVOLTAGE], wxT("V_MAX"),
	  wxT("Maximum depolarising hair cell voltage, Vmax (V)."),
	  UNIPAR_REAL,
	  &bMCarneyPtr->maxHCVoltage, NULL,
	  (void * (*)) SetMaxHCVoltage_BasilarM_Carney);
	SetPar_UniParMgr(&pars[BASILARM_CARNEY_CFLIST], wxT("CFLIST"),
	  wxT(""),
	  UNIPAR_CFLIST,
	  &bMCarneyPtr->cFList, NULL,
	  (void * (*)) SetCFList_BasilarM_Carney);
	return(TRUE);

}

/****************************** GetUniParListPtr ******************************/

/*
 * This function returns a pointer to the module's universal parameter
 * list.
 */

UniParListPtr
GetUniParListPtr_BasilarM_Carney(void)
{
	static const WChar	*funcName = wxT("GetUniParListPtr_BasilarM_Carney");

	if (bMCarneyPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (bMCarneyPtr->parList == NULL) {
		NotifyError(wxT("%s: UniParList data structure has not been "
		  "initialised. NULL returned."), funcName);
		return(NULL);
	}
	return(bMCarneyPtr->parList);

}

/****************************** SetPars ***************************************/

/*
 * This function sets all the module's parameters.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetPars_BasilarM_Carney(int cascade, double cutOffFrequency,
  double hCOperatingPoint, double asymmetricalBias, double maxHCVoltage,
  CFListPtr cFList)
{
	static const WChar	*funcName = wxT("SetPars_BasilarM_Carney");
	BOOLN	ok;

	ok = TRUE;
	if (!SetCascade_BasilarM_Carney(cascade))
		ok = FALSE;
	if (!SetCutOffFrequency_BasilarM_Carney(cutOffFrequency))
		ok = FALSE;
	if (!SetHCOperatingPoint_BasilarM_Carney(hCOperatingPoint))
		ok = FALSE;
	if (!SetAsymmetricalBias_BasilarM_Carney(asymmetricalBias))
		ok = FALSE;
	if (!SetMaxHCVoltage_BasilarM_Carney(maxHCVoltage))
		ok = FALSE;
	if (!SetCFList_BasilarM_Carney(cFList))
		ok = FALSE;
	if (!ok)
		NotifyError(wxT("%s: Failed to set all module parameters.") ,funcName);
	return(ok);

}

/****************************** SetCascade ************************************/

/*
 * This function sets the module's cascade parameter.
 * It returns TRUE if the operation is successful.
 * Because of the way some of the code works, (e.g. [i - 1] used) the filter
 * cascade must be greater than 
 */

BOOLN
SetCascade_BasilarM_Carney(int theCascade)
{
	static const WChar	*funcName = wxT("SetCascade_BasilarM_Carney");

	if (bMCarneyPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (theCascade < 1) {
		NotifyError(wxT("%s: The cascade must be greater than 0."), funcName);
		return(FALSE);
	}
	bMCarneyPtr->cascadeFlag = TRUE;
	bMCarneyPtr->updateProcessVariablesFlag = TRUE;
	bMCarneyPtr->cascade = theCascade;
	return(TRUE);

}

/****************************** SetCutOffFrequency ****************************/

/*
 * This function sets the module's cutOffFrequency parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetCutOffFrequency_BasilarM_Carney(double theCutOffFrequency)
{
	static const WChar	*funcName = wxT("SetCutOffFrequency_BasilarM_Carney");

	if (bMCarneyPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	bMCarneyPtr->cutOffFrequencyFlag = TRUE;
	bMCarneyPtr->updateProcessVariablesFlag = TRUE;
	bMCarneyPtr->cutOffFrequency = theCutOffFrequency;
	return(TRUE);

}

/****************************** SetHCOperatingPoint **************************/

/*
 * This function sets the module's hCOperatingPoint parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetHCOperatingPoint_BasilarM_Carney(double theHCOperatingPoint)
{
	static const WChar	*funcName = wxT("SetHCOperatingPoint_BasilarM_Carney");

	if (bMCarneyPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	bMCarneyPtr->hCOperatingPointFlag = TRUE;
	bMCarneyPtr->updateProcessVariablesFlag = TRUE;
	bMCarneyPtr->hCOperatingPoint = theHCOperatingPoint;
	return(TRUE);

}

/****************************** SetAsymmetricalBias ***************************/

/*
 * This function sets the module's asymmetricalBias parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetAsymmetricalBias_BasilarM_Carney(double theAsymmetricalBias)
{
	static const WChar	*funcName = wxT("SetAsymmetricalBias_BasilarM_Carney");

	if (bMCarneyPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	bMCarneyPtr->asymmetricalBiasFlag = TRUE;
	bMCarneyPtr->updateProcessVariablesFlag = TRUE;
	bMCarneyPtr->asymmetricalBias = theAsymmetricalBias;
	return(TRUE);

}

/****************************** SetMaxHCVoltage *******************************/

/*
 * This function sets the module's maxHCVoltage parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetMaxHCVoltage_BasilarM_Carney(double theMaxHCVoltage)
{
	static const WChar	*funcName = wxT("SetMaxHCVoltage_BasilarM_Carney");

	if (bMCarneyPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	bMCarneyPtr->maxHCVoltageFlag = TRUE;
	bMCarneyPtr->updateProcessVariablesFlag = TRUE;
	bMCarneyPtr->maxHCVoltage = theMaxHCVoltage;
	return(TRUE);

}

/****************************** SetCFList *************************************/

/*
 * This function sets the module's cFList parameter.
 * This function also sets the bandwidth mode to wxT("internal"), as the
 * bandwidth is variable, controlled by the main process.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetCFList_BasilarM_Carney(CFListPtr theCFList)
{
	static const WChar	*funcName = wxT("SetCFList_BasilarM_Carney");

	if (bMCarneyPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (!CheckPars_CFList(theCFList)) {
		NotifyError(wxT("%s: Centre frequency structure not initialised "
		  "correctly set."), funcName);
		return(FALSE);
	}
	if (!SetBandwidths_CFList(theCFList, wxT("internal_dynamic"), NULL)) {
		NotifyError(wxT("%s: Failed to set bandwidth mode."), funcName);
		return(FALSE);
	}
	theCFList->bParList->pars[BANDWIDTH_PAR_MODE].enabled = FALSE;
	if (bMCarneyPtr->cFList != NULL)
		Free_CFList(&bMCarneyPtr->cFList);
	bMCarneyPtr->cFList = theCFList;
	bMCarneyPtr->updateProcessVariablesFlag = TRUE;
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
CheckPars_BasilarM_Carney(void)
{
	static const WChar	*funcName = wxT("CheckPars_BasilarM_Carney");
	BOOLN	ok;

	ok = TRUE;
	if (bMCarneyPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (!bMCarneyPtr->cascadeFlag) {
		NotifyError(wxT("%s: cascade variable not set."), funcName);
		ok = FALSE;
	}
	if (!bMCarneyPtr->cutOffFrequencyFlag) {
		NotifyError(wxT("%s: cutOffFrequency variable not set."), funcName);
		ok = FALSE;
	}
	if (!bMCarneyPtr->hCOperatingPointFlag) {
		NotifyError(wxT("%s: hCOperatingPoint variable not set."), funcName);
		ok = FALSE;
	}
	if (!bMCarneyPtr->asymmetricalBiasFlag) {
		NotifyError(wxT("%s: asymmetricalBias variable not set."), funcName);
		ok = FALSE;
	}
	if (!bMCarneyPtr->maxHCVoltageFlag) {
		NotifyError(wxT("%s: maxHCVoltage variable not set."), funcName);
		ok = FALSE;
	}
	if (!CheckPars_CFList(bMCarneyPtr->cFList)) {
		NotifyError(wxT("%s: Centre frequency list  parameters not correctly "
		  "set."), funcName);
		ok = FALSE;
	}
	return(ok);

}

/****************************** GetCFListPtr **********************************/

/*
 * This routine returns a pointer to the module's CFList data pointer.
 *
 */

CFListPtr
GetCFListPtr_BasilarM_Carney(void)
{
	static const WChar *funcName = wxT("GetCFListPtr_BasilarM_GCarney");

	if (bMCarneyPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (bMCarneyPtr->cFList == NULL) {
		NotifyError(wxT("%s: CFList data structure has not been correctly set.  ")\
		  wxT("NULL returned."), funcName);
		return(NULL);
	}
	return(bMCarneyPtr->cFList);

}

/****************************** PrintPars *************************************/

/*
 * This routine prints all the module's parameters to the file stream.
 * specified by the dSAM.parsFile file pointer.
 */

BOOLN
PrintPars_BasilarM_Carney(void)
{
	static const WChar	*funcName = wxT("PrintPars_BasilarM_Carney");

	if (!CheckPars_BasilarM_Carney()) {
		NotifyError(wxT("%s: Parameters have not been correctly set."), funcName);
		return(FALSE);
	}
	DPrint(wxT("Carney Non-Linear Basilar Membrane Module Parameters:-\n"));
	PrintPars_CFList(bMCarneyPtr->cFList);
	DPrint(wxT("\tFilter cascade = %d,\n"), bMCarneyPtr->cascade);
	DPrint(wxT("\tLow-pass cut-off frequency, Fc = %g (Hz),\n"),
	  bMCarneyPtr->cutOffFrequency);
	DPrint(wxT("\tOHC operating point, P_Dfb = %g (uPa),\n"),
	  bMCarneyPtr->hCOperatingPoint);
	DPrint(wxT("\tAsymmetrical bias (OHC non-linearity), P0 = %g (rad.)\n"),
	  bMCarneyPtr->asymmetricalBias);
	DPrint(wxT("\tMax, HC voltage, Vmax = %g V,\n"), bMCarneyPtr->maxHCVoltage);
	return(TRUE);

}

/****************************** ReadPars **************************************/

/*
 * This program reads a specified number of parameters from a file.
 * It returns FALSE if it fails in any way.n */

BOOLN
ReadPars_BasilarM_Carney(WChar *fileName)
{
	static const WChar	*funcName = wxT("ReadPars_BasilarM_Carney");
	BOOLN	ok;
	WChar	*filePath;
	int		cascade;
	double	cutOffFrequency, hCOperatingPoint, asymmetricalBias, maxHCVoltage;
	CFListPtr	cFList;
	FILE	*fp;

	filePath = GetParsFileFPath_Common(fileName);
	if ((fp = DSAM_fopen(filePath, "r")) == NULL) {
		NotifyError(wxT("%s: Cannot open data file '%s'.\n"), funcName,
		  filePath);
		return(FALSE);
	}
	DPrint(wxT("%s: Reading from '%s':\n"), funcName, filePath);
	Init_ParFile();
	ok = TRUE;
	if (!GetPars_ParFile(fp, wxT("%d"), &cascade))
		ok = FALSE;
	if ((cFList = ReadPars_CFList(fp)) == NULL)
		ok = FALSE;
	if (!GetPars_ParFile(fp, wxT("%lf"), &cutOffFrequency))
		ok = FALSE;
	if (!GetPars_ParFile(fp, wxT("%lf"), &hCOperatingPoint))
		ok = FALSE;
	if (!GetPars_ParFile(fp, wxT("%lf"), &asymmetricalBias))
		ok = FALSE;
	if (!GetPars_ParFile(fp, wxT("%lf"), &maxHCVoltage))
		ok = FALSE;
	fclose(fp);
	Free_ParFile();
	if (!ok) {
		NotifyError(wxT("%s: Not enough lines, or invalid parameters, in "
		  "module parameter file '%s'."), funcName, filePath);
		return(FALSE);
	}
	if (!SetPars_BasilarM_Carney(cascade, cutOffFrequency, hCOperatingPoint,
	  asymmetricalBias, maxHCVoltage, cFList)) {
		NotifyError(wxT("%s: Could not set parameters."), funcName);
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
SetParsPointer_BasilarM_Carney(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("SetParsPointer_BasilarM_Carney");

	if (!theModule) {
		NotifyError(wxT("%s: The module is not set."), funcName);
		return(FALSE);
	}
	bMCarneyPtr = (BMCarneyPtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 */

BOOLN
InitModule_BasilarM_Carney(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("InitModule_BasilarM_Carney");

	if (!SetParsPointer_BasilarM_Carney(theModule)) {
		NotifyError(wxT("%s: Cannot set parameters pointer."), funcName);
		return(FALSE);
	}
	if (!Init_BasilarM_Carney(GLOBAL)) {
		NotifyError(wxT("%s: Could not initialise process structure."),
		  funcName);
		return(FALSE);
	}
	theModule->parsPtr = bMCarneyPtr;
	theModule->threadMode = MODULE_THREAD_MODE_SIMPLE;
	theModule->CheckPars = CheckPars_BasilarM_Carney;
	theModule->Free = Free_BasilarM_Carney;
	theModule->GetUniParListPtr = GetUniParListPtr_BasilarM_Carney;
	theModule->PrintPars = PrintPars_BasilarM_Carney;
	theModule->ReadPars = ReadPars_BasilarM_Carney;
	theModule->RunProcess = RunModel_BasilarM_Carney;
	theModule->SetParsPointer = SetParsPointer_BasilarM_Carney;
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
CheckData_BasilarM_Carney(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("CheckData_BasilarM_Carney");

	if (data == NULL) {
		NotifyError(wxT("%s: EarObject not initialised."), funcName);
		return(FALSE);
	}
	if (!CheckInSignal_EarObject(data, funcName))
		return(FALSE);
	if (!CheckRamp_SignalData(data->inSignal[0])) {
		NotifyError(wxT("%s: Input signal not correctly initialised."),
		  funcName);
		return(FALSE);
	}
	/*** Put additional checks here. ***/
	return(TRUE);

}

/**************************** FreeProcessVariables ****************************/

/*
 * This routine releases the memory allocated for the process variables
 * if they have been initialised.
 */

BOOLN
FreeProcessVariables_BasilarM_Carney(void)
{
	int		i;

	if (bMCarneyPtr->coefficients) {
		for (i = 0; i < bMCarneyPtr->numChannels; i++)
    		FreeCarneyGTCoeffs_BasilarM_Carney(&bMCarneyPtr->coefficients[i]);
		free(bMCarneyPtr->coefficients);
		bMCarneyPtr->coefficients = NULL;
	}
	if (bMCarneyPtr->f) {
		for (i = 0; i < bMCarneyPtr->numThreads; i++)
			free(bMCarneyPtr->f[i]);
		free(bMCarneyPtr->f);
		bMCarneyPtr->f= NULL;
	}
	bMCarneyPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);
}

/**************************** FreeCarneyGTCoeffs ******************************/

/*
 * This routine frees the space allocated for the coefficients.
 */

void
FreeCarneyGTCoeffs_BasilarM_Carney(CarneyGTCoeffsPtr *p)
{
	if (*p == NULL)
		return;
	if ((*p)->fLast)
		free((*p)->fLast);
	free(*p);
	*p = NULL;

}

/**************************** InitCarneyGTCoeffs ******************************/

/*
 * This routine initialises the Carney non-linear gamma tone coefficients,
 * and prepares space for the appropriate arrays.
 * Parameter 'x' is the position of cf unit; from Liberman's map.
 */

CarneyGTCoeffsPtr
InitCarneyGTCoeffs_BasilarM_Carney(int cascade, double cF)
{
	static const WChar *funcName = wxT("InitCarneyGTCoeffs_BasilarM_Carney");
	CarneyGTCoeffsPtr p;
	
	if ((p = (CarneyGTCoeffsPtr) malloc(sizeof(CarneyGTCoeffs))) == NULL) {
		NotifyError(wxT("%s: Out of memory for coefficient."), funcName);
		return(NULL);
	}
	if ((p->fLast = (Complex *) calloc(cascade + 1, sizeof(Complex))) == NULL) {
		NotifyError(wxT("%s: Out of memory for 'fLast' array (%d elements)."),
		  funcName, cascade);
		FreeCarneyGTCoeffs_BasilarM_Carney(&p);
		return(NULL);
	}
	p->x = BANDWIDTH_CARNEY_X(cF);
	p->tau0 = ( BM_CARNEY_CC0 * exp( -p->x / BM_CARNEY_SS0) + BM_CARNEY_CC1 *
	  exp( -p->x / BM_CARNEY_SS1) );
	p->oHCLast = 0.0;
	p->oHCTempLast = 0.0;
	return(p);

}

/**************************** InitProcessVariables ****************************/

/*
 * This function allocates the memory for the process variables.
 */

BOOLN
InitProcessVariables_BasilarM_Carney(EarObjectPtr data)
{
	static const WChar *funcName = wxT("InitProcessVariables_BasilarM_Carney");
	int		i, j, cFIndex, chan;
	BMCarneyPtr	p = bMCarneyPtr;

	if (p->updateProcessVariablesFlag || data->updateProcessFlag ||
	  p->cFList->updateFlag || (data->timeIndex == PROCESS_START_TIME)) {
		if (p->updateProcessVariablesFlag || data->updateProcessFlag ||
		  p->cFList->updateFlag) {
			FreeProcessVariables_BasilarM_Carney();
			p->numThreads = data->numThreads;
			p->numChannels = _OutSig_EarObject(data)->numChannels;
			if ((p->coefficients = (CarneyGTCoeffsPtr *) calloc(p->numChannels,
			   sizeof(CarneyGTCoeffsPtr))) == NULL) {
		 		NotifyError(wxT("%s: Out of memory for coefficients array."),
		 		  funcName);
		 		return(FALSE);
			}
			for (i = 0; i < _OutSig_EarObject(data)->numChannels; i++) {
				cFIndex = i / data->inSignal[0]->interleaveLevel;
				if ((p->coefficients[i] = InitCarneyGTCoeffs_BasilarM_Carney(
				  p->cascade, p->cFList->frequency[cFIndex])) == NULL) {
					NotifyError(wxT("%s: Out of memory for coefficient (%d)."),
					  funcName, i);
					FreeProcessVariables_BasilarM_Carney();
					return(FALSE);
				}
			}
			p->numComplexCoeffs = p->cascade + 1;
			if ((p->f = (ComplexPtr *) calloc(p->numThreads, sizeof(
			  ComplexPtr))) == NULL) {
				NotifyError(wxT("%s: Out of memory for 'f' array pointers."),
				  funcName);
				return(FALSE);
			}
			for (i = 0; i < p->numThreads; i++)
				if ((p->f[i] = (Complex *) calloc(p->numComplexCoeffs,
				  sizeof(Complex))) == NULL) {
					NotifyError(wxT("%s: Out of memory for 'f[%d]' array (%d "
					  "elements)."), funcName, i, p->numComplexCoeffs);
					FreeProcessVariables_BasilarM_Carney();
					return(FALSE);
				}
			SetLocalInfoFlag_SignalData(_OutSig_EarObject(data), TRUE);
			SetInfoChannelTitle_SignalData(_OutSig_EarObject(data), wxT("Frequency "
			  "(Hz)"));
			SetInfoChannelLabels_SignalData(_OutSig_EarObject(data),
			   p->cFList->frequency);
			SetInfoCFArray_SignalData(_OutSig_EarObject(data), p->cFList->frequency);
			p->updateProcessVariablesFlag = FALSE;
			p->cFList->updateFlag = FALSE;
		}
		p->pix2xDt = PIx2 * data->inSignal[0]->dt;
		for (i = 0; i < _OutSig_EarObject(data)->numChannels; i++) {
			chan = i % data->inSignal[0]->interleaveLevel;
			cFIndex = i / data->inSignal[0]->interleaveLevel;
			RThetaSet_CmplxM(data->inSignal[0]->channel[chan][0], -p->pix2xDt *
			  p->cFList->frequency[cFIndex], &p->coefficients[i]->fLast[0]);
			for (j = 1; j < p->cascade + 1; j++)
				RThetaSet_CmplxM(0.0, 0.0, &p->coefficients[i]->fLast[j]);
			p->coefficients[i]->oHCLast = 0.0;
			p->coefficients[i]->oHCTempLast = 0.0;
		}
				
	}
	return(TRUE);

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
RunModel_BasilarM_Carney(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("RunModel_BasilarM_Carney");
	register	ChanData	 *inPtr, *outPtr;
	uShort	totalChannels;
	int		j, chan, cFIndex;
	double	aCoeff, pix2xDtxCF, cF, fF, bCoeff, oHCTemp, oHC;
	ChanLen	i;
	Complex	z, *f;
	BMCarneyPtr	p = bMCarneyPtr;
	CarneyGTCoeffsPtr	cC;

	if (!data->threadRunFlag) {
		if (!CheckPars_BasilarM_Carney())
			return(FALSE);
		if (!CheckData_BasilarM_Carney(data)) {
			NotifyError(wxT("%s: Process data invalid."), funcName);
			return(FALSE);
		}
		SetProcessName_EarObject(data, wxT("Carney non-linear basilar membrane "
		  "filtering"));
		totalChannels = p->cFList->numChannels * data->inSignal[0]->numChannels;
		if (!InitOutTypeFromInSignal_EarObject(data, totalChannels)) {
			NotifyError(wxT("%s: Could not initialise output channels."),
			  funcName);
			return(FALSE);
		}
		if (!InitProcessVariables_BasilarM_Carney(data)) {
			NotifyError(wxT("%s: Could not initialise the process variables."),
			  funcName);
			return(FALSE);
		}
		/* Main Processing. */
		p->c = 2.0 / data->inSignal[0]->dt;
		p->aA = p->maxHCVoltage / (1.0 + tanh(p->asymmetricalBias));
		p->c1LP = (p->c - PIx2 * p->cutOffFrequency) / (p->c + PIx2 *
		  p->cutOffFrequency);
		p->c2LP = PIx2 * p->cutOffFrequency / (PIx2 * p->cutOffFrequency +
		  p->c);
		if (data->initThreadRunFlag)
			return(TRUE);
	}
	InitOutDataFromInSignal_EarObject(data);
	f = p->f[data->threadIndex];
	for (chan = _OutSig_EarObject(data)->offset; chan < _OutSig_EarObject(data)->numChannels;
	  chan++) {
		cFIndex = chan / _OutSig_EarObject(data)->interleaveLevel;
		cC = *(p->coefficients + chan);
		aCoeff = BM_CARNEY_A(p->c, cC->tau0);
		cF = p->cFList->frequency[cFIndex];
		pix2xDtxCF = p->pix2xDt * cF;
		inPtr = data->inSignal[0]->channel[chan % data->inSignal[0]->
		  interleaveLevel];
		outPtr = _OutSig_EarObject(data)->channel[chan];
		for(i = 0; i < _OutSig_EarObject(data)->length; i++) {	
			/* FREQUENCY SHIFT THE ARRAY BUF  */
			RThetaSet_CmplxM(*inPtr++, -pix2xDtxCF * (i + 1), &f[0]);
			fF = (cC->tau0 * 3.0 / 2.0 - (cC->tau0 / 2.0) * (cC->oHCLast /
			  p->maxHCVoltage));
			bCoeff = p->c * fF - 1.0;
			for (j = 1; j < p->numComplexCoeffs; j++) {
				f[j] = cC->fLast[j];
				ScalerMult_CmplxM(&f[j], bCoeff);
				Add_CmplxM(&f[j], &f[j - 1], &f[j]);
				Add_CmplxM(&f[j], &cC->fLast[j - 1], &f[j]);
				ScalerMult_CmplxM(&f[j], aCoeff);
			}
			/* FREQUENCY SHIFT BACK UP  */
			RThetaSet_CmplxM(1.0, pix2xDtxCF * (i + 1), &z);
			Mult_CmplxM(&z, &f[p->cascade], &z);
			*outPtr++ = oHCTemp = z.re;

			/* Put through OHC saturating nonlinearity-  SO THAT 
			 * Output=.5*vmaxd AT wave=operating point   */
			oHCTemp = p->aA * (tanh(0.746 * oHCTemp / p->hCOperatingPoint -
			  p->asymmetricalBias) + tanh(p->asymmetricalBias));

			/* lowpass filter the feedback voltage   */
			oHC = p->c1LP * cC->oHCLast + p->c2LP * (oHCTemp + cC->oHCTempLast);

			/* save all loop parameters */
			for(j = 0; j < p->numComplexCoeffs; j++)
				cC->fLast[j] = f[j];
			cC->oHCLast = oHC;
			cC->oHCTempLast = oHCTemp;
		}    /* END of TIME LOOP */
	}
	SetProcessContinuity_EarObject(data);
	return(TRUE);

}

