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

/*************************** MDI **********************************************/

#define MDI_QUIT        1
#define MDI_NEW_WINDOW  2
#define MDI_REFRESH     3
#define MDI_CHILD_QUIT  4
#define MDI_ABOUT       5

/*************************** Menu Commands ************************************/

#define DISPLAY_QUIT			101
#define DISPLAY_PRINT_EPS		102

/*************************** Button Tags **************************************/

#define	DISPLAY_SIGNAL_BUTTON_TAG		200
#define	DISPLAY_SCALE_BUTTON_TAG		201
#define	DISPLAY_PRINTER_BUTTON_TAG		202
#define	DISPLAY_QUIT_BUTTON_TAG			203

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

/*************************** DisplayS *****************************************/

class DisplayS: public wxFrame
{
  private:
	MyButton *printerBtn;
	MyButton *scaleBtn;
	MyButton *signalBtn;
	MyButton *quitBtn;

 public:
	MyCanvas	*canvas;
	wxPanel		*panel;

	DisplayS(wxFrame *frame, char *title, int x, int y, int w, int h, int type,
	  long handle);
	~DisplayS(void);
	
	void	InitButtons(void);
	void	OnActivate(Bool active);
	Bool	OnClose(void);
	void	OnSize(int width, int height);
	
  // Call backs
	void	ButtonProc(wxButton& but, wxCommandEvent& event);

};

/******************************************************************************/
/*************************** External variables *******************************/
/******************************************************************************/

#endif
