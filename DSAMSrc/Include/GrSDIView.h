/**********************
 *
 * File:		GrSDIView.h
 * Purpose: 	Implements view functionality for Simulation Design Interface
 * Comments:	Revised from Julian Smart's Ogledit/view.h
 * Author:		L.P.O'Mard
 * Created:		04 Nov 2002
 * Updated:		
 * Copyright:	(c) 2002, University of Essex
 *
 **********************/

#ifndef _GRSDIVIEW_H
#define _GRSDIVIEW_H 1

#ifdef __GNUG__
// #pragma interface "GrSDIView.h"
#endif

#include "GrSDIDoc.h"
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

class SDIView: public wxView
{
	DECLARE_DYNAMIC_CLASS(SDIView)

  public:
	SDIFrame *frame;
	SDICanvas *canvas;

	SDIView(void) { canvas = NULL; frame = NULL; };
	~SDIView(void) {};

	SDICanvas *	CreateCanvas(wxView *view, wxFrame *parent);
	void	EditCtrlProperties(void);
	wxShape	*FindSelectedShape(void);
	bool	OnCreate(wxDocument *doc, long flags);
	void	OnDraw(wxDC *dc);
	void	OnUpdate(wxView *sender, wxObject *hint = NULL);
	bool	OnClose(bool deleteWindow = TRUE);

	void	ProcessListDialog(void);

	//  void OnMenuCommand(int cmd);

	void	OnCut(wxCommandEvent& event);
	void	OnChangeBackgroundColour(wxCommandEvent& event);
	void	OnEditProperties(wxCommandEvent& event);
	void	OnSetProcessLabel(wxCommandEvent& event);

	DECLARE_EVENT_TABLE()
};

#endif
