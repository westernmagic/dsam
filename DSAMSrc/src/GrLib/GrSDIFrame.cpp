/**********************
 *
 * File:		GrSDIFrame.cpp
 * Purpose: 	Frame clase for Simulation Design Interface .
 * Comments:	
 * Author:		L.P.O'Mard
 * Created:		11 Nov 2002
 * Updated:		
 * Copyright:	(c) 2002, CNBH, University of Essex
  *
 **********************/

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG_H */

#ifdef HAVE_WX_OGL_OGL_H

#include "DSAM.h"

/******************************************************************************/
/****************************** Bitmaps ***************************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Bitmaps ***************************************/
/******************************************************************************/

#if defined(__WXGTK__) || defined(__WXMOTIF__)
#include "Bitmaps/dsam.xpm"
#endif

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Methods (Subroutines) *************************/
/******************************************************************************/

IMPLEMENT_CLASS(SDIFrame, wxDocParentFrame)

BEGIN_EVENT_TABLE(SDIFrame, wxDocParentFrame)
	EVT_MENU(SDIFRAME_ABOUT, SDIFrame::OnAbout)
	EVT_MENU(SDIFRAME_DIAG_WINDOW, SDIFrame::OnDiagWindow)
	EVT_MENU(SDIFRAME_EDIT_MAIN_PARS, SDIFrame::OnEditMainPars)
	EVT_MENU(wxID_HELP, SDIFrame::OnHelp)
	/*
	EVT_MENU(MYFRAME_ID_EDIT_SIM_PARS, SDIFrame::OnEditSimPars)
	EVT_MENU(wxID_OPEN, SDIFrame::OnLoadSimFile)
	EVT_MENU(wxID_REVERT, SDIFrame::OnReloadSimFile)
	EVT_MENU(wxID_EXIT, SDIFrame::OnQuit)
	EVT_MENU(wxID_SAVEAS, SDIFrame::OnSaveSimPars)
	EVT_MENU(MYFRAME_ID_VIEW_SIM_PARS, SDIFrame::OnViewSimPars)
	*/
	EVT_MENU(SDIFRAME_SIM_THREAD_DISPLAY_EVENT, SDIFrame::OnSimThreadEvent)
	EVT_MENU(SDIFRAME_STOP_SIMULATION, SDIFrame::OnStopSimulation)
	EVT_MENU(SDIFRAME_EXECUTE, SDIFrame::OnExecute)
	EVT_SIZE(SDIFrame::OnSize)
	EVT_CLOSE(SDIFrame::OnCloseWindow)
END_EVENT_TABLE()

/******************************************************************************/
/*************************** Constructor **************************************/
/******************************************************************************/

SDIFrame::SDIFrame(wxDocManager *manager, wxFrame *frame, const wxString& title,
  const wxPoint& pos, const wxSize& size, long type): wxDocParentFrame(manager,
  frame, -1, title, pos, size, type)
{
	SetIcon(wxICON((wxGetApp().icon)? *wxGetApp().icon: dsam));

	canvas = NULL;
	mainParDialog = NULL;
	
#	ifdef MPI_SUPPORT
	static const char *funcName = "SDIFrame::MyFrame";
	int		argc = 1;
	
	if ((initStringPtrs = (char **) calloc(argc, sizeof(char *))) == NULL) {
		NotifyError("%s: Out of memory for %d initialisation strings.",
		 funcName, argc);
		 exit(1);
	}
	initStringPtrs[0] = (char *) funcName;
	MPI_Init( &argc, &initStringPtrs );
#	endif

	palette = new EditorToolPalette(this, wxPoint(0, 0), wxSize(-1,-1),
	  wxTB_VERTICAL);
	Centre(wxBOTH);

}

/******************************************************************************/
/*************************** Desstructor **************************************/
/******************************************************************************/

SDIFrame::~SDIFrame(void)
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

}

/******************************************************************************/
/*************************** CreateCanvas *************************************/
/******************************************************************************/

// Creates a canvas. Called by OnInit as a child of the main window

SDICanvas *
SDIFrame::CreateCanvas(wxView *view, wxFrame *parent)
{
	int width, height;
	parent->GetClientSize(&width, &height);

	// Non-retained canvas
	SDICanvas *canvas = new SDICanvas(view, parent, -1, wxPoint(0, 0),
	  wxSize(width, height), 0);
	canvas->SetCursor(wxCursor(wxCURSOR_HAND));

	// Give it scrollbars
	canvas->SetScrollbars(20, 20, 50, 50);

	return canvas;

}

/****************************** DeleteMainParDialog ***************************/

void
SDIFrame::DeleteMainParDialog(void)
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
SDIFrame::UpdateMainParDialog(void)
{
	if (!mainParDialog)
		return;
	

}

/****************************** SetSimFileAndLoad *****************************/

/*
 * This routine sets the simulation file and attempts to load it.
 */

void
SDIFrame::SetSimFileAndLoad(void)
{
	//diagnosticsWindow->Clear();
	printf("SDIFrame::SetSimFileAndLoad: diagnosticsWindow->Clear()\n");
	if (!SetParValue_UniParMgr(&GetPtr_AppInterface()->parList,
	  APP_INT_SIMULATIONFILE, (char *) wxGetApp().simFilePath.GetData()))
		return;
	wxGetApp().ResetSimulation();
	if (mainParDialog)
		mainParDialog->parListInfoList->UpdateAllControlValues();

}

/******************************************************************************/
/****************************** Call backs ************************************/
/******************************************************************************/

/****************************** OnExecute *************************************/

void
SDIFrame::OnExecute(wxCommandEvent& WXUNUSED(event))
{
	static char *funcName = "SDIFrame::OnExecute";

	if (wxGetApp().simThread) {
		wxLogWarning("%s: Running simulation not yet terminated!", funcName);
		return;
	}

	if (wxGetApp().GetDiagFrame())
		wxGetApp().GetDiagFrame()->Clear();
	ResetGUIDialogs();
	if (GetPtr_AppInterface()->Init) {
//		if (wxGetApp().GetSimModuleDialog() && !wxGetApp().GetSimModuleDialog(
//		  )->CheckChangedValues())
//			return;
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
	wxGetApp().programMenu->Enable(SDIFRAME_STOP_SIMULATION, TRUE);

}

/****************************** OnStopSimulation ******************************/

void
SDIFrame::OnStopSimulation(wxCommandEvent& WXUNUSED(event))
{

	if (!wxGetApp().simThread)
		return;
	wxGetApp().DeleteSimThread();
	wxLogWarning("Simulation terminated by user.");

}

/****************************** OnQuit ****************************************/

void
SDIFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
	Close();

}

/****************************** OnHelp ****************************************/

void
SDIFrame::OnHelp(wxCommandEvent& WXUNUSED(event))
{
	wxGetApp().GetHelpController()->Display("Main page");

}

/****************************** OnAbout ***************************************/

void
SDIFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
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
SDIFrame::OnEditMainPars(wxCommandEvent& WXUNUSED(event))
{
	if (!GetPtr_AppInterface()->parList)
		return;

	if (mainParDialog)
		return;
	mainParDialog = new ModuleParDialog(this, "Main Parameters", NULL,
	  GetPtr_AppInterface()->parList, NULL, 300, 300, 500, 500,
	  wxDEFAULT_DIALOG_STYLE);
	mainParDialog->SetNotebookSelection();
	printf("SDIFrame::OnEditMainPars: dialog created\n");
	mainParDialog->Show(TRUE);

}

/****************************** OnEditSimPars *********************************/

void
SDIFrame::OnEditSimPars(wxCommandEvent& WXUNUSED(event))
{
	ResetGUIDialogs();
	if (wxGetApp().StatusChanged()) {
		ResetStepCount_Utility_Datum();
		wxGetApp().CheckInitialisation();
	}
	printf("SDIFrame::OnEditSimPars: Open simulation view.\n");
//	if (!wxGetApp().GetSimModuleDialog()) {
//		wxGetApp().SetSimModuleDialog(new SimModuleDialog(this,
//		  "Simulation Parameters", -1, GetSimulation_ModuleMgr(
//		  GetPtr_AppInterface()->audModel), GetUniParListPtr_ModuleMgr(
//		  GetPtr_AppInterface()->audModel)));
//	}

}

/****************************** OnLoadSimFile *********************************/

void
SDIFrame::OnLoadSimFile(wxCommandEvent& event)
{
	ResetGUIDialogs();
	wxString extension = (event.GetId() == MYFRAME_ID_LOAD_SIM_PAR_FILE)?
	  "All files|*.*|Sim. Par Files (*.spf)|*.spf|Sim. scripts (*.sim)|*.sim": "*.sim";
	wxFileDialog dialog(this, "Choose a file", wxGetApp().defaultDir, "",
	  extension);
	if (dialog.ShowModal() == wxID_OK) {
		wxGetApp().defaultDir = dialog.GetDirectory();
		wxGetApp().simFilePath = dialog.GetPath();
		SetSimFileAndLoad();
	}

}

/****************************** OnReloadSimFile *******************************/

void
SDIFrame::OnReloadSimFile(wxCommandEvent& event)
{

	ResetGUIDialogs();
	SetSimFileAndLoad();

}

/****************************** OnSaveSimPars *********************************/

void
SDIFrame::OnSaveSimPars(wxCommandEvent& WXUNUSED(event))
{
	wxString newFilePath, fileName;

	fileName = wxFileNameFromPath(wxGetApp().simFilePath).BeforeLast('.') +
	  ".spf";
	newFilePath = wxFileSelector("Simulation parameter file", wxPathOnly(
	  wxGetApp().simFilePath), fileName, "spf", "*.spf", wxSAVE |
	  wxOVERWRITE_PROMPT | wxHIDE_READONLY, this);
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

}

/****************************** OnViewSimPars *********************************/

void
SDIFrame::OnViewSimPars(wxCommandEvent& WXUNUSED(event))
{
	FILE	*oldParsFile = GetDSAMPtr_Common()->parsFile;

	SetParsFile_Common("screen", OVERWRITE);
	ListParameters_AppInterface();
	GetDSAMPtr_Common()->parsFile = oldParsFile;

}

/****************************** OnDiagWindow *********************************/

void
SDIFrame::OnDiagWindow(wxCommandEvent& WXUNUSED(event))
{
	wxGetApp().OpenDiagWindow();

}

/****************************** OnSimThreadEvent ******************************/

void
SDIFrame::OnSimThreadEvent(wxCommandEvent& event)
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
  
/******************************************************************************/
/*************************** EditProcessMenu **********************************/
/******************************************************************************/

void
SDIFrame::OnEditProcess(wxCommandEvent& event)
{
	wxMenu menu("Edit Process");

	menu.Append(SDIFRAME_CUT, "&Cut");
	menu.AppendSeparator();
	menu.Append(SDIFRAME_EDIT_PROCESS_NAME, "&Process name");

	PopupMenu(&menu, 0, 0);

}

/******************************************************************************/
/*************************** OnSize *******************************************/
/******************************************************************************/

void
SDIFrame::OnSize(wxSizeEvent& event)
{
	if (canvas && palette) {
		int cw, ch;
		GetClientSize(&cw, &ch);
		int paletteX = 0;
		int paletteY = 0;
		int paletteW = 30;
		int paletteH = ch;
		int canvasX = paletteX + paletteW;
		int canvasY = 0;
		int canvasW = cw - paletteW;
		int canvasH = ch;

		palette->SetSize(paletteX, paletteY, paletteW, paletteH);
		canvas->SetSize(canvasX, canvasY, canvasW, canvasH);
	}

}

/******************************************************************************/
/*************************** OnCloseWindow ************************************/
/******************************************************************************/

void
SDIFrame::OnCloseWindow(wxCloseEvent& event)
{
	wxDocParentFrame::OnCloseWindow(event);
	if (!event.GetVeto())
		wxOGLCleanUp();

}

#endif /* HAVE_WX_OGL_H */