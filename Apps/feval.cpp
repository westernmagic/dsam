/**********************
 *
 * File:		feval.cpp
 * Purpose:		This program runs a series of test on a filter simulation.
 * Comments:	
 * Author:		L. P. O'Mard
 * Created:		28 Jun 2000
 * Updated:		
 * Copyright:	(c) 2000, University of Essex
 *
 *********************/

#ifdef HAVE_CONFIG_H
#	include "FEvalSetup.h"
#endif /* HAVE_CONFIG_H */

#include <stdlib.h> 
#include <stdio.h>
#include <time.h>

#include "DSAM.h"
#include "Utils.h"
#include "TuningCurve.h"
#include "MultiIOFuncs.h"
// #include "ClickGainFreqFuncs.h"
// #include "ClickPhaseFreqFuncs.h"
#include "FilterShape.h"
// #include "PhaseIntFuncs.h"
// #include "PhaseFreqFuncs.h"
// #include "RelClickPhaseFreqFuncs.h"
// #include "RelPhaseFreqFuncs.h"
// #include "TwoTSRatio.h"
// #include "TwoTSResp.h"
#include "ImpulseResp.h"
// #include "DistProds.h"
#include "feval.h"

/******************************************************************************/
/****************************** Bitmaps ***************************************/
/******************************************************************************/

#if defined(GRAPHICS_SUPPORT) && !defined(__WXMSW__)
#	include "feval.xpm"
#endif

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

WChar	parFileName[MAX_FILE_PATH];
WChar	fileLockingMode[MAXLINE] = wxT("off");

int		fileLockingModeSpecifier;
int		numTestModules = 0;

EarObjectPtr	*testProcess = NULL;
UniParListPtr	*testParList = NULL;

/******************************************************************************/
/****************************** Functions and subroutines *********************/
/******************************************************************************/

/****************************** SetNumTestModules *****************************/

/*
 * This routine returns the name specifier for the display panel list.
 */

void
SetNumTestModules(void)
{
	uShort	i;

	for (i = 0; i < GetNumUserModules_ModuleReg(); i++)
		if (StrNCmpNoCase_Utility_String(UserList_ModuleReg(i)->name,
		  FEVAL_TEST_MOD_PREFIX) == 0)
			numTestModules++;

}

/****************************** SetUniParList *********************************/

/*
 * This function initialises and sets the programs universal parameter
 * list. This list provides universal access to the program's
 * parameters.  It needs to be called from an initialise routine.
 * The other parameter list entries are set in the post-init function.
 */

BOOLN
SetUniParList(UniParListPtr *parList)
{
	static const WChar *funcName = PROGRAM_NAME wxT(": SetUniParList");
	int		i, count;
	UniParPtr	pars;
	ModRegEntryPtr	p;

	if ((*parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  FEVAL_NUM_PARS + numTestModules, NULL)) == NULL) {
		NotifyError(wxT("%s: Could not initialise parList."), funcName);
		return(FALSE);
	}
	pars = (*parList)->pars;
	SetPar_UniParMgr(&pars[FEVAL_FILELOCKINGMODE], wxT("FILELOCKING_MODE"),
	  wxT("File locking mode ('on' or 'off')."),
	  UNIPAR_BOOL,
	  &fileLockingModeSpecifier, NULL,
	  (void * (*)) SetFileLockingMode);

	for (i = 0, count = 0; i < GetNumUserModules_ModuleReg(); i++) {
		p = UserList_ModuleReg(i);
		if (StrNCmpNoCase_Utility_String(p->name, FEVAL_TEST_MOD_PREFIX) != 0)
			continue;
		if ((testProcess[count] = Init_EarObject(p->name)) == NULL) {
			NotifyError(wxT("%s: Could not initialise test process '%s'."),
			  funcName, p->name);
			return(FALSE);
		}
		testParList[count] = (* testProcess[count]->module->GetUniParListPtr)();
		SetPar_UniParMgr(&pars[FEVAL_NUM_PARS + count], p->name,
		  p->name,
		  UNIPAR_PARLIST,
		  &testParList[count], NULL,
		  NULL);
		count++;
	}

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

/****************************** PrintInitialDiagnostics ***********************/

/*
 * This routine prints the initial diagnostics for the program.
 */

void
PrintInitialDiagnostics(void)
{
	DPrint(wxT("This test routine runs a series of filter tests on a model.\n"));
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
	  wxT("\t-P <file name>\t: Use this main parameter file\n")
	  wxT("\t-v            \t: Print program version\n"),
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
ProcessOptions(int argc, WChar **argv, int *optInd)
{
	/* static WChar *funcName = PROGRAM_NAME wxT(": ProcessOptions"); */
	BOOLN	foundOption = FALSE;
	WChar	c, *argument;
	int		optSub = 0;

	while ((c = Process_Options(argc, argv, optInd, &optSub, &argument,
	  wxT("@#:r:v"))))
		switch (c) {
		case '@':
		case '#':
			break;
		case 'v':
			fprintf_Utility_String(stderr, wxT("Version %s, compile date %s, ")
			  wxT("DSAM %s (dynamic), ")
			  wxT("%s (compiled).\n"), FEVAL_VERSION, __DATE__, GetDSAMPtr_Common()->
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
	static WChar *funcName = PROGRAM_NAME wxT(": SetLockFile");
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

/****************************** RegisterUserModules ***************************/

/*
 * This routine registers my user modules.
 * In this special version, it also initialises global instances of the module.
 */

BOOLN
RegisterUserModules(void)
{
	static WChar *funcName = PROGRAM_NAME wxT(": RegisterUserModules");

	RegEntry_ModuleReg(FEVAL_TEST_MOD_PREFIX FILTERSHAPE_MOD_NAME,
	  InitModule_FilterShape);
	RegEntry_ModuleReg(FEVAL_TEST_MOD_PREFIX IMPULSERESP_MOD_NAME,
	  InitModule_ImpulseResp);
	RegEntry_ModuleReg(FEVAL_TEST_MOD_PREFIX MULTIIOFUNCS_MOD_NAME,
	  InitModule_MultiIOFuncs);
	RegEntry_ModuleReg(FEVAL_TEST_MOD_PREFIX TUNINGCURVE_MOD_NAME,
	  InitModule_TuningCurve);

	SetNumTestModules();
	if ((testProcess = (EarObjectPtr *) calloc(numTestModules, sizeof(
	  EarObjectPtr))) == NULL) {
		NotifyError(wxT("%s: Could not initialise test process list (%d)."),
		  funcName, numTestModules);
		return(FALSE);
	}
	if ((testParList = (UniParListPtr *) calloc(numTestModules, sizeof(
	  UniParListPtr))) == NULL) {
		NotifyError(wxT("%s: Could not initialise test process parameter list list ")
		  wxT("(%d)."), funcName, numTestModules);
		return(FALSE);
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
	static WChar *funcName = PROGRAM_NAME wxT(": Init");

	if (!appInterfacePtr && !Init_AppInterface(GLOBAL)) {
		NotifyError(wxT("%s: Could not initialise the application interface."),
		  funcName);
		exit(1);
	}

	SetAppName_AppInterface(PROGRAM_NAME);
	SetAppVersion_AppInterface(FEVAL_VERSION);
	SetCompiledDSAMVersion_AppInterface(DSAM_VERSION);
	SetInstallDir_AppInterface(FEVAL_DATA_INSTALL_DIR);

#	ifdef USE_GUI
	//wxGetApp().SetIcon(new wxICON(feval));
#	endif

	SetTitle_AppInterface(PROGRAM_NAME);
	SetAppSetUniParList_AppInterface(SetUniParList);

	SetAppPrintUsage_AppInterface(PrintUsage);
	SetAppProcessOptions_AppInterface(ProcessOptions);
	SetAppRegisterUserModules_AppInterface(RegisterUserModules);

	return(TRUE);

}

/****************************** FreeProcessVariables **************************/

/*
 * This routine free's any memory allocated during the program.
 * The individual parLists are free'd by their modules, only the main
 * array needs to be free'd.
 */

void
FreeProcessVariables(void)
{
	int		i;

	if (testProcess) {
		for (i = 0; i < numTestModules; i++)
			Free_EarObject(&testProcess[i]);
		free(testProcess);
	}
	if (testParList)
		free(testParList);
			

}

/******************************************************************************/
/****************************** Main Body *************************************/
/******************************************************************************/

int MainSimulation(MAIN_ARGS)
{
	WChar		*timeDesc;
	clock_t		startClock;
	time_t		startTime;
	uShort		i, count;
	double		diffTime;

	if (!InitProcessVariables_AppInterface(Init, ARGC, ARGV))
		return(1);

	if (!GetDSAMPtr_Common()->usingGUIFlag)
		PrintInitialDiagnostics();
	PrintSimPars_AppInterface();

	DPrint(wxT("Starting process...\n"));
	startTime = time(NULL);
	startClock = clock();
	ResetSim_AppInterface();

	if (fileLockingModeSpecifier)
		SetLockFile(TRUE);
	
	for (i = 0, count = 0; i < numTestModules; i++)
		if (*GetUniParPtr_ModuleMgr(testProcess[i], wxT("enabled"))->valuePtr.i) {
			PrintPars_ModuleMgr(testProcess[i]);
			if (!RunProcess_ModuleMgr(testProcess[i])) {
				NotifyError(wxT("%s: Could not run test %d."), PROGRAM_NAME, i);
				return(1);
			}
			count++;
		}

	if (fileLockingModeSpecifier)
		SetLockFile(FALSE);

	diffTime = difftime(time(NULL), startTime);
	i = 0;
	while (diffTime > 60.0) {
		i++;
		diffTime /= 60.0;
	}
	switch (i) {
	case 0:
		timeDesc = wxT("seconds");
		break;
	case 1:
		timeDesc = wxT("minutes");
		break;
	default:
		timeDesc = wxT("hours");
	}
	DPrint(wxT("The process took %g %s (%g CPU) to run.\n"),  diffTime, timeDesc,
	  (double) (clock() - startClock) / CLOCKS_PER_SEC);
	DPrint(wxT("There %s %d test%s executed.\n"), (count == 1)? wxT("was"):
	  wxT("were"), count, (count == 1)? wxT(""): wxT("s"));
	DPrint(wxT("Finished test.\n"));
	FreeProcessVariables();
	return(0);
	
}
