/**********************
 *
 * File:		UtAppInterface.h
 * Purpose:		This Module contains the interface routines for DSAM
 *				applications.
 * Comments:	
 * Author:		L. P. O'Mard
 * Created:		15 Mar 2000
 * Updated:		
 * Copyright:	(c) 1999, University of Essex.
 *
 *********************/

#ifndef _UTAPPINTERFACE_H
#define	_UTAPPINTERFACE_H 1

#include <stdlib.h> 
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <string.h>

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define DEFAULT_ERRORS_MODE			"screen"
#define	APP_INT_NUM_PARS			4		/* Number of main parameters. */
#define	APP_MAX_AUTHORS				5

#if defined(GRAPHICS_SUPPORT) && defined(__cplusplus)
#	define	MAIN_ARGS	void
#else
#	define	MAIN_ARGS	int argc, char **argv
#endif /* GRAPHICS_SUPPORT */

/******************************************************************************/
/****************************** Bitmaps ***************************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

typedef enum {

	APP_INT_DIAGNOSTICMODE,
	APP_INT_SIMULATIONFILE,
	APP_INT_SEGMENTMODE,
	APP_INT_PARLIST

} AppInteraceParSpecifier;

typedef enum {

	APP_INT_PARAMETERS_LIST_MODE,
	APP_INT_CFLIST_LIST_MODE,
	APP_INT_LIST_NULL

} AppInterfaceListSpecifier;

typedef struct {

	ParameterSpecifier parSpec;
	
	BOOLN	simulationFileFlag, useParComsFlag, checkMainInit, listParsAndExit;
	BOOLN	listCFListAndExit, readMainParsFlag, printUsageFlag;
	BOOLN	updateProcessVariablesFlag;
	char	appName[MAXLINE];
	char	appVersion[MAXLINE];
	char	compiledDSAMVersion[MAXLINE];
	char	title[MAXLINE];
	char	authors[APP_MAX_AUTHORS][MAXLINE];
	char	simulationFile[MAX_FILE_PATH];
	char	segmentMode[SMALL_STRING];
	char	diagMode[MAX_FILE_PATH];
	char	installDir[MAX_FILE_PATH];
	char	**argv;
	int		argc;
	int		initialCommand;
	int		segmentModeSpecifier;
	int		diagModeSpecifier;
	EarObjectPtr	audModel;

	/* Private members */
	BOOLN	simParFileFlag;
	NameSpecifier	*listingModeList;
	NameSpecifier	*diagModeList;
	UniParListPtr	appParList;
	UniParListPtr	parList;
	time_t	simLastModified;
	void	(* PrintUsage)(void);
	void	(* PrintSimMgrUsage)(void);
	int		(* ProcessOptions)(int , char **, int *);
	BOOLN	(* SetInitialPars)(void);
	BOOLN	(* SetFinalPars)(void);

} AppInterface, *AppInterfacePtr;

typedef struct stat *StatPtr;

/******************************************************************************/
/*************************** Class definitions ********************************/
/******************************************************************************/

/******************************************************************************/
/*************************** External Variables *******************************/
/******************************************************************************/

extern char **argv;
extern int	argc;
extern AppInterfacePtr	appInterfacePtr;

/******************************************************************************/
/*************************** Subroutine declarations **************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

BOOLN	Free_AppInterface(void);

StatPtr	GetFileStatusPtr_AppInterface(char *fileName);

BOOLN	Init_AppInterface(ParameterSpecifier parSpec);

BOOLN	InitProcessVariables_AppInterface(int theArgc, char **theArgv);

BOOLN	InitSimulation_AppInterface(void);

void	ListCFListAndExit_AppInterface(void);

BOOLN	ListParameters_AppInterface(void);

void	ListParsAndExit_AppInterface(void);

void	PrintPars_AppInterface(void);

BOOLN	ProcessParComs_AppInterface(void);

BOOLN	ProcessOptions_AppInterface(void);

void	PrintUsage_AppInterface(void);

BOOLN	ReadPars_AppInterface(FILE *fp);

BOOLN	ReadProgParFile_AppInterface(void);

void	ResetCommandArgFlags_AppInterface(void);

BOOLN	SetAppName_AppInterface(char *appName);

BOOLN	SetAppParList_AppInterface(UniParListPtr appParList);

BOOLN	SetAppPrintUsage_AppInterface(void (* PrintUsage)(void));

BOOLN	SetAppProcessOptions_AppInterface(int (* ProcessOptions)(int, char **,
		  int *));

BOOLN	SetAppSetFinalPars_AppInterface(BOOLN (* SetFinalPars)(void));

BOOLN	SetAppSetInitialPars_AppInterface(BOOLN (* SetInitialPars)(void));

BOOLN	SetAppVersion_AppInterface(char *appVersion);

void	SetArgcAndArgV_AppInterface(int theArgc, char **theArgv);

BOOLN	SetCompiledDSAMVersion_AppInterface(char *compiledDSAMVersion);

BOOLN	SetDiagMode_AppInterface(char *theDiagMode);

BOOLN	SetInstallDir_AppInterface(char *theInstallDir);

BOOLN	SetPars_AppInterface(char *diagMode, char *simulationFile,
		  char *segmentMode);

BOOLN	SetProgramParValue_AppInterface(char *parName, char *parValue);

BOOLN	SetSegmentMode_AppInterface(char *theSegmentMode);

BOOLN	SetSimulationFile_AppInterface(char *theSimulationFile);

BOOLN	SetTitle_AppInterface(char *title);

BOOLN	SetUniParList_AppInterface(void);

BOOLN	SimulationFileChanged_AppInterface(StatPtr simSpecFileStat,
		  BOOLN updateStatus);

__END_DECLS

#endif
