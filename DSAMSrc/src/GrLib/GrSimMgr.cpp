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

#include "DSAM.h"
#include "UtSSParser.h"

#include <wx/image.h>
#include <wx/config.h>
#include <wx/thread.h>
#include <wx/dynarray.h>


#if !wxUSE_CONSTRAINTS
#	error "You must set wxUSE_CONSTRAINTS to 1 in setup.h!"
#endif

#if !wxUSE_THREADS
#	error "You must enable thread support!"
#endif // wxUSE_THREADS

#if !wxUSE_DOC_VIEW_ARCHITECTURE
#	error You must set wxUSE_DOC_VIEW_ARCHITECTURE to 1 in setup.h!
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

MyApp::MyApp(void): help(wxHF_DEFAULTSTYLE | wxHF_OPENFILES)
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
	myDocManager = NULL;
	diagFrame = NULL;
	fileMenu = editMenu = viewMenu = programMenu = NULL;
	helpCount = 0;
	audModelLoadedFlag = false;

}

/****************************** OnExit ****************************************/

/*.
 */

int
MyApp::OnExit(void)
{
	int		i;

	wxOGLCleanUp();
	delete printData;

	if (myServer)
		myServer->Destroy();
	if (myClient)
		myClient->Destroy();
	if (myDocManager)
		delete myDocManager;

	if (myArgv && (myArgv != argv)) {
		for (i = 0; i < myArgc; i++)
			if (myArgv)
				free(myArgv[i]);
		free(myArgv);
	}
	delete wxConfigBase::Set((wxConfigBase *) NULL);
	Free_AppInterface();
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
	wxOGLInitialize();
	/* static const char *funcName = "MyApp::OnInit"; */

	// Create a document manager
	myDocManager = new wxDocManager;

	// Create templates relating drawing documents to their views
	(void) new wxDocTemplate(myDocManager, "Simulation Parameter File", "*.spf",
	  "", "spf", "Simulation Design", "Simulation view", CLASSINFO(SDIDocument),
	CLASSINFO(SDIView));
	(void) new wxDocTemplate(myDocManager, "Simulation Script", "*.sim",
	  "", "sim", "Simulation Design", "Simulation view", CLASSINFO(SDIDocument),
	CLASSINFO(SDIView));

	myDocManager->SetMaxDocsOpen(1);

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

	pConfig = wxConfigBase::Get();

	InitAppInterface();

	// Get config setup
	pConfig = wxConfigBase::Get();

	InitHelp();

	// Get frame position and size
	pConfig->SetPath(SIM_MANAGER_REG_MAIN_FRAME);
	int		x = pConfig->Read("x", (long int) 0);
	int		y = pConfig->Read("y", (long int) 0);
	int		w = pConfig->Read("w", (long int) SIM_MANAGER_DEFAULT_WIDTH);
	int		h = pConfig->Read("h", (long int) SIM_MANAGER_DEFAULT_HEIGHT);

	// Create the main frame window
	frame = new SDIFrame(myDocManager, (wxFrame *) NULL, "Simulation Manager",
	  wxPoint(x, y), wxSize(w, h), wxDEFAULT_FRAME_STYLE);

	// Ensure title it set after the frame is created.
	SetTitle();

	frame->SetMenuBar(CreateMenuBar());
	frame->editMenu = editMenu;
	myDocManager->FileHistoryUseMenu(fileMenu);

	frame->canvas = frame->CreateCanvas(NULL, frame);

	if (GetPtr_AppInterface())
		SetAppName(GetPtr_AppInterface()->appName);
	else
		SetAppName("DSAM_App");

	// Set up process lists for dialogs
	CreateProcessLists();

	myDocManager->CreateDocument("", wxDOC_SILENT);

    // Show frame and tell the application that it's our main window

	frame->Show(TRUE);
    //SetTopWindow(frame);

	if (myServer)
		StartSimThread();
	// Essential - return the main frame window
	return(TRUE);

}

/****************************** InitAppInterface ******************************/

/*
 * This routine initialises the application interface.
 */

void
MyApp::InitAppInterface(void)
{
	InitRun();
	if (GetPtr_AppInterface() && GetPtr_AppInterface()->Init) {
		GetPtr_AppInterface()->PrintSimMgrUsage = PrintUsage_MyApp;
		InitMain();
		// ??
		// The code below, and after the "else" needs to be done somewhere else.
		//simFilePath = GetParString_UniParMgr(&GetPtr_AppInterface()->parList->
		//  pars[APP_INT_SIMULATIONFILE]);
		//defaultDir = (wxIsAbsolutePath(simFilePath))? wxPathOnly(simFilePath):
		//  wxGetCwd() + "/" + wxPathOnly(simFilePath);
	} else 
		defaultDir = wxGetCwd();

}

/****************************** InitHelp **************************************/

/*
 * This routine initialises the help.
 */

void
MyApp::InitHelp(void)
{
	help.UseConfig(wxConfig::Get());
	// help.SetTempDir(".");  -- causes crashes on solaris
	if (GetPtr_AppInterface()) {
		AddHelpBook(SIM_MANAGER_REG_APP_HELP_PATH,
		  GetPtr_AppInterface()->installDir,
		  GetPtr_AppInterface()->appName);
		AddHelpBook(SIM_MANAGER_REG_DSAM_HELP_PATH, DSAM_DATA_INSTALL_DIR,
		  DSAM_PACKAGE);
	}

}

/****************************** CreateMenus ***********************************/

/*
 * This routine creates the application menus
 */

wxMenuBar *
MyApp::CreateMenuBar(void)
{
	fileMenu = new wxMenu("", wxMENU_TEAROFF);
	fileMenu->Append(wxID_NEW, "&New...\tCtrl-N", "Design new simulation.");
	fileMenu->Append(wxID_OPEN, "&Open...\tCtrl-O", "Load Simulation from "
	  "file.");
	fileMenu->Append(wxID_REVERT, "&Reload...\tCtrl-R", "Reload Simulation.");
	fileMenu->Append(wxID_CLOSE, "&Close");
 	fileMenu->AppendSeparator();
	fileMenu->Append(wxID_SAVE, "&Save...\tCtrl-S", "Save simulation parameter "
	  "file.");
	fileMenu->Append(wxID_SAVEAS, "Save as...", "Save simulation "
	  "parameter file...");
	fileMenu->AppendSeparator();
	fileMenu->Append(wxID_EXIT, "E&xit\tCtrl-x", "Exit from program.");
	myDocManager->FileHistoryUseMenu(fileMenu);

	editMenu = new wxMenu("", wxMENU_TEAROFF);
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

	windowsMenu = new wxMenu("", wxMENU_TEAROFF);
	windowsMenu->Append(SDIFRAME_DIAG_WINDOW, "Diagnostic Window",
	  "Open diagnostics window.");

	wxMenu *helpMenu = new wxMenu;
	helpMenu->Append(MYFRAME_ID_ABOUT, _("&About...\tCtrl-A"),
	  _("Show about dialog"));
	helpMenu->Append(wxID_HELP, "&Help...\tCtrl-H");
	if (!helpCount)
		helpMenu->Enable(wxID_HELP, FALSE);

	// Disable unusable menus
	if (!GetPtr_AppInterface() || !GetPtr_AppInterface()->Init ||
	  !GetPtr_AppInterface()->audModel) {
		EnableSimParMenuOptions(FALSE);
	}
	programMenu->Enable(MYFRAME_ID_STOP_SIMULATION, FALSE);
#	ifndef HAVE_WX_OGL_OGL_H
	fileMenu->Enable(MYFRAME_ID_NEW_SIM, FALSE);
#	endif

	// Make a menubar
	wxMenuBar *menuBar = new wxMenuBar;

	menuBar->Append(fileMenu, "&File");
	menuBar->Append(editMenu, "&Edit");
	menuBar->Append(viewMenu, "&View");
	menuBar->Append(programMenu, "&Program");
	menuBar->Append(windowsMenu, "&Windows");
	menuBar->Append(helpMenu, "&Help");
	return(menuBar);

}

/****************************** AddToProcessList ******************************/

// This routine adds to a process list using the supplied prefix.
// It assumes that all process modules with similar prefixs are in the same
// region.
// If the prefix is NULL then the process list is constructed from the user
// module list.

void
MyApp::AddToProcessList(wxStringList& list, const wxString& prefix)
{
	ModRegEntryPtr	prefixStart, modList;

	if (prefix.GetChar(0) == '@') {
		list.Add(prefix.Mid(1));
		return;
	}
	if ((prefixStart = (prefix.IsEmpty())? UserList_ModuleReg(0):
	  GetRegEntry_ModuleReg((char *) prefix.GetData())) == NULL)
		return;
	for (modList = prefixStart; (modList->name[0] != '\0'); modList++)
		if (StrNCmpNoCase_Utility_String(modList->name, (char *) prefix.GetData(
		  )) == 0)
			list.Add(modList->name);

}

/****************************** CreateProcessLists ****************************/

// This routine creates the lists of processes for the dialog windows.
// The 

void
MyApp::CreateProcessLists(void)
{
	wxString	string;
	SymbolPtr	p, symList = NULL;

	AddToProcessList(anaList, "Ana_");
	AddToProcessList(filtList, "Filt_");
	AddToProcessList(ioList, "DataFile_");
	AddToProcessList(ioList, "Display_");
	AddToProcessList(modelsList, "AN_SG_");
	AddToProcessList(modelsList, "BM_");
	AddToProcessList(modelsList, "IHC_");
	AddToProcessList(modelsList, "IHCRP_");
	AddToProcessList(modelsList, "NEUR_");
	AddToProcessList(stimList, "Stim_");
	AddToProcessList(transList, "Trans_");
	AddToProcessList(userList, "");
	AddToProcessList(utilList, "Util_");

	InitKeyWords_Utility_SSSymbols(&symList);
	for (p = symList; p; p = p->next) {
		if ((p->type == STOP) || (p->type == BEGIN) || (p->type == REPEAT))
			continue;
		string.Printf("@%s", p->name);
		AddToProcessList(ctrlList, string);
	}
	FreeSymbols_Utility_SSSymbols(&symList);

}

/****************************** EnableSimParMenuOptions ***********************/

// Intercept menu commands

void
MyApp::EnableSimParMenuOptions(bool on)
{
	/*
	**** to be removed ?***
	fileMenu->Enable(MYFRAME_ID_SAVE_SIM_PARS, on);
	editMenu->Enable(MYFRAME_ID_EDIT_SIM_PARS, on);
	viewMenu->Enable(MYFRAME_ID_VIEW_SIM_PARS, on);
	*/
	programMenu->Enable(MYFRAME_ID_EXECUTE, on);

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
 * This saves the configuration and closes all DSAM interfaces.
 */

void
MyApp::ExitMain(void)
{
	if (GetPtr_AppInterface())
		SaveConfiguration(GetPtr_AppInterface()->parList);

	DeleteSimThread();

	SetCanFreePtrFlag_AppInterface(TRUE);
	// FreeAll_EarObject();

	// save the control's values to the config
	if ( pConfig != NULL ) {
		// save the frame position
		pConfig->SetPath(SIM_MANAGER_REG_MAIN_FRAME);
		int		x, y, w, h;
		frame->GetSize(&w, &h);
		frame->GetPosition(&x, &y);
		pConfig->Write("x", (long) x);
		pConfig->Write("y", (long) y);
		pConfig->Write("w", (long) w);
		pConfig->Write("h", (long) h);
	}

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
		title.sprintf("%s: %s (%s)", GetPtr_AppInterface()->appName, 
		  wxFileNameFromPath(GetPtr_AppInterface()->title), (char *) wxPathOnly(
		  GetPtr_AppInterface()->title).GetData());
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
	int		i, j;
	wxString	indexedName, parValue, indexedParValue;
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
		case UNIPAR_INT_ARRAY:
		case UNIPAR_REAL_ARRAY:
		case UNIPAR_STRING_ARRAY:
		case UNIPAR_NAME_SPEC_ARRAY:
			for (j = 0; j < *p->valuePtr.array.numElements; j++) {
				indexedName.Printf("%s.%d", p->abbr, j);
				parValue = pConfig->Read(indexedName, "");
				if (parValue.Len() != 0) {
					indexedParValue.Printf("%d:%s", j, (char *) parValue.
					  GetData());
					SetParValue_UniParMgr(&parList, i, (char *) indexedParValue.
					  GetData());
				}
			}
			break;
		default:
			parValue = pConfig->Read(p->abbr, "");
			if (parValue.Len() != 0)
				SetParValue_UniParMgr(&parList, i, (char *) parValue.GetData());
		}
	}
	GetPtr_AppInterface()->simulationFileFlag = FALSE; //Don't use sim file name

}

/****************************** SaveConfiguration *****************************/

/*
 * This routine saves the configuration
 */

void
MyApp::SaveConfiguration(UniParListPtr parList)
{
	int		i, j, oldIndex;
	UniParPtr	p;
	wxString	name;

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
		case UNIPAR_INT_ARRAY:
		case UNIPAR_REAL_ARRAY:
		case UNIPAR_STRING_ARRAY:
		case UNIPAR_NAME_SPEC_ARRAY:
			for (j = 0; j < *p->valuePtr.array.numElements; j++) {
				oldIndex = p->valuePtr.array.index;
				p->valuePtr.array.index = j;
				name.Printf("%s.%d", p->abbr, j);
				pConfig->Write(name, GetParString_UniParMgr(p));
				p->valuePtr.array.index = oldIndex;
			}
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

/****************************** ResetSimulation *******************************/

bool
MyApp::ResetSimulation(void)
{
	static char *funcName = "MyApp::ResetSimulation";

	if (simThread) {
		wxLogWarning("%s: Running simulation not yet terminated!", funcName);
		return(FALSE);
	}
	DeleteSimModuleDialog();

	SwitchGUILocking_Common(FALSE);
	if (!CheckInitialisation()) {
		EnableSimParMenuOptions(FALSE);
		return(FALSE);
	}
	EnableSimParMenuOptions(TRUE);
	return(TRUE);

}

/****************************** OpenDiagWindow ********************************/

void
MyApp::OpenDiagWindow(void)
{
	if (diagFrame)
		return;

	diagFrame = new DiagFrame(frame, "DSAM Diagnostics");
	diagFrame->Show(TRUE);

}

/****************************** CloseDiagWindow *******************************/

void
MyApp::CloseDiagWindow(void)
{
	delete diagFrame;
	diagFrame = NULL;

}

/****************************** AddHelpBook ***********************************/

void
MyApp::AddHelpBook(const wxString& path, const wxString& defaultPath,
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

