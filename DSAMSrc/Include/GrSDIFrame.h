/**********************
 *
 * File:		GrSDIFrame.h
 * Purpose: 	Frame class for Simulation Design Interface .
 * Comments:	
 * Author:		L.P.O'Mard
 * Created:		11 Nov 2002
 * Updated:		
 * Copyright:	(c) 2002, CNBH, University of Essex
 *
 **********************/

#ifndef _GRSDIFRAME_H
#define _GRSDIFRAME_H 1

#ifdef __GNUG__
// #pragma interface
#endif

/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Enum definitions *********************************/
/******************************************************************************/

enum {

	SDIFRAME_CUT = 1,
	SDIFRAME_ABOUT,
	SDIFRAME_ADD_SHAPE,
	SDIFRAME_ADD_LINE,
	SDIFRAME_EDIT_LABEL,
	SDIFRAME_DIAG_WINDOW,
	SDIFRAME_CHANGE_BACKGROUND_COLOUR

};

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

/********************************** Pre-references ****************************/

class ModuleParDialog;

/******************************************************************************/
/*************************** Class definitions ********************************/
/******************************************************************************/

class SDIFrame: public wxDocParentFrame
{
	DECLARE_CLASS(SDIFrame)

	ModuleParDialog	*mainParDialog;

  public:
	wxMenu *editMenu;

	SDICanvas	*canvas;
	EditorToolPalette *palette;

	SDIFrame(wxDocManager *manager, wxFrame *parent, const wxString& title,
	  const wxPoint& pos = wxDefaultPosition, const wxSize& size =
	  wxDefaultSize, long style = wxDEFAULT_FRAME_STYLE);
	~SDIFrame(void);

	SDICanvas *	CreateCanvas(wxView *view, wxFrame *parent);
	void	DeleteMainParDialog(void);
	void	OnAbout(wxCommandEvent& event);
	void	OnCloseWindow(wxCloseEvent& event);
	void	OnDiagWindow(wxCommandEvent& event);
	void	OnExecute(wxCommandEvent& event);
	void	OnEditMainPars(wxCommandEvent& event);
	void	OnEditSimPars(wxCommandEvent& event);
	void	OnHelp(wxCommandEvent& event);
	void	OnLoadSimFile(wxCommandEvent& event);
	void	OnReloadSimFile(wxCommandEvent& event);
	void	OnQuit(wxCommandEvent& event);
	void	OnSaveSimPars(wxCommandEvent& event);
	void	OnSimThreadEvent(wxCommandEvent& event);
	void	OnStopSimulation(wxCommandEvent& event);
	void	OnViewSimPars(wxCommandEvent& event);
	void	OnSize(wxSizeEvent& event);
	void	SetMainParDialog(ModuleParDialog *dlg) { mainParDialog = dlg; }
	void	SetSimFileAndLoad(void);
	void	UpdateMainParDialog(void);

	DECLARE_EVENT_TABLE()

};

#endif
