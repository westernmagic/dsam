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
	signalLines = NULL;
	summaryLine = NULL;
	chanLength = 0;
	numChannels = 0;

	parent = frame;
	mySignalDispPtr = theSignalDispPtr;

	memBmp = NULL;
	CreateBackingBitmap();
	
	labelFont = new wxFont(15, wxROMAN, wxNORMAL, wxNORMAL);
	superLabelFont = new wxFont(15, wxROMAN, wxNORMAL, wxNORMAL);
	axisTitleFont = new wxFont(15, wxROMAN, wxNORMAL,
	  wxBOLD);
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
	if (signalLines)
		delete signalLines;
	if (summaryLine)
		delete summaryLine;
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

	xAxisSpace = (xAxis)? (int) (graph.GetHeight() * (GRAPH_X_AXIS_SCALE +
	  GRAPH_X_TITLE_SCALE)): 0;
	yAxisSpace = (yAxis)? (int) (graph.GetWidth() * (GRAPH_Y_AXIS_SCALE +
	  GRAPH_Y_TITLE_SCALE)): 0;
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

/****************************** SetGraphPars **********************************/

/*
 * This routine sets the graph parameters that need to be set from the 
 * signalDisp parameter list.
 */

void
MyCanvas::SetGraphPars(void)
{
	signalLines->ResetScaleAdjustment();

	signalLines->SetChannelStep(mySignalDispPtr->channelStep);
	signalLines->SetYMagnification(mySignalDispPtr->magnification);
	greyBrushes->SetGreyScales(mySignalDispPtr->numGreyScales);

}

/****************************** InitGraph *************************************/

void
MyCanvas::InitGraph(EarObjectPtr data, EarObjectPtr summaryEarO)
{
	SetGraphAreas();
	InitData(data);
	SetGraphPars();

	if (mySignalDispPtr->automaticYScaling) {
		signalLines->CalcMaxMinLimits(data);
		mySignalDispPtr->minY = signalLines->GetMinY();
		mySignalDispPtr->maxY = signalLines->GetMaxY();
	} else {
		signalLines->SetMinY(mySignalDispPtr->minY);
		signalLines->SetMaxY(mySignalDispPtr->maxY);
	}
	
	if (mySignalDispPtr->automaticYScaling)
		summaryLine->CalcMaxMinLimits(summaryEarO);
	else {
		summaryLine->SetMinY(mySignalDispPtr->minY);
		summaryLine->SetMaxY(mySignalDispPtr->maxY);
	}
	summaryLine->SetYMagnification(1.0);
	switch (mySignalDispPtr->mode) {
	case GRAPH_MODE_LINE:
		SetLines(signalLines, data, signal);
		SetLines(summaryLine, summaryEarO, summary);
		break;
	case GRAPH_MODE_GREY_SCALE:
		SetGreyScaleLines(signalLines, data, signal, mySignalDispPtr->
		  xResolution);
		SetGreyScaleLines(summaryLine, summaryEarO, summary, 1.0);
		break;
	} /* switch */
	summaryLine->SetYSpacing(summary);

}

/****************************** InitData **************************************/

/*
 * This method initialises the signal lines and summary line graphs.
 */

void
MyCanvas::InitData(EarObjectPtr data)
{
	if (signalLines && ((chanLength != data->outSignal->length) ||
	  (data->outSignal->numChannels != signalLines->GetNumLines()))) {
			delete signalLines;
		signalLines = NULL;
		if (summaryLine)
			delete summaryLine;
		summaryLine = NULL;
	}
	numChannels = data->outSignal->numChannels;
	chanLength = data->outSignal->length;
	timeIndex = _WorldTime_EarObject(data);
	dt = data->outSignal->dt;
	if (!signalLines) {
		if ((signalLines = new MultiLine(numChannels,
		  mySignalDispPtr->channelStep, chanLength)) == NULL) {
			wxMessageBox("MyCanvas::InitData: out of memory for signal "
			  "lines", "Client: Error", wxOK);
			exit(1);
		}
		if ((summaryLine = new MultiLine(mySignalDispPtr->summary->outSignal->
		  numChannels, 1, chanLength)) == NULL) {
			wxMessageBox("MyCanvas::InitData out of memory for summary "
			  "line", "Client: Error", wxOK);
			exit(1);
		}
	} else {
		signalLines->Reset();
		summaryLine->Reset();
	}
	outputTimeOffset = data->outSignal->outputTimeOffset;
		/* Tidy up, using wxString - when wxString works. */
	if (strcmp(xTitle, data->outSignal->info.sampleTitle) != 0)
		xTitle.Printf("%s", data->outSignal->info.sampleTitle);
	if (strcmp(yTitle, data->outSignal->info.channelTitle) != 0)
		yTitle.Printf("%s", data->outSignal->info.channelTitle);

}

/****************************** SetLines **************************************/

/*
 * This method sets the lists of points from the output signal of an EarObject.
 * The routine uses the minYRecord array to ensure that channels that are
 * "underneath" do not show through the "upper" channels.
 * 
 */

void
MyCanvas::SetLines(MultiLine *lines, EarObjectPtr data, wxRect& rect)
{
	char	stringNum[MAXLINE];
	int		j, xIndex;
	double	xScale, yScale, xPos, deltaX, yNormalisationOffset;
	ChanLen	i;
	ChanData	*p;

	lines->SetGreyScaleMode(FALSE);
	lines->SetYNormalisationMode(mySignalDispPtr->yNormalisationMode);
	lines->SetParams(rect);
	xScale = (double) rect.GetWidth() / (data->outSignal->length - 1);
	yScale = lines->GetYScale();
	if (xScale >= 1.0)
		xIndex = 1;
	else
		xIndex = (mySignalDispPtr->xResolution / xScale > 1.0)? (int)
		  (mySignalDispPtr->xResolution / xScale): 1;
	deltaX = xScale * xIndex;
	yNormalisationOffset = (mySignalDispPtr->yNormalisationMode ==
	  LINE_YNORM_MIDDLE_MODE )?
	  (lines->GetMaxY() + lines->GetMinY()) / 2.0: lines->GetMinY();
	for (j = 0; j < data->outSignal->numChannels; j++) {
		p = data->outSignal->channel[j];
		for (i = 0, xPos = 0.0; i < data->outSignal->length; i += xIndex, p +=
		  xIndex, xPos += deltaX)
			lines->AddPoint(j, (int) xPos, (int) (-(*p - yNormalisationOffset) *
			  yScale));
		sprintf(stringNum, "%.0f", data->outSignal->info.chanLabel[j]);
		lines->SetLineLabel(j, stringNum);
	}

}

/****************************** SetGreyScaleLines *****************************/

/*
 * This method sets the lists of points from the output signal of an EarObject.
 * The points are used to draw the filled rectangles for the GreyScale display.
 */

void
MyCanvas::SetGreyScaleLines(MultiLine *lines, EarObjectPtr data, wxRect& rect,
  double xResolution)
{
	char	stringNum[MAXLINE];
	int		j, xIndex;
	double	xScale, yScale, xPos, deltaX;
	ChanLen	i;
	ChanData	*p;

	lines->SetGreyScaleMode(TRUE);
	lines->SetParams(rect);
	yScale = (mySignalDispPtr->numGreyScales - 1) / (lines->GetMaxY() -
	  lines->GetMinY());
	xScale = rect.GetWidth() / data->outSignal->length;
	if (xScale >= 1.0)
		xIndex = 1;
	else
		xIndex = (xResolution / xScale > 1.0)? (int) (xResolution / xScale): 1;
	deltaX = xScale * xIndex;
	for (j = 0; j < data->outSignal->numChannels; j++) {
		p = data->outSignal->channel[j];
		for (i = 0, xPos = 0.0; i < data->outSignal->length; i += xIndex, p +=
		  xIndex, xPos += deltaX)
			lines->AddPoint(j, (int) xPos, (int) ((*p - lines->GetMinY()) *
			  yScale));
		sprintf(stringNum, "%.0f", data->outSignal->info.chanLabel[j]);
		lines->SetLineRectHeight(j, lines->GetChannelSpace());
		lines->SetLineRectWidth(j, deltaX);
		lines->SetLineLabel(j, stringNum);
	}

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

void
MyCanvas::DrawXAxis(wxDC& dc, int theXOffset, int theYOffset)
{
	int		i, tickLength, xTickSpacing, yPos, xPos;
	long int	stringWidth, stringHeight;
	double	interval, scale, timeOffsetMS, tempXAdjust, tempYAdjust;
	double	roundingScaler;
	wxString stringNum, format, label, space = " ";

	if (!xAxis)
		return;

	dc.SetPen(*wxBLACK_PEN);
	SetTextAdjust(&tempXAdjust, &tempYAdjust);
	dc.SetFont(*labelFont);
	dc.DrawLine(xAxis->GetLeft() + theXOffset,  xAxis->GetTop() + theYOffset,
	  xAxis->GetRight() + theXOffset, xAxis->GetTop() + theYOffset);
	timeOffsetMS = MSEC(outputTimeOffset + timeIndex * dt) ;
	roundingScaler = pow(10.0, mySignalDispPtr->xDecPlaces);
	interval = floor(MSEC(((chanLength - 1) * dt) * roundingScaler) + 0.5) /
	  roundingScaler;
	scale = interval / (mySignalDispPtr->xTicks - 1);
	xTickSpacing = xAxis->GetWidth() / (mySignalDispPtr->xTicks - 1);
	tickLength = (int) (xAxis->GetHeight() * GRAPH_TICK_LENGTH_SCALE);
	format.Printf("%%.%df", mySignalDispPtr->xDecPlaces);
	yPos = (int) (xAxis->GetTop() + theYOffset + xAxis->GetHeight() *
	  GRAPH_X_LABELS_Y_OFFSET_SCALE);
	for (i = 0; i < mySignalDispPtr->xTicks; i++) {
		xPos = xAxis->GetLeft() + i * xTickSpacing + theXOffset;
		stringNum.Printf(format, i * scale + timeOffsetMS);
		dc.GetTextExtent(stringNum, &stringWidth, &stringHeight);
		dc.DrawText(stringNum, (int) (xPos - stringWidth * tempXAdjust / 2.0),
		  (int) (yPos + ((useTextAdjust)? stringHeight / 2.0: 0.0)));
		dc.DrawLine(xPos, xAxis->GetTop() - tickLength + theYOffset, xPos,
		  xAxis->GetTop() + theYOffset);
	}
	xTitle.Replace("(s)", "(ms)");
	dc.SetFont(*axisTitleFont);
	dc.GetTextExtent(xTitle, &stringWidth, &stringHeight);
	dc.DrawText(xTitle, (int) ((xAxis->GetWidth() + stringWidth * tempXAdjust) /
	   2.0 + theXOffset), (int) (xAxis->GetBottom() - stringHeight *
	   tempYAdjust - xAxis->GetHeight() * GRAPH_X_TITLE_MARGIN_SCALE +
	   theYOffset));

}

/****************************** DrawExponent **********************************/

void
MyCanvas::DrawExponent(wxDC& dc, int exponent, int x, int y)
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

/****************************** DrawYAxis *************************************/

void
MyCanvas::DrawYAxis(wxDC& dc, int theXOffset, int theYOffset)
{
	static const char *funcName = "MyCanvas::DrawYAxis";
	int		i, j, numDisplayedChans, tickLength, exponent = 0, xPos, yPos;
	int		exponentTest;
	long int	stringWidth, stringHeight, charWidth;
	double	tempXAdjust, tempYAdjust, chanSpacing, yTickSpacing, yOffset, scale;
	wxString format, label, space = " ";

	if (!yAxis)
		return;

	dc.SetPen(*wxBLACK_PEN);
	SetTextAdjust(&tempXAdjust, &tempYAdjust);
	dc.SetFont(*labelFont);
	dc.DrawLine(yAxis->GetRight() + theXOffset, yAxis->GetTop() + theYOffset,
	  yAxis->GetRight() + theXOffset, yAxis->GetBottom() + theYOffset);
	numDisplayedChans = signalLines->GetNumDisplayedLines();
	chanSpacing = signalLines->GetChannelSpace();
	tickLength = (int) (yAxis->GetWidth() * GRAPH_TICK_LENGTH_SCALE);
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
			label = signalLines->GetLineLabel(i);
			dc.GetTextExtent(label, &stringWidth, &stringHeight);
			dc.DrawText(label, (int) (xPos - (stringWidth - charWidth / 2.0) *
			  tempXAdjust), (int) (yPos - stringHeight * tempYAdjust / 2.0));
			dc.DrawLine(yAxis->GetRight() - tickLength + theXOffset, yPos,
			  yAxis->GetRight() + theXOffset, yPos);
		}
		break;
	case GRAPH_Y_AXIS_MODE_LINEAR_SCALE:
		exponentTest = (int) floor(log10(signalLines->GetMaxY()));
		if ((exponentTest < GRAPH_Y_SCALE_LOWER_EXP_LIMIT) || (exponentTest >
		  GRAPH_Y_SCALE_UPPER_EXP_LIMIT))
			exponent = exponentTest;
		scale = (signalLines->GetMaxY() - signalLines->GetMinY()) /
		  (mySignalDispPtr->yTicks - 1);
		yTickSpacing = chanSpacing / (mySignalDispPtr->yTicks - 1);
		format.Printf("%%.%df", mySignalDispPtr->yDecPlaces);
		for (j = 0; j < numDisplayedChans; j++) {
			yOffset = yAxis->GetBottom() - j * chanSpacing + theYOffset;
			for (i = 0; i < mySignalDispPtr->yTicks; i++) {
				yPos = (int) (yOffset - i * yTickSpacing + 0.5);
				label.sprintf(format, (i * scale + signalLines->GetMinY()) *
				  pow(10.0, -exponent));
				dc.GetTextExtent(label, &stringWidth, &stringHeight);
				dc.DrawText(label, (int) (xPos - (stringWidth - charWidth /
				  2.0) * tempXAdjust), (int) (yPos - stringHeight *
				  tempYAdjust / 2.0));
				dc.DrawLine(yAxis->GetRight() - tickLength + theXOffset, yPos,
				  yAxis->GetRight() + theXOffset, yPos);
			}
		}
		break;
	default:
		NotifyError("%s: Scale (%d) not implemented.", funcName);
	}
	if (exponent)
		DrawExponent(dc, exponent, yAxis->GetLeft() + theXOffset,
		  yAxis->GetTop() + theYOffset);
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
	//signalLines->RescaleY(*parList->pars[DISPLAY_MAGNIFICATION].valuePtr.r);
	signalLines->DrawLines(dc,  theXOffset,  theYOffset);
	if (mySignalDispPtr->summaryDisplay)
		summaryLine->DrawLines(dc, theXOffset, theYOffset);
	labelFont->SetPointSize((int) (xAxis->GetHeight() *
	  GRAPH_AXIS_LABEL_SCALE));
	axisTitleFont->SetPointSize((int) (xAxis->GetHeight() *
	  GRAPH_AXIS_TITLE_SCALE));
	DrawXAxis(dc, theXOffset, theYOffset);
	DrawYAxis(dc, theXOffset, theYOffset);

}

/****************************** RescaleGraph **********************************/

// This routine rescales the areas and signal lines for a graph.

void
MyCanvas::RescaleGraph(void)
{
	SetGraphAreas();
	signalLines->Rescale(signal);
	summaryLine->Rescale(summary);

}

/****************************** ForceRepaintforMSW ****************************/

/*
 * Using 'Refresh' for MSW does not work at present, so this routine will force
 * the window to be repainted, when GrSignalDisp wants a redraw.
 */

void
MyCanvas::ForceRepaintforMSW(void)
{
	wxPaintEvent	event;
	
	OnPaint(event);

}

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
	mySignalDispPtr->redrawFlag = TRUE;

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

	if (mySignalDispPtr->redrawFlag) {
		DrawGraph(memDC, 0, 0);
		mySignalDispPtr->redrawFlag = FALSE;
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
			NotifyError("%s:There was a problem printing.\nPerhaps your "
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
	ModuleParDialog dialog(this, mySignalDispPtr->title, 0, NULL,
	  mySignalDispPtr->parList, 300, 300, 500, 500, wxDEFAULT_DIALOG_STYLE |
	  wxDIALOG_MODAL);

	signalDispPtr = mySignalDispPtr;
	if (dialog.ShowModal() == wxID_OK) {
		GetSignalDispPtr()->redrawFlag = TRUE;
		SetGraphPars();
		RescaleGraph();
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

