/**********************
 *
 * File:		GrSimMgr.cpp
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

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG_H */

#include <stdio.h>
#include <stdlib.h>

#define DSAM_DO_NOT_DEFINE_MYAPP	1

#include "DSAM.h"
#include "UtSSParser.h"
#include <wx/image.h>
#include <wx/config.h>
#include <wx/thread.h>
#include <wx/dynarray.h>

#if !wxUSE_CONSTRAINTS
#error "You must set wxUSE_CONSTRAINTS to 1 in setup.h!"
#endif

#if !wxUSE_THREADS
    #error "You must enable thread support!"
#endif // wxUSE_THREADS


/******************************************************************************/
/****************************** Bitmaps ***************************************/
/******************************************************************************/

#if defined(__WXGTK__) || defined(__WXMOTIF__)
#include "dsam.xpm"
#endif

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

int		grSimMgrPleaseLink = 0;

/* Create a new application object: this macro will allow wxWindows to
 * create the application object during program execution (it's better than
 * using a static object for many reasons) and also declares the accessor
 * function wxGetApp() which will return the reference of the right type
 * (i.e. MyApp and not wxApp)
 */
IMPLEMENT_APP(MyApp)

#ifdef wx_mac
void CreateApp(void)
{
}
#endif // wx_mac

/******************************************************************************/
/****************************** Methods (Subroutines) *************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Event tables *************************************/
/******************************************************************************/

BEGIN_EVENT_TABLE(MyApp, wxApp)
	EVT_SOCKET(MYAPP_SERVER_ID,	MyApp::OnServerEvent)
	EVT_SOCKET(MYAPP_SOCKET_ID,	MyApp::OnSocketEvent)
END_EVENT_TABLE()

/******************************************************************************/
/****************************** MyApp Methods *********************************/
/******************************************************************************/

/****************************** Constructor ***********************************/

/*
 * "Testing of ressources"
 * I'm not quite sure why this is here, but it is in the example hello.cpp, so
 * I have put it in.
 */

MyApp::MyApp(void)
{
	GetDSAMPtr_Common()->usingGUIFlag = TRUE;
	serverId = 0;
	clientServerFlag = FALSE;
	displayDefaultX = 0;
	displayDefaultY = 0;
	myArgc = 0;
	myArgv = NULL;
	frame = NULL;
	myServer = NULL;
	myClient = NULL;
	dataInstallDir = DSAM_DATA_INSTALL_DIR;
	icon = NULL;
	busy = FALSE;
	simModuleDialog = NULL;
	simThread = NULL;

}

/****************************** OnExit ****************************************/

/*.
 */

int
MyApp::OnExit(void)
{
	int		i;

	delete printData;

	if (myServer)
		myServer->Destroy();
	if (myClient)
		myClient->Destroy();

	if (myArgv && (myArgv != argv)) {
		for (i = 0; i < myArgc; i++)
			if (myArgv)
				free(myArgv[i]);
		free(myArgv);
	}
	delete wxConfigBase::Set((wxConfigBase *) NULL);
	return(0);

}

/****************************** OnInit ****************************************/

/*
 * The `main program' equivalent, creating the windows and returning the
 * main frame.
 * The first call to 'ResetGUIDialogs' ensures that the initial diagnostics
 * are sent to the GUI support routines.
 */

bool
MyApp::OnInit(void)
{
	/* static const char *funcName = "MyApp::OnInit"; */

	// Setup the printing global variables.
    printData = new wxPrintData;
#	if defined(__WXGTK__) || defined(__WXMOTIF__)
		*printData = *wxThePrintSetupData;
#	endif

	ResetGUIDialogs();
	SwitchGUILocking_Common(FALSE);

	myArgc = argc;
	myArgv = argv;

	// Check call options
	CheckOptions();
	if (clientServerFlag) {
		if (!SetClientServerMode())
			return(FALSE);
	}
	
	if (myClient) {
		RunInClientMode();
		exit(0);
	}

	// Set brushes
	greyBrushes = new GrBrushes();
	
	// Set fonts
	Init_Fonts();

	// Image handlers
	wxInitAllImageHandlers();

	// Get config setup
	SetVendorName("CNBH");

	wxConfigBase *pConfig = wxConfigBase::Get();
	pConfig = pConfig;		// Use, so compiler does not complain.

	// Create the main frame window
	frame = new MyFrame("Simulation Manager", wxPoint(400, 0), wxSize(500,
	  200));

	// Ensure title it set after the frame is created.
	SetTitle();

	if (GetPtr_AppInterface())
		SetAppName(GetPtr_AppInterface()->appName);
	else
		SetAppName("DSAM_App");

    // Show frame and tell the application that it's our main window

	frame->Show(TRUE);
    SetTopWindow(frame);

	if (myServer)
		StartSimThread();
	// Essential - return the main frame window
	return(TRUE);

}

/****************************** SetClientServerMode ***************************/

/*
 * This routine sets up the general parameters for the client/server mode.
 */

bool
MyApp::SetClientServerMode(void)
{
	static const char *funcName = "MyApp::SetClientServerMode";
	char	userId[MAXLINE];
	wxIPV4address	addr;

	if (wxGetUserId(userId, MAXLINE))
		serverName.sprintf("%u", Hash_Utils(userId) + serverId);
	else
		serverName = DEFAULT_SERVER_NAME;
	addr.Hostname("localhost");
	addr.Service(serverName);
	myClient = new wxSocketClient();
	myClient->SetEventHandler(*this, MYAPP_SOCKET_ID);
	myClient->SetNotify(wxSOCKET_CONNECTION_FLAG | wxSOCKET_INPUT_FLAG |
	  wxSOCKET_LOST_FLAG);
	myClient->Notify(TRUE);

	if (myClient->Connect(addr, TRUE))
		return(TRUE);

	myClient = NULL;
	myServer = new wxSocketServer(addr);
	if (!myServer->Ok()) {
		NotifyError("%s: Could not listen at specified port.", funcName);
		return(FALSE);
	}
	myServer->SetEventHandler(*this, MYAPP_SERVER_ID);
	myServer->SetNotify(wxSOCKET_CONNECTION_FLAG);
	myServer->Notify(TRUE);
	return(TRUE);
			
}
			
/****************************** CheckInitialisation ***************************/

/*
 * This routine checks that the simulation run has been initialised.
 */

bool
MyApp::CheckInitialisation(void)
{
	static const char *funcName = "MyApp::CheckInitialisation";

	if (!GetPtr_AppInterface()) {
		NotifyError("%s: Application interface not initialised.", funcName);
		return(FALSE);
	}
	if (GetPtr_AppInterface()->updateProcessVariablesFlag)
		ResetStepCount_Utility_Datum();
	if (!InitProcessVariables_AppInterface(NULL, myArgc, myArgv)) {
		NotifyError("%s: Could not initialise process variables.", funcName);
		return(FALSE);
	}

	SetTitle();
	return(TRUE);

}

/****************************** CheckOptions **********************************/

/*
 * This routine checks for the program call options.
 * It sets the ones only to be used here to be ignored by other routines.
 */

void
MyApp::CheckOptions(void)
{
	int		optInd = 1, optSub = 0;
	char	c, *argument;

	while ((c = Process_Options(argc, argv, &optInd, &optSub, &argument,
	  "SI:l:")))
		switch (c) {
		case 'S':
			clientServerFlag = TRUE;
			MarkIgnore_Options(argc, argv, "-S", OPTIONS_NO_ARG);
			break;
		case 'I':
			serverId = atoi(argument);
			MarkIgnore_Options(argc, argv, "-I", OPTIONS_WITH_ARG);
			break;
		case 'l':
			wxGetApp().InitRun();
			if (GetPtr_AppInterface() && GetPtr_AppInterface()->Init) {
				InitMain();
				if (!CheckInitialisation())
					exit(1);
			}
			MainSimulation();
		default:
			;
		} /* switch */

}

/****************************** ResetCommandArgs ******************************/

bool
MyApp::ResetCommandArgs(void)
{
	static const char *funcName = "MyApp::ResetCommandArgs";

	if (!GetPtr_AppInterface()) {
		NotifyError("%s: Application interface not initialised.", funcName);
		return(FALSE);
	}
	ResetCommandArgFlags_AppInterface();
	if (!InitProcessVariables_AppInterface(NULL, myArgc, myArgv)) {
		NotifyError("%s: Could not initialise process variables.", funcName);
		return(FALSE);
	}
	return(TRUE);

}

/****************************** InitMain **************************************/

/*
 * This routine initialises the program.
 */

void
MyApp::InitMain(void)
{
	ResetDefaultDisplayPos();
	if (GetPtr_AppInterface())
		SetConfiguration(GetPtr_AppInterface()->parList);
	ResetCommandArgs();
	ResetStepCount_Utility_Datum();
	SetTitle();
	
}

/****************************** ExitMain **************************************/

/*
 * This saves the configuration and runs the OnExit virtual function.
 */

void
MyApp::ExitMain(void)
{
	if (GetPtr_AppInterface())
		SaveConfiguration(GetPtr_AppInterface()->parList);

	DeleteSimThread();

	SetCanFreePtrFlag_AppInterface(TRUE);
	Free_AppInterface();
	FreeAll_EarObject();

}

/****************************** StartSimThread ********************************/

/*
 * This routine starts the simulation thread.
 */

void
MyApp::StartSimThread(void)
{
	static const char *funcName = "MyApp::StartSimThread";

	mainCritSect.Enter();
	SwitchGUILocking_Common(TRUE);
	mainCritSect.Leave();
	simThread = new SimThread();
	if (simThread->Create() != wxTHREAD_NO_ERROR)
		wxLogFatalError("%s: Can't create simulation thread!", funcName);

	if (simThread->Run() != wxTHREAD_NO_ERROR)
		wxLogError("%s: Cannot start simulation thread.", funcName);

}

/****************************** RunInClientMode *******************************/

/*
 * This routine runs the simulation as a client.
 * A request is sent to the server to ensure that the connection is not
 * closed before the simulation is finished.
 */

void
MyApp::RunInClientMode(void)
{
	/* static const char *funcName = "MyApp::RunInClientMode"; */
	char	c;
	int		i;

	busy = TRUE;
	c = (char) argc;
	myClient->Write(&c, 1);
	for (i = 0; i < argc; i++)
		myClient->WriteMsg(argv[i], strlen(argv[i]) + 1);
	// myClient->Read(&c, 1);
	// if (c != 0)
	//	NotifyError("%s: Server simulation did not run.", funcName);

}

/****************************** InitRun ***************************************/

/*
 * This function runs the simulation in the first initialisation run
 * during which the application interface is initialised.
 * With correct usage of the 'InitProcessVariables_AppInterface' routine
 * nothing after the program should be run.
 */

bool
MyApp::InitRun(void)
{
	static const char *funcName = "MyApp::InitRun";

	wxCriticalSectionLocker locker(mainCritSect);

	if (GetDSAMPtr_Common()->appInitialisedFlag) {
		NotifyWarning("%s: Attempted to re-initialise application.", funcName);
		return(TRUE);
	}
	MainSimulation();
	SetCanFreePtrFlag_AppInterface(FALSE);
	return(CXX_BOOL(GetDSAMPtr_Common()->appInitialisedFlag));

}

/****************************** SetTitle **************************************/

// This routine sets the frame title from the application interface title.
// The NULL frame test is necessary because the first time this is called the
// frame will not have been set.

void
MyApp::SetTitle(void)
{
	if (!GetPtr_AppInterface())
		return;

	if (frame) {
		title.sprintf("%s: %s", GetPtr_AppInterface()->appName,
		  GetPtr_AppInterface()->title);
		frame->SetTitle(title);
	}

}

/****************************** GetDefaultDisplayPos **************************/

/*
 * This routine sets the argument variables to the next default display
 * position.
 */

void
MyApp::GetDefaultDisplayPos(int *x, int *y)
{
	*x = displayDefaultX;
	*y = displayDefaultX;
	displayDefaultX += 15;
	displayDefaultY += 10;

}

/****************************** SetArgv ***************************************/

bool
MyApp::SetArgvString(int index, char *string, int size)
{
	static char *funcName = "MyServer::SetArgv";
	
	if ((myArgv[index] = (char *) malloc(size)) == NULL) {
		NotifyError("%s: Out of memory for myArgv[%d].", funcName, argc);
		return(FALSE);
	}
	strcpy(myArgv[index], string);
	return(TRUE);

}

/****************************** InitArgv **************************************/

/*
 * This allocates the memory for the argv pointer.
 */

bool
MyApp::InitArgv(int argc)
{
	static char *funcName = "MyServer::InitArgv";

	if (!argc)
		return(TRUE);
	myArgc = argc;
	if ((myArgv = (char **) calloc(myArgc, sizeof(char *))) == NULL) {
		NotifyError("%s: Out of memory for myArgv[%d] array.", funcName,
		  myArgc);
		return(FALSE);
	}
	return(TRUE);

}

/****************************** SetConfiguration ******************************/

/*
 * This attempts to set the initial configuration from the saved configuration
 */

void
MyApp::SetConfiguration(UniParListPtr parList)
{
	int		i;
	wxString	parValue;
	UniParPtr	p;

	if (!parList)
		return;

	wxConfigBase *pConfig = wxConfigBase::Get();
 	pConfig->SetPath(SIM_MANAGER_REG_MAIN_PARS);

	for (i = 0; i < parList->numPars; i++) {
		p = &parList->pars[i];
		switch (p->type) {
		case UNIPAR_PARLIST:
			SetConfiguration(*p->valuePtr.parList);
			break;
		default:
			parValue = pConfig->Read(p->abbr, "");
			if (parValue.Len() != 0)
				SetParValue_UniParMgr(&parList, i, (char *) parValue.GetData());
		}
	}

}

/****************************** SaveConfiguration *****************************/

/*
 * This routine saves the configuration
 */

void
MyApp::SaveConfiguration(UniParListPtr parList)
{
	int		i;
	UniParPtr	p;

	if (!parList)
		return;

	wxConfigBase *pConfig = wxConfigBase::Get();
 	pConfig->SetPath(SIM_MANAGER_REG_MAIN_PARS);

	for (i = 0; i < parList->numPars; i++) {
		p = &parList->pars[i];
		switch (p->type) {
		case UNIPAR_PARLIST:
			SaveConfiguration(*p->valuePtr.parList);
			break;
		default:
			pConfig->Write(p->abbr, GetParString_UniParMgr(p));
		}
	}

}

/****************************** StatusChanged *********************************/

/*
 * This routine checks if any status change occurs.  This will signal the
 * re-initialisation of the program.
 */

bool
MyApp::StatusChanged(void)
{
	if (!GetPtr_AppInterface())
		return(FALSE);
	if (SimulationFileChanged_AppInterface(GetFileStatusPtr_AppInterface(
	  GetPtr_AppInterface()->simulationFile), FALSE))
		return(TRUE);
	if (!GetPtr_AppInterface()->audModel)
		return(TRUE);
	return(FALSE);
	
}

/****************************** DeleteSimModuleDialog *************************/

/*
 * This routine deletes the simulation module dialog.
 */

void
MyApp::DeleteSimModuleDialog(void)
{
	if (simModuleDialog) {
		delete simModuleDialog;
		simModuleDialog = NULL;
	}


}

/****************************** DeleteSimThread *******************************/

/*
 * This routine deletes the simulation thread.
 * It first turns off the GUI output so that locking problems with exit
 * diagnostics do not occur.
 * It also waits until the simThread pointer is set to NULL.
 */

void
MyApp::DeleteSimThread(void)
{
	SetGUIDialogStatus(FALSE);
	if (simThread) {
		simThread->SuspendDiagnostics();
		simThread->Delete();
	}

}

/****************************** GetSuspendDiagnostics *************************/

/*
 * This function returns FALSE if there has been a request to suspend the
 * diagnotsics for the main simulation.
 * This is needed, for instance, when the thread is deleted.
 */

bool
MyApp::GetSuspendDiagnostics(void)
{
	if (!simThread)
		return(FALSE);
	return(simThread->GetSuspendDiagnostics());

}

/****************************** SetDiagLocks **********************************/

/*
 * This routine sets the dialog locking on and off.
 */

void
MyApp::SetDiagLocks(bool on)
{
	if (!GetDSAMPtr_Common()->lockGUIFlag)
		return;
	if (on) {
		simThread->diagsCritSect.Enter();
		wxMutexGuiEnter();
	} else {
		simThread->diagsCritSect.Leave();
		wxMutexGuiLeave();
	}

}

/****************************** OnServerEvent *********************************/

/*
 * Server socket event handler routine.
 */

void
MyApp::OnServerEvent(wxSocketEvent& event)
{
	static const char *funcName = "MyApp::OnServerEvent";

	if (event.GetSocketEvent() != wxSOCKET_CONNECTION) {
		NotifyError("%s: Unexpected socket event.", funcName);
		return;
	}
	wxSocketBase *sock = myServer->Accept(FALSE);
	if (!sock) {
		NotifyError("%s: Couldn't accept a new connection.\n");
		return;
	}
	sock->SetEventHandler(*this, MYAPP_SOCKET_ID);
	sock->SetNotify(wxSOCKET_INPUT_FLAG | wxSOCKET_LOST_FLAG);
	sock->Notify(TRUE);

}

/****************************** OnSocketEvent *********************************/

/*
 * Socket event handler routine.
 */

#define MAX_MSG_SIZE	255

void
MyApp::OnSocketEvent(wxSocketEvent& event)
{
	static const char *funcName = "MyApp::OnSocketEvent";
	wxSocketBase *sock = event.GetSocket();

	switch(event.GetSocketEvent()) {
	case wxSOCKET_INPUT: {
		unsigned char c;
		char	msg[MAX_MSG_SIZE];
		int		i, len;
		sock->SetNotify(wxSOCKET_LOST_FLAG);
		sock->Read((char *) &c, 1);
		if (!InitArgv((int) c)) {
			NotifyError("%s: Could not initialise local argv.", funcName);
			return;
		}
		for (i = 0; i < myArgc; i++) {
			len = sock->ReadMsg(msg, MAX_MSG_SIZE).LastCount();
			SetArgvString(i, msg, len);
		}
		if (GetPtr_AppInterface() && myArgv) {
			SetArgcAndArgV_AppInterface(myArgc, myArgv);
			ResetGUIDialogs();
			ResetCommandArgs();
			StartSimThread();
		}
		sock->SetNotify(wxSOCKET_LOST_FLAG | wxSOCKET_INPUT);
		// sock->Write((char *) &c, 1);
		break; }
	case wxSOCKET_LOST:
		sock->Destroy();
		break;
	case wxSOCKET_CONNECTION:
		break;
	default:
		;
	}

}

#undef MAX_MSG_SIZE

/****************************** PrintUsage ************************************/

/*
 * This routine prints the usage for the options used by myApp.
 * It expects to be called by the operating routine's "PrintUsage" routine.
 * So it cannot be a member function.
 */

void
PrintUsage_MyApp(void)
{
	fprintf(stderr, "\n"
	  "\t-S            \t: Run in server mode\n"
	  "\t-I <x>        \t: Run using server ID <x>.\n"
	  );

}

/******************************************************************************/
/****************************** MyFrame Methods *******************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Event tables *************************************/
/******************************************************************************/

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
	EVT_MENU(MYFRAME_ID_ABOUT, MyFrame::OnAbout)
	EVT_MENU(MYFRAME_ID_SIM_THREAD_DISPLAY_EVENT, MyFrame::OnSimThreadEvent)
	EVT_MENU(MYFRAME_ID_EXECUTE, MyFrame::OnExecute)
	EVT_MENU(MYFRAME_ID_EDIT_MAIN_PARS, MyFrame::OnEditMainPars)
	EVT_MENU(MYFRAME_ID_EDIT_SIM_PARS, MyFrame::OnEditSimPars)
	EVT_MENU(MYFRAME_ID_HELP, MyFrame::OnHelp)
	EVT_MENU(MYFRAME_ID_LOAD_SIM_PAR_FILE, MyFrame::OnLoadSimFile)
	EVT_MENU(MYFRAME_ID_LOAD_SIM_SCRIPT_FILE, MyFrame::OnLoadSimFile)
	EVT_MENU(MYFRAME_ID_RELOAD_SIM_SCRIPT_FILE, MyFrame::OnReloadSimFile)
	EVT_MENU(MYFRAME_ID_QUIT, MyFrame::OnQuit)
	EVT_MENU(MYFRAME_ID_SAVE_SIM_PARS, MyFrame::OnSaveSimPars)
	EVT_MENU(MYFRAME_ID_STOP_SIMULATION, MyFrame::OnStopSimulation)
	EVT_MENU(MYFRAME_ID_VIEW_SIM_PARS, MyFrame::OnViewSimPars)
	EVT_BUTTON(MYFRAME_ID_EXECUTE, MyFrame::OnExecute)
	EVT_CLOSE(MyFrame::OnCloseWindow)
	EVT_SIZE(MyFrame::OnSize)
END_EVENT_TABLE()


/****************************** Constructor ***********************************/

// Define my frame constructor
// Note that for the 'LayoutConstraints' to work properly the panel must be
// set using constraints..

MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size):
  wxFrame((wxFrame *) NULL, -1, title, pos, size), help(wxHF_DEFAULTSTYLE |
  wxHF_OPENFILES)
{
	/* static const char *funcName = "MyFrame::MyFrame"; */
	SetIcon(wxICON((wxGetApp().icon)? *wxGetApp().icon: dsam));
	wxLayoutConstraints	*c;

	defaultDir = wxGetCwd();

	fileMenu = editMenu = viewMenu = NULL;
	mainParDialog = NULL;

#	ifdef MPI_SUPPORT
	static const char *funcName = "MyFrame::MyFrame";
	int		argc = 1;
	
	if ((initStringPtrs = (char **) calloc(argc, sizeof(char *))) == NULL) {
		NotifyError("%s: Out of memory for %d initialisation strings.",
		 funcName, argc);
		 exit(1);
	}
	initStringPtrs[0] = (char *) funcName;
	MPI_Init( &argc, &initStringPtrs );
#	endif

	// Make menus
	fileMenu = new wxMenu("", wxMENU_TEAROFF);
	fileMenu->Append(MYFRAME_ID_LOAD_SIM_PAR_FILE, "L&oad parameter file "
	  "(*.spf)...\tCtrl-O", "Load Simulation parameter file.");
	fileMenu->Append(MYFRAME_ID_LOAD_SIM_SCRIPT_FILE, "&Load script file "
	  "(*.sim)...\tCtrl-L", "Load Simulation script file.");
	fileMenu->Append(MYFRAME_ID_RELOAD_SIM_SCRIPT_FILE, "Reload simulation "
	  "file", "Reload Simulation file.");
	fileMenu->AppendSeparator();
	fileMenu->Append(MYFRAME_ID_SAVE_SIM_PARS, "&Save Simulation Pars..."
	  "\tCtrl-S", "Save simulation parameters...");
	fileMenu->AppendSeparator();
	fileMenu->Append(MYFRAME_ID_QUIT, "&Quit\tCtrl-Q", "Quit from program.");

	editMenu = new wxMenu("", wxMENU_TEAROFF);
	editMenu->Append(MYFRAME_ID_EDIT_SIM_PARS, "Simulation parameters..."
	  "\tCtrl-E", "Edit simulation parameters");
	editMenu->Append(MYFRAME_ID_EDIT_MAIN_PARS, "&Main Parameters...\tCtrl-M",
	  "Edit main program preferences");

	viewMenu = new wxMenu("", wxMENU_TEAROFF);
	viewMenu->Append(MYFRAME_ID_VIEW_SIM_PARS, "&Simulation parameters",
	  "List simulation parameters (to diagnostic window)");

	programMenu = new wxMenu;
	programMenu->Append(MYFRAME_ID_EXECUTE, "&Execute\tCtrl-G", "Execute "
	  "simulation");
	programMenu->Append(MYFRAME_ID_STOP_SIMULATION, "S&top simulation\tCtrl-C",
	  "Stop simulation execution.");

	wxMenu *helpMenu = new wxMenu;
	helpMenu->Append(MYFRAME_ID_ABOUT, _("&About...\tCtrl-A"),
	  _("Show about dialog"));
	helpMenu->Append(MYFRAME_ID_HELP, "&Help...\tCtrl-H");
	
	// Make a menubar
	wxMenuBar *menuBar = new wxMenuBar;

	menuBar->Append(fileMenu, "&File");
	menuBar->Append(editMenu, "&Edit");
	menuBar->Append(viewMenu, "&View");
	menuBar->Append(programMenu, "&Program");
	menuBar->Append(helpMenu, "&Help");
	
	// Associate the menu bar with the frame
	SetMenuBar(menuBar);

	// Make a panel
	panel = new wxPanel(this, 0, 0, 1000, 30, wxTAB_TRAVERSAL);
	panel->SetBackgroundColour(wxSystemSettings::GetSystemColour(
	  wxSYS_COLOUR_3DFACE));

	// Panel controls

	// Buttons
	wxButton *goBtn = new wxButton(panel, MYFRAME_ID_EXECUTE, "GO");
	goBtn->SetDefault();

	c = new wxLayoutConstraints;

	c->left.SameAs(panel, wxLeft, 4);
	c->top.SameAs(panel, wxTop, 4);
	c->bottom.SameAs(panel, wxBottom, 4);
	c->width.AsIs();
	goBtn->SetConstraints(c);

	// Set constraints for panel sub window
	c = new wxLayoutConstraints;

	c->left.SameAs(this, wxLeft);
	c->top.SameAs(this, wxTop);
	c->right.SameAs(this, wxRight);
	c->bottom.AsIs();
	panel->SetConstraints(c);

	// Make text window for diagnostics
	diagnosticsWindow = new wxTextCtrl(this, -1, "DSAM Diagnostics "
	  "Window\n", wxPoint(0, 0), wxDefaultSize, wxTE_MULTILINE);
	diagnosticsWindow->SetFont(*diagnosticsWindowFont);

	c = new wxLayoutConstraints;

	c->top.Below(panel);
	c->left.SameAs(this, wxLeft);
	c->right.SameAs(this, wxRight);
	c->bottom.SameAs(this, wxBottom);
	diagnosticsWindow->SetConstraints(c);

	// Initialise application
	wxGetApp().InitRun();
	if (GetPtr_AppInterface() && GetPtr_AppInterface()->Init) {
		GetPtr_AppInterface()->PrintSimMgrUsage = PrintUsage_MyApp;
		wxGetApp().InitMain();
	}

	// Disable unusable menus
	if (!GetPtr_AppInterface() || !GetPtr_AppInterface()->Init ||
	  !GetPtr_AppInterface()->audModel) {
		EnableSimParMenuOptions(FALSE);
	}
	programMenu->Enable(MYFRAME_ID_STOP_SIMULATION, FALSE);

	// Get config setup
	pConfig = wxConfigBase::Get();

	// Help setup
	helpCount = 0;
	wxString helpFile;
	help.UseConfig(wxConfig::Get());
	// help.SetTempDir(".");  -- causes crashes on solaris
	if (GetPtr_AppInterface()) {
		AddHelpBook(SIM_MANAGER_REG_APP_HELP_PATH,
		  GetPtr_AppInterface()->installDir,
		  GetPtr_AppInterface()->appName);
		AddHelpBook(SIM_MANAGER_REG_DSAM_HELP_PATH, DSAM_DATA_INSTALL_DIR,
		  DSAM_PACKAGE);
	}
	if (!helpCount)
		helpMenu->Enable(MYFRAME_ID_HELP, FALSE);

	// restore frame position and size
	pConfig->SetPath(SIM_MANAGER_REG_MAIN_FRAME);
	int		x = pConfig->Read("x", pos.x),	y = pConfig->Read("y", pos.y);
	int		w = pConfig->Read("w", size.GetWidth());
	int		h = pConfig->Read("h", size.GetHeight());
	Move(x, y);
    CreateStatusBar(2);
	SetClientSize(w, h);

}

/****************************** Destructor ************************************/

// Define my frame destructor

MyFrame::~MyFrame(void)
{
#	ifdef MPI_SUPPORT
	int		i, myRank, numProcesses, ok = TRUE;

	MPI_Comm_rank( MPI_COMM_WORLD, &myRank);
	MPI_Group_size( MPI_COMM_WORLD, &numProcesses);	
	for (i = 0; i < numProcesses; i++)
		if (i != myRank)
			MPI_Send(&ok, 1, MPI_INT, i, MASTER_EXIT_TAG, MPI_COMM_WORLD);
	
	free(initStringPtrs);
	MPI_Finalize();
#	endif
	
	DeleteMainParDialog();
	wxGetApp().ExitMain();

	// save the control's values to the config
	if ( pConfig != NULL ) {
		// save the frame position
		pConfig->SetPath(SIM_MANAGER_REG_MAIN_FRAME);
		int		x, y, w, h;
		GetClientSize(&w, &h);
		GetPosition(&x, &y);
		pConfig->Write("x", (long) x);
		pConfig->Write("y", (long) y);
		pConfig->Write("w", (long) w);
		pConfig->Write("h", (long) h);
	}

}

/****************************** AddHelpBook ***********************************/

void
MyFrame::AddHelpBook(const wxString& path, const wxString& defaultPath,
  const wxString& fileName)
{
	wxString helpFile, helpFilePath;

	pConfig->SetPath(SIM_MANAGER_REG_PATHS);
	helpFilePath = pConfig->Read(path, "");
	if (GetPtr_AppInterface()) {
		helpFile = ((helpFilePath.Len() != 0)? helpFilePath: defaultPath) +
		  "/" + SIM_MANAGER_HELP_DIR + "/" + fileName + ".hhp";
#		ifdef __WXMSW__
		wxUnix2DosFilename((char *) helpFile.GetData());
#		endif
	} else
		helpFile = defaultPath + "/" + SIM_MANAGER_HELP_DIR + "/DSAMApp.hhp";
	if (wxFileExists(helpFile) && help.AddBook(helpFile))
		 helpCount++;

}

/****************************** EnableSimParMenuOptions ***********************/

// Intercept menu commands

void
MyFrame::EnableSimParMenuOptions(bool on)
{
	fileMenu->Enable(MYFRAME_ID_SAVE_SIM_PARS, on);
	editMenu->Enable(MYFRAME_ID_EDIT_SIM_PARS, on);
	viewMenu->Enable(MYFRAME_ID_VIEW_SIM_PARS, on);
	programMenu->Enable(MYFRAME_ID_EXECUTE, on);

}

/****************************** ResetSimulation *******************************/

bool
MyFrame::ResetSimulation(void)
{
	static char *funcName = "MyFrame::ResetSimulation";

	if (wxGetApp().simThread) {
		wxLogWarning("%s: Running simulation not yet terminated!", funcName);
		return(FALSE);
	}
	wxGetApp().DeleteSimModuleDialog();

	SwitchGUILocking_Common(FALSE);
	if (!wxGetApp().CheckInitialisation()) {
		EnableSimParMenuOptions(FALSE);
		return(FALSE);
	}
	EnableSimParMenuOptions(TRUE);
	return(TRUE);

}

/****************************** DeleteMainParDialog ***************************/

void
MyFrame::DeleteMainParDialog(void)
{
	if (!mainParDialog)
		return;
	delete mainParDialog;
	mainParDialog = NULL;

}

/****************************** UpdateMainParDialog ***************************/

/*
 * This routine checks if main dialog window is open, and if so it refreshes
 * the parameters in the dialog.
 */

void
MyFrame::UpdateMainParDialog(void)
{
	if (!mainParDialog)
		return;
	

}

/****************************** SetSimFileAndLoad *****************************/

/*
 * This routine sets the simulation file and attempts to load it.
 */

void
MyFrame::SetSimFileAndLoad(void)
{
	if (!SetParValue_UniParMgr(&GetPtr_AppInterface()->parList,
	  APP_INT_SIMULATIONFILE, (char *) simFilePath.GetData()))
		return;
	ResetSimulation();
	if (mainParDialog)
		mainParDialog->parListInfoList->UpdateAllControlValues();

}

/******************************************************************************/
/****************************** Call backs ************************************/
/******************************************************************************/

/****************************** OnExecute *************************************/

void
MyFrame::OnExecute(wxCommandEvent& WXUNUSED(event))
{
	static char *funcName = "MyFrame::OnExecute";

	if (wxGetApp().simThread) {
		wxLogWarning("%s: Running simulation not yet terminated!", funcName);
		return;
	}

	ResetGUIDialogs();
	if (GetPtr_AppInterface()->Init) {
		if (wxGetApp().StatusChanged()) {
			wxGetApp().DeleteSimModuleDialog();
			if (!wxGetApp().CheckInitialisation())
				return;
		}
		if (wxGetApp().GetSimModuleDialog() && !wxGetApp().GetSimModuleDialog(
		  )->CheckChangedValues())
			return;
		if (!GetPtr_AppInterface()->audModel) {
			NotifyError("%s: Simulation not initialised.", funcName);
			return;
		}
	}
	if (mainParDialog) {
		if (!mainParDialog->CheckChangedValues())
			return;
		mainParDialog->parListInfoList->UpdateAllControlValues();
		mainParDialog->cancelBtn->Enable(FALSE);
	}
	ResetGUIDialogs();
	wxGetApp().StartSimThread();
	programMenu->Enable(MYFRAME_ID_STOP_SIMULATION, TRUE);

}

/****************************** OnStopSimulation ******************************/

void
MyFrame::OnStopSimulation(wxCommandEvent& WXUNUSED(event))
{

	if (!wxGetApp().simThread)
		return;
	wxGetApp().DeleteSimThread();
	wxLogWarning("Simulation terminated by user.");

}

/****************************** OnQuit ****************************************/

void
MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
	Close();

}

/****************************** OnCloseWindow *********************************/

// Define the behaviour for the frame closing
// must delete all frames except for the main one.

void
MyFrame::OnCloseWindow(wxCloseEvent& WXUNUSED(event))
{
	Destroy();

}

/****************************** OnHelp ****************************************/

void
MyFrame::OnHelp(wxCommandEvent& WXUNUSED(event))
{
	help.Display("Main page");

}

/****************************** OnAbout ***************************************/

void
MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
	int		i;
	wxString title, authors, message, dsamVersion;

	if (GetPtr_AppInterface()) {
		title.sprintf("About %s %s", GetPtr_AppInterface()->appName,
		  GetPtr_AppInterface()->appVersion);
		dsamVersion = GetPtr_AppInterface()->compiledDSAMVersion;
		for (i = 0, authors = "Authors: "; i < APP_MAX_AUTHORS; i++)
			if (GetPtr_AppInterface()->authors[i][0] != '\0')
				authors += GetPtr_AppInterface()->authors[i];
	} else {
		title.sprintf("About Application");
		dsamVersion = DSAM_VERSION;
	}
	message.sprintf("%s"
	  "DSAM version: %s (dynamic), compiled with %s\n"
	  "Author, Dr. Lowel P. O'Mard (with God's Help)\n"
	  "(c) 2001 Centre for the Neural Basis of Hearing (CNBH)\n",
	  authors.GetData(), GetDSAMPtr_Common()->version, dsamVersion.GetData());
	wxMessageBox(message, title, wxOK | wxICON_INFORMATION, this);

}

/****************************** OnEditMainPars ********************************/

/*
 * This routine creates the run paraneter dialog.
 * If the GetPtr_AppInterface()->updateProcessFlag is set, then on
 * 'OK' the simulation parameter windows and display windows will be closed.
 */

void
MyFrame::OnEditMainPars(wxCommandEvent& WXUNUSED(event))
{
	if (!GetPtr_AppInterface()->parList)
		return;

	if (mainParDialog)
		return;
	mainParDialog = new ModuleParDialog(this, "Main Parameters",
	  MODPARDIALOG_MAIN_PARENT_INFONUM, NULL, GetPtr_AppInterface()->parList,
	  300, 300, 500, 500, wxDEFAULT_DIALOG_STYLE);
	mainParDialog->Show(TRUE);

	/*if ((dialog.ShowModal() == wxID_OK) && GetPtr_AppInterface()->
	  updateProcessVariablesFlag)
		ResetSimulation();*/

}

/****************************** OnEditSimPars *********************************/

void
MyFrame::OnEditSimPars(wxCommandEvent& WXUNUSED(event))
{
	ResetGUIDialogs();
	if (wxGetApp().StatusChanged()) {
		ResetStepCount_Utility_Datum();
		wxGetApp().CheckInitialisation();
	}
	if (!wxGetApp().GetSimModuleDialog()) {
		wxGetApp().SetSimModuleDialog(new SimModuleDialog(this,
		  "Simulation Parameters", -1, GetSimulation_ModuleMgr(
		  GetPtr_AppInterface()->audModel), GetUniParListPtr_ModuleMgr(
		  GetPtr_AppInterface()->audModel)));
	}

}

/****************************** OnLoadSimFile *********************************/

void
MyFrame::OnLoadSimFile(wxCommandEvent& event)
{
	ResetGUIDialogs();
	wxString extension = (event.GetId() == MYFRAME_ID_LOAD_SIM_PAR_FILE)?
	  "*.spf": "*.sim";
	wxFileDialog dialog(this, "Choose a file", defaultDir, "", extension);
	if (dialog.ShowModal() == wxID_OK) {
		defaultDir = dialog.GetDirectory();
		simFilePath = dialog.GetPath();
		SetSimFileAndLoad();
	}

}

/****************************** OnReloadSimFile *******************************/

void
MyFrame::OnReloadSimFile(wxCommandEvent& event)
{

	ResetGUIDialogs();
	SetSimFileAndLoad();

}

/****************************** OnSaveSimPars *********************************/

void
MyFrame::OnSaveSimPars(wxCommandEvent& WXUNUSED(event))
{
	char *filePath, path[MAX_FILE_PATH], fileName[MAXLINE];
	wxString newFilePath;

	filePath = GetPtr_AppInterface()->parList->pars[APP_INT_SIMULATIONFILE].
	  valuePtr.s;
	FindFilePathAndName_Common(filePath, path, fileName);
	newFilePath = FileSelector_Utils("Simulation parameter file", path,
	  fileName, "spf", "*.spf", wxSAVE | wxOVERWRITE_PROMPT |
	  wxHIDE_READONLY, this);
	if (!newFilePath)
		return;
	bool dialogOutputFlag = CXX_BOOL(GetDSAMPtr_Common()->dialogOutputFlag);
	FILE *oldFp = GetDSAMPtr_Common()->parsFile;
	SetGUIDialogStatus(FALSE);
	SetParsFile_Common((char *) newFilePath.GetData(), OVERWRITE);
	ListParameters_AppInterface();
	fclose(GetDSAMPtr_Common()->parsFile);
	GetDSAMPtr_Common()->parsFile = oldFp;
	SetGUIDialogStatus(dialogOutputFlag);
	if (filePath)
		snprintf(filePath,MAX_FILE_PATH, "%s", (char *) newFilePath.GetData());

}

/****************************** OnViewSimPars *********************************/

void
MyFrame::OnViewSimPars(wxCommandEvent& WXUNUSED(event))
{
	FILE	*oldParsFile = GetDSAMPtr_Common()->parsFile;

	SetParsFile_Common("screen", OVERWRITE);
	ListParameters_AppInterface();
	GetDSAMPtr_Common()->parsFile = oldParsFile;

}

/****************************** OnSimThreadEvent ******************************/

void
MyFrame::OnSimThreadEvent(wxCommandEvent& event)
{
 	switch (event.GetInt()) {
	case MYAPP_THREAD_DRAW_GRAPH: {
		SignalDispPtr	signalDispPtr = (SignalDispPtr) event.GetClientData();

		signalDispPtr->critSect->Enter();
		if (!signalDispPtr->display) {
			signalDispPtr->display = new DisplayS(this, signalDispPtr);
			signalDispPtr->display->canvas->InitGraph();
			signalDispPtr->display->Show(TRUE);
		} else {
			signalDispPtr->display->canvas->InitGraph();
			signalDispPtr->display->canvas->RedrawGraph();
		}
		signalDispPtr->drawCompletedFlag = TRUE;
		signalDispPtr->critSect->Leave();
		break; }
	default:
		;
	}
 
}
  
/****************************** OnSize ****************************************/

// Size the subwindows when the frame is resized.
// Note that the 'width' and 'height' parameters are not used, but are reset
// using GetClientSize(...).

void
MyFrame::OnSize(wxSizeEvent& WXUNUSED(event))
{
	Layout();
	
}

/******************************************************************************/
/****************************** General Routines ******************************/
/******************************************************************************/

