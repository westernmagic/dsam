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

/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

#define MAINAPP_PARAMETER_STR_DELIMITERS	" ,\t"

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

/*************************** MainApp ******************************************/

class MainApp {

	bool	initOk, serverFlag, superServerFlag, argsAreLocalFlag;
	char	**argv;
	int		argc;
	int		serverPort;
	int		(* ExternalMain)(void);
	int		(* ExternalRunSimulation)(void);

  public:
	SimThread	*simThread;
	wxCriticalSection	mainCritSect;

  	MainApp(int theArgc = 0, char **theArgv = NULL, int (* TheExternalMain)(
	  void) = NULL, int (* TheExternalRunSimulation)(void) = NULL);
  	virtual ~MainApp(void);

	virtual EarObjectPtr	GetSimProcess(void);
	virtual bool	InitRun(void);
	virtual int		Main(void);
	virtual	bool	RunSimulation(void);
	virtual bool	ResetSimulation(void);
	virtual void	SetRunIndicators(bool on)	{ ; }

	// has the initialization been successful? (implicit test)
	operator bool() const { return initOk; }

	bool	CheckInitialisation(void);
	void	CheckOptions(void);
	void	DeleteSimThread(void);
	void	FreeArgStrings(void);
	int		GetArgc(void)	{ return argc; }
	char **	GetArgv(void)	{ return argv; }
	int		GetServerFlag(void)	{ return(serverFlag); }
	int		GetServerPort(void)	{ return(serverPort); }
	bool	InitArgv(int theArgc);
	bool	InitMain(bool loadSimulationFlag = false);
	void	RemoveCommands(int offset, char *prefix);
	int		RunServer(void);
	void	SetInitStatus(bool status)	{ initOk = status; }
	void	StartSimThread(void);
	void	SetArgc(int theArgc)	{ argc = theArgc; }
	void	SetArgv(char **theArgv)	{ argv = theArgv; }
	bool	SetArgvString(int index, char *string, int size);
	int		SetParameterOptionArgs(int indexStart, char *parameterOptions,
			  bool countOnly);

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
	main(int argc, char **argv)
	{
		wxApp::CheckBuildOptions(wxBuildOptions());

		wxInitializer initializer;
		if (!initializer) {
			NotifyError("Failed to initialize the wxWindows library, "
			  "aborting.");
			return(1);
		}

		MainApp	mainApp(argc, argv, MainSimulation);
		return(mainApp.Main());

	}
#	endif

#endif /* defined(EXTENSIONS_SUPPORT) && !defined(GRAPHICS_SUPPORT) */

BOOLN	OnExecute_MainApp(void);
void	PrintUsage_MainApp(void);

/******************************************************************************/
/*************************** Call back prototypes *****************************/
/******************************************************************************/

#endif

