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
#define	MAINAPP_NUM_BASE_ARGUMENTS			3

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
class IPCClient;

/*************************** MainApp ******************************************/

class MainApp {

	bool	initOk, serverFlag, superServerFlag, diagsOn;
	bool	threadedSimExecutionFlag;
	wxChar	**argv;
	int		argc;
	int		(* ExternalMain)(void);
	int		(* ExternalRunSimulation)(void);
	SymbolPtr	symList;
	IPCClient	*myClient;
	wxFileName	simFileName;

  public:
	wxChar	serverHost[MAXLINE];
	int		serverPort;
	SimThread	*simThread;
	DSAMXMLDocument	*doc;
	RunThreadedProc	*runThreadedProc;
	wxCriticalSection	mainCritSect;

  	MainApp(int theArgc = 0, wxChar **theArgv = NULL, int (* TheExternalMain)(
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
	bool	CreateClient(wxChar * serverHost, uShort serverPort);
	void	DeleteSimThread(void);
	void	FreeArgStrings(void);
	int		GetArgc(void)	{ return argc; }
	wxChar **	GetArgv(void)	{ return argv; }
	IPCClient *	GetClient(void)	{ return myClient; }
	int		GetServerFlag(void)	{ return(serverFlag); }
	int		GetServerPort(void)	{ return(serverPort); }
	wxFileName &	GetSimFileName(void)	{ return simFileName; }
	SymbolPtr GetSymList(void)	{ return symList; }
	bool	InitArgv(int theArgc);
	bool	InitCommandLineArgs(int theArgc, wxChar **theArgv);
	bool	InitMain(bool loadSimulationFlag = false);
	bool	LoadXMLDocument(void);
	bool	ProtectQuotedStr(wxChar *str);
	void	RemoveCommands(int offset, wxChar *prefix);
	wxChar *	RestoreQuotedStr(wxChar *str);
	int		RunIPCMode(void);
	int		RunServer(void);
	int		RunClient(void);
	void	SetArgc(int theArgc)	{ argc = theArgc; }
	void	SetArgv(wxChar **theArgv)	{ argv = theArgv; }
	bool	SetArgvString(int index, const wxChar *string, int size);
	void	SetInitStatus(bool status)	{ initOk = status; }
	int		SetParameterOptionArgs(int indexStart,
			  const wxChar *parameterOptions, bool countOnly);
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
	main(int argc, wxChar **argv)
	{
		wxInitializer initializer;
		if (!initializer) {
			NotifyError(wxT("main: Failed to initialize the wxWidgets library, ")
			  wxT("aborting."));
			return -1;
		}
		MainApp	mainApp(argc, argv, MainSimulation);
		return(mainApp.Main());

	}
#	endif

#endif /* defined(EXTENSIONS_SUPPORT) && !defined(GRAPHICS_SUPPORT) */

void	DPrintSysLog_MainApp(wxChar *format, va_list args);

void	PrintUsage_MainApp(void);

BOOLN	OnExecute_MainApp(void);

BOOLN	ReadXMLSimFile_MainApp(wxChar *fileName);

/******************************************************************************/
/*************************** Call back prototypes *****************************/
/******************************************************************************/

#endif

