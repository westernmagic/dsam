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

#include "GrLines.h"
#include "GrAxisScale.h"

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define	GRAPH_RIGHT_MARGIN_SCALE			0.02
#define	GRAPH_TOP_MARGIN_PERCENT			5.0

#define	GRAPH_Y_TITLE_SCALE					0.025
#define	GRAPH_Y_TITLE_MARGIN_SCALE			0.05
#define	GRAPH_Y_AXIS_SCALE					0.2
#define	GRAPH_Y_LABELS_X_OFFSET_SCALE		0.04
#define	GRAPH_Y_INSET_SCALE_OFFSET_SCALE	0.55
#define	GRAPH_Y_INSET_SCALE_HEIGHT_SCALE	0.1
#define GRAPH_Y_INSET_SCALE_UPPER_EXP_LIMIT	2

#define	GRAPH_X_AXIS_SCALE					0.13
#define	GRAPH_X_TITLE_SCALE					0.03
#define	GRAPH_X_TITLE_MARGIN_SCALE			0.02
#define	GRAPH_X_LABELS_Y_OFFSET_SCALE		0.04

#define GRAPH_EXPONENT_VS_LABEL_SCALE		0.7
#define GRAPH_EXPONENT_LENGTH				4
#define GRAPH_INSET_VS_LABEL_SCALE			0.6

#if defined(__WXMSW__)
#	define	GRAPH_AXIS_TITLE_SCALE			0.3
#	define	GRAPH_AXIS_LABEL_SCALE			0.03
#else
#	define	GRAPH_AXIS_TITLE_SCALE			0.40
#	define	GRAPH_AXIS_LABEL_SCALE			0.035
#endif /* __WXMSW__ */

#define	GRAPH_X_TICK_LENGTH_SCALE			0.08
#define	GRAPH_Y_TICK_LENGTH_SCALE			0.04
#define GRAPH_SUMMARY_SIGNAL_SCALE			0.14
#define	GRAPH_SIGNAL_PEN_WIDTH				1
#define	GRAPH_NUM_GREY_SCALES				10

/* The following is needed because of errors in scale position - can't get
 * Get extent to work properly.
 */
#if defined(__WXGTK__)
#	define CANVAS_TEXT_X_ADJUST		1.0
#	define CANVAS_TEXT_Y_ADJUST		0.5
#else
#	define CANVAS_TEXT_X_ADJUST		1.0
#	define CANVAS_TEXT_Y_ADJUST		1.0
#endif

/************************** ID's for the menu commands ************************/

enum {

	DISPLAY_MENU_PRINT = 1,
	DISPLAY_MENU_PREFERENCES,
	DISPLAY_MENU_EXIT

};

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
	bool	useTextAdjust, firstSizeEvent;
	int		bitmapWidth, bitmapHeight;
	double	dt;
	ChanLen	chanLength, timeIndex;
	wxFont  *labelFont, *insetLabelFont, *axisTitleFont, *superLabelFont;
	wxRect	signal, summary, *xAxis, *yAxis;
	wxFrame	*parent;
	GrLines	*signalLines, *summaryLine;
	wxString	xTitle, yTitle;
	wxMemoryDC	memDC;
	wxBitmap	*memBmp;
	AxisScale	xAxisScale, yAxisScale;
	SignalDispPtr	mySignalDispPtr;

  public:

	MyCanvas(wxFrame *frame, SignalDispPtr theSignalDispPtr);
	~MyCanvas(void);

	void	CreateBackingBitmap(void);
	void	DrawGraph(wxDC& dc, int theXOffset, int theYOffset);
	void	DrawExponent(wxDC& dc, wxFont *labelFont, int exponent, int x,
			  int y);
	void	DrawXAxis(wxDC& dc, int theXOffset, int theYOffset);
	void	DrawYAxis(wxDC& dc, int theXOffset, int theYOffset);
	void	DrawYScale(wxDC& dc, AxisScale &yAxisScale, wxRect *yAxisRect,
			  wxFont *labelFont, int theXOffset, int theYOffset, int yTicks,
			  int numDisplayedChans, double minYValue, double maxYValue);
	int		GetMinimumIntLog(double value);
	GrLines *GetSignalLines(void)		{ return signalLines; }
	void	InitData(EarObjectPtr data);
	void	InitGraph(void);
	SignalDispPtr GetSignalDispPtr(void)	{ return mySignalDispPtr; }

	void	OnCloseWindow(wxCloseEvent& event);
	void	OnPreferences(wxCommandEvent& event);
	void	OnPrint(wxCommandEvent& event);
	void	OnQuit(wxCommandEvent& event);
	void	OnRightDown(wxMouseEvent &event);
	void	OnSize(wxSizeEvent& event);

	void	RedrawGraph(void);
	void	RescaleGraph(void);
	void	ResetGrLines(GrLines **lines, SignalDataPtr signal,
			  int channelStep);
	void	SetGraphAreas(void);
	void	SetGraphPars(void);
	void	SetLines(GrLines *lines);
	void	SetGreyScaleLines(GrLines *lines);
	void	SetTextAdjust(double *xAdjust, double *yAdjust);
	void	SetUseTextAdjust(bool state)	{ useTextAdjust = state; };

 	private:
	void	OnPaint(wxPaintEvent& event);

	DECLARE_EVENT_TABLE()

};

/******************************************************************************/
/*************************** External Variables *******************************/
/******************************************************************************/

#endif

