/**********************
 *
 * File:		GrCanvas.h
 * Purpose: 	Canvas routines for drawing graphs.
 * Comments:	27-10-97 LPO: Summary EarObject is only set up when the
 *				summary display mode is on (default).
 *				26-05-98 LPO: Summary line's maximum and minimum Y limits were
 *				not being calculated.
 *				23-06-98 LPO: memDC should not be deleted, as it is not a 
 *				pointer to which memory is allocated, and xAxis and yAxis were
 *				being deleted twice!
 *				24-06-98 LPO: Corrected display resizing. The memory bitmap is
 *				now independent of the frame size.
 *				08-07-98 LPO: Corrected display resizing.  It now works under
 *				MSW and it has no "bitmap size" maximum limit.
 *				There seems to be a difference between the X and MSW behaviour.
 * Author:		L.P.O'Mard
 * Created:		06 Jan 1995
 * Updated:		08 Jul 1998
 * Copyright:	(c) 1998, University of Essex
 *
 **********************/

#ifndef _GRCANVAS_H
#define _GRCANVAS_H 1

#include "GrLine.h"

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define	CANVAS_BITMAP_WIDTH		1000
#define	CANVAS_BITMAP_HEIGHT	1000

#define	CANVAS_PS_X_OFFSET		50.0
#define	CANVAS_PS_Y_OFFSET		50.0

/* The following is needed because of errors in scale position - can't get
 * Get extent to work properly.
 */
#if defined(wx_xt)
#	define CANVAS_TEXT_X_ADJUST		1.5
#	define CANVAS_TEXT_Y_ADJUST		0.5
#else
#	define CANVAS_TEXT_X_ADJUST		1.0
#	define CANVAS_TEXT_Y_ADJUST		1.0
#endif

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Class definitions *****************************/
/******************************************************************************/

/********************************** MyCanvas **********************************/

// Define a new canvas which can receive some events

class MyCanvas: public wxWindow
{
	BOOLN	useTextAdjust;
	int		yAxisTicks, numChannels;
	double	dt, outputTimeOffset;
	ChanLen	chanLength, timeIndex;
	Box		signal, summary, *originalCanvas, *xAxis, *yAxis;
	wxFont  *labelFont, *axisTitleFont;
	wxString	xTitle, yTitle;
	wxMemoryDC	memDC;
	wxBitmap	*memBmp;
	MultiLine	*signalLines, *summaryLine;

  public:
	SignalCtrls		signalCtrl;
	AxisCtrls		axisCtrl;

	MyCanvas(wxFrame *frame, const wxPoint& pos = wxDefaultPosition,
	  const wxSize& size = wxDefaultSize, long style = 0);
	~MyCanvas(void);

	void	InitData(EarObjectPtr data);
	void	InitGraph(EarObjectPtr data, EarObjectPtr summaryEarO,
			  double xResolution);
	void	SetGraphAreas(void);
	void	SetLines(MultiLine *lines, EarObjectPtr data, Box box,
			  float xResolution);
	void	SetRasterLines(MultiLine *lines, EarObjectPtr data, Box box,
			  float xResolution);
	void	SetTextAdjust(float *xAdjust, float *yAdjust);
	void	SetUseTextAdjust(BOOLN state)	{ useTextAdjust = state; };
	void	DrawAxes(wxDC& dc, float theXOffset, float theYOffset);
	void	DrawGraph(wxDC& dc, double theXOffset, double theYOffset);
	void	DrawVerticalText(wxDC& dc, wxString& string, float x, float y);

 	// private:
	void	OnPaint(void); // Set back to private when possible.

	DECLARE_EVENT_TABLE()

};

/******************************************************************************/
/*************************** External Variables *******************************/
/******************************************************************************/

#endif

