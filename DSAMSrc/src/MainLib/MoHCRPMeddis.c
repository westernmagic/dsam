/**********************
 *
 * File:		MoHCRPMeddis.c
 * Purpose:		This module contains the model for the hair cell receptor
 *				potential.
 * Comments:	07-05-98 LPO: This model has now changed to a new version.  It
 *				uses the Meddis 86 permeability function with the low-pass
 *				filter.
 * Author:		L. P. O'Mard
 * Created:		12 Jul 1993
 * Updated:		07 May 1998
 * Copyright:	(c) 1998, University of Essex
 *
 *********************/

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG_H */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "GeCommon.h"
#include "GeSignalData.h"
#include "GeEarObject.h"
#include "GeUniParMgr.h"
#include "GeModuleMgr.h"
#include "FiParFile.h"
#include "MoHCRPMeddis.h"

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

MeddisRPPtr	meddisRPPtr = NULL;

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
Init_IHCRP_Meddis(ParameterSpecifier parSpec)
{
	static const char *funcName = "Init_IHCRP_Meddis";

	if (parSpec == GLOBAL) {
		if (meddisRPPtr != NULL)
			Free_IHCRP_Meddis();
		if ((meddisRPPtr = (MeddisRPPtr) malloc(sizeof(MeddisRP))) == NULL) {
			NotifyError("%s: Out of memory for 'global' pointer", funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (meddisRPPtr == NULL) { 
			NotifyError("%s:  'local' pointer not set.", funcName);
			return(FALSE);
		}
	}
	meddisRPPtr->parSpec = parSpec;
	meddisRPPtr->updateProcessVariablesFlag = TRUE;
	meddisRPPtr->permConstAFlag = FALSE;
	meddisRPPtr->permConstBFlag = FALSE;
	meddisRPPtr->releaseRateFlag = FALSE;
	meddisRPPtr->mTimeConstTmFlag = FALSE;
	meddisRPPtr->permConst_A = 0.0;
	meddisRPPtr->permConst_B = 0.0;
	meddisRPPtr->releaseRate_g = 0.0;
	meddisRPPtr->mTimeConst_tm = 0.0;

	if (!SetUniParList_IHCRP_Meddis()) {
		NotifyError("%s: Could not initialise parameter list.", funcName);
		Free_IHCRP_Meddis();
		return(FALSE);
	}
	meddisRPPtr->lastOutput = NULL;
	return(TRUE);

}

/********************************* Free ***************************************/

/*
 * This function releases of the memory allocated for the process variables.
 * It should be called when the module is no longer in use.
 * It is defined as returning a BOOLN value because the generic module
 * interface reaAuires that a non-void value be returned.
 */

BOOLN
Free_IHCRP_Meddis(void)
{
	if (meddisRPPtr == NULL)
		return(TRUE);
	FreeProcessVariables_IHCRP_Meddis();
	if (meddisRPPtr->parList)
		FreeList_UniParMgr(&meddisRPPtr->parList);
	if (meddisRPPtr->parSpec == GLOBAL) {
		free(meddisRPPtr);
		meddisRPPtr = NULL;
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
SetUniParList_IHCRP_Meddis(void)
{
	static const char *funcName = "SetUniParList_IHCRP_Meddis";
	UniParPtr	pars;

	if ((meddisRPPtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  IHCRP_MEDDIS_NUM_PARS, NULL)) == NULL) {
		NotifyError("%s: Could not initialise parList.", funcName);
		return(FALSE);
	}
	pars = meddisRPPtr->parList->pars;
	SetPar_UniParMgr(&pars[IHCRP_MEDDIS_PERMCONST_A], "PERM_CONST_A",
	  "Permeability constant A (units/s).",
	  UNIPAR_REAL,
	  &meddisRPPtr->permConst_A, NULL,
	  (void * (*)) SetPermConstA_IHCRP_Meddis);
	SetPar_UniParMgr(&pars[IHCRP_MEDDIS_PERMCONST_B], "PERM_CONST_B",
	  "Permeability constant B (units/s).",
	  UNIPAR_REAL,
	  &meddisRPPtr->permConst_B, NULL,
	  (void * (*)) SetPermConstB_IHCRP_Meddis);
	SetPar_UniParMgr(&pars[IHCRP_MEDDIS_RELEASERATE_G], "RELEASE_G",
	  "Release rate (units/s).",
	  UNIPAR_REAL,
	  &meddisRPPtr->releaseRate_g, NULL,
	  (void * (*)) SetReleaseRate_IHCRP_Meddis);
	SetPar_UniParMgr(&pars[IHCRP_MEDDIS_MTIMECONST_TM], "TIME_CONST_TM",
	  "Receptor potential time constant for IHC model (s).",
	  UNIPAR_REAL,
	  &meddisRPPtr->mTimeConst_tm, NULL,
	  (void * (*)) SetMTimeConstTm_IHCRP_Meddis);
	return(TRUE);

}

/****************************** GetUniParListPtr ******************************/

/*
 * This function returns a pointer to the module's universal parameter
 * list.
 */

UniParListPtr
GetUniParListPtr_IHCRP_Meddis(void)
{
	static const char	*funcName = "GetUniParListPtr_IHCRP_Meddis";

	if (meddisRPPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (meddisRPPtr->parList == NULL) {
		NotifyError("%s: UniParList data structure has not been initialised. "
		  "NULL returned.", funcName);
		return(NULL);
	}
	return(meddisRPPtr->parList);

}

/********************************* SetPermConstA ******************************/

/*
 * This function sets the module's , A, parameter.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetPermConstA_IHCRP_Meddis(double thePermConstA)
{
	static const char	 *funcName = "SetPermConstA_IHCRP_Meddis";

	if (meddisRPPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	meddisRPPtr->permConstAFlag = TRUE;
	meddisRPPtr->permConst_A = thePermConstA;
	meddisRPPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/********************************* SetPermConstB ******************************/

/*
 * This function sets the module's amplitude scale, B, parameter.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetPermConstB_IHCRP_Meddis(double thePermConstB)
{
	static const char	 *funcName = "SetPermConstB_IHCRP_Meddis";

	if (meddisRPPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	meddisRPPtr->permConstBFlag = TRUE;
	meddisRPPtr->permConst_B = thePermConstB;
	meddisRPPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/********************************* SetReleaseRate *****************************/

/*
 * This function sets the module's release rate constant, r, parameter.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetReleaseRate_IHCRP_Meddis(double theReleaseRate)
{
	static const char	*funcName = "SetReleaseRate_IHCRP_Meddis";

	if (meddisRPPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	meddisRPPtr->releaseRateFlag = TRUE;
	meddisRPPtr->releaseRate_g = theReleaseRate;
	meddisRPPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/********************************* SetMTimeConstTm ****************************/

/*
 * This function sets the module's basilar membrane time constant, tm,
 * parameter.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetMTimeConstTm_IHCRP_Meddis(double theMTimeConstTm)
{
	static const char	 *funcName = "SetMTimeConstTm_IHCRP_Meddis";

	if (meddisRPPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	meddisRPPtr->mTimeConstTmFlag = TRUE;
	meddisRPPtr->mTimeConst_tm = theMTimeConstTm;
	meddisRPPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/********************************* SetPars ************************************/

/*
 * This function sets all the module's parameters.
 */

BOOLN
SetPars_IHCRP_Meddis(double aA, double bB, double g, double tm)
{
	BOOLN	ok;
	
	ok = TRUE;
	if (!SetPermConstA_IHCRP_Meddis(aA))
		ok = FALSE;
	if (!SetPermConstB_IHCRP_Meddis(bB))
		ok = FALSE;
	if (!SetReleaseRate_IHCRP_Meddis(g))
		ok = FALSE;
	if (!SetMTimeConstTm_IHCRP_Meddis(tm))
		ok = FALSE;
	return(ok);
	  
}

/********************************* CheckPars **********************************/

/*
 * This routine checks that all of the parameters for the module are set.
 * Because this module is defined by its coeffients, default values are always
 * set if none of the parameters are set, otherwise all of them must be
 * individually set.
 */

BOOLN
CheckPars_IHCRP_Meddis(void)
{
	static const char *funcName = "CheckPars_IHCRP_Meddis";
	BOOLN	ok;
	
	ok = TRUE;
	if (meddisRPPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (!meddisRPPtr->permConstAFlag) {
		NotifyError("%s: Permeability constant, A, not correctly set.",
		  funcName);
		ok = FALSE;
	}
	if (!meddisRPPtr->permConstBFlag) {
		NotifyError("%s: Permeability constant, B, not correctly set.",
		  funcName);
		ok = FALSE;
	}
	if (!meddisRPPtr->releaseRateFlag) {
		NotifyError("%s: Release rate constant, g, not correctly set.",
		  funcName);
		ok = FALSE;
	}
	if (!meddisRPPtr->mTimeConstTmFlag) {
		NotifyError("%s: Basilar membrane time constant, tm, not correctly "\
		  "set.", funcName);
		ok = FALSE;
	}
	return(ok);
		
}

/****************************** PrintPars *************************************/

/*
 * This program prints the parameters of the module to the standard output.
 */
 
BOOLN
PrintPars_IHCRP_Meddis(void)
{
	static const char *funcName = "PrintPars_IHCRP_Meddis";

	if (!CheckPars_IHCRP_Meddis()) {
		NotifyError("%s: Parameters have not been correctly set.", funcName);
		return(FALSE);
	}
	DPrint("Receptor Potential Module Parameters:-\n");
	DPrint("\tPermeability constant, A = %g,\t", meddisRPPtr->permConst_A);
	DPrint("\tPermeability constant, B = %g,\n", meddisRPPtr->permConst_B);
	DPrint("\tRelease rate, g = %g,\t", meddisRPPtr->releaseRate_g);
	DPrint("Time constant = %g ms\n", MSEC(meddisRPPtr->mTimeConst_tm));
	return(TRUE);

}

/****************************** ReadPars **************************************/

/*
 * This program reads a specified number of parameters from a file.
 * It returns FALSE if it fails in any way.
 */
 
BOOLN
ReadPars_IHCRP_Meddis(char *fileName)
{
	static const char *funcName = "ReadPars_IHCRP_Meddis";
	BOOLN	ok;
	char	*filePath;
	double	permConstA, permConstB, releaseRate_g, MTimeConst;
    FILE    *fp;
    
	filePath = GetParsFileFPath_Common(fileName);
    if ((fp = fopen(filePath, "r")) == NULL) {
        NotifyError("%s: Cannot open data file '%s'.\n", funcName, filePath);
		return(FALSE);
    }
    DPrint("%s: Reading from '%s':\n", funcName, filePath);
    Init_ParFile();
	ok = TRUE;
	if (!GetPars_ParFile(fp, "%lf", &permConstA))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &permConstB))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &releaseRate_g))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &MTimeConst))
		ok = FALSE;
    fclose(fp);
    Free_ParFile();
	if (!ok) {
		NotifyError("%s: Not enough lines, or invalid parameters, in module "
		  "parameter file '%s'.", funcName, filePath);
		return(FALSE);
	}
	if (!SetPars_IHCRP_Meddis(permConstA, permConstB, releaseRate_g,
	  MTimeConst)) {
		NotifyError("%s: Could not set parameters.", funcName);
		return(FALSE);
	}
	return(TRUE);
    
}

/**************************** GetRestingResponse ******************************/

/*
 * This routine returns the resting response of the Meddis IHC
 * receptor potential model.  It can then be used for things like calculating
 * the AC/DC ratio.
 */

double
GetRestingResponse_IHCRP_Meddis(void)
{
	static const char	*funcName = "GetRestingResponse_IHCRP_Meddis";

	if (meddisRPPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	return(meddisRPPtr->releaseRate_g * meddisRPPtr->permConst_A /
	  (meddisRPPtr->permConst_A + meddisRPPtr->permConst_B));

}

/****************************** SetParsPointer ********************************/

/*
 * This function sets the global parameter pointer for the module to
 * that associated with the module instance.
 */

BOOLN
SetParsPointer_IHCRP_Meddis(ModulePtr theModule)
{
	static const char	*funcName = "SetParsPointer_IHCRP_Meddis";

	if (!theModule) {
		NotifyError("%s: The module is not set.", funcName);
		return(FALSE);
	}
	meddisRPPtr = (MeddisRPPtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 */

BOOLN
InitModule_IHCRP_Meddis(ModulePtr theModule)
{
	static const char	*funcName = "InitModule_IHCRP_Meddis";

	if (!SetParsPointer_IHCRP_Meddis(theModule)) {
		NotifyError("%s: Cannot set parameters pointer.", funcName);
		return(FALSE);
	}
	if (!Init_IHCRP_Meddis(GLOBAL)) {
		NotifyError("%s: Could not initialise process structure.", funcName);
		return(FALSE);
	}
	theModule->parsPtr = meddisRPPtr;
	theModule->CheckPars = CheckPars_IHCRP_Meddis;
	theModule->Free = Free_IHCRP_Meddis;
	theModule->GetUniParListPtr = GetUniParListPtr_IHCRP_Meddis;
	theModule->GetRestingResponse = GetRestingResponse_IHCRP_Meddis;
	theModule->PrintPars = PrintPars_IHCRP_Meddis;
	theModule->ReadPars = ReadPars_IHCRP_Meddis;
	theModule->RunProcess = RunModel_IHCRP_Meddis;
	theModule->SetParsPointer = SetParsPointer_IHCRP_Meddis;
	return(TRUE);

}

/********************************* CheckData **********************************/

/*
 * This routine checks that the input signal is correctly initialised
 * (using CheckRamp_SignalData), and determines whether the parameter values
 * are valid for the signal sampling interval.
 */

BOOLN
CheckData_IHCRP_Meddis(EarObjectPtr data)
{
	static const char *funcName = "CheckData_IHCRP_Meddis";
	
	if (data == NULL) {
		NotifyError("%s: EarObject not initialised.", funcName);
		return(FALSE);
	}	
	if (meddisRPPtr->mTimeConst_tm <= data->inSignal[0]->dt / 2.0) {
		NotifyError("%s: Membrane time constant (%g ms) is too small it must\n"
		  "be greater than %g ms", funcName, MILLI(meddisRPPtr->mTimeConst_tm),
		  MILLI(data->inSignal[0]->dt / 2.0));
		return(FALSE);
	}
	return(TRUE);
	
}

/**************************** InitProcessVariables ****************************/

/*
 * This function allocates the memory for the process variables.
 * The spontaneous probability assumes preceeding silence.
 * Initial, spontaneous probability calculation: 1st sample is set to zero.
 * This value is used by the hair cell reservoir models.
 */

BOOLN
InitProcessVariables_IHCRP_Meddis(EarObjectPtr data)
{
	static const char *funcName = "InitProcessVariables_IHCRP_Meddis";
	int		i;
	double	spontPerm_k0;

	if (meddisRPPtr->updateProcessVariablesFlag || data->updateProcessFlag ||
	  (data->timeIndex == PROCESS_START_TIME)) {
		if (meddisRPPtr->updateProcessVariablesFlag || data->
		  updateProcessFlag) {
			FreeProcessVariables_IHCRP_Meddis();
			if ((meddisRPPtr->lastOutput = (double *)
			  calloc(data->outSignal->numChannels, sizeof(double))) == NULL) {
			 	NotifyError("%s: Out of memory.", funcName);
			 	return(FALSE);
			}
			meddisRPPtr->updateProcessVariablesFlag = FALSE;
		}
		spontPerm_k0 = meddisRPPtr->releaseRate_g * meddisRPPtr->permConst_A /
		  (meddisRPPtr->permConst_A + meddisRPPtr->permConst_B);
		for (i = 0; i < data->outSignal->numChannels; i++)
			meddisRPPtr->lastOutput[i] = spontPerm_k0;
	}
	return(TRUE);

}

/**************************** FreeProcessVariables ****************************/

/*
 * This routine releases the memory allocated for the process variables
 * if they have been initialised.
 */

void
FreeProcessVariables_IHCRP_Meddis(void)
{

	if (meddisRPPtr->lastOutput == NULL)
		return;
	free(meddisRPPtr->lastOutput);
	meddisRPPtr->lastOutput = NULL;
	meddisRPPtr->updateProcessVariablesFlag = TRUE;

}

/********************************* RunModel ***********************************/

/*
 * This routine runs the input signal through the model and puts the result
 * into the output signal.  It checks that all initialisation has been
 * correctly carried out by calling the appropriate checking routines.
 */

#define	PERMEABILITY(INPUT)			(((st_Plus_A = (INPUT) + \
		  meddisRPPtr->permConst_A) > 0.0)? meddisRPPtr->releaseRate_g * \
		  st_Plus_A / (st_Plus_A + meddisRPPtr->permConst_B): 0.0)

#define LOWPASSFILTER(LASTOUTPUT)	((ChanData) ((LASTOUTPUT) + \
		  (permeability_K - (LASTOUTPUT)) * dtOverTm))

BOOLN
RunModel_IHCRP_Meddis(EarObjectPtr data)
{
	static const char *funcName = "RunModel_IHCRP_Meddis";
	register ChanData	*inPtr, *outPtr;
	register double		dtOverTm, permeability_K, st_Plus_A;
	int		chan;
	ChanLen	i;
	
	if (!CheckPars_IHCRP_Meddis()) {
		NotifyError("%s: Parameters have not been correctly set.", funcName);
		return(FALSE);
	}
	if (!CheckData_IHCRP_Meddis(data)) {
		NotifyError("%s: Process data invalid.", funcName);
		return(FALSE);
	}
	SetProcessName_EarObject(data, "Hair cell receptor potential");
	if (!InitOutSignal_EarObject(data, data->inSignal[0]->numChannels,
	  data->inSignal[0]->length, data->inSignal[0]->dt)) {
		NotifyError("%s: Could not initialise output signal.", funcName);
		return(FALSE);
	}
	if (!InitProcessVariables_IHCRP_Meddis(data)) {
		NotifyError("%s: Could not initialise the process variables.",
		  funcName);
		return(FALSE);
	}
	dtOverTm = data->outSignal->dt / meddisRPPtr->mTimeConst_tm;
	for (chan = 0; chan < data->outSignal->numChannels; chan++) {
		inPtr = data->inSignal[0]->channel[chan];
		outPtr = data->outSignal->channel[chan];
		permeability_K = PERMEABILITY(*inPtr++);
		*outPtr++ = LOWPASSFILTER(meddisRPPtr->lastOutput[chan]);
		/* Probability calculation for the rest of the signal. */
		for (i = 1; i < data->outSignal->length; i++) {
			permeability_K = PERMEABILITY(*inPtr++);
			*outPtr++ = LOWPASSFILTER(*(outPtr - 1));
		}
		meddisRPPtr->lastOutput[chan] = *(outPtr - 1);
	}
	SetProcessContinuity_EarObject(data);
	return(TRUE);
		
} /* RunModel_IHCRP_Meddis */

#undef PERMEABILITY
#undef LOWPASSFILTER
