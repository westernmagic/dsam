/**********************
 *
 * File:		UtMathOp.c
 * Purpose:		This utility carries out simple mathematical operations.
 * Comments:	Written using ModuleProducer version 1.4.0 (Mar 14 2002).
 * Author:		L.  P. O'Mard
 * Created:		08 Jul 2002
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
#include "UtMathOp.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

MathOpPtr	mathOpPtr = NULL;

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
Free_Utility_MathOp(void)
{
	/* static const char	*funcName = "Free_Utility_MathOp";  */

	if (mathOpPtr == NULL)
		return(FALSE);
	if (mathOpPtr->parList)
		FreeList_UniParMgr(&mathOpPtr->parList);
	if (mathOpPtr->parSpec == GLOBAL) {
		free(mathOpPtr);
		mathOpPtr = NULL;
	}
	return(TRUE);

}

/****************************** InitOperatorModeList **************************/

/*
 * This function initialises the 'operatorMode' list array
 */

BOOLN
InitOperatorModeList_Utility_MathOp(void)
{
	static NameSpecifier	modeList[] = {

			{ "ADD",		UTILITY_MATHOP_OPERATORMODE_ADD },
			{ "ABSOLUTE",	UTILITY_MATHOP_OPERATORMODE_ABSOLUTE },
			{ "SCALE",		UTILITY_MATHOP_OPERATORMODE_SCALE },
			{ "SQR",		UTILITY_MATHOP_OPERATORMODE_SQR },
			{ "SUBTRACT",	UTILITY_MATHOP_OPERATORMODE_SUBTRACT },
			{ "",	UTILITY_MATHOP_OPERATORMODE_SUBTRACT },
		};
	mathOpPtr->operatorModeList = modeList;
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
Init_Utility_MathOp(ParameterSpecifier parSpec)
{
	static const char	*funcName = "Init_Utility_MathOp";

	if (parSpec == GLOBAL) {
		if (mathOpPtr != NULL)
			Free_Utility_MathOp();
		if ((mathOpPtr = (MathOpPtr) malloc(sizeof(MathOp))) == NULL) {
			NotifyError("%s: Out of memory for 'global' pointer", funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (mathOpPtr == NULL) {
			NotifyError("%s:  'local' pointer not set.", funcName);
			return(FALSE);
		}
	}
	mathOpPtr->parSpec = parSpec;
	mathOpPtr->operatorModeFlag = TRUE;
	mathOpPtr->operandFlag = TRUE;
	mathOpPtr->operatorMode = UTILITY_MATHOP_OPERATORMODE_SCALE;
	mathOpPtr->operand = 1.0;

	InitOperatorModeList_Utility_MathOp();
	if (!SetUniParList_Utility_MathOp()) {
		NotifyError("%s: Could not initialise parameter list.", funcName);
		Free_Utility_MathOp();
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
SetUniParList_Utility_MathOp(void)
{
	static const char *funcName = "SetUniParList_Utility_MathOp";
	UniParPtr	pars;

	if ((mathOpPtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  UTILITY_MATHOP_NUM_PARS, NULL)) == NULL) {
		NotifyError("%s: Could not initialise parList.", funcName);
		return(FALSE);
	}
	pars = mathOpPtr->parList->pars;
	SetPar_UniParMgr(&pars[UTILITY_MATHOP_OPERATORMODE], "OPERATOR",
	  "Mathematical operator ('add', 'modulus', 'scale', 'sqr' or 'subtract').",
	  UNIPAR_NAME_SPEC,
	  &mathOpPtr->operatorMode, mathOpPtr->operatorModeList,
	  (void * (*)) SetOperatorMode_Utility_MathOp);
	SetPar_UniParMgr(&pars[UTILITY_MATHOP_OPERAND], "OPERAND",
	  "Operand (only used in scale mode at present).",
	  UNIPAR_REAL,
	  &mathOpPtr->operand, NULL,
	  (void * (*)) SetOperand_Utility_MathOp);
	return(TRUE);

}

/****************************** GetUniParListPtr ******************************/

/*
 * This function returns a pointer to the module's universal parameter
 * list.
 */

UniParListPtr
GetUniParListPtr_Utility_MathOp(void)
{
	static const char	*funcName = "GetUniParListPtr_Utility_MathOp";

	if (mathOpPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (mathOpPtr->parList == NULL) {
		NotifyError("%s: UniParList data structure has not been initialised. "
		  "NULL returned.", funcName);
		return(NULL);
	}
	return(mathOpPtr->parList);

}

/****************************** SetOperatorMode *******************************/

/*
 * This function sets the module's operatorMode parameter.
 * It returns TRUE if the OPERATORMODE is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetOperatorMode_Utility_MathOp(char * theOperatorMode)
{
	static const char	*funcName = "SetOperatorMode_Utility_MathOp";
	int		specifier;

	if (mathOpPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theOperatorMode,
		mathOpPtr->operatorModeList)) == UTILITY_MATHOP_OPERATORMODE_NULL) {
		NotifyError("%s: Illegal name (%s).", funcName, theOperatorMode);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	mathOpPtr->operatorModeFlag = TRUE;
	mathOpPtr->operatorMode = specifier;
	mathOpPtr->parList->pars[UTILITY_MATHOP_OPERAND].enabled = (mathOpPtr->
	  operatorMode == UTILITY_MATHOP_OPERATORMODE_SCALE);
	mathOpPtr->parList->updateFlag = TRUE;
	return(TRUE);

}

/****************************** SetOperand ************************************/

/*
 * This function sets the module's operand parameter.
 * It returns TRUE if the OPERATORMODE is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetOperand_Utility_MathOp(double theOperand)
{
	static const char	*funcName = "SetOperand_Utility_MathOp";

	if (mathOpPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	mathOpPtr->operandFlag = TRUE;
	mathOpPtr->operand = theOperand;
	return(TRUE);

}

/****************************** CheckPars *************************************/

/*
 * This routine checks that the necessary parameters for the module
 * have been correctly initialised.
 * Other 'OPERATORMODEal' tests which can only be done when all
 * parameters are present, should also be carried out here.
 * It returns TRUE if there are no problems.
 */

BOOLN
CheckPars_Utility_MathOp(void)
{
	static const char	*funcName = "CheckPars_Utility_MathOp";
	BOOLN	ok;

	ok = TRUE;
	if (mathOpPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (!mathOpPtr->operatorModeFlag) {
		NotifyError("%s: operatorMode variable not set.", funcName);
		ok = FALSE;
	}
	if (!mathOpPtr->operandFlag) {
		NotifyError("%s: operand variable not set.", funcName);
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
PrintPars_Utility_MathOp(void)
{
	static const char	*funcName = "PrintPars_Utility_MathOp";

	if (!CheckPars_Utility_MathOp()) {
		NotifyError("%s: Parameters have not been correctly set.", funcName);
		return(FALSE);
	}
	DPrint("Mathematicl Operation Utility Module Parameters:-\n");
	DPrint("\tOperatorMode = %s, ", mathOpPtr->operatorModeList[mathOpPtr->
	  operatorMode].name);
	if (mathOpPtr->operatorMode == UTILITY_MATHOP_OPERATORMODE_SCALE)
		DPrint("\tOperand = %g (units).\n", mathOpPtr->operand);
	else
		DPrint("\n");
	return(TRUE);

}

/****************************** SetParsPointer ********************************/

/*
 * This function sets the global parameter pointer for the module to
 * that associated with the module instance.
 */

BOOLN
SetParsPointer_Utility_MathOp(ModulePtr theModule)
{
	static const char	*funcName = "SetParsPointer_Utility_MathOp";

	if (!theModule) {
		NotifyError("%s: The module is not set.", funcName);
		return(FALSE);
	}
	mathOpPtr = (MathOpPtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 * It also initialises the process structure.
 */

BOOLN
InitModule_Utility_MathOp(ModulePtr theModule)
{
	static const char	*funcName = "InitModule_Utility_MathOp";

	if (!SetParsPointer_Utility_MathOp(theModule)) {
		NotifyError("%s: Cannot set parameters pointer.", funcName);
		return(FALSE);
	}
	if (!Init_Utility_MathOp(GLOBAL)) {
		NotifyError("%s: Could not initialise process structure.", funcName);
		return(FALSE);
	}
	theModule->parsPtr = mathOpPtr;
	theModule->CheckPars = CheckPars_Utility_MathOp;
	theModule->Free = Free_Utility_MathOp;
	theModule->GetUniParListPtr = GetUniParListPtr_Utility_MathOp;
	theModule->PrintPars = PrintPars_Utility_MathOp;
	theModule->RunProcess = Process_Utility_MathOp;
	theModule->SetParsPointer = SetParsPointer_Utility_MathOp;
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
CheckData_Utility_MathOp(EarObjectPtr data)
{
	static const char	*funcName = "CheckData_Utility_MathOp";

	if (data == NULL) {
		NotifyError("%s: EarObject not initialised.", funcName);
		return(FALSE);
	}
	if (!CheckInit_SignalData(data->inSignal[0], funcName))
		return(FALSE);

	if ((mathOpPtr->operatorMode == UTILITY_MATHOP_OPERATORMODE_ADD) ||
	  (mathOpPtr->operatorMode == UTILITY_MATHOP_OPERATORMODE_SUBTRACT)) {
		if (data->maxInSignals != 2) {
			NotifyError("%s: Process must be receive 2 inputs in '%s' mode.",
			  funcName, mathOpPtr->operatorModeList[mathOpPtr-> operatorMode].
			  name);
			return(FALSE);
		}
		if (!CheckPars_SignalData(data->inSignal[1])) {
			NotifyError("%s: Input signals not correctly set.", funcName);		
			return(FALSE);
		}
		if (!SameType_SignalData(data->inSignal[0], data->inSignal[1])) {
			NotifyError("%s: Input signals are not the same.", funcName);		
			return(FALSE);
		}
		if (data->inSignal[0]->interleaveLevel != data->inSignal[1]->
		  interleaveLevel) {
			NotifyError("%s: Input signals do not have the same interleave "
			  "level.", funcName);		
			return(FALSE);
		}
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
Process_Utility_MathOp(EarObjectPtr data)
{
	static const char	*funcName = "Process_Utility_MathOp";
	register ChanData	 *inPtr1, *inPtr2 = NULL, *outPtr;
	int		chan;
	ChanLen	i;

	if (data == NULL) {
		NotifyError("%s: EarObject not initialised.", funcName);
		return(FALSE);
	}
	if (!CheckPars_Utility_MathOp())
		return(FALSE);
	if (!CheckData_Utility_MathOp(data)) {
		NotifyError("%s: Process data invalid.", funcName);
		return(FALSE);
	}
	SetProcessName_EarObject(data, "Mathematical operation module process");

	/*** Example Initialise output signal - ammend/change if required. ***/
	if (!InitOutSignal_EarObject(data, data->inSignal[0]->numChannels,
	  data->inSignal[0]->length, data->inSignal[0]->dt)) {
		NotifyError("%s: Cannot initialise output channels.", funcName);
		return(FALSE);
	}

	for (chan = 0; chan < data->inSignal[0]->numChannels; chan++) {
		inPtr1 = data->inSignal[0]->channel[chan];
		if ((mathOpPtr->operatorMode == UTILITY_MATHOP_OPERATORMODE_ADD) ||
		  (mathOpPtr->operatorMode == UTILITY_MATHOP_OPERATORMODE_SUBTRACT))
			inPtr2 = data->inSignal[1]->channel[chan];
		outPtr = data->outSignal->channel[chan];
		switch (mathOpPtr->operatorMode) {
		case UTILITY_MATHOP_OPERATORMODE_ADD:
			for (i = 0; i < data->outSignal->length; i++)
				*outPtr++ = *inPtr1++ + *inPtr2++;
			break;
		case UTILITY_MATHOP_OPERATORMODE_ABSOLUTE:
			for (i = 0; i < data->outSignal->length; i++)
				*outPtr++ = fabs(*inPtr1++);
			break;
		case UTILITY_MATHOP_OPERATORMODE_SCALE:
			for (i = 0; i < data->outSignal->length; i++)
				*outPtr++ = *inPtr1++ * mathOpPtr->operand;
			break;
		case UTILITY_MATHOP_OPERATORMODE_SQR:
			for (i = 0; i < data->outSignal->length; i++, inPtr1++)
				*outPtr++ = SQR(*inPtr1);
			break;
		case UTILITY_MATHOP_OPERATORMODE_SUBTRACT:
			for (i = 0; i < data->outSignal->length; i++)
				*outPtr++ = *inPtr1++ - *inPtr2++;
			break;
		default:
			;
		}
	}

	SetProcessContinuity_EarObject(data);
	return(TRUE);

}

