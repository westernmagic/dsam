/**********************
 *
 * File:		GrSDIView.cpp
 * Purpose: 	Implements view functionality for Simulation Design Interface
 * Comments:	Revised from Julian Smart's Ogledit/view.cpp
 * Author:		L.P.O'Mard
 * Created:		04 Nov 2002
 * Updated:		
 * Copyright:	(c) 2002, University of Essex
 *
 **********************/

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG_H */

#ifdef USE_WX_OGL

#ifdef __GNUG__
// #pragma implementation
#endif

// For compilers that support precompilation, includes "wx.h".
#include <wx/wxprec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/docview.h>
#include <wx/colordlg.h>
#include <wx/wxexpr.h>
#include <wx/cmdproc.h>

#if !wxUSE_DOC_VIEW_ARCHITECTURE
#error You must set wxUSE_DOC_VIEW_ARCHITECTURE to 1 in wx_setup.h!
#endif

#include "GeCommon.h"
#include "GeSignalData.h"
#include "GeEarObject.h"
#include "UtDatum.h"
#include "UtSSSymbols.h"
#include "UtSSParser.h"
#include "GrIPCServer.h"
#include "GrSDIPalette.h"
#include "GrSDICanvas.h"
#include "GrSDIFrame.h"
#include "GrSimMgr.h"
#include "GrSDIDiagram.h"
#include "GrSDIEvtHandler.h"
#include "GrSDIDoc.h"
#include "GrSDIView.h"
#include "GrSDICommand.h"

#include "GeUniParMgr.h"
#include "UtAppInterface.h"

/******************************************************************************/
/****************************** Bitmaps ***************************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Methods (Subroutines) *************************/
/******************************************************************************/

IMPLEMENT_DYNAMIC_CLASS(SDIView, wxView)

BEGIN_EVENT_TABLE(SDIView, wxView)
    EVT_MENU(SDIFRAME_CUT, SDIView::OnCut)
    EVT_MENU(SDIFRAME_CHANGE_BACKGROUND_COLOUR,
	  SDIView::OnChangeBackgroundColour)
    EVT_MENU(SDIFRAME_EDIT_MENU_PROPERTIES, SDIView::OnEditProperties)
    EVT_MENU(SDIFRAME_EDIT_PROCESS, SDIView::OnSetProcessLabel)
END_EVENT_TABLE()

/******************************************************************************/
/****************************** ProcessListDialog *****************************/
/******************************************************************************/

void
SDIView::ProcessListDialog(void)
{
	wxShape *theShape = FindSelectedShape();

	if (theShape) {
		wxArrayString	*procList = wxGetApp().GetProcessList(((SDIEvtHandler *)
		  theShape->GetEventHandler())->processType);
		wxSingleChoiceDialog dialog(theShape->GetCanvas(), "Select a process",
		  "Please select a process", procList->Count(), &procList->Item(0));

		if (dialog.ShowModal() == wxID_OK) {
			SDICanvas *canvas = (SDICanvas *) theShape->GetCanvas();
			canvas->view->GetDocument()->GetCommandProcessor()->Submit(
			  new SDICommand("Edit process name", SDIFRAME_EDIT_PROCESS,
			  (SDIDocument *) canvas->view->GetDocument(),
			  dialog.GetStringSelection(), theShape));
		
		}
	}

}

/******************************************************************************/
/****************************** EditCtrlProperties *************************/
/******************************************************************************/

void
SDIView::EditCtrlProperties(void)
{
	wxShape *theShape = FindSelectedShape();

	if (theShape) {
		DatumPtr	pc = ((SDIEvtHandler *) theShape->GetEventHandler())->pc;
		SDICanvas *canvas = (SDICanvas *) theShape->GetCanvas();

		switch (pc->type) {
		case REPEAT: {
			wxString	oldStrCount;
			oldStrCount.Printf("%d", pc->u.loop.count);
			wxString newStrCount = wxGetTextFromUser("Enter repeat count",
			  "Shape Control Par", oldStrCount);
			canvas->view->GetDocument()->GetCommandProcessor()->Submit(
			  new SDICommand("Edit control parameters", SDIFRAME_EDIT_PROCESS,
			  (SDIDocument *) canvas->view->GetDocument(), newStrCount,
			  theShape));
			break; }
		default:
			;
		} /* switch */
	}

}

/******************************************************************************/
/****************************** OnCreate **************************************/
/******************************************************************************/

// What to do when a view is created. Creates actual
// windows for displaying the view.

bool
SDIView::OnCreate(wxDocument *doc, long flags)
{
	frame = wxGetApp().GetFrame();
	//frame->palette = new EditorToolPalette(frame, wxPoint(0, 0), wxSize(-1,
	//  -1),wxTB_HORIZONTAL);
	canvas = frame->canvas;
	canvas->view = this;

	ResetGUIDialogs();

	SetFrame(frame);
	Activate(TRUE);

	// Initialize the edit menu Undo and Redo items
	doc->GetCommandProcessor()->SetEditMenu(((SDIFrame *) frame)->editMenu);
	doc->GetCommandProcessor()->Initialize();


	wxShapeCanvas *shapeCanvas = (wxShapeCanvas *)canvas;
	SDIDocument *diagramDoc = (SDIDocument *)doc;
	shapeCanvas->SetDiagram(diagramDoc->GetDiagram());
	diagramDoc->GetDiagram()->SetCanvas(shapeCanvas);

	return TRUE;
}

/******************************************************************************/
/****************************** OnDraw ****************************************/
/******************************************************************************/

#define CENTER  FALSE // Place the drawing to the center of the page


// Sneakily gets used for default print/preview
// as well as drawing on the screen. 
void
SDIView::OnDraw(wxDC *dc) 
{ 

	/* You might use THIS code if you were scaling 
	* graphics of known size to fit on the page. 
	*/ 
	int w, h; 

	// We need to adjust for the graphic size, a formula will be added 
	float maxX = 900; 
	float maxY = 700; 
	// A better way of find the maxium values would be to search through 
	// the linked list 

	// Let's have at least 10 device units margin 
	float marginX = 10; 
	float marginY = 10; 

	// Add the margin to the graphic size 
	maxX += (2 * marginX); 
	maxY += (2 * marginY); 

	// Get the size of the DC in pixels 
	dc->GetSize (&w, &h); 

	// Calculate a suitable scaling factor 
	float scaleX = (float) (w / maxX); 
	float scaleY = (float) (h / maxY); 

	// Use x or y scaling factor, whichever fits on the DC 
	float actualScale = wxMin (scaleX, scaleY); 

	float posX, posY; 
	// Calculate the position on the DC for centring the graphic 
	if (CENTER == TRUE) { // center the drawing 
		posX = (float) ((w - (200 * actualScale)) / 2.0); 
		posY = (float) ((h - (200 * actualScale)) / 2.0); 
	} else {   // Use defined presets 
		posX = 10; 
		posY = 35; 
	} 

	// Set the scale and origin 
	dc->SetUserScale (actualScale, actualScale); 
	dc->SetDeviceOrigin ((long) posX, (long) posY); 

	// This part was added to preform the print preview and printing functions 

	dc->BeginDrawing(); // Allows optimization of drawing code under MS Windows.

	// Get the current diagram
	wxDiagram *diagram_p=((SDIDocument*)GetDocument())->GetDiagram();
	if (diagram_p->GetShapeList()) { 
		/*wxCursor *old_cursor = NULL; */
		wxNode *current = diagram_p->GetShapeList()->First();

		while (current) {// Loop through the entire list of shapes 
			wxShape *object = (wxShape *)current->Data();
			if (!object->GetParent())
				object->Draw(* dc); // Draw the shape onto our printing dc
			current = current->Next();  // Procede to the next shape in the list
		}
	}
	dc->EndDrawing(); // Allows optimization of drawing code under MS Windows. 

}

/******************************************************************************/
/****************************** OnUpdate **************************************/
/******************************************************************************/

void
SDIView::OnUpdate(wxView *sender, wxObject *hint)
{

	if (canvas)
		canvas->Refresh();

}

/******************************************************************************/
/****************************** OnClose ***************************************/
/******************************************************************************/

// Clean up windows used for displaying the view.
bool
SDIView::OnClose(bool deleteWindow)
{
	if (!GetDocument()->Close())
		return FALSE;

	SDIDocument *diagramDoc = (SDIDocument *)GetDocument();
	diagramDoc->GetDiagram()->SetCanvas(NULL);

	canvas->Clear();
	canvas->SetDiagram(NULL);
	canvas->view = NULL;
	canvas = NULL;

	wxString s = wxTheApp->GetAppName();
	if (frame)
		frame->SetTitle(s);

	SetFrame((wxFrame *) NULL);

	Activate(FALSE);

	return TRUE;

}

/******************************************************************************/
/****************************** FindSelectedShape *****************************/
/******************************************************************************/

wxShape *
SDIView::FindSelectedShape(void)
{
	SDIDocument *doc = (SDIDocument *)GetDocument();
	wxShape *theShape = NULL;
	wxNode *node = doc->GetDiagram()->GetShapeList()->First();
	while (node) {
		wxShape *eachShape = (wxShape *)node->Data();
		if ((eachShape->GetParent() == NULL) && eachShape->Selected()) {
			theShape = eachShape;
			node = NULL;
		} else
			node = node->Next();
	}
	return theShape;
}

/******************************************************************************/
/****************************** OnCut *****************************************/
/******************************************************************************/

void
SDIView::OnCut(wxCommandEvent& event)
{
	SDIDocument *doc = (SDIDocument *)GetDocument();

	wxShape *theShape = FindSelectedShape();
	if (theShape)
		doc->GetCommandProcessor()->Submit(new SDICommand("Cut", SDIFRAME_CUT,
		  doc, NULL, -1, 0.0, 0.0, TRUE, theShape));

}

/******************************************************************************/
/****************************** OnChangeBackgroundColour **********************/
/******************************************************************************/

void
SDIView::OnChangeBackgroundColour(wxCommandEvent& event)
{
	SDIDocument *doc = (SDIDocument *)GetDocument();

	wxShape *theShape = FindSelectedShape();
	if (theShape) {
		wxColourData data;
		data.SetChooseFull(TRUE);
		data.SetColour(theShape->GetBrush()->GetColour());

		wxColourDialog *dialog = new wxColourDialog(frame, &data);
		wxBrush *theBrush = NULL;
		if (dialog->ShowModal() == wxID_OK) {
			wxColourData retData = dialog->GetColourData();
			wxColour col = retData.GetColour();
			theBrush = wxTheBrushList->FindOrCreateBrush(col, wxSOLID);
		}
		dialog->Close();

		if (theBrush)
			doc->GetCommandProcessor()->Submit(new SDICommand("Change colour",
			  SDIFRAME_CHANGE_BACKGROUND_COLOUR, doc, theBrush, theShape));
	}

}

/******************************************************************************/
/****************************** OnSetProcessLabel *****************************/
/******************************************************************************/

void
SDIView::OnSetProcessLabel(wxCommandEvent& WXUNUSED(event))
{
	ProcessListDialog();

}

/******************************************************************************/
/****************************** OnEditProperties ******************************/
/******************************************************************************/

void
SDIView::OnEditProperties(wxCommandEvent& WXUNUSED(event))
{
	printf("SDIView::OnEditProperties: Entered\n");
	wxShape *shape = FindSelectedShape();
	SDIEvtHandler *myHandler = (SDIEvtHandler *) shape->GetEventHandler();

	myHandler->ProcessProperties(shape->GetX(), shape->GetY());

}

#endif /* HAVE_WX_OGL_H */
