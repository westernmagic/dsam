/**********************
 *
 * File:		MoIHCCarney.c
 * Purpose:		Laurel H. Carney IHC synapse module: Carney L. H. (1993)
 *				"A model for the responses of low-frequency auditory-nerve
 *				fibers in cat", JASA, 93, pp 401-417.
 * Comments:	Written using ModuleProducer version 1.9 (Feb 29 1996).
 * Authors:		L. P. O'Mard modified from L. H. Carney's code
 * Created:		30 April 1996
 * Updated:	
 * Copyright:	(c) 1998, University of Essex.
 *
 *********************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG_H */

#include "GeCommon.h"
#include "GeSignalData.h"
#include "GeEarObject.h"
#include "GeUniParMgr.h"
#include "GeModuleMgr.h"
#include "FiParFile.h"
#include "MoIHCCarney.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

CarneyHCPtr	carneyHCPtr = NULL;

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
Free_IHC_Carney(void)
{
	/* static const char	*funcName = "Free_IHC_Carney"; */

	if (carneyHCPtr == NULL)
		return(FALSE);
	FreeProcessVariables_IHC_Carney();
	if (carneyHCPtr->parList)
		FreeList_UniParMgr(&carneyHCPtr->parList);
	if (carneyHCPtr->parSpec == GLOBAL) {
		free(carneyHCPtr);
		carneyHCPtr = NULL;
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
Init_IHC_Carney(ParameterSpecifier parSpec)
{
	static const char	*funcName = "Init_IHC_Carney";

	if (parSpec == GLOBAL) {
		if (carneyHCPtr != NULL)
			Free_IHC_Carney();
		if ((carneyHCPtr = (CarneyHCPtr) malloc(sizeof(CarneyHC))) == NULL) {
			NotifyError("%s: Out of memory for 'global' pointer", funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (carneyHCPtr == NULL) {
			NotifyError("%s:  'local' pointer not set.", funcName);
			return(FALSE);
		}
	}
	carneyHCPtr->parSpec = parSpec;
	carneyHCPtr->updateProcessVariablesFlag = TRUE;
	carneyHCPtr->maxHCVoltageFlag = FALSE;
	carneyHCPtr->restingReleaseRateFlag = FALSE;
	carneyHCPtr->restingPermFlag = FALSE;
	carneyHCPtr->maxGlobalPermFlag = FALSE;
	carneyHCPtr->maxLocalPermFlag = FALSE;
	carneyHCPtr->maxImmediatePermFlag = FALSE;
	carneyHCPtr->maxLocalVolumeFlag = FALSE;
	carneyHCPtr->minLocalVolumeFlag = FALSE;
	carneyHCPtr->maxImmediateVolumeFlag = FALSE;
	carneyHCPtr->minImmediateVolumeFlag = FALSE;
	carneyHCPtr->maxHCVoltage = 0.0;
	carneyHCPtr->restingReleaseRate = 0.0;
	carneyHCPtr->restingPerm = 0.0;
	carneyHCPtr->maxGlobalPerm = 0.0;
	carneyHCPtr->maxLocalPerm = 0.0;
	carneyHCPtr->maxImmediatePerm = 0.0;
	carneyHCPtr->maxLocalVolume = 0.0;
	carneyHCPtr->minLocalVolume = 0.0;
	carneyHCPtr->maxImmediateVolume = 0.0;
	carneyHCPtr->minImmediateVolume = 0.0;

	if (!SetUniParList_IHC_Carney()) {
		NotifyError("%s: Could not initialise parameter list.", funcName);
		Free_IHC_Carney();
		return(FALSE);
	}
	carneyHCPtr->hCChannels = NULL;
	return(TRUE);

}

/****************************** SetUniParList *********************************/

/*
 * This function initialises and sets the module's universal parameter
 * list. This list provides universal access to the module's
 * parameters.  It expects to be called from the 'Init_' routine.
 */

BOOLN
SetUniParList_IHC_Carney(void)
{
	static const char *funcName = "SetUniParList_IHC_Carney";
	UniParPtr	pars;

	if ((carneyHCPtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  IHC_CARNEY_NUM_PARS, NULL)) == NULL) {
		NotifyError("%s: Could not initialise parList.", funcName);
		return(FALSE);
	}
	pars = carneyHCPtr->parList->pars;
	SetPar_UniParMgr(&pars[IHC_CARNEY_MAXHCVOLTAGE], "V_MAX",
	  "Maximum depolarising hair cell voltage, Vmax (V).",
	  UNIPAR_REAL,
	  &carneyHCPtr->maxHCVoltage, NULL,
	  (void * (*)) SetMaxHCVoltage_IHC_Carney);
	SetPar_UniParMgr(&pars[IHC_CARNEY_RESTINGRELEASERATE], "R0",
	  "Resting release from synapse, R0 (spikes/s).",
	  UNIPAR_REAL,
	  &carneyHCPtr->restingReleaseRate, NULL,
	  (void * (*)) SetRestingReleaseRate_IHC_Carney);
	SetPar_UniParMgr(&pars[IHC_CARNEY_RESTINGPERM], "P_REST",
	  "Resting permeability, Prest ('volume'/s).",
	  UNIPAR_REAL,
	  &carneyHCPtr->restingPerm, NULL,
	  (void * (*)) SetRestingPerm_IHC_Carney);
	SetPar_UniParMgr(&pars[IHC_CARNEY_MAXGLOBALPERM], "PG_MAX",
	  "Maximum global permeability, PGmax ('volume'/s).",
	  UNIPAR_REAL,
	  &carneyHCPtr->maxGlobalPerm, NULL,
	  (void * (*)) SetMaxGlobalPerm_IHC_Carney);
	SetPar_UniParMgr(&pars[IHC_CARNEY_MAXLOCALPERM], "PL_MAX",
	  "Maximum local permeability, PLmax ('volume'/s).",
	  UNIPAR_REAL,
	  &carneyHCPtr->maxLocalPerm, NULL,
	  (void * (*)) SetMaxLocalPerm_IHC_Carney);
	SetPar_UniParMgr(&pars[IHC_CARNEY_MAXIMMEDIATEPERM], "PI_MAX",
	  "Maximum immediate permeability, PImax ('volume'/s).",
	  UNIPAR_REAL,
	  &carneyHCPtr->maxImmediatePerm, NULL,
	  (void * (*)) SetMaxImmediatePerm_IHC_Carney);
	SetPar_UniParMgr(&pars[IHC_CARNEY_MAXLOCALVOLUME], "VL_MAX",
	  "Maximum local volume, VLmax ('volume').",
	  UNIPAR_REAL,
	  &carneyHCPtr->maxLocalVolume, NULL,
	  (void * (*)) SetMaxLocalVolume_IHC_Carney);
	SetPar_UniParMgr(&pars[IHC_CARNEY_MINLOCALVOLUME], "VL_MIN",
	  "Minimum local volume, VLmin ('volume').",
	  UNIPAR_REAL,
	  &carneyHCPtr->minLocalVolume, NULL,
	  (void * (*)) SetMinLocalVolume_IHC_Carney);
	SetPar_UniParMgr(&pars[IHC_CARNEY_MAXIMMEDIATEVOLUME], "VI_MAX",
	  "Maximum immediate volume, VImax ('volume').",
	  UNIPAR_REAL,
	  &carneyHCPtr->maxImmediateVolume, NULL,
	  (void * (*)) SetMaxImmediateVolume_IHC_Carney);
	SetPar_UniParMgr(&pars[IHC_CARNEY_MINIMMEDIATEVOLUME], "VI_MIN",
	  "Minimum immediate volume, VImin ('volume').",
	  UNIPAR_REAL,
	  &carneyHCPtr->minImmediateVolume, NULL,
	  (void * (*)) SetMinImmediateVolume_IHC_Carney);
	return(TRUE);

}

/****************************** GetUniParListPtr ******************************/

/*
 * This function returns a pointer to the module's universal parameter
 * list.
 */

UniParListPtr
GetUniParListPtr_IHC_Carney(void)
{
	static const char	*funcName = "GetUniParListPtr_IHC_Carney";

	if (carneyHCPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (carneyHCPtr->parList == NULL) {
		NotifyError("%s: UniParList data structure has not been initialised. "
		  "NULL returned.", funcName);
		return(NULL);
	}
	return(carneyHCPtr->parList);

}

/****************************** SetPars ***************************************/

/*
 * This function sets all the module's parameters.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetPars_IHC_Carney(double maxHCVoltage, double restingReleaseRate,
  double restingPerm, double maxGlobalPerm, double maxLocalPerm,
  double maxImmediatePerm, double maxLocalVolume, double minLocalVolume,
  double maxImmediateVolume, double minImmediateVolume)
{
	static const char	*funcName = "SetPars_IHC_Carney";
	BOOLN	ok;

	ok = TRUE;
	if (!SetMaxHCVoltage_IHC_Carney(maxHCVoltage))
		ok = FALSE;
	if (!SetRestingReleaseRate_IHC_Carney(restingReleaseRate))
		ok = FALSE;
	if (!SetRestingPerm_IHC_Carney(restingPerm))
		ok = FALSE;
	if (!SetMaxGlobalPerm_IHC_Carney(maxGlobalPerm))
		ok = FALSE;
	if (!SetMaxLocalPerm_IHC_Carney(maxLocalPerm))
		ok = FALSE;
	if (!SetMaxImmediatePerm_IHC_Carney(maxImmediatePerm))
		ok = FALSE;
	if (!SetMaxLocalVolume_IHC_Carney(maxLocalVolume))
		ok = FALSE;
	if (!SetMinLocalVolume_IHC_Carney(minLocalVolume))
		ok = FALSE;
	if (!SetMaxImmediateVolume_IHC_Carney(maxImmediateVolume))
		ok = FALSE;
	if (!SetMinImmediateVolume_IHC_Carney(minImmediateVolume))
		ok = FALSE;
	if (!ok)
		NotifyError("%s: Failed to set all module parameters." ,funcName);
	return(ok);

}

/****************************** SetMaxHCVoltage *******************************/

/*
 * This function sets the module's maxHCVoltage parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetMaxHCVoltage_IHC_Carney(double theMaxHCVoltage)
{
	static const char	*funcName = "SetMaxHCVoltage_IHC_Carney";

	if (carneyHCPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (fabs(theMaxHCVoltage) < DBL_EPSILON) {
		NotifyError("%s: Illegal value (%g V).", theMaxHCVoltage);
		return(FALSE);
	}
	carneyHCPtr->maxHCVoltageFlag = TRUE;
	carneyHCPtr->updateProcessVariablesFlag = TRUE;
	carneyHCPtr->maxHCVoltage = theMaxHCVoltage;
	return(TRUE);

}

/****************************** SetRestingReleaseRate *************************/

/*
 * This function sets the module's restingReleaseRate parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetRestingReleaseRate_IHC_Carney(double theRestingReleaseRate)
{
	static const char	*funcName = "SetRestingReleaseRate_IHC_Carney";

	if (carneyHCPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	carneyHCPtr->restingReleaseRateFlag = TRUE;
	carneyHCPtr->updateProcessVariablesFlag = TRUE;
	carneyHCPtr->restingReleaseRate = theRestingReleaseRate;
	return(TRUE);

}

/****************************** SetRestingPerm ********************************/

/*
 * This function sets the module's restingPerm parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetRestingPerm_IHC_Carney(double theRestingPerm)
{
	static const char	*funcName = "SetRestingPerm_IHC_Carney";

	if (carneyHCPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	carneyHCPtr->restingPermFlag = TRUE;
	carneyHCPtr->updateProcessVariablesFlag = TRUE;
	carneyHCPtr->restingPerm = theRestingPerm;
	return(TRUE);

}

/****************************** SetMaxGlobalPerm ******************************/

/*
 * This function sets the module's maxGlobalPerm parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetMaxGlobalPerm_IHC_Carney(double theMaxGlobalPerm)
{
	static const char	*funcName = "SetMaxGlobalPerm_IHC_Carney";

	if (carneyHCPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	carneyHCPtr->maxGlobalPermFlag = TRUE;
	carneyHCPtr->updateProcessVariablesFlag = TRUE;
	carneyHCPtr->maxGlobalPerm = theMaxGlobalPerm;
	return(TRUE);

}

/****************************** SetMaxLocalPerm *******************************/

/*
 * This function sets the module's maxLocalPerm parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetMaxLocalPerm_IHC_Carney(double theMaxLocalPerm)
{
	static const char	*funcName = "SetMaxLocalPerm_IHC_Carney";

	if (carneyHCPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	carneyHCPtr->maxLocalPermFlag = TRUE;
	carneyHCPtr->updateProcessVariablesFlag = TRUE;
	carneyHCPtr->maxLocalPerm = theMaxLocalPerm;
	return(TRUE);

}

/****************************** SetMaxImmediatePerm ***************************/

/*
 * This function sets the module's maxImmediatePerm parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetMaxImmediatePerm_IHC_Carney(double theMaxImmediatePerm)
{
	static const char	*funcName = "SetMaxImmediatePerm_IHC_Carney";

	if (carneyHCPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	carneyHCPtr->maxImmediatePermFlag = TRUE;
	carneyHCPtr->updateProcessVariablesFlag = TRUE;
	carneyHCPtr->maxImmediatePerm = theMaxImmediatePerm;
	return(TRUE);

}

/****************************** SetMaxLocalVolume *****************************/

/*
 * This function sets the module's maxLocalVolume parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetMaxLocalVolume_IHC_Carney(double theMaxLocalVolume)
{
	static const char	*funcName = "SetMaxLocalVolume_IHC_Carney";

	if (carneyHCPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	carneyHCPtr->maxLocalVolumeFlag = TRUE;
	carneyHCPtr->updateProcessVariablesFlag = TRUE;
	carneyHCPtr->maxLocalVolume = theMaxLocalVolume;
	return(TRUE);

}

/****************************** SetMinLocalVolume *****************************/

/*
 * This function sets the module's minLocalVolume parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetMinLocalVolume_IHC_Carney(double theMinLocalVolume)
{
	static const char	*funcName = "SetMinLocalVolume_IHC_Carney";

	if (carneyHCPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	carneyHCPtr->minLocalVolumeFlag = TRUE;
	carneyHCPtr->updateProcessVariablesFlag = TRUE;
	carneyHCPtr->minLocalVolume = theMinLocalVolume;
	return(TRUE);

}

/****************************** SetMaxImmediateVolume *************************/

/*
 * This function sets the module's maxImmediateVolume parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetMaxImmediateVolume_IHC_Carney(double theMaxImmediateVolume)
{
	static const char	*funcName = "SetMaxImmediateVolume_IHC_Carney";

	if (carneyHCPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	carneyHCPtr->maxImmediateVolumeFlag = TRUE;
	carneyHCPtr->updateProcessVariablesFlag = TRUE;
	carneyHCPtr->maxImmediateVolume = theMaxImmediateVolume;
	return(TRUE);

}

/****************************** SetMinImmediateVolume *************************/

/*
 * This function sets the module's minImmediateVolume parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetMinImmediateVolume_IHC_Carney(double theMinImmediateVolume)
{
	static const char	*funcName = "SetMinImmediateVolume_IHC_Carney";

	if (carneyHCPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	carneyHCPtr->minImmediateVolumeFlag = TRUE;
	carneyHCPtr->updateProcessVariablesFlag = TRUE;
	carneyHCPtr->minImmediateVolume = theMinImmediateVolume;
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
CheckPars_IHC_Carney(void)
{
	static const char	*funcName = "CheckPars_IHC_Carney";
	BOOLN	ok;

	ok = TRUE;
	if (carneyHCPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (!carneyHCPtr->maxHCVoltageFlag) {
		NotifyError("%s: maxHCVoltage variable not set.", funcName);
		ok = FALSE;
	}
	if (!carneyHCPtr->restingReleaseRateFlag) {
		NotifyError("%s: restingReleaseRate variable not set.", funcName);
		ok = FALSE;
	}
	if (!carneyHCPtr->restingPermFlag) {
		NotifyError("%s: restingPerm variable not set.", funcName);
		ok = FALSE;
	}
	if (!carneyHCPtr->maxGlobalPermFlag) {
		NotifyError("%s: maxGlobalPerm variable not set.", funcName);
		ok = FALSE;
	}
	if (!carneyHCPtr->maxLocalPermFlag) {
		NotifyError("%s: maxLocalPerm variable not set.", funcName);
		ok = FALSE;
	}
	if (!carneyHCPtr->maxImmediatePermFlag) {
		NotifyError("%s: maxImmediatePerm variable not set.", funcName);
		ok = FALSE;
	}
	if (!carneyHCPtr->maxLocalVolumeFlag) {
		NotifyError("%s: maxLocalVolume variable not set.", funcName);
		ok = FALSE;
	}
	if (!carneyHCPtr->minLocalVolumeFlag) {
		NotifyError("%s: minLocalVolume variable not set.", funcName);
		ok = FALSE;
	}
	if (!carneyHCPtr->maxImmediateVolumeFlag) {
		NotifyError("%s: maxImmediateVolume variable not set.", funcName);
		ok = FALSE;
	}
	if (!carneyHCPtr->minImmediateVolumeFlag) {
		NotifyError("%s: minImmediateVolume variable not set.", funcName);
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
PrintPars_IHC_Carney(void)
{
	static const char	*funcName = "PrintPars_IHC_Carney";

	if (!CheckPars_IHC_Carney()) {
		NotifyError("%s: Parameters have not been correctly set.", funcName);
		return(FALSE);
	}
	DPrint("Carney IHC Synapse Module Parameters:-\n");
	DPrint("\tMax. depolarizing HC voltage, Vmax = %g (V)\n",
	  carneyHCPtr->maxHCVoltage);
	DPrint("\tResting release rate, R0 = %g (spikes/s)\n",
	  carneyHCPtr->restingReleaseRate);
	DPrint("\tResting permeability, Prest = %g "
	  "('volume'/s)\n", carneyHCPtr->restingPerm);
	DPrint("\tMax. global permeability, PGmax = %g "
	  "('volume'/s)\n", carneyHCPtr->maxGlobalPerm);
	DPrint("\tMax. local , PLmax = %g ('volume'/s)\n",
	  carneyHCPtr->maxLocalPerm);
	DPrint("\tMax. immediate permeability, PImax = %g "
	  "('volume'/s)\n", carneyHCPtr->maxImmediatePerm);
	DPrint("\tMax. local volume, VLmax = %g ('volume')\n",
	  carneyHCPtr->maxLocalVolume);
	DPrint("\tMin. local volume, VLmin = %g ('volume')\n",
	  carneyHCPtr->minLocalVolume);
	DPrint("\tMax. immediate volume, VImax = %g ('volume')\n",
	  carneyHCPtr->maxImmediateVolume);
	DPrint("\tMin. immediate volume, VLmax = %g ('volume')\n",
	  carneyHCPtr->minImmediateVolume);
	return(TRUE);

}

/****************************** ReadPars **************************************/

/*
 * This program reads a specified number of parameters from a file.
 * It returns FALSE if it fails in any way.n */

BOOLN
ReadPars_IHC_Carney(char *fileName)
{
	static const char	*funcName = "ReadPars_IHC_Carney";
	BOOLN	ok;
	char	*filePath;
	double	maxHCVoltage, restingReleaseRate, restingPerm, maxGlobalPerm;
	double	maxLocalPerm, maxImmediatePerm, maxLocalVolume, minLocalVolume;
	double	maxImmediateVolume, minImmediateVolume;
	FILE	*fp;

	filePath = GetParsFileFPath_Common(fileName);
	if ((fp = fopen(filePath, "r")) == NULL) {
		NotifyError("%s: Cannot open data file '%s'.\n", funcName, filePath);
		return(FALSE);
	}
	DPrint("%s: Reading from '%s':\n", funcName, filePath);
	Init_ParFile();
	ok = TRUE;
	if (!GetPars_ParFile(fp, "%lf", &maxHCVoltage))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &restingReleaseRate))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &restingPerm))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &maxGlobalPerm))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &maxLocalPerm))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &maxImmediatePerm))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &maxLocalVolume))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &minLocalVolume))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &maxImmediateVolume))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &minImmediateVolume))
		ok = FALSE;
	fclose(fp);
	Free_ParFile();
	if (!ok) {
		NotifyError("%s: Not enough lines, or invalid parameters, in module "
		  "parameter file '%s'.", funcName, filePath);
		return(FALSE);
	}
	if (!SetPars_IHC_Carney(maxHCVoltage, restingReleaseRate, restingPerm,
	  maxGlobalPerm, maxLocalPerm, maxImmediatePerm, maxLocalVolume,
	  minLocalVolume, maxImmediateVolume, minImmediateVolume)) {
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
SetParsPointer_IHC_Carney(ModulePtr theModule)
{
	static const char	*funcName = "SetParsPointer_IHC_Carney";

	if (!theModule) {
		NotifyError("%s: The module is not set.", funcName);
		return(FALSE);
	}
	carneyHCPtr = (CarneyHCPtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 */

BOOLN
InitModule_IHC_Carney(ModulePtr theModule)
{
	static const char	*funcName = "InitModule_IHC_Carney";

	if (!SetParsPointer_IHC_Carney(theModule)) {
		NotifyError("%s: Cannot set parameters pointer.", funcName);
		return(FALSE);
	}
	if (!Init_IHC_Carney(GLOBAL)) {
		NotifyError("%s: Could not initialise process structure.", funcName);
		return(FALSE);
	}
	theModule->parsPtr = carneyHCPtr;
	theModule->CheckPars = CheckPars_IHC_Carney;
	theModule->Free = Free_IHC_Carney;
	theModule->GetUniParListPtr = GetUniParListPtr_IHC_Carney;
	theModule->PrintPars = PrintPars_IHC_Carney;
	theModule->ReadPars = ReadPars_IHC_Carney;
	theModule->RunProcess = RunModel_IHC_Carney;
	theModule->SetParsPointer = SetParsPointer_IHC_Carney;
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
CheckData_IHC_Carney(EarObjectPtr data)
{
	static const char	*funcName = "CheckData_IHC_Carney";

	if (data == NULL) {
		NotifyError("%s: EarObject not initialised.", funcName);
		return(FALSE);
	}
	if (!CheckInit_SignalData(data->inSignal[0], "CheckData_IHC_Carney"))
		return(FALSE);
	/*** Put additional checks here. ***/
	return(TRUE);

}

/**************************** InitProcessVariables ****************************/

/*
 * This function allocates the memory for the process variables.
 * It initialises the hair cells to the spontaneous firing rate.
 * The Spontaneous firing rate is determined from the equilbrium state of the
 * system with no input: the reservoir contents do not change in time.
 */

BOOLN
InitProcessVariables_IHC_Carney(EarObjectPtr data)
{
	static const char *funcName = "InitProcessVariables_IHC_Carney";
	int		i;
	double	restingImmediateConc_CI0, restingLocalConc_CL0;
	CarneyHCPtr	hC;
	
	if (carneyHCPtr->updateProcessVariablesFlag || data->updateProcessFlag ||
	  (data->timeIndex == PROCESS_START_TIME)) {
		hC = carneyHCPtr;		/* Shorter variable for long formulae. */
		if (carneyHCPtr->updateProcessVariablesFlag ||
		  data->updateProcessFlag) {
			FreeProcessVariables_IHC_Carney();
			if ((hC->hCChannels = (CarneyHCVarsPtr) calloc(
			  data->outSignal->numChannels, sizeof (CarneyHCVars))) == NULL) {
				NotifyError("%s: Out of memory.", funcName);
				return(FALSE);
			}
			carneyHCPtr->updateProcessVariablesFlag = FALSE;
		}
		restingImmediateConc_CI0 = carneyHCPtr->restingReleaseRate /
		  carneyHCPtr->restingPerm;
		restingLocalConc_CL0 = CARNEY_IHC_RESTING_LOCAL_CONC_FACTOR *
		  restingImmediateConc_CI0;
		for (i = 0; i < data->outSignal->numChannels; i++) {
			hC->hCChannels[i].vI = carneyHCPtr->minImmediateVolume;
			hC->hCChannels[i].vL = carneyHCPtr->minLocalVolume;
			hC->hCChannels[i].cI = restingImmediateConc_CI0;
			hC->hCChannels[i].cL = restingLocalConc_CL0;
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
FreeProcessVariables_IHC_Carney(void)
{
	if (carneyHCPtr->hCChannels == NULL)
		return;
	free(carneyHCPtr->hCChannels);
	carneyHCPtr->hCChannels = NULL;
	carneyHCPtr->updateProcessVariablesFlag = TRUE;

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
RunModel_IHC_Carney(EarObjectPtr data)
{
	static const char	*funcName = "RunModel_IHC_Carney";
	register	ChanData	 *inPtr, *outPtr;
	int			chan;
	double		dt, pIMaxMinusPrest, pLMaxMinusPrest, pGMaxMinusPrest;
	double		cG, releaseProb, pI, pL, pG;
	ChanLen		i;
	CarneyHCPtr		hC;
	CarneyHCVarsPtr	vPtr;

	if (!CheckPars_IHC_Carney())
		return(FALSE);
	if (!CheckData_IHC_Carney(data)) {
		NotifyError("%s: Process data invalid.", funcName);
		return(FALSE);
	}
	SetProcessName_EarObject(data, "Carney IHC Synapse");
	if (!InitOutSignal_EarObject(data, data->inSignal[0]->numChannels,
	  data->inSignal[0]->length, data->inSignal[0]->dt)) {
		NotifyError("%s: Could not initialise output signal.", funcName);
		return(FALSE);
	}
	if (!InitProcessVariables_IHC_Carney(data)) {
		NotifyError("%s: Could not initialise the process variables.",
		  funcName);
		return(FALSE);
	}
	dt = data->outSignal->dt;
	hC = carneyHCPtr;			/* Shorter variable for long formulae. */
	cG = CARNEY_IHC_RESTING_GLOBAL_CONC_FACTOR * hC->restingReleaseRate /
	  hC->restingPerm;
	pIMaxMinusPrest = hC->maxImmediatePerm - hC->restingPerm;
	pLMaxMinusPrest = hC->maxLocalPerm - hC->restingPerm;
	pGMaxMinusPrest = hC->maxGlobalPerm - hC->restingPerm;
	for (chan = 0; chan < data->outSignal->numChannels; chan++) {
		inPtr = data->inSignal[0]->channel[chan];
		outPtr = data->outSignal->channel[chan];
		vPtr = &hC->hCChannels[chan];
		for (i = 0; i < data->outSignal->length; i++) {
			releaseProb = *inPtr++ / hC->maxHCVoltage;
			if ((pI = pIMaxMinusPrest * releaseProb + hC->restingPerm) < 0.0)
				pI = 0.0;
			if ((pL = pLMaxMinusPrest * releaseProb + hC->restingPerm) < 0.0)
				pL = 0.0;
			if ((pG = pGMaxMinusPrest * releaseProb + hC->restingPerm) < 0.0)
				pG = 0.0;

			if (releaseProb < 0.0)
				releaseProb = 0.0;
			vPtr->vI += (hC->maxImmediateVolume - vPtr->vI) * releaseProb;
			vPtr->vL += (hC->maxLocalVolume - vPtr->vL) * releaseProb;

			/* Use Euler Method to solve Diff EQ's */
			vPtr->cI += dt / vPtr->vI * (-pI * vPtr->cI + pL * (vPtr->cL -
			  vPtr->cI));
			vPtr->cL += dt / vPtr->vL * (-pL * (vPtr->cL - vPtr->cI) + pG *
			  (cG - vPtr->cL));
			
			/* Spike prob. */
			*outPtr++ = (ChanData) (vPtr->cI * pI * dt);
		}
	}

	SetProcessContinuity_EarObject(data);
	return(TRUE);

}

