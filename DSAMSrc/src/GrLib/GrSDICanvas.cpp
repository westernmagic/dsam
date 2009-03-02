/**********************
 *
 * File:		GrSDICanvas.cpp
 * Purpose: 	Canvas clase for Simulation Design Interface .
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

#ifdef USE_WX_OGL

#include "ExtCommon.h"

#include <wx/colordlg.h>
#include <wx/docview.h>
#include <wx/cmdproc.h>

#include "GeCommon.h"
#include "GeSignalData.h"
#include "GeEarObject.h"
#include "UtDatum.h"
#include "GeUniParMgr.h"
#include "GeModuleMgr.h"
#include "GrIPCServer.h"
#include "GrSimMgr.h"
#include "GrSDIPalette.h"
#include "GrSDICanvas.h"
#include "GrSDIFrame.h"
#include "GrSDIDiagram.h"
#include "GrSDIDoc.h"
#include "GrSDICommand.h"
#include "GrSDIBaseShapes.h"
#include "GrSDIShapes.h"

/******************************************************************************/
/****************************** Bitmaps ***************************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Methods (Subroutines) *************************/
/******************************************************************************/

/*
 * Window implementations
 */

BEGIN_EVENT_TABLE(SDICanvas, wxShapeCanvas)
    EVT_MOUSE_EVENTS(SDICanvas::OnMouseEvent)
    EVT_PAINT(SDICanvas::OnPaint)
END_EVENT_TABLE()

/******************************************************************************/
/*************************** Constructor **************************************/
/******************************************************************************/

SDICanvas::SDICanvas(wxView *v, wxWindow *theParent, wxWindowID id,
  const wxPoint& pos, const wxSize& size, long style):
 wxShapeCanvas(theParent, id, pos, size, style)
{
    parent = theParent;
    //palette = ((SDIFrame *) parent)->palette;
 	SetCursor(wxCursor(wxCURSOR_HAND));
    SetScrollbars(20, 20, 50, 50);
	SetBackgroundColour(*wxWHITE);
	view = v;

}

/******************************************************************************/
/*************************** Destructor ***************************************/
/******************************************************************************/

SDICanvas::~SDICanvas(void)
{
}

/******************************************************************************/
/*************************** GetClassInfo *************************************/
/******************************************************************************/

wxClassInfo *
SDICanvas::GetClassInfo(int classSpecifier)
{
	/*static const char *funcName = "SDICanvas::GetClassInfo";*/

	switch (classSpecifier) {
	case ANALYSIS_MODULE_CLASS:
		return(CLASSINFO(SDIAnalysisShape));
	case CONTROL_MODULE_CLASS:
		return(CLASSINFO(SDIControlShape));
	case DISPLAY_MODULE_CLASS:
		return(CLASSINFO(SDIDisplayShape));
	case FILTER_MODULE_CLASS:
		return(CLASSINFO(SDIFilterShape));
	case IO_MODULE_CLASS:
		return(CLASSINFO(SDIIOShape));
	case MODEL_MODULE_CLASS:
		return(CLASSINFO(SDIModelShape));
	case TRANSFORM_MODULE_CLASS:
		return(CLASSINFO(SDITransformShape));
	case USER_MODULE_CLASS:
		return(CLASSINFO(SDIUserShape));
	case UTILITY_MODULE_CLASS:
		return(CLASSINFO(SDIUtilityShape));
	default:
		;
	}
	return(NULL);

}

/******************************************************************************/
/*************************** OnLeftClick **************************************/
/******************************************************************************/

void
SDICanvas::OnLeftClick(double x, double y, int keys)
{
	EditorToolPalette *palette = ((SDIFrame *) parent)->palette;
	wxClassInfo *info = GetClassInfo(palette->currentlySelected);

	wprintf(wxT("SDICanvas::OnLeftClick: Entered.\n"));
	if (info) {
		view->GetDocument()->GetCommandProcessor()->Submit(new SDICommand(
		  (wxChar *) info->GetClassName(), SDIFRAME_ADD_SHAPE, (SDIDocument *)
		  view->GetDocument(), info, palette->currentlySelected, x, y));
	}
}

/******************************************************************************/
/*************************** OnRightClick *************************************/
/******************************************************************************/

void
SDICanvas::OnRightClick(double x, double y, int keys)
{

}

void SDICanvas::OnDragLeft(bool draw, double x, double y, int keys)
{
}

void SDICanvas::OnBeginDragLeft(double x, double y, int keys)
{
}

void SDICanvas::OnEndDragLeft(double x, double y, int keys)
{
}

void SDICanvas::OnDragRight(bool draw, double x, double y, int keys)
{
}

void SDICanvas::OnBeginDragRight(double x, double y, int keys)
{
}

void SDICanvas::OnEndDragRight(double x, double y, int keys)
{
}

/******************************************************************************/
/*************************** OnMouseEvent *************************************/
/******************************************************************************/

void
SDICanvas::OnMouseEvent(wxMouseEvent& event)
{
	if (!wxGetApp().GetAudModelLoadedFlag())
		return;
    wxShapeCanvas::OnMouseEvent(event);
}

/******************************************************************************/
/*************************** OnPaint ******************************************/
/******************************************************************************/

void
SDICanvas::OnPaint(wxPaintEvent& event)
{
//  if (GetDiagram())
    wxShapeCanvas::OnPaint(event);
}

#endif /* HAVE_WX_OGL_H */
