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

/******************************************************************************/
/*************************** Enum definitions *********************************/
/******************************************************************************/

enum {

	MYFRAME_ID_QUIT = 1,
	MYFRAME_ID_ABOUT,
	MYFRAME_ID_EXECUTE,
	MYFRAME_ID_HELP,
	MYFRAME_ID_EDIT_MAIN_PARS,
	MYFRAME_ID_EDIT_SIM_PARS,
	MYFRAME_ID_SAVE_SIM_PARS,
	MYFRAME_ID_VIEW_SIM_PARS

};

enum {

	MYAPP_SOCKET_ID = 1,
	MYAPP_SERVER_ID

};

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Class definitions ********************************/
/******************************************************************************/

/*************************** MyFrame ******************************************/

// Define a new frame
class MyFrame: public wxFrame {

#	ifdef MPI_SUPPORT
	char	**initStringPtrs;
#	endif
	int		helpCount;
	wxMenu		*fileMenu, *editMenu, *viewMenu;
	wxConfigBase	*pConfig;

  public:
    wxPanel		*panel;
    wxTextCtrl	*diagnosticsWindow;

    MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
    ~MyFrame(void);

	void	EnableSimParMenuOptions(bool on);
	void	AddHelpBook(const wxString& path, const wxString& defaultPath,
			  const wxString& fileName);
	void	OnAbout(wxCommandEvent& event);
	void	OnCloseWindow(wxCloseEvent& event);
	void	OnExecute(wxCommandEvent& event);
	void	OnHelp(wxCommandEvent& event);
	void	OnEditMainPars(wxCommandEvent& event);
	void	OnEditSimPars(wxCommandEvent& event);
	void	OnQuit(wxCommandEvent& event);
	void	OnSaveSimPars(wxCommandEvent& event);
	void	OnViewSimPars(wxCommandEvent& event);
	void	OnSize(wxSizeEvent& event);

   private:
      wxHtmlHelpController help;

	DECLARE_EVENT_TABLE()

};

/*************************** MyApp ********************************************/

// Define a new application
class MyApp: public wxApp {

	bool	clientServerFlag, busy;
	uInt	serverId;
	int		displayDefaultX, displayDefaultY, myArgc;
	char	**myArgv;
	wxString	serverName;
	MyFrame		*frame;
	wxSocketServer	*myServer;
	wxSocketClient	*myClient;

  public:

	MyApp(void);
	
	void	CheckOptions(void);
	void	GetDefaultDisplayPos(int *x, int *y);
	MyFrame *	GetFrame(void)	{ return frame; }
	bool	InitArgv(int argc);
	bool	OnInit(void);
    int		OnExit(void);
	void	OnServerEvent(wxSocketEvent& event);
	void	OnSocketEvent(wxSocketEvent& event);
	void	PrintUsage(void);
	void	ResetDefaultDisplayPos(void)
			  { displayDefaultX = 0; displayDefaultY = 0; }
	void	RunInClientMode(void);
	bool	RunSimulation(void);
	bool	SetArgvString(int index, char *string, int size);
	bool	SetClientServerMode(void);

	DECLARE_EVENT_TABLE()

};

/******************************************************************************/
/*************************** External Variables *******************************/
/******************************************************************************/

DECLARE_APP(MyApp)

extern wxList	myChildren;

#ifdef MPI_SUPPORT
	extern "C" MPI_Init(int *, char ***);
	extern "C" MPI_Finalize(void);
#endif
extern int		MainSimulation(void); /* ?? until RunSimMgr is put back. */

/******************************************************************************/
/*************************** Subroutine declarations **************************/
/******************************************************************************/

void	CreateApp(void);

/******************************************************************************/
/*************************** Call back prototypes *****************************/
/******************************************************************************/

void	ButtonProc_SimMgr(wxButton& but, wxCommandEvent& event);

#endif

