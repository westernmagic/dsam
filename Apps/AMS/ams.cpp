/**********************
 *
 * File:		ams.c
 * Purpose:		This program generates a simulation response using the
 *				simulation script utility.
 * Comments:	22-04-98 LPO:  Amended to allow users to override the stimulus
 *				parameters.
 *				To allow the parameter file to be specified as an option,
 *				flags have to be used to ensure that general parameters set by
 *				options are not overwritten.
 *				28-09-1998 LPO: The simulation is now declared globally, and it
 *				is only initialised if the simulation script file was
 *				modified since the last time the simulation was initialised.
 *				In this mode the EarObjects are only deallocated at the end of
 *				of the program (by default), or if re-initialisation occurs.
 *				05-10-98 LPO: Removed override parameters, as this is no longer
 *				necessary because of the universal parameter option handling.
 *				07-10-98 LPO: Introduced '-l' option for listings.
 *				09-10-1998 LPO: Introduced the parameter file mode.  If it is
 *				'on' then parameters for the simulation will be read from a
 *				file: A simulation parameter file (SPF) - resource file.
 *				13-10-98 LPO: Added the 'CFLIST' list option.
 *				07-01-99 LPO: Added support for ignoring options with arguments.
 *				26-01-99 LPO: Added overridefunction for printing the parameters
 *				from the simulation manager.
 *				I also removed the dependence on the main parameter file.  The
 *				main parameters will now be set to defaults.
 *				09-02-99 LPO: The main parameters are now only read when the
 *				program is first started.
 *				20-05-99 LPO: I have introduced the "DialogList::
 *				SetUpdateParent" Routine which will allow the changing of the
 *				simulation script and the simulation parameter file to signal
 *				a re-initialisation of the simulation and or parameters.
 *				19-07-99 LPO: The CygWin port of wxwin did not have the program
 *				name as the first argv[] string.  This was corrected in the
 *				version that I am using.
 *				27-07-99 LPO: The "SetSimScriptFile" routine now free's
 *				the "audModel" simulation script, to ensure that the new
 *				script is initialised, and the old one is not used.
 *				18-08-99 LPO: Introduced the file-locking option.
 * Author:		L. P. O'Mard
 * Created:		19 Mar 1995
 * Updated:		18 Aug 1999
 * Copyright:	(c) 1999, University of Essex
 *
 *********************/

#include <stdlib.h> 
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <sys/stat.h>

#include "DSAM.h"
#include "ams.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

char	progName[MAXLINE], parFileName[MAX_FILE_PATH];
char	simScriptFile[MAX_FILE_PATH] = NO_FILE;
char	segmentMode[MAXLINE] = "on";
char	fileLockingMode[MAXLINE] = "off";
char	simParFileMode[MAX_FILE_PATH] = "off";
char	diagnosticMode[MAX_FILE_PATH] = "screen";
char	diagnosticFile[MAX_FILE_PATH] = NO_FILE;

BOOLN	readMainParsFlag = FALSE, simScriptFileFlag, numberOfRunsFlag;
BOOLN	listCFListAndExit = FALSE, listParsAndExit = FALSE;
BOOLN	simParFileModeFlag = FALSE;
BOOLN	checkMainInit = TRUE;
BOOLN	checkInitialisation = TRUE;
BOOLN	gUIModeFlag = FALSE, parComsUsedFlag = FALSE;
char 	**argv, *simFile;
int		argc, numberOfRuns = 1, initialCommand, simParFileModeSpecifier;
int		segmentModeSpecifier, fileLockingModeSpecifier, diagnosticModeSpecifier;
time_t	simLastModified = 0;
EarObjectPtr	audModel = NULL;
UniParListPtr	programParList = NULL;
NameSpecifier	*diagnosticModeList = NULL;

NameSpecifier	listingModeList[] = {

	{ "PARAMETERS", AMS_PARAMETERS_LIST_MODE },
	{ "CFINFO", 	AMS_CFLIST_LIST_MODE },
	{ "",			AMS_LIST_NULL }

};

#ifdef GRAPHICS_SUPPORT

// Tell SimMgr that we are passing run information.
MyRunMgr	myRunMgr;

/******************************************************************************/
/****************************** Class Methods *********************************/
/******************************************************************************/

/******************************************************************************/
/****************************** MyRunMgr Methods *****************************/
/******************************************************************************/

/****************************** OnInit ****************************************/

bool
MyRunMgr::OnInit(void)
{
	static const char *funcName = "MyRunMgr::OnInit";

	icon = new wxIcon(wxICON(ams));
	appName = PROGRAM_NAME;
	dataInstallDir = AMS_DATA_INSTALL_DIR;
	gUIModeFlag	= TRUE;
	if (!Init()) {
		NotifyError("%s: Could not initialise program.", funcName);
		return(FALSE);
	}
	return(TRUE);

}

/****************************** OnExit ****************************************/

bool
MyRunMgr::OnExit(void)
{
	FreeAll_EarObject();
	return(TRUE);

}

/****************************** ResetCommandArgs ******************************/

bool
MyRunMgr::ResetCommandArgs(void)
{
	argc = myArgc;
	argv = myArgv;
	initialCommand = ProcessOptions(argc, argv);
	parComsUsedFlag = FALSE;
	return(TRUE);

}

/****************************** CheckInitialisation ***************************/

/*
 * This routine sets the simulation member for this class.  It runs the
 * "Init" routine, with the disagnostics turned off, to ensure that the
 * simulation has been initialised.
 * I use the 'ok' boolean variable so that the 'parList' and 'simProcess'
 * members are set to respective variables regardless of whether the program
 * initialises correctly.
 */

bool
MyRunMgr::CheckProgInitialisation(void)
{
	return(InitProcess());

}

/****************************** GetSimParFile *********************************/

/*
 * This routine returns a pointer to the simulation parameter file.
 */

char *
MyRunMgr::GetSimParFile(void)
{
	return(programParList->pars[AMS_SIMPARFILE].valuePtr.s);

}

/****************************** GetParList ************************************/

/*
 * This routine returns a pointer to the program parameter list.
 */

UniParListPtr
MyRunMgr::GetParList(void)
{
	return(programParList);

}

/****************************** GetSimProcess *********************************/

/*
 * This routine returns a pointer to the simulation process.
 */

EarObjectPtr
MyRunMgr::GetSimProcess(void)
{
	return(audModel);

}

/****************************** ListSimParameters *****************************/

/*
 * This routine prints the simulation parameters.
 */

void
MyRunMgr::ListSimParameters(void)
{
	ListParameters();
	
}

/****************************** SetTitle **************************************/

/*
 * This routine sets the title for the simulation manager window.
 * The NULL frame test is necessary because the first time this is called the
 * frame will not have been set.
 */

void
MyRunMgr::SetTitle(char *theTitle)
{
	MyFrame	*myFrame;

	if (!title)
		title = new wxString;
	title->sprintf("%s: %s", PROGRAM_NAME, theTitle);
	if ((myFrame = wxGetApp().GetFrame()) != NULL)
		myFrame->SetTitle(*title);

}

/****************************** StatusChanged *********************************/

/*
 * This routine checks if any status change occurs.  This will signal the
 * re-initialisation of the program.
 */

bool
MyRunMgr::StatusChanged(void)
{
	if (SimScriptFileChanged(GetFileStatusPtr(simFile), FALSE))
		return(TRUE);
	if (!audModel)
		return(TRUE);
	return(FALSE);
	
}

#endif /* GRAPHICS_SUPPORT */

/******************************************************************************/
/****************************** Functions and subroutines *********************/
/******************************************************************************/

/****************************** ReadMainParsFromFile **************************/

/*
 * This program reads a specified number of parameters from a file.
 * It expects there to be one parameter per line.
 */
 
BOOLN
ReadMainParsFromFile(char *fileName)
{
	static const char *funcName = "ReadMainParsFromFile";
	BOOLN	ok = TRUE;
	FILE	*fp;
	
	if ((fp = fopen(fileName, "r")) == NULL) {
		return(FALSE);
	}
	Init_ParFile();
	GetPars_ParFile(fp, "%s", diagnosticMode);
	if (!simScriptFileFlag)
		GetPars_ParFile(fp, "%s", simScriptFile);
	else
		GetPars_ParFile(fp, "");
	if (!simParFileModeFlag)
		GetPars_ParFile(fp, "%s", simParFileMode);
	else
		GetPars_ParFile(fp, "");
	GetPars_ParFile(fp, "%s", segmentMode);
	GetPars_ParFile(fp, "%s", fileLockingMode);
	if (!numberOfRunsFlag)
		GetPars_ParFile(fp, "%d", &numberOfRuns);
	else
		GetPars_ParFile(fp, "");
	fclose(fp);
	Free_ParFile();
	if (!ok)
		NotifyError("%s: Failed to read parameters.", funcName);
	return(ok);
	
}

/****************************** SetUniParList *********************************/

/*
 * This function initialises and sets the programs universal parameter
 * list. This list provides universal access to the program's
 * parameters.  It needs to be called from an initialise routine.
 */

BOOLN
SetUniParList(void)
{
	static const char *funcName = "SetUniParList";
	UniParPtr	pars;

	if ((programParList = InitList_UniParMgr(UNIPAR_SET_GENERAL, AMS_NUM_PARS,
	  NULL)) == NULL) {
		NotifyError("%s: Could not initialise parList.", funcName);
		return(FALSE);
	}
	pars = programParList->pars;
	SetPar_UniParMgr(&pars[AMS_DIAGNOSTICMODE], "DIAG_MODE",
	  "Diagnostics mode specifier ('off', 'screen' or <filename>).",
	  UNIPAR_NAME_SPEC_WITH_FILE,
	  &diagnosticModeSpecifier, diagnosticModeList,
	  (void * (*)) SetDiagnosticMode);
	SetPar_UniParMgr(&pars[AMS_SIMSCRIPTFILE], "SIM_SCRIPT_FILE",
	  "Simulation script file.",
	  UNIPAR_FILE_NAME,
	  &simScriptFile, (char *) "*.sim",
	  (void * (*)) SetSimScriptFile);
	SetPar_UniParMgr(&pars[AMS_SIMPARFILE], "SIM_PAR_FILE",
	  "Simulation parameter file mode ('off' or <file name>).",
	  UNIPAR_FILE_NAME,
	  &simParFileMode, (char *) "*.spf",
	  (void * (*)) SetSimParFileMode);
	SetPar_UniParMgr(&pars[AMS_SEGMENTMODE], "SEGMENT_MODE",
	  "Segmented or frame-base processing mode ('on' or 'off').",
	  UNIPAR_BOOL,
	  &segmentModeSpecifier, NULL,
	  (void * (*)) SetSegmentMode);
	SetPar_UniParMgr(&pars[AMS_FILELOCKINGMODE], "FILELOCKING_MODE",
	  "File locking mode ('on' or 'off').",
	  UNIPAR_BOOL,
	  &fileLockingModeSpecifier, NULL,
	  (void * (*)) SetFileLockingMode);
	SetPar_UniParMgr(&pars[AMS_NUMBEROFRUNS], "NUM_RUNS",
	  "Number of repeat runs, or segments/frames.",
	  UNIPAR_INT,
	  &numberOfRuns, NULL,
	  (void * (*)) SetNumberOfRuns);
	return(TRUE);

}

/****************************** SetDiagnosticMode *****************************/

/*
 * This functions sets the diagnostic mode.
 * It returns false if it fails in any way.
 */

BOOLN
SetDiagnosticMode(char *theDiagnosticMode)
{
	/* static const char	*funcName = PROGRAM_NAME": SetDiagnosticMode"; */

	diagnosticModeSpecifier = IdentifyDiag_NSpecLists(theDiagnosticMode,
	  diagnosticModeList);
	SetParsFile_Common(diagnosticModeList[diagnosticModeSpecifier].name,
	  OVERWRITE);
	return(TRUE);

}


/****************************** FlagReinitialise ******************************/

/*
 * This routine is called when the program needs to be re-initialised.
 * At the moment it is used for the GUI support.
 */

void
FlagReinitialise(void)
{
	checkMainInit = TRUE;
	checkInitialisation = TRUE;
#	ifdef GRAPHICS_SUPPORT
	myRunMgr.SetUpdateProgram(TRUE);
#	endif

}

/****************************** SetSimScriptFile ******************************/

/*
 * This functions sets the simulation script file name.
 * It returns false if it fails in any way.
 * It also sets the global "simLastModified" variable to zero.
 */

BOOLN
SetSimScriptFile(char *theSimScriptFile)
{
	static const char	*funcName = PROGRAM_NAME": SetSimParFile";

	if (*theSimScriptFile == '\0') {
		NotifyError("%s: Illegal zero length name.", funcName);
		return(FALSE);
	}
	strcpy(simScriptFile, theSimScriptFile);
	simScriptFileFlag = TRUE;
	if (!checkMainInit)
		SetSimParFileMode("off");
	FlagReinitialise();
	return(TRUE);

}

/****************************** SetSimParFileMode *****************************/

/*
 * This functions sets the simulation parameter file mode.
 * It returns false if it fails in any way.
 */

BOOLN
SetSimParFileMode(char *theSimParFileMode)
{
	static const char	*funcName = PROGRAM_NAME": SetSimParFileMode";

	if ((simParFileModeSpecifier = Identify_NameSpecifier(theSimParFileMode,
	  DiagModeList_NSpecLists(0))) == GENERAL_DIAGNOSTIC_MODE_NULL) {
		if (*theSimParFileMode == '\0') {
			NotifyError("%s: Illegal zero length name.", funcName);
			return(FALSE);
		}
	}
	strcpy(simParFileMode, theSimParFileMode);
	FlagReinitialise();
	return(TRUE);

}

/****************************** SetFileLockingMode ****************************/

/*
 * This functions sets the file Locking processing mode.
 * It returns false if it fails in any way.
 */

BOOLN
SetFileLockingMode(char *theFileLockingMode)
{
	static const char	*funcName = PROGRAM_NAME": SetFileLockingMode";

	if ((fileLockingModeSpecifier = Identify_NameSpecifier(theFileLockingMode,
	  BooleanList_NSpecLists(0))) == GENERAL_BOOLEAN_NULL) {
		NotifyError("%s: Illegal file locking mode (%s): must be "
		  "'on' or 'off'.", funcName, fileLockingMode);
		return(FALSE);
	}
	strcpy(fileLockingMode, theFileLockingMode);
	return(TRUE);

}

/****************************** SetSegmentMode ********************************/

/*
 * This functions sets the segment processing mode.
 * It returns false if it fails in any way.
 */

BOOLN
SetSegmentMode(char *theSegmentMode)
{
	static const char	*funcName = PROGRAM_NAME": SetSegmentMode";

	if ((segmentModeSpecifier = Identify_NameSpecifier(theSegmentMode,
	  BooleanList_NSpecLists(0))) == GENERAL_BOOLEAN_NULL) {
		NotifyError("%s: Illegal segment processing mode (%s): must be "
		  "'on' or 'off'.", funcName, segmentMode);
		return(FALSE);
	}
	strcpy(segmentMode, theSegmentMode);
	return(TRUE);

}

/****************************** SetNumberOfRuns *******************************/

/*
 * This functions sets the number of simulation runs.
 * It returns false if it fails in any way.
 */

BOOLN
SetNumberOfRuns(int theNumberOfRuns)
{
	static const char	*funcName = PROGRAM_NAME": SetNumberOfRuns";

	if (theNumberOfRuns < 1) {
		NotifyError("%s: Number of runs must be greater than zero (%d)",
		  funcName, theNumberOfRuns);
		return(FALSE);
	}
	numberOfRuns = theNumberOfRuns;
	numberOfRunsFlag = TRUE;
	return(TRUE);

}

/****************************** PrintInitialDiagnostics ***********************/

/*
 * This routine prints the initial diagnostics for the program.
 */

void
PrintInitialDiagnostics(void)
{
	DPrint("This test routine calculates the response of a model.\n");
	DPrint("Diagnostics specifier is set to '%s'.\n", diagnosticModeList[
	  diagnosticModeSpecifier].name);
	DPrint("This simulation is run from the script in file '%s'.\n",
	  simScriptFile);
	DPrint("The model simulation is run %d times.\n", numberOfRuns);
	DPrint("The Simulation parameter ");
	if (simParFileModeSpecifier == GENERAL_BOOLEAN_OFF)
		DPrint("mode is 'OFF'.\n");
	else
		DPrint("file is '%s'.\n", simParFileMode);
	DPrint("\n");

}

/****************************** SetProgramName ********************************/

/*
 * This routine sets the global progName variable.  It takes into account the
 * difference between Unix and Dos file names.
 */

void
SetProgramName(void)
{
	char	*p;

#	if defined(GRAPHICS_SUPPORT) && defined(wx_msw)
		wxDos2UnixFilename(*argv);
#	endif
	if ((p = strrchr (*argv, '/')))
		++p;
	else
		p = *argv;
	strcpy(progName, p);

} 

/****************************** PrintUsage ************************************/

/* * This routine prints the usage diagnostics.
 */

void
PrintUsage(void)
{
	SetProgramName();
	fprintf(stderr, "\n"
	  "Usage: %s [options] [parameter1 value1 parameter2 value2 ...]\n"
	  "\t-h            \t: Produce this help message.\n"
	  "\t-l <list>     \t: List options: 'parameters', 'cfinfo'.\n"
	  "\t-P <file name>\t: Use this main parameter file\n"
	  "\t-p <file name>\t: Use this simulation parameter file\n"
	  "\t-r <x>        \t: Repeat the simulation 'x' times.\n"
	  "\t-s <file name>\t: Use this simulation script file\n"
	  "\t-V            \t: Print program version\n",
	  progName);
#	ifdef GRAPHICS_SUPPORT
	wxGetApp().PrintUsage();
#	endif
	exit(1);

}

/****************************** ProcessParComs ********************************/

/*
 * This routine processes parameter-value command pairs, passed as command-line
 * arguments.
 * These parameters are only used once, then the 'parComsUsedFlag' is set.
 * If a new set commands is given (as in the server/client mode) then this
 * flag needs to be reset.
 */

BOOLN
ProcessParComs(EarObjectPtr audModel, int argc, char **argv, int initialCom)
{
	static char	*funcName = PROGRAM_NAME": ProcessParComs";
	int		i;
	DatumPtr	simulation;

	if (parComsUsedFlag)
		return(TRUE);

	if ((simulation = GetSimulation_ModuleMgr(audModel)) == NULL) {
		NotifyError("%s: Simulation not initialised.", funcName);
		exit(1);
	}
	if ((argc - initialCom) % 2 != 0) {
		NotifyError("%s: parameter values must be in <name> <value> pairs.",
		  funcName);
		return(FALSE);
	}
	for (i = initialCom; i < argc; i += 2)
		if (!SetProgramParValue(argv[i], argv[i + 1]) &&
		  !SetUniParValue_Utility_Datum(simulation, argv[i], argv[i + 1])) {
			NotifyError("%s: Could not set '%s' parameter to '%s'.", funcName,
			  argv[i], argv[i + 1]);
			return(FALSE);
		}
	parComsUsedFlag = TRUE;
	return(TRUE);

}

/****************************** ProcessOptions ********************************/

/*
 * This routine processes any command-line options.
 * The '@' options is to be ignored.  It is used by other modules, i.e. myApp.
 * This routine should only change flags and/or.  This allows it to be run
 * more than once.
 */

int
ProcessOptions(int argc, char **argv)
{
	static char *funcName = PROGRAM_NAME": ProcessOptions";
	int		optind;
	char	c, *argument;

	optind = 0;
	while ((c = Process_Options(argc, argv, &optind, &argument,
	  "@#:hl:P:p:r:s:V")))
		switch (c) {
		case '@':
		case '#':
			break;
		case 'h':
			PrintUsage();
			break;
		case 'r':
			if ((numberOfRuns = atoi(argument)) <= 0) {
				NotifyError("%s: No. of simulation runs must be greater than "
				"zero (%d).", funcName, numberOfRuns);
				exit(1);
			}
			numberOfRunsFlag = TRUE;
			break;
		case 'l':
			switch (Identify_NameSpecifier(argument, listingModeList)) {
			case AMS_PARAMETERS_LIST_MODE:
				listParsAndExit = TRUE;
				break;
			case AMS_CFLIST_LIST_MODE:
				listCFListAndExit = TRUE;
				break;
			default:
				fprintf(stderr, "Unknown list mode (%s).\n", argument);
				exit(1);
			}
			break;
		case 'P':
			strcpy(parFileName, argument);
			readMainParsFlag = TRUE;
			break;
		case 'p':
			strcpy(simParFileMode, argument);
			simParFileModeFlag = TRUE;
			break;
		case 's':
			strcpy(simScriptFile, argument);
			simScriptFileFlag = TRUE;
			break;
		case 'V':
			fprintf(stderr, "Version %s, compile date %s, DSAM %s (dynamic), "
			  "%s (compiled).\n", AMS_VERSION, __DATE__, dSAM.version,
			  DSAM_VERSION);
			exit(0);
			break;
		default:
			PrintUsage();
		} /* switch */
	return(optind);

}

/****************************** GetFileStatusPtr ******************************/

/*
 * This routine returns a pointer to the status of a file.
 * The status of the file is a static variable, so the pointer will change if
 * other files are checked with this routine.
 * It returns NULL if the file does not exist or there is any other error.
 */

StatPtr
GetFileStatusPtr(char *fileName)
{
	static char *funcName = PROGRAM_NAME": GetFileStatus";
	static struct stat	simScriptFileStat;

	if (stat(fileName, &simScriptFileStat) == -1) {
		NotifyError("%s: Could not get status of file\n'%s'.", funcName,
		  fileName);
		return(NULL);
	}
	return(&simScriptFileStat);

}

/****************************** SimScriptFileChanged **************************/

/*
 * This routine checks whether the simulation script file has been changed.
 * It will return FALSE if the global 'simLastModified' was originally unset.
 */

BOOLN
SimScriptFileChanged(StatPtr simScriptFileStat, BOOLN updateStatus)
{
	static char *funcName = PROGRAM_NAME": SimScriptFileChanged";
	BOOLN	changed;

	if (!simScriptFileStat) {
		NotifyError("%s: Could not get status of simulation script "
		  "file.", funcName);
		return(FALSE);
	}
	if (simLastModified != simScriptFileStat->st_mtime) {
		changed = (simLastModified != 0);
		if (updateStatus)
			simLastModified = simScriptFileStat->st_mtime;
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
 */

BOOLN
SetProgramParValue(char *parName, char *parValue)
{
	BOOLN	ok = TRUE;
	char	*p, parNameCopy[MAXLINE];
	UniParPtr	par;
	ParFilePtr	localParFilePtr;

	localParFilePtr = parFile;
	parFile = NULL;
	strcpy(parNameCopy, parName);
	if ((p = strchr(parNameCopy, UNIPAR_NAME_SEPARATOR)) != NULL)
		*p = '\0';
	if ((par = FindUniPar_UniParMgr(&programParList, parNameCopy)) == NULL)
		ok = FALSE;
	else if ((par->index != AMS_SIMPARFILE) && !SetParValue_UniParMgr(
	  &programParList, par->index, parValue))
		ok = FALSE;
	parFile = localParFilePtr;
	return(ok);

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
ReadProgParFile(char *fileName)
{
	static BOOLN	readProgParFileFlag = FALSE;
	static char		*funcName = PROGRAM_NAME": ReadProgParFile";
	BOOLN	ok = TRUE, foundDivider = FALSE;
	char	parName[MAXLINE], parValue[MAX_FILE_PATH];
	FILE	*fp;

	if (readProgParFileFlag)
		return(TRUE);
	readProgParFileFlag = TRUE;
	if ((fp = fopen(fileName, "r")) == NULL) {
		NotifyError("%s: Could not open '%s' parameter file.", funcName,
		  fileName);
		readProgParFileFlag = FALSE;
		return(FALSE);
	}
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
		ok = SetProgramParValue(parName, parValue);
	SetEmptyLineMessage_ParFile(TRUE);
	fclose(fp);
	Free_ParFile();
	readProgParFileFlag = FALSE;
	if (!ok) {
		NotifyError("%s: Invalid parameters in file '%s', program parameter "
		  "section.", funcName, fileName);
		return(FALSE);
	}
	return(TRUE);

}

/****************************** InitSimFromSimScript **************************/

/*
 * This routine Initialises the simulation from a simulation script.
 */

BOOLN
InitSimFromSimScript(void)
{
	static char *funcName = PROGRAM_NAME": InitSimFromSimScript";
	StatPtr	simScriptFileStatPtr;

	if ((simScriptFileStatPtr = GetFileStatusPtr(simScriptFile)) == NULL) {
		NotifyError("%s: Could not find status of simulation script\nfile"
		  "'%s'", funcName, simScriptFile);
		FlagReinitialise();
		return(FALSE);
	}
	if (!audModel || SimScriptFileChanged(simScriptFileStatPtr, TRUE)) {
		FreeAll_EarObject();
		datumStepCount = 0;
		if ((audModel = Init_EarObject("Util_SimScript")) == NULL) {
			NotifyError("%s: Could not initialise process.", funcName);
			FreeAll_EarObject();
			FlagReinitialise();
			return(FALSE);
		}
		
	}
	return(TRUE);

}

/****************************** InitSimulation ********************************/

/*
 * This routine Initialises the simulation.
 * It sets the initialCommand global variable.
 */

BOOLN
InitSimulation(void)
{
	static char *funcName = PROGRAM_NAME": InitSimulation";
	BOOLN	ok = TRUE;
	StatPtr	simFileStatPtr;

	simFile = (simParFileModeSpecifier == GENERAL_BOOLEAN_OFF)? simScriptFile:
	  simParFileMode;
	if ((simFileStatPtr = GetFileStatusPtr(simFile)) == NULL) {
		NotifyError("%s: Could not find status of file '%s'", funcName,
		  simFile);
		FlagReinitialise();
		return(FALSE);
	}
	if (!audModel || SimScriptFileChanged(simFileStatPtr, TRUE)) {
		FreeAll_EarObject();
		datumStepCount = 0;
		if ((audModel = Init_EarObject("Util_SimScript")) == NULL) {
			NotifyError("%s: Could not initialise process.", funcName);
			ok = FALSE;
		}
	}
	if (ok) {
		switch (simParFileModeSpecifier) {
		case GENERAL_BOOLEAN_OFF:
			if (!ReadPars_ModuleMgr(audModel, simScriptFile)) {
				NotifyError("%s: Could not read process parameters from\nfile "
				  "'%s'.", funcName, simScriptFile);
				ok = FALSE;
			}
			break;
		default:
			if (!ReadSimParFile_ModuleMgr(audModel, simParFileMode)) {
				NotifyError("%s: Could not read simulation parameters from\n"
				  " file '%s'.", funcName, simParFileMode);
				ok = FALSE;
			}
		}
	}
	if (!ok) {
		FreeAll_EarObject();
		audModel = NULL;
		FlagReinitialise();
		return(FALSE);
	}

#	ifdef GRAPHICS_SUPPORT
	myRunMgr.SetTitle(simFile);
#	endif
	if (gUIModeFlag)
		PrintInitialDiagnostics();
	return(TRUE);

}

/****************************** Init ******************************************/

/*
 * This routine carries out general initialisation tasks.
 * It sets the initialCommand global variable.
 */

BOOLN
Init(void)
{
	static char *funcName = PROGRAM_NAME": Init";

	SetProgramName();
	sprintf(parFileName, "%s.par", PROGRAM_NAME);
	simScriptFileFlag = FALSE;
	numberOfRunsFlag = FALSE;

	if ((diagnosticModeList = InitNameList_NSpecLists(DiagModeList_NSpecLists(
	  0), diagnosticFile)) == NULL) {
		NotifyError("%s: Cannot initalise the diagnostic mode name list.",
		  funcName);
		return(FALSE);
	}

	if (!programParList && !SetUniParList()) {
		NotifyError("%s: Could not initialise parameter list.", funcName);
		return(FALSE);
	}
	initialCommand = ProcessOptions(argc, argv);
	if (!SetDiagnosticMode(diagnosticMode)) {
		NotifyError("%s: Could not set diagnostic mode.", funcName);
		return(FALSE);
	}
	if (!SetSimScriptFile(simScriptFile)) {
		NotifyError("%s: Could not set simulation parameter file mode.",
		  funcName);
		return(FALSE);
	}
	if (!SetSimParFileMode(simParFileMode)) {
		NotifyError("%s: Could not set simulation parameter file mode.",
		  funcName);
		return(FALSE);
	}
	if (!SetSegmentMode(segmentMode)) {
		NotifyError("%s: Could not set segment processing mode.", funcName);
		return(FALSE);
	}
	if (!SetNumberOfRuns(numberOfRuns)) {
		NotifyError("%s: Could not set number of simulation runs.", funcName);
		return(FALSE);
	}
	if ((simParFileModeSpecifier != GENERAL_BOOLEAN_OFF) && !ReadProgParFile(
	  simParFileMode)) {
		NotifyError("%s: Could not read the program settings in\nfile '%s'.",
		  funcName, simParFileMode);
		return(FALSE);
	}
	if (!SetFileLockingMode(fileLockingMode)) {
		NotifyError("%s: Could not set file locking mode.", funcName);
		return(FALSE);
	}
	SetParsFile_Common(diagnosticModeList[diagnosticModeSpecifier].name,
	  OVERWRITE);

	DPrint("Starting %s Application version %s [DSAM Version: %s (dynamic),\n"
	  "%s (compiled)]...\n", PROGRAM_NAME, AMS_VERSION, dSAM.version,
	  DSAM_VERSION);
	DPrint("Parameters read from '%s' file.\n", parFileName);

	checkMainInit = FALSE;
	return(TRUE);

}

/****************************** InitProcess ***********************************/

/*
 * This function initialises the simulation process.
 */

BOOLN
InitProcess(void)
{
	static char *funcName = PROGRAM_NAME": InitProcess";

	if (checkMainInit && !Init())
		return(FALSE);
	if (!InitSimulation()) {
		NotifyError("%s: Could not Initialise simulation.", funcName);
		return(FALSE);
	}
	if (!ProcessParComs(audModel, argc, argv, initialCommand))
		return(FALSE);
	checkInitialisation = FALSE;
	return(TRUE);

}

/****************************** ListParsAndExit *******************************/

/*
 * This routine prints the simulation parameters and exits.
 */

void
ListParsAndExit(void)
{	
	dSAM.noGUIOutputFlag = TRUE;
	SetParsFile_Common("screen", OVERWRITE);
	ListParameters();
	exit(0);

}

/****************************** ListParameters ********************************/

/*
 * This routine prints the simulation and main program parameters.
 */

void
ListParameters(void)
{
	static char *funcName = PROGRAM_NAME": ListParameters";
	char		suffix[MAXLINE];
	
	if (!PrintSimParFile_ModuleMgr(audModel)) {
		NotifyError("%s: Could not print simulation parameter file.", funcName);
		return;
	}
	sprintf(suffix, ".%s.0", PROGRAM_NAME);
	PrintPars_UniParMgr(programParList, "", suffix);

}

/****************************** ListCFListAndExit *****************************/

/*
 * This routine prints the simulation parameters and exits.
 */

void
ListCFListAndExit(void)
{
	static char *funcName = PROGRAM_NAME": ListCFListAndExit";
	CFListPtr	theBMCFs;
	DatumPtr	simulation, bMDatumPtr;
	
	if ((simulation = GetSimulation_ModuleMgr(audModel)) == NULL) {
		NotifyError("%s: Simulation not initialised.", funcName);
		exit(1);
	}
	dSAM.noGUIOutputFlag = TRUE;
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

/****************************** SetArgcAndArgV ********************************/

/*
 * This routine sets the global variables when not in GUI mode.
 * When used in GUI mode, 'argc' and 'argv' will already have been set, so they
 * will be reset to themselves.  This is because 'argc' and 'argv' will not be
 * in the main program arguments, and within 'MainSimulation' the global
 * variables will be in scope.
 */

void
SetArgcAndArgV(int theArgc, char **theArgv)
{
	argc = theArgc;
	argv = theArgv;

}

/****************************** SetLockFile ***********************************/

/*
 * This routine sets the lock file.
 */

BOOLN
SetLockFile(BOOLN on)
{
	static char *funcName = PROGRAM_NAME": SetLockFile";
	FILE	*fp;

	if (on) {
		if ((fp = fopen(LOCK_FILE, "w")) == NULL) {
			NotifyError("%s: Could not create lock file '%s'.", funcName,
			  LOCK_FILE);
			return(FALSE);
		}
		fclose(fp);
	} else
		remove(LOCK_FILE);
	return(TRUE);

}

/******************************************************************************/
/****************************** Main Body *************************************/
/******************************************************************************/

int MainSimulation(MAIN_ARGS)
{
	clock_t		startTime;
	int			i;

	SetArgcAndArgV(argc, argv);
	if (checkInitialisation && !InitProcess())
		return(1);	

	if (listParsAndExit)
		ListParsAndExit();
	if (listCFListAndExit)
		ListCFListAndExit();
	if (!ProcessParComs(audModel, argc, argv, initialCommand))
		return(1);
	if (!gUIModeFlag)
		PrintInitialDiagnostics();
	PrintPars_ModuleMgr( audModel );

	DPrint("Starting process...\n");
	SetSegmentedMode(segmentModeSpecifier);
	startTime = clock();
	ResetProcess_EarObject(audModel);

	if (fileLockingModeSpecifier)
		SetLockFile(TRUE);
	for (i = 0; i < numberOfRuns; i++)
		if (!RunProcess_ModuleMgr(audModel))
			return(0);
	if (fileLockingModeSpecifier)
		SetLockFile(FALSE);

	DPrint("The process took %g CPU seconds to run.\n", (double) (clock() -
	  startTime) / CLOCKS_PER_SEC);
	DPrint("Finished test.\n");
	return(0);
	
}
