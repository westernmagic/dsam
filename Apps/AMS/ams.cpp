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

#include "DSAM.h"
#include "ams.h"

/******************************************************************************/
/****************************** Bitmaps ***************************************/
/******************************************************************************/

#if defined(GRAPHICS_SUPPORT) && !defined(__WXMSW__)
#	include "ams.xpm"
#endif

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

BOOLN	readMainParsFlag = FALSE, numberOfRunsFlag;
BOOLN	checkMainInit = TRUE;
BOOLN	checkInitialisation = TRUE;

char	diagnosticMode[MAX_FILE_PATH], simScriptFile[MAX_FILE_PATH];
char	simParFileMode[MAX_FILE_PATH], parFileName[MAX_FILE_PATH];
char	segmentMode[MAXLINE];
char	fileLockingMode[MAXLINE] = "off";

int		numberOfRuns = 1, fileLockingModeSpecifier;
UniParListPtr	programParList = NULL;

#ifdef GRAPHICS_SUPPORT

// Tell SimMgr that we are passing run information.
RunMgr	myRunMgr(Init);

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
	GetPars_ParFile(fp, "%s", simScriptFile);
	GetPars_ParFile(fp, "%s", simParFileMode);
	GetPars_ParFile(fp, "%s", segmentMode);
	GetPars_ParFile(fp, "%s", fileLockingMode);
	GetPars_ParFile(fp, "%d", &numberOfRuns);
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
	DPrint("The model simulation is run %d times.\n", numberOfRuns);
	DPrint("\n");

}

/****************************** PrintUsage ************************************/

/* * This routine prints the usage diagnostics.
 */

void
PrintUsage(void)
{
	fprintf(stderr, "\n"
	  "%s specific options:\n"
	  "\t-P <file name>\t: Use this main parameter file\n"
	  "\t-r <x>        \t: Repeat the simulation 'x' times.\n"
	  "\t-v            \t: Print program version\n",
	  appInterfacePtr->appName);
	exit(1);

}

/****************************** ProcessOptions ********************************/

/*
 * This routine processes any command-line options.
 * The '@' options is to be ignored.  It is used by other modules, i.e. myApp.
 * This routine should only change flags and/or parameters.  This allows it to
 * be run more than once.
 */

BOOLN
ProcessOptions(int argc, char **argv, int *optInd)
{
	static char *funcName = PROGRAM_NAME": ProcessOptions";
	BOOLN	foundOption = FALSE;
	char	c, *argument;
	int		optSub = 0;

	while ((c = Process_Options(argc, argv, optInd, &optSub, &argument,
	  "@#:P:r:v")))
		switch (c) {
		case '@':
		case '#':
			break;
		case 'r':
			if ((numberOfRuns = atoi(argument)) <= 0) {
				NotifyError("%s: No. of simulation runs must be greater than "
				"zero (%d).", funcName, numberOfRuns);
				exit(1);
			}
			numberOfRunsFlag = TRUE;
			foundOption = TRUE;
			break;
		case 'P':
			strcpy(parFileName, argument);
			readMainParsFlag = TRUE;
			foundOption = TRUE;
			break;
		case 'v':
			fprintf(stderr, "Version %s, compile date %s, DSAM %s (dynamic), "
			  "%s (compiled).\n", AMS_VERSION, __DATE__, dSAM.version,
			  DSAM_VERSION);
			exit(0);
			break;
		default:
			*optInd--;
		} /* switch */
	return(foundOption);

}

/****************************** SetAppInterfacePars ***************************/

/*
 * This function sets the application interface parameters and is passed to the
 * application interface as a function pointer.
 */

BOOLN
SetAppInterfacePars(void)
{
	static const char *funcName = "SetAppInterfacePars";

	if (readMainParsFlag) {
		if (!ReadMainParsFromFile(parFileName)) {
			NotifyError("%s: Could not read parameters from '%s'", funcName, 
			  parFileName);
			return(FALSE);
		}
		if (!SetPars_AppInterface(diagnosticMode, simScriptFile,
		  simParFileMode, segmentMode)) {
			NotifyError("%s: Could not set Application Interface parameters.\n",
			  funcName);
			return(FALSE);
		}
		readMainParsFlag = FALSE;
	}
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

	if (!appInterfacePtr && !Init_AppInterface(GLOBAL)) {
		NotifyError("%s: Could not initialise the application interface.",
		  funcName);
		exit(1);
	}

	SetAppName_AppInterface(PROGRAM_NAME);
	SetAppVersion_AppInterface(AMS_VERSION);
	SetInstallDir_AppInterface(AMS_DATA_INSTALL_DIR);

#	ifdef GRAPHICS_SUPPORT
	myRunMgr.SetIcon(new wxICON(ams));
#	endif

	SetUniParList();

	SetTitle_AppInterface(PROGRAM_NAME);
	SetAppParList_AppInterface(programParList);

	SetAppPrintUsage_AppInterface(PrintUsage);
	SetAppProcessOptions_AppInterface(ProcessOptions);
	SetAppSetInitialPars_AppInterface(SetAppInterfacePars);

	checkInitialisation = FALSE;
	return(TRUE);

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

	if (checkInitialisation && !Init())
		return(1);	

	if (!InitProcessVariables_AppInterface(argc, argv))
		return(1);

	if (!dSAM.usingGUIFlag)
		PrintInitialDiagnostics();
	PrintPars_ModuleMgr(appInterfacePtr->audModel);

	DPrint("Starting process...\n");
	startTime = clock();
	ResetProcess_EarObject(appInterfacePtr->audModel);

	if (fileLockingModeSpecifier)
		SetLockFile(TRUE);
	for (i = 0; i < numberOfRuns; i++)
		if (!RunProcess_ModuleMgr(appInterfacePtr->audModel))
			return(0);
	if (fileLockingModeSpecifier)
		SetLockFile(FALSE);

	DPrint("The process took %g CPU seconds to run.\n", (double) (clock() -
	  startTime) / CLOCKS_PER_SEC);
	DPrint("Finished test.\n");
	return(0);
	
}
