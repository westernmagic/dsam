/*******
 *
 * File:		MoIHCCooke.C
 * Purpose: 	Cooke 1991 hair cell.
 * Comments:	
 * Authors:		M. Cooke, L.P.O'Mard.
 * Created:		21 May 1995
 * Updated:	
 * Copyright:	(c) 1998, University of Essex.
 *
 ********/

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
#include "MoIHCCooke.h"

/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Global Variables *********************************/
/******************************************************************************/

CookeHCPtr	cookeHCPtr = NULL;

/******************************************************************************/
/*************************** Subroutines and Functions ************************/
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
Init_IHC_Cooke91(ParameterSpecifier parSpec)
{
	static const char *funcName = "Init_IHC_Cooke91";

	if (parSpec == GLOBAL) {
		if (cookeHCPtr != NULL)
			Free_IHC_Cooke91();
		if ((cookeHCPtr = (CookeHCPtr) malloc(sizeof(CookeHC))) == NULL) {
			NotifyError("%s: Out of memory for 'global' pointer", funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (cookeHCPtr == NULL) { 
			NotifyError("%s:  'local' pointer not set.", funcName);
			return(FALSE);
		}
	}
	cookeHCPtr->parSpec = parSpec;
	cookeHCPtr->updateProcessVariablesFlag = TRUE;
	cookeHCPtr->crawfordConstFlag = TRUE;
	cookeHCPtr->releaseFractionFlag = TRUE;
	cookeHCPtr->refillFractionFlag = TRUE;
	cookeHCPtr->spontRateFlag = TRUE;
	cookeHCPtr->maxSpikeRateFlag = TRUE;
					
	cookeHCPtr->crawfordConst = 100;
	cookeHCPtr->releaseFraction = 24.0;
	cookeHCPtr->refillFraction = 6.0;
	cookeHCPtr->spontRate = 50.0;
	cookeHCPtr->maxSpikeRate = 1000.0;

	if (!SetUniParList_IHC_Cooke91()) {
		NotifyError("%s: Could not initialise parameter list.", funcName);
		Free_IHC_Cooke91();
		return(FALSE);
	}
	cookeHCPtr->hCChannels = NULL;
	return(TRUE);

}

/********************************* Free ***************************************/

/*
 * This function releases of the memory allocated for the process variables.
 * It should be called when the module is no longer in use.
 * It is defined as returning a BOOLN value because the generic module
 * interface requires that a non-void value be returned.
 */

BOOLN
Free_IHC_Cooke91(void)
{
	if (cookeHCPtr == NULL)
		return(TRUE);
	FreeProcessVariables_IHC_Cooke91();
	if (cookeHCPtr->parList)
		FreeList_UniParMgr(&cookeHCPtr->parList);
	if (cookeHCPtr->parSpec == GLOBAL) {
		free(cookeHCPtr);
		cookeHCPtr = NULL;
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
SetUniParList_IHC_Cooke91(void)
{
	static const char *funcName = "SetUniParList_IHC_Cooke91";
	UniParPtr	pars;

	if ((cookeHCPtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  IHC_COOKE91_NUM_PARS, NULL)) == NULL) {
		NotifyError("%s: Could not initialise parList.", funcName);
		return(FALSE);
	}
	pars = cookeHCPtr->parList->pars;
	SetPar_UniParMgr(&pars[IHC_COOKE91_CRAWFORDCONST], "C_VALUE",
	  "Crawford and Fettiplace c value.",
	  UNIPAR_REAL,
	  &cookeHCPtr->crawfordConst, NULL,
	  (void * (*)) SetCrawfordConst_IHC_Cooke91);
	SetPar_UniParMgr(&pars[IHC_COOKE91_RELEASEFRACTION], "RELEASE",
	  "Release fraction.",
	  UNIPAR_REAL,
	  &cookeHCPtr->releaseFraction, NULL,
	  (void * (*)) SetReleaseFraction_IHC_Cooke91);
	SetPar_UniParMgr(&pars[IHC_COOKE91_REFILLFRACTION], "REFILL",
	  "Replenishment (refill) fraction.",
	  UNIPAR_REAL,
	  &cookeHCPtr->refillFraction, NULL,
	  (void * (*)) SetRefillFraction_IHC_Cooke91);
	SetPar_UniParMgr(&pars[IHC_COOKE91_SPONTRATE], "SPONT_FIRING",
	  "Desired spontaneous firing rate (spikes/s).",
	  UNIPAR_REAL,
	  &cookeHCPtr->spontRate, NULL,
	  (void * (*)) SetSpontRate_IHC_Cooke91);
	SetPar_UniParMgr(&pars[IHC_COOKE91_MAXSPIKERATE], "MAX_FIRING",
	  "Maximum possible firing rate (spikes/s).",
	  UNIPAR_REAL,
	  &cookeHCPtr->maxSpikeRate, NULL,
	  (void * (*)) SetMaxSpikeRate_IHC_Cooke91);
	return(TRUE);

}

/****************************** GetUniParListPtr ******************************/

/*
 * This function returns a pointer to the module's universal parameter
 * list.
 */

UniParListPtr
GetUniParListPtr_IHC_Cooke91(void)
{
	static const char	*funcName = "GetUniParListPtr_IHC_Cooke91";

	if (cookeHCPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (cookeHCPtr->parList == NULL) {
		NotifyError("%s: UniParList data structure has not been initialised. "
		  "NULL returned.", funcName);
		return(NULL);
	}
	return(cookeHCPtr->parList);

}

/********************************* SetCrawfordConst ***************************/

/*
 * This function sets the module's CrawfordConst parameter.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetCrawfordConst_IHC_Cooke91(double theCrawfordConst)
{
	static const char	*funcName = "SetCrawfordConst_IHC_Cooke91";

	if (cookeHCPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	cookeHCPtr->crawfordConstFlag = TRUE;
	cookeHCPtr->crawfordConst = theCrawfordConst;
	cookeHCPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/********************************* SetReleaseFraction *************************/

/*
 * This function sets the module's release fraction parameter.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetReleaseFraction_IHC_Cooke91(double theReleaseFraction)
{
	static const char	*funcName = "SetReleaseFraction_IHC_Cooke91";

	if (cookeHCPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	cookeHCPtr->releaseFractionFlag = TRUE;
	cookeHCPtr->releaseFraction = theReleaseFraction;
	cookeHCPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/********************************* SetRefillFraction **************************/

/*
 * This function sets the module's refill fraction parameter.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetRefillFraction_IHC_Cooke91(double theRefillFraction)
{
	static const char	*funcName = "SetRefillFraction_IHC_Cooke91";

	if (cookeHCPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	cookeHCPtr->refillFractionFlag = TRUE;
	cookeHCPtr->refillFraction = theRefillFraction;
	cookeHCPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/********************************* SetSpontRate *******************************/

/*
 * This function sets the module's spontaneous rate parameter.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetSpontRate_IHC_Cooke91(double theSpontRate)
{
	static const char	*funcName = "SetSpontRate_IHC_Cooke91";

	if (cookeHCPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	cookeHCPtr->spontRateFlag = TRUE;
	cookeHCPtr->spontRate = theSpontRate;
	cookeHCPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/********************************* SetMaxSpikeRate ****************************/

/*
 * This function sets the module's maximum spike rate parameter.
 * it was designated "NORMSPIKE" in the original code.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetMaxSpikeRate_IHC_Cooke91(double theMaxSpikeRate)
{
	static const char *funcName = "SetMaxSpikeRate_IHC_Cooke91";

	if (cookeHCPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (theMaxSpikeRate < 0) {
		NotifyError("%s: Illegal initial value:  %d.", funcName,
		  theMaxSpikeRate);
		return(FALSE);
	}
	cookeHCPtr->maxSpikeRateFlag = TRUE;
	cookeHCPtr->maxSpikeRate = theMaxSpikeRate;
	cookeHCPtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/********************************* SetPars ************************************/

/*
 * This function sets all the module's parameters.
 */

BOOLN
SetPars_IHC_Cooke91(double crawfordConst, double releaseFraction,
  double refillFraction, double spontRate, double maxSpikeRate)
{
	BOOLN	ok;
	
	ok = TRUE;
	if (!SetCrawfordConst_IHC_Cooke91(crawfordConst))
		ok = FALSE;
	if (!SetReleaseFraction_IHC_Cooke91(releaseFraction))
		ok = FALSE;
	if (!SetRefillFraction_IHC_Cooke91(refillFraction))
		ok = FALSE;
	if (!SetSpontRate_IHC_Cooke91(spontRate))
		ok = FALSE;
	if (!SetMaxSpikeRate_IHC_Cooke91(maxSpikeRate))
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
CheckPars_IHC_Cooke91(void)
{
	static const char *funcName = "CheckPars_IHC_Cooke91";
	BOOLN	ok;
	
	ok = TRUE;
	if (cookeHCPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (!cookeHCPtr->releaseFractionFlag) {
		NotifyError("%s: Release fraction not correctly set.", funcName);
		ok = FALSE;
	}
	if (!cookeHCPtr->refillFractionFlag) {
		NotifyError("%s: Release fraction not correctly set.", funcName);
		ok = FALSE;
	}
	if (!cookeHCPtr->refillFractionFlag) {
		NotifyError("%s: refill fraction not correctly set.", funcName);
		ok = FALSE;
	}
	if (!cookeHCPtr->spontRateFlag) {
		NotifyError("%s: Spontaneous rate not correctlyset.", funcName);
		ok = FALSE;
	}
	if (!cookeHCPtr->maxSpikeRateFlag) {
		NotifyError("%s: Maximum spike rate not correctly set.", funcName);
		ok = FALSE;
	}
	return(ok);
		
}

/****************************** PrintPars *************************************/

/*
 * This program prints the parameters of the module to the standard output.
 */
 
BOOLN
PrintPars_IHC_Cooke91(void)
{
	static const char *funcName = "PrintPars_IHC_Cooke91";

	if (!CheckPars_IHC_Cooke91()) {
		NotifyError("%s: Parameters have not been correctly set.", funcName);
		return(FALSE);
	}
	DPrint("Cooke 91 Inner Hair Cell Module Parameters:-\n");
	DPrint("\tCrawford constant = %g\n",
	  cookeHCPtr->crawfordConst);
	DPrint("\tRelease fraction = %g,\tRefill fraction = "\
	  "%g,\n", cookeHCPtr->releaseFraction, cookeHCPtr->refillFraction);
	DPrint("\tSpontaneous firing rate = %g /s,\tMaximum "\
	  "firing rate = %g\n", cookeHCPtr->spontRate, cookeHCPtr->maxSpikeRate);
	return(TRUE);

}

/****************************** ReadPars **************************************/

/*
 * This program reads a specified number of parameters from a file.
 * It returns FALSE if it fails in any way.
 */
 
BOOLN
ReadPars_IHC_Cooke91(char *fileName)
{
	static const char *funcName = "ReadPars_IHC_Cooke91";
	BOOLN	ok;
	char	*filePath;
	double	crawfordConst, releaseFraction, refillFraction, spontRate;
	double	maxSpikeRate;
	FILE	*fp;

	filePath = GetParsFileFPath_Common(fileName);
	if ((fp = fopen(filePath, "r")) == NULL) {
		NotifyError("%s: Cannot open data file '%s'.\n", funcName, filePath);
		return(FALSE);
	}
	DPrint("%s: Reading from '%s", funcName, filePath);
	Init_ParFile();
	ok = TRUE;
	if (!GetPars_ParFile(fp, "%lf", &crawfordConst))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &releaseFraction))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &refillFraction))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &spontRate))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &maxSpikeRate))
		ok = FALSE;
	fclose(fp);
	Free_ParFile();
	if (!ok) {
		NotifyError("%s: Not enough lines, or invalid parameters, in module "\
		  "parameter file '%s'.", funcName, filePath);
		return(FALSE);
	}
	if (!SetPars_IHC_Cooke91(crawfordConst, releaseFraction, refillFraction,
	  spontRate, maxSpikeRate)) {
		NotifyError("%s: Could not set parameters.", funcName);
		return(FALSE);
	}
	return(ok);
	
}

/****************************** SetParsPointer ********************************/

/*
 * This function sets the global parameter pointer for the module to
 * that associated with the module instance.
 */

BOOLN
SetParsPointer_IHC_Cooke91(ModulePtr theModule)
{
	static const char	*funcName = "SetParsPointer_IHC_Cooke91";

	if (!theModule) {
		NotifyError("%s: The module is not set.", funcName);
		return(FALSE);
	}
	cookeHCPtr = (CookeHCPtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 */

BOOLN
InitModule_IHC_Cooke91(ModulePtr theModule)
{
	static const char	*funcName = "InitModule_IHC_Cooke91";

	if (!SetParsPointer_IHC_Cooke91(theModule)) {
		NotifyError("%s: Cannot set parameters pointer.", funcName);
		return(FALSE);
	}
	if (!Init_IHC_Cooke91(GLOBAL)) {
		NotifyError("%s: Could not initialise process structure.", funcName);
		return(FALSE);
	}
	theModule->parsPtr = cookeHCPtr;
	theModule->threadMode = MODULE_THREAD_MODE_SIMPLE;
	theModule->CheckPars = CheckPars_IHC_Cooke91;
	theModule->Free = Free_IHC_Cooke91;
	theModule->GetUniParListPtr = GetUniParListPtr_IHC_Cooke91;
	theModule->PrintPars = PrintPars_IHC_Cooke91;
	theModule->ReadPars = ReadPars_IHC_Cooke91;
	theModule->RunProcess = RunModel_IHC_Cooke91;
	theModule->SetParsPointer = SetParsPointer_IHC_Cooke91;
	return(TRUE);

}

/********************************* CheckData **********************************/

/*
 * This routine checks that the input signal is correctly initialised, and
 * determines whether the parameter values are valid for the signal sampling
 * interval.
 */

BOOLN
CheckData_IHC_Cooke91(EarObjectPtr data)
{
	static const char *funcName = "CheckData_IHC_Cooke91";
	
	if (data == NULL) {
		NotifyError("%s: EarObject not initialised.", funcName);
		return(FALSE);
	}	
	if (!CheckInSignal_EarObject(data, funcName))
		return(FALSE);
	if (!CheckRamp_SignalData(data->inSignal[0]))
		return(FALSE);
	return(TRUE);
	
}

/**************************** InitProcessVariables ****************************/

/*
 * This function allocates the memory for the process variables.
 * It initialises the hair cells to the spontaneous firing rate.
 */

BOOLN
InitProcessVariables_IHC_Cooke91(EarObjectPtr data)
{
	static const char *funcName = "InitProcessVariables_IHC_Cooke91";
	int		i;
	double	dt, vmin, k, l;
	CookeHCPtr	p = cookeHCPtr;		/* Shorter variable for long formulae. */;
	
	if (p->updateProcessVariablesFlag || data->updateProcessFlag || (data->
	  timeIndex == PROCESS_START_TIME)) {
		if (p->updateProcessVariablesFlag || data->updateProcessFlag) {
			FreeProcessVariables_IHC_Cooke91();
			if ((p->hCChannels = (CookeHCVarsPtr) calloc(data->outSignal->
			  numChannels, sizeof (CookeHCVars))) == NULL) {
				NotifyError("%s: Out of memory.", funcName);
				return(FALSE);
			}
			p->updateProcessVariablesFlag = FALSE;
		}
		dt = data->outSignal->dt;
		vmin = p->spontRate / p->maxSpikeRate;
		k = p->releaseFraction * dt;
		l = p->refillFraction * dt;
		for (i = 0; i < data->outSignal->numChannels; i++) {
			p->hCChannels[i].vimm = vmin;
			p->hCChannels[i].vrel = 0.0;
			p->hCChannels[i].crel = 0.0;
			p->hCChannels[i].vres = 1.0 - vmin;
			p->hCChannels[i].cimm = l / (vmin * k + l);
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
FreeProcessVariables_IHC_Cooke91(void)
{
	if (cookeHCPtr->hCChannels == NULL)
		return;
	free(cookeHCPtr->hCChannels);
	cookeHCPtr->hCChannels = NULL;
	cookeHCPtr->updateProcessVariablesFlag = TRUE;

}

/********************************* RunModel ***********************************/

/*
 * This routine runs the input signal through the model and puts the result
 * into the output signal.  It checks that all initialisation has been
 * correctly carried out by calling the appropriate checking routines.
 */

BOOLN
RunModel_IHC_Cooke91(EarObjectPtr data)
{
	static const char *funcName = "RunModel_IHC_Cooke91";
	int		i;
	double	dt, rp, spmIn, rate, delta;
	ChanLen	j;
	ChanData	*inPtr, *outPtr;
	CookeHCPtr	p = cookeHCPtr; /* Shorter name for use with long equations. */
	CookeHCVarsPtr	c;
	
	if (!data->threadRunFlag) {
		if (!CheckPars_IHC_Cooke91())		
			return(FALSE);
		if (!CheckData_IHC_Cooke91(data)) {
			NotifyError("%s: Process data invalid.", funcName);
			return(FALSE);
		}
		if (!InitOutSignal_EarObject(data, data->inSignal[0]->numChannels,
		  data->inSignal[0]->length, data->inSignal[0]->dt)) {
			NotifyError("%s: Could not initialise output signal.", funcName);
			return(FALSE);
		}
		SetProcessName_EarObject(data, "Meddis 94 probabilistic hair cell");
		if (!InitProcessVariables_IHC_Cooke91(data)) {
			NotifyError("%s: Could not initialise the process variables.",
			  funcName);
			return(FALSE);
		}
		dt = data->outSignal->dt;
		p->vmin = p->spontRate / p->maxSpikeRate;
		p->k = p->releaseFraction * dt;
		p->l = p->refillFraction * dt;
		p->rateScale = p->maxSpikeRate / p->k;
		if (data->initThreadRunFlag)
			return(TRUE);
	}
		
	for (i = data->outSignal->offset; i < data->outSignal->numChannels; i++) {
		inPtr = data->inSignal[0]->channel[i];
		outPtr = data->outSignal->channel[i];
		c = &p->hCChannels[i];
		for (j = 0; j < data->outSignal->length; j++, inPtr++, outPtr++) {
			rp = *inPtr / (*inPtr + p->crawfordConst);
   			if (rp < 0.0)
   				rp = 0.0;
			/* Adaptation */
			spmIn = (rp < p->vmin)? p->vmin: rp;
			if (spmIn > c->vimm) {
				if (spmIn > (c->vimm + c->vrel)) {
					delta = spmIn - (c->vrel + c->vimm);
					c->cimm = ((c->cimm * c->vimm + c->crel * c->vrel + delta) /
					  (c->vimm + c->vrel + delta));
					c->vres = 1.0 - spmIn;
					c->vrel = 0.0;
				} else {
					delta = spmIn-c->vimm;
					c->cimm = (c->cimm * c->vimm + delta * c->crel) / spmIn;
					c->vres = c->vres - delta;
				}
			} else {
				if (c->vimm > spmIn) {
					delta = c->vimm - spmIn;
					c->crel=(delta * c->cimm + c->vrel * c->crel) / (delta +
					  c->vrel);
					c->vrel = c->vrel + delta;
				}
			}
			/* Compute output and refill */
			c->vimm = spmIn;
			rate = (p->k * c->cimm * c->vimm);
			c->cimm = c->cimm - rate;
			c->cimm = c->cimm + p->l * (1.0 - c->cimm);
			if (c->vrel > 0.000001)
				c->crel = c->crel + p->l * (1.0 - c->crel);
			*outPtr = rate * p->rateScale;
		}
	}
	SetProcessContinuity_EarObject(data);
	return(TRUE);
		
} /* RunModel_IHC_Cooke91 */
