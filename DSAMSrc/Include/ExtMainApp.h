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

#define EXTMAINAPP_DEFAULT_SERVER_PORT		3000

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

	bool	serverFlag;
	char	**argv;
	int		argc;
	int		serverPort;

  public:
	SimThread	*simThread;
	wxCriticalSection	mainCritSect;

  	MainApp(int theArgc, char **theArgv);
  	virtual ~MainApp(void);

	virtual bool	ResetSimulation(void);
	virtual void	SetRunIndicators(bool on)	{ ; }

	bool	CheckInitialisation(void);
	void	CheckOptions(void);
	void	DeleteSimThread(void);
	int		GetServerFlag(void)	{ return(serverFlag); }
	int		GetServerPort(void)	{ return(serverPort); }
	bool	InitMain(bool loadSimulationFlag = false);
	bool	InitRun(void);
	void	StartSimThread(void);
	int		Main(void);

};

/******************************************************************************/
/*************************** External Variables *******************************/
/******************************************************************************/

extern int		MainSimulation(void);
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
			exit(1);
		}

		MainApp	mainApp(argc, argv);
		exit(mainApp.Main());

	}
#	endif

#endif /* defined(EXTENSIONS_SUPPORT) && !defined(GRAPHICS_SUPPORT) */

BOOLN	OnExecute_MainApp(void);
void	PrintUsage_MainApp(void);

/******************************************************************************/
/*************************** Call back prototypes *****************************/
/******************************************************************************/

#endif

