/**********************
 *
 * File:		GrCanvas.cpp
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

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG_H */

#ifdef __GNUG__
#pragma implementation
#pragma interface
#endif

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>

#if defined(GRAPHICS_SUPPORT) && defined(__cplusplus)
// For compilers that support precompilation, includes "wx.h".
#	include <wx/wxprec.h>

#	ifdef __BORLANDC__
 	   #pragma hdrstop
#	endif

// Any files you want to include if not precompiling by including
// the whole of <wx/wx.h>
#	ifndef WX_PRECOMP
#		include <wx/wx.h>
#	endif

// Any files included regardless of precompiled headers
#	include "wx/print.h"
#	include <wx/notebook.h>
#	include "wx/image.h"
#endif

#include "GeCommon.h"
#include "GeSignalData.h"
#include "GeEarObject.h"
#include "GeUniParMgr.h"
#include "GrUtils.h"
#include "GrBrushes.h"
#include "GrSignalDisp.h"
#include "GrAxisScale.h"
#include "GrCanvas.h"
#include "GrDisplayS.h"
#include "GrPrintDisp.h"
#include "GrModParDialog.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Event tables *************************************/
/******************************************************************************/

BEGIN_EVENT_TABLE(MyCanvas, wxWindow)
	EVT_MENU(DISPLAY_MENU_PRINT,		MyCanvas::OnPrint)
	EVT_MENU(DISPLAY_MENU_PREFERENCES,	MyCanvas::OnPreferences)
	EVT_MENU(DISPLAY_MENU_EXIT,			MyCanvas::OnQuit)
	EVT_PAINT(	MyCanvas::OnPaint)
	EVT_CLOSE(	MyCanvas::OnCloseWindow)
	EVT_SIZE(	MyCanvas::OnSize)
    EVT_RIGHT_DOWN(	MyCanvas::OnRightDown)
    EVT_MOTION(	MyCanvas::OnMouseMove)
END_EVENT_TABLE()

/******************************************************************************/
/****************************** Methods ***************************************/
/******************************************************************************/

/****************************** Constructor ***********************************/

/*
 * Define a constructor for my canvas.
 */

MyCanvas::MyCanvas(wxFrame *frame, SignalDispPtr theSignalDispPtr):
  wxWindow(frame, -1)
{
	/* static const char *funcName = "MyCanvas::MyCanvas"; */

	useTextAdjust = FALSE;
	firstSizeEvent = TRUE;
	offset = 0;
	chanLength = 0;

	parent = frame;
	mySignalDispPtr = theSignalDispPtr;

	memBmp = NULL;
	CreateBackingBitmap();
	
	labelFont = new wxFont(14, wxROMAN, wxNORMAL, wxNORMAL);
	superLabelFont = new wxFont(14, wxROMAN, wxNORMAL, wxNORMAL);
	insetLabelFont = new wxFont(14, wxROMAN, wxNORMAL, wxNORMAL);
	axisTitleFont = new wxFont(14, wxROMAN, wxNORMAL, wxBOLD);
	xAxis = new wxRect();
	yAxis = new wxRect();

	SetToolTip("Press right mouse button for menu");

}

/****************************** Destructor ************************************/

MyCanvas::~MyCanvas(void)
{	
	if (xAxis)
		delete xAxis;
	if (yAxis)
		delete yAxis;
	if (memBmp) {
		memDC.SelectObject(wxNullBitmap);
		delete memBmp;
	}

}

/****************************** CreateBackingBitmap ***************************/

// This routine creates the backing bitmap and sets its associated parameters.

void
MyCanvas::CreateBackingBitmap(void)
{
	if (memBmp) {
		memDC.SelectObject(wxNullBitmap);
		delete memBmp;
	}
	parent->GetClientSize(&bitmapWidth, &bitmapHeight);
	memBmp = new wxBitmap(bitmapWidth, bitmapHeight);
	memDC.SelectObject(*memBmp);
	memDC.Clear();	
	
}

/****************************** SetGraphAreas *********************************/

/*
 * This routine sets the graph areas, i. signal space, axis space etc.
 */

void
MyCanvas::SetGraphAreas(void)
{
	int		xAxisSpace, yAxisSpace, summarySpace, topMargin;
	
	wxRect	graph(wxDefaultPosition, parent->GetClientSize());

	graph.SetWidth((int) (graph.GetWidth() * (1.0 - GRAPH_RIGHT_MARGIN_SCALE)));
	topMargin = (int) (graph.GetHeight() * mySignalDispPtr->topMargin / 100.0);
	graph.SetY(graph.GetY() + topMargin);
	graph.SetHeight(graph.GetHeight() - topMargin);

	// Graph element dimensions

	xAxisSpace = (xAxis)? (int) floor(graph.GetHeight() * (GRAPH_X_AXIS_SCALE +
	  GRAPH_X_TITLE_SCALE) + 0.5): 0;
	yAxisSpace = (yAxis)? (int) floor(graph.GetWidth() * (GRAPH_Y_AXIS_SCALE +
	  GRAPH_Y_TITLE_SCALE) + 0.5): 0;
	summarySpace = (int) (graph.GetHeight() * GRAPH_SUMMARY_SIGNAL_SCALE);

	if (xAxis) {
		graph.SetHeight(graph.GetHeight() - xAxisSpace);
		xAxis->SetX(graph.GetX() + yAxisSpace);
		xAxis->SetY(graph.GetBottom() + 1);
		xAxis->SetHeight(xAxisSpace);
		xAxis->SetWidth(graph.GetWidth() - yAxisSpace);
	}

	if (yAxis) {
		*yAxis = graph;
		yAxis->SetWidth(yAxisSpace);
		yAxis->SetHeight(yAxis->GetHeight() -
		  ((mySignalDispPtr->summaryDisplay)? summarySpace: 0));
		graph.SetWidth(graph.GetWidth() - yAxisSpace);
		graph.SetX(yAxis->GetRight() + 1);
	}
	// Summary signal dimensions
	summary = graph;
	summary.SetHeight(summarySpace);
	summary.SetY(graph.GetBottom() - summary.GetHeight());
	if (mySignalDispPtr->summaryDisplay)
		graph.SetBottom(summary.GetTop() + 1);
	
	// Signal drawing dimensions
	signal = graph;

}

/****************************** InitGraph *************************************/

/* 
 * This routine initialises the graph, i.e. scaling etc.
 * The minYFlag and maxYFlag signalDisp parameters are set manually when the
 * 'minY' and 'maxy' variables.
 */

void
MyCanvas::InitGraph(void)
{
	SetGraphAreas();
	InitData(mySignalDispPtr->data);
	signalLines.SetChannelStep(mySignalDispPtr->channelStep);
	signalLines.SetYMagnification(mySignalDispPtr->magnification);
	summaryLine.SetChannelStep(1);
	greyBrushes->SetGreyScales(mySignalDispPtr->numGreyScales);

	if (mySignalDispPtr->autoYScale) {
		signalLines.CalcMaxMinLimits();
		mySignalDispPtr->minY = signalLines.GetMinY();
		mySignalDispPtr->minYFlag = TRUE;
		mySignalDispPtr->maxY = signalLines.GetMaxY();
		mySignalDispPtr->maxYFlag = TRUE;
		summaryLine.CalcMaxMinLimits();
	} else {
		signalLines.SetYLimits(mySignalDispPtr->minY, mySignalDispPtr->maxY);
		summaryLine.SetYLimits(mySignalDispPtr->minY, mySignalDispPtr->maxY);
	}
	
	switch (mySignalDispPtr->mode) {
	case GRAPH_MODE_LINE:
		SetLines(signalLines);
		SetLines(summaryLine);
		break;
	case GRAPH_MODE_GREY_SCALE:
		SetGreyScaleLines(signalLines);
		SetGreyScaleLines(summaryLine);
		break;
	} /* switch */
	signalLines.Rescale(signal);
	summaryLine.Rescale(summary);

}

/****************************** InitData **************************************/

/*
 * This method initialises the signal lines and summary line graphs.
 */

void
MyCanvas::InitData(EarObjectPtr data)
{
	SignalDataPtr signal = data->outSignal;

	if (mySignalDispPtr->autoXScale) {
		offset = 0;
		chanLength = signal->length;
	} else {
		offset = (ChanLen) floor(mySignalDispPtr->xOffset / signal->dt + 0.5);
		chanLength = (mySignalDispPtr->xExtent < DBL_EPSILON)? signal->length -
		  offset: (ChanLen) floor((mySignalDispPtr->xExtent) / signal->dt +
		  0.5);
	}
		
	timeIndex = _WorldTime_EarObject(data);
	dt = data->outSignal->dt;
	signalLines.Set(data->outSignal, offset, chanLength);
	summaryLine.Set(mySignalDispPtr->summary->outSignal, offset, chanLength);
		/* Tidy up, using wxString - when wxString works. */
	if (strcmp(xTitle, mySignalDispPtr->xAxisTitle) != 0)
		xTitle.Printf("%s", mySignalDispPtr->xAxisTitle);
	if (strcmp(yTitle, mySignalDispPtr->yAxisTitle) != 0)
		yTitle.Printf("%s", mySignalDispPtr->yAxisTitle);

}

/****************************** SetLines **************************************/

/*
 * This method sets the lists of points from the output signal of an EarObject.
 * The routine uses the minYRecord array to ensure that channels that are
 * "underneath" do not show through the "upper" channels.
 * 
 */

void
MyCanvas::SetLines(GrLines &lines)
{

	lines.SetGreyScaleMode(FALSE);
	lines.SetXResolution(mySignalDispPtr->xResolution);
	lines.SetYNormalisationMode(mySignalDispPtr->yNormalisationMode);

}

/****************************** SetGreyScaleLines *****************************/

/*
 * This method sets the lists of points from the output signal of an EarObject.
 * The points are used to draw the filled rectangles for the GreyScale display.
 */

void
MyCanvas::SetGreyScaleLines(GrLines &lines)
{

	lines.SetGreyScaleMode(TRUE);
	lines.SetNumGreyScales(mySignalDispPtr->numGreyScales);

}

/****************************** UseTextAdjust *********************************/

/*
 * This routine is a temporary measure to get the correct scale positioning
 * using the wx_xt port.
 */

void
MyCanvas::SetTextAdjust(double *xAdjust, double *yAdjust)
{
	if (useTextAdjust) {
		*xAdjust = CANVAS_TEXT_X_ADJUST;
		*yAdjust = CANVAS_TEXT_Y_ADJUST;
	} else {
		*xAdjust = 1.0;
		*yAdjust = 1.0;
	}

}

/****************************** DrawXAxis *************************************/

/*
 * The "mySignalDispPtr->autoXScale == TRUE" code was put in to remove compiler
 * warnings with MSVC.
 */

void
MyCanvas::DrawXAxis(wxDC& dc, int theXOffset, int theYOffset)
{
	static const char *funcName = "MyCanvas::DrawXAxis";
	int		i, tickLength, yPos, xPos, xTitlePos, yTitlePos;
	long int	stringWidth, stringHeight;
	double	tempXAdjust, tempYAdjust, xValue, outputTimeOffset;
	ChanLen	displayLength;
	wxString stringNum, label, space = " ";

	if (!xAxis)
		return;

	dc.SetPen(*wxBLACK_PEN);
	SetTextAdjust(&tempXAdjust, &tempYAdjust);
	dc.SetFont(*labelFont);
	dc.DrawLine(xAxis->GetLeft() + theXOffset,  xAxis->GetTop() + theYOffset,
	  xAxis->GetRight() + theXOffset, xAxis->GetTop() + theYOffset);
	outputTimeOffset = signalLines.GetSignalPtr()->outputTimeOffset +
	  ((mySignalDispPtr->autoXScale)? 0.0: mySignalDispPtr->xOffset);
	displayLength = timeIndex + chanLength - 1;
	if (mySignalDispPtr->mode == GRAPH_MODE_GREY_SCALE) {
		outputTimeOffset -= dt;
		displayLength++;
	}
	if (!xAxisScale.Set(mySignalDispPtr->xNumberFormat, outputTimeOffset +
	  timeIndex * dt, outputTimeOffset + displayLength * dt, xAxis->GetLeft(),
	  xAxis->GetRight(), mySignalDispPtr->xTicks, (mySignalDispPtr->
	  autoXScale == TRUE))) {
		wxLogWarning("%s: Failed to set x-axis scale.", funcName);
		return;
	}
	tickLength = (int) (xAxis->GetHeight() * GRAPH_X_TICK_LENGTH_SCALE);
	yPos = (int) (xAxis->GetTop() + theYOffset + xAxis->GetHeight() *
	  GRAPH_X_LABELS_Y_OFFSET_SCALE);
	for (i = 0; i < xAxisScale.GetNumTicks(); i++) {
		xValue = xAxisScale.GetTickValue(i);
		xPos = xAxisScale.GetTickPosition(xValue) + theXOffset;
		stringNum.Printf(xAxisScale.GetOutputFormatString(), xValue);
		dc.GetTextExtent(stringNum, &stringWidth, &stringHeight);
		dc.DrawText(stringNum, (int) (xPos - stringWidth * tempXAdjust / 2.0),
		  (int) (yPos + ((useTextAdjust)? stringHeight / 2.0: 0.0)));
		dc.DrawLine(xPos, xAxis->GetTop() - tickLength + theYOffset, xPos,
		  xAxis->GetTop() + theYOffset);
	}
	dc.SetFont(*axisTitleFont);
	dc.GetTextExtent(xTitle, &stringWidth, &stringHeight);
	xTitlePos = (int) ((xAxis->GetWidth() + stringWidth * tempXAdjust) / 2.0 +
	  theXOffset);
	yTitlePos = (int) (xAxis->GetBottom() - stringHeight * tempYAdjust - xAxis->
	  GetHeight() * GRAPH_X_TITLE_MARGIN_SCALE + theYOffset);
	dc.DrawText(xTitle, xTitlePos, yTitlePos);
	if (xAxisScale.GetExponent())
		DrawExponent(dc, labelFont, xAxisScale.GetExponent(), xAxis->GetRight(
		  ) - stringWidth * GRAPH_EXPONENT_LENGTH / xTitle.Length(), yTitlePos);

	if (xAxisScale.GetSettingsChanged()) {
		strcpy(mySignalDispPtr->xNumberFormat, (char *) xAxisScale.
		  GetFormatString('x').GetData());
		mySignalDispPtr->xTicks = xAxisScale.GetNumTicks();
	}

}

/****************************** DrawExponent **********************************/

void
MyCanvas::DrawExponent(wxDC& dc, wxFont *labelFont, int exponent, int x, int y)
{
	long int	stringWidth, stringHeight;
	wxString	string("10");

	dc.SetFont(*labelFont);
	dc.DrawText(string, x, y);
	dc.GetTextExtent(string, &stringWidth, &stringHeight);
	string.sprintf("%d", exponent);
	superLabelFont->SetPointSize((int)(labelFont->GetPointSize() *
	  GRAPH_EXPONENT_VS_LABEL_SCALE));
	dc.SetFont(*superLabelFont);
	dc.DrawText(string, x + stringWidth, y - stringHeight / 2);

}

/****************************** GetMinimumIntLog ******************************/

/*
 * This routine returns the  log result for a specified value, returning a value
 * of zero if value is close to zero.
 */

int
MyCanvas::GetMinimumIntLog(double value)
{
	double	absValue;

	if ((absValue = fabs(value)) < DBL_EPSILON)
		return 0;
	return((int) floor(log10(absValue)));

}

/******************************* DrawYScale ***********************************/

/*
 * This routine draws the y-scale.
 * The (xOffset, yOffset) point defines the top of the scale indicator.
 */

void
MyCanvas::DrawYScale(wxDC& dc, AxisScale &yAxisScale, wxRect *yAxisRect,
  wxFont *labelFont, char *numFormat, int theXOffset, int theYOffset,
  int yTicks, int numDisplayedChans, double minYValue, double maxYValue,
  bool autoScale)
{
	/* static const char *funcName = "MyCanvas::DrawYScale"; */
	int		i, j, tickLength, xPos, yPos, top, xLabel;
	int		leftMostLabel;
	long int	stringWidth, stringHeight, charWidth;
	double	tempXAdjust, tempYAdjust, chanSpacing, yOffset;
	double	displayScale, chanDisplayScale, yValue;
	wxString label, space = " ";

	dc.SetPen(*wxBLACK_PEN);
	SetTextAdjust(&tempXAdjust, &tempYAdjust);
	dc.SetFont(*labelFont);
	top = yAxisRect->GetBottom() - (int) floor(yAxisRect->GetHeight() + 0.5);
	dc.DrawLine(yAxisRect->GetRight() + theXOffset, top + theYOffset,
	  yAxisRect->GetRight() + theXOffset, yAxisRect->GetBottom() + theYOffset);
	displayScale = (double) yAxisRect->GetHeight() / yAxis->GetHeight();
	chanDisplayScale = (double) signalLines.GetNumDisplayedLines() /
	  numDisplayedChans;
	chanSpacing = signalLines.GetChannelSpace() * chanDisplayScale *
	  displayScale;
	yAxisScale.Set(numFormat, minYValue, maxYValue, 0, (int) chanSpacing,
	  yTicks, autoScale);
	tickLength = (int) (yAxisRect->GetWidth() * GRAPH_Y_TICK_LENGTH_SCALE);
	dc.GetTextExtent(space, &charWidth, &stringHeight);
	xPos = (int) (yAxisRect->GetRight() - tickLength + theXOffset - yAxisRect->
	  GetWidth() * GRAPH_Y_LABELS_X_OFFSET_SCALE);
	leftMostLabel = xPos;
	for (j = 0; j < numDisplayedChans; j++) {
		yOffset = yAxisRect->GetBottom() - j * chanSpacing + theYOffset;
		for (i = 0; i < yAxisScale.GetNumTicks(); i++) {
			yValue = yAxisScale.GetTickValue(i);
			yPos = (int) (yOffset - yAxisScale.GetTickPosition(yValue));
			label.sprintf(yAxisScale.GetOutputFormatString(), yValue /
			  mySignalDispPtr->magnification);
			dc.GetTextExtent(label, &stringWidth, &stringHeight);
			xLabel = (int) (xPos - (stringWidth - charWidth / 2.0) *
			  tempXAdjust);
			if (xLabel < leftMostLabel)
				leftMostLabel = xLabel;
			dc.DrawText(label, xLabel, (int) (yPos - stringHeight *
			  tempYAdjust / 2.0));
			dc.DrawLine(yAxisRect->GetRight() - tickLength + theXOffset, yPos,
			  yAxisRect->GetRight() + theXOffset, yPos);
		}
	}
	if (yAxisScale.GetExponent())
		DrawExponent(dc, labelFont, yAxisScale.GetExponent(), leftMostLabel -
		  charWidth * 8, top + theYOffset + stringHeight);

}

/****************************** DrawYAxis *************************************/

/*
 * The "mySignalDispPtr->autoYScale == TRUE" code was put in to remove compiler
 * warnings with MSVC.
 */

void
MyCanvas::DrawYAxis(wxDC& dc, int theXOffset, int theYOffset)
{
	static const char *funcName = "MyCanvas::DrawYAxis";
	int		i, numDisplayedChans, tickLength, xPos, yPos;
	long int	stringWidth, stringHeight, charWidth;
	double	tempXAdjust, tempYAdjust, chanSpacing, yTickSpacing, yOffset, minY;
	double	maxY;
	wxString format, label, space = " ";
	AxisScale	insetAxisScale;

	if (!yAxis)
		return;

	dc.SetPen(*wxBLACK_PEN);
	SetTextAdjust(&tempXAdjust, &tempYAdjust);
	dc.SetFont(*labelFont);
	dc.DrawLine(yAxis->GetRight() + theXOffset, yAxis->GetTop() + theYOffset,
	  yAxis->GetRight() + theXOffset, yAxis->GetBottom() + theYOffset);
	numDisplayedChans = signalLines.GetNumDisplayedLines();
	chanSpacing = signalLines.GetChannelSpace();
	tickLength = (int) (yAxis->GetWidth() * GRAPH_Y_TICK_LENGTH_SCALE);
	dc.GetTextExtent(space, &charWidth, &stringHeight);
	xPos = (int) (yAxis->GetRight() - tickLength + theXOffset - yAxis->GetWidth(
	  ) * GRAPH_Y_LABELS_X_OFFSET_SCALE);
	switch (mySignalDispPtr->yAxisMode) {
	case GRAPH_Y_AXIS_MODE_CHANNEL:
		yOffset = yAxis->GetBottom() - chanSpacing / 2.0 + theYOffset;
		yTickSpacing = (numDisplayedChans < mySignalDispPtr->yTicks)? 1:
		  numDisplayedChans / mySignalDispPtr->yTicks;
		for (i = 0; i < numDisplayedChans; i += (int) yTickSpacing) {
			yPos = (int) (yOffset - i * chanSpacing);
			label = signalLines.GetLineLabel(i);
			dc.GetTextExtent(label, &stringWidth, &stringHeight);
			dc.DrawText(label, (int) (xPos - (stringWidth - charWidth / 2.0) *
			  tempXAdjust), (int) (yPos - stringHeight * tempYAdjust / 2.0));
			dc.DrawLine(yAxis->GetRight() - tickLength + theXOffset, yPos,
			  yAxis->GetRight() + theXOffset, yPos);
		}
		if (mySignalDispPtr->yInsetScale) {
			wxRect	yInset = *yAxis;
			yInset.SetHeight((int) floor(yAxis->GetHeight() * 
			  GRAPH_Y_INSET_SCALE_HEIGHT_SCALE + 0.5));
			yInset.SetY(yInset.GetY() + yAxis->GetHeight() - yInset.GetHeight(
			  ));
			insetLabelFont->SetPointSize((int)(labelFont->GetPointSize() *
			  GRAPH_INSET_VS_LABEL_SCALE));
			minY = 0.0,
            maxY = fabs(signalLines.GetMaxY() - signalLines.GetMinY()) *
			  GRAPH_Y_INSET_SCALE_HEIGHT_SCALE;
			DrawYScale(dc, insetAxisScale, &yInset, insetLabelFont, "x",
			  theXOffset - (int) floor(yAxis->GetWidth() *
			  GRAPH_Y_INSET_SCALE_OFFSET_SCALE + 0.5), theYOffset, 2, 1, minY,
			  maxY, FALSE);
		}
		break;
	case GRAPH_Y_AXIS_MODE_LINEAR_SCALE:
		DrawYScale(dc, yAxisScale, yAxis, labelFont, mySignalDispPtr->
		  yNumberFormat, theXOffset, theYOffset, mySignalDispPtr->yTicks,
		  numDisplayedChans, signalLines.GetMinY(), signalLines.GetMaxY(),
		  (mySignalDispPtr->autoYScale == TRUE));
		if (yAxisScale.GetSettingsChanged()) {
			strcpy(mySignalDispPtr->yNumberFormat, (char *) yAxisScale.
			  GetFormatString('y').GetData());
			mySignalDispPtr->yTicks = yAxisScale.GetNumTicks();
		}
		break;
	default:
		wxLogWarning("%s: Scale (%d) not implemented.", funcName,
		  mySignalDispPtr->yAxisMode);
	}
	dc.SetFont(*axisTitleFont);
	dc.GetTextExtent(yTitle, &stringWidth, &stringHeight);
	dc.DrawRotatedText(yTitle, (int) (yAxis->GetLeft() + theXOffset +
	  yAxis->GetWidth() * GRAPH_Y_TITLE_MARGIN_SCALE), (int) (yAxis->GetTop() +
	  (yAxis->GetHeight() + stringWidth * tempXAdjust) / 2.0 + theYOffset),
	  90.0);

}

/****************************** DrawGraph *************************************/

//
// Draws the graph.

void
MyCanvas::DrawGraph(wxDC& dc, int theXOffset, int theYOffset)
{
	dc.SetMapMode(wxMM_TEXT);
	dc.Clear();
	dc.SetLogicalFunction(wxCOPY);
	signalLines.DrawLines(dc,  theXOffset,  theYOffset);
	if (mySignalDispPtr->summaryDisplay)
		summaryLine.DrawLines(dc, theXOffset, theYOffset);
	labelFont->SetPointSize((int) (parent->GetClientSize().GetHeight() *
	  GRAPH_AXIS_LABEL_SCALE));
	axisTitleFont->SetPointSize((int) (parent->GetClientSize().GetHeight() *
	  GRAPH_AXIS_LABEL_SCALE));
	DrawXAxis(dc, theXOffset, theYOffset);
	DrawYAxis(dc, theXOffset, theYOffset);

}

/****************************** RescaleGraph **********************************/

// This routine rescales the areas and signal lines for a graph.

void
MyCanvas::RescaleGraph(void)
{
	SetGraphAreas();
	signalLines.Rescale(signal);
	summaryLine.Rescale(summary);

}

/****************************** RedrawGraph ***********************************/

/*
 * This routine redraws the graph when required.
 */

void
MyCanvas::RedrawGraph(void)
{
	wxClientDC dc(this);

	if (mySignalDispPtr->redrawGraphFlag) {
		DrawGraph(memDC, 0, 0);
		mySignalDispPtr->redrawGraphFlag = FALSE;
	}
	dc.Blit(0, 0, bitmapWidth, bitmapHeight, &memDC, 0, 0, wxCOPY);

}

/******************************************************************************/
/****************************** Call backs ************************************/
/******************************************************************************/

/****************************** OnSize ****************************************/

// This routine resises the canvas.

void
MyCanvas::OnSize(wxSizeEvent& WXUNUSED(event))
{
	if (firstSizeEvent) {
		firstSizeEvent = FALSE;
		return;
	}
	CreateBackingBitmap();
	RescaleGraph();
	mySignalDispPtr->redrawGraphFlag = TRUE;
	mySignalDispPtr->critSect->Enter();
	RedrawGraph();
	mySignalDispPtr->critSect->Leave();

}

/****************************** OnPaint ***************************************/

/*
 * This routine draws the graph in the memory DC first, then copies the memory
 * DC to the display (to avoid flickering etc.)
 */

void
MyCanvas::OnPaint(wxPaintEvent& WXUNUSED(event))
{
	wxPaintDC dc(this);

	if (mySignalDispPtr->redrawGraphFlag) {
		DrawGraph(memDC, 0, 0);
		mySignalDispPtr->redrawGraphFlag = FALSE;
	}
	dc.Blit(0, 0, bitmapWidth, bitmapHeight, &memDC, 0, 0, wxCOPY);

}

/****************************** OnRightDown ***********************************/

/*
 * This is the right button mouse down pop up menu callback.
 */

void
MyCanvas::OnRightDown(wxMouseEvent &event)
{
	wxMenu *menu = new wxMenu;

	menu->Append(DISPLAY_MENU_PRINT, "&Print", "Print the display window");
	menu->Append(DISPLAY_MENU_PREFERENCES, "P&references", "Set the display "
	  "user preferences");
	menu->Append(DISPLAY_MENU_EXIT, "&Close", "Close the display window");

    PopupMenu( menu, event.GetX(), event.GetY() );

}

/****************************** OnPrint ***************************************/

/*
 * Print dialog callback.
 */

void
MyCanvas::OnPrint(wxCommandEvent& WXUNUSED(event))
{
	static char *funcName = "MyCanvas::OnPrint";

#	if defined(wxUSE_PRINTING_ARCHITECTURE)
		wxPrintDialogData printDialogData(* printData);
		wxPrinter printer(& printDialogData);
		PrintDisp printout(this);

		if (!printer.Print(this, &printout, TRUE))
			wxLogWarning("%s:There was a problem printing.\nPerhaps your "
			  "current printer is not set correctly?",funcName);
		else
			*printData = printer.GetPrintDialogData().GetPrintData();
#	endif // wxUSE_PRINTING_ARCHITECTURE

}

/****************************** OnPreferences *********************************/

/*
 * Canvas preferences callback.
 */

void
MyCanvas::OnPreferences(wxCommandEvent& WXUNUSED(event))
{
	if (mySignalDispPtr->dialog) {
		mySignalDispPtr->dialog->Raise();
		return;
	}
	ModuleParDialog dialog(this, mySignalDispPtr->title, NULL,
	  mySignalDispPtr->parList, NULL, 300, 300, 500, 500,
	  wxDEFAULT_DIALOG_STYLE | wxDIALOG_MODAL);

	signalDispPtr = mySignalDispPtr;
	if (dialog.ShowModal() == wxID_OK) {
		GetSignalDispPtr()->redrawGraphFlag = TRUE;
		InitGraph();
		Refresh(FALSE);
	}

}

/****************************** OnQuit ****************************************/

/*
 * Quit callback.
 */

void
MyCanvas::OnQuit(wxCommandEvent& WXUNUSED(event))
{
	parent->Close();

}

/****************************** OnCloseWindow *********************************/

/*
 * On Close window.
 */

void
MyCanvas::OnCloseWindow(wxCloseEvent& WXUNUSED(event))
{
	parent->Destroy();

}

/****************************** OnMouseMove ***********************************/

void
MyCanvas::OnMouseMove(wxMouseEvent &event)
{
    wxClientDC dc(this);
    PrepareDC(dc);
    parent->PrepareDC(dc);

    wxPoint pos = event.GetPosition();
    pointer.x = dc.DeviceToLogicalX( pos.x );
    pointer.y = dc.DeviceToLogicalY( pos.y );

}


