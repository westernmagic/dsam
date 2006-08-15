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

#include "ExtCommon.h"
#if defined(GRAPHICS_SUPPORT) && defined(__cplusplus)
#	include <wx/textctrl.h>
#	include <wx/wxhtml.h>
#	include <wx/socket.h>
#	include <wx/docview.h>
#endif

/* sccsid[] = "%W% %G%" */

/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

#define SIM_MANAGER_DEFAULT_WIDTH		500
#define SIM_MANAGER_DEFAULT_HEIGHT		200	
#define SIM_MANAGER_HELP_DIR			wxT("HelpFiles")
#define SIM_MANAGER_REG_MAIN_FRAME		wxT("/MainFrame")
#define SIM_MANAGER_REG_PATHS			wxT("/Paths")
#define SIM_MANAGER_REG_DSAM_HELP_PATH	wxT("DSAMHelp")
#define SIM_MANAGER_REG_DSAM_HELP_BOOK	wxT("DSAMHelp")
#define SIM_MANAGER_REG_APP_HELP_PATH	wxT("Help")
#define SIM_MANAGER_REG_MAIN_PARS		wxT("/MainPars")


#define	SIM_MANAGER_DEFAULT_SERVER_NAME		wxT("DSAM IPC Server")
#define SIM_MANAGER_PROGRAM_PARS_DIALOG_TITLE	wxT("Program parameters")

/******************************************************************************/
/*************************** Enum definitions *********************************/
/******************************************************************************/

enum {

	MYFRAME_ID_QUIT = 1,
	MYFRAME_ID_EDIT_SIM_PARS,
	MYFRAME_ID_LOAD_SIM_PAR_FILE,
	MYFRAME_ID_LOAD_SIM_SCRIPT_FILE,
	MYFRAME_ID_RELOAD_SIM_SCRIPT_FILE,
	MYFRAME_ID_SAVE_SIM_PARS,
	MYFRAME_ID_VIEW_SIM_PARS,

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

class ModuleParDialog;
class EditorToolPalette;
class SDICanvas;
class SDIFrame;
class DiagFrame;
class GrMainApp;
class GrIPCServer;
class SDIDocManager;

/*************************** wxArrayDisplay ***********************************/

WX_DEFINE_ARRAY(wxFrame *, wxArrayDisplay);

/*************************** MyApp ********************************************/

// Define a new application
class MyApp: public wxApp {

	bool	audModelLoadedFlag;
	int		displayDefaultX, displayDefaultY, helpCount;
	wxString	dataInstallDir, title;
	SDIFrame	*frame;
	GrMainApp	*grMainApp;
	DiagFrame	*diagFrame;
	GrIPCServer	*iPCServer;
	wxConfigBase	*pConfig;
	SDIDocManager	*myDocManager;
	wxHtmlHelpController help;

  public:
	wxMenu	*fileMenu, *editMenu, *viewMenu, *programMenu, *windowsMenu;
	wxIcon	*icon;
	wxArrayString	anaList, ctrlList, displayList, filtList, ioList,
					  modelsList;
	wxArrayString	transList, userList, utilList;
	wxArrayDisplay	displays;

	MyApp(void);

	void	AddHelpBook(const wxString& path, const wxString& defaultPath,
			  const wxString& fileName);
	wxMenuBar	*CreateMenuBar(void);
	EditorToolPalette *CreatePalette(wxFrame *parent);
	void	CloseDiagWindow(void);
	void	CreateDocument(const wxString& fileName);
	void	CreateProcessLists(void);
	void	EnableSimParMenuOptions(bool on);
	bool	GetAudModelLoadedFlag(void)		{ return audModelLoadedFlag; }
	void	GetDefaultDisplayPos(int *x, int *y);
	DiagFrame *	GetDiagFrame(void)	{ return diagFrame; }
	GrMainApp *	GetGrMainApp(void)	{ return grMainApp; }
	SDIFrame *	GetFrame(void)	{ return frame; }
	wxHtmlHelpController * GetHelpController(void)	{ return &help; }
	wxArrayString *	GetProcessList(int classSpecifier);

	void	InitHelp(void);
	void	ExitMain(void);
	bool	OnInit(void);
	int		OnExit(void);
	void	OnServerEvent(wxSocketEvent& event);
	void	OnSocketEvent(wxSocketEvent& event);
	void	OpenDiagWindow(void);
	void	ResetDefaultDisplayPos(void)
			  { displayDefaultX = 0; displayDefaultY = 0; }
	void	SaveConfiguration(UniParListPtr	parList, const wxString&
			  processSuffix = wxT(""));
	void	SetAudModelLoadedFlag(bool status)	{ audModelLoadedFlag = status; }
	bool	InitServer(void);
	void	SetConfiguration(UniParListPtr	parList);
	void	SetDataInstallDir(wxChar *theDir)	{ dataInstallDir = theDir; }
	void	SetIcon(wxIcon *theIcon) { icon = theIcon; };

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

BOOLN	OnExecute_MyApp(void);

void	OnExit_MyApp(void);

void	DPrint_MyApp(wxChar *format, va_list args);

void	EmptyDiagWinBuffer_MyApp(wxChar *s, int *c);

void	Notify_MyApp(const wxChar *message, CommonDiagSpecifier type);

/******************************************************************************/
/*************************** Call back prototypes *****************************/
/******************************************************************************/

#endif

