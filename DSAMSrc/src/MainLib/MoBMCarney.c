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
	/* static const char	*funcName = "Free_BasilarM_Carney"; */

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
	static const char	*funcName = "Init_BasilarM_Carney";

	if (parSpec == GLOBAL) {
		if (bMCarneyPtr != NULL)
			Free_BasilarM_Carney();
		if ((bMCarneyPtr = (BMCarneyPtr) malloc(sizeof(BMCarney))) == NULL) {
			NotifyError("%s: Out of memory for 'global' pointer", funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (bMCarneyPtr == NULL) {
			NotifyError("%s:  'local' pointer not set.", funcName);
			return(FALSE);
		}
	}
	bMCarneyPtr->parSpec = parSpec;
	bMCarneyPtr->updateProcessVariablesFlag = TRUE;
	bMCarneyPtr->cascadeFlag = FALSE;
	bMCarneyPtr->cutOffFrequencyFlag = FALSE;
	bMCarneyPtr->hCOperatingPointFlag = FALSE;
	bMCarneyPtr->asymmetricalBiasFlag = FALSE;
	bMCarneyPtr->maxHCVoltageFlag = FALSE;
	bMCarneyPtr->cascade = 0;
	bMCarneyPtr->cutOffFrequency = 0.0;
	bMCarneyPtr->hCOperatingPoint = 0.0;
	bMCarneyPtr->asymmetricalBias = 0.0;
	bMCarneyPtr->maxHCVoltage = 0.0;
	bMCarneyPtr->cFList = NULL;

	if (!SetUniParList_BasilarM_Carney()) {
		NotifyError("%s: Could not initialise parameter list.", funcName);
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
	static const char *funcName = "SetUniParList_BasilarM_Carney";
	UniParPtr	pars;

	if ((bMCarneyPtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  BASILARM_CARNEY_NUM_PARS, NULL)) == NULL) {
		NotifyError("%s: Could not initialise parList.", funcName);
		return(FALSE);
	}
	pars = bMCarneyPtr->parList->pars;
	SetPar_UniParMgr(&pars[BASILARM_CARNEY_CASCADE], "CASCADE",
	  "Filter cascade.",
	  UNIPAR_INT,
	  &bMCarneyPtr->cascade, NULL,
	  (void * (*)) SetCascade_BasilarM_Carney);
	SetPar_UniParMgr(&pars[BASILARM_CARNEY_CUTOFFFREQUENCY], "FC",
	  "Cut-off frequency for OHC low-pass filter, Fc (Hz).",
	  UNIPAR_REAL,
	  &bMCarneyPtr->cutOffFrequency, NULL,
	  (void * (*)) SetCutOffFrequency_BasilarM_Carney);
	SetPar_UniParMgr(&pars[BASILARM_CARNEY_HCOPERATINGPOINT], "P_DFB",
	  "Operating point of OHC (feedback) non-linearity, P_Dfb (Pa).",
	  UNIPAR_REAL,
	  &bMCarneyPtr->hCOperatingPoint, NULL,
	  (void * (*)) SetHCOperatingPoint_BasilarM_Carney);
	SetPar_UniParMgr(&pars[BASILARM_CARNEY_ASYMMETRICALBIAS], "P0",
	  "Asymmetrical bias for OHC non-linearity, P0 (rad).",
	  UNIPAR_REAL,
	  &bMCarneyPtr->asymmetricalBias, NULL,
	  (void * (*)) SetAsymmetricalBias_BasilarM_Carney);
	SetPar_UniParMgr(&pars[BASILARM_CARNEY_MAXHCVOLTAGE], "V_MAX",
	  "Maximum depolarising hair cell voltage, Vmax (V).",
	  UNIPAR_REAL,
	  &bMCarneyPtr->maxHCVoltage, NULL,
	  (void * (*)) SetMaxHCVoltage_BasilarM_Carney);
	SetPar_UniParMgr(&pars[BASILARM_CARNEY_CFLIST], "CFLIST",
	  "",
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
	static const char	*funcName = "GetUniParListPtr_BasilarM_Carney";

	if (bMCarneyPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (bMCarneyPtr->parList == NULL) {
		NotifyError("%s: UniParList data structure has not been initialised. "
		  "NULL returned.", funcName);
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
	static const char	*funcName = "SetPars_BasilarM_Carney";
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
		NotifyError("%s: Failed to set all module parameters." ,funcName);
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
	static const char	*funcName = "SetCascade_BasilarM_Carney";

	if (bMCarneyPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (theCascade < 1) {
		NotifyError("%s: The cascade must be greater than 0.", funcName);
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
	static const char	*funcName = "SetCutOffFrequency_BasilarM_Carney";

	if (bMCarneyPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
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
	static const char	*funcName = "SetHCOperatingPoint_BasilarM_Carney";

	if (bMCarneyPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
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
	static const char	*funcName = "SetAsymmetricalBias_BasilarM_Carney";

	if (bMCarneyPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
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
	static const char	*funcName = "SetMaxHCVoltage_BasilarM_Carney";

	if (bMCarneyPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
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
 * This function also sets the bandwidth mode to "internal", as the bandwidth
 * is variable, controlled by the main process.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetCFList_BasilarM_Carney(CFListPtr theCFList)
{
	static const char	*funcName = "SetCFList_BasilarM_Carney";

	if (bMCarneyPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (!CheckPars_CFList(theCFList)) {
		NotifyError("%s: Centre frequency structure not initialised "\
		  "correctly set.", funcName);
		return(FALSE);
	}
	if (!SetBandwidths_CFList(theCFList, "internal_dynamic", NULL)) {
		NotifyError("%s: Failed to set bandwidth mode.", funcName);
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
	static const char	*funcName = "CheckPars_BasilarM_Carney";
	BOOLN	ok;

	ok = TRUE;
	if (bMCarneyPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (!bMCarneyPtr->cascadeFlag) {
		NotifyError("%s: cascade variable not set.", funcName);
		ok = FALSE;
	}
	if (!bMCarneyPtr->cutOffFrequencyFlag) {
		NotifyError("%s: cutOffFrequency variable not set.", funcName);
		ok = FALSE;
	}
	if (!bMCarneyPtr->hCOperatingPointFlag) {
		NotifyError("%s: hCOperatingPoint variable not set.", funcName);
		ok = FALSE;
	}
	if (!bMCarneyPtr->asymmetricalBiasFlag) {
		NotifyError("%s: asymmetricalBias variable not set.", funcName);
		ok = FALSE;
	}
	if (!bMCarneyPtr->maxHCVoltageFlag) {
		NotifyError("%s: maxHCVoltage variable not set.", funcName);
		ok = FALSE;
	}
	if (!CheckPars_CFList(bMCarneyPtr->cFList)) {
		NotifyError("%s: Centre frequency list  parameters not correctly set.",
		  funcName);
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
	static const char *funcName = "GetCFListPtr_BasilarM_GCarney";

	if (bMCarneyPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (bMCarneyPtr->cFList == NULL) {
		NotifyError("%s: CFList data structure has not been correctly set.  "\
		  "NULL returned.", funcName);
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
	static const char	*funcName = "PrintPars_BasilarM_Carney";

	if (!CheckPars_BasilarM_Carney()) {
		NotifyError("%s: Parameters have not been correctly set.", funcName);
		return(FALSE);
	}
	DPrint("Carney Non-Linear Basilar Membrane Module "
	  "Parameters:-\n");
	PrintPars_CFList(bMCarneyPtr->cFList);
	DPrint("\tFilter cascade = %d,\n", bMCarneyPtr->cascade);
	DPrint("\tLow-pass cut-off frequency, Fc = %g (Hz),\n",
	  bMCarneyPtr->cutOffFrequency);
	DPrint("\tOHC operating point, P_Dfb = %g (uPa),\n",
	  bMCarneyPtr->hCOperatingPoint);
	DPrint("\tAsymmetrical bias (OHC non-linearity), P0 = "
	  "%g (rad.)\n", bMCarneyPtr->asymmetricalBias);
	DPrint("\tMax, HC voltage, Vmax = %g V,\n",
	  bMCarneyPtr->maxHCVoltage);
	return(TRUE);

}

/****************************** ReadPars **************************************/

/*
 * This program reads a specified number of parameters from a file.
 * It returns FALSE if it fails in any way.n */

BOOLN
ReadPars_BasilarM_Carney(char *fileName)
{
	static const char	*funcName = "ReadPars_BasilarM_Carney";
	BOOLN	ok;
	char	*filePath;
	int		cascade;
	double	cutOffFrequency, hCOperatingPoint, asymmetricalBias, maxHCVoltage;
	CFListPtr	cFList;
	FILE	*fp;

	filePath = GetParsFileFPath_Common(fileName);
	if ((fp = fopen(filePath, "r")) == NULL) {
		NotifyError("%s: Cannot open data file '%s'.\n", funcName, filePath);
		return(FALSE);
	}
	DPrint("%s: Reading from '%s':\n", funcName, filePath);
	Init_ParFile();
	ok = TRUE;
	if (!GetPars_ParFile(fp, "%d", &cascade))
		ok = FALSE;
	if ((cFList = ReadPars_CFList(fp)) == NULL)
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &cutOffFrequency))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &hCOperatingPoint))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &asymmetricalBias))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &maxHCVoltage))
		ok = FALSE;
	fclose(fp);
	Free_ParFile();
	if (!ok) {
		NotifyError("%s: Not enough lines, or invalid parameters, in module "
		  "parameter file '%s'.", funcName, filePath);
		return(FALSE);
	}
	if (!SetPars_BasilarM_Carney(cascade, cutOffFrequency, hCOperatingPoint,
	  asymmetricalBias, maxHCVoltage, cFList)) {
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
SetParsPointer_BasilarM_Carney(ModulePtr theModule)
{
	static const char	*funcName = "SetParsPointer_BasilarM_Carney";

	if (!theModule) {
		NotifyError("%s: The module is not set.", funcName);
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
	static const char	*funcName = "InitModule_BasilarM_Carney";

	if (!SetParsPointer_BasilarM_Carney(theModule)) {
		NotifyError("%s: Cannot set parameters pointer.", funcName);
		return(FALSE);
	}
	if (!Init_BasilarM_Carney(GLOBAL)) {
		NotifyError("%s: Could not initialise process structure.", funcName);
		return(FALSE);
	}
	theModule->parsPtr = bMCarneyPtr;
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
 * The 'CheckRamp_SignalData()' can be used instead of the
 * 'CheckInit_SignalData()' routine if the signal must be ramped for
 * the process.
 */

BOOLN
CheckData_BasilarM_Carney(EarObjectPtr data)
{
	static const char	*funcName = "CheckData_BasilarM_Carney";

	if (data == NULL) {
		NotifyError("%s: EarObject not initialised.", funcName);
		return(FALSE);
	}
	if (!CheckInit_SignalData(data->inSignal[0], "CheckData_BasilarM_Carney"))
		return(FALSE);
	if (!CheckRamp_SignalData(data->inSignal[0])) {
		NotifyError("%s: Input signal not correctly initialised.", funcName);
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
	static const char *funcName = "InitCarneyGTCoeffs_BasilarM_Carney";
	CarneyGTCoeffsPtr p;
	
	if ((p = (CarneyGTCoeffsPtr) malloc(sizeof(CarneyGTCoeffs))) == NULL) {
		NotifyError("%s: Out of memory for coefficient.", funcName);
		return(NULL);
	}
	if ((p->fLast = (Complex *) calloc(cascade + 1, sizeof(Complex))) == NULL) {
		NotifyError("%s: Out of memory for 'fLast' array (%d elements).",
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
	static const char *funcName = "InitProcessVariables_BasilarM_Carney";
	int		i, j, cFIndex, numComplexCoeffs, chan;
	double	pix2xDt;
	BMCarneyPtr	p;
	
	p = bMCarneyPtr;
	if (p->updateProcessVariablesFlag || data->updateProcessFlag ||
	  p->cFList->updateFlag || (data->timeIndex == PROCESS_START_TIME)) {
		if (p->updateProcessVariablesFlag || data->updateProcessFlag ||
		  p->cFList->updateFlag) {
			FreeProcessVariables_BasilarM_Carney();
			if ((p->coefficients = (CarneyGTCoeffsPtr *) calloc(
			  data->outSignal->numChannels, sizeof(CarneyGTCoeffsPtr))) ==
			    NULL) {
		 		NotifyError("%s: Out of memory for coefficients array.",
		 		  funcName);
		 		return(FALSE);
			}
			p->numChannels = data->outSignal->numChannels;
			for (i = 0; i < data->outSignal->numChannels; i++) {
				cFIndex = i / data->inSignal[0]->interleaveLevel;
				if ((p->coefficients[i] = InitCarneyGTCoeffs_BasilarM_Carney(
				  p->cascade, p->cFList->frequency[cFIndex])) == NULL) {
					NotifyError("%s: Out of memory for coefficient (%d).",
					  funcName, i);
					FreeProcessVariables_BasilarM_Carney();
					return(FALSE);
				}
			}
			numComplexCoeffs = p->cascade + 1;
			if ((p->f = (Complex *) calloc(numComplexCoeffs,
			  sizeof(Complex))) == NULL) {
				NotifyError("%s: Out of memory for 'f' array (%d elements).",
				  funcName, numComplexCoeffs);
				FreeProcessVariables_BasilarM_Carney();
				return(FALSE);
			}
			SetLocalInfoFlag_SignalData(data->outSignal, TRUE);
			SetInfoChannelTitle_SignalData(data->outSignal, "Frequency (Hz)");
			SetInfoChannelLabels_SignalData(data->outSignal,
			   p->cFList->frequency);
			SetInfoCFArray_SignalData(data->outSignal, p->cFList->frequency);
			p->updateProcessVariablesFlag = FALSE;
			p->cFList->updateFlag = FALSE;
		}
		pix2xDt = PIx2 * data->inSignal[0]->dt;
		for (i = 0; i < data->outSignal->numChannels; i++) {
			chan = i % data->inSignal[0]->interleaveLevel;
			cFIndex = i / data->inSignal[0]->interleaveLevel;
			RThetaSet_CmplxM(data->inSignal[0]->channel[chan][0], -pix2xDt *
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
	static const char	*funcName = "RunModel_BasilarM_Carney";
	register	ChanData	 *inPtr, *outPtr;
	int		j, chan, totalChannels, numComplexCoeffs, cFIndex;
	double	c, c1LP, c2LP, aCoeff, pix2xDt, pix2xDtxCF, cF, fF;
	double	bCoeff, aA, oHCTemp, oHC;
	ChanLen	i;
	Complex	z;
	BMCarneyPtr	p;
	CarneyGTCoeffsPtr	cC;

	if (!CheckPars_BasilarM_Carney())
		return(FALSE);
	if (!CheckData_BasilarM_Carney(data)) {
		NotifyError("%s: Process data invalid.", funcName);
		return(FALSE);
	}
	SetProcessName_EarObject(data, "Carney non-linear basilar membrane "
	  "filtering");
	p = bMCarneyPtr;
	totalChannels = p->cFList->numChannels * data->inSignal[0]->numChannels;
	if (!InitOutFromInSignal_EarObject(data, totalChannels)) {
		NotifyError("%s: Could not initialise output channels.", funcName);
		return(FALSE);
	}
	if (!InitProcessVariables_BasilarM_Carney(data)) {
		NotifyError("%s: Could not initialise the process variables.",
		  funcName);
		return(FALSE);
	}
	/* Main Processing. */
	c = 2.0 / data->inSignal[0]->dt;
	aA = p->maxHCVoltage / (1.0 + tanh(p->asymmetricalBias));
	c1LP = (c - PIx2 * p->cutOffFrequency) / (c + PIx2 * p->cutOffFrequency);
	c2LP = PIx2 * p->cutOffFrequency / (PIx2 * p->cutOffFrequency + c);
	pix2xDt = PIx2 * data->inSignal[0]->dt;
	numComplexCoeffs = p->cascade + 1;
	for (chan = 0; chan < data->outSignal->numChannels; chan++) {
		cFIndex = chan / data->outSignal->interleaveLevel;
		cC = *(p->coefficients + chan);
		aCoeff = BM_CARNEY_A(c, cC->tau0);
		cF = p->cFList->frequency[cFIndex];
		pix2xDtxCF = pix2xDt * cF;
		inPtr = data->inSignal[0]->channel[chan %
		  data->inSignal[0]->interleaveLevel];
		outPtr = data->outSignal->channel[chan];
		for(i = 0; i < data->outSignal->length; i++) {	
			/* FREQUENCY SHIFT THE ARRAY BUF  */
			RThetaSet_CmplxM(*inPtr++, -pix2xDtxCF * (i + 1), &p->f[0]);
			fF = (cC->tau0 * 3.0 / 2.0 - (cC->tau0 / 2.0) * (cC->oHCLast /
			  p->maxHCVoltage));
			bCoeff = c * fF - 1.0;
			for (j = 1; j < numComplexCoeffs; j++) {
				p->f[j] = cC->fLast[j];
				ScalerMult_CmplxM(&p->f[j], bCoeff);
				Add_CmplxM(&p->f[j], &p->f[j - 1], &p->f[j]);
				Add_CmplxM(&p->f[j], &cC->fLast[j - 1], &p->f[j]);
				ScalerMult_CmplxM(&p->f[j], aCoeff);
			}
			/* FREQUENCY SHIFT BACK UP  */
			RThetaSet_CmplxM(1.0, pix2xDtxCF * (i + 1), &z);
			Mult_CmplxM(&z, &p->f[p->cascade], &z);
			*outPtr++ = oHCTemp = z.re;

			/* Put through OHC saturating nonlinearity-  SO THAT 
			 * Output=.5*vmaxd AT wave=operating point   */
			oHCTemp = aA * (tanh(0.746 * oHCTemp / p->hCOperatingPoint -
			  p->asymmetricalBias) + tanh(p->asymmetricalBias));

			/* lowpass filter the feedback voltage   */
			oHC = c1LP * cC->oHCLast + c2LP * (oHCTemp + cC->oHCTempLast);

			/* save all loop parameters */
			for(j = 0; j < numComplexCoeffs; j++)
				cC->fLast[j] = p->f[j];
			cC->oHCLast = oHC;
			cC->oHCTempLast = oHCTemp;
		}    /* END of TIME LOOP */
	}
	SetProcessContinuity_EarObject(data);
	return(TRUE);

}
