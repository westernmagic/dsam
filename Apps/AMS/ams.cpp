/**********************
 *
 * File:		ams.cpp
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

#ifdef HAVE_CONFIG_H
#	include "AMSSetup.h"
#endif /* HAVE_CONFIG_H */

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

#if defined(USE_GUI) && !defined(__WXMSW__)
#	include "ams.xpm"
#endif

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

char	fileLockingMode[MAXLINE] = "off";
char	autoNumRunsMode[MAXLINE] = "off";

int		numberOfRuns = 1;
int		fileLockingModeSpecifier = GENERAL_BOOLEAN_OFF;
int		autoNumRunsModeSpecifier = GENERAL_BOOLEAN_OFF;

/******************************************************************************/
/****************************** Functions and subroutines *********************/
/******************************************************************************/

/****************************** SetUniParList *********************************/

/*
 * This function initialises and sets the programs universal parameter
 * list. This list provides universal access to the program's
 * parameters.  It needs to be called from an initialise routine.
 */

BOOLN
SetUniParList(UniParListPtr *parList)
{
	static const char *funcName = "SetUniParList";
	UniParPtr	pars;

	if ((*parList = InitList_UniParMgr(UNIPAR_SET_GENERAL, AMS_NUM_PARS,
	  NULL)) == NULL) {
		NotifyError("%s: Could not initialise parList.", funcName);
		return(FALSE);
	}
	pars = (*parList)->pars;
	SetPar_UniParMgr(&pars[AMS_FILELOCKINGMODE], "FILELOCKING_MODE",
	  "File locking mode ('on' or 'off').",
	  UNIPAR_BOOL,
	  &fileLockingModeSpecifier, NULL,
	  (void * (*)) SetFileLockingMode);
	SetPar_UniParMgr(&pars[AMS_AUTONUMRUNSMODE], "AUTO_NUM_RUNS_MODE",
	  "Auto-setting of the number of runs (data files only) ('on' or 'off').",
	  UNIPAR_BOOL,
	  &autoNumRunsModeSpecifier, NULL,
	  (void * (*)) SetAutoNumRunsMode);
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

/****************************** SetAutoNumRunsMode ****************************/

/*
 * This functions sets the automatic number of runs setting mode.
 * It returns false if it fails in any way.
 */

BOOLN
SetAutoNumRunsMode(char *theAutoNumRunsMode)
{
	static const char	*funcName = PROGRAM_NAME": SetAutoNumRunsMode";

	if ((autoNumRunsModeSpecifier = Identify_NameSpecifier(theAutoNumRunsMode,
	  BooleanList_NSpecLists(0))) == GENERAL_BOOLEAN_NULL) {
		NotifyError("%s: Illegal auto. number of runs mode mode (%s): must be "
		  "'on' or 'off'.", funcName, theAutoNumRunsMode);
		return(FALSE);
	}
	strcpy(autoNumRunsMode, theAutoNumRunsMode);
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

	if (theNumberOfRuns < 0) {
		NotifyError("%s: Illegal number of runs (%d).", funcName,
		  theNumberOfRuns);
		return(FALSE);
	}
	numberOfRuns = theNumberOfRuns;
	return(TRUE);

}

/****************************** GetDataFileInProcess **************************/

/*
 * This routine returns a pointer to the a 'DataFile_In' process at the
 * beginning of a simulation.  If it is not there, then it returns NULL.
 */

EarObjectPtr
GetDataFileInProcess(void)
{
	static char *funcName = PROGRAM_NAME": GetDataFileInProcess";
	FILE	*savedErrorsFilePtr = GetDSAMPtr_Common()->errorsFile;
	EarObjectPtr	process;

	SetErrorsFile_Common("off", OVERWRITE);
	process = GetFirstProcess_Utility_Datum(GetSimulation_ModuleMgr(
	  GetPtr_AppInterface()->audModel));
	GetDSAMPtr_Common()->errorsFile = savedErrorsFilePtr;
	if (!process)
		return(NULL);
	if (StrCmpNoCase_Utility_String(process->module->name, "DataFile_In") !=
	  0)
		return(NULL);
	return(process);

}

/****************************** AutoSetNumberOfRuns ***************************/

/*
 * This routine sets the number of runs according to a loaded simulation script
 * which has a 'DataFile_In' process at the beginning.  This means it can
 * only be used it the simulation has been initialised.
 */

BOOLN
AutoSetNumberOfRuns(void)
{
	static char *funcName = PROGRAM_NAME": AutoSetNumberOfRuns";
	double	totalDuration, segmentDuration;
	EarObjectPtr	process;

	if (!GetDSAMPtr_Common()->segmentedMode) {
		numberOfRuns = 1;
		return(TRUE);
	}

	if ((process = GetDataFileInProcess()) == NULL)
		return(TRUE);
	numberOfRuns = 1;	/* Default value */
	segmentDuration = *GetUniParPtr_ModuleMgr(process, "duration")->valuePtr.r;
	if (segmentDuration < 0.0) {
		NotifyError("%s: Segment size must be set when using auto 'number of "
		  "runs' mode.", funcName);
		return(FALSE);
	}
	if ((totalDuration = (((DataFilePtr) process->module->parsPtr)->
	  GetDuration)()) < 0.0) {
		NotifyError("%s: Could not determine signal size for data file.",
		  funcName);
		return(FALSE);
	}
	if (segmentDuration > totalDuration) {
		NotifyError("%s: Segment size (%g ms) is larger than total signal "
		  "duration (%g ms).", funcName, MILLI(segmentDuration), MILLI(
		  totalDuration));
		return(FALSE);
	}
	numberOfRuns = (int) floor(totalDuration / segmentDuration);
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
	  "\t-r <x>        \t: Repeat the simulation 'x' times.\n"
	  "\t-v            \t: Print program version\n",
	  GetPtr_AppInterface()->appName);
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
	  "@#:r:v")))
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
			foundOption = TRUE;
			break;
		case 'v':
			fprintf(stderr, "Version %s, compile date %s, DSAM %s (dynamic), "
			  "%s (compiled).\n", AMS_VERSION, __DATE__, GetDSAMPtr_Common()->
			  version, DSAM_VERSION);
			exit(0);
			break;
		default:
			*optInd--;
		} /* switch */
	return(foundOption);

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

/****************************** PostInitFunc **********************************/

/*
 * This routine is run at the endi of the 'InitProcessVariables_UtAppInterface'
 * routine.
 */

BOOLN
PostInitFunc(void)
{
	static char *funcName = PROGRAM_NAME": PostInitFunc";

	return(TRUE);

}

/****************************** RegisterUserModules ***************************/

/*
 * This routine registers my user modules.
 */

BOOLN
RegisterUserModules(void)
{
	/*RegEntry_ModuleReg("BM_Carney2001", InitModule_BasilarM_Carney2001); */
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

	SetAppName_AppInterface(PROGRAM_NAME);
	SetAppVersion_AppInterface(AMS_VERSION);
	SetCompiledDSAMVersion_AppInterface(DSAM_VERSION);
	SetInstallDir_AppInterface(AMS_DATA_INSTALL_DIR);
	AddAppHelpBook_AppInterface(AMS_HELP);
	AddAppHelpBook_AppInterface(AMS_TUTORIAL_HELP);

#	ifdef USE_GUI
	wxGetApp().SetIcon(new wxICON(ams));
#	endif

	SetAppSetUniParList_AppInterface(SetUniParList);

	SetAppPrintUsage_AppInterface(PrintUsage);
	SetAppProcessOptions_AppInterface(ProcessOptions);
	SetAppPostInitFunc_AppInterface(PostInitFunc);

	SetAppRegisterUserModules_AppInterface(RegisterUserModules);
	return(TRUE);

}


/******************************************************************************/
/****************************** Main Body *************************************/
/******************************************************************************/

int MainSimulation(MAIN_ARGS)
{
	clock_t	startTime;
	int		i;

	if (!InitProcessVariables_AppInterface(Init, ARGC, ARGV))
		return(1);

	if (!GetDSAMPtr_Common()->usingGUIFlag)
		PrintInitialDiagnostics();
	PrintSimPars_AppInterface();

	DPrint("Starting process...\n");
	startTime = clock();
	ResetSim_AppInterface();

	if (fileLockingModeSpecifier)
		SetLockFile(TRUE);
	if (autoNumRunsModeSpecifier && GetDataFileInProcess() &&
	  !AutoSetNumberOfRuns())
		return(FALSE);
	for (i = 0; i < numberOfRuns; i++)
		if (!RunSim_AppInterface())
			return(0);
	if (fileLockingModeSpecifier)
		SetLockFile(FALSE);

	DPrint("The process took %g CPU seconds to run.\n", (double) (clock() -
	  startTime) / CLOCKS_PER_SEC);

	Free_AppInterface();
	DPrint("Finished test.\n");
	return(0);
	
}
