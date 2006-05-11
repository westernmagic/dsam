/**********************
 *
 * File:		PaMaster1.c
 * Purpose:		This module runs an auditory model in parallel channels, using
 *				MPI.
 * Comments:	Written using ModuleProducer version 1.5.
 *				A simulation EarObject is initialised so that values from the
 *				simulation specification can be obtained.
 *				21-04-97 LPO: Routine changed to read the numWorkers parameter
 *				and not read a CFList.
 * Author:		L. P. O'Mard
 * Created:		28 Nov 1995
 * Updated:		21 Apr 1997
 * Copyright:	(c) 1998, University of Essex.
 *
 **********************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG_H */

#ifdef MPI_SUPPORT	/* To stop package dependency problems. */
#	include "mpi.h"
#endif

#include "GeCommon.h"
#include "GeSignalData.h"
#include "GeEarObject.h"
#include "GeUniParMgr.h"
#include "UtBandwidth.h"
#include "UtCFList.h"
#include "UtSimScript.h"
#include "FiParFile.h"
#include "PaGeneral.h"
#include "UtString.h"
#include "PaComUtils.h"
#include "PaMaster1.h"
#include "UtSSParser.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

Master1Ptr	master1Ptr = NULL;

struct {

	BOOLN		initialised;
	WChar		**initStringPtrs;
	WChar		hostName[MPI_MAX_PROCESSOR_NAME];
	int			myRank;
	int			numProcesses;
	int			instanceCount;

} masterAdmin = {

		FALSE,	/* initialised */
		NULL,	/* initStringPtrs */
		wxT(""),		/* hostName */
		0,		/* myRank */
		0,		/* numProcesses */
		0		/* instanceCount */

	};

/******************************************************************************/
/****************************** Subroutines and functions *********************/
/******************************************************************************/

/****************************** Shutdown **************************************/

/*
 * This routine closes MPI if no more master processes are using it.
 * It also sends the final shutdown to the workers.
 */

void
Shutdown_MPI_Master1(int masterRank, int status)
{
	int		i;

	if (masterAdmin.instanceCount > 0)
		return;
#	ifndef GRAPHICS_SUPPORT
	for (i = 0; i < masterAdmin.numProcesses; i++)
		if (i != masterRank)
			MPI_Send(&status, 1, MPI_INT, i, MASTER_EXIT_TAG,
			  MPI_COMM_WORLD);
#	else
	masterRank = 0;	/* These lines are to stop C++ complaining. */
	status = 0;
	i = 0;
#	endif  /* GRAPHICS_SUPPORT */
	MPI_Finalize();
	free(masterAdmin.initStringPtrs);
	masterAdmin.initialised = FALSE;

}

/****************************** Free ******************************************/

/*
 * This function releases of the memory allocated for the process
 * variables. It should be called when the module is no longer in use.
 * It is defined as returning a BOOLN value because the generic
 * module interface requires that a non-void value be returned.
 */

BOOLN
Free_MPI_Master1(void)
{
	/* static const WChar	*funcName = wxT("Free_MPI_Master1"); */

	if (master1Ptr == NULL)
		return(FALSE);
	FreeProcessVariables_MPI_Master1();
	RemoveQueue_SetComUtil(&master1Ptr->setComQueue);

	masterAdmin.instanceCount--;
#	ifndef GRAPHICS_SUPPORT
	Shutdown_MPI_Master1(masterAdmin.myRank, master1Ptr->ok);
#	endif
	if (master1Ptr->parList)
		FreeList_UniParMgr(&master1Ptr->parList);
	if (master1Ptr->parSpec == GLOBAL) {
		free(master1Ptr);
		master1Ptr = NULL;
	}
	return(TRUE);

}

/****************************** InitMPI ***************************************/

/*
 * This routine initialises the MPI system.
 * It should be called from the main module Init routine.
 */

BOOLN
InitMPI_MPI_Master1(void)
{
	WChar	charBuffer[BUFSIZ];
	int		i, nameLength;

#	ifndef GRAPHICS_SUPPORT
	static const WChar	*funcName = wxT("InitMPI_MPI_Master1");
	int		argc = 1;

	if (masterAdmin.instanceCount == 0) {
		if ((masterAdmin.initStringPtrs = (WChar **) calloc(argc,
		  sizeof(WChar *))) == NULL) {
			NotifyError(wxT("%s: Out of memory for %d initialisation strings."),
			 funcName, argc);
			 return(FALSE);
		}
		masterAdmin.initStringPtrs[0] = (WChar *) funcName;
		MPI_Init( &argc, &masterAdmin.initStringPtrs );
		masterAdmin.initialised = TRUE;
	}
#	endif
	MPI_Comm_rank( MPI_COMM_WORLD, &masterAdmin.myRank);
	MPI_Group_size( MPI_COMM_WORLD, &masterAdmin.numProcesses);
	MPI_Get_processor_name(masterAdmin.hostName, &nameLength);
	DSAM_strcpy(charBuffer, masterAdmin.hostName);
	for (i = 0; i < masterAdmin.numProcesses; i++)
		if (i != masterAdmin.myRank) {
			MPI_Send(&masterAdmin.myRank, 1, MPI_INT, i, MASTER_INIT_TAG,
			  MPI_COMM_WORLD);
			MPI_Send(charBuffer, nameLength, MPI_CHAR, i, MASTER_INIT_TAG,
			  MPI_COMM_WORLD);
		}
	masterAdmin.instanceCount++;
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
Init_MPI_Master1(ParameterSpecifier parSpec)
{
	static const WChar	*funcName = wxT("Init_MPI_Master1");

	if (parSpec == GLOBAL) {
		if (master1Ptr != NULL)
			Free_MPI_Master1();
		if ((master1Ptr = (Master1Ptr) malloc(sizeof(Master1))) == NULL) {
			NotifyError(wxT("%s: Out of memory for 'global' pointer"),
			  funcName);
			return(FALSE);
		}
	} else { /* LOCAL */
		if (master1Ptr == NULL) {
			NotifyError(wxT("%s:  'local' pointer not set."), funcName);
			return(FALSE);
		}
	}
	master1Ptr->updateProcessVariablesFlag = TRUE;
	master1Ptr->parSpec = parSpec;
	master1Ptr->diagnosticsModeFlag = FALSE;
	master1Ptr->channelModeFlag = FALSE;
	master1Ptr->numWorkersFlag = FALSE;
	master1Ptr->simScriptParFile = NULL;
	master1Ptr->cFListParFile = NULL;
	master1Ptr->diagnosticsMode = 0;
	master1Ptr->channelMode = 0;
	master1Ptr->numWorkers = 0;
	master1Ptr->ok = TRUE;
	master1Ptr->printTheCFs = FALSE;
	master1Ptr->numChannels = 0;
	master1Ptr->interleaveLevel = 0;
	master1Ptr->workDetails = NULL;
	master1Ptr->dataPtr = NULL;
	master1Ptr->setComQueue = NULL;
	simScriptPtr = &master1Ptr->simScript;
	Init_Utility_SimScript(LOCAL);
	master1Ptr->theCFs = NULL;

	master1Ptr->workerDiagModeList = InitWorkerDiagModeList_MPI_General();
	master1Ptr->workerChanModeList = InitWorkerChanModeList_MPI_General();
	if (!SetUniParList_MPI_Master1()) {
		NotifyError(wxT("%s: Could not initialise parameter list."), funcName);
		Free_MPI_Master1();
		return(FALSE);
	}
	InitMPI_MPI_Master1();
	return(TRUE);

}

/****************************** SetUniParList *********************************/

/*
 * This function initialises and sets the module's universal parameter
 * list. This list provides universal access to the module's
 * parameters.  It expects to be called from the 'Init_' routine.
 */

BOOLN
SetUniParList_MPI_Master1(void)
{
	static const WChar *funcName = wxT("SetUniParList_MPI_Master1");
	UniParPtr	pars;

	if ((master1Ptr->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,
	  MPI_MASTER1_NUM_PARS, NULL)) == NULL) {
		NotifyError(wxT("%s: Could not initialise parList."), funcName);
		return(FALSE);
	}
	pars = master1Ptr->parList->pars;
	SetPar_UniParMgr(&pars[MPI_MASTER1_NUMWORKERS], wxT("NUM_WORKERS"),
	  wxT("Number of workers."),
	  UNIPAR_INT,
	  &master1Ptr->numWorkers, NULL,
	  (void * (*)) SetNumWorkers_MPI_Master1);
	SetPar_UniParMgr(&pars[MPI_MASTER1_DIAGNOSTICSMODE], wxT("DIAG_MODE"),
	  wxT("Diagnostics mode ('OFF', 'FILE' or 'SCREEN')."),
	  UNIPAR_NAME_SPEC,
	  &master1Ptr->diagnosticsMode, master1Ptr->diagnosticsModeList,
	  (void * (*)) SetDiagnosticsMode_MPI_Master1);
	SetPar_UniParMgr(&pars[MPI_MASTER1_CHANNELMODE], wxT("CHANNEL_MODE"),
	  wxT("Worker channel mode ('Normal' or 'SET_TO_BM')."),
	  UNIPAR_NAME_SPEC,
	  &master1Ptr->channelMode, master1Ptr->channelModeList,
	  (void * (*)) SetChannelMode_MPI_Master1);
	SetPar_UniParMgr(&pars[MPI_MASTER1_SIMSPECPARFILE], wxT("SIMSPEC_FILE"),
	  wxT("Simulation specification utility parameter file."),
	  UNIPAR_FILE_NAME,
	  &master1Ptr->simScriptParFile, (WChar *) wxT("*.sim"),
	  (void * (*)) SetSimScriptParFile_MPI_Master1);
	SetPar_UniParMgr(&pars[MPI_MASTER1_CFLISTPARFILE], wxT("CFLIST_FILE"),
	  wxT("Centre frequency and bandwidth specification file."),
	  UNIPAR_FILE_NAME,
	  &master1Ptr->cFListParFile, &dSAM.parsFilePath,
	  (void * (*)) SetCFListParFile_MPI_Master1);
	return(TRUE);

}

/****************************** GetUniParListPtr ******************************/

/*
 * This function returns a pointer to the module's universal parameter
 * list.
 */

UniParListPtr
GetUniParListPtr_MPI_Master1(void)
{
	static const WChar	*funcName = wxT("GetUniParListPtr_MPI_Master1");

	if (master1Ptr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (master1Ptr->parList == NULL) {
		NotifyError(wxT("%s: UniParList data structure has not been "
		  "initialised. NULL returned."), funcName);
		return(NULL);
	}
	return(master1Ptr->parList);

}

/****************************** SetPars ***************************************/

/*
 * This function sets all the module's parameters.
 * It returns TRUE if the operation is successful.
 */

BOOLN
SetPars_MPI_Master1(WChar *simScriptParFile, WChar *cFListParFile,
  WChar *diagnosticsMode, WChar *channelMode, int numWorkers)
{
	static const WChar	*funcName = wxT("SetPars_MPI_Master1");
	BOOLN	ok;

	ok = TRUE;
	if (!SetDiagnosticsMode_MPI_Master1(diagnosticsMode))
		ok = FALSE;
	if (!SetChannelMode_MPI_Master1(channelMode))
		ok = FALSE;
	if (!SetNumWorkers_MPI_Master1(numWorkers))
		ok = FALSE;
	if (!SetCFListParFile_MPI_Master1(cFListParFile))
		ok = FALSE;
	if (!SetSimScriptParFile_MPI_Master1(simScriptParFile))
		ok = FALSE;
	if (!ok)
		NotifyError(wxT("%s: Failed to set all module parameters.") ,funcName);
	return(ok);

}

/****************************** SetNumWorkers *********************************/

/*
 * This function sets the number of workers expected to be used with the module.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetNumWorkers_MPI_Master1(int theNumWorkers)
{
	static const WChar	*funcName = wxT("SetTheCFs_MPI_Master1");

	if (master1Ptr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (theNumWorkers < 1) {
		NotifyError(wxT("%s: Invalid no. of workers (%d) - must be greater "
		  "than zero"), funcName);
		return(FALSE);
	}
	master1Ptr->numWorkersFlag = TRUE;
	master1Ptr->numWorkers = theNumWorkers;
	return(TRUE);

}

/****************************** SetDiagnosticsMode ****************************/

/*
 * This function sets the module's diagnosticsMode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetDiagnosticsMode_MPI_Master1(WChar *theDiagnosticsMode)
{
	static const WChar	*funcName = wxT("SetDiagnosticsMode_MPI_Master1");
	int		specifier;

	if (master1Ptr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theDiagnosticsMode,
	  master1Ptr->workerDiagModeList)) == WORKER_DIAGNOSTICS_NULL) {
		NotifyError(wxT("%s: Illegal mode name (%s)."), funcName,
		  theDiagnosticsMode);
		return(FALSE);
	}
	master1Ptr->diagnosticsModeFlag = TRUE;
	master1Ptr->diagnosticsMode = specifier;
	return(TRUE);

}

/****************************** SetChannelMode ********************************/

/*
 * This function sets the module's channelMode parameter.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetChannelMode_MPI_Master1(WChar *theChannelMode)
{
	static const WChar	*funcName = wxT("SetChannelMode_MPI_Master1");
	int		specifier;

	if (master1Ptr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(theChannelMode,
	  master1Ptr->workerChanModeList)) == WORKER_CHANNEL_MODE_NULL) {
		NotifyError(wxT("%s: Illegal mode name (%s)."), funcName,
		  theChannelMode);
		return(FALSE);
	}
	/*** Put any other required checks here. ***/
	master1Ptr->channelModeFlag = TRUE;
	master1Ptr->channelMode = specifier;
	return(TRUE);

}

/****************************** SetCFListParFile *****************************/

/*
 * This function sets the module's CFList parameter file name.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetCFListParFile_MPI_Master1(WChar *theCFListParFile)
{
	static const WChar	*funcName = wxT("SetCFListParFile_MPI_Master1");

	if (theCFListParFile == NULL) {
		NotifyError(wxT("%s: CFList parameter file has not been set."),
		  funcName);
		return(FALSE);
	}
	master1Ptr->cFListParFile = theCFListParFile;
	return(TRUE);

}

/****************************** SetSimScriptParFile ***************************/

/*
 * This function sets the module's stimulation instruction list.
 * It returns TRUE if the operation is successful.
 * Additional checks should be added as required.
 */

BOOLN
SetSimScriptParFile_MPI_Master1(WChar *theSimScriptParFile)
{
	static const WChar	*funcName = wxT("SetSimScriptParFile_MPI_Master1");

	if (theSimScriptParFile == NULL) {
		NotifyError(wxT("%s: Simulation specification parameter file has not "
		  "been set."), funcName);
		return(FALSE);
	}
	master1Ptr->simScriptParFile= theSimScriptParFile;
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
CheckPars_MPI_Master1(void)
{
	static const WChar	*funcName = wxT("CheckPars_MPI_Master1");
	BOOLN	ok;

	ok = TRUE;
	if (master1Ptr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (master1Ptr->simScriptParFile == NULL) {
		NotifyError(wxT("%s: Simulation Specification parameter file not set."),
		  funcName);
		ok = FALSE;
	}
	if (master1Ptr->cFListParFile == NULL) {
		NotifyError(wxT("%s: CF list parameter  file not set."),
		  funcName);
		ok = FALSE;
	}
	if (!master1Ptr->diagnosticsModeFlag) {
		NotifyError(wxT("%s: Diagnostics mode variable not set."), funcName);
		ok = FALSE;
	}
	if (!master1Ptr->channelModeFlag) {
		NotifyError(wxT("%s: Channel mode variable not set."), funcName);
		ok = FALSE;
	}
	if (!master1Ptr->numWorkersFlag) {
		NotifyError(wxT("%s: No. of Workers variable not set."), funcName);
		ok = FALSE;
	}
	return(ok);

}

/****************************** GetCFListPtr **********************************/

/*
 * This routine returns a pointer to the module's CFList data pointer.
 * If the module has not yet been run, then it will read in the parameter
 * file and set the CFs.
 *
 */

CFListPtr
GetCFListPtr_MPI_Master1(void)
{
	static const WChar *funcName = wxT("GetCFListPtr_MPI_Master1");

	if (master1Ptr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (master1Ptr->simScriptParFile != NULL)
		ReadCFListParFile_MPI_Master1();
	if (master1Ptr->theCFs == NULL) {
		NotifyError(wxT("%s: CFList data structure has not been correctly "
		  "set.  NULL returned.", funcName);
		return(NULL);
	}
	return(master1Ptr->theCFs);

}

/****************************** GetNumWorkers *********************************/

/*
 * This routine returns the no. of workers used for the module.
 *
 */

int
GetNumWorkers_MPI_Master1(void)
{
	static const WChar *funcName = wxT("GetNumWorkers_MPI_Master1");

	if (master1Ptr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (!master1Ptr->numWorkersFlag) {
		NotifyError(wxT("%s: Number of workers not correctly set, zero "
		  "returnd."), funcName);
		return(0);
	}
	return(master1Ptr->numWorkers);

}

/****************************** GetSimulation *********************************/

/*
 * This routine returns a pointer to the module's simulation instruction list.
 * It returns NULL if it fails in any way.
 */
 
DatumPtr
GetSimulation_MPI_Master1(void)
{
	static const WChar	*funcName = wxT("GetSimulation_MPI_Master1");

	if (master1Ptr == NULL) {
		NotifyError(wxT("%s: Module not initialised."), funcName);
		return(FALSE);
	}
	if (master1Ptr->simScript.simulation == NULL) {
		NotifyError(wxT("%s: Simulation not set -- this routine can only be "
		  "used\nafter the module has been run."), funcName);
		return(NULL);
	}
	return(master1Ptr->simScript.simulation);

}

/****************************** PrintPars *************************************/

/*
 * This routine prints all the module's parameters to the file stream.
 * specified by the dSAM.parsFile file pointer.
 */

BOOLN
PrintPars_MPI_Master1(void)
{
	static const WChar	*funcName = wxT("PrintPars_MPI_Master1");

	if (!CheckPars_MPI_Master1()) {
		NotifyError(wxT("%s: Parameters have not been correctly set."), funcName);
		return(FALSE);
	}
	DPrint(wxT("Auditory Model MPI Module Parameters:-\n"));
	if (master1Ptr->channelMode == WORKER_CHANNEL_SET_TO_BM_MODE) {
		DPrint(wxT("\tCFList parameter file: %s,"), master1Ptr->cFListParFile);
		master1Ptr->printTheCFs = TRUE;
	}
	DPrint(wxT("\tNo. of Workers: %d,\n"), master1Ptr->numWorkers);
	DPrint(wxT("\tStimulation specification parameter file: %s\n"),
	  master1Ptr->simScriptParFile);
	DPrint(wxT("\tWorker diagnostics mode: %s,"),
	  master1Ptr->workerDiagModeList[master1Ptr->diagnosticsMode].name);
	DPrint(wxT("\tChannel mode: %s,\n"),
	  master1Ptr->workerChanModeList[master1Ptr->channelMode].name);
	return(TRUE);

}

/****************************** ReadPars **************************************/

/*
 * This routine reads a specified number of parameters from a file.
 * It returns FALSE if it fails in any way.n */

BOOLN
ReadPars_MPI_Master1(WChar *fileName)
{
	static const WChar	*funcName = wxT("ReadPars_MPI_Master1");
	static	WChar simScriptParFile[MAXLINE];
	static	WChar cFListParFile[MAXLINE];
	BOOLN	ok;
	WChar	*filePath;
	WChar	diagnosticsMode[MAXLINE], channelMode[MAXLINE];
	int		numWorkers;
	FILE	*fp;

	filePath = GetParsFileFPath_Common(fileName);
	if ((fp = ConvUTF8_Utility_String(filePath), "r")) == NULL) {
		NotifyError(wxT("%s: Cannot open data file '%s'.\n"), funcName, filePath);
		return(FALSE);
	}
	DPrint(wxT("%s: Reading from '%s':\n"), funcName, filePath);
	Init_ParFile();
	ok = TRUE;
	if (!GetPars_ParFile(fp, wxT("%d"), &numWorkers))
		ok = FALSE;
	if (!GetPars_ParFile(fp, wxT("%s"), simScriptParFile))
		ok = FALSE;
	if (!GetPars_ParFile(fp, wxT("%s"), diagnosticsMode))
		ok = FALSE;
	if (!GetPars_ParFile(fp, wxT("%s"), channelMode))
		ok = FALSE;
	if (!GetPars_ParFile(fp, wxT("%s"), cFListParFile))
		ok = FALSE;
	Free_ParFile();
	if (!ok) {
		NotifyError(wxT("%s: Not enough lines, or invalid parameters, in module "
		  "parameter file '%s'."), funcName, filePath);
		return(FALSE);
	}
	if (!SetPars_MPI_Master1(simScriptParFile, cFListParFile, diagnosticsMode,
	  channelMode, numWorkers)) {
		NotifyError(wxT("%s: Could not set parameters."), funcName);
		return(FALSE);
	}
	return(TRUE);

}

/****************************** SetParsPointer ********************************/

/*
 * This function sets the global parameter pointer for the module to
 * that associated with the module instance.
 */

BOOLN
SetParsPointer_MPI_Master1(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("SetParsPointer_MPI_Master1");

	if (!theModule) {
		NotifyError(wxT("%s: The module is not set."), funcName);
		return(FALSE);
	}
	master1Ptr = (Master1Ptr) theModule->parsPtr;
	return(TRUE);

}

/****************************** InitModule ************************************/

/*
 * This routine sets the function pointers for this process module.
 */

BOOLN
InitModule_MPI_Master1(ModulePtr theModule)
{
	static const WChar	*funcName = wxT("InitModule_MPI_Master1");

	if (!SetParsPointer_MPI_Master1(theModule)) {
		NotifyError(wxT("%s: Cannot set parameters pointer."), funcName);
		return(FALSE);
	}
	if (!Init_MPI_Master1(GLOBAL)) {
		NotifyError(wxT("%s: Could not initialise process structure."),
		  funcName);
		return(FALSE);
	}
	theModule->parsPtr = master1Ptr;
	theModule->CheckPars = CheckPars_MPI_Master1;
	theModule->Free = Free_MPI_Master1;
	theModule->GetNumWorkers = GetNumWorkers_MPI_Master1;
	theModule->GetUniParListPtr = GetUniParListPtr_MPI_Master1;
	theModule->PrintPars = PrintPars_MPI_Master1;
	theModule->ReadPars = ReadPars_MPI_Master1;
	theModule->RunProcess = DriveWorkers_MPI_Master1;
	theModule->SetParsPointer = SetParsPointer_MPI_Master1;
	theModule->QueueCommand = QueueCommand_MPI_Master1;
	theModule->SendQueuedCommands = SendQueuedCommands_MPI_Master1;
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
CheckData_MPI_Master1(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("CheckData_MPI_Master1");

	if (data == NULL) {
		NotifyError(wxT("%s: EarObject not initialised."), funcName);
		return(FALSE);
	}
	if (master1Ptr->numWorkers != masterAdmin.numProcesses - 1) {
		NotifyError(wxT("%s:  %d workers are required, but %d have been "
		  "started."), funcName, master1Ptr->numWorkers, masterAdmin.
		  numProcesses - 1);
		return(FALSE);
	}
	/*** Put additional checks here. ***/
	return(TRUE);

}

/**************************** FreeProcessVariables ****************************/

/*
 * This routine releases the memory allocated for the process variables
 * if they have been initialised.
 */

BOOLN
FreeProcessVariables_MPI_Master1(void)
{
	if (master1Ptr->workDetails) {
		free(master1Ptr->workDetails);
		master1Ptr->workDetails = NULL;
	}
	Free_CFList(&master1Ptr->theCFs);
	simScriptPtr = &master1Ptr->simScript;
	Free_Utility_SimScript();
	if (master1Ptr->dataPtr) {
		free(master1Ptr->dataPtr);
		master1Ptr->dataPtr = NULL;
	}
	master1Ptr->updateProcessVariablesFlag = TRUE;
	return(TRUE);

}

/**************************** SetWorkDivison **********************************/

/*
 * This routine sets the work division for each worker.  It divides the
 * number of CF channels between each worker.
 * It assumes that the module has been correctly initialised.
 * Note that binaural signals must be sent in pairs to the workers.
 * If there is more than one channel described using the CFs structure, then
 * copies of the same input signal will be sent to each worker.
 *
 */

BOOLN
SetWorkDivision_MPI_Master1(int masterRank)
{
	static const WChar *funcName = wxT("SetWorkDivisionMPI_Master1");
	int		i, workerCount, allocatedChannels, channelAllocation, numChanUnits;
	WorkDetailPtr	work;

	numChanUnits = master1Ptr->numChannels / master1Ptr->interleaveLevel;
	if (master1Ptr->numWorkers > numChanUnits) {
		NotifyError(wxT("%s: Not enough work for %d workers (%d units)."),
		  funcName, master1Ptr->numWorkers, numChanUnits);
		return(FALSE);
	}
	if ((master1Ptr->workDetails = (WorkDetail *) calloc(master1Ptr->numWorkers,
	  sizeof(WorkDetail))) == NULL) {
		NotifyError(wxT("%s: Out of memory for %d work details."), funcName,
		  master1Ptr->numWorkers);
		return(FALSE);
	}
	channelAllocation = numChanUnits / master1Ptr->numWorkers *
	  master1Ptr->interleaveLevel;
	for (i = allocatedChannels = workerCount = 0; i < master1Ptr->numWorkers +
	  1; i++)
		if (i != masterRank) {
			work = &master1Ptr->workDetails[workerCount++];
			work->rank = i;
			work->sampleCount = 0;
			work->numChannels = channelAllocation;
			allocatedChannels += channelAllocation;
		}
	for (i = 0; (allocatedChannels != master1Ptr->numChannels) && (i <
	  master1Ptr->numWorkers); i++) {
		master1Ptr->workDetails[i].numChannels += master1Ptr->interleaveLevel;
		allocatedChannels += master1Ptr->interleaveLevel;
	}
	master1Ptr->workDetails[0].offset = 0;
	for (i = 1; i < master1Ptr->numWorkers; i++)
		master1Ptr->workDetails[i].offset = master1Ptr->workDetails[i -
		  1].offset + master1Ptr->workDetails[i - 1].numChannels;
	return(TRUE);

}

/**************************** SendParsToWorkers *******************************/

/*
 * This routine sends the simulation parameters to the workers.
 * It returns the status response from the worker
 */

int
SendParsToWorkers_MPI_Master1(EarObjectPtr data, int worker)
{
	BOOLN	ok = TRUE;
	int		control, response;
	double	*ptr;
	WorkDetailPtr	work;
	MPI_Status		status;

	work = &master1Ptr->workDetails[worker];
	control = INIT_SIMULATION;
	MPI_Send(&control, 1, MPI_INT, work->rank, MASTER_MESG_TAG, MPI_COMM_WORLD);
	MPI_Send(&data->handle, 1, MPI_UNSIGNED, work->rank, MASTER_MESG_TAG,
	  MPI_COMM_WORLD);
	MPI_Send(master1Ptr->simScriptParFile, MAXLINE, MPI_CHAR, work->rank,
	  MASTER_MESG_TAG, MPI_COMM_WORLD);
	MPI_Send(&work->numChannels, 1, MPI_INT, work->rank, MASTER_MESG_TAG,
	  MPI_COMM_WORLD);
	MPI_Send(&master1Ptr->diagnosticsMode, 1, MPI_INT, work->rank,
	  MASTER_MESG_TAG, MPI_COMM_WORLD);
	MPI_Send(&master1Ptr->channelMode, 1, MPI_INT, work->rank, MASTER_MESG_TAG,
	  MPI_COMM_WORLD);
	if (master1Ptr->channelMode == WORKER_CHANNEL_SET_TO_BM_MODE) {
		ptr = master1Ptr->theCFs->frequency + work->offset;
		MPI_Send(ptr, work->numChannels, MPI_DOUBLE, work->rank,
		  MASTER_MESG_TAG, MPI_COMM_WORLD);
		ptr = master1Ptr->theCFs->bandwidth + work->offset;
		MPI_Send(ptr, work->numChannels, MPI_DOUBLE, work->rank,
		  MASTER_MESG_TAG, MPI_COMM_WORLD);
	}
	if (data->inSignal[0] == NULL)
		MPI_Send(&ok, 1, MPI_INT, work->rank, MASTER_MESG_TAG, MPI_COMM_WORLD);
	else {
		if (!SendSignalPars_MPI_General(data->inSignal[0], work->numChannels,
		  work->rank, MASTER_INIT_TAG))
			ok = FALSE;
		MPI_Send(&ok, 1, MPI_INT, work->rank, MASTER_INIT_TAG, MPI_COMM_WORLD);
	}
	MPI_Recv(&response, 1, MPI_INT, work->rank, WORKER_MESG_TAG, MPI_COMM_WORLD,
	  &status);
	return(response);

}

/**************************** ReadCFListParFile *******************************/

/*
 * This routine reads the CF list and bandwidth specifcation from the
 * specified file.
 * It will only read the CF list if they have not been read already: this
 * routine could already have been run by the GetCFListPtr_... routine.
 */

BOOLN
ReadCFListParFile_MPI_Master1(void)
{
	static const WChar	*funcName = wxT("ReadCFListParFile_MPI_Master1");
	BOOLN	ok = TRUE;
	FILE	*fp;

	if (master1Ptr->theCFs != NULL)
		return(TRUE);
	if ((fp = fopen((char *) master1Ptr->cFListParFile, "r")) == NULL) {
		NotifyError(wxT("%s: Cannot parameter file '%s'.\n"), funcName,
		  master1Ptr->cFListParFile);
		return(FALSE);
	}
	Init_ParFile();
	if ((master1Ptr->theCFs = ReadPars_CFList(fp)) == NULL) {
		NotifyError(wxT("%s: Could not read CF list.\n"), funcName);
		ok = FALSE;
	}
	if (!ReadBandwidths_CFList(fp, master1Ptr->theCFs)) {
		NotifyError(wxT("%s: Could not read bandwidths.\n"), funcName);
		ok = FALSE;
	}
	fclose(fp);
	Free_ParFile();
	return(ok);


}

/**************************** InitProcessVariables ****************************/

/*
 * This function allocates the memory for the process variables.
 */

BOOLN
InitProcessVariables_MPI_Master1(EarObjectPtr data)
{
	static const WChar *funcName = wxT("InitProcessVariables_MPI_Master1");
	BOOLN	ok;
	int		i;
	
	if (master1Ptr->updateProcessVariablesFlag || data->updateProcessFlag ||
	  (master1Ptr->theCFs && master1Ptr->theCFs->updateFlag)) {
		FreeProcessVariables_MPI_Master1();
		if ((master1Ptr->dataPtr = (ChanData **) calloc(master1Ptr->numWorkers,
		  sizeof(ChanData *))) == NULL) {
			NotifyError(wxT("%s: Out of memory for %d data pointers."),
			  funcName, master1Ptr->numWorkers);
			return(FALSE);
		}
		master1Ptr->handle = data->handle;
		master1Ptr->interleaveLevel = (data->inSignal[0] == NULL)? 1:
		  data->inSignal[0]->interleaveLevel;
		if (!ReadCFListParFile_MPI_Master1()) {
			NotifyError(wxT("%s: Could not read CF list parameters."),
			  funcName);
			return(FALSE);
		}
		switch (master1Ptr->channelMode) {
		case WORKER_CHANNEL_SET_TO_BM_MODE:
			if (master1Ptr->printTheCFs) {
				PrintPars_CFList(master1Ptr->theCFs);
				master1Ptr->printTheCFs = FALSE;
			}
			master1Ptr->numChannels = (data->inSignal[0])?
			  master1Ptr->theCFs->numChannels * data->inSignal[0]->numChannels:
			  master1Ptr->theCFs->numChannels;
			break;
		default:
			master1Ptr->numChannels = (data->inSignal[0] == NULL)?
			  master1Ptr->numWorkers: data->inSignal[0]->numChannels;
			break;
		} /* switch */
		if (!SetWorkDivision_MPI_Master1(masterAdmin.myRank)) {
			NotifyError(wxT("%s: Failed to divide work."), funcName);
			return(FALSE);
		}
		for (i = 0, ok = TRUE; i < master1Ptr->numWorkers; i++) {
			if (SendParsToWorkers_MPI_Master1(data, i) == FALSE) {
				NotifyError(wxT("%s: Failed to initialise worker[%d]."),
				  funcName, master1Ptr->workDetails[i].rank);
				ok = FALSE;
			}
		}
		if (!ok)
			return(FALSE);
		simScriptPtr = &master1Ptr->simScript;
		Init_Utility_SimScript(LOCAL);
		if (!ReadPars_Utility_SimScript(master1Ptr->simScriptParFile)) {
			NotifyError(wxT("%s: Could not read simulation specification."),
			  funcName);
			return(FALSE);
		}
		master1Ptr->updateProcessVariablesFlag = FALSE;
		if (master1Ptr->theCFs)
			master1Ptr->theCFs->updateFlag = FALSE;
	} else if (data->timeIndex == PROCESS_START_TIME) {
		
		/* Processes to be reset at start time. */
		
	}
	ok = SendQueuedCommands_MPI_Master1();
	return(ok);

}

/****************************** FindWorkDetail ********************************/

/*
 * This routine searches within a work detail list for the work detail which
 * corresponds to a particular rank.
 * It returns the index to the work detail.
 */

int
FindWorkDetail_MPI_Master1(int rank)
{
	int		i;
	
	for (i = 0; i < master1Ptr->numWorkers; i++)
		if (rank == master1Ptr->workDetails[i].rank)
			return(i);
	return(-1);

}

/****************************** QueueCommand **********************************/

/*
 * This routine installs commands into a queue, to be sent to the workers.
 * Commands can be sent to all workers, or to a single worker.
 * Commands sent to single workers will be applied cyclicly to each worker. The
 * user should beware of this.
 */

BOOLN
QueueCommand_MPI_Master1(void *parameter, int parCount, TypeSpecifier type,
  WChar *label, CommandSpecifier command, ScopeSpecifier scope)
{
	static const WChar *funcName = wxT("QueueCommand_MPI_Master1");

	if (!CheckPars_MPI_Master1()) {
		NotifyError(wxT("%s: Parameters have not been correctly set."),
		  funcName);
		return(FALSE);
	}
	if (label == NULL) {
		NotifyError(wxT("%s: Illegal NULL label!"), funcName);
		return(FALSE);
	}
	return(AddToQueue_SetComUtil(&master1Ptr->setComQueue, parameter, parCount,
	  type, label, command, scope));

}

/****************************** SendSetCommandToWorker ************************/

/*
 * This routine sends a parameter and an associated command description to a
 * specified worker.
 * It uses the SET tags for the "set" conversations.
 * It assumes that the master module has been correctly initialised.
 */

BOOLN
SendSetCommandToWorker_MPI_Master1(WorkDetailPtr work, SetComPtr com)
{
	static const WChar *funcName = wxT("SendSetCommandToWorker_MPI_Master1");
	WChar	charBuffer[BUFSIZ];
	int		sendLabelLength;

	DSAM_strcpy(charBuffer, com->label);
	sendLabelLength = DSAM_strlen(com->label) + 1;
	MPI_Send(&com->command, 1, MPI_INT, work->rank, MASTER_SET_TAG,
	  MPI_COMM_WORLD);
	MPI_Send(&master1Ptr->handle, 1, MPI_UNSIGNED, work->rank, MASTER_SET_TAG,
	  MPI_COMM_WORLD);
	MPI_Send(charBuffer, sendLabelLength, MPI_CHAR, work->rank, MASTER_SET_TAG,
	  MPI_COMM_WORLD);
	switch (com->type) {
	case PA_VOID:
		break;
	case PA_STRING:
		MPI_Send(com->u.string, com->parCount, MPI_CHAR, work->rank,
		  MASTER_SET_TAG, MPI_COMM_WORLD);
		break;
	case PA_INT:
		MPI_Send(com->u.intVal, com->parCount, MPI_INT, work->rank,
		  MASTER_SET_TAG, MPI_COMM_WORLD);
		break;
	case PA_DOUBLE:
		MPI_Send(com->u.doubleVal, com->parCount, MPI_DOUBLE, work->rank,
		  MASTER_SET_TAG, MPI_COMM_WORLD);
		break;
	case PA_LONG:
		MPI_Send(com->u.longVal, com->parCount, MPI_LONG, work->rank,
		  MASTER_SET_TAG, MPI_COMM_WORLD);
		break;
	default:
		NotifyError(wxT("%s: Illegal parameter type (%d)."), funcName,
		  (int) com->type);
		return(FALSE);
	} /* switch */
	return(TRUE);

}

/****************************** SendQueuedCommands ****************************/

/*
 * This routine sends a parameter and an associated command description to all
 * of the workers.
 */

BOOLN
SendQueuedCommands_MPI_Master1(void)
{
	static const WChar *funcName = wxT("SendQueuedCommands_MPI_Master1");
	BOOLN	ok;
	int		i, response, numResponses, count = 0;
	WorkDetailPtr	work;
	MPI_Status		status;
	SetComPtr		p;

	if (!CheckPars_MPI_Master1()) {
		NotifyError(wxT("%s: Parameters have not been correctly set."),
		  funcName);
		return(FALSE);
	}
	for (p = master1Ptr->setComQueue, ok = TRUE; ok && (p != NULL); p = p->next)
		switch (p->scope) {
		case ALL_WORKERS:
			numResponses = master1Ptr->numWorkers;
			for (i = 0; i < master1Ptr->numWorkers; i++) {
				work = &master1Ptr->workDetails[i];
				if (!SendSetCommandToWorker_MPI_Master1(work, p)) {
					NotifyError(wxT("%s: Failed to send command to "
					  "worker[%d]."), work->rank);
					ok = FALSE;
					numResponses--;
				}
			}
			for (i = 0; i < numResponses; i++) {
				MPI_Recv(&response, 1, MPI_INT, MPI_ANY_SOURCE, WORKER_SET_TAG,
				  MPI_COMM_WORLD, &status);
				if (response == FALSE) {
					NotifyError(wxT("%s: Worker[%d] failed execute set "
					  "command."), funcName, status.MPI_SOURCE);
					ok = FALSE;
				}
			}
			break;
		case WORKER_CYCLIC:
			work = &master1Ptr->workDetails[count];
			if (!SendSetCommandToWorker_MPI_Master1(work, p)) {
				NotifyError(wxT("%s: Failed to send command to worker[%d]."),
				  work->rank);
				ok = FALSE;
			}
			if (ok) {
				MPI_Recv(&response, 1, MPI_INT, work->rank, WORKER_SET_TAG,
				  MPI_COMM_WORLD, &status);
				if (response == FALSE) {
					NotifyError(wxT("%s: Worker[%d] failed to execute set "
					  "command."), funcName, work->rank);
					ok = FALSE;
				}
			}
			count = (++count % master1Ptr->numWorkers);
			break;
		default:
			NotifyError(wxT("%s: Illegal command scope (%d)."), (int) p->scope);
			ok = FALSE;
			break;
		} /* switch */
	RemoveQueue_SetComUtil(&master1Ptr->setComQueue);
	return(ok);

}

/****************************** SendControlToWorkers **************************/

/*
 * This routine sends a control command to the workers and awaits the
 * respective responses.
 * It returns FALSE if any of the workers return a FALSE response.
 * The total number of channels for this processes output signal is calculated
 * using the work allocation of the first worker.
 * It assumes that the process has been correctly initialised.
 */

BOOLN
SendControlToWorkers_MPI_Master1(EarObjectPtr data, int numWorkers,
  ControlSpecifier control)
{
	static const WChar	*funcName = wxT("SendControlToWorkers_MPI_Master1");
	BOOLN	ok;
	int		i, response;
	SignalDataPtr	templateSignal;
	WorkDetailPtr	work;
	MPI_Status		status;

	if (numWorkers < 1) {
		NotifyError(wxT("%s: Illegal number of workers (%d)."), funcName,
		  numWorkers);
		return(FALSE);
	}
	for (i = 0; i < numWorkers; i++) {
		work = &master1Ptr->workDetails[i];
		MPI_Send(&control, 1, MPI_INT, work->rank, MASTER_MESG_TAG,
		  MPI_COMM_WORLD);
		MPI_Send(&data->handle, 1, MPI_UNSIGNED, work->rank, MASTER_MESG_TAG,
		  MPI_COMM_WORLD);
	}
	for (i = 0, ok = TRUE; i < numWorkers; i++) {
		MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
		switch (status.MPI_TAG) {
		case WORKER_MESG_TAG:
			MPI_Recv(&response, 1, MPI_INT, MPI_ANY_SOURCE, WORKER_MESG_TAG,
			  MPI_COMM_WORLD, &status);
			break;
		case WORKER_INIT_TAG:
			if ((templateSignal = ReceiveSignalPars_MPI_General(
			  status.MPI_SOURCE, WORKER_INIT_TAG)) == NULL) {
				NotifyError(wxT("%s: Could not receive signal parameters from "
				  "Worker[%d]."), funcName, status.MPI_SOURCE);
				return(FALSE);
			}
			work = &master1Ptr->workDetails[FindWorkDetail_MPI_Master1(
			  status.MPI_SOURCE)];
			templateSignal->numChannels *= (master1Ptr->numChannels /
			  work->numChannels);
			if (!SetOutSignal_MPI_General(data, templateSignal))
				return(FALSE);
			if (master1Ptr->channelMode == WORKER_CHANNEL_SET_TO_BM_MODE) {
				SetLocalInfoFlag_SignalData(data->outSignal, TRUE);
				SetInfoChannelLabels_SignalData(data->outSignal,
				  master1Ptr->theCFs->frequency);
				SetInfoCFArray_SignalData(data->outSignal,
				  master1Ptr->theCFs->frequency);
			}
			break;
		default:
			NotifyError(wxT("%s: Unknown tag received (%d)."), funcName,
			  status.MPI_TAG);
			return(FALSE);
		} /* switch */
		if (response == FALSE) {
			NotifyError(wxT("%s: Worker[%d] failed to execute the %s command."),
			  funcName, status.MPI_SOURCE, ControlStr_MPI_General(control));
			ok = FALSE;
		}
	}
	return(ok);

}

/****************************** SendInSignalToWorkers *************************/

/*
 * This routine sends a connected input signal to the workers, if one exists.
 * It assumes that the data EarObject is correctly initialised.
 * It returns FALSE if the program fails in any way.
 */

BOOLN
SendInSignalToWorkers_MPI_Master1(EarObjectPtr data)
{
	BOOLN	ok = TRUE;
	int		i;
	WorkDetailPtr	work;

	if (data->inSignal[0] == NULL)
		return(TRUE);
	for (i = 0; i < master1Ptr->numWorkers; i++) {
		work = &master1Ptr->workDetails[i];
		MPI_Send(&data->handle, 1, MPI_UNSIGNED, work->rank, MASTER_INIT_TAG,
		  MPI_COMM_WORLD);
		if (!SendChannelData_MPI_General(data->inSignal[0], work->offset,
		  work->numChannels, work->rank, MASTER_DATA_TAG,
		  MASTER_END_CHANNEL_TAG))
			ok = FALSE;
	}
	for (i = 0; i < master1Ptr->numWorkers; i++) {
		work = &master1Ptr->workDetails[i];
		MPI_Send(&ok, 1, MPI_UNSIGNED, work->rank, MASTER_END_SIGNAL_TAG,
		  MPI_COMM_WORLD);
	}
	return(ok);

}

/****************************** DriveWorkers **********************************/

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
 * The work channel receive offset (the initialisation of the work->chanCount)
 * must be adjusted for the number of channels comming in.
 */

BOOLN
DriveWorkers_MPI_Master1(EarObjectPtr data)
{
	static const WChar	*funcName = wxT("RunModel_MPI_Master1");
	BOOLN	ok;
	int		i, control, response, responseCount, packageLength;
	int		workIndex;
	WorkDetailPtr	work;
	MPI_Status		status;
	MPI_Request		request;

	if (!CheckPars_MPI_Master1())
		return(FALSE);
	if (!CheckData_MPI_Master1(data)) {
		NotifyError(wxT("%s: Process data invalid."), funcName);
		return(FALSE);
	}
	SetProcessName_EarObject(data, wxT("MPI Parallel processing module version "
	  "I"));
	if (!InitProcessVariables_MPI_Master1(data)) {
		NotifyError(wxT("%s: Could not initialise the process variables."),
		  funcName);
		return(FALSE);
	}
	if (!SendInSignalToWorkers_MPI_Master1(data)) {
		NotifyError(wxT("%s: Could not send input signal to workers."),
		  funcName);
		return(FALSE);
	}
	if (!SendControlToWorkers_MPI_Master1(data, master1Ptr->numWorkers,
	  RUN_SIMULATION))
		return(FALSE);
	if (!SendControlToWorkers_MPI_Master1(data, 1, REQUEST_SIGNAL_PARS))
		return(FALSE);
	for (i = 0, ok = TRUE; i < master1Ptr->numWorkers; i++) {
		work = &master1Ptr->workDetails[i];
		work->chanCount = work->offset * data->outSignal->numChannels / 
		  master1Ptr->numChannels;
		master1Ptr->dataPtr[i] = data->outSignal->channel[work->chanCount];
	}
	for (i = 0, ok = TRUE; i < master1Ptr->numWorkers; i++) {
		control = REQUEST_CHANNEL_DATA;
		MPI_Send(&control, 1, MPI_INT, master1Ptr->workDetails[i].rank,
		  MASTER_MESG_TAG, MPI_COMM_WORLD);
		MPI_Send(&data->handle, 1, MPI_UNSIGNED,
		 master1Ptr->workDetails[i].rank, MASTER_MESG_TAG, MPI_COMM_WORLD);
	}
	for (responseCount = 0; responseCount < master1Ptr->numWorkers; ) {
		MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
		workIndex = FindWorkDetail_MPI_Master1(status.MPI_SOURCE);
		work = &master1Ptr->workDetails[workIndex];
		switch (status.MPI_TAG) {
		case WORKER_DATA_TAG:
			MPI_Get_count(&status, MPI_DOUBLE, &packageLength);
			if (data->outSignal->length < work->sampleCount + packageLength) {
				NotifyError(wxT("%s: Worker channel length is longer than the "
				  "initialised length for this process."), funcName);
				return(FALSE);
			}
			MPI_Irecv(master1Ptr->dataPtr[workIndex], packageLength, MPI_DOUBLE,
			  status.MPI_SOURCE, WORKER_DATA_TAG, MPI_COMM_WORLD, &request);
			/*** Do something useful here? ***/
			work->sampleCount += packageLength;
			MPI_Wait(&request, NULL);
			master1Ptr->dataPtr[workIndex] += packageLength;
			break;
		case WORKER_END_CHANNEL_TAG:
			MPI_Recv(&response, 1, MPI_INT, status.MPI_SOURCE,
			  WORKER_END_CHANNEL_TAG, MPI_COMM_WORLD, &status);
			master1Ptr->dataPtr[workIndex] = data->outSignal->channel[
			  ++work->chanCount];
			if (data->outSignal->length != work->sampleCount)
				NotifyWarning(wxT("%s: Worker channel length is shorter than "
				  "the initialised\nlength for this process (%u)."), funcName,
				  data->outSignal->length);
			work->sampleCount = 0;
			break;
		case WORKER_END_SIGNAL_TAG:
			MPI_Recv(&response, 1, MPI_INT, status.MPI_SOURCE,
			  WORKER_END_SIGNAL_TAG, MPI_COMM_WORLD, &status);
			responseCount++;
			break;
		default:
			NotifyError(wxT("%s: Unknown tag received (%d)."), funcName,
			  status.MPI_TAG);
			return(FALSE);
		} /* switch */
	}
	SetProcessContinuity_EarObject(data);
	return(ok);

}
