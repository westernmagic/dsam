/**********************
 *
 * File:		UtReduceDt.c
 * Purpose:		This routine produces an output signal whose sampling rate is
 *				increased by reducing the sampling interval of the input signal
 *				by a specified denominator.
 * Comments:	Written using ModuleProducer version 1.8.
 *				The length of the signal will be increased accordingly.
 *				A new output signal is created, if required, or the old output
 *				signal is overwritten.
 * Author:		L. P. O'Mard
 * Created:		21 Dec 1995
 * Updated:	
 * Copyright:	(c) 1998, University of Essex
 *
**********************/

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
#include "UtReduceDt.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

ReduceDtPtr	reduceDtPtr = NULL;

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
Free_Utility_ReduceDt(void)
{
	/* static const char	*funcName = "Free_Utility_ReduceDt"; */

	if (reduceDtPtr == NULL)
		return(FALSE);
	if (reduceDtPtr->parList)
		FreeList_UniParMgr(&reduceDtPtr->parList);
	if (reduceDtPtr->parSpec == GLOBAL) {
		free(reduceDtPtr);
		reduceDtPtr = NULL;
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
Init_Utility_ReduceDt(ParameterSpecifier parSpec)
{
	static const char	*funcName = "Init_Utility_ReduceDt";

	if (parSpec == GLOBAL) {
		if (reduceDtPtr != NULL)
			Free_Utility_ReduceDt();
		if ((reduceDtPtr = (ReduceDtPtr) malloc(sizeof(ReduceDt))) == NULL) {
			NotifyError("%s: Out of memory for 'global' pointer", funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (reduceDtPtr == NULL) {
			NotifyError("%s:  'local' pointer not set.", funcName);
			return(FALSE);
		}
	}
	reduceDtPtr->parSpec = parSpec;
	reduceDtPtr->denominatorFlag = TRUE;
	reduceDtPtr->denominator = 1;

	if (!SetUniParList_Utility_ReduceDt()) {
		NotifyError("%s: Could not initialise parameter list.", funcName);
		Free_Utility_ReduceDt();
		return(FALSE);
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
SetUniParList_Utility_ReduceDt(void)
{
	static const char *funcName = "SetUniParList_Utility_ReduceDt";
	UniParPtr	pars;

	if ((reduceDtPtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  UTILITY_REDUCEDT_NUM_PARS, NULL)) == NULL) {
		NotifyError("%s: Could not initialise parList.", funcName);
		return(FALSE);
	}
	pars = reduceDtPtr->parList->pars;
	SetPar_UniParMgr(&pars[UTILITY_REDUCEDT_DENOMINATOR], "DENOMINATOR",
	  "Reduction denominator (integer).",
	  UNIPAR_INT,
	  &reduceDtPtr->denominator, NULL,
	  (void * (*)) SetDenominator_Utility_ReduceDt);

	SetAltAbbreviation_UniParMgr(&pars[UTILITY_REDUCEDT_DENOMINATOR],
	  "DENOMINTOR");
	return(TRUE);

}

/****************************** GetUniParListPtr ******************************/

/*
 * This function returns a pointer to the module's universal parameter
 * list.
 */

UniParListPtr
GetUniParListPtr_Utility_ReduceDt(void)
{
	static const char	*funcName = "GetUniParListPtr_Utility_ReduceDt";

	if (reduceDtPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (reduceDtPtr->parList == NULL) {
		NotifyError("%s: UniParList data structure has not been initialised. "
		  "NULL returned.", funcName);
		return(NULL);
	}
	return(reduceDtPtr->parList);

}

/****************************** SetPars ***************************************/

/*
 * This function sets all the module's parameters.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetPars_Utility_ReduceDt(int denominator)
{
	static const char	*funcName = "SetPars_Utility_ReduceDt";
	BOOLN	ok;

	ok = TRUE;
	if (!SetDenominator_Utility_ReduceDt(denominator))
		ok = FALSE;
	if (!ok)
		NotifyError("%s: Failed to set all module parameters." ,funcName);
	return(ok);

}

/****************************** SetDenominator ********************************/

/*
 * This function sets the module's denominator parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetDenominator_Utility_ReduceDt(int theDenominator)
{
	static const char	*funcName =
	  "SetDenominator_Utility_ReduceDt";

	if (reduceDtPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (theDenominator < 1) {
		NotifyError("%s: Illegal denomitor = %d; valid values are\ngreater "\
		  "than 0.", funcName, theDenominator);
		return(FALSE);
	}
	reduceDtPtr->denominatorFlag = TRUE;
	reduceDtPtr->denominator = theDenominator;
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
CheckPars_Utility_ReduceDt(void)
{
	static const char	*funcName = "CheckPars_Utility_ReduceDt";
	BOOLN	ok;

	ok = TRUE;
	if (reduceDtPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (!reduceDtPtr->denominatorFlag) {
		NotifyError("%s: denominator variable not set.", funcName);
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
PrintPars_Utility_ReduceDt(void)
{
	static const char	*funcName = "PrintPars_Utility_ReduceDt";

	if (!CheckPars_Utility_ReduceDt()) {
		NotifyError("%s: Parameters have not been correctly set.", funcName);
		return(FALSE);
	}
	DPrint("Reduce Sampling Interval, dt, Utility Module "
	  "Parameters:-\n");
	DPrint("\tReduction denominator = %d.\n",
	  reduceDtPtr->denominator);
	return(TRUE);

}

/****************************** ReadPars **************************************/

/*
 * This program reads a specified number of parameters from a file.
 * It returns FALSE if it fails in any way.n */

BOOLN
ReadPars_Utility_ReduceDt(char *fileName)
{
	static const char	*funcName = "ReadPars_Utility_ReduceDt";
	BOOLN	ok;
	char	*filePath;
	int		denominator;
	FILE	*fp;

	filePath = GetParsFileFPath_Common(fileName);
	if ((fp = fopen(filePath, "r")) == NULL) {
		NotifyError("%s: Cannot open data file '%s'.\n", funcName, filePath);
		return(FALSE);
	}
	DPrint("%s: Reading from '%s':\n", funcName, filePath);
	Init_ParFile();
	ok = TRUE;
	if (!GetPars_ParFile(fp, "%d", &denominator))
		ok = FALSE;
	fclose(fp);
	Free_ParFile();
	if (!ok) {
		NotifyError("%s: Not enough lines, or invalid parameters, in module "
		  "parameter file '%s'.", funcName, filePath);
		return(FALSE);
	}
	if (!SetPars_Utility_ReduceDt(denominator)) {
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
SetParsPointer_Utility_ReduceDt(ModulePtr theModule)
{
	static const char	*funcName = "SetParsPointer_Utility_ReduceDt";

	if (!theModule) {
		NotifyError("%s: The module is not set.", funcName);
		return(FALSE);
	}
	reduceDtPtr = (ReduceDtPtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 */

BOOLN
InitModule_Utility_ReduceDt(ModulePtr theModule)
{
	static const char	*funcName = "InitModule_Utility_ReduceDt";

	if (!SetParsPointer_Utility_ReduceDt(theModule)) {
		NotifyError("%s: Cannot set parameters pointer.", funcName);
		return(FALSE);
	}
	if (!Init_Utility_ReduceDt(GLOBAL)) {
		NotifyError("%s: Could not initialise process structure.", funcName);
		return(FALSE);
	}
	theModule->parsPtr = reduceDtPtr;
	theModule->CheckPars = CheckPars_Utility_ReduceDt;
	theModule->Free = Free_Utility_ReduceDt;
	theModule->GetUniParListPtr = GetUniParListPtr_Utility_ReduceDt;
	theModule->PrintPars = PrintPars_Utility_ReduceDt;
	theModule->ReadPars = ReadPars_Utility_ReduceDt;
	theModule->RunProcess = Process_Utility_ReduceDt;
	theModule->SetParsPointer = SetParsPointer_Utility_ReduceDt;
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
CheckData_Utility_ReduceDt(EarObjectPtr data)
{
	static const char	*funcName = "CheckData_Utility_ReduceDt";

	if (data == NULL) {
		NotifyError("%s: EarObject not initialised.", funcName);
		return(FALSE);
	}
	if (!CheckInSignal_EarObject(data, funcName))
		return(FALSE);
	/*** Put additional checks here. ***/
	return(TRUE);

}

/****************************** Process ***************************************/

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
Process_Utility_ReduceDt(EarObjectPtr data)
{
	static const char	*funcName = "Process_Utility_ReduceDt";
	register	ChanData	 *inPtr, *outPtr;
	int		chan, j;
	double	gradient;
	ChanLen	i;

	if (!CheckPars_Utility_ReduceDt())
		return(FALSE);
	if (!CheckData_Utility_ReduceDt(data)) {
		NotifyError("%s: Process data invalid.", funcName);
		return(FALSE);
	}
	SetProcessName_EarObject(data, "Module process ??");
	if (!InitOutSignal_EarObject(data, data->inSignal[0]->numChannels,
	  data->inSignal[0]->length * reduceDtPtr->denominator,
	  data->inSignal[0]->dt / reduceDtPtr->denominator)) {
		NotifyError("%s: Cannot initialise output channels.", funcName);
		return(FALSE);
	}	
	for (chan = 0; chan < data->inSignal[0]->numChannels; chan++) {
		gradient = 0.0;
		inPtr = data->inSignal[0]->channel[chan];
		outPtr = data->outSignal->channel[chan];
		for (i = 0; i < data->inSignal[0]->length - 1; i++, inPtr++) {
			gradient = (*(inPtr + 1) - *inPtr) / reduceDtPtr->denominator;
			for (j = 0; j < reduceDtPtr->denominator; j++)
				*(outPtr++) = *inPtr + gradient * j;
		}
		/* Use the previous gradient for the last input sample. */
		for (j = 0; j < reduceDtPtr->denominator; j++)
			*(outPtr++) = *inPtr + gradient * j;
	}
	SetUtilityProcessContinuity_EarObject(data);
	SetProcessContinuity_EarObject(data);
	return(TRUE);

}

