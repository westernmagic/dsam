/**********************
 *
 * File:		UtAppInterface.c
 * Purpose:		This Module contains the interface routines for DSAM
 *				application's.
 * Comments:	The 'PostInitFunc' routine is run at the end of the
 *				'InitProcessVariables' routine.
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


#include "GeCommon.h"
#include "GeSignalData.h"
#include "GeEarObject.h"
#include "GeUniParMgr.h"
#include "GeNSpecLists.h"
#include "GeModuleMgr.h"
#include "GeModuleReg.h"

#include "UtSSSymbols.h"
#include "UtSimScript.h"
#include "UtOptions.h"
#include "UtString.h"

#include "FiParFile.h"

#include "UtAppInterface.h"

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
	GetDSAMPtr_Common()->appInitialisedFlag = FALSE;
	if (appInterfacePtr->diagModeList)
		free(appInterfacePtr->diagModeList);
	if (appInterfacePtr->parList)
		FreeList_UniParMgr(&appInterfacePtr->parList);
	if (appInterfacePtr->appParList)
		FreeList_UniParMgr(&appInterfacePtr->appParList);
	if (appInterfacePtr->audModel)
		Free_EarObject(&appInterfacePtr->audModel);
	if (appInterfacePtr->FreeAppProcessVars)
		(* appInterfacePtr->FreeAppProcessVars)();
	if (appInterfacePtr->parSpec == GLOBAL) {
		free(appInterfacePtr);
		appInterfacePtr = NULL;
	}
	CloseFiles();
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
			Free_AppInterface();
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
	appInterfacePtr->canLoadSimulationFlag = TRUE;
	appInterfacePtr->numThreadsFlag = TRUE;
	appInterfacePtr->listParsAndExit = FALSE;
	appInterfacePtr->listCFListAndExit = FALSE;
	appInterfacePtr->appParFileFlag = FALSE;
	appInterfacePtr->readAppParFileFlag = FALSE;
	appInterfacePtr->printUsageFlag = FALSE;
	appInterfacePtr->simulationFinishedFlag = TRUE;
	strcpy(appInterfacePtr->appName, "No Name");
	strcpy(appInterfacePtr->appParFile, NO_FILE);
	strcpy(appInterfacePtr->appVersion, "?.?.?");
	strcpy(appInterfacePtr->compiledDSAMVersion, "?.?.?");
	strcpy(appInterfacePtr->title, "No title");
	for (i = 0; i < APP_MAX_AUTHORS; i++)
		appInterfacePtr->authors[i][0] = '\0';
	strcpy(appInterfacePtr->simulationFile, NO_FILE);
	strcpy(appInterfacePtr->diagMode, DEFAULT_FILE_NAME);
	strcpy(appInterfacePtr->installDir, ".");
	appInterfacePtr->argv = NULL;
	appInterfacePtr->argc = 0;
	appInterfacePtr->initialCommand = 0;
	appInterfacePtr->numThreads = -1;
	appInterfacePtr->segmentModeSpecifier = GENERAL_BOOLEAN_OFF;
	appInterfacePtr->diagModeSpecifier = GENERAL_DIAGNOSTIC_OFF_MODE;
	appInterfacePtr->maxUserModules = -1;
	appInterfacePtr->numHelpBooks = 0;
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
	appInterfacePtr->simFileType = UTILITY_SIMSCRIPT_UNKNOWN_FILE;
	appInterfacePtr->canFreePtrFlag = TRUE;
	appInterfacePtr->OnExit = NULL;
	appInterfacePtr->FreeAppProcessVars = NULL;
	appInterfacePtr->Init = NULL;
	appInterfacePtr->OnExecute = NULL;
	appInterfacePtr->PrintExtMainAppUsage = NULL;
	appInterfacePtr->PrintUsage = NULL;
	appInterfacePtr->ProcessOptions = NULL;
	appInterfacePtr->RegisterUserModules = NULL;
	appInterfacePtr->PostInitFunc = NULL;
	appInterfacePtr->SetUniParList = NULL;
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
	SetPar_UniParMgr(&pars[APP_INT_NUMTHREADS], "NUM_THREADS",
	  "No. of processing threads for simulation (-ve defaults to no. CPU's).",
	  UNIPAR_INT,
	  &appInterfacePtr->numThreads, NULL,
	  (void * (*)) SetNumThreads_AppInterface);
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

/****************************** SetSimulationFileFlag *************************/

/*
 * This routine sets the simulationFileFlag which signals whether a simulation
 * file needs to be loaded.
 */

void
SetSimulationFileFlag_AppInterface(BOOLN theSimulationFileFlag)
{
	appInterfacePtr->simulationFileFlag = theSimulationFileFlag;

}

/****************************** SetSimulationFile *****************************/

/*
 * This functions sets the simulation file name.
 * It returns false if it fails in any way.
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
	SetSegmentedMode(appInterfacePtr->segmentModeSpecifier);
	return(TRUE);

}

/****************************** SetNumThreads *********************************/

/*
 * This functions sets the application's number of processing threads to use for
 * the simulation.
 * It returns false if it fails in any way.
 */

BOOLN
SetNumThreads_AppInterface(int numThreads)
{
	static const char	*funcName = "SetNumThreads_AppInterface";

	if (!appInterfacePtr) {
		NotifyError("%s: Application interface not initialised.", funcName);
		return(FALSE);
	}
	appInterfacePtr->numThreads = numThreads;
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

/****************************** SetAppSetUniParList ***************************/

/*
 * This functions sets the application's SetUniParList routine which is used
 * to set the applications parameters.
 */

BOOLN
SetAppSetUniParList_AppInterface(BOOLN (* SetUniParList)(UniParListPtr
  *parList))
{
	static const char	*funcName = "SetAppSetUniParList_AppInterface";

	if (!appInterfacePtr) {
		NotifyError("%s: Application interface not initialised.", funcName);
		return(FALSE);
	}
	appInterfacePtr->SetUniParList = SetUniParList;
	return(TRUE);

}

/****************************** SetAppFreeAppProcessVars **********************/

/*
 * This functions sets the application's FreeAppProcessVars routine which is
 * used free allocated memory for the applications process.
 */

BOOLN
SetAppFreeProcessVars_AppInterface(BOOLN (* FreeAppProcessVars)(void))
{
	static const char	*funcName = "SetAppFreeProcessVars_AppInterface";

	if (!appInterfacePtr) {
		NotifyError("%s: Application interface not initialised.", funcName);
		return(FALSE);
	}
	appInterfacePtr->FreeAppProcessVars = FreeAppProcessVars;
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

/****************************** SetAppPostInitFunc ****************************/

/*
 * This functions sets the application's final pars routine.
 * These are parameters which set by the application interace, after the
 * simulation is initialised.
 * It returns false if it fails in any way.
 */

BOOLN
SetAppPostInitFunc_AppInterface(BOOLN (* PostInitFunc)(void))
{
	static const char	*funcName = "SetAppPostInitFunc_AppInterface";

	if (!appInterfacePtr) {
		NotifyError("%s: Application interface not initialised.", funcName);
		return(FALSE);
	}
	appInterfacePtr->PostInitFunc = PostInitFunc;
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

/****************************** SetOnExecute **********************************/

/*
 * This functions sets the application's OnExecute routine.
 * This routine is used by the application to hold it's execution routine.
 * It returns FALSE if it fails in any way.
 */

BOOLN
SetOnExecute_AppInterface(BOOLN (* OnExecute)(void))
{
	static const char	*funcName = "SetOnExecute_AppInterface";

	if (!appInterfacePtr) {
		NotifyError("%s: Application interface not initialised.", funcName);
		return(FALSE);
	}
	appInterfacePtr->OnExecute = OnExecute;
	return(TRUE);

}

/****************************** SetOnExit *************************************/

/*
 * This functions sets the application's OnExit routine.
 * This routine is used by the application to hold it's exit routine.
 * It returns FALSE if it fails in any way.
 */

BOOLN
SetOnExit_AppInterface(void (* OnExit)(void))
{
	static const char	*funcName = "SetOnExit_AppInterface";

	if (!appInterfacePtr) {
		NotifyError("%s: Application interface not initialised.", funcName);
		return(FALSE);
	}
	appInterfacePtr->OnExit = OnExit;
	return(TRUE);

}

/****************************** PrintInitialDiagnostics ***********************/

/*
 * This routine prints the initial diagnostics for the program.
 */

void
PrintPars_AppInterface(void)
{
	static const char *funcName = "PrintPars_AppInterface";
	if (!appInterfacePtr) {
		NotifyError("%s: Application interface not initialised.", funcName);
		return;
	}
	DPrint("Simulation script application interface settings:\n");
	DPrint("Diagnostics specifier is set to '%s'.\n",
	  appInterfacePtr->diagModeList[appInterfacePtr->diagModeSpecifier].name);
	DPrint("This simulation is run from the file '%s'.\n",
	  GetFilePath_AppInterface(appInterfacePtr->simulationFile));
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
	DatumPtr	simulation = NULL;
	AppInterfacePtr	p = appInterfacePtr;

	if (!p) {
		NotifyError("%s: Application interface not initialised.", funcName);
		return(FALSE);
	}
	if (!p->useParComsFlag)
		return(TRUE);

	if (p->audModel && (simulation = GetSimulation_ModuleMgr(p->audModel)) ==
	  NULL) {
		NotifyError("%s: No simulation has been initialised.", funcName);
		return(FALSE);
	}
	if (p->argc && ((p->argc - p->initialCommand) % 2 != 0)) {
		NotifyError("%s: parameter values must be in <name> <value> pairs.",
		  funcName);
		return(FALSE);
	}
	for (i = p->initialCommand; i < p->argc; i += 2) {
		if (SetProgramParValue_AppInterface(p->argv[i], p->argv[i + 1],
		  FALSE) || (simulation && SetPar_ModuleMgr(p->audModel, p->argv[i],
		    p->argv[i + 1])) || !simulation)
			continue;
		NotifyError("%s: Could not set '%s' parameter to '%s'.", funcName,
		  p->argv[i], p->argv[i + 1]);
		return(FALSE);
	}
	p->useParComsFlag = (!p->audModel);
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
	  &optInd, &optSub, &argument, "@#:d:hl:P:s:Sp:"))) {
		optionFound = TRUE;
		switch (c) {
		case '@':
		case '#':
			break;
		case 'd':
			ok = SetProgramParValue_AppInterface(appInterfacePtr->parList->pars[
			  APP_INT_DIAGNOSTICMODE].abbr, argument, FALSE);
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
			MarkIgnore_Options(appInterfacePtr->argc, appInterfacePtr->argv,
			  "-s", OPTIONS_WITH_ARG);
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

/****************************** GetFilePath ***********************************/

/*
 * This routine returns the given filePath, depending upon whether GUI mode is
 * used.
 * This was introduced to conform to the use of streams in the wxWindows
 * document/view code.
 */

char *
GetFilePath_AppInterface(char *filePath)
{
	static char guiFilePath[MAX_FILE_PATH];

	if (!GetDSAMPtr_Common()->usingExtFlag)
		return(filePath);
	snprintf(guiFilePath, MAX_FILE_PATH, "%s/%s", appInterfacePtr->parsFilePath,
	  appInterfacePtr->simulationFile);
	return(guiFilePath);
	
}

/************************ ParseParSpecifiers **********************************/

/*
 * This routine parses the parameter specifiers.
 * It expects the strings spaces passed as arguments to have sufficient space.
 */

void
ParseParSpecifiers_AppInterface(char *parName, char *appName, char *subProcess)
{
	char	*p;

	*appName = *subProcess = '\0';
	if ((p = strchr(parName, UNIPAR_NAME_SEPARATOR)) == NULL)
		return;
	*p = '\0';
	snprintf(appName, MAXLINE, "%s", p + 1);
	if ((p = strchr(appName, UNIPAR_NAME_SEPARATOR)) == NULL)
		return;
	*p = '\0';
	snprintf(subProcess, MAXLINE, "%s", p + 1);

}

/************************ SetProgramParValue **********************************/

/*
 * This function sets a program parameter.
 * A copy of the 'parList' must be taken, as the argument 'parList' it can be
 * changed by the 'FindUniPar_UniParMgr' routine.
 */

BOOLN
SetProgramParValue_AppInterface(char *parName, char *parValue, BOOLN readSPF)
{
	static const char *funcName = "SetProgramParValue_AppInterface";
	BOOLN	ok = TRUE, creatorApp = TRUE;
	char	parNameCopy[MAXLINE], appName[MAXLINE], subProcess[MAXLINE];
	UniParPtr	par;
	UniParListPtr	parList;

	snprintf(parNameCopy, MAXLINE, "%s", parName);
	ParseParSpecifiers_AppInterface(parNameCopy, appName, subProcess);
	if (*appName)  {
		creatorApp = (StrNCmpNoCase_Utility_String(appInterfacePtr->appName,
		  appName) == 0);
		if (!creatorApp && !readSPF)
			return(FALSE);
	}
	parList = appInterfacePtr->parList;
	if (*subProcess) {
		if ((par = FindUniPar_UniParMgr(&parList, subProcess,
		  UNIPAR_SEARCH_ABBR)) == NULL) {
			NotifyError("%s: Unknown sub-process '%s' for application.",
			  funcName, subProcess);
			return(FALSE);
		} else
			parList = *(par->valuePtr.parList.list);
	}
	if ((par = FindUniPar_UniParMgr(&parList, parNameCopy,
	  UNIPAR_SEARCH_ABBR)) == NULL) {
		if (creatorApp)
			ok = FALSE;
	} else if (((parList != appInterfacePtr->parList) || (par->index !=
	  APP_INT_SIMULATIONFILE) || (appInterfacePtr->audModel &&
	 (GetSimFileType_ModuleMgr(appInterfacePtr->audModel) ==
	 UTILITY_SIMSCRIPT_SPF_FILE))) && !SetParValue_UniParMgr(&parList,
	 par->index, parValue))
		ok = FALSE;
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

/****************************** GetSimPtr *************************************/

/*
 * This function returns a pointer to the application interface's audModel
 * simulation pointer.
 */

DatumPtr *
GetSimPtr_AppInterface(void)
{
	static const char *funcName = "GetSimPtr_AppInterface";
	DatumPtr	*simPtr;

	if (!appInterfacePtr) {
		NotifyError("%s: Application interface not initialised.", funcName);
		return(NULL);
	}
	if (!appInterfacePtr->audModel) {
		NotifyError("%s: Simulation EarObject nod initialised.", funcName);
		return(NULL);
	}
	if ((simPtr = GetSimPtr_ModuleMgr(appInterfacePtr->audModel)) == NULL) {
		NotifyError("%s: Simulation not initialised.", funcName);
		return(NULL);
	}
	return(simPtr);

}

/****************************** ReadPars **************************************/

/*
 * This program reads a specified number of parameters using the specified file
 * pointer.
 * It is meant to be read from within the Main Program ReadPars routine.
 * It returns FALSE if it fails in any way.n */

BOOLN
ReadPars_AppInterface(char *parFileName)
{
	static const char *funcName = "ReadPars_AppInterface";
	BOOLN	ok = TRUE;
	char	*filePath;
	char	parName[MAXLINE], appName[MAXLINE], subProcess[MAXLINE];
	char	parValue[MAX_FILE_PATH];
	FILE	*fp;
	UniParPtr	par;
	UniParListPtr	tempParList;

	filePath = GetParsFileFPath_Common(parFileName);
	if ((fp = fopen(filePath, "r")) == NULL) {
		NotifyError("%s: Cannot open data file '%s'.\n", funcName, parFileName);
		return(FALSE);
	}
	Init_ParFile();
	SetEmptyLineMessage_ParFile(FALSE);
	while (GetPars_ParFile(fp, "%s %s", parName, parValue)) {
		ParseParSpecifiers_AppInterface(parName, appName, subProcess);
		tempParList = appInterfacePtr->parList;
		if (*subProcess) {
			if ((par = FindUniPar_UniParMgr(&tempParList, subProcess,
			  UNIPAR_SEARCH_ABBR)) == NULL) {
				NotifyError("%s: Unknown sub-process '%s' for application.",
				  funcName, subProcess);
				ok = FALSE;
				break;
			} else
				tempParList = *(par->valuePtr.parList.list);
		}
		if ((par = FindUniPar_UniParMgr(&tempParList, parName,
		  UNIPAR_SEARCH_ABBR)) == NULL) {
			NotifyError("%s: Unknown parameter '%s' for application.", funcName,
			  parName);
			ok = FALSE;
		} else {
			if (!SetParValue_UniParMgr(&tempParList, par->index, parValue))
				ok = FALSE;
		}
	
	}
	SetEmptyLineMessage_ParFile(TRUE);
	fclose(fp);
	Free_ParFile();
	if (!ok) {
		NotifyError("%s: Could not set parameters for application.", funcName);
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
	static const char	*funcName = "ReadProgParFile_AppInterface";
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
		  GetFilePath_AppInterface(appInterfacePtr->simulationFile));
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
		ok = SetProgramParValue_AppInterface(parName, parValue, TRUE);
	SetEmptyLineMessage_ParFile(TRUE);
	fclose(fp);
	Free_ParFile();
	readProgParFileFlag = FALSE;
	if (!ok && (strcmp(parName, SIMSCRIPT_SIMPARFILE_SDI_DIVIDER) != 0)) {
		NotifyError("%s: Invalid parameters in file '%s', program parameter "
		  "section (%s).", funcName, GetFilePath_AppInterface(appInterfacePtr->
		  simulationFile), parName);
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

	if (!appInterfacePtr) {
		NotifyError("%s: Application interface not initialised.", funcName);
		return(FALSE);
	}
	if (!appInterfacePtr->audModel) {
		FreeSim_AppInterface();
		if ((appInterfacePtr->audModel = Init_EarObject("Util_SimScript")) ==
		  NULL) {
			NotifyError("%s: Could not initialise process.", funcName);
			FreeSim_AppInterface();
			appInterfacePtr->checkMainInit = TRUE;
			return(FALSE);
		}
		
	}
	return(TRUE);

}

/****************************** FreeSim ***************************************/

/*
 * This routine resets the simulation.
 */

void
FreeSim_AppInterface(void)
{
	Free_EarObject(&appInterfacePtr->audModel);
	ResetStepCount_Utility_Datum();
	appInterfacePtr->updateProcessVariablesFlag = TRUE;

}

/****************************** SetSimFileType ********************************/

/*
 * This function sets the module's simParFileFlag field.
 */

BOOLN
SetSimFileType_AppInterface(int simFileType)
{
	static const char	*funcName = "SetSimFileType_AppInterface";

	if (!appInterfacePtr) {
		NotifyError("%s: Application interface not initialised.", funcName);
		return(FALSE);
	}
	appInterfacePtr->simFileType = simFileType;
	return(TRUE);

}

/****************************** SetParsFilePath *******************************/

/*
 * This function sets the module's parsFilePath field.
 */

BOOLN
SetParsFilePath_AppInterface(char * parsFilePath)
{
	static const char	*funcName = "SetParsFilePath_AppInterface";

	if (appInterfacePtr == NULL) {
		NotifyError("%s: Module not initialised.", funcName);
		return(FALSE);
	}
	snprintf(appInterfacePtr->parsFilePath, MAX_FILE_PATH, "%s", parsFilePath);
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

	if (!appInterfacePtr) {
		NotifyError("%s: Application interface not initialised.", funcName);
		return(FALSE);
	}
	if (!appInterfacePtr->audModel) {
		FreeSim_AppInterface();
		if ((appInterfacePtr->audModel = Init_EarObject("Util_SimScript")) ==
		  NULL) {
			NotifyError("%s: Could not initialise process.", funcName);
			ok = FALSE;
		}
		if (GetDSAMPtr_Common()->usingGUIFlag) {
			SET_PARS_POINTER(appInterfacePtr->audModel);
			SetSimFileType_Utility_SimScript(appInterfacePtr->simFileType);
			SetParsFilePath_Utility_SimScript(appInterfacePtr->parsFilePath);
			SetSimFileName_Utility_SimScript(appInterfacePtr->simulationFile);
		}
	}
	if (ok && !ReadPars_ModuleMgr(appInterfacePtr->audModel, appInterfacePtr->
	  simulationFile))
		ok = FALSE;
	if (!ok) {
		FreeSim_AppInterface();
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
	snprintf(appInterfacePtr->title, MAX_FILE_PATH, "%s", title);
	return(TRUE);

} 

/****************************** AddAppHelpBook ********************************/

/*
 * This function adds a help book to the application's help book list.
 * It returns FALSE if it fails in any way
 */

BOOLN
AddAppHelpBook_AppInterface(const char *bookName)
{
	static const char *funcName = "AddAppHelpBook_AppInterface";

	if (!appInterfacePtr) {
		NotifyError("%s: Application interface not initialised.", funcName);
		return(FALSE);
	}
	if (*bookName == '\0') {
		NotifyError("%s: Book name not set.\n", funcName);
		return(FALSE);
	}
	if ((appInterfacePtr->numHelpBooks - 1) == APP_MAX_HELP_BOOKS) {
		NotifyError("%s: Maximum number of help books added (%d).", funcName,
		  APP_MAX_HELP_BOOKS);
		return(FALSE);
	}
	strcpy(appInterfacePtr->appHelpBooks[appInterfacePtr->numHelpBooks++],
	  bookName);
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
	SetUsingGUIStatus(FALSE);
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
	snprintf(suffix, MAXLINE, ".%s%s", appInterfacePtr->appName,
	  UNIPAR_TOP_PARENT_LABEL);
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
	SetDiagMode(COMMON_CONSOLE_DIAG_MODE);
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
			if (appInterfacePtr->RegisterUserModules) {
				if (!InitUserModuleList_ModuleReg(appInterfacePtr->
				  maxUserModules)) {
					NotifyError("%s: Could not initialise user module list.",
					  funcName);
					return(FALSE);
				}
				if (!(* appInterfacePtr->RegisterUserModules)()) {
					NotifyError("%s: Failed to register user modules.",
					  funcName);
					return(FALSE);
				}
			}
			if (appInterfacePtr->SetUniParList)
				(* appInterfacePtr->SetUniParList)(&appInterfacePtr->
				  appParList);
			GetDSAMPtr_Common()->appInitialisedFlag = TRUE;
			if (GetDSAMPtr_Common()->usingExtFlag)
				return(FALSE);
		}
	}

	SetArgcAndArgV_AppInterface(theArgc, theArgv);
	if (appInterfacePtr->checkMainInit) {
		ProcessOptions_AppInterface();
		if (appInterfacePtr->printUsageFlag) {
			PrintUsage_AppInterface();
			if (appInterfacePtr->PrintUsage)
				(* appInterfacePtr->PrintUsage)();
			exit(0);
		}
		DPrint("Starting %s Application version %s [DSAM Version: %s "
		  "(dynamic),\n%s (compiled)]...\n", appInterfacePtr->appName,
		  appInterfacePtr->appVersion, GetDSAMPtr_Common()->version,
		  appInterfacePtr->compiledDSAMVersion);

	}
	if (appInterfacePtr->updateProcessVariablesFlag) {
		if (appInterfacePtr->readAppParFileFlag) {
			if (!ReadPars_AppInterface(appInterfacePtr->appParFile)) {
				NotifyError("%s: Failed to set application parameters.",
				  funcName);
				return(FALSE);
			}
			appInterfacePtr->readAppParFileFlag = FALSE;
		}
		if (appInterfacePtr->canLoadSimulationFlag &&
		  appInterfacePtr->simulationFileFlag) {
			if (!InitSimulation_AppInterface()) {
				NotifyError("%s: Could not Initialise simulation.", funcName);
				return(FALSE);
			}
			SetParsFilePath_Common(GetParsFilePath_ModuleMgr(appInterfacePtr->
			  audModel));
			if ((GetSimFileType_ModuleMgr(appInterfacePtr->audModel) ==
			  UTILITY_SIMSCRIPT_SPF_FILE) && !ReadProgParFile_AppInterface()) {
				NotifyError("%s: Could not read the program settings in\nfile "
				  "'%s'.", funcName, GetFilePath_AppInterface(appInterfacePtr->
				  simulationFile));
				return(FALSE);
			}
			if (!ProcessParComs_AppInterface())
				return(FALSE);
		}
		if (appInterfacePtr->PostInitFunc && !(* appInterfacePtr->
		  PostInitFunc)()) {
			NotifyError("%s: Failed to run post initialisation function.",
			  funcName);
			return(FALSE);
		}

		if (appInterfacePtr->listParsAndExit)
			ListParsAndExit_AppInterface();
		if (appInterfacePtr->listCFListAndExit)
			ListCFListAndExit_AppInterface();

		appInterfacePtr->updateProcessVariablesFlag = FALSE;

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
	/*static const char *funcName = "SetCanFreePtrFlag_AppInterface";*/

	if (!appInterfacePtr) {
		/*NotifyError("%s: Application interface not initialised.",funcName);*/
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

/****************************** GetUniParPtr **********************************/

/*
 * This function returns a universal parameter for the main application
 * simulation.
 * It returns NULL if it fails in any way.
 */

UniParPtr
GetUniParPtr_AppInterface(char *parName)
{
	static const char *funcName = "GetUniParPtr_AppInterface";
	UniParPtr	par;

	if (!appInterfacePtr) {
		NotifyError("%s: Application interface not initialised.", funcName);
		return(NULL);
	}
	if ((par = GetUniParPtr_ModuleMgr(appInterfacePtr->audModel, parName)) ==
	  NULL) {
		NotifyError("%s: Could not find simulation parameter '%s'.", funcName,
		  parName);
		return(NULL);
	}
	return(par);

}

/****************************** PrintSimPars **********************************/

/*
 * This function prints the parameters for the main auditory model simulation.
 * It returns NULL if it fails in any way.
 */

BOOLN
PrintSimPars_AppInterface(void)
{
	static const char *funcName = "PrintSimPars_AppInterface";

	if (!appInterfacePtr) {
		NotifyError("%s: Application interface not initialised.", funcName);
		return(FALSE);
	}
	return(PrintPars_ModuleMgr(appInterfacePtr->audModel));

}

/****************************** ResetSim **************************************/

/*
 * This routine resets the processes in the main auditory model simulation.
 */

BOOLN
ResetSim_AppInterface(void)
{
	static const char *funcName = "ResetSim_AppInterface";

	if (!appInterfacePtr) {
		NotifyError("%s: Application interface not initialised.", funcName);
		return(FALSE);
	}
	ResetProcess_EarObject(appInterfacePtr->audModel);
	return(TRUE);

}

/****************************** RunSim ****************************************/

/*
 * This routine runs the main auditory model simulation.
 * It returns FALSE if it fails in any way.
 */

BOOLN
RunSim_AppInterface(void)
{
	static const char *funcName = "RunSim_AppInterface";

	if (!appInterfacePtr) {
		NotifyError("%s: Application interface not initialised.", funcName);
		return(FALSE);
	}
	if (!RunProcess_ModuleMgr(appInterfacePtr->audModel)) {
		NotifyError("%s: Could not run main auditory model simulation '%s'.",
		  funcName, appInterfacePtr->simulationFile);
		return(FALSE);
	}
	return(TRUE);

}

/****************************** GetSimProcess *********************************/

/*
 * This routine returns the main auditory model simulation process EarObject.
 */

EarObjectPtr
GetSimProcess_AppInterface(void)
{
	static const char *funcName = "GetSimProcess_AppInterface";

	if (!appInterfacePtr) {
		NotifyError("%s: Application interface not initialised.", funcName);
		return(NULL);
	}
	if (!appInterfacePtr->audModel) {
		NotifyError("%s: Simulation EarObject not initialised.", funcName);
		return(NULL);
	}
	return(appInterfacePtr->audModel);

}

/****************************** SetSimPar *************************************/

/*
 * This function sets a parameter in the main auditory model simulation
 * process EarObject.
 * It returns FALSE if it fails in any way.
 */

BOOLN
SetSimPar_AppInterface(char *parName, char *value)
{
	static const char *funcName = "SetSimPar_AppInterface";

	if (!appInterfacePtr) {
		NotifyError("%s: Application interface not initialised.", funcName);
		return(FALSE);
	}
	return(SetPar_ModuleMgr(appInterfacePtr->audModel, parName, value));

}

/****************************** SetRealSimPar *********************************/

/*
 * This function sets a real parameter in the main auditory model simulation
 * process EarObject.
 * It returns FALSE if it fails in any way.
 */

BOOLN
SetRealSimPar_AppInterface(char *parName, double value)
{
	static const char *funcName = "SetRealSimPar_AppInterface";

	if (!appInterfacePtr) {
		NotifyError("%s: Application interface not initialised.", funcName);
		return(FALSE);
	}
	return(SetRealPar_ModuleMgr(appInterfacePtr->audModel, parName, value));

}

/****************************** SetRealArraySimPar ****************************/

/*
 * This function sets a real array parameter element in the main auditory model
 * simulation process EarObject.
 * It returns FALSE if it fails in any way.
 */

BOOLN
SetRealArraySimPar_AppInterface(char *parName, int index, double value)
{
	static const char *funcName = "SetRealArraySimPar_AppInterface";

	if (!appInterfacePtr) {
		NotifyError("%s: Application interface not initialised.", funcName);
		return(FALSE);
	}
	return(SetRealArrayPar_ModuleMgr(appInterfacePtr->audModel, parName, index,
	  value));

}

/****************************** OnExit ***************************************/

/*
 * This routine calls the application's exit routine, if on exists.
 */

void
OnExit_AppInterface(void)
{
	if (appInterfacePtr->OnExit)
		(* appInterfacePtr->OnExit)();
}

/****************************** OnExecute *************************************/

/*
 * This routine calls the application's OnExecute routine if one exists.
 * This routine is used when the application interface needs to do various
 * other things, i.e. under the GUI.
 */

BOOLN
OnExecute_AppInterface(void)
{
	BOOLN	ok;

	if (!appInterfacePtr->OnExecute)
		return(FALSE);
	appInterfacePtr->simulationFinishedFlag = FALSE;
	ok = (* appInterfacePtr->OnExecute)();
	return(ok);

}
