/**********************
 *
 * File:		GrSDICanvas.h
 * Purpose: 	Canvas clase for Simulation Design Interface .
 * Comments:	
 * Author:		L.P.O'Mard
 * Created:		11 Nov 2002
 * Updated:		
 * Copyright:	(c) 2002, CNBH, University of Essex
 *
 **********************/

#ifndef _GRSDICANVAS_H
#define _GRSDICANVAS_H 1

#ifdef __GNUG__
// #pragma interface
#endif

#include <wx/ogl/ogl.h>

/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Enum definitions *********************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Class definitions ********************************/
/******************************************************************************/

class SDICanvas: public wxShapeCanvas
{
// DECLARE_DYNAMIC_CLASS(wxShapeCanvas)
  protected:
  public:
	wxView	*view;
  	wxWindow	*parent;

	SDICanvas(wxView *view, wxWindow *theParent = NULL, wxWindowID id = -1,
	  const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
	  long style = wxRETAINED);
	~SDICanvas(void);

	void OnMouseEvent(wxMouseEvent& event);
	void OnPaint(wxPaintEvent& event);

	virtual void OnLeftClick(double x, double y, int keys = 0);
	virtual void OnRightClick(double x, double y, int keys = 0);

	virtual void OnDragLeft(bool draw, double x, double y, int keys=0); // Erase if draw false
	virtual void OnBeginDragLeft(double x, double y, int keys=0);
	virtual void OnEndDragLeft(double x, double y, int keys=0);

	virtual void OnDragRight(bool draw, double x, double y, int keys=0); // Erase if draw false
	virtual void OnBeginDragRight(double x, double y, int keys=0);
	virtual void OnEndDragRight(double x, double y, int keys=0);

	DECLARE_EVENT_TABLE()
};

#endif
