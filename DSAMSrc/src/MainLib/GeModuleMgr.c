/******************
 *
 * File:		GeModuleMgr.c
 * Purpose:		This module contains the module managment routines.
 * Comments:	These routines carry out various functions such as freeing
 *				all module memory space, and so on.
 *				17-04-98 LPO: Added the NULL_MODULE_NAME string constant.
 *				22-04-98 LPO: Changed all the stimulus module names so that they
 *				start with "Stim_".
 *				10-12-98 LPO: Changed the old 'NullFunction_...' to
 *				'TrueFunction_...' which alwaysreturns a TRUE value.  I have
 *				created a new 'NullFunction_...' which always returns the
 *				correct NULL (zero) value.
 *				05-03-99 LPO: The 'SetParsPointer_' routine now checks for a
 *				NULL module passed as an argument.
 *				18-03-99 LPO: I have added the 'LinkGlueRoutine'.  This routine
 *				is not meant to be used, but it is to access global variables
 *				of modules that wouldn't otherwise be linked.
 *				20-04-99 LPO: The "Free_ModuleMgr" routine was not setting the
 *				appropriate module parameters pointer using the 
 *				"SetParsPointer_" routine.
 *				27-04-99 LPO:  The universal parameter lists have allowed me
 *				to introduce the 'ReadPar_', 'PrintPar_' and 'RunProcess'
 *				'SetPar_' commands.  These commands replace the previous
 *				generic programming interface.  It will also mean that the
 *				ModuleMgr module can be automatically generated.
 *				10-05-99 LPO: Introduced the 'ReadSimParFile_' routine.  There
 *				is only one module which uses this routine, but since it may
 *				be used quite alot in programs, it is best to put it in.
 *				02-06-99 LPO: In line with my drive to remove the 'DoFun'
 *				interface I have created the 'SetRealPar_', 'GetCFListPtr_' and
 *				'GetSimulation_' routines.  These routines will only work with
 *				modules that have these functions.
 *				22-10-99 LPO: The global 'nullModule' is initialised by the
 *				first call to 'Init_'.  Only one instance is ever initialised.
 * Authors:		L. P. O'Mard
 * Created:		29 Mar 1993
 * Updated:		02 Jun 1999
 * Copyright:	(c) 1999, University of Essex.
 * 
 ******************/

#include <stdio.h>
#include <stdlib.h>

#define DSAM_DO_NOT_DEFINE_MYAPP	1

#include "DSAM.h"

/******************************************************************************/
/************************** Global Variables **********************************/
/******************************************************************************/

ModuleRefPtr	moduleList = NULL;
ModulePtr		nullModule = NULL;

/******************************************************************************/
/************************** Subroutines and functions *************************/
/******************************************************************************/

/************************** NoFunction ****************************************/

/*
 * This routine is set as the default for all non-assigned functions.
 * It will print a message it an attempt is made to use it.
 */

void *
NoFunction_ModuleMgr(void)
{
	static const char *funcName = "NoFunction_ModuleMgr";
	BOOLN	dummy = FALSE;

	NotifyError("%s: Attempted to use a function which does not\nexist for "\
	  "module.", funcName );
	return((BOOLN *) dummy);

}

/************************** NullFunction **************************************/

/*
 * This routine is set as the NULL default for non-assigned functions.
 * It always returns zero.
 */

void *
NullFunction_ModuleMgr(void)
{
	int		dummy = 0;
	
	return((int *) dummy);

}

/************************** TrueFunction **************************************/

/*
 * This routine is set as the default for non-assigned functions.
 * It always returns TRUE.
 */

void *
TrueFunction_ModuleMgr(void)
{
	BOOLN	dummy = TRUE;
	
	return((BOOLN *) dummy);

}

/************************** SetDefault ****************************************/

/*
 * This routine sets all of the function pointers to the default function.
 */

void
SetDefault_ModuleMgr(ModulePtr module, void *(* DefaultFunc)(void))
{
	module->CheckData = (BOOLN (*)(EarObjectPtr)) DefaultFunc;
	module->CheckPars = (BOOLN (*)(void)) DefaultFunc;
	module->Free = (BOOLN (*)(void)) TrueFunction_ModuleMgr;
	module->GetPotentialResponse = (double (*)(double)) DefaultFunc;
	module->GetRestingResponse = (double (*)(void)) DefaultFunc;
	module->GetUniParListPtr = (UniParListPtr (*)(void)) NullFunction_ModuleMgr;
	module->PrintPars = (BOOLN (*)(void)) DefaultFunc;
#	ifdef _PAMASTER1_H
		module->QueueCommand = (BOOLN (*)(void *, int, TypeSpecifier, char *,
		  CommandSpecifier, ScopeSpecifier)) DefaultFunc;
		module->SendQueuedCommands = (BOOLN (*)(void)) DefaultFunc;
#	endif
	module->ReadPars = NULL;
	module->ReadSignal = (BOOLN (*)(char *, EarObjectPtr)) DefaultFunc;
	module->RunProcess = (BOOLN (*)(EarObjectPtr)) DefaultFunc;
	module->InitModule = (BOOLN (*)(ModulePtr)) DefaultFunc;
	module->SetParsPointer = (BOOLN (*)(ModulePtr)) DefaultFunc;

}

/************************** Init **********************************************/

/*
 * This function initialises and sets a module structure.
 * It allocates the necessary memory for the structure and connects the
 * necessary routines.
 * The stored module name is converted to upper case for (later) comparison
 * purposes.
 * This routine first checks if the global 'nullModule' has been initialised,
 * and initialises it if necessary.
 */

ModulePtr
Init_ModuleMgr(char *theModuleName)
{
	static const char *funcName = "Init_ModuleMgr";
	static ModuleHandle handleNumber = 0;
	ModulePtr	theModule;
	ModRegEntryPtr	modRegEntryPtr;

	if ((modRegEntryPtr = GetRegEntry_ModuleReg(theModuleName)) == NULL) {
		NotifyError("%s: Unknown module '%s'.", funcName, theModuleName);
		return(NULL);
	}		
	if (modRegEntryPtr->specifier == NULL_MODULE) {
		if (nullModule)
			return(nullModule);
	} else {
		if (!nullModule)
			nullModule = Init_ModuleMgr("NULL");
	}
		
	if ((theModule = (ModulePtr) (malloc(sizeof (Module)))) == NULL) {
		NotifyError("%s: Could not allocate memory.", funcName);
		return(NULL);
	}
	theModule->specifier = modRegEntryPtr->specifier;
	theModule->onFlag = TRUE;
	theModule->parsPtr = NULL;
	ToUpper_Utility_String(theModule->name, theModuleName);
	SetDefault_ModuleMgr(theModule, NoFunction_ModuleMgr);
	(* modRegEntryPtr->InitModule)(theModule);
	theModule->handle = handleNumber++;		/* Unique handle for each module. */
	if (!AddModuleRef_ModuleMgr(&moduleList, theModule)) {
		NotifyError("%s: Could not register new module.", funcName);
		Free_ModuleMgr(&theModule);
		return(NULL);
	}
	return(theModule);

}

/**************************** CreateModuleRef *********************************/

/*
 * This function returns a pointer to a module node.
 * It returns NULL if it fails.
 */
 
ModuleRefPtr
CreateModuleRef_ModuleMgr(ModulePtr theModule)
{
	static const char *funcName = "CreateModuleRef_ModuleMgr";
	ModuleRef	*newNode;
	
	if ((newNode = (ModuleRef *) malloc(sizeof (ModuleRef))) == NULL) {
		NotifyError("%s: Out of memory.", funcName);
		return(NULL);
	}
	newNode->module = theModule;
	newNode->next = NULL;
	return(newNode);

}

/**************************** AddModuleRef ************************************/

/*
 * This routine adds a module reference to a list.
 * It places the module handles in ascending order.
 */

BOOLN
AddModuleRef_ModuleMgr(ModuleRefPtr *theList, ModulePtr theModule)
{
	static const char *funcName = "AddModuleRef_ModuleMgr";
	ModuleRefPtr	temp, p;

    if (*theList == NULL) {
    	if ((*theList = CreateModuleRef_ModuleMgr(theModule)) == NULL) {
    		NotifyError("%s: Could not create new node.", funcName);
    		return(FALSE);
    	}
		return(TRUE);
	}
	for (p = *theList; (p->next != NULL) && (p->module->handle <
	  theModule->handle); p = p->next)
		;
	if (p->module->handle == theModule->handle) {
		NotifyWarning("%s: Module already in list.", funcName);
		return(TRUE);
	}
	if ((temp = CreateModuleRef_ModuleMgr(theModule)) == NULL) {
		NotifyError("%s: Could not create temp node.", funcName);
		return(FALSE);
	}
	if (p->module->handle > theModule->handle) {
		temp->module = theModule;
		temp->next = p;
		if (p == *theList)	/* Test for head of list. */
			*theList = temp;
	} else
		p->next = temp;
	return(TRUE);

}

/**************************** FreeModuleRef ***********************************/

/*
 * This routine removes a ModuleRef from a list.
 */

void
FreeModuleRef_ModuleMgr(ModuleRefPtr *theList, ModuleHandle theHandle)
{
	ModuleRefPtr	p, lastRef;

    if (*theList == NULL)
    	return;
	for (p = lastRef = *theList; (p != NULL) && (p->module->handle !=
	  theHandle); p = p->next)
		lastRef = p;
	if (p->module->handle == theHandle) {
		if (p == *theList)	/* Test for head of list. */
			*theList = p->next;
		else
			lastRef->next = p->next;
		free(p);
	}

}

/************************** Free **********************************************/

/*
 * This routine frees the memory, allocated for module structures.
 * It calls the explicit module freeing routine.  The default NullFunction is
 * used if another has not been assigned.
 * It also unregisters the module from the main list.
 * This routine will not delete the global 'nullModule'.
 */

void
Free_ModuleMgr(ModulePtr *theModule)
{
	if ((*theModule == NULL) || (*theModule == nullModule))
		return;
	(* (*theModule)->SetParsPointer)(*theModule);
	FreeModuleRef_ModuleMgr(&moduleList, (*theModule)->handle);
	(* (*theModule)->Free)();
	free(*theModule);
	*theModule = NULL;

}

/**************************** FreeAll *****************************************/

/*
 * This routine frees the memory for all of the initialised modules from
 * the main register list.
 * It does not, however, set the pointer variables to NULL - watch this.
 */

void
FreeAll_ModuleMgr(void)
{
	while (moduleList != NULL)
		FreeModuleRef_ModuleMgr(&moduleList, moduleList->module->handle);

}

/************************** RunModel...Null ***********************************/

/*
 * This function connects the input signal to the output signal, without
 * doing any processing.
 */

BOOLN
RunModel_ModuleMgr_Null(EarObjectPtr data)
{
	static const char *funcName = "RunModel_ModuleMgr_Null";

	if (data == NULL) {
		NotifyError("%s: EarObject not initialised.", funcName);
		return(FALSE);
	}	
	SetProcessName_EarObject(data, NULL_MODULE_PROCESS_NAME);
	if (!CheckPars_SignalData(data->inSignal[0])) {
		NotifyError("%s: Input signal not set correctly.", funcName);
		return(FALSE);
	}
	data->updateCustomersFlag = (data->inSignal[0] != data->outSignal);
	data->outSignal = data->inSignal[0];
 	SetProcessContinuity_EarObject(data);
	return(TRUE);

}

/*************************** LinkGlueRoutine **********************************/

/*
 * This routine has references to modules which are not properly linked
 * otherwise.
 */

void
LinkGlueRoutine_ModuleMgr(void)
{
	optionsPleaseLink++;

}

/*************************** CheckData ****************************************/

/*
 * This function checks the correct initialisation of a data object.
 * It returns FALSE if it fails in any way.
 */

BOOLN
CheckData_ModuleMgr(EarObjectPtr data, const char *callingFunction)
{
	static const char *funcName = "CheckData_ModuleMgr";

	if (data == NULL) {
		NotifyError("%s: %s: Process EarObject not initialised.", funcName,
		  callingFunction);
		return(FALSE);
	}
	if (data->module == NULL) {
		NotifyError("%s: %s: Process EarObject  module not initialised.",
		  funcName, callingFunction);
		return(FALSE);
	}
	return(TRUE);
}


/**************************** Enable ******************************************/

/*
 * This routine sets the onFlag for an EarObject structure.
 * On re-enabling a process the data->outSignal pointer must be set to NULL
 * because it will have been set manually by the 'RunModel_ModuleMgr_NulL'
 * routine. 
 */
 
BOOLN
Enable_ModuleMgr(EarObjectPtr data, BOOLN on)
{
	static const char *funcName = "Enable_ModuleMgr";

	if (!data) {
		NotifyError("%s: EarObject not initialised.", funcName);
		return(FALSE);
	}
	if (!data->module) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	
	if (on) {
		data->module->onFlag = TRUE;
		data->outSignal = NULL;
	} else {
		data->module->onFlag = FALSE;
		FreeOutSignal_EarObject(data);
	}
	return(TRUE);

}

/*************************** GetPotentialResponse *****************************/

/*
 * This function returns the poential response from a module.
 * It only works for routines that have a potential response function.
 */

double
GetPotentialResponse_ModuleMgr(EarObjectPtr data, double potential)
{
	static const char *funcName = "GetPotentialResponse_ModuleMgr";

	if (!CheckData_ModuleMgr(data, funcName))
		return(0.0);
	(* data->module->SetParsPointer)(data->module);
	return((* data->module->GetPotentialResponse)(potential));

}

/*************************** GetRestingResponse *******************************/

/*
 * This function returns the resting response from a module.
 * It only works for routines that have a resting response function.
 */

double
GetRestingResponse_ModuleMgr(EarObjectPtr data)
{
	static const char *funcName = "GetRestingResponse_ModuleMgr";

	if (!CheckData_ModuleMgr(data, funcName))
		return(0.0);
	(* data->module->SetParsPointer)(data->module);
	return((* data->module->GetRestingResponse)());

}

/*************************** GetSimulation ************************************/

/*
 * This function returns the simulation pointer from a module.
 * It only works for routines that can return a simulation.
 */

DatumPtr
GetSimulation_ModuleMgr(EarObjectPtr data)
{
	static const char *funcName = "GetSimulation_ModuleMgr";

	if (!CheckData_ModuleMgr(data, funcName))
		return(NULL);
	(* data->module->SetParsPointer)(data->module);
	return((data->module->specifier != SIMSCRIPT_MODULE)? NULL: ((SimScriptPtr)
	  data->module->parsPtr)->simulation);

}

/*************************** GetProcess ***************************************/

/*
 * This function returns pointer to specified process in a simulation, or just
 * the EarObject's pointer otherwise.
 */

EarObjectPtr
GetProcess_ModuleMgr(EarObjectPtr data, char *processSpecifier)
{
	static const char *funcName = "GetProcess_ModuleMgr";
	EarObjectPtr	process;

	if (!CheckData_ModuleMgr(data, funcName))
		return(NULL);
	if (data->module->specifier != SIMSCRIPT_MODULE)
		return(data);
	(* data->module->SetParsPointer)(data->module);
	if ((process = FindProcess_Utility_Datum(((SimScriptPtr) data->module->
	  parsPtr)->simulation, processSpecifier)) != NULL)
		return(process);

	NotifyError("%s: Could not find process.", funcName);
	return(NULL);

}

/*************************** GetUniParPtr *************************************/

/*
 * This function returns the specified parameter pointer from a module
 */

UniParPtr
GetUniParPtr_ModuleMgr(EarObjectPtr data, char *parName)
{
	static const char *funcName = "GetUniParPtr_ModuleMgr";
	UniParPtr	par;
	UniParListPtr	parList;

	if (!CheckData_ModuleMgr(data, funcName))
		return(NULL);
	(* data->module->SetParsPointer)(data->module);
	parList = (* data->module->GetUniParListPtr)();
	if (!parList)
		NotifyError("%s: arggh!\n", funcName);
	if ((par = FindUniPar_UniParMgr(&parList, parName, UNIPAR_SEARCH_ABBR)) ==
	  NULL) {
		NotifyError("%s: Could not find parameter '%s' for process '%s'",
		  funcName, parName, data->module->name);
		return(NULL);
	}
	return(par);

}

/*************************** GetUniParListPtr *********************************/

/*
 * This function returns the specified parameter list pointer from a module.
 */

UniParListPtr
GetUniParListPtr_ModuleMgr(EarObjectPtr data)
{
	static const char *funcName = "GetUniParListPtr_ModuleMgr";

	if (!CheckData_ModuleMgr(data, funcName))
		return(NULL);
	(* data->module->SetParsPointer)(data->module);
	return((* data->module->GetUniParListPtr)());

}

/*************************** PrintPars ****************************************/

/*
 * This function prints the parameters for a module.
 * It returns FALSE if it fails in any way.
 */

BOOLN
PrintPars_ModuleMgr(EarObjectPtr data)
{
	static const char *funcName = "PrintPars_ModuleMgr";

	if (!CheckData_ModuleMgr(data, funcName))
		return(FALSE);
	(* data->module->SetParsPointer)(data->module);
	return((* data->module->PrintPars)());

}

/*************************** ReadPars *****************************************/

/*
 * This function reads the parameters for a module.
 * It returns FALSE if it fails in any way.
 */

BOOLN
ReadPars_ModuleMgr(EarObjectPtr data, char *fileName)
{
	static const char *funcName = "ReadPars_ModuleMgr";
	BOOLN	ok = TRUE, useOldReadPars = FALSE, firstParameter = TRUE;
	char	*filePath, parName[MAXLINE], parValue[MAX_FILE_PATH];
	FILE	*fp;
	UniParPtr	par;
	UniParListPtr	parList, tempParList;

	if (!CheckData_ModuleMgr(data, funcName))
		return(FALSE);
	(* data->module->SetParsPointer)(data->module);
	if (data->module->specifier == SIMSCRIPT_MODULE)
		return((* data->module->ReadPars)(fileName));
	parList = (* data->module->GetUniParListPtr)();
	if ((strcmp(fileName, NO_FILE) == 0) || !parList)
		return(TRUE);
	filePath = GetParsFileFPath_Common(fileName);
	if ((fp = fopen(filePath, "r")) == NULL) {
		NotifyError("%s: Cannot open data file '%s'.\n", funcName, fileName);
		return(FALSE);
	}
	Init_ParFile();
	SetEmptyLineMessage_ParFile(FALSE);
	while (GetPars_ParFile(fp, "%s %s", parName, parValue)) {
		tempParList = parList;
		if ((par = FindUniPar_UniParMgr(&tempParList, parName,
		  UNIPAR_SEARCH_ABBR)) == NULL) {
		  	if (data->module->ReadPars) {
				useOldReadPars = TRUE;
				break;
			}
			NotifyError("%s: Unknown parameter '%s' for module '%s'.", funcName,
			  parName, data->module->name);
			ok = FALSE;
		} else {
			if (firstParameter) {
				DPrint("%s: Reading '%s' parameters from '%s':\n", funcName,
				  data->module->name, fileName);
				firstParameter = FALSE;
			}
			if (!SetParValue_UniParMgr(&tempParList, par->index, parValue))
				ok = FALSE;
		}
	}
	SetEmptyLineMessage_ParFile(TRUE);
	fclose(fp);
	Free_ParFile();
	if (useOldReadPars) {
		/* Enable this following line after an adjustment period. */
		/* NotifyWarning("%s: Using old parameter format for module '%s'.",
		  funcName, data->module->name); */
		return((* data->module->ReadPars)(fileName));
	}
	if (!ok) {
		NotifyError("%s: Invalid parameters, in '%s' module parameter file "
		  "'%s'.", funcName, data->module->name, fileName);
		return(FALSE);
	}
	return(TRUE);

}

/*************************** PrintSimParFile **********************************/

/*
 * This function prints the parameters for the simulation module.
 * It returns FALSE if it fails in any way.
 */

BOOLN
PrintSimParFile_ModuleMgr(EarObjectPtr data)
{
	static const char *funcName = "PrintSimParFile_ModuleMgr";

	if (!CheckData_ModuleMgr(data, funcName))
		return(FALSE);
	(* data->module->SetParsPointer)(data->module);
	if (data->module->specifier != SIMSCRIPT_MODULE) {
		NotifyError("%s: This function can only be used with a simulation.",
		  funcName);
		return(FALSE);
	}
	return(PrintSimParFile_Utility_SimScript());

}

/*************************** GetSimParFileFlag ********************************/

/*
 * This function returns the SimParFileFlag for a simulation script.
 * It also returns FALSE if it fails, but in this case it prints an error
 * messages.
 * It is not being called in places where it will fail.
 */

BOOLN
GetSimParFileFlag_ModuleMgr(EarObjectPtr data)
{
	static const char *funcName = "GetSimParFileFlag_ModuleMgr";

	if (!CheckData_ModuleMgr(data, funcName))
		return(FALSE);
	if (data->module->specifier != SIMSCRIPT_MODULE) {
		NotifyError("%s: This function can only be used with a simulation.",
		  funcName);
		return(FALSE);
	}
	return(((SimScriptPtr) data->module->parsPtr)->simParFileFlag);

}

/*************************** GetParsFilePath **********************************/

/*
 * This function returns the parsFilePath for a simulation script.
 * It also returns NULL if it fails, but in this case it prints an error
 * messages.
 * It is not being called in places where it will fail.
 */

char *
GetParsFilePath_ModuleMgr(EarObjectPtr data)
{
	static const char *funcName = "GetParsFilePath_ModuleMgr";

	if (!CheckData_ModuleMgr(data, funcName))
		return(NULL);
	if (data->module->specifier != SIMSCRIPT_MODULE) {
		NotifyError("%s: This function can only be used with a simulation.",
		  funcName);
		return(NULL);
	}
	return(((SimScriptPtr) data->module->parsPtr)->parsFilePath);

}

/*************************** RunProcess ***************************************/

/*
 * This function runs the module process
 * It returns FALSE if it fails in any way.
 */

BOOLN
RunProcess_ModuleMgr(EarObjectPtr data)
{
	static const char *funcName = "RunProcess_ModuleMgr";

	if (!CheckData_ModuleMgr(data, funcName))
		return(FALSE);

	if (GetDSAMPtr_Common()->usingGUIFlag && TestDestroy_SimThread())
		return(FALSE);

	if (data->module->onFlag) {
		(* data->module->SetParsPointer)(data->module);
		return((* data->module->RunProcess)(data));
	} else {
		(* nullModule->SetParsPointer)(nullModule);
		return((* nullModule->RunProcess)(data));
	}

}

/*************************** SetPar *******************************************/

/*
 * This function sets the specified value of a universal parameter list for a
 * module.
 * It returns FALSE if it fails in any way.
 */

BOOLN
SetPar_ModuleMgr(EarObjectPtr data, char *parName, char *value)
{
	static const char *funcName = "SetPar_ModuleMgr";
	UniParPtr	par;
	UniParListPtr	parList;

	if (!CheckData_ModuleMgr(data, funcName))
		return(FALSE);
	(* data->module->SetParsPointer)(data->module);
	parList = (* data->module->GetUniParListPtr)();
	switch (data->module->specifier) {
	case SIMSCRIPT_MODULE:
		if (!SetUniParValue_Utility_SimScript(parName, value)) {
			NotifyError("%s: Could not find parameter '%s' for process '%s'",
			  funcName, parName, data->module->name);
			return(FALSE);
		}
		return(TRUE);
	default:
		if ((par = FindUniPar_UniParMgr(&parList, parName,
		  UNIPAR_SEARCH_ABBR)) == NULL) {
			NotifyError("%s: Could not find parameter '%s' for process '%s'",
			  funcName, parName, data->module->name);
			return(FALSE);
		}
		return(SetParValue_UniParMgr(&parList, par->index, value));
	}

}

/*************************** SetRealPar ***************************************/

/*
 * This function calls the converts a real value to a string then calls the
 * 'SetPar_' routine.
 * It returns FALSE if it fails in any way.
 */

BOOLN
SetRealPar_ModuleMgr(EarObjectPtr data, char *name, double value)
{
	/* static const char *funcName = "SetRealPar_ModuleMgr"; */
	char	stringValue[MAXLINE];

	snprintf(stringValue, MAXLINE, "%g", value);
	return(SetPar_ModuleMgr(data, name, stringValue));

}

/*************************** SetRealArrayPar **********************************/

/*
 * This function calls the converts a real value to a string with its respective
 * index value then calls the 'SetPar_' routine.
 * It returns FALSE if it fails in any way.
 */

BOOLN
SetRealArrayPar_ModuleMgr(EarObjectPtr data, char *name, int index,
  double value)
{
	/* static const char *funcName = "SetRealPar_ModuleMgr"; */
	char	arrayElementString[MAXLINE];

	snprintf(arrayElementString, MAXLINE, "%d:%g", index, value);
	return(SetPar_ModuleMgr(data, name, arrayElementString));

}

/************************** SetNull *******************************************/

/*
 * This routine sets the null module.
 */

BOOLN
SetNull_ModuleMgr(ModulePtr theModule)
{
	theModule->specifier = NULL_MODULE;
	SetDefault_ModuleMgr(theModule, TrueFunction_ModuleMgr);
	theModule->RunProcess = RunModel_ModuleMgr_Null;
	return(TRUE);
	
}

/************************** FreeNull ******************************************/

/*
 * This routine frees the null module.
 * It first copies the pointer to temporary pointer so that the conditional
 * statement in the Free_ModuleMgr routine will actually work.
 */

void
FreeNull_ModuleMgr(void)
{
	ModulePtr	tempPtr;
	
	if (!nullModule)
		return;
	tempPtr = nullModule;
	nullModule = NULL;
	Free_ModuleMgr(&tempPtr);
	
}

