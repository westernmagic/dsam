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
	SDIFRAME_CHANGE_BACKGROUND_COLOUR

};

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Class definitions ********************************/
/******************************************************************************/

class SDIFrame: public wxDocChildFrame
{
	DECLARE_CLASS(SDIFrame)
  public:
	wxMenu *editMenu;

	SDICanvas	*canvas;
	EditorToolPalette *palette;

	SDIFrame(wxDocument *doc, wxView *view, wxFrame *parent,
	  const wxString& title, const wxPoint& pos = wxDefaultPosition,
	  const wxSize& size = wxDefaultSize, long style = wxDEFAULT_FRAME_STYLE);

	void	OnSize(wxSizeEvent& event);
	void	OnCloseWindow(wxCloseEvent& event);
	void	OnAbout(wxCommandEvent& event);

	DECLARE_EVENT_TABLE()
};

#endif
