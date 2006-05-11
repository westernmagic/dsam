/**********************
 *
 * File:		ExtMainApp.h
 * Purpose:		This routine contains the main handling code running simulations
 *				using the extensions library.
 * Comments:	This routine contains the main routine for when the non-GUI mode
 *				is used.
 * Author:		L. P. O'Mard
 * Created:		12 Dec 2003
 * Updated:		
 * Copyright:	(c) 2003 CNBH, University of Essex
 *
 **********************/
 
#ifndef _EXTMAINAPP
#define _EXTMAINAPP 1

/* sccsid[] = "%W% %G%" */

#include <wx/filename.h>
#include "UtSSSymbols.h"

/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

#define MAINAPP_PARAMETER_STR_DELIMITERS	wxT(" ,\t")
#define MAINAPP_QUOTE						'"'
#define MAINAPP_SPACE_SUBST					'\1'

/******************************************************************************/
/*************************** Enum definitions *********************************/
/******************************************************************************/
/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Class definitions ********************************/
/******************************************************************************/

/********************************** Pre-references ****************************/

class SimThread;
class RunThreadedProc;
class DSAMXMLDocument;

/*************************** MainApp ******************************************/

class MainApp {

	bool	initOk, serverFlag, superServerFlag, argsAreLocalFlag, diagsOn;
	bool	threadedSimExecutionFlag;
	WChar	**argv;
	int		argc;
	int		serverPort;
	int		(* ExternalMain)(void);
	int		(* ExternalRunSimulation)(void);
	SymbolPtr	symList;
	wxFileName	simFileName;

  public:
	SimThread	*simThread;
	DSAMXMLDocument	*doc;
	RunThreadedProc	*runThreadedProc;
	wxCriticalSection	mainCritSect;

  	MainApp(int theArgc = 0, WChar **theArgv = NULL, int (* TheExternalMain)(
	  void) = NULL, int (* TheExternalRunSimulation)(void) = NULL);
  	virtual ~MainApp(void);

	virtual void	InitXMLDocument(void);
	virtual EarObjectPtr	GetSimProcess(void);
	virtual bool	InitRun(void);
	virtual int		Main(void);
	virtual	bool	RunSimulation(void);
	virtual bool	ResetSimulation(void);
	virtual void	SetRunIndicators(bool on)	{ on = true; }

	// has the initialization been successful? (implicit test)
	operator bool() const { return initOk; }

	bool	CheckInitialisation(void);
	void	CheckOptions(void);
	void	DeleteSimThread(void);
	void	FreeArgStrings(void);
	int		GetArgc(void)	{ return argc; }
	WChar **	GetArgv(void)	{ return argv; }
	int		GetServerFlag(void)	{ return(serverFlag); }
	int		GetServerPort(void)	{ return(serverPort); }
	wxFileName &	GetSimFileName(void)	{ return simFileName; }
	SymbolPtr GetSymList(void)	{ return symList; }
	bool	InitArgv(int theArgc);
	bool	InitMain(bool loadSimulationFlag = false);
	bool	LoadXMLDocument(void);
	bool	ProtectQuotedStr(WChar *str);
	void	RemoveCommands(int offset, WChar *prefix);
	WChar *	RestoreQuotedStr(WChar *str);
	int		RunServer(void);
	void	SetArgc(int theArgc)	{ argc = theArgc; }
	void	SetArgv(WChar **theArgv)	{ argv = theArgv; }
	bool	SetArgvString(int index, WChar *string, int size);
	void	SetInitStatus(bool status)	{ initOk = status; }
	int		SetParameterOptionArgs(int indexStart, WChar *parameterOptions,
			  bool countOnly);
	void	SetSimulationFile(wxFileName &fileName);
	void	SetThreadPars(void);
	void	StartSimThread(wxThreadKind kind = wxTHREAD_DETACHED);

};

/******************************************************************************/
/*************************** External Variables *******************************/
/******************************************************************************/

extern int	MainSimulation(void);
extern MainApp	*dSAMMainApp;

/******************************************************************************/
/*************************** Subroutine declarations **************************/
/******************************************************************************/

#if defined(EXTENSIONS_SUPPORT) && !defined(USE_GUI)
#	if !defined(LIBRARY_COMPILE)
	int
	main(int argc, WChar **argv)
	{
		wxInitializer initializer;
		if (!initializer) {
			NotifyError(wxT("main: Failed to initialize the wxWidgets library, "
			  "aborting."));
			return -1;
		}
		MainApp	mainApp(argc, argv, MainSimulation);
		return(mainApp.Main());

	}
#	endif

#endif /* defined(EXTENSIONS_SUPPORT) && !defined(GRAPHICS_SUPPORT) */

void	DPrintSysLog_MainApp(WChar *format, va_list args);

void	PrintUsage_MainApp(void);

BOOLN	OnExecute_MainApp(void);

BOOLN	ReadXMLSimFile_MainApp(WChar *fileName);

/******************************************************************************/
/*************************** Call back prototypes *****************************/
/******************************************************************************/

#endif

