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
#	include "wxstring.h"
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
/*************************** Type definitions *********************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Class definitions ********************************/
/******************************************************************************/

/*************************** MyApp ********************************************/

// Define a new application
class MyApp: public wxApp {

	BOOLN		clientServerFlag;
	uInt		serverId;
	wxString	serverName;
	IPCOpModeSpecifier	operationMode;

  public:
	wxMenu		*fileMenu, *editMenu, *viewMenu;

	MyApp(void);

	void	CheckOptions(void);
	void	PrintUsage(void);
	wxFrame *OnInit(void);
    int		OnExit(void);
	void	RunInServerMode(void);
	void	RunInClientMode(void);
	BOOLN	RunSimulation(void);
	void	SetClientServerMode(void);
	

};

/*************************** MyTextWindow *************************************/

class MyTextWindow: public wxTextCtrl
{
  public:
	MyTextWindow(wxFrame *frame, int x=-1, int y=-1, int width=-1,
	  int height=-1, long style=0): wxTextCtrl(frame, x, y, width, height,
	  style) { DragAcceptFiles(TRUE); }

	/* void OnChar(wxKeyEvent& event); */
};

/*************************** MyFrame ******************************************/

// Define a new frame
class MyFrame: public wxFrame {

#	ifdef MPI_SUPPORT
	char	**initStringPtrs;
#	endif	

  public:
    wxPanel		*panel;
    wxTextCtrl	*diagnosticsWindow;

    MyFrame(wxFrame *frame, char *title, int x, int y, int w, int h, int type);
    ~MyFrame(void);

    Bool	OnClose(void);
 	void	OnSize(int width, int height);

};

/******************************************************************************/
/*************************** External Variables *******************************/
/******************************************************************************/

extern MyApp	myApp;
extern MyFrame	*myFrame;
extern wxList	myChildren;
extern int		childFrameType;

#ifdef MPI_SUPPORT
	extern "C" MPI_Init(int *, char ***);
	extern "C" MPI_Finalize(void);
#endif

/******************************************************************************/
/*************************** Subroutine declarations **************************/
/******************************************************************************/

void	CreateApp(void);

/******************************************************************************/
/*************************** Call back prototypes *****************************/
/******************************************************************************/

void	ButtonProc_SimMgr(wxButton& but, wxCommandEvent& event);

#endif

