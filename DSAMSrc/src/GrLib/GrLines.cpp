/**********************
 *
 * File:		GrLines.cpp
 * Purpose: 	Graphical line drawing handling class.
 * Comments:	26 Nov: support for drawing lines of filled rectangles added.
 *				26-10-97 LPO: Added yNormalisationMode.
 *				14-10-99 LPO: The 'chanSpace' member field is real to avoid
 *				rounding errors when resizing and the y-axis placings.
 * Author:		L.P.O'Mard
 * Created:		03 Dec 1995
 * Updated:		26 Oct 1997
 * Copyright:	(c) 1998, University of Essex
 *
 **********************/

#ifdef __GNUG__
#pragma implementation
#pragma interface
#endif

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG_H */

#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <math.h>
#include <limits.h>

#if defined(GRAPHICS_SUPPORT) && defined(__cplusplus)
// For compilers that support precompilation, includes "wx.h".
#	include <wx/wxprec.h>

#	ifdef __BORLANDC__
	    #pragma hdrstop
#	endif

// Any files you want to include if not precompiling by including
// the whole of <wx/wx.h>
#	ifndef WX_PRECOMP
#		include <wx/msgdlg.h>
//#		include <wx/wx_cmdlg.h>
#		include <wx/dc.h>
#	endif

// Any files included regardless of precompiled headers
#endif

#include "GeCommon.h"
#include "GeSignalData.h"
#include "GrUtils.h"
#include "GrLines.h"
#include "GrBrushes.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

NameSpecifier yNormModeList_Line[] = {

						{"MIDDLE",		LINE_YNORM_MIDDLE_MODE },
						{"BOTTOM",		LINE_YNORM_BOTTOM_MODE },
						{"",			LINE_YNORM_MODE_NULL }

					};

/******************************************************************************/
/****************************** GrLines Methods *******************************/
/******************************************************************************/

/****************************** Constructor ***********************************/

/*
 * It assumes that the signal has been correctly initialised.
 */

GrLines::GrLines(SignalDataPtr theSignal, int theChannelStep, ChanLen theOffset,
  ChanLen theLength)
{
	signal = theSignal;
	if ((minYRecord = (wxCoord *) calloc(signal->length, sizeof(wxCoord))) ==
	  NULL) {
		wxMessageBox("GrLines::GrLines: out of memory for minYRecord.",
		  "Client: Error", wxOK);
		exit(1);
	}
	offset = theOffset;
	length = theLength;
	yNormalisationMode = LINE_YNORM_MIDDLE_MODE;
	redrawFlag = TRUE;
	greyScaleMode = FALSE;
	channelStep = 1;
	numDisplayedLines = signal->numChannels;
	SetChannelStep(theChannelStep);
	maxY = -(minY = FLT_MAX);
	yOffset = 0;
	yMagnification = 1.0;
	xResolution = 1.0;
	rect.SetWidth(-1);
	Reset();

}
	
/****************************** Destructor ************************************/

GrLines::~GrLines(void)
{
	free(minYRecord);
	
}

/****************************** Reset *****************************************/

/*
 * This routine resets the signal ready for re-drawing.
 */

void
GrLines::Reset(void)
{
	wxCoord	*minYPtr;
	ChanLen	i;

	redrawFlag = TRUE;
	for (i = offset, minYPtr = minYRecord; i < length; i++)
		*minYPtr++ = LONG_MAX;		
	
}

/****************************** SetGreyScaleMode ******************************/

/*
 * This routine sets the GreyScaleMode.
 */

void
GrLines::SetGreyScaleMode(bool status)
{
	if (greyScaleMode == status)
		return;
	greyScaleMode = status;
	Reset();

}

/****************************** Rescale ***************************************/

/*
 * This routine re-scales the line space values, if required.
 * This routine only has meaning in the line graph mode.
 * The 'SetYSpacing' routine must always be called because the channel step
 * could have been changed.
 */

void
GrLines::Rescale(wxRect& newRect)
{
	if (!redrawFlag && (newRect.GetHeight() == rect.GetHeight()) && (newRect.
	  GetWidth() == rect.GetWidth()))
		return;
	rect = newRect;
	chanSpace = (double) rect.GetHeight() / numDisplayedLines;
	channelScale = chanSpace / (maxY - minY);
	if (greyScaleMode)
		yOffset = (wxCoord) (rect.GetBottom() - chanSpace);
	else if (yNormalisationMode == LINE_YNORM_MIDDLE_MODE)
		yOffset = (wxCoord) (rect.GetBottom() - chanSpace / 2.0);
	else
		yOffset = rect.GetBottom();
	Reset();

}

/****************************** CalcMaxMinLimits ******************************/

/*
 * This method calculates the minY and MaxY members of each line, and the
 * overall minimum and maximum limits, using the specified signal.
 */

void
GrLines::CalcMaxMinLimits(void)
{
	int		j;
	ChanLen	i;
	ChanData	*p;

	maxY = -(minY = DBL_MAX);
	for (j = 0; j < signal->numChannels; j++) {
		p = signal->channel[j];
		for (i = offset; i < length; i++, p++) {
			if (*p > maxY)
				maxY = *p;
			if (*p < minY)
				minY = *p;
		}
	}

}

/****************************** GetLineLabel **********************************/

/*
 * This routine returns a label for a specified line.
 * It uses the local channel step;
 */

char *
GrLines::GetLineLabel(int theLine)
{
	static char	stringNum[MAXLINE];
	int		line = theLine * channelStep;

	snprintf(stringNum, MAXLINE, "%.0f", signal->info.chanLabel[line]);
	return(stringNum);

}

/****************************** SetChannelStep ********************************/

/*
 * This method sets the y spacing of lines for channels.  It sets the original
 * scaling and the Y offset of each line.
 * It also sets the 'yAdjustment' variable for use in re-scaling.
 */

void
GrLines::SetChannelStep(int theChannelStep)
{
	if (theChannelStep != channelStep)
		redrawFlag = TRUE;
	channelStep = theChannelStep;
	if (signal->numChannels >= channelStep)
		numDisplayedLines = signal->numChannels / channelStep;
	else {
		numDisplayedLines = signal->numChannels;
		channelStep = 1;
	}

}

/****************************** DrawLines *************************************/

/*
 * This routine draws the signal graph lines using the DC DrawLine routine.
 * In the GreyScale mode the DC DrawRectangle routine is used:
 * The x and theYOffset points describe the top left hand corner of the
 * rectangles, which are drawn using the graph brushes.
 * it uses the minYRecord to prevent overdrawing lines.
 */

#define	X_COORD(X)	((wxCoord) ((X) + xOffset))
#define	Y_COORD(Y)	((wxCoord) (yChan - ((Y) - yNormalOffset) * yScale))

void
GrLines::DrawLines(wxDC& dc, double theXOffset, double theYOffset)
{
	int		chan;
	double	xScale, yScale, x, deltaX, yNormalOffset, yChan;
	wxCoord	xCoord, yCoord, lastXCoord, lastYCoord, *minYPtr, xOffset;
	ChanLen	i, xIndex;
	ChanData	*p;

	if (greyScaleMode)
		dc.SetPen(*wxTRANSPARENT_PEN);
	else
		dc.SetPen(*wxBLACK_PEN);
	xScale = (double) rect.GetWidth() / (length - offset - ((greyScaleMode)? 0:
	  1));
	yScale = (greyScaleMode)? (numGreyScales - 1) / (maxY - minY):
	  yMagnification * channelScale;
	if (xScale >= 1.0)
		xIndex = 1;
	else
		xIndex = (xResolution / xScale > 1.0)? (int) (xResolution / xScale): 1;
	deltaX = xScale * xIndex;
	yNormalOffset = (yNormalisationMode == LINE_YNORM_MIDDLE_MODE)? (maxY +
	  minY) / 2.0: minY;

	xOffset = (wxCoord) (theXOffset + rect.GetLeft());
	yChan = theYOffset + yOffset;
	for (chan = 0; chan < signal->numChannels; chan += channelStep) {
		p = signal->channel[chan];
		if (greyScaleMode) {
			for (i = offset, x = 0; i < length; i += xIndex, p += xIndex, x +=
			  deltaX) {
				dc.SetBrush(*greyBrushes->GetBrush((int) floor((*p - minY) *
				  yScale + 0.5)));
				dc.DrawRectangle(X_COORD(x), (wxCoord) yChan, (wxCoord) deltaX,
				  (wxCoord) chanSpace);
			}
		} else {
			lastXCoord = X_COORD(0.0);
			lastYCoord = Y_COORD(*p++);
			for (i = offset + 1, x = deltaX, minYPtr = minYRecord; i < length;
			  i += xIndex, p += xIndex, x += deltaX, minYPtr += xIndex) {
		  		xCoord = X_COORD(x);
				yCoord = Y_COORD(*p);
				if (yCoord < *minYPtr) {
					*minYPtr = yCoord;
					dc.DrawLine(lastXCoord, lastYCoord, xCoord, yCoord);
				}
				lastXCoord = xCoord;
				lastYCoord = yCoord;
			}
		}
		yChan -= chanSpace;
	}
	dc.SetPen(wxNullPen);
	dc.SetBrush(wxNullBrush);
	redrawFlag = FALSE;

}

#undef X_COORD
#undef Y_COORD
