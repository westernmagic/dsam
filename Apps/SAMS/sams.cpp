/**********************
 *
 * File:		sams.cpp
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

#include <DSAM.h>

#undef 	PACKAGE_NAME
#undef	PACKAGE_STRING
#undef	PACKAGE_TARNAME
#undef	PACKAGE_VERSION

#ifdef HAVE_CONFIG_H
#	include "SAMSSetup.h"
#endif /* HAVE_CONFIG_H */

#include "sams.h"

/******************************************************************************/
/****************************** Bitmaps ***************************************/
/******************************************************************************/

#if defined(USE_GUI) && !defined(__WXMSW__)
#	include "sams.xpm"
#endif

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

WChar	fileLockingMode[MAXLINE] = wxT("off");
WChar	autoNumRunsMode[MAXLINE] = wxT("off");

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
	static const WChar *funcName = wxT("SetUniParList");
	UniParPtr	pars;

	if ((*parList = InitList_UniParMgr(UNIPAR_SET_GENERAL, SAMS_NUM_PARS,
	  NULL)) == NULL) {
		NotifyError(wxT("%s: Could not initialise parList."), funcName);
		return(FALSE);
	}
	pars = (*parList)->pars;
	SetPar_UniParMgr(&pars[SAMS_FILELOCKINGMODE], wxT("FILELOCKING_MODE"),
	  wxT("File locking mode ('on' or 'off')."),
	  UNIPAR_BOOL,
	  &fileLockingModeSpecifier, NULL,
	  (void * (*)) SetFileLockingMode);
	SetPar_UniParMgr(&pars[SAMS_AUTONUMRUNSMODE], wxT("AUTO_NUM_RUNS_MODE"),
	  wxT("Auto-setting of the number of runs (data files only) ('on' or ")
	    wxT("'off')."),
	  UNIPAR_BOOL,
	  &autoNumRunsModeSpecifier, NULL,
	  (void * (*)) SetAutoNumRunsMode);
	SetPar_UniParMgr(&pars[SAMS_NUMBEROFRUNS], wxT("NUM_RUNS"),
	  wxT("Number of repeat runs, or segments/frames."),
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
SetFileLockingMode(WChar *theFileLockingMode)
{
	static const WChar	*funcName = PROGRAM_NAME wxT(": SetFileLockingMode");

	if ((fileLockingModeSpecifier = Identify_NameSpecifier(theFileLockingMode,
	  BooleanList_NSpecLists(0))) == GENERAL_BOOLEAN_NULL) {
		NotifyError(wxT("%s: Illegal file locking mode (%s): must be ")
		  wxT("'on' or 'off'."), funcName, fileLockingMode);
		return(FALSE);
	}
	DSAM_strcpy(fileLockingMode, theFileLockingMode);
	return(TRUE);

}

/****************************** SetAutoNumRunsMode ****************************/

/*
 * This functions sets the automatic number of runs setting mode.
 * It returns false if it fails in any way.
 */

BOOLN
SetAutoNumRunsMode(WChar *theAutoNumRunsMode)
{
	static const WChar	*funcName = PROGRAM_NAME wxT(": SetAutoNumRunsMode");

	if ((autoNumRunsModeSpecifier = Identify_NameSpecifier(theAutoNumRunsMode,
	  BooleanList_NSpecLists(0))) == GENERAL_BOOLEAN_NULL) {
		NotifyError(wxT("%s: Illegal auto. number of runs mode mode (%s): must ")
		  wxT("be 'on' or 'off'."), funcName, theAutoNumRunsMode);
		return(FALSE);
	}
	DSAM_strcpy(autoNumRunsMode, theAutoNumRunsMode);
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
	static const WChar	*funcName = PROGRAM_NAME wxT(": SetNumberOfRuns");

	if (theNumberOfRuns < 0) {
		NotifyError(wxT("%s: Illegal number of runs (%d)."), funcName,
		  theNumberOfRuns);
		return(FALSE);
	}
	numberOfRuns = theNumberOfRuns;
	return(TRUE);

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
	static const WChar *funcName = PROGRAM_NAME wxT(": AutoSetNumberOfRuns");
	Float	totalDuration, segmentDuration;
	EarObjectPtr	process;

	if (!GetDSAMPtr_Common()->segmentedMode) {
		numberOfRuns = 1;
		return(TRUE);
	}

	if ((process = GetDataFileInProcess_AppInterface()) == NULL)
		return(TRUE);
	numberOfRuns = 1;	/* Default value */
	segmentDuration = *GetUniParPtr_ModuleMgr(process, wxT("duration"))->
	  valuePtr.r;
	if (segmentDuration < 0.0) {
		NotifyError(wxT("%s: Segment size must be set when using auto 'number ")
		  wxT("of runs' mode."), funcName);
		return(FALSE);
	}
	if ((totalDuration = (((DataFilePtr) process->module->parsPtr)->
	  GetDuration)()) < 0.0) {
		NotifyError(wxT("%s: Could not determine signal size for data file."),
		  funcName);
		return(FALSE);
	}
	if (segmentDuration > totalDuration) {
		NotifyError(wxT("%s: Segment size (%g ms) is larger than total signal ")
		  wxT("duration (%g ms)."), funcName, MILLI(segmentDuration), MILLI(
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
	DPrint(wxT("This test routine calculates the response of a model.\n"));
	DPrint(wxT("The model simulation is run %d time%s.\n"), numberOfRuns,
	  (numberOfRuns == 1)? wxT(""): wxT("s"));
	DPrint(wxT("\n"));

}

/****************************** PrintUsage ************************************/

/* * This routine prints the usage diagnostics.
 */

void
PrintUsage(void)
{
	fprintf_Utility_String(stderr, wxT("\n")
	  wxT("%s specific options:\n")
	  wxT("\t-r <x>        \t: Repeat the simulation 'x' times.\n")
	  wxT("\t-v            \t: Print program version\n"),
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
ProcessOptions(int argc, WChar **argv, int *optInd)
{
	static const WChar *funcName = PROGRAM_NAME wxT(": ProcessOptions");
	BOOLN	foundOption = FALSE;
	WChar	c, *argument;
	int		optSub = 0;

	while ((c = Process_Options(argc, argv, optInd, &optSub, &argument,
	  wxT("@#:r:v"))) != '\0')
		switch (c) {
		case '@':
		case '#':
			break;
		case 'r':
			if ((numberOfRuns = DSAM_atoi(argument)) <= 0) {
				NotifyError(wxT("%s: No. of simulation runs must be greater ")
				  wxT("than zero (%d)."), funcName, numberOfRuns);
				exit(1);
			}
			foundOption = TRUE;
			break;
		case 'v':
			fprintf_Utility_String(stderr, wxT("Version %s, compile date %s, ")
			  wxT("DSAM %s (dynamic), %s (compiled).\n"), SAMS_VERSION,
			  __TDATE__, GetDSAMPtr_Common()->version, DSAM_VERSION);
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
	static const WChar *funcName = PROGRAM_NAME wxT(": SetLockFile");
	FILE	*fp;

	if (on) {
		if ((fp = fopen(ConvUTF8_Utility_String(LOCK_FILE), "w")) == NULL) {
			NotifyError(wxT("%s: Could not create lock file '%s'."), funcName,
			  LOCK_FILE);
			return(FALSE);
		}
		fclose(fp);
	} else
		remove(ConvUTF8_Utility_String(LOCK_FILE));
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
	/*static WChar *funcName = PROGRAM_NAMEwxT(": PostInitFunc");*/

	return(TRUE);

}

/****************************** RegisterUserModules ***************************/

/*
 * This routine registers my user modules.
 */

BOOLN
RegisterUserModules(void)
{
	/*RegEntry_ModuleReg(wxT("BM_Carney2001"),InitModule_BasilarM_Carney2001);*/
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
	/* static WChar *funcName = PROGRAM_NAMEwxT(": Init"); */

	SetAppName_AppInterface(PROGRAM_NAME);
	SetAppVersion_AppInterface(SAMS_VERSION);
	SetCompiledDSAMVersion_AppInterface(DSAM_VERSION);
	SetInstallDir_AppInterface(SAMS_DATA_INSTALL_DIR);
	AddAppHelpBook_AppInterface(SAMS_HELP);
	AddAppHelpBook_AppInterface(SAMS_TUTORIAL_HELP);

#	ifdef USE_GUI
	wxGetApp().SetIcon(new wxICON(sams));
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
	time_t	startTime;
	int		i;

	if (!InitProcessVariables_AppInterface(Init, ARGC, ARGV))
		return(1);

	if (!GetDSAMPtr_Common()->usingGUIFlag)
		PrintInitialDiagnostics();
	PrintSimPars_AppInterface();

	DPrint(wxT("Starting process...\n"));
	startTime = time(NULL);
	ResetSim_AppInterface();

	if (fileLockingModeSpecifier)
		SetLockFile(TRUE);
	if (autoNumRunsModeSpecifier && !AutoSetNumberOfRuns())
		return(FALSE);
	for (i = 0; i < numberOfRuns; i++)
		if (!RunSim_AppInterface())
			return(0);
	if (fileLockingModeSpecifier)
		SetLockFile(FALSE);

	DPrint(wxT("The process took %g seconds to run.\n"), difftime(time(NULL),
	  startTime));

	Free_AppInterface();
	DPrint(wxT("Finished test.\n"));
	return(0);

}
