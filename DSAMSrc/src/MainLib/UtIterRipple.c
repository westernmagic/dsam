/**********************
 *
 * File:		UtIterRipple.c
 * Purpose:		This routine transforms a signal using the Iterated Ripple
 *				algorithm as described in: "A time domain description for the
 *				pitch strength of iterated rippled noide", W.A. Yost, R.
 * 				Patterson, S. Sheft. J.Acoust. Soc. Am. 999(2), February 1996
 * Comments:	Written using ModuleProducer version 1.9 (May 27 1996).
 *				It generates two classes of Iterated Ripple signal:
 * 				IRSO Iterated Rippled Signal ADD-ORIGINAL
 * 				IRSS Iterated Rippled Signal ADD-SAME
 * Authors:		Almudena Eustaquio-Martin, L. P. O'Mard
 * Created:		04-JUN-1996.
 * Updated:	
 * Copyright:	(c) 1998, University of Essex
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
#include "UtIterRipple.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

IterRipplePtr	iterRipplePtr = NULL;

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
Free_Utility_IteratedRipple(void)
{
	/* static const char	*funcName = "Free_Utility_IteratedRipple";  */

	if (iterRipplePtr == NULL)
		return(FALSE);
	if (iterRipplePtr->parList)
		FreeList_UniParMgr(&iterRipplePtr->parList);
	if (iterRipplePtr->parSpec == GLOBAL) {
		free(iterRipplePtr);
		iterRipplePtr = NULL;
	}
	return(TRUE);

}

/****************************** InitModeList **********************************/

/*
 * This routine intialises the Mode list array.
 */

BOOLN
InitModeList_Utility_IteratedRipple(void)
{
	static NameSpecifier	modeList[] = {

					{ "IRSO", ITERRIPPLE_IRSO_MODE },
					{ "IRSS", ITERRIPPLE_IRSS_MODE },
					{ "", ITERRIPPLE_NULL }
				};
	iterRipplePtr->modeList = modeList;
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
Init_Utility_IteratedRipple(ParameterSpecifier parSpec)
{
	static const char	*funcName = "Init_Utility_IteratedRipple";

	if (parSpec == GLOBAL) {
		if (iterRipplePtr != NULL)
			Free_Utility_IteratedRipple();
		if ((iterRipplePtr = (IterRipplePtr) malloc(sizeof(IterRipple))) == NULL) {
			NotifyError("%s: Out of memory for 'global' pointer", funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (iterRipplePtr == NULL) {
			NotifyError("%s:  'local' pointer not set.", funcName);
			return(FALSE);
		}
	}
	iterRipplePtr->parSpec = parSpec;
	iterRipplePtr->numIterationsFlag = TRUE;
	iterRipplePtr->modeFlag = TRUE;
	iterRipplePtr->delayFlag = TRUE;
	iterRipplePtr->signalMultiplierFlag = TRUE;
	iterRipplePtr->numIterations = 2;
	iterRipplePtr->mode = ITERRIPPLE_IRSO_MODE;
	iterRipplePtr->delay = 0.0;
	iterRipplePtr->signalMultiplier = 1.0;

	InitModeList_Utility_IteratedRipple();
	if (!SetUniParList_Utility_IteratedRipple()) {
		NotifyError("%s: Could not initialise parameter list.", funcName);
		Free_Utility_IteratedRipple();
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
SetUniParList_Utility_IteratedRipple(void)
{
	static const char *funcName = "SetUniParList_Utility_IteratedRipple";
	UniParPtr	pars;

	if ((iterRipplePtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  UTILITY_ITERATEDRIPPLE_NUM_PARS, NULL)) == NULL) {
		NotifyError("%s: Could not initialise parList.", funcName);
		return(FALSE);
	}
	pars = iterRipplePtr->parList->pars;
	SetPar_UniParMgr(&pars[UTILITY_ITERATEDRIPPLE_NUMITERATIONS], "NUM_IT",
	  "Number of iterations.",
	  UNIPAR_INT,
	  &iterRipplePtr->numIterations, NULL,
	  (void * (*)) SetNumIterations_Utility_IteratedRipple);
	SetPar_UniParMgr(&pars[UTILITY_ITERATEDRIPPLE_MODE], "MODE",
	  "Ripple signal mode: 'IRSO' - add original, 'IRSS' - add same.",
	  UNIPAR_NAME_SPEC,
	  &iterRipplePtr->mode, iterRipplePtr->modeList,
	  (void * (*)) SetMode_Utility_IteratedRipple);
	SetPar_UniParMgr(&pars[UTILITY_ITERATEDRIPPLE_DELAY], "DELAY",
	  "Delay (s).",
	  UNIPAR_REAL,
	  &iterRipplePtr->delay, NULL,
	  (void * (*)) SetDelay_Utility_IteratedRipple);
	SetPar_UniParMgr(&pars[UTILITY_ITERATEDRIPPLE_SIGNALMULTIPLIER], "GAIN",
	  "Gain (scalar).",
	  UNIPAR_REAL,
	  &iterRipplePtr->signalMultiplier, NULL,
	  (void * (*)) SetSignalMultiplier_Utility_IteratedRipple);
	return(TRUE);

}

/****************************** GetUniParListPtr ******************************/

/*
 * This function returns a pointer to the module's universal parameter
 * list.
 */

UniParListPtr
GetUniParListPtr_Utility_IteratedRipple(void)
{
	static const char	*funcName = "GetUniParListPtr_Utility_IteratedRipple";

	if (iterRipplePtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (iterRipplePtr->parList == NULL) {
		NotifyError("%s: UniParList data structure has not been initialised. "
		  "NULL returned.", funcName);
		return(NULL);
	}
	return(iterRipplePtr->parList);

}

/****************************** SetPars ***************************************/

/*
 * This function sets all the module's parameters.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetPars_Utility_IteratedRipple(int numIterations, char *mode, double delay,
  double signalMultiplier)
{
	static const char	*funcName = "SetPars_Utility_IteratedRipple";
	BOOLN	ok;

	ok = TRUE;
	if (!SetNumIterations_Utility_IteratedRipple(numIterations))
		ok = FALSE;
	if (!SetMode_Utility_IteratedRipple(mode))
		ok = FALSE;
	if (!SetDelay_Utility_IteratedRipple(delay))
		ok = FALSE;
	if (!SetSignalMultiplier_Utility_IteratedRipple(signalMultiplier))
		ok = FALSE;
	if (!ok)
		NotifyError("%s: Failed to set all module parameters." ,funcName);
	return(ok);

}

/****************************** SetNumIterations ******************************/

/*
 * This function sets the module's numIterations parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetNumIterations_Utility_IteratedRipple(int theNumIterations)
{
	static const char	*funcName = "SetNumIterations_Utility_IteratedRipple";

	if (iterRipplePtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	if (theNumIterations <= 0) {
		NotifyError("%s: The number of iterations must be bigger the 0 (%d).", 
		  funcName, theNumIterations);
		return(FALSE);
	}	
	iterRipplePtr->numIterationsFlag = TRUE;
	iterRipplePtr->numIterations = theNumIterations;
	return(TRUE);

}

/****************************** SetMode ***************************************/

/*
 * This function sets the module's mode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetMode_Utility_IteratedRipple(char *theMode)
{
	static const char	*funcName = "SetMode_Utility_IteratedRipple";
	int		specifier;

	if (iterRipplePtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theMode,
	  iterRipplePtr->modeList)) == ITERRIPPLE_NULL) {
		NotifyError("%s: Illegal mode name (%s).", funcName, theMode);
		return(FALSE);
	}
	iterRipplePtr->modeFlag = TRUE;
	iterRipplePtr->mode = specifier;
	return(TRUE);

}

/****************************** SetDelay **************************************/

/*
 * This function sets the module's delay parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetDelay_Utility_IteratedRipple(double theDelay)
{
	static const char	*funcName = "SetDelay_Utility_IteratedRipple";

	if (iterRipplePtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	iterRipplePtr->delayFlag = TRUE;
	iterRipplePtr->delay = theDelay;
	return(TRUE);

}

/****************************** SetSignalMultiplier ***************************/

/*
 * This function sets the module's signalMultiplier parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetSignalMultiplier_Utility_IteratedRipple(double theSignalMultiplier)
{
	static const char *funcName = "SetSignalMultiplier_Utility_IteratedRipple";

	if (iterRipplePtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if ((theSignalMultiplier < -1.0) || (theSignalMultiplier > 1.0)) {
		NotifyError("%s: Value must be between (-1, +1) (%d).", funcName,
		  theSignalMultiplier);
		return(FALSE);
	}
	iterRipplePtr->signalMultiplierFlag = TRUE;
	iterRipplePtr->signalMultiplier = theSignalMultiplier;
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
CheckPars_Utility_IteratedRipple(void)
{
	static const char	*funcName = "CheckPars_Utility_IteratedRipple";
	BOOLN	ok;

	ok = TRUE;
	if (iterRipplePtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (!iterRipplePtr->numIterationsFlag) {
		NotifyError("%s: numIterations variable not set.", funcName);
		ok = FALSE;
	}
	if (!iterRipplePtr->modeFlag) {
		NotifyError("%s: mode variable not set.", funcName);
		ok = FALSE;
	}
	if (!iterRipplePtr->delayFlag) {
		NotifyError("%s: delay variable not set.", funcName);
		ok = FALSE;
	}
	if (!iterRipplePtr->signalMultiplierFlag) {
		NotifyError("%s: signalMultiplier variable not set.", funcName);
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
PrintPars_Utility_IteratedRipple(void)
{
	static const char	*funcName = "PrintPars_Utility_IteratedRipple";

	if (!CheckPars_Utility_IteratedRipple()) {
		NotifyError("%s: Parameters have not been correctly set.", funcName);
		return(FALSE);
	}
	DPrint("Iterated Ripple Utility Module Parameters:-\n");
	DPrint("\tNumber of iterations = %d,",
	  iterRipplePtr->numIterations);
	DPrint("\tNetwork mode = %s,",
	  iterRipplePtr->modeList[iterRipplePtr->mode].name);
	DPrint("\tDelay = %g (ms),\n", MSEC(iterRipplePtr->delay));
	DPrint("\tSignal multiplier = %g (units).\n",
	  iterRipplePtr->signalMultiplier);
	return(TRUE);

}

/****************************** ReadPars **************************************/

/*
 * This program reads a specified number of parameters from a file.
 * It returns FALSE if it fails in any way.n */

BOOLN
ReadPars_Utility_IteratedRipple(char *fileName)
{
	static const char	*funcName = "ReadPars_Utility_IteratedRipple";
	BOOLN	ok;
	char	*filePath;
	char	mode[MAXLINE];
	int		numIterations;
	double	delay, signalMultiplier;
	FILE	*fp;

	filePath = GetParsFileFPath_Common(fileName);
	if ((fp = fopen(filePath, "r")) == NULL) {
		NotifyError("%s: Cannot open data file '%s'.\n", funcName, filePath);
		return(FALSE);
	}
	DPrint("%s: Reading from '%s':\n", funcName, filePath);
	Init_ParFile();
	ok = TRUE;
	if (!GetPars_ParFile(fp, "%d", &numIterations))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%s", mode))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &delay))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &signalMultiplier))
		ok = FALSE;
	fclose(fp);
	Free_ParFile();
	if (!ok) {
		NotifyError("%s: Not enough lines, or invalid parameters, in module "
		  "parameter file '%s'.", funcName, filePath);
		return(FALSE);
	}
	if (!SetPars_Utility_IteratedRipple(numIterations, mode, delay,
	  signalMultiplier)) {
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
SetParsPointer_Utility_IteratedRipple(ModulePtr theModule)
{
	static const char	*funcName = "SetParsPointer_Utility_IteratedRipple";

	if (!theModule) {
		NotifyError("%s: The module is not set.", funcName);
		return(FALSE);
	}
	iterRipplePtr = (IterRipplePtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 */

BOOLN
InitModule_Utility_IteratedRipple(ModulePtr theModule)
{
	static const char	*funcName = "InitModule_Utility_IteratedRipple";

	if (!SetParsPointer_Utility_IteratedRipple(theModule)) {
		NotifyError("%s: Cannot set parameters pointer.", funcName);
		return(FALSE);
	}
	if (!Init_Utility_IteratedRipple(GLOBAL)) {
		NotifyError("%s: Could not initialise process structure.", funcName);
		return(FALSE);
	}
	theModule->parsPtr = iterRipplePtr;
	theModule->CheckPars = CheckPars_Utility_IteratedRipple;
	theModule->Free = Free_Utility_IteratedRipple;
	theModule->GetUniParListPtr = GetUniParListPtr_Utility_IteratedRipple;
	theModule->PrintPars = PrintPars_Utility_IteratedRipple;
	theModule->ReadPars = ReadPars_Utility_IteratedRipple;
	theModule->RunProcess = Process_Utility_IteratedRipple;
	theModule->SetParsPointer = SetParsPointer_Utility_IteratedRipple;
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
CheckData_Utility_IteratedRipple(EarObjectPtr data)
{
	static const char	*funcName = "CheckData_Utility_IteratedRipple";
	double signalDuration;

	if (data == NULL) {
		NotifyError("%s: EarObject not initialised.", funcName);
		return(FALSE);
	}
	if (!CheckInSignal_EarObject(data, funcName))
		return(FALSE);
	signalDuration = _GetDuration_SignalData(data->inSignal[0]);
	if (iterRipplePtr->delay > signalDuration)	{
		NotifyError("%s: Delay (%g ms) is longer than the signal duration "
		  "(%g ms)", funcName, MSEC(iterRipplePtr->delay), MSEC(
		  signalDuration));
		return(FALSE);
	}
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
Process_Utility_IteratedRipple(EarObjectPtr data)
{
	static const char	*funcName = "Process_Utility_IteratedRipple";
	register	ChanData	 *outPtr;
	int		j, chan;
	ChanLen		i, samplesDelay;

	if (!CheckPars_Utility_IteratedRipple())
		return(FALSE);
	if (!CheckData_Utility_IteratedRipple(data)) {
		NotifyError("%s: Process data invalid.", funcName);
		return(FALSE);
	}
	SetProcessName_EarObject(data, "Iterated Ripple Utility process.");
	if (!InitOutFromInSignal_EarObject(data, 0)) {
		NotifyError("%s: Could not initialise output signal.", funcName);
		return(FALSE);
	}
	switch (iterRipplePtr->mode) {
	case ITERRIPPLE_IRSO_MODE:	
		for (j = 0; j < iterRipplePtr->numIterations; j++)	{
			Delay_SignalData(data->outSignal, iterRipplePtr->delay);
			Scale_SignalData(data->outSignal, iterRipplePtr->signalMultiplier);
			Add_SignalData(data->outSignal, data->inSignal[0]);
		}	
		break;
	case ITERRIPPLE_IRSS_MODE:	
		samplesDelay = (ChanLen) ( iterRipplePtr->delay / data->outSignal->dt);
		for (j = 0; j < iterRipplePtr->numIterations; j++)	{
			for (chan = 0; chan < data->outSignal->numChannels; chan++) {
				outPtr = data->outSignal->channel[chan] +
				  data->outSignal->length - samplesDelay - 1;
				for (i = 0; i < data->outSignal->length - samplesDelay; i++,
				  outPtr--)
					*(outPtr + samplesDelay) += *outPtr *
					  iterRipplePtr->signalMultiplier;
			}
		}	
		break;
	}
	SetProcessContinuity_EarObject(data);
	return(TRUE);

}

