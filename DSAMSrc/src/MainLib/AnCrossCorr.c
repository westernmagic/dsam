/**********************
 *
 * File:		AnCrossCorr.c
 * Purpose:		This routine generates a cross-correlation function from
 *				the EarObject's input signal i.e.
 *				channel[i] * channel[i + 1]...etc.
 * Comments:	Written using ModuleProducer version 1.9.
 *				It expects a signal to be a 2N multi-channel signal, i.e. it
 *				assumes that an interleaved signal only has an
 *				interleaveLevel = 2.
 *				It overwrites previous data if the output signal has already
 *				been initialised.
 *				The function is calculated backwards from the time specified.
 *				An exponential decay function is included.
 *				The same binning as for the input signal is used.
 *				The "SignalInfo" continuity needs some work, the CF should be
 *				set automatically.
 *				02-09-97 LPO: Corrected the checking of the cross-correlation
 *				bounds in "CheckData_...".
 *				30-06-98 LPO: Introduced use of the SignalData structure's
 *				'numWindowFrames' field.
 * Author:		L. P. O'Mard & E. A. Lopez-Poveda
 * Created:		14 Feb 1996
 * Updated:		30 Jun 1998
 * Copyright:	(c) 1998, University of Essex.
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
#include "FiParFile.h"
#include "GeModuleMgr.h"
#include "AnCrossCorr.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

CrossCorrPtr	crossCorrPtr = NULL;

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
Free_Analysis_CCF(void)
{
	/* static const char	*funcName = "Free_Analysis_CCF"; */

	if (crossCorrPtr == NULL)
		return(FALSE);
	FreeProcessVariables_Analysis_CCF();
	if (crossCorrPtr->parList)
		FreeList_UniParMgr(&crossCorrPtr->parList);
	if (crossCorrPtr->parSpec == GLOBAL) {
		free(crossCorrPtr);
		crossCorrPtr = NULL;
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
Init_Analysis_CCF(ParameterSpecifier parSpec)
{
	static const char	*funcName = "Init_Analysis_CCF";

	if (parSpec == GLOBAL) {
		if (crossCorrPtr != NULL)
			Free_Analysis_CCF();
		if ((crossCorrPtr = (CrossCorrPtr) malloc(sizeof(CrossCorr))) == NULL) {
			NotifyError("%s: Out of memory for 'global' pointer", funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (crossCorrPtr == NULL) {
			NotifyError("%s:  'local' pointer not set.", funcName);
			return(FALSE);
		}
	}
	crossCorrPtr->parSpec = parSpec;
	crossCorrPtr->updateProcessVariablesFlag = TRUE;
	crossCorrPtr->timeOffsetFlag = FALSE;
	crossCorrPtr->timeConstantFlag = FALSE;
	crossCorrPtr->periodFlag = FALSE;
	crossCorrPtr->timeOffset = 0.0;
	crossCorrPtr->timeConstant = 0.0;
	crossCorrPtr->period = 0.0;
	crossCorrPtr->exponentDt = NULL;

	if (!SetUniParList_Analysis_CCF()) {
		NotifyError("%s: Could not initialise parameter list.", funcName);
		Free_Analysis_CCF();
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
SetUniParList_Analysis_CCF(void)
{
	static const char *funcName = "SetUniParList_Analysis_CCF";
	UniParPtr	pars;

	if ((crossCorrPtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  ANALYSIS_CCF_NUM_PARS, NULL)) == NULL) {
		NotifyError("%s: Could not initialise parList.", funcName);
		return(FALSE);
	}
	pars = crossCorrPtr->parList->pars;
	SetPar_UniParMgr(&pars[ANALYSIS_CCF_TIMEOFFSET], "OFFSET",
	  "Time offset, t0 (s).",
	  UNIPAR_REAL,
	  &crossCorrPtr->timeOffset, NULL,
	  (void * (*)) SetTimeOffset_Analysis_CCF);
	SetPar_UniParMgr(&pars[ANALYSIS_CCF_TIMECONSTANT], "TIME_CONST",
	  "Time constant, tw (s).",
	  UNIPAR_REAL,
	  &crossCorrPtr->timeConstant, NULL,
	  (void * (*)) SetTimeConstant_Analysis_CCF);
	SetPar_UniParMgr(&pars[ANALYSIS_CCF_PERIOD], "MAX_LAG",
	  "Maximum lag period, t (s).",
	  UNIPAR_REAL,
	  &crossCorrPtr->period, NULL,
	  (void * (*)) SetPeriod_Analysis_CCF);
	return(TRUE);

}

/****************************** GetUniParListPtr ******************************/

/*
 * This function returns a pointer to the module's universal parameter
 * list.
 */

UniParListPtr
GetUniParListPtr_Analysis_CCF(void)
{
	static const char	*funcName = "GetUniParListPtr_Analysis_CCF";

	if (crossCorrPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (crossCorrPtr->parList == NULL) {
		NotifyError("%s: UniParList data structure has not been initialised. "
		  "NULL returned.", funcName);
		return(NULL);
	}
	return(crossCorrPtr->parList);

}

/****************************** SetPars ***************************************/

/*
 * This function sets all the module's parameters.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetPars_Analysis_CCF(double timeOffset, double timeConstant,
  double period)
{
	static const char	*funcName = "SetPars_Analysis_CCF";
	BOOLN	ok;

	ok = TRUE;
	if (!SetTimeOffset_Analysis_CCF(timeOffset))
		ok = FALSE;
	if (!SetTimeConstant_Analysis_CCF(timeConstant))
		ok = FALSE;
	if (!SetPeriod_Analysis_CCF(period))
		ok = FALSE;
	if (!ok)
		NotifyError("%s: Failed to set all module parameters." ,funcName);
	return(ok);

}

/****************************** SetTimeOffset *********************************/

/*
 * This function sets the module's timeOffset parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetTimeOffset_Analysis_CCF(double theTimeOffset)
{
	static const char	*funcName = "SetTimeOffset_Analysis_CCF";

	if (crossCorrPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (theTimeOffset < 0.0) {
		NotifyError("%s: Value must be greater than zero (%g s)", funcName,
		  theTimeOffset);
		return(FALSE);
	}
	crossCorrPtr->timeOffsetFlag = TRUE;
	crossCorrPtr->timeOffset = theTimeOffset;
	return(TRUE);

}

/****************************** SetTimeConstant *******************************/

/*
 * This function sets the module's timeConstant parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetTimeConstant_Analysis_CCF(double theTimeConstant)
{
	static const char	*funcName = "SetTimeConstant_Analysis_CCF";

	if (crossCorrPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (theTimeConstant < 0.0) {
		NotifyError("%s: Value must be greater than zero (%g s)", funcName,
		  theTimeConstant);
		return(FALSE);
	}
	crossCorrPtr->timeConstantFlag = TRUE;
	crossCorrPtr->timeConstant = theTimeConstant;
	return(TRUE);

}

/****************************** SetPeriod *************************************/

/*
 * This function sets the module's period parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetPeriod_Analysis_CCF(double thePeriod)
{
	static const char	*funcName = "SetPeriod_Analysis_CCF";

	if (crossCorrPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (thePeriod < 0.0) {
		NotifyError("%s: Value must be greater than zero (%g s)", funcName,
		  thePeriod);
		return(FALSE);
	}
	crossCorrPtr->periodFlag = TRUE;
	crossCorrPtr->period = thePeriod;
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
CheckPars_Analysis_CCF(void)
{
	static const char	*funcName = "CheckPars_Analysis_CCF";
	BOOLN	ok;

	ok = TRUE;
	if (crossCorrPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (!crossCorrPtr->timeOffsetFlag) {
		NotifyError("%s: timeOffset variable not set.", funcName);
		ok = FALSE;
	}
	if (!crossCorrPtr->timeConstantFlag) {
		NotifyError("%s: timeConstant variable not set.", funcName);
		ok = FALSE;
	}
	if (!crossCorrPtr->periodFlag) {
		NotifyError("%s: period variable not set.", funcName);
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
PrintPars_Analysis_CCF(void)
{
	static const char	*funcName = "PrintPars_Analysis_CCF";

	if (!CheckPars_Analysis_CCF()) {
		NotifyError("%s: Parameters have not been correctly set.", funcName);
		return(FALSE);
	}
	DPrint("Cross-correlation Analysis Module "
	  "Parameters:-\n");
	DPrint("\tTime Offset = ");
	if (crossCorrPtr->timeOffset < 0.0)
		DPrint("end of signal,\n");
	else
		DPrint("%g ms,\n", MSEC(crossCorrPtr->timeOffset));
	DPrint("\tTime constant = %g ms,\t",
	  MSEC(crossCorrPtr->timeConstant));
	DPrint("\tPeriod = %g ms\n", MSEC(crossCorrPtr->period));
	return(TRUE);

}

/****************************** ReadPars **************************************/

/*
 * This program reads a specified number of parameters from a file.
 * It returns FALSE if it fails in any way.n */

BOOLN
ReadPars_Analysis_CCF(char *fileName)
{
	static const char	*funcName = "ReadPars_Analysis_CCF";
	BOOLN	ok;
	char	*filePath;
	double	timeOffset, timeConstant, period;
	FILE	*fp;

	filePath = GetParsFileFPath_Common(fileName);
	if ((fp = fopen(filePath, "r")) == NULL) {
		NotifyError("%s: Cannot open data file '%s'.\n", funcName, filePath);
		return(FALSE);
	}
	DPrint("%s: Reading from '%s':\n", funcName, filePath);
	Init_ParFile();
	ok = TRUE;
	if (!GetPars_ParFile(fp, "%lf", &timeOffset))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &timeConstant))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%lf", &period))
		ok = FALSE;
	fclose(fp);
	Free_ParFile();
	if (!ok) {
		NotifyError("%s: Not enough lines, or invalid parameters, in module "
		  "parameter file '%s'.", funcName, filePath);
		return(FALSE);
	}
	if (!SetPars_Analysis_CCF(timeOffset, timeConstant, period)) {
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
SetParsPointer_Analysis_CCF(ModulePtr theModule)
{
	static const char	*funcName = "SetParsPointer_Analysis_CCF";

	if (!theModule) {
		NotifyError("%s: The module is not set.", funcName);
		return(FALSE);
	}
	crossCorrPtr = (CrossCorrPtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 */

BOOLN
InitModule_Analysis_CCF(ModulePtr theModule)
{
	static const char	*funcName = "InitModule_Analysis_CCF";

	if (!SetParsPointer_Analysis_CCF(theModule)) {
		NotifyError("%s: Cannot set parameters pointer.", funcName);
		return(FALSE);
	}
	if (!Init_Analysis_CCF(GLOBAL)) {
		NotifyError("%s: Could not initialise process structure.", funcName);
		return(FALSE);
	}
	theModule->parsPtr = crossCorrPtr;
	theModule->CheckPars = CheckPars_Analysis_CCF;
	theModule->Free = Free_Analysis_CCF;
	theModule->GetUniParListPtr = GetUniParListPtr_Analysis_CCF;
	theModule->PrintPars = PrintPars_Analysis_CCF;
	theModule->ReadPars = ReadPars_Analysis_CCF;
	theModule->RunProcess = Calc_Analysis_CCF;
	theModule->SetParsPointer = SetParsPointer_Analysis_CCF;
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
 * A specific check for when timeOffset < 0.0 (set to signal duration) need
 * not be made.
 */

BOOLN
CheckData_Analysis_CCF(EarObjectPtr data)
{
	static const char	*funcName = "CheckData_Analysis_CCF";
	double	signalDuration;

	if (data == NULL) {
		NotifyError("%s: EarObject not initialised.", funcName);
		return(FALSE);
	}
	if (!CheckInit_SignalData(data->inSignal[0], "CheckData_Analysis_CCF"))
		return(FALSE);
	signalDuration = _GetDuration_SignalData(data->inSignal[0]);
	if (crossCorrPtr->timeOffset > signalDuration) {
		NotifyError("%s: Time offset is longer than signal duration.",
		  funcName);
		return(FALSE);
	}		
	if ((crossCorrPtr->timeOffset - crossCorrPtr->period * 2.0 < 0.0) ||
	  (crossCorrPtr->timeOffset + crossCorrPtr->period > signalDuration)) {
		NotifyError("%s: cross-correlation period extends outside the range "
		  "of the signal.", funcName);
		return(FALSE);
	}		
	if (data->inSignal[0]->numChannels % 2 != 0) {
		NotifyError("%s: Number of channels must be a factor of 2.", funcName);
		return(FALSE);
	}
	return(TRUE);

}

/**************************** InitProcessVariables ****************************/

/*
 * This routine initialises the exponential table.
 * It assumes that all of the parameters for the module have been correctly
 * initialised.
 */

BOOLN
InitProcessVariables_Analysis_CCF(EarObjectPtr data)
{
	static const char *funcName = "InitProcessVariables_Analysis_CCF";
	
	double	*expDtPtr, dt;
	ChanLen	i, maxPeriodIndex;
	
	if (crossCorrPtr->updateProcessVariablesFlag || data->updateProcessFlag) {
		FreeProcessVariables_Analysis_CCF();
		dt = data->inSignal[0]->dt;
		maxPeriodIndex = (ChanLen) (crossCorrPtr->period / dt);
		if ((crossCorrPtr->exponentDt = (double *) calloc(maxPeriodIndex,
		  sizeof(double))) == NULL) {
			NotifyError("%s: Out of memory for exponent lookup table.",
			  funcName);
			return(FALSE);
		}
		SetNumWindowFrames_SignalData(data->outSignal, 0);
		for (i = 0, expDtPtr = crossCorrPtr->exponentDt; i < maxPeriodIndex;
		  i++, expDtPtr++)
			*expDtPtr = exp( -(i * dt) / crossCorrPtr->timeConstant);
		crossCorrPtr->updateProcessVariablesFlag = FALSE;
	}
	return(TRUE);

}

/**************************** FreeProcessVariables ****************************/

/*
 * This routine releases the memory allocated for the process variables
 * if they have been initialised.
 */

void
FreeProcessVariables_Analysis_CCF(void)
{
	if (crossCorrPtr->exponentDt) {
		free(crossCorrPtr->exponentDt);
		crossCorrPtr->exponentDt = NULL;
	}
	crossCorrPtr->updateProcessVariablesFlag = TRUE;
}

/****************************** Calc ******************************************/

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
Calc_Analysis_CCF(EarObjectPtr data)
{
	static const char	*funcName = "Calc_Analysis_CCF";
	register	double	*expDtPtr;
	register	ChanData	 *inPtrL, *inPtrR, *outPtr;
	int		chan;
	long	deltaT;
	double	dt;
	ChanLen	i, periodIndex, totalPeriodIndex, timeOffsetIndex;

	if (!CheckPars_Analysis_CCF())
		return(FALSE);
	if (!CheckData_Analysis_CCF(data)) {
		NotifyError("%s: Process data invalid.", funcName);
		return(FALSE);
	}
	SetProcessName_EarObject(data, "Cross Correlation Function (CCF) analysis");
	dt = data->inSignal[0]->dt;
	periodIndex = (ChanLen) (crossCorrPtr->period / dt + 0.5);
	totalPeriodIndex = periodIndex * 2 + 1;
	if (!InitOutSignal_EarObject(data, data->inSignal[0]->numChannels / 2,
	  totalPeriodIndex, dt)) {
		NotifyError("%s: Cannot initialise output channels.", funcName);
		return(FALSE);
	}
	data->outSignal->interleaveLevel = 1;
	SetLocalInfoFlag_SignalData(data->outSignal, TRUE);
	for (chan = 0; chan < data->outSignal->numChannels; chan++)
		data->outSignal->info.cFArray[chan] =
		  data->inSignal[0]->info.cFArray[chan];
	SetInfoSampleTitle_SignalData(data->outSignal, "Delay period (s)");
	SetStaticTimeFlag_SignalData(data->outSignal, TRUE);
	SetOutputTimeOffset_SignalData(data->outSignal, -crossCorrPtr->period);
	timeOffsetIndex = (ChanLen) (crossCorrPtr->timeOffset / dt);
	if (!InitProcessVariables_Analysis_CCF(data)) {
		NotifyError("%s: Could not initialise the process variables.",
		  funcName);
		return(FALSE);
	}
	for (chan = 0; chan < data->inSignal[0]->numChannels; chan += 2) {
		outPtr = data->outSignal->channel[chan / 2];
		for (deltaT = -((long) periodIndex); deltaT <= (long) periodIndex;
		  deltaT++, outPtr++) {
			inPtrL = data->inSignal[0]->channel[chan] + timeOffsetIndex;
			inPtrR = data->inSignal[0]->channel[chan + 1] + timeOffsetIndex;
			for (i = 0, *outPtr = 0.0, expDtPtr = crossCorrPtr->exponentDt;
			  i < periodIndex; i++, expDtPtr++, inPtrL--, inPtrR--)
				*outPtr += *inPtrL * *(inPtrR - deltaT) * *expDtPtr;
			*outPtr /= periodIndex;
		}
	}
	data->outSignal->numWindowFrames++;
	SetProcessContinuity_EarObject(data);
	return(TRUE);

}

