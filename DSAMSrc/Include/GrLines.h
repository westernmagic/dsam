/**********************
 *
 * File:		GrLines.h
 * Purpose: 	Graphical line drawing handling class.
 * Comments:	26 Nov: support for drawing lines of filled rectangles added.
 *				26-10-97 LPO: Added yNormalisationMode.
 *				14-10-99 LPO: The 'chanSpace' member field is real to avoid
 *				rounding errors when resizing and the y-axis placings.
 * Author:		L.P.O'Mard
 * Created:		03 Dec 1995
 * Updated:		26 Oct 1997
 * Copyright:	(c) 1997, University of Essex
 *
 **********************/

#ifndef _GRLINES_H
#define _GRLINES_H 1

#include "UtNameSpecs.h"

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define		LINE_LABEL_STRING	6	/* String representation of line label. */

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef enum {

	LINE_YNORM_MIDDLE_MODE,
	LINE_YNORM_BOTTOM_MODE,
	LINE_YNORM_MODE_NULL

} YNormalisationSpecifier;

/******************************************************************************/
/****************************** Class definitions *****************************/
/******************************************************************************/

/********************************** GrLines ***********************************/

class GrLines
{
	bool	redrawFlag, greyScaleMode;
	int		numDisplayedLines, channelStep, yNormalisationMode;
	int		numGreyScales;
	wxCoord	yOffset, *minYRecord;
	double	chanSpace, yMagnification, channelScale, minY, maxY, xResolution;
	ChanLen	offset, length;
	wxRect	rect;
	SignalDataPtr	signal;
	
  public:
	GrLines(SignalDataPtr theSignal, int theChannelStep, ChanLen offset,
	  ChanLen length);
	~GrLines(void);

	void	CalcMaxMinLimits(void);
	void	DrawLines(wxDC& dc, double theXOffset, double theYOffset);
	double	GetChannelScale(void)		{ return channelScale; }
	double	GetChannelSpace(void)		{ return chanSpace; }
	char *	GetLineLabel(int theLine);
	double	GetMaxY(void)				{ return(maxY); }
	double	GetMinY(void)				{ return(minY); }
	int		GetNumDisplayedLines(void)	{ return numDisplayedLines; }
	int		GetNumLines(void)			{ return signal->numChannels; }
	SignalDataPtr	GetSignalPtr(void)	{ return signal; }
	void	Rescale(wxRect& newRect);
	void	Reset(void);
	void	SetChannelStep(int theChannelStep);
	void	SetGreyScaleMode(bool status);
	void	SetMaxY(double theMaxY)				{ maxY = theMaxY; }
	void	SetMinY(double theMinY)				{ minY = theMinY; }
	void	SetNumGreyScales(int theNumGS)	{ numGreyScales = theNumGS; }
	void	SetXResolution(double theXRes)		{ xResolution = theXRes; }
	void	SetYMagnification(double theYMagnification) { yMagnification = 
			  theYMagnification; }
	void	SetYNormalisationMode(int theYNormMode) { yNormalisationMode =
			  theYNormMode; }

};

/******************************************************************************/
/*************************** External Variables *******************************/
/******************************************************************************/

extern NameSpecifier yNormModeList_Line[];

#endif
