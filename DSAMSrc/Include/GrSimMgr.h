/**********************
 *
 * File:		GrSimMgr.h
 * Purpose:		Graphical Display Simulation Manager
 * Comments:	23-06-98 LPO: Introduced manager window resizing:
 *				MyFrame::OnSize
 *				13-09-98: LPO Added Server mode so that it is no longer
 *				necessary to press the "Go" button.
 *				28-09-98: LPO: Moved the textWindow declaration to above the
 *				menu bar, so that diagnostics can be sent to the text window
 *				when the menu bar is being set up.  Otherwise the program will
 *				crash.
 *				30-09-98 LPO: Some headers appear both in the source file and
 *				header file.  This is because they are needed in the header
 *				file for compilation, but automake needs them in the source
 *				file for generating dependancy information.
 *				05-01-99 LPO: Implemented "MyApp::RunSimulation" routine.
 *				29-01-99 LPO: The 'RunSimulation' routine now always uses the
 *				'ResetGUIDialogs()' routine.  After the first unless this
 *				routine is used all other messages are sent to the console.
 *				16-04-99 LPO: Moved the 'myApp' declaration to DSAM.h to solve
 *				some linking problems with shared libraries on IRIX 6.2.
 * Author:		L. P. O'Mard
 * Created:		06 Jan 1995
 * Updated:		16 Apr 1999
 * Copyright:	(c) 1999, University of Essex
 *
 **********************/
 
#ifndef _GRSIMMGR_H
#define _GRSIMMGR_H 1

#if defined(GRAPHICS_SUPPORT) && defined(__cplusplus)
#	include <wx/textctrl.h>
#	include <wx/socket.h>
#	include <wx/wxhtml.h>
#endif

/* sccsid[] = "%W% %G%" */

/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

#define	DEFAULT_SERVER_NAME				"110773"
#define SIM_MANAGER_HELP_DIR			"HelpFiles"
#define SIM_MANAGER_REG_MAIN_FRAME		"/MainFrame"
#define SIM_MANAGER_REG_PATHS			"/Paths"
#define SIM_MANAGER_REG_DSAM_HELP_PATH	"DSAMHelp"
#define SIM_MANAGER_REG_APP_HELP_PATH	"Help"

#define SIM_MANAGER_PROGRAM_PARS_DIALOG_TITLE	"Program parameters"
#define SIM_MANAGER_REG_MAIN_PARS				"/MainPars"

/******************************************************************************/
/*************************** Enum definitions *********************************/
/******************************************************************************/

enum {

	MYFRAME_ID_QUIT = 1,
	MYFRAME_ID_ABOUT,
	MYFRAME_ID_EXECUTE,
	MYFRAME_ID_STOP_SIMULATION,
	MYFRAME_ID_HELP,
	MYFRAME_ID_EDIT_MAIN_PARS,
	MYFRAME_ID_EDIT_SIM_PARS,
	MYFRAME_ID_LOAD_SIM_PAR_FILE,
	MYFRAME_ID_LOAD_SIM_SCRIPT_FILE,
	MYFRAME_ID_RELOAD_SIM_SCRIPT_FILE,
	MYFRAME_ID_SAVE_SIM_PARS,
	MYFRAME_ID_VIEW_SIM_PARS,
	MYFRAME_ID_SIM_THREAD_DISPLAY_EVENT = 100

};

enum {

	MYAPP_SOCKET_ID = 1,
	MYAPP_SERVER_ID

};

enum {

	MYAPP_THREAD_DRAW_GRAPH = 1

};

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Class definitions ********************************/
/******************************************************************************/

/********************************** Pre-references ****************************/

class SimModuleDialog;
class ModuleParDialog;

/*************************** MyFrame ******************************************/

// Define a new frame
class MyFrame: public wxFrame {

#	ifdef MPI_SUPPORT
	char	**initStringPtrs;
#	endif
	int		helpCount;
	wxMenu	*fileMenu, *editMenu, *viewMenu, *programMenu;
	wxConfigBase	*pConfig;
	ModuleParDialog	*mainParDialog;

  public:
    wxPanel		*panel;
    wxTextCtrl	*diagnosticsWindow;

    MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
    ~MyFrame(void);

	void	EnableSimParMenuOptions(bool on);
	wxMenu *	GetProgramMenu(void)	{ return programMenu; }
	void	AddHelpBook(const wxString& path, const wxString& defaultPath,
			  const wxString& fileName);
	void	DeleteMainParDialog(void);
	void	OnAbout(wxCommandEvent& event);
	void	OnCloseWindow(wxCloseEvent& event);
	void	OnExecute(wxCommandEvent& event);
	void	OnHelp(wxCommandEvent& event);
	void	OnEditMainPars(wxCommandEvent& event);
	void	OnEditSimPars(wxCommandEvent& event);
	void	OnLoadSimFile(wxCommandEvent& event);
	void	OnReloadSimFile(wxCommandEvent& event);
	void	OnQuit(wxCommandEvent& event);
	void	OnSaveSimPars(wxCommandEvent& event);
	void	OnSimThreadEvent(wxCommandEvent& event);
	void	OnStopSimulation(wxCommandEvent& event);
	void	OnViewSimPars(wxCommandEvent& event);
	void	OnSize(wxSizeEvent& event);
	bool	ResetSimulation(void);
	void	SetMainParDialog(ModuleParDialog *dlg) { mainParDialog = dlg; }
	void	UpdateMainParDialog(void);

   private:
      wxHtmlHelpController help;

	DECLARE_EVENT_TABLE()

};

/*************************** SimThread pre-reference **************************/

class	SimThread;

/*************************** wxArrayDisplay ***********************************/

WX_DEFINE_ARRAY(wxFrame *, wxArrayDisplay);

/*************************** MyApp ********************************************/

// Define a new application
class MyApp: public wxApp {

	bool	clientServerFlag, busy;
	uInt	serverId;
	int		displayDefaultX, displayDefaultY, myArgc;
	char	**myArgv;
	wxString	serverName, dataInstallDir, title;
	MyFrame		*frame;
	wxSocketServer	*myServer;
	wxSocketClient	*myClient;
	SimModuleDialog	*simModuleDialog;

  public:
	wxIcon		*icon;
	SimThread	*simThread;
	wxArrayDisplay	displays;
	wxCriticalSection	mainCritSect;

	MyApp(void);

	bool	CheckInitialisation(void);
	void	CheckOptions(void);
	void	DeleteSimModuleDialog(void);
	void	DeleteSimThread(void);
	void	GetDefaultDisplayPos(int *x, int *y);
	MyFrame *	GetFrame(void)	{ return frame; }
	SimModuleDialog *	GetSimModuleDialog(void)	{ return simModuleDialog; }
	bool	GetSuspendDiagnostics(void);
	bool	InitArgv(int argc);
	void	InitMain(void);
	bool	InitRun(void);
	void	ExitMain(void);
	bool	OnInit(void);
	int		OnExit(void);
	void	OnServerEvent(wxSocketEvent& event);
	void	OnSocketEvent(wxSocketEvent& event);
	bool	ResetCommandArgs(void);
	void	ResetDefaultDisplayPos(void)
			  { displayDefaultX = 0; displayDefaultY = 0; }
	void	RunInClientMode(void);
	void	SaveConfiguration(UniParListPtr	parList);
	bool	SetArgvString(int index, char *string, int size);
	bool	SetClientServerMode(void);
	void	SetConfiguration(UniParListPtr	parList);
	void	SetDataInstallDir(char *theDir)	{ dataInstallDir = theDir; }
	void	SetDiagLocks(bool on);
	void	SetIcon(wxIcon *theIcon) { icon = theIcon; };
	void	SetSimModuleDialog(SimModuleDialog *dlg) { simModuleDialog = dlg; }
	void	SetTitle(void);
	void	StartSimThread(void);
	bool	StatusChanged(void);

	DECLARE_EVENT_TABLE()

};

/******************************************************************************/
/*************************** External Variables *******************************/
/******************************************************************************/

DECLARE_APP(MyApp)

#ifdef MPI_SUPPORT
	extern "C" MPI_Init(int *, char ***);
	extern "C" MPI_Finalize(void);
#endif
extern int		MainSimulation(void); /* ?? until RunSimMgr is put back. */

/******************************************************************************/
/*************************** Subroutine declarations **************************/
/******************************************************************************/

void	CreateApp(void);

void	PrintUsage_MyApp(void);

/******************************************************************************/
/*************************** Call back prototypes *****************************/
/******************************************************************************/

#endif

