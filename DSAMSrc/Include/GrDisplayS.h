/******************
 *		
 * File:		GrDisplayS.h
 * Purpose: 	Display class module.
 * Comments:	It was created by extraction from the original GrSignalDisp
 *				module.
 *				23-06-98 LPO: Introduced display window resizing:
 *				DisplayS::OnSize
 * Author:		L.P.O'Mard
 * Created:		27 Jul 1997
 * Updated:		23 Jun 1998
 * Copyright:	(c) 1998, University of Essex
 *
 ******************/

#ifndef	_GRDISPLAYS_H
#define _GRDISPLAYS_H	1

/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

/*************************** Misc *********************************************/

#ifndef WXDIR
#	define	WXDIR						"$WXDIR"
#endif

#define	DISPLAY_PANEL_TEXT_LEN			5
#define	DISPLAY_MSW_PRINTER_X_SCALE		2.0
#define	DISPLAY_MSW_PRINTER_Y_SCALE		2.0

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Class definitions ********************************/
/******************************************************************************/


/*************************** MyCanvas prefreeference **************************/

class MyCanvas;

/*************************** DisplayS *****************************************/

class DisplayS: public wxFrame
{
	long	handle;
	SignalDispPtr	mySignalDispPtr;

 public:
	MyCanvas	*canvas;

	DisplayS(wxFrame *frame, long theHandle, SignalDispPtr signalDispPtr,
	  const wxString& title, const wxPoint& pos, const wxSize& size);
	~DisplayS(void);

	SignalDispPtr	GetSignalDispPtr(void) { return(mySignalDispPtr); }

	void	OnMove(wxMoveEvent& event);

	DECLARE_EVENT_TABLE()

};

/******************************************************************************/
/*************************** External variables *******************************/
/******************************************************************************/

#endif
