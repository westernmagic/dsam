/**********************
 *
 * File:		GrDiagFrame.h
 * Purpose: 	Frame class for diagnostic output.
 * Comments:	
 * Author:		L.P.O'Mard
 * Created:		21 Nov 2002
 * Updated:		
 * Copyright:	(c) 2002, CNBH, University of Essex
 *
 **********************/

#ifndef _GRDIAGFRAME_H
#define _GRDIAGFRAME_H 1

#ifdef __GNUG__
// #pragma interface
#endif

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

class DiagFrame: public wxFrame
{

  public:
	wxMenu *fileMenu;
	wxTextCtrl	*diagnosticsText;
	wxScrolledWindow   *scrolledWindow;

	DiagFrame(wxFrame *parent, const wxString& title, const wxPoint& pos =
	  wxDefaultPosition, const wxSize& size = wxDefaultSize, long style =
	  wxDEFAULT_FRAME_STYLE | wxVSCROLL);

	void	Clear(void)		{ diagnosticsText->Clear(); }
	void	OnCloseWindow(wxCloseEvent& event);
	void	OnSize(wxSizeEvent& event);


	DECLARE_DYNAMIC_CLASS(DiagFrame)
	DECLARE_EVENT_TABLE()

};

/******************************************************************************/
/*************************** Subroutine declarations **************************/
/******************************************************************************/

#endif
