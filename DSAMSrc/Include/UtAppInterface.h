/**********************
 *
 * File:		UtAppInterface.h
 * Purpose:		This Module contains the interface routines for DSAM
 *				applications.
 * Comments:	The 'PostInitFunc' routine is run at the end of the
 *				'InitProcessVariables' routine.
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
#define	APP_MAX_HELP_BOOKS			4

#ifdef USE_GUI
#	define	MAIN_ARGS	void
#	define ARGC		0
#	define ARGV		NULL
#else
#	define	MAIN_ARGS	int argc, char **argv
#	define ARGC		argc
#	define ARGV		argv
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
	BOOLN	listCFListAndExit, readAppParFileFlag, printUsageFlag;
	BOOLN	appParFileFlag;
	BOOLN	updateProcessVariablesFlag;
	char	appName[MAXLINE];
	char	appHelpBooks[APP_MAX_HELP_BOOKS][MAXLINE];
	char	appParFile[MAX_FILE_PATH];
	char	appVersion[MAXLINE];
	char	compiledDSAMVersion[MAXLINE];
	char	title[MAX_FILE_PATH];
	char	authors[APP_MAX_AUTHORS][MAXLINE];
	char	simulationFile[MAX_FILE_PATH];
	char	segmentMode[SMALL_STRING];
	char	diagMode[MAX_FILE_PATH];
	char	installDir[MAX_FILE_PATH];
	char	simFileName[MAX_FILE_PATH];
	char	parsFilePath[MAX_FILE_PATH];
	char	**argv;
	int		argc;
	int		initialCommand;
	int		segmentModeSpecifier;
	int		diagModeSpecifier;
	int		maxUserModules;
	int		numHelpBooks;
	EarObjectPtr	audModel;

	/* Private members */
	BOOLN	canFreePtrFlag;
	NameSpecifier	*listingModeList;
	NameSpecifier	*diagModeList;
	UniParListPtr	appParList;
	UniParListPtr	parList;
	int		simFileType;
	BOOLN	(* FreeAppProcessVars)(void);
	BOOLN	(* Init)(void);
	void	(* PrintUsage)(void);
	void	(* PrintSimMgrUsage)(void);
	int		(* ProcessOptions)(int , char **, int *);
	BOOLN	(* RegisterUserModules)(void);
	BOOLN	(* PostInitFunc)(void);
	BOOLN	(* SetUniParList)(UniParListPtr *);

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

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

extern AppInterfacePtr	appInterfacePtr;

__END_DECLS

/******************************************************************************/
/*************************** Subroutine declarations **************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

BOOLN	AddAppHelpBook_AppInterface(const char *bookName);

BOOLN	Free_AppInterface(void);

AppInterfacePtr	GetPtr_AppInterface(void);

EarObjectPtr	GetEarObjectPtr_AppInterface(void);

char *	GetFilePath_AppInterface(char *filePath);

DatumPtr *	GetSimPtr_AppInterface(void);

DatumPtr	GetSimulation_AppInterface(void);

UniParPtr	GetUniParPtr_AppInterface(char *parName);

BOOLN	Init_AppInterface(ParameterSpecifier parSpec);

BOOLN	InitProcessVariables_AppInterface(BOOLN (* Init)(void), int theArgc,
		  char **theArgv);

BOOLN	InitSimulation_AppInterface(void);

void	ListCFListAndExit_AppInterface(void);

BOOLN	ListParameters_AppInterface(void);

void	ListParsAndExit_AppInterface(void);

void	PrintPars_AppInterface(void);

BOOLN	ProcessParComs_AppInterface(void);

BOOLN	ProcessOptions_AppInterface(void);

void	PrintUsage_AppInterface(void);

BOOLN	ReadPars_AppInterface(char *parFileName);

BOOLN	ReadProgParFile_AppInterface(void);

void	ResetCommandArgFlags_AppInterface(void);

void	ResetSimulation_AppInterface(void);

BOOLN	SetAppParFile_AppInterface(char *fileName);

BOOLN	SetAppName_AppInterface(char *appName);

BOOLN	SetAppPrintUsage_AppInterface(void (* PrintUsage)(void));

BOOLN	SetAppProcessOptions_AppInterface(int (* ProcessOptions)(int, char **,
		  int *));

BOOLN	SetAppRegisterUserModules_AppInterface(BOOLN (* RegisterUserModules)(
		  void));

BOOLN	SetAppPostInitFunc_AppInterface(BOOLN (* PostInitFunc)(void));

BOOLN	SetAppSetUniParList_AppInterface(BOOLN (* SetUniParList)(UniParListPtr
		  *parList));

BOOLN	SetAppVersion_AppInterface(char *appVersion);

void	SetArgcAndArgV_AppInterface(int theArgc, char **theArgv);

BOOLN	SetCanFreePtrFlag_AppInterface(BOOLN status);

BOOLN	SetCompiledDSAMVersion_AppInterface(char *compiledDSAMVersion);

BOOLN	SetDiagMode_AppInterface(char *theDiagMode);

BOOLN	SetAppFreeProcessVars_AppInterface(BOOLN (* FreeAppProcessVars)(void));

BOOLN	SetInstallDir_AppInterface(char *theInstallDir);

BOOLN	SetMaxUserModules_AppInterface(int maxUserModules);

BOOLN	SetParsFilePath_AppInterface(char * parsFilePath);

BOOLN	SetProgramParValue_AppInterface(char *parName, char *parValue);

BOOLN	SetSegmentMode_AppInterface(char *theSegmentMode);

BOOLN	SetSimFileName_AppInterface(char * simFileName);

BOOLN	SetSimFileType_AppInterface(int simFileType);

BOOLN	SetSimulationFile_AppInterface(char *theSimulationFile);

BOOLN	SetTitle_AppInterface(char *title);

BOOLN	SetUniParList_AppInterface(void);

__END_DECLS

#endif
