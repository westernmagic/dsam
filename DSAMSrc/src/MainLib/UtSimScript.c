/**********************
 *
 * File:		UtSimScript.c
 * Purpose:		This is the source file for the simulation script reading
 *				module.
 * Comments:	Written using ModuleProducer version 1.9 (May 27 1996).
 *				21-08-97 LPO: Removed rampInterval parameter - use new ramp
 *				module.
 *				26-11-97 LPO: Using new GeNSpecLists module for 'operationMode'
 *				parameter.
 *				01-05-98 LPO: The GetInSignalRef_EarObject routine is now used
 *				to ensure that the input signals are correctly transferred to
 *				the first process, and that the numInputs count is updated
 *				At the moment the input signals to a simulation script
 *				are always re-connected each time the process is run.  This is
 *				because to add the connection to the supplier/customer lists
 *				the process would need the previous process' EarObjectPtr.
 *				24-09-1998 LPO: Introduced the parameter file mode.  If it is
 *				'on' then parameters for the simulation will be read from a
 *				file with the same name as the simulation script, but
 *				with the ".spf" extension.  A simulation parameter file (SPF)
 *				file will be created if one does not already exist.
 *				30-09-98 LPO: I have introduced the uniParLockFlag field which
 *				is set to disallow command line changes of the universal
 *				parameters when a uniParList file is being used.
 *				10-12-98 LPO: Introduced handling of NULL parLists, i.e. for 
 *				modules with no parameters.
 *				02-02-99 LPO: The 'SetUniParValue_' has been moved to the
 *				UtDatum module.
 *				25-05-99 LPO: Introduced the 'parsFilePath' field.  All modules
 *				now read their parameter files using a path relative to the
 *				path of the simulation script file.
 *				03-06-99 LPO: Introduced 'SetUniParValue_' routine, so that it
 *				can be called from 'GeModuleMgr' module.
 *				23-08-99 LPO: The 'NotifyError_' routine was not calling the
 *				CheckInitErrorsFile_Common" routine so the
 *				'GetDSAMPtr_Common()->errorsFile' field was not set and was
 *				causing a crash.
 * Author:		L. P. O'mard
 * Created:		19 Jan 1996
 * Updated:		23 Aug 1999
 * Copyright:	(c) 1999, University of Essex
 *
 *********************/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG_H */

#include "DSAM.h"
#include "UtSSParser.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

SimScriptPtr	simScriptPtr = NULL;

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
Free_Utility_SimScript(void)
{
	/* static const char	*funcName = "Free_Utility_SimScript";  */

	if (simScriptPtr == NULL)
		return(FALSE);
	FreeSimulation_Utility_SimScript();
	if (simScriptPtr->parFilePathModeList)
		free(simScriptPtr->parFilePathModeList);
	if (simScriptPtr->parList)
		FreeList_UniParMgr(&simScriptPtr->parList);
	if (simScriptPtr->parSpec == GLOBAL) {
		free(simScriptPtr);
		simScriptPtr = NULL;
	}
	return(TRUE);

}

/****************************** ParFilePathModePrototypeList ******************/

/*
 * This function returns a pointer to a prototype name list.
 */

NameSpecifierPtr
ParFilePathModePrototypeList_Utility_SimScript(void)
{
	static NameSpecifier	modeList[] = {

			{ "NONE",		UTILITY_SIMSCRIPT_PARFILEPATHMODE_NONE },
			{ "RELATIVE",	UTILITY_SIMSCRIPT_PARFILEPATHMODE_RELATIVE },
			{ NO_FILE,		UTILITY_SIMSCRIPT_PARFILEPATHMODE_PATH },
			{ "",			UTILITY_SIMSCRIPT_PARFILEPATHMODE_NULL },
		};
	return(modeList);

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
Init_Utility_SimScript(ParameterSpecifier parSpec)
{
	static const char	*funcName = "Init_Utility_SimScript";

	if (parSpec == GLOBAL) {
		if (simScriptPtr != NULL)
			Free_Utility_SimScript();
		if ((simScriptPtr = (SimScriptPtr) malloc(sizeof(SimScript))) == NULL) {
			NotifyError("%s: Out of memory for 'global' pointer", funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (simScriptPtr == NULL) {
			NotifyError("%s:  'local' pointer not set.", funcName);
			return(FALSE);
		}
	}
	simScriptPtr->parSpec = parSpec;
	simScriptPtr->operationModeFlag = FALSE;
	simScriptPtr->parFilePathModeFlag = FALSE;
	simScriptPtr->simulation = NULL;
	simScriptPtr->operationMode = 0;
	simScriptPtr->parFilePathMode = 0;

	if ((simScriptPtr->parFilePathModeList = InitNameList_NSpecLists(
	  ParFilePathModePrototypeList_Utility_SimScript(),
	  simScriptPtr->parsFilePath)) == NULL)
		return(FALSE);
	if (!SetUniParList_Utility_SimScript()) {
		NotifyError("%s: Could not initialise parameter list.", funcName);
		Free_Utility_SimScript();
		return(FALSE);
	}
	sprintf(simScriptPtr->parsFilePath, "No path");
	simScriptPtr->lineNumber = 0;
	simScriptPtr->symList = NULL;
	simScriptPtr->simPtr = NULL;
	simScriptPtr->subSimList = NULL;
	return(TRUE);

}

/****************************** SetUniParList *********************************/

/*
 * This function initialises and sets the module's universal parameter
 * list. This list provides universal access to the module's
 * parameters.  It expects to be called from the 'Init_' routine.
 */

BOOLN
SetUniParList_Utility_SimScript(void)
{
	static const char *funcName = "SetUniParList_Utility_SimScript";
	UniParPtr	pars;

	if ((simScriptPtr->parList = InitList_UniParMgr(UNIPAR_SET_SIMSPEC,
	  UTILITY_SIMSCRIPT_NUM_PARS, NULL)) == NULL) {
		NotifyError("%s: Could not initialise parList.", funcName);
		return(FALSE);
	}
	pars = simScriptPtr->parList->pars;
	SetPar_UniParMgr(&pars[UTILITY_SIMSCRIPT_OPERATIONMODE], "DIAG_MODE",
	  "Diagnostics operation mode ('ON' or 'OFF').",
	  UNIPAR_BOOL,
	  &simScriptPtr->operationMode, NULL,
	  (void * (*)) SetOperationMode_Utility_SimScript);
	SetPar_UniParMgr(&pars[UTILITY_SIMSCRIPT_PARFILEPATHMODE],
	  "PAR_FILE_PATH_MODE",
	  "Parameter file path mode ('relative', 'none' or <file path>).",
	  UNIPAR_NAME_SPEC_WITH_FPATH,
	  &simScriptPtr->parFilePathMode, simScriptPtr->parFilePathModeList,
	  (void * (*)) SetParFilePathMode_Utility_SimScript);
	SetPar_UniParMgr(&pars[UTILITY_SIMSCRIPT_SIMULATION], "SIMULATION",
	  "Simulation script.",
	  UNIPAR_SIMSCRIPT,
	  &simScriptPtr->simulation, simScriptPtr->simFileName,
	  (void * (*)) SetSimulation_Utility_SimScript);
	return(TRUE);

}

/****************************** GetUniParListPtr ******************************/

/*
 * This function returns a pointer to the module's universal parameter
 * list.
 */

UniParListPtr
GetUniParListPtr_Utility_SimScript(void)
{
	static const char	*funcName = "GetUniParListPtr_Utility_SimScript";

	if (simScriptPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (simScriptPtr->parList == NULL) {
		NotifyError("%s: UniParList data structure has not been initialised. "
		  "NULL returned.", funcName);
		return(NULL);
	}
	return(simScriptPtr->parList);

}

/****************************** SetPars ***************************************/

/*
 * This function sets all the module's parameters.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetPars_Utility_SimScript(DatumPtr simulation, char * operationMode,
  char * parFilePathMode)
{
	static const char	*funcName = "SetPars_Utility_SimScript";
	BOOLN	ok;

	ok = TRUE;
	if (!SetOperationMode_Utility_SimScript(operationMode))
		ok = FALSE;
	if (!SetParFilePathMode_Utility_SimScript(parFilePathMode))
		ok = FALSE;
	if (!SetSimulation_Utility_SimScript(simulation))
		ok = FALSE;
	if (!ok)
		NotifyError("%s: Failed to set all module parameters." ,funcName);
	return(ok);

}

/****************************** SetSimulation *********************************/

/*
 * This function sets the module's simulation parameter.
 * Additional checks should be added as required.
 */

BOOLN
SetSimulation_Utility_SimScript(DatumPtr theSimulation)
{
	static const char	*funcName = "SetSimulation_Utility_SimScript";
	BOOLN	ok = TRUE;

	if (simScriptPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (!simScriptPtr->parFilePathModeFlag) {
		NotifyError("%s: parFilePathMode must be set first.", funcName);
		return(FALSE);
	}
	if (!InitSimulation_Utility_SimScript(theSimulation)) {
		NotifyError("%s: Simulation not correctly Initialised.", funcName);
		return(FALSE);
	}
	simScriptPtr->simulation = theSimulation;
	return(ok);

}

/****************************** SetOperationMode ******************************/

/*
 * This function sets the module's operationMode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetOperationMode_Utility_SimScript(char *theOperationMode)
{
	static const char	*funcName = "SetOperationMode_Utility_SimScript";
	int		specifier;

	if (simScriptPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theOperationMode,
	  BooleanList_NSpecLists(0))) == GENERAL_BOOLEAN_NULL) {
		NotifyError("%s: Illegal operationMode name (%s).", funcName,
		  theOperationMode);
		return(FALSE);
	}
	simScriptPtr->operationModeFlag = TRUE;
	simScriptPtr->operationMode = specifier;
	return(TRUE);

}

/****************************** SetParFilePathMode ****************************/

/*
 * This function sets the module's parFilePathMode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetParFilePathMode_Utility_SimScript(char *theParFilePathMode)
{
	static const char	*funcName = "SetParFilePathMode_Utility_SimScript";

	if (simScriptPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	simScriptPtr->parFilePathMode = Identify_NameSpecifier(theParFilePathMode,
	  simScriptPtr->parFilePathModeList);
	switch (simScriptPtr->parFilePathMode) {
	case UTILITY_SIMSCRIPT_PARFILEPATHMODE_PATH:
	case UTILITY_SIMSCRIPT_PARFILEPATHMODE_NULL:
		simScriptPtr->parFilePathMode = UTILITY_SIMSCRIPT_PARFILEPATHMODE_PATH;
		strcpy(simScriptPtr->parsFilePath, theParFilePathMode);
		break;
	default:
		;
	}
	simScriptPtr->parFilePathModeFlag = TRUE;
	return(TRUE);

}

/****************************** SetUniParValue ********************************/

/*
 * This function universal parameter value for the simulation.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetUniParValue_Utility_SimScript(char *parName, char *parValue)
{
	static const char	*funcName = "SetUniParValue_Utility_SimScript";

	if (simScriptPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (!SetUniParValue_Utility_Datum(simScriptPtr->simulation, parName,
	  parValue)) {
		NotifyError("%s: Could not set '%s' value to '%s'.", funcName, parName,
		  parValue);
		return(FALSE);
	}
	return(TRUE);

}

/****************************** PrintSimParFile *******************************/

/*
 * This routine prints the simulation script and the universal parameters.
 */

BOOLN
PrintSimParFile_Utility_SimScript(void)
{
	static char *funcName = "PrintSimParFile_Utility_SimScript(";
	SimScriptPtr	localSimScriptPtr = simScriptPtr;

	if (simScriptPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	DPrint("\t##--------------- Simulation script ----------------------##\n");
	DPrint("\t%s\tDiagnostics mode ('off' or 'on').\n", BooleanList_NSpecLists(
	  simScriptPtr->operationMode)->name);
	DPrint("\n");
	PrintInstructions_Utility_Datum(localSimScriptPtr->simulation, "",
	  DATUM_INITIAL_INDENT_LEVEL, "\t");
	DPrint("\n");
	PrintParListModules_Utility_Datum(localSimScriptPtr->simulation, "");
	DPrint("\n%s Simulation parameter file divider.\n\n",
	  SIMSCRIPT_SIMPARFILE_DIVIDER);
	simScriptPtr = localSimScriptPtr;
	return(TRUE);

}

/************************ ReadSimParFile **************************************/

/*
 * Read a list of universal parameters from a simulation parameter file.
 */

BOOLN
ReadSimParFile_Utility_SimScript(char *filePath)
{
	static const char	*funcName = "ReadSimParFile_Utility_SimScript";
	BOOLN	ok = TRUE, foundDivider = FALSE;
	char	parName[MAXLINE], parValue[MAXLINE], operationMode[MAXLINE];
	FILE	*fp;
	DatumPtr	simulation;
	SimScriptPtr	localSimScriptPtr = simScriptPtr;
	
	if (localSimScriptPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if ((fp = fopen(filePath, "r")) == NULL) {
		NotifyError("%s: Could not open '%s' simulation parameter file.",
		  funcName, filePath);
		return(FALSE);
	}
	FindFilePathAndName_Common(filePath, localSimScriptPtr->parsFilePath,
	  localSimScriptPtr->simParFileName);
	FreeSimulation_Utility_SimScript();
	Init_ParFile();
	if (!GetPars_ParFile(fp, "%s", operationMode))
		ok = FALSE;
	if (ok && ((simulation = Read_Utility_SimScript(fp)) == NULL)) {
		NotifyError("%s: Could not read simulation script.", funcName);
		ok = FALSE;
	}
	if (ok && !SetPars_Utility_SimScript(simulation, operationMode, "none")) {
		NotifyError("%s: Could not set simulation script.", funcName);
		ok = FALSE;
	}
	SetEmptyLineMessage_ParFile(FALSE);
	while (ok && !foundDivider && GetPars_ParFile(fp, "%s %s", parName,
	  parValue))
		if (strcmp(parName, SIMSCRIPT_SIMPARFILE_DIVIDER) == 0)
			foundDivider = TRUE;
		else if (!SetUniParValue_Utility_Datum(localSimScriptPtr->simulation,
		  parName, parValue)) {
			NotifyError("%s: Could not set '%s' parameter.", funcName, parName);
			ok = FALSE;
		}
	SetEmptyLineMessage_ParFile(TRUE);
	fclose(fp);
	Free_ParFile();
	simScriptPtr = localSimScriptPtr;
	if (!ok) {
		NotifyError("%s: Invalid parameters, in simulation parameter file "
		  "'%s'.", funcName, filePath);
		return(FALSE);
	}
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
CheckPars_Utility_SimScript(void)
{
	static const char	*funcName = "CheckPars_Utility_SimScript";
	BOOLN	ok;

	ok = TRUE;
	if (simScriptPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (simScriptPtr->simulation == NULL) {
		NotifyError("%s: simulation not set.", funcName);
		ok = FALSE;
	}
	if (!simScriptPtr->operationModeFlag) {
		NotifyError("%s: operationMode variable not set.", funcName);
		ok = FALSE;
	}
	if (!simScriptPtr->parFilePathModeFlag) {
		NotifyError("%s: parFilePathMode variable not set.", funcName);
		ok = FALSE;
	}
	return(ok);

}

/****************************** PrintPars *************************************/

/*
 * This routine prints all the module's parameters to the file stream.
 * specified by the GetDSAMPtr_Common()->parsFile file pointer.
 */

BOOLN
PrintPars_Utility_SimScript(void)
{
	static const char	*funcName = "PrintPars_Utility_SimScript";

	if (!CheckPars_Utility_SimScript()) {
		NotifyError("%s: Parameters have not been correctly set.", funcName);
		return(FALSE);
	}
	DPrint("Simulation Script Utility Module "
	  "Parameters:-\n");
	DPrint("\tSimulation instruction list:-\n");
	PrintInstructions_Utility_Datum(simScriptPtr->simulation, "",
	  DATUM_INITIAL_INDENT_LEVEL, "\t");
	DPrint("\tDiagnostics operation mode = %s,\n",
	  BooleanList_NSpecLists(simScriptPtr->operationMode)->name);
	DPrint("\tParameter file path mode: %s \n",
	  simScriptPtr->parFilePathModeList[simScriptPtr->parFilePathMode].name);
	return(TRUE);

}

/****************************** ReadPars **************************************/

/*
 * This program reads a specified number of parameters from a file.
 * This routine is special in the way that it treats the file path.  For this
 * module, all file paths for modules needs to be stored.
 * It returns FALSE if it fails in any way.
 */

BOOLN
ReadPars_Utility_SimScript(char *fileName)
{
	static const char	*funcName = "ReadPars_Utility_SimScript";
	BOOLN	ok;
	char	*filePath;
	char	operationMode[MAXLINE], parFilePathMode[MAX_FILE_PATH];
	DatumPtr	simulation;
	FILE	*fp;

	filePath = GetParsFileFPath_Common(fileName);
	if ((fp = fopen(filePath, "r")) == NULL) {
		NotifyError("%s: Cannot open data file '%s'.\n", funcName, filePath);
		return(FALSE);
	}
	DPrint("%s: Reading from '%s':\n", funcName, filePath);
	Init_ParFile();
	ok = TRUE;
	if (!GetPars_ParFile(fp, "%s", operationMode))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%s", parFilePathMode))
		ok = FALSE;
	if ((simulation = Read_Utility_SimScript(fp)) == NULL)
		ok = FALSE;
	fclose(fp);
	Free_ParFile();
	if (!ok) {
		NotifyError("%s: Not enough lines, or invalid parameters, in module "
		  "parameter file '%s'.", funcName, filePath);
		return(FALSE);
	}
	FindFilePathAndName_Common(filePath, simScriptPtr->parsFilePath,
	  simScriptPtr->simFileName);
	if (!SetPars_Utility_SimScript(simulation, operationMode,
	  parFilePathMode)) {
		NotifyError("%s: Could not set parameters.", funcName);
		return(FALSE);
	}
	return(TRUE);

}

/****************************** GetSimulation *********************************/

/*
 * This routine returns a pointer to the module's simulation instruction list.
 * It returns NULL if it fails in any way.
 */
 
DatumPtr
GetSimulation_Utility_SimScript(void)
{
	static const char	*funcName = "GetSimulation_Utility_SimScript";

	if (simScriptPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	return(simScriptPtr->simulation);

}

/****************************** FreeSimulation ********************************/

/*
 * This routine frees a simulation, its symbols and resets the parameters
 * associated with it.
 * It returns FALSE if it fails in any way.
 */

BOOLN
FreeSimulation_Utility_SimScript(void)
{
	static char *funcName = "FreeSimulation_Utility_SimScript(";
	SimScriptPtr		localSimScriptPtr = simScriptPtr;

	if (simScriptPtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	if (!localSimScriptPtr->simulation)
		return(TRUE);
	FreeInstructions_Utility_Datum(&localSimScriptPtr->simulation);
	FreeSymbols_Utility_SSSymbols(&localSimScriptPtr->symList);
	localSimScriptPtr->lineNumber = 0;
	simScriptPtr = localSimScriptPtr;
	return(TRUE);

}

/****************************** Read ******************************************/

/*
 * This routine reads the simulation script from a file.
 * It returns a point to a list of instructions, which must be free'd by the
 * calling program.
 */

DatumPtr
Read_Utility_SimScript(FILE *fp)
{
	static const char	*funcName = "Read_Utility_SimScript";
	int	yyparse(void);
	SimScriptPtr	localSimScriptPtr = simScriptPtr;

	if (localSimScriptPtr == NULL) {
		NotifyError("%s: Module not initialised, the 'Init_...' function must\n"
		  "be called.", funcName);
		return(NULL);
	}
	FreeSimulation_Utility_SimScript();
	localSimScriptPtr->fp = fp;
	InitKeyWords_Utility_SSSymbols(&simScriptPtr->symList);
	localSimScriptPtr->simPtr = &localSimScriptPtr->simulation;
	if (yyparse() != 0)
		FreeSimulation_Utility_SimScript();
	/*  Is this next bit  done twice? */
	if (!ResolveInstLabels_Utility_Datum(
	  localSimScriptPtr->simulation))
		FreeSimulation_Utility_SimScript();
	simScriptPtr = localSimScriptPtr;
	return(simScriptPtr->simulation);

}

/****************************** NotifyError ***********************************/

/*
 * This is the NotifyError routine for this module.  It also returns the
 * line number at which the error occured.
 */

void
NotifyError_Utility_SimScript(char *format, ...)
{
	va_list	args;
	
	CheckInitErrorsFile_Common();
	if (GetDSAMPtr_Common()->errorsFile == stderr)
		fprintf(stderr, "\07");
	va_start(args, format);
	vfprintf(GetDSAMPtr_Common()->errorsFile, format, args);
	va_end(args);
	fprintf(GetDSAMPtr_Common()->errorsFile, " [line %d]\n", simScriptPtr->
	  lineNumber);

} /* NotifyError */

/****************************** InitialiseEarObjects **************************/

/*
 * This routine initialises the EarObjects in a simulation.
 * It is called by the parser routine.
 * A return value of -1 means that parsing should continue.
 */

BOOLN
InitialiseEarObjects_Utility_SimScript(void)
{
	static char *funcName = "InitialiseEarObjects_Utility_SimScript";
	BOOLN	ok = TRUE;
	SimScriptPtr	localSimScriptPtr = simScriptPtr;

	if (!InitialiseEarObjects_Utility_Datum(*simScriptPtr->simPtr)) {
		NotifyError("%s: Could not initialise EarObjects.", funcName);
		ok = FALSE;
	}
	simScriptPtr = localSimScriptPtr;
	return(ok);

}
	
/****************************** GetSimScriptSimulation ************************/

/*
 * This function returns the pointer to a simulation of a process node.
 * It checks that the process is the SIMSCRIPT_MODULE process.
 * It is used as a wrapper.
 * It exits if there is an error, as this would be a programming error anyway.
 */

DatumPtr *
GetSimScriptSimulation_Utility_SimScript(DatumPtr pc)
{
	static const char	*funcName = "GetSimScriptSimulation_Utility_SimScript";

	if ((pc->type != PROCESS) || (pc->data->module->specifier !=
	  SIMSCRIPT_MODULE)) {
		NotifyError("%s: Not a 'SIMSCRIPT_MODULE' process.", funcName);
		exit(1);
	}
	return (&((SimScriptPtr) pc->data->module->parsPtr)->simulation);

}

/****************************** SetParsPointer ********************************/

/*
 * This function sets the global parameter pointer for the module to
 * that associated with the module instance.
 */

BOOLN
SetParsPointer_Utility_SimScript(ModulePtr theModule)
{
	static const char	*funcName = "SetParsPointer_Utility_SimScript";

	if (!theModule) {
		NotifyError("%s: The module is not set.", funcName);
		return(FALSE);
	}
	simScriptPtr = (SimScriptPtr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 */

BOOLN
InitModule_Utility_SimScript(ModulePtr theModule)
{
	static const char	*funcName = "InitModule_Utility_SimScript";

	if (!SetParsPointer_Utility_SimScript(theModule)) {
		NotifyError("%s: Cannot set parameters pointer.", funcName);
		return(FALSE);
	}
	if (!Init_Utility_SimScript(GLOBAL)) {
		NotifyError("%s: Could not initialise process structure.", funcName);
		return(FALSE);
	}
	theModule->parsPtr = simScriptPtr;
	theModule->CheckPars = CheckPars_Utility_SimScript;
	theModule->Free = Free_Utility_SimScript;
	theModule->GetUniParListPtr = GetUniParListPtr_Utility_SimScript;
	theModule->PrintPars = PrintPars_Utility_SimScript;
	theModule->ReadPars = ReadPars_Utility_SimScript;
	theModule->RunProcess = Process_Utility_SimScript;
	theModule->SetParsPointer = SetParsPointer_Utility_SimScript;
	return(TRUE);

}

/****************************** CheckData *************************************/

/*
 * This routine checks that the 'data' EarObject and input signal are
 * correctly initialised.
 * It should also include checks that ensure that the module's
 * parameters are compatible with the signal parameters, i.e. dt is
 * not too small, etc...
 * If an input signal is available, then it is set as the input signal, other-
 * wise it will not be used.
 */

BOOLN
CheckData_Utility_SimScript(EarObjectPtr data)
{
	static const char	*funcName = "CheckData_Utility_SimScript";

	if (data == NULL) {
		NotifyError("%s: EarObject not initialised.", funcName);
		return(FALSE);
	}
	/*** Put additional checks here. ***/
	return(TRUE);

}

/**************************** InitSimulation **********************************/

/*
 * This routine initialises the simulation.
 * It sets the parameters for any sub-simulation. These may be reset for the
 * sub-simulation script if it is read from a file.
 */

BOOLN
InitSimulation_Utility_SimScript(DatumPtr simulation)
{
	static const char *funcName = "InitSimulation_Utility_SimScript";
	char	*oldParsFilePath = GetDSAMPtr_Common()->parsFilePath;
	int		lenBMModPrefix = strlen(BM_MODULE_PREFIX);
	DatumPtr	pc;
	CFListPtr	theCFs = NULL;
	SimScriptPtr	localSimScriptPtr = simScriptPtr;

	if (localSimScriptPtr->parFilePathMode ==
	  UTILITY_SIMSCRIPT_PARFILEPATHMODE_NULL) {
		strcpy(localSimScriptPtr->parsFilePath,
		  localSimScriptPtr->parFilePathModeList[
		  UTILITY_SIMSCRIPT_PARFILEPATHMODE_NULL].name);
	}
	for (pc = localSimScriptPtr->simulation; pc != NULL; pc = pc->next)
		if (pc->type == PROCESS) {
			pc->data->module->onFlag = pc->onFlag;
			switch (pc->data->module->specifier) {
			case SIMSCRIPT_MODULE:
				(* pc->data->module->SetParsPointer)(pc->data->module);
				SetOperationMode_Utility_SimScript(BooleanList_NSpecLists(
				  localSimScriptPtr->operationMode)->name);
				SetParFilePathMode_Utility_SimScript(localSimScriptPtr->
				  parFilePathModeList[localSimScriptPtr->parFilePathMode].name);
				SetSimulation_Utility_SimScript(
				  *GetSimScriptSimulation_Utility_SimScript(pc));
				break;
			case ANA_SAI_MODULE:
				SetPar_ModuleMgr(pc->data, "STROBE_PAR_FILE", NO_FILE);
				break;
			default:
				if (strncmp(pc->u.proc.moduleName, BM_MODULE_PREFIX,
				  lenBMModPrefix) == 0) {
					if ((theCFs = GenerateDefault_CFList()) == NULL) {
						NotifyError("%s: Could not generate default CF list.",
						  funcName);
						return(FALSE);
					}
					if (!SetBandwidths_CFList(theCFs, "erb", NULL)) {
						NotifyError("%s: Could not set CF bandwidths.",
						  funcName);
						Free_CFList(&theCFs);
						return(FALSE);
					}
					SetPar_ModuleMgr(pc->data, "CFList", (char *) theCFs);
				}
			}
		}
	if (localSimScriptPtr->parFilePathMode !=
	  UTILITY_SIMSCRIPT_PARFILEPATHMODE_NONE) {
		SetParsFilePath_Common(localSimScriptPtr->parsFilePath);
		if (!InitialiseModules_Utility_Datum(simulation)) {
			NotifyError("%s: Could not initialise modules.", funcName);
			return(FALSE);
		}
		SetParsFilePath_Common(oldParsFilePath);
	}
	simScriptPtr = localSimScriptPtr;
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
 * The 'localSimScriptPtr' is required because with nested usage, the global
 * simScriptPtr will be changed for other instances of this module (cf LUT:558).
 * The first process must be manually disconnected and connected to the
 * 'data' EarObject's signals.
 */

BOOLN
Process_Utility_SimScript(EarObjectPtr data)
{
	static const char	*funcName = "Process_Utility_SimScript";
	char	*oldParsFilePath = GetDSAMPtr_Common()->parsFilePath;
	int		i;
	SignalDataPtr	lastOutSignal;
	EarObjectPtr	firstProcess;
	SimScriptPtr	localSimScriptPtr = simScriptPtr;

	if (!CheckPars_Utility_SimScript())
		return(FALSE);
	if (!CheckData_Utility_SimScript(data)) {
		NotifyError("%s: Process data invalid.", funcName);
		return(FALSE);
	}
	SetProcessName_EarObject(data, "Simulation script");
	firstProcess = GetFirstProcess_Utility_Datum(localSimScriptPtr->simulation);
	for (i = 0; i < firstProcess->numInSignals; i++)
		UnSetInSignal_EarObject(firstProcess, i);
	for (i = 0; firstProcess->numInSignals < data->numInSignals; i++) {
		if (data->inSignalFlag[i])
			firstProcess->inSignal[GetInSignalRef_EarObject(firstProcess)] =
			  data->inSignal[i];
	}
	if (!CheckParLists_Utility_Datum(localSimScriptPtr->simulation)) {
		NotifyError("%s: Universal parameter lists check failed.", funcName);
		return(FALSE);
	}
	if (data->updateProcessFlag)
		ResetSimulation_Utility_Datum(localSimScriptPtr->simulation);
	if (localSimScriptPtr->operationMode == GENERAL_BOOLEAN_ON)
		PrintParsModules_Utility_Datum(localSimScriptPtr->simulation);
	SetParsFilePath_Common(localSimScriptPtr->parsFilePath);
	if (!Execute_Utility_Datum(localSimScriptPtr->simulation)) {
		NotifyError("%s: Could not execute simulation modules.", funcName);
		return(FALSE);
	}
	SetParsFilePath_Common(oldParsFilePath);
	lastOutSignal = data->outSignal;
	data->outSignal = GetLastProcess_Utility_Datum(
 	  localSimScriptPtr->simulation)->outSignal;
	simScriptPtr = localSimScriptPtr;
	data->updateCustomersFlag = (lastOutSignal != data->outSignal);
	SetProcessContinuity_EarObject(data);
	return(TRUE);

}
