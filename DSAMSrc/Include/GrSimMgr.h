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
#endif

#ifdef wx_mac
#include "wx_text.h"
// class wxPanel;
// class wxDC;
// class wxButton;
// class wxCheckBox;
// class wxListBox;
// class wxTextCtrl;
// class wxMenu;
// class wxCommandEvent;
#endif // wx_mac

#include "GrIPC.h"

/* sccsid[] = "%W% %G%" */

/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

/*************************** Menu Commands ************************************/

/*************************** File Menu ****************************************/

#define SIM_MANAGER_FILE_SAVE_SIM_PARS	100
#define SIM_MANAGER_FILE_EXIT			101

/*************************** Edit Menu ****************************************/

#define SIM_MANAGER_EDIT_SIM_PARS		200
#define SIM_MANAGER_EDIT_MAIN_PARS		201

/*************************** View Menu ****************************************/

#define SIM_MANAGER_VIEW_SIM_PARS		300

/*************************** Program Menu *************************************/

#define SIM_MANAGER_PROGRAM_EXECUTE			400

/*************************** Help Menu ****************************************/

#define SIM_MANAGER_HELP_MANUAL			600
#define SIM_MANAGER_HELP_KEYWORD		601

/*************************** Misc *********************************************/


/*************************** Button Tags **************************************/

#define	SIM_MANAGER_EXIT_BUTTON_TAG		100
#define	SIM_MANAGER_GO_BUTTON_TAG		101

/*************************** Misc *********************************************/

#define	SIM_MANAGER_HELP_TOP_LEVEL			"DSAMHelp"

/******************************************************************************/
/*************************** Enum definitions *********************************/
/******************************************************************************/

enum {

	MYFRAME_ID_QUIT = 1,
	MYFRAME_ID_EXECUTE,
	MYFRAME_ID_HELP,
	MYFRAME_ID_EDIT_MAIN_PARS,
	MYFRAME_ID_EDIT_SIM_PARS,
	MYFRAME_ID_SAVE_SIM_PARS,
	MYFRAME_ID_VIEW_SIM_PARS

};

/******************************************************************************/
/*************************** Class definitions ********************************/
/******************************************************************************/

/*************************** MyFrame ******************************************/

// Define a new frame
class MyFrame: public wxFrame {

#	ifdef MPI_SUPPORT
	char	**initStringPtrs;
#	endif	
	wxMenu		*fileMenu, *editMenu, *viewMenu;

  public:
    wxPanel		*panel;
    wxTextCtrl	*diagnosticsWindow;

    MyFrame(wxFrame *frame, const wxString &title, const wxPoint& pos,
	  const wxSize& size);
    ~MyFrame(void);

	void	EnableSimParMenuOptions(bool on);
    bool	OnCloseWindow(wxCloseEvent& event);
	void	OnExecute(wxCommandEvent& event);
	void	OnQuit(wxCommandEvent& event);
	void	OnHelp(wxCommandEvent& event);
	void	OnEditMainPars(wxCommandEvent& event);
	void	OnEditSimPars(wxCommandEvent& event);
	void	OnSaveSimPars(wxCommandEvent& event);
	void	OnViewSimPars(wxCommandEvent& event);
	void	OnSize(wxSizeEvent& event);

	DECLARE_EVENT_TABLE()

};

/*************************** MyApp ********************************************/

// Define a new application
class MyApp: public wxApp {

	bool		clientServerFlag;
	uInt		serverId;
	wxString	serverName;
	MyFrame		*frame;
	IPCOpModeSpecifier	operationMode;

  public:

	MyApp(void);

	void	CheckOptions(void);
	MyFrame *	GetFrame(void)	{ return frame; }
	bool	OnInit(void);
    int		OnExit(void);
	void	PrintUsage(void);
	void	RunInServerMode(void);
	void	RunInClientMode(void);
	BOOLN	RunSimulation(void);
	void	SetClientServerMode(void);
	

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

