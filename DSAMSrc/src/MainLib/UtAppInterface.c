/**********************
 *
 * File:		UtAppInterface.c
 * Purpose:		This Module contains the interface routines for DSAM
 *				application's.
 * Comments:	
 * Author:		L. P. O'Mard
 * Created:		15 Mar 2000
 * Updated:		
 * Copyright:	(c) 1999, University of Essex.
 *
 *********************/

#include <stdlib.h> 
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <sys/stat.h>

#include "DSAM.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

AppInterfacePtr	appInterfacePtr = NULL;

/******************************************************************************/
/****************************** Functions and subroutines *********************/
/******************************************************************************/

/****************************** Free ******************************************/

/*
 * This function releases of the memory allocated for the process
 * variables. It should be called when the module is no longer in use.
 * It is defined as returning a BOOLN value because the generic
 * module interface requires that a non-void value be returned.
 * The "FreeAll_EarObject" call is used here when the main
 * This routine will return without doing anything if the 'canFreePtrFlag' is
 * not set.
 * This is to allow it to be placed in the 'MainSimulation' routine of programs
 * but it will not do anything when the GUI is calling 'MainSimulation' routine.
 * This is because the GUI interface calls it when the GUI exits.
 * This routine also calls the routine to free the nullModule global variable.
 */

BOOLN
Free_AppInterface(void)
{
	if (!appInterfacePtr)
		return(FALSE);
	if (!appInterfacePtr->canFreePtrFlag)
		return(TRUE);
	if (appInterfacePtr->diagModeList)
		free(appInterfacePtr->diagModeList);
	if (appInterfacePtr->appParList)
		FreeList_UniParMgr(&appInterfacePtr->appParList);
	if (appInterfacePtr->parList)
		FreeList_UniParMgr(&appInterfacePtr->parList);
	if (appInterfacePtr->audModel)
		Free_EarObject(&appInterfacePtr->audModel);
	if (appInterfacePtr->parSpec == GLOBAL) {
		free(appInterfacePtr);
		appInterfacePtr = NULL;
	}
	FreeNull_ModuleMgr();
	return(TRUE);

}

/****************************** InitListingModeList ***************************/

/*
 * This function initialises the 'listingMode' list array
 */

BOOLN
InitListingModeList_AppInterface(void)
{
	static NameSpecifier	modeList[] = {

			{ "PARAMETERS", APP_INT_PARAMETERS_LIST_MODE },
			{ "CFINFO", 	APP_INT_CFLIST_LIST_MODE },
			{ "",			APP_INT_LIST_NULL }
		};
	appInterfacePtr->listingModeList = modeList;
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
Init_AppInterface(ParameterSpecifier parSpec)
{
	static const char	*funcName = "Init_AppInterface";
	int		i;

	if (parSpec == GLOBAL) {
		if (appInterfacePtr != NULL)
			Free_Analysis_ACF();
		if ((appInterfacePtr = (AppInterfacePtr) malloc(sizeof(
		  AppInterface))) == NULL) {
			NotifyError("%s: Out of memory for 'global' pointer", funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (!appInterfacePtr) {
			NotifyError("%s:  'local' pointer not set.", funcName);
			return(FALSE);
		}
	}
	appInterfacePtr->parSpec = parSpec;
	appInterfacePtr->updateProcessVariablesFlag = TRUE;
	appInterfacePtr->simulationFileFlag = FALSE;
	appInterfacePtr->useParComsFlag = TRUE;
	appInterfacePtr->checkMainInit = TRUE;
	appInterfacePtr->listParsAndExit = FALSE;
	appInterfacePtr->listCFListAndExit = FALSE;
	appInterfacePtr->appParFileFlag = FALSE;
	appInterfacePtr->readAppParFileFlag = FALSE;
	appInterfacePtr->printUsageFlag = FALSE;
	strcpy(appInterfacePtr->appName, "No Name");
	strcpy(appInterfacePtr->appParFile, NO_FILE);
	strcpy(appInterfacePtr->appVersion, "?.?.?");
	strcpy(appInterfacePtr->compiledDSAMVersion, "?.?.?");
	strcpy(appInterfacePtr->title, "No title");
	for (i = 0; i < APP_MAX_AUTHORS; i++)
		appInterfacePtr->authors[i][0] = '\0';
	strcpy(appInterfacePtr->simulationFile, NO_FILE);
	strcpy(appInterfacePtr->diagMode, NO_FILE);
	strcpy(appInterfacePtr->installDir, ".");
	appInterfacePtr->argv = NULL;
	appInterfacePtr->argc = 0;
	appInterfacePtr->initialCommand = 0;
	appInterfacePtr->segmentModeSpecifier = GENERAL_BOOLEAN_OFF;
	appInterfacePtr->diagModeSpecifier = GENERAL_DIAGNOSTIC_OFF_MODE;
	appInterfacePtr->maxUserModules = -1;
	appInterfacePtr->audModel = NULL;

	InitListingModeList_AppInterface();
	if ((appInterfacePtr->diagModeList = InitNameList_NSpecLists(
	  DiagModeList_NSpecLists(0), appInterfacePtr->diagMode)) == NULL)
		return(FALSE);
	if (!SetUniParList_AppInterface()) {
		NotifyError("%s: Could not initialise parameter list.", funcName);
		Free_AppInterface();
		return(FALSE);
	}
	appInterfacePtr->appParList = NULL;
	strcpy(appInterfacePtr->simulationFile, NO_FILE);
	appInterfacePtr->simLastModified = 0;
	appInterfacePtr->Init = NULL;
	appInterfacePtr->PrintUsage = NULL;
	appInterfacePtr->PrintSimMgrUsage = NULL;
	appInterfacePtr->ProcessOptions = NULL;
	appInterfacePtr->RegisterUserModules = NULL;
	appInterfacePtr->ReadInitialPars = NULL;
	appInterfacePtr->SetFinalPars = NULL;
	return(TRUE);

}

/****************************** SetUniParList *********************************/

/*
 * This function initialises and sets the programs universal parameter
 * list. This list provides universal access to the program's
 * parameters.  It needs to be called from an initialise routine.
 */

BOOLN
SetUniParList_AppInterface(void)
{
	static const char *funcName = "SetUniParList_AppInterface";
	UniParPtr	pars;

	if ((appInterfacePtr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  APP_INT_NUM_PARS, NULL)) == NULL) {
		NotifyError("%s: Could not initialise parList.", funcName);
		return(FALSE);
	}
	pars = appInterfacePtr->parList->pars;
	SetPar_UniParMgr(&pars[APP_INT_DIAGNOSTICMODE], "DIAG_MODE",
	  "Diagnostics mode specifier ('off', 'screen' or <filename>).",
	  UNIPAR_NAME_SPEC_WITH_FILE,
	  &appInterfacePtr->diagModeSpecifier, appInterfacePtr->diagModeList,
	  (void * (*)) SetDiagMode_AppInterface);
	SetPar_UniParMgr(&pars[APP_INT_SIMULATIONFILE], "SIM_FILE",
	  "Simulation file.",
	  UNIPAR_FILE_NAME,
	  &appInterfacePtr->simulationFile, (char *) "Sim. Par File (*.spf)|*.spf|"
	  "Sim. script (*.sim)|*.sim|All files (*.*)|*.*",
	  (void * (*)) SetSimulationFile_AppInterface);
	SetPar_UniParMgr(&pars[APP_INT_SEGMENTMODE], "SEGMENT_MODE",
	  "Segmented or frame-base processing mode ('on' or 'off').",
	  UNIPAR_BOOL,
	  &appInterfacePtr->segmentModeSpecifier, NULL,
	  (void * (*)) SetSegmentMode_AppInterface);
	SetPar_UniParMgr(&pars[APP_INT_PARLIST], "PAR_LIST",
	  "App. Specific Pars.",
	  UNIPAR_PARLIST,
	  &appInterfacePtr->appParList, NULL,
	  (void * (*)) SetUniParList_AppInterface);
	return(TRUE);

}

/****************************** SetDiagMode ***********************************/

/*
 * This functions sets the diagnostic mode.
 * It returns false if it fails in any way.
 */

BOOLN
SetDiagMode_AppInterface(char *theDiagMode)
{
	static const char	*funcName = "SetDiagMode_AppInterface";

	if (!appInterfacePtr) {
		NotifyError("%s: Application interface not initialised.", funcName);
		return(FALSE);
	}
	appInterfacePtr->diagModeSpecifier = IdentifyDiag_NSpecLists(theDiagMode,
	  appInterfacePtr->diagModeList);
	SetParsFile_Common(appInterfacePtr->diagModeList[
	  appInterfacePtr->diagModeSpecifier].name, OVERWRITE);
	return(TRUE);

}

/****************************** SetSimulationFile *****************************/

/*
 * This functions sets the simulation file name.
 * It returns false if it fails in any way.
 * It also sets the global "simLastModified" variable to zero.
 */

BOOLN
SetSimulationFile_AppInterface(char *theSimulationFile)
{
	static const char	*funcName = "SetSimulationFile_AppInterface";

	if (!appInterfacePtr) {
		NotifyError("%s: Application interface not initialised.", funcName);
		return(FALSE);
	}
	if (*theSimulationFile == '\0') {
		NotifyError("%s: Illegal zero length name.", funcName);
		return(FALSE);
	}
	snprintf(appInterfacePtr->simulationFile, MAX_FILE_PATH, "%s",
	  theSimulationFile);
	appInterfacePtr->simulationFileFlag = TRUE;
	appInterfacePtr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/****************************** SetSegmentMode ********************************/

/*
 * This functions sets the segment processing mode.
 * It returns false if it fails in any way.
 */

BOOLN
SetSegmentMode_AppInterface(char *theSegmentMode)
{
	static const char	*funcName = "SetSegmentMode_AppInterface";

	if (!appInterfacePtr) {
		NotifyError("%s: Application interface not initialised.", funcName);
		return(FALSE);
	}
	if ((appInterfacePtr->segmentModeSpecifier = Identify_NameSpecifier(
	  theSegmentMode, BooleanList_NSpecLists(0))) == GENERAL_BOOLEAN_NULL) {
		NotifyError("%s: Illegal segment processing mode (%s): must be "
		  "'on' or 'off'.", funcName, theSegmentMode);
		return(FALSE);
	}
	strcpy(appInterfacePtr->segmentMode, theSegmentMode);
	return(TRUE);

}

/****************************** SetMaxUserModules *****************************/

/*
 * This functions sets the application's final pars routine.
 * These are parameters which set by the application interace, after the
 * simulation is initialised.
 * It returns false if it fails in any way.
 */

BOOLN
SetMaxUserModules_AppInterface(int maxUserModules)
{
	static const char	*funcName = "SetMaxUserModules_AppInterface";

	if (!appInterfacePtr) {
		NotifyError("%s: Application interface not initialised.", funcName);
		return(FALSE);
	}
	appInterfacePtr->maxUserModules = maxUserModules;
	return(TRUE);

}

/****************************** SetInstallDir *********************************/

/*
 * This functions sets the installation directory.
 * It returns false if it fails in any way.
 */

BOOLN
SetInstallDir_AppInterface(char *theInstallDir)
{
	static const char	*funcName = "SetInstallDir_AppInterface";

	if (!appInterfacePtr) {
		NotifyError("%s: Application interface not initialised.", funcName);
		return(FALSE);
	}
	snprintf(appInterfacePtr->installDir, MAX_FILE_PATH, "%s", theInstallDir);
	return(TRUE);

}

/****************************** SetAppParFile *********************************/

/*
 * This functions sets the application's parameter file name'.
 * It returns false if it fails in any way.
 */

BOOLN
SetAppParFile_AppInterface(char *fileName)
{
	static const char	*funcName = "SetAppParFile_AppInterface";

	if (!appInterfacePtr) {
		NotifyError("%s: Application interface not initialised.", funcName);
		return(FALSE);
	}
	if ((fileName == NULL) || (*fileName == '\0')) {
		NotifyError("%s: illegal file name.", funcName);
		return(FALSE);
	}
	snprintf(appInterfacePtr->appParFile, MAX_FILE_PATH, "%s", fileName);
	appInterfacePtr->appParFileFlag = TRUE;
	return(TRUE);

}

/****************************** SetAppParList *********************************/

/*
 * This functions sets the application's extra 'parList'.
 * It returns false if it fails in any way.
 */

BOOLN
SetAppParList_AppInterface(UniParListPtr appParList)
{
	static const char	*funcName = "SetAppParList_AppInterface";

	if (!appInterfacePtr) {
		NotifyError("%s: Application interface not initialised.", funcName);
		return(FALSE);
	}
	appInterfacePtr->appParList = appParList;
	return(TRUE);

}

/****************************** SetAppPrintUsage ******************************/

/*
 * This functions sets the application's print usage routine.
 * It returns false if it fails in any way.
 */

BOOLN
SetAppPrintUsage_AppInterface(void (* PrintUsage)(void))
{
	static const char	*funcName = "SetAppPrintUsage_AppInterface";

	if (!appInterfacePtr) {
		NotifyError("%s: Application interface not initialised.", funcName);
		return(FALSE);
	}
	appInterfacePtr->PrintUsage = PrintUsage;
	return(TRUE);

}

/****************************** SetAppProcessOptions **************************/

/*
 * This functions sets the application's process options routine.
 * It returns false if it fails in any way.
 */

BOOLN
SetAppProcessOptions_AppInterface(int (* ProcessOptions)(int, char **, int *))
{
	static const char	*funcName = "SetAppProcessOptions_AppInterface";

	if (!appInterfacePtr) {
		NotifyError("%s: Application interface not initialised.", funcName);
		return(FALSE);
	}
	appInterfacePtr->ProcessOptions = ProcessOptions;
	return(TRUE);

}

/****************************** SetAppReadInitialPars *************************/

/*
 * This functions sets the application's Initial pars routine.
 * These are parameters which need to be set by the application interace, as
 * they are being used by the application interface.
 * It returns false if it fails in any way.
 */

BOOLN
SetAppReadInitialPars_AppInterface(BOOLN (* ReadInitialPars)(char *))
{
	static const char	*funcName = "SetAppReadInitialPars_AppInterface";

	if (!appInterfacePtr) {
		NotifyError("%s: Application interface not initialised.", funcName);
		return(FALSE);
	}
	appInterfacePtr->ReadInitialPars = ReadInitialPars;
	return(TRUE);

}

/****************************** SetAppSetFinalPars ****************************/

/*
 * This functions sets the application's final pars routine.
 * These are parameters which set by the application interace, after the
 * simulation is initialised.
 * It returns false if it fails in any way.
 */

BOOLN
SetAppSetFinalPars_AppInterface(BOOLN (* SetFinalPars)(void))
{
	static const char	*funcName = "SetAppSetFinalPars_AppInterface";

	if (!appInterfacePtr) {
		NotifyError("%s: Application interface not initialised.", funcName);
		return(FALSE);
	}
	appInterfacePtr->SetFinalPars = SetFinalPars;
	return(TRUE);

}

/****************************** SetAppRegisterUserModules *********************/

/*
 * This functions sets the application's RegisterUserModules routine.
 * This routine registers the users modules so that they can be used by the
 * simulation script interface.
 * It returns FALSE if it fails in any way.
 */

BOOLN
SetAppRegisterUserModules_AppInterface(BOOLN (* RegisterUserModules)(void))
{
	static const char	*funcName = "SetAppRegisterUserModules_AppInterface";

	if (!appInterfacePtr) {
		NotifyError("%s: Application interface not initialised.", funcName);
		return(FALSE);
	}
	appInterfacePtr->RegisterUserModules = RegisterUserModules;
	return(TRUE);

}

/****************************** PrintInitialDiagnostics ***********************/

/*
 * This routine prints the initial diagnostics for the program.
 */

void
PrintPars_AppInterface(void)
{
	static const char *funcName = "PrintInitialDiagnostics_AppInterface";
	if (!appInterfacePtr) {
		NotifyError("%s: Application interface not initialised.", funcName);
		return;
	}
	DPrint("Simulation script application interface settings:\n");
	DPrint("Diagnostics specifier is set to '%s'.\n",
	  appInterfacePtr->diagModeList[appInterfacePtr->diagModeSpecifier].name);
	DPrint("This simulation is run from the file '%s'.\n",
	  appInterfacePtr->simulationFile);
	DPrint("\n");

}

/****************************** PrintUsage ************************************/

/* * This routine prints the usage diagnostics.
 */

void
PrintUsage_AppInterface(void)
{
	static const char *funcName = "PrintUsage_AppInterface";

	if (!appInterfacePtr) {
		NotifyError("%s: Application interface not initialised.", funcName);
		return;
	}
	fprintf(stderr, "\n"
	  "Usage: %s [options] [parameter1 value1 parameter2 value2 ...]\n"
	  "\t-d <state>    \t: Diagnostics mode ('off', 'screen' or filename).\n"
	  "\t-h            \t: Produce this help message.\n"
	  "\t-l <list>     \t: List options: 'parameters', 'cfinfo'.\n"
	  "\t-P <file name>\t: Use this main parameter file\n"
	  "\t-s <file name>\t: Use this simulation file (*.spf or *.sim)\n",
	  appInterfacePtr->appName);

}

/****************************** ProcessParComs ********************************/

/*
 * This routine processes parameter-value command pairs, passed as command-line
 * arguments.
 * These parameters are only used once, then the 'useParComsFlag' is set to
 * FALSE. If a new set commands is given (as in the server/client mode) then
 * this flag needs to be set to TRUE.
 */

BOOLN
ProcessParComs_AppInterface(void)
{
	static const char *funcName = "ProcessParComs_AppInterface";
	int		i;
	DatumPtr	simulation;


	if (!appInterfacePtr) {
		NotifyError("%s: Application interface not initialised.", funcName);
		return(FALSE);
	}
	if (!appInterfacePtr->useParComsFlag)
		return(TRUE);

	if ((simulation = GetSimulation_ModuleMgr(appInterfacePtr->audModel)) ==
	  NULL) {
		NotifyError("%s: Simulation not initialised.", funcName);
		return(FALSE);
	}
	if ((appInterfacePtr->argc - appInterfacePtr->initialCommand) % 2 != 0) {
		NotifyError("%s: parameter values must be in <name> <value> pairs.",
		  funcName);
		return(FALSE);
	}
	for (i = appInterfacePtr->initialCommand; i < appInterfacePtr->argc; i += 2)
		if (!SetProgramParValue_AppInterface(appInterfacePtr->argv[i],
		  appInterfacePtr->argv[i + 1]) && !SetUniParValue_Utility_Datum(
		  simulation, appInterfacePtr->argv[i], appInterfacePtr->argv[i + 1])) {
			NotifyError("%s: Could not set '%s' parameter to '%s'.", funcName,
			  appInterfacePtr->argv[i], appInterfacePtr->argv[i + 1]);
			return(FALSE);
		}
	appInterfacePtr->useParComsFlag = FALSE;
	return(TRUE);

}

/****************************** ProcessOptions ********************************/

/*
 * This routine processes any command-line options.
 * The '@' options is to be ignored.  It is used by other modules, i.e. myApp.
 * This routine should only change flags and/or.  This allows it to be run
 * more than once.
 */

BOOLN
ProcessOptions_AppInterface(void)
{
	static const char *funcName = "ProcessOptions_AppInterface";
	BOOLN	optionFound = FALSE, ok = TRUE;
	int		optInd = 1, optSub = 0;
	char	c, *argument;

	if (!appInterfacePtr) {
		NotifyError("%s: Application interface not initialised.", funcName);
		return(FALSE);
	}
	while ((c = Process_Options(appInterfacePtr->argc, appInterfacePtr->argv,
	  &optInd, &optSub, &argument, "@#:d:hl:P:s:"))) {
		optionFound = TRUE;
		switch (c) {
		case '@':
		case '#':
			break;
		case 'd':
			ok = SetProgramParValue_AppInterface(appInterfacePtr->parList->pars[
			  APP_INT_DIAGNOSTICMODE].abbr, argument);
			break;
		case 'h':
			appInterfacePtr->printUsageFlag = TRUE;
			break;
		case 'l':
			switch (Identify_NameSpecifier(argument,
			  appInterfacePtr->listingModeList)) {
			case APP_INT_PARAMETERS_LIST_MODE:
				appInterfacePtr->listParsAndExit = TRUE;
				break;
			case APP_INT_CFLIST_LIST_MODE:
				appInterfacePtr->listCFListAndExit = TRUE;
				break;
			default:
				fprintf(stderr, "Unknown list mode (%s).\n", argument);
				appInterfacePtr->printUsageFlag = TRUE;
			}
			break;
		case 'P':
			snprintf(appInterfacePtr->appParFile, MAX_FILE_PATH, "%s",
			  argument);
			appInterfacePtr->readAppParFileFlag = TRUE;
			break;
		case 's':
			if (!SetSimulationFile_AppInterface(argument))
				ok = FALSE;
			break;
		default:
			if (appInterfacePtr->ProcessOptions && (* appInterfacePtr->
			  ProcessOptions)(appInterfacePtr->argc, appInterfacePtr->argv,
			  &optInd)) {
				optSub = 0;
				break;
			}
			appInterfacePtr->printUsageFlag = TRUE;
		} /* switch */
	}
	appInterfacePtr->initialCommand = (optionFound)? optInd: 1;
	return(ok);

}

/****************************** GetFileStatusPtr ******************************/

/*
 * This routine returns a pointer to the status of a file.
 * The status of the file is a static variable, so the pointer will change if
 * other files are checked with this routine.
 * It returns NULL if the file does not exist or there is any other error.
 */

StatPtr
GetFileStatusPtr_AppInterface(char *fileName)
{
	static const char *funcName = "GetFileStatusPtr_AppInterface";
	static struct stat	simulationFileStat;

	if (!appInterfacePtr) {
		NotifyError("%s: Application interface not initialised.", funcName);
		return(FALSE);
	}
	if (stat(fileName, &simulationFileStat) == -1) {
		NotifyError("%s: Could not get status of file\n'%s'.", funcName,
		  fileName);
		return(NULL);
	}
	return(&simulationFileStat);

}

/****************************** SimulationFileChanged *************************/

/*
 * This routine checks whether the simulation script file has been changed.
 * It will return FALSE if the global 'simLastModified' was originally unset.
 */

BOOLN
SimulationFileChanged_AppInterface(StatPtr simulationFileStat,
  BOOLN updateStatus)
{
	static const char *funcName = "SimulationFileChanged_AppInterface";
	BOOLN	changed;

	if (!appInterfacePtr) {
		NotifyError("%s: Application interface not initialised.", funcName);
		return(FALSE);
	}
	if (!simulationFileStat) {
		NotifyError("%s: Could not get status of simulation script "
		  "file.", funcName);
		return(FALSE);
	}
	if (appInterfacePtr->simLastModified != simulationFileStat->st_mtime) {
		changed = (appInterfacePtr->simLastModified != 0);
		if (updateStatus)
			appInterfacePtr->simLastModified = simulationFileStat->st_mtime;
		return(changed);
	}
	return(FALSE);

}

/************************ SetProgramParValue **********************************/

/*
 * This function sets a program parameter.
 * The 'localParFilePtr' pointer is required because the setting of the
 * program/simulation parameters can involve calling the 'Init_ParFile' routine.
 * It returns FALSE if it fails in any way.
 * A copy of the 'parList' must be taken, as the argument 'parList' it can be
 * changed by the 'FindUniPar_UniParMgr' routine.
 */

BOOLN
SetProgramParValue_AppInterface(char *parName, char *parValue)
{
	BOOLN	ok = TRUE;
	char	*p, parNameCopy[MAXLINE];
	UniParPtr	par;
	ParFilePtr	localParFilePtr;
	UniParListPtr	parList;

	localParFilePtr = parFile;
	parFile = NULL;
	snprintf(parNameCopy, MAXLINE, "%s", parName);
	if ((p = strchr(parNameCopy, UNIPAR_NAME_SEPARATOR)) != NULL)
		*p = '\0';
	parList = appInterfacePtr->parList;
	if ((par = FindUniPar_UniParMgr(&parList, parNameCopy)) == NULL)
		ok = FALSE;
	else if (((parList != appInterfacePtr->parList) || (par->index !=
	  APP_INT_SIMULATIONFILE) || (appInterfacePtr->audModel &&
	 GetSimParFileFlag_ModuleMgr(appInterfacePtr->audModel))) &&
	  !SetParValue_UniParMgr(&parList, par->index, parValue))
		ok = FALSE;
	parFile = localParFilePtr;
	return(ok);

}

/****************************** GetSimulation *********************************/

/*
 * This function returns a pointer to the application interface's audModel
 * simulation.
 */

DatumPtr
GetSimulation_AppInterface(void)
{
	static const char *funcName = "GetSimulation_AppInterface";
	DatumPtr	simulation;

	if (!appInterfacePtr) {
		NotifyError("%s: Application interface not initialised.", funcName);
		return(NULL);
	}
	if (!appInterfacePtr->audModel) {
		NotifyError("%s: Simulation EarObject nod initialised.", funcName);
		return(NULL);
	}
	if ((simulation = GetSimulation_ModuleMgr(appInterfacePtr->audModel)) ==
	   NULL) {
		NotifyError("%s: Simulation not initialised.", funcName);
		return(NULL);
	}
	return(simulation);

}

/****************************** GetEarObjectPtr *******************************/

/*
 * This function returns a pointer to the application interface's audModel
 * EarObject.
 */

EarObjectPtr
GetEarObjectPtr_AppInterface(void)
{
	static const char *funcName = "GetEarObjectPtr_AppInterface";

	if (!appInterfacePtr) {
		NotifyError("%s: Application interface not initialised.", funcName);
		return(NULL);
	}
	if (!appInterfacePtr->audModel) {
		NotifyError("%s: Simulation EarObject nod initialised.", funcName);
		return(NULL);
	}
	return(appInterfacePtr->audModel);

}

/****************************** ReadPars **************************************/

/*
 * This program reads a specified number of parameters using the specified file
 * pointer.
 * It is meant to be read from within the Main Program ReadPars routine.
 * It returns FALSE if it fails in any way.n */

BOOLN
ReadPars_AppInterface(FILE *fp)
{
	static const char *funcName = "ReadPars_AppInterface";
	BOOLN	ok = TRUE;
	char	diagnosticMode[MAX_FILE_PATH], simulationFile[MAX_FILE_PATH];
	char	segmentMode[MAXLINE];

	if (!fp) {
		NotifyError("%s: File pointer not set.", funcName);
		return(FALSE);
	}
	if (!GetPars_ParFile(fp, "%s", diagnosticMode))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%s", simulationFile))
		ok = FALSE;
	if (!GetPars_ParFile(fp, "%s", segmentMode))
		ok = FALSE;
	if (!ok) {
		NotifyError("%s: Failed to read parameters.", funcName);
		return(FALSE);
	}
    if (!SetPars_AppInterface(diagnosticMode, simulationFile, segmentMode)) {
		NotifyError("%s: Could not set parameters.", funcName);
		return(FALSE);
	}
	return(TRUE);

}

/************************ ReadProgParFile *************************************/

/*
 * This routine reads the parameters, saved in the simulation parameter file,
* which control the main program parameters.  These parameters are found
 * after the 'SIMSCRIPT_SIMPARFILE_DIVIDER' string, if one exists.
 * The 'filePath' variable must have storage space here, as setting one of the
 * parameter values may overwrite the 'static' storage for the
 * 'GetParsFileFPath_DSAM' routine.
 * This routine currently ignores any characters after and including a period,
 * "." in the parameter name.
 * It returns FALSE if it fails in any way.
 */

BOOLN
ReadProgParFile_AppInterface(void)
{
	static const char		*funcName = "ReadProgParFile_AppInterface";
	static BOOLN	readProgParFileFlag = FALSE;
	BOOLN	ok = TRUE, foundDivider = FALSE;
	char	parName[MAXLINE], parValue[MAX_FILE_PATH], oldSPF[MAX_FILE_PATH];
	FILE	*fp;

	if (!appInterfacePtr) {
		NotifyError("%s: Application interface not initialised.", funcName);
		return(FALSE);
	}
	if (readProgParFileFlag)
		return(TRUE);
	readProgParFileFlag = TRUE;
	if ((fp = fopen(appInterfacePtr->simulationFile, "r")) == NULL) {
		NotifyError("%s: Could not open '%s' parameter file.", funcName,
		  appInterfacePtr->simulationFile);
		readProgParFileFlag = FALSE;
		return(FALSE);
	}
	strcpy(oldSPF, appInterfacePtr->simulationFile);
	Init_ParFile();
	SetEmptyLineMessage_ParFile(FALSE);
	while (!foundDivider && GetPars_ParFile(fp, "%s %s", parName, parValue))
		if (strcmp(parName, SIMSCRIPT_SIMPARFILE_DIVIDER) == 0)
			foundDivider = TRUE;
	if (!foundDivider) {
		Free_ParFile();
		readProgParFileFlag = FALSE;
		return(TRUE);
	}
	while (ok && GetPars_ParFile(fp, "%s %s", parName, parValue))
		ok = SetProgramParValue_AppInterface(parName, parValue);
	SetEmptyLineMessage_ParFile(TRUE);
	fclose(fp);
	Free_ParFile();
	readProgParFileFlag = FALSE;
	if (!ok) {
		NotifyError("%s: Invalid parameters in file '%s', program parameter "
		  "section (%s).", funcName, appInterfacePtr->simulationFile, parName);
		return(FALSE);
	}
	strcpy(appInterfacePtr->simulationFile, oldSPF);
	return(TRUE);

}

/****************************** InitSimFromSimScript **************************/

/*
 * This routine Initialises the simulation from a simulation script.
 */

BOOLN
InitSimFromSimScript_AppInterface(void)
{
	static const char *funcName = "InitSimFromSimScript_AppInterface";
	StatPtr	simulationFileStatPtr;

	if (!appInterfacePtr) {
		NotifyError("%s: Application interface not initialised.", funcName);
		return(FALSE);
	}
	if ((simulationFileStatPtr = GetFileStatusPtr_AppInterface(
	  appInterfacePtr->simulationFile)) == NULL) {
		NotifyError("%s: Could not find status of simulation script\nfile"
		  "'%s'", funcName, appInterfacePtr->simulationFile);
		appInterfacePtr->updateProcessVariablesFlag = TRUE;
		return(FALSE);
	}
	if (!appInterfacePtr->audModel || SimulationFileChanged_AppInterface(
	  simulationFileStatPtr, TRUE)) {
		FreeAll_EarObject();
		datumStepCount = 0;
		if ((appInterfacePtr->audModel = Init_EarObject("Util_SimScript")) ==
		  NULL) {
			NotifyError("%s: Could not initialise process.", funcName);
			FreeAll_EarObject();
			appInterfacePtr->updateProcessVariablesFlag = TRUE;
			appInterfacePtr->checkMainInit = TRUE;
			return(FALSE);
		}
		
	}
	return(TRUE);

}

/****************************** InitSimulation ********************************/

/*
 * This routine Initialises the simulation.
 */

BOOLN
InitSimulation_AppInterface(void)
{
	static const char *funcName = "InitSimulation_AppInterface";
	BOOLN	ok = TRUE;
	StatPtr	simulationFileStatPtr;

	if (!appInterfacePtr) {
		NotifyError("%s: Application interface not initialised.", funcName);
		return(FALSE);
	}
	if ((simulationFileStatPtr = GetFileStatusPtr_AppInterface(
	  appInterfacePtr->simulationFile)) == NULL) {
		NotifyError("%s: Could not find status of file '%s'", funcName,
		  appInterfacePtr->simulationFile);
		appInterfacePtr->updateProcessVariablesFlag = TRUE;
		return(FALSE);
	}
	if (!appInterfacePtr->audModel || SimulationFileChanged_AppInterface(
	  simulationFileStatPtr, TRUE)) {
		FreeAll_EarObject();
		datumStepCount = 0;
		if ((appInterfacePtr->audModel = Init_EarObject("Util_SimScript")) ==
		  NULL) {
			NotifyError("%s: Could not initialise process.", funcName);
			ok = FALSE;
		}
	}
	if (ok && !ReadPars_ModuleMgr(appInterfacePtr->audModel, appInterfacePtr->
	  simulationFile))
		ok = FALSE;
	if (!ok) {
		FreeAll_EarObject();
		appInterfacePtr->audModel = NULL;
		appInterfacePtr->updateProcessVariablesFlag = TRUE;
		return(FALSE);
	}

	SetTitle_AppInterface(appInterfacePtr->simulationFile);
	if (GetDSAMPtr_Common()->usingGUIFlag)
		PrintPars_AppInterface();
	return(TRUE);

}

/****************************** SetAppName ************************************/

/*
 * This routine sets the global appName variable.
 */

BOOLN
SetAppName_AppInterface(char *appName)
{
	static const char *funcName = "SetAppName_AppInterface";

	if (!appInterfacePtr) {
		NotifyError("%s: Application interface not initialised.", funcName);
		return(FALSE);
	}

	snprintf(appInterfacePtr->appName, MAX_FILE_PATH, "%s", appName);
	return(TRUE);

} 

/****************************** SetAppVersion *********************************/

/*
 * This routine sets the appVersion variable.
 */

BOOLN
SetAppVersion_AppInterface(char *appVersion)
{
	static const char *funcName = "SetAppVersion_AppInterface";

	if (!appInterfacePtr) {
		NotifyError("%s: Application interface not initialised.", funcName);
		return(FALSE);
	}

	strcpy(appInterfacePtr->appVersion, appVersion);
	return(TRUE);

} 

/****************************** SetCompiledDSAMVersion ************************/

/*
 * This routine sets the version of DSAM that the application was compiled
 * with, as compared to the dynamic library that is being used.
 */

BOOLN
SetCompiledDSAMVersion_AppInterface(char *compiledDSAMVersion)
{
	static const char *funcName = "SetCompiledDSAMVersion_AppInterface";

	if (!appInterfacePtr) {
		NotifyError("%s: Application interface not initialised.", funcName);
		return(FALSE);
	}

	strcpy(appInterfacePtr->compiledDSAMVersion, compiledDSAMVersion);
	return(TRUE);

} 

/****************************** SetTitle **************************************/

/*
 * This routine sets the title that is used in the simulation manager window.
 */

BOOLN
SetTitle_AppInterface(char *title)
{
	static const char *funcName = "SetTitle_AppInterface";

	if (!appInterfacePtr) {
		NotifyError("%s: Application interface not initialised.", funcName);
		return(FALSE);
	}
	snprintf(appInterfacePtr->title, MAXLINE, "%s", title);
	return(TRUE);

} 

/****************************** SetPars ***************************************/

/*
 * This routine carries out general initialisation tasks for the application
 * interface.
 */

BOOLN
SetPars_AppInterface(char *diagMode, char *simulationFile, char *segmentMode)
{
	static const char *funcName = "SetPars_AppInterface";

	if (!appInterfacePtr) {
		NotifyError("%s: Application interface not initialised.", funcName);
		return(FALSE);
	}
	if (!SetDiagMode_AppInterface(diagMode)) {
		NotifyError("%s: Could not set diagnostic mode.", funcName);
		return(FALSE);
	}
	if (!SetSimulationFile_AppInterface(simulationFile)) {
		NotifyError("%s: Could not set simulation file mode.", funcName);
		return(FALSE);
	}
	if (!SetSegmentMode_AppInterface(segmentMode)) {
		NotifyError("%s: Could not set segment processing mode.", funcName);
		return(FALSE);
	}
	appInterfacePtr->checkMainInit = FALSE;
	return(TRUE);

}

/****************************** ListParsAndExit *******************************/

/*
 * This routine prints the simulation parameters and exits.
 */

void
ListParsAndExit_AppInterface(void)
{	
	SetGUIDialogStatus(FALSE);
	SetParsFile_Common("screen", OVERWRITE);
	ListParameters_AppInterface();
	exit(0);

}

/****************************** ListParameters ********************************/

/*
 * This routine prints the simulation and main program parameters.
 */

BOOLN
ListParameters_AppInterface(void)
{
	static const char *funcName = "ListParameters_AppInterface";
	char	suffix[MAXLINE];
	
	if (!appInterfacePtr) {
		NotifyError("%s: Application interface not initialised.", funcName);
		return(FALSE);
	}
	if (!PrintSimParFile_ModuleMgr(appInterfacePtr->audModel)) {
		NotifyError("%s: Could not print simulation parameter file.", funcName);
		return(FALSE);
	}
	snprintf(suffix, MAXLINE, ".%s.0", appInterfacePtr->appName);
	PrintPars_UniParMgr(appInterfacePtr->parList, "", suffix);
	return(TRUE);

}

/****************************** ListCFListAndExit *****************************/

/*
 * This routine prints the simulation parameters and exits.
 */

void
ListCFListAndExit_AppInterface(void)
{
	static const char *funcName = "ListCFListAndExit_AppInterface";
	CFListPtr	theBMCFs;
	DatumPtr	simulation, bMDatumPtr;
	
	if (!appInterfacePtr) {
		NotifyError("%s: Application interface not initialised.", funcName);
		exit(1);
	}
	if ((simulation = GetSimulation_ModuleMgr(appInterfacePtr->audModel)) ==
	   NULL) {
		NotifyError("%s: Simulation not initialised.", funcName);
		exit(1);
	}
	SetGUIDialogStatus(FALSE);
	SetParsFile_Common("screen", OVERWRITE);
	if (((bMDatumPtr = FindModuleProcessInst_Utility_Datum(simulation,
	  "BM_")) == NULL) || ((theBMCFs = *GetUniParPtr_ModuleMgr(bMDatumPtr->data,
	  "cflist")->valuePtr.cFPtr) == NULL)) {
		NotifyError("%s: Could not list CFList.", funcName);
		exit(1);
	}
	PrintList_CFList(theBMCFs);
	exit(0);

}

/****************************** ResetCommandArgFlags **************************/

/*
 * This routine sets the appInterfacePtr command arg flags so that the
 * InitProcessVariables routine knows when to process the options or parameter
 * commands.
 */

void
ResetCommandArgFlags_AppInterface(void)
{
	static const char *funcName = "ResetCommandArgFlags_AppInterface";

	if (!appInterfacePtr) {
		NotifyError("%s: Application interface not initialised.", funcName);
		exit(1);
	}
	appInterfacePtr->checkMainInit = TRUE;
	appInterfacePtr->useParComsFlag = TRUE;

}

/****************************** SetArgcAndArgV ********************************/

/*
 * This routine sets the appInterfacePtr argc and argv parameters.
 * The global variables are also set.
 */

void
SetArgcAndArgV_AppInterface(int theArgc, char **theArgv)
{
	static const char *funcName = "SetArgcAndArgV_AppInterface";
	
	if (!appInterfacePtr) {
		NotifyError("%s: Application interface not initialised.", funcName);
		exit(1);
	}
	appInterfacePtr->argc = theArgc;
	appInterfacePtr->argv = theArgv;

}

/****************************** InitProcessVariables **************************/

/*
 * This function initialises the simulation process.
 * This routine is a special one, in that it actually initialises the
 * application interface too.
 */

BOOLN
InitProcessVariables_AppInterface(BOOLN (* Init)(void), int theArgc,
  char **theArgv)
{
	static const char *funcName = "InitProcessVariables_AppInterface";

	if (!appInterfacePtr && !Init_AppInterface(GLOBAL)) {
		NotifyError("%s: Could not initialise the application interface.",
		  funcName);
		exit(1);
	}

	if (Init) {
		if (!GetDSAMPtr_Common()->appInitialisedFlag && !(* Init)())
			return(FALSE);
		if (!GetDSAMPtr_Common()->appInitialisedFlag) {
			appInterfacePtr->Init = Init;
			GetDSAMPtr_Common()->appInitialisedFlag = TRUE;
			if (GetDSAMPtr_Common()->usingGUIFlag)
				return(FALSE);
		}
	}

	SetArgcAndArgV_AppInterface(theArgc, theArgv);
	if (appInterfacePtr->checkMainInit) {
		ProcessOptions_AppInterface();
		if (appInterfacePtr->printUsageFlag) {
			PrintUsage_AppInterface();
			if (appInterfacePtr->PrintSimMgrUsage) (* appInterfacePtr->
			  PrintSimMgrUsage)();
			if (appInterfacePtr->PrintUsage)
				(* appInterfacePtr->PrintUsage)();
			exit(0);
		}
	}
	if (appInterfacePtr->updateProcessVariablesFlag) {
		if (appInterfacePtr->readAppParFileFlag) {
			if (appInterfacePtr->ReadInitialPars && !(* appInterfacePtr->
			  ReadInitialPars)(appInterfacePtr->appParFile)) {
				NotifyError("%s: Failed to set initial parameters.", funcName);
				return(FALSE);
			}
			appInterfacePtr->readAppParFileFlag = FALSE;
		}
		if (appInterfacePtr->RegisterUserModules) {
			if (!InitUserModuleList_ModuleReg(appInterfacePtr->
			  maxUserModules)) {
				NotifyError("%s: Could not initialise user module list.",
				  funcName);
				return(FALSE);
			}
			if (!(* appInterfacePtr->RegisterUserModules)()) {
				NotifyError("%s: Failed to register user modules.", funcName);
				return(FALSE);
			}
		}
		DPrint("Starting %s Application version %s [DSAM Version: %s "
		  "(dynamic),\n%s (compiled)]...\n", appInterfacePtr->appName,
		  appInterfacePtr->appVersion, GetDSAMPtr_Common()->version,
		  DSAM_VERSION);

		if (!InitSimulation_AppInterface()) {
			NotifyError("%s: Could not Initialise simulation.", funcName);
			return(FALSE);
		}
		if (GetSimParFileFlag_ModuleMgr(appInterfacePtr->audModel) && 
		  !ReadProgParFile_AppInterface()) {
			NotifyError("%s: Could not read the program settings in\nfile "
			  "'%s'.", funcName, appInterfacePtr->simulationFile);
			return(FALSE);
		}
		if (!ProcessParComs_AppInterface())
			return(FALSE);

		SetSegmentedMode(appInterfacePtr->segmentModeSpecifier);

		if (appInterfacePtr->listParsAndExit)
			ListParsAndExit_AppInterface();
		if (appInterfacePtr->listCFListAndExit)
			ListCFListAndExit_AppInterface();

		appInterfacePtr->updateProcessVariablesFlag = FALSE;

		if (appInterfacePtr->SetFinalPars &&
		  !(* appInterfacePtr->SetFinalPars)()) {
				NotifyError("%s: Failed to set final parameters.", funcName);
				return(FALSE);
		}
	}
	appInterfacePtr->checkMainInit = FALSE;
	if (!ProcessParComs_AppInterface())
		return(FALSE);
	return(TRUE);

}

/****************************** SetCanFreePtrFlag *****************************/

/*
 * This routine set the structure's 'canFreePtrFlag' which is used by the
 * "Free_" to decide whether it does anything when called.
 */

BOOLN
SetCanFreePtrFlag_AppInterface(BOOLN status)
{
	static const char *funcName = "SetCanFreePtrFlag_AppInterface";

	if (!appInterfacePtr) {
		NotifyError("%s: Application interface not initialised.", funcName);
		return(FALSE);
	}
	appInterfacePtr->canFreePtrFlag = status;
	return(TRUE);

}

/****************************** GetPtr ****************************************/

/*
 * This function returns the application interface pointer.
 * This routine is useful for the VC++ compile.
 */

AppInterfacePtr
GetPtr_AppInterface(void)
{
	static const char *funcName = "GetPtr_AppInterface";

	if (!appInterfacePtr) {
		NotifyError("%s: Application interface not initialised.", funcName);
		return(FALSE);
	}
	return(appInterfacePtr);

}

