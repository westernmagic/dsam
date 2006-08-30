/**********************
 *
 * File:		MoHCRPZhang.c
 * Purpose:		Incorporates the Zhang et al. 'ihc' receptor potential code.
 * Comments:	Written using ModuleProducer version 1.4.0 (Mar 14 2002).
 *				This code was revised from the ARLO matlab code.
 * Author:		Revised by L. P. O'Mard
 * Created:		01 Aug 2002
 * Updated:	
 * Copyright:	(c) 2002, CNBH, University of Essex
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
#include "FiParFile.h"
#include "UtCmplxM.h"
#include "UtZhang.h"
#include "MoHCRPZhang.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

IHCRPZhangPtr	iHCRPZhangPtr = NULL;

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
Free_IHCRP_Zhang(void)
{
	if (iHCRPZhangPtr == NULL)
		return(FALSE);
	FreeProcessVariables_IHCRP_Zhang();
	if (iHCRPZhangPtr->parList)
		FreeList_UniParMgr(&iHCRPZhangPtr->parList);
	if (iHCRPZhangPtr->parSpec == GLOBAL) {
		free(iHCRPZhangPtr);
		iHCRPZhangPtr = NULL;
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
Init_IHCRP_Zhang(ParameterSpecifier parSpec)
{
	static const WChar	*funcName = wxT("Init_IHCRP_Zhang");

	if (parSpec == GLOBAL) {
		if (iHCRPZhangPtr != NULL)
			Free_IHCRP_Zhang();
		if ((iHCRPZhangPtr = (IHCRPZhangPtr) malloc(sizeof(
		  IHCRPZhang))) == NULL) {
			NotifyError(wxT("%s: Out of memory for 'global' pointer"), funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (iHCRPZhangPtr == NULL) {
			NotifyError(wxT("%s:  'local' pointer not set."), funcName);
			return(FALSE);
		}
	}
	iHCRPZhangPtr->parSpec = parSpec;
	iHCRPZhangPtr->updateProcessVariablesFlag = TRUE;
	iHCRPZhangPtr->a0Flag = TRUE;
	iHCRPZhangPtr->bFlag = TRUE;
	iHCRPZhangPtr->cFlag = TRUE;
	iHCRPZhangPtr->dFlag = TRUE;
	iHCRPZhangPtr->cutFlag = TRUE;
	iHCRPZhangPtr->kFlag = TRUE;
	iHCRPZhangPtr->a0 = 0.1;
	iHCRPZhangPtr->b = 2000.0;
	iHCRPZhangPtr->c = 1.74;
	iHCRPZhangPtr->d = 6.87e-9;
	iHCRPZhangPtr->cut = 4500.0;
	iHCRPZhangPtr->k = 7;

	if (!SetUniParList_IHCRP_Zhang()) {
		NotifyError(wxT("%s: Could not initialise parameter list."), funcName);
		Free_IHCRP_Zhang();
		return(FALSE);
	}
	iHCRPZhangPtr->hCRP = NULL;
	return(TRUE);

}

/****************************** SetUniParList *********************************/

/*
 * This function initialises and sets the module's universal parameter
 * list. This list provides universal access to the module's
 * parameters.  It expects to be called from the 'Init_' routine.
 */

BOOLN
SetUniParList_IHCRP_Zhang(void)
{
	static const WChar *funcName = wxT("SetUniParList_IHCRP_Zhang");
	UniParPtr	pars;

	if ((iHCRPZhangPtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  IHCRP_ZHANG_NUM_PARS, NULL)) == NULL) {
		NotifyError(wxT("%s: Could not initialise parList."), funcName);
		return(FALSE);
	}
	pars = iHCRPZhangPtr->parList->pars;
	SetPar_UniParMgr(&pars[IHCRP_ZHANG_A0], wxT("A0"),
	  wxT("Scaler in IHC nonlinear function."),
	  UNIPAR_REAL,
	  &iHCRPZhangPtr->a0, NULL,
	  (void * (*)) SetA0_IHCRP_Zhang);
	SetPar_UniParMgr(&pars[IHCRP_ZHANG_B], wxT("B"),
	  wxT("Parameter in IHC nonlinear function."),
	  UNIPAR_REAL,
	  &iHCRPZhangPtr->b, NULL,
	  (void * (*)) SetB_IHCRP_Zhang);
	SetPar_UniParMgr(&pars[IHCRP_ZHANG_C], wxT("C"),
	  wxT("Parameter in IHC nonlinear function."),
	  UNIPAR_REAL,
	  &iHCRPZhangPtr->c, NULL,
	  (void * (*)) SetC_IHCRP_Zhang);
	SetPar_UniParMgr(&pars[IHCRP_ZHANG_D], wxT("D"),
	  wxT("Parameter in IHC nonlinear function."),
	  UNIPAR_REAL,
	  &iHCRPZhangPtr->d, NULL,
	  (void * (*)) SetD_IHCRP_Zhang);
	SetPar_UniParMgr(&pars[IHCRP_ZHANG_CUT], wxT("CUT"),
	  wxT("Cutoff frequency of IHC low-pass filter (Hz)."),
	  UNIPAR_REAL,
	  &iHCRPZhangPtr->cut, NULL,
	  (void * (*)) SetCut_IHCRP_Zhang);
	SetPar_UniParMgr(&pars[IHCRP_ZHANG_K], wxT("K"),
	  wxT("Order of IHC lowe-pass filter."),
	  UNIPAR_INT,
	  &iHCRPZhangPtr->k, NULL,
	  (void * (*)) SetK_IHCRP_Zhang);
	return(TRUE);

}

/****************************** GetUniParListPtr ******************************/

/*
 * This function returns a pointer to the module's universal parameter
 * list.
 */

UniParListPtr
GetUniParListPtr_IHCRP_Zhang(void)
{
	static const WChar	*funcName = wxT("GetUniParListPtr_IHCRP_Zhang");

	if (iHCRPZhangPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (iHCRPZhangPtr->parList == NULL) {
		NotifyError(wxT("%s: UniParList data structure has not been ")
		  wxT("initialised. NULL returned."), funcName);
		return(NULL);
	}
	return(iHCRPZhangPtr->parList);

}

/****************************** SetA0 *****************************************/

/*
 * This function sets the module's a0 parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetA0_IHCRP_Zhang(double theA0)
{
	static const WChar	*funcName = wxT("SetA0_IHCRP_Zhang");

	if (iHCRPZhangPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	iHCRPZhangPtr->a0Flag = TRUE;
	iHCRPZhangPtr->updateProcessVariablesFlag = TRUE;
	iHCRPZhangPtr->a0 = theA0;
	return(TRUE);

}

/****************************** SetB ******************************************/

/*
 * This function sets the module's b parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetB_IHCRP_Zhang(double theB)
{
	static const WChar	*funcName = wxT("SetB_IHCRP_Zhang");

	if (iHCRPZhangPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	iHCRPZhangPtr->bFlag = TRUE;
	iHCRPZhangPtr->updateProcessVariablesFlag = TRUE;
	iHCRPZhangPtr->b = theB;
	return(TRUE);

}

/****************************** SetC ******************************************/

/*
 * This function sets the module's c parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetC_IHCRP_Zhang(double theC)
{
	static const WChar	*funcName = wxT("SetC_IHCRP_Zhang");

	if (iHCRPZhangPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	iHCRPZhangPtr->cFlag = TRUE;
	iHCRPZhangPtr->updateProcessVariablesFlag = TRUE;
	iHCRPZhangPtr->c = theC;
	return(TRUE);

}

/****************************** SetD ******************************************/

/*
 * This function sets the module's d parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetD_IHCRP_Zhang(double theD)
{
	static const WChar	*funcName = wxT("SetD_IHCRP_Zhang");

	if (iHCRPZhangPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	iHCRPZhangPtr->dFlag = TRUE;
	iHCRPZhangPtr->updateProcessVariablesFlag = TRUE;
	iHCRPZhangPtr->d = theD;
	return(TRUE);

}

/****************************** SetCut ****************************************/

/*
 * This function sets the module's cut parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetCut_IHCRP_Zhang(double theCut)
{
	static const WChar	*funcName = wxT("SetCut_IHCRP_Zhang");

	if (iHCRPZhangPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	iHCRPZhangPtr->cutFlag = TRUE;
	iHCRPZhangPtr->updateProcessVariablesFlag = TRUE;
	iHCRPZhangPtr->cut = theCut;
	return(TRUE);

}

/****************************** SetK ******************************************/

/*
 * This function sets the module's k parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetK_IHCRP_Zhang(int theK)
{
	static const WChar	*funcName = wxT("SetK_IHCRP_Zhang");

	if (iHCRPZhangPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	iHCRPZhangPtr->kFlag = TRUE;
	iHCRPZhangPtr->updateProcessVariablesFlag = TRUE;
	iHCRPZhangPtr->k = theK;
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
CheckPars_IHCRP_Zhang(void)
{
	static const WChar	*funcName = wxT("CheckPars_IHCRP_Zhang");
	BOOLN	ok;

	ok = TRUE;
	if (iHCRPZhangPtr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (!iHCRPZhangPtr->a0Flag) {
		NotifyError(wxT("%s: a0 variable not set."), funcName);
		ok = FALSE;
	}
	if (!iHCRPZhangPtr->bFlag) {
		NotifyError(wxT("%s: b variable not set."), funcName);
		ok = FALSE;
	}
	if (!iHCRPZhangPtr->cFlag) {
		NotifyError(wxT("%s: c variable not set."), funcName);
		ok = FALSE;
	}
	if (!iHCRPZhangPtr->dFlag) {
		NotifyError(wxT("%s: d variable not set."), funcName);
		ok = FALSE;
	}
	if (!iHCRPZhangPtr->cutFlag) {
		NotifyError(wxT("%s: cut variable not set."), funcName);
		ok = FALSE;
	}
	if (!iHCRPZhangPtr->kFlag) {
		NotifyError(wxT("%s: k variable not set."), funcName);
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
PrintPars_IHCRP_Zhang(void)
{
	static const WChar	*funcName = wxT("PrintPars_IHCRP_Zhang");

	if (!CheckPars_IHCRP_Zhang()) {
		NotifyError(wxT("%s: Parameters have not been correctly set."),
		  funcName);
		return(FALSE);
	}
	DPrint(wxT("Zhang et al. IHCRP Module Parameters:-\n"));
	DPrint(wxT("\ta0 = %g,"), iHCRPZhangPtr->a0);
	DPrint(wxT("\tb = %g,"), iHCRPZhangPtr->b);
	DPrint(wxT("\tc = %g\n"), iHCRPZhangPtr->c);
	DPrint(wxT("\td = %g,"), iHCRPZhangPtr->d);
	DPrint(wxT("\tcut = %g (Hz),"), iHCRPZhangPtr->cut);
	DPrint(wxT("\tk = %d\n"), iHCRPZhangPtr->k);
	return(TRUE);

}

/****************************** SetParsPointer ********************************/

/*
 * This function sets the global parameter pointer for the module to
 * that associated with the module instance.
 */

BOOLN
SetParsPointer_IHCRP_Zhang(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("SetParsPointer_IHCRP_Zhang");

	if (!theModule) {
		NotifyError(wxT("%s: The module is not set."), funcName);
		return(FALSE);
	}
	iHCRPZhangPtr = (IHCRPZhangPtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 * It also initialises the process structure.
 */

BOOLN
InitModule_IHCRP_Zhang(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("InitModule_IHCRP_Zhang");

	if (!SetParsPointer_IHCRP_Zhang(theModule)) {
		NotifyError(wxT("%s: Cannot set parameters pointer."), funcName);
		return(FALSE);
	}
	if (!Init_IHCRP_Zhang(GLOBAL)) {
		NotifyError(wxT("%s: Could not initialise process structure."),
		   funcName);
		return(FALSE);
	}
	theModule->parsPtr = iHCRPZhangPtr;
	theModule->threadMode = MODULE_THREAD_MODE_SIMPLE;
	theModule->CheckPars = CheckPars_IHCRP_Zhang;
	theModule->Free = Free_IHCRP_Zhang;
	theModule->GetUniParListPtr = GetUniParListPtr_IHCRP_Zhang;
	theModule->PrintPars = PrintPars_IHCRP_Zhang;
	theModule->RunProcess = RunModel_IHCRP_Zhang;
	theModule->SetParsPointer = SetParsPointer_IHCRP_Zhang;
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
CheckData_IHCRP_Zhang(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("CheckData_IHCRP_Zhang");

	if (data == NULL) {
		NotifyError(wxT("%s: EarObject not initialised."), funcName);
		return(FALSE);
	}
	if (!CheckInSignal_EarObject(data, funcName))
		return(FALSE);
	/*** Put additional checks here. ***/
	return(TRUE);

}

/****************************** RunIHCNL2 *************************************/

/*
 * Original name: runIHCNL2
 * Original file: hc.c
 */

void
RunIHCNL2_IHCRP_Zhang(TNonLinear* p, const double *in, double *out,
  const int length)
{
  ChanLen	register i;
  register double	temp,dtemp,tempA;

	for (i = 0; i < (ChanLen) length; i++) {
		/*/begin Vsp -> Vihc */
    	temp = in[i];
		if(temp >= 0) {
			tempA = p->A0;
		} else {
			dtemp = pow(-temp,p->C);
			tempA = -p->A0*(dtemp+p->D)/(3*dtemp+p->D);
		};
		out[i] = tempA*log(fabs(temp)*p->B+1.0);
	};

};

/****************************** InitProcessVariables **************************/

/*
 * This function allocates the memory for the process variables.
 * It assumes that all of the parameters for the module have been
 * correctly initialised.
 */

BOOLN
InitProcessVariables_IHCRP_Zhang(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("InitProcessVariables_IHCRP_Zhang");
	int		i;
	IHCRPZhangPtr	p = iHCRPZhangPtr;
	THairCellPtr	hCRP;

	if (p->updateProcessVariablesFlag || data->updateProcessFlag) {
		/*** Additional update flags can be added to above line ***/
		FreeProcessVariables_IHCRP_Zhang();
		if ((p->hCRP = (THairCell *) calloc(_OutSig_EarObject(data)->numChannels,
		  sizeof(THairCell))) == NULL) {
		 	NotifyError(wxT("%s: Out of memory for coefficients array."),
			  funcName);
		 	return(FALSE);
		}
		p->updateProcessVariablesFlag = FALSE;
	}
	if (data->timeIndex == PROCESS_START_TIME) {
		for (i = 0; i < _OutSig_EarObject(data)->numChannels; i++) {
			hCRP = &p->hCRP[i];
			InitLowPass_Utility_Zhang(&(hCRP->hclp), _InSig_EarObject(data, 0)->dt,
			  p->cut, 1.0, p->k);
			hCRP->hcnl.A0 = p->a0;  /* Inner Hair Cell Nonlinear Function */
			hCRP->hcnl.B = p->b;
			hCRP->hcnl.C = p->c;
			hCRP->hcnl.D = p->d;
			hCRP->hcnl.Run2 = RunIHCNL2_IHCRP_Zhang;
		}
	}
	return(TRUE);

}

/****************************** FreeProcessVariables **************************/

/*
 * This routine releases the memory allocated for the process variables
 * if they have been initialised.
 */

BOOLN
FreeProcessVariables_IHCRP_Zhang(void)
{
	if (iHCRPZhangPtr->hCRP) {
		free(iHCRPZhangPtr->hCRP);
		iHCRPZhangPtr->hCRP = NULL;
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
RunModel_IHCRP_Zhang(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("RunModel_IHCRP_Zhang");
	register ChanData	 *inPtr, *outPtr;
	int		chan;
	THairCellPtr	hCRP;
	SignalDataPtr	outSignal;
	IHCRPZhangPtr	p = iHCRPZhangPtr;

	if (!data->threadRunFlag) {
		if (!CheckPars_IHCRP_Zhang())
			return(FALSE);
		if (!CheckData_IHCRP_Zhang(data)) {
			NotifyError(wxT("%s: Process data invalid."), funcName);
			return(FALSE);
		}
		SetProcessName_EarObject(data, wxT("Zhang et al. IHC Receptor ")
		  wxT("Potential."));
		if (!InitOutSignal_EarObject(data, _InSig_EarObject(data, 0)->numChannels,
		  _InSig_EarObject(data, 0)->length, _InSig_EarObject(data, 0)->dt)) {
			NotifyError(wxT("%s: Cannot initialise output channels."), funcName);
			return(FALSE);
		}

		if (!InitProcessVariables_IHCRP_Zhang(data)) {
			NotifyError(wxT("%s: Could not initialise the process variables."),
			  funcName);
			return(FALSE);
		}
		if (data->initThreadRunFlag)
			return(TRUE);
	}
	outSignal = _OutSig_EarObject(data);
	for (chan = outSignal->offset; chan < outSignal->numChannels; chan++) {
		hCRP = &p->hCRP[chan];
		inPtr = _InSig_EarObject(data, 0)->channel[chan];
		outPtr = outSignal->channel[chan];
		hCRP->hcnl.Run2(&hCRP->hcnl, inPtr, outPtr, outSignal->length);
		hCRP->hclp.Run2(&hCRP->hclp, outPtr, outPtr, outSignal->length);
	}

	SetProcessContinuity_EarObject(data);
	return(TRUE);

}

