/**********************
 *
 * File:		GrAxisScale.cpp
 * Purpose: 	Axis scale class for organising scales in a rational manner.
 * Comments:	
 * Author:		L.P.O'Mard
 * Created:		12 Feb 2002
 * Updated:		
 * Copyright:	(c) 2002, CNBH, University of Essex
 *
 **********************/

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG_H */

#include <stdio.h>
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
#endif

#include "GeCommon.h"
#include "GrAxisScale.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Methods ***************************************/
/******************************************************************************/

/****************************** Constructor ***********************************/

/****************************** Destructor ************************************/

/****************************** Set *******************************************/

bool
AxisScale::Set(char *numberFormat, double minVal, double maxVal, int minPos,
  int maxPos, int theNumTicks, bool theAutoScale)
{
	static const char *funcName = "AxisScale::Set";

	if (minVal > maxVal) {
		wxLogError("%s: minmum value (%g) is greater than maximum value "
		  "(%g)\n", funcName, minVal, maxVal);
		return(FALSE);
	}
	if (minPos > maxPos) {
		wxLogError("%s: minmum position (%d) is greater than maximum position "
		  "(%d)\n", funcName, minPos, maxPos);
		return(FALSE);
	}
	settingsChanged = FALSE;
 	if (fabs(maxVal - minVal) <= 0.0) {
		minValue = minVal - 1.0;
		maxValue = maxVal + 1.0;
	} else {
		minValue = minVal;
		maxValue = maxVal;
	}
	minPosition = minPos;
	maxPosition = maxPos;
	tickOffset = 0;
	dataExponent = (int) floor(log10(MAXIMUM(fabs(minValue), fabs(maxValue))) +
      0.5);
	sigDigits = AXIS_SCALE_DEFAULT_SIG_DIGITS;
	decPlaces = AXIS_SCALE_DEFAULT_DEC_PLACES;
	exponent = dataExponent - sigDigits;
	autoScale = theAutoScale;
	if (autoScale) {
		numTicks = 10;
	} else {
		numTicks = theNumTicks;
		if (!ParseNumberFormat(numberFormat)) {
			wxLogError("%s: Failed to recognise number format '%s'", funcName,
			  numberFormat);
			return(FALSE);
		}
	}
	CalculateScales();
	outputFormat.Printf("%%.%df", decPlaces);
	return(TRUE);

}

/****************************** ParseNumberFormat *****************************/

/*
 * This routine parses the number format and routines the number of decimal
 * number of decimal places and exponent as argument pointers.
 * It returns FALSE if it fails in any way.
 */

bool
AxisScale::ParseNumberFormat(char *format)
{
	static const char *funcName = "AxisScale::ParseNumberFormat";
	char	*p1, *p2;

	if (!format || (format[0] == '\0')) {
		wxLogError("%s: Format string not set.\n", funcName);
		return(FALSE);
	}
	if (strcmp(format, "auto") == 0)
		return(TRUE);
	sigDigits = 0;
	decPlaces = 0;
	p1 = strchr(format, '.');
	p2 = strchr((p1)? p1: format, 'e');
	if (p1)
		sigDigits = p1 - format;
	else if (p2)
		sigDigits = p2 - format;
	else
		sigDigits = strlen(format);
		
	if (p1 && p2) {
		decPlaces = p2 - p1 - 1;
	}
	if (p2)
		exponent = atoi(p2 + 1);
	else {
		exponent = dataExponent - sigDigits;
		settingsChanged = !autoScale;
	}
	return(TRUE);

}

/****************************** CalculateScales *******************************/

/*
 * This determines the axis scalers.
 */

bool
AxisScale::CalculateScales(void)
{
	double	baseScale;

	roundingScaler = pow(10.0, decPlaces);
	baseScale = (maxValue - minValue) / (numTicks - 1);
	while (fabs(RoundedValue(baseScale * pow(10.0, -exponent)) < DBL_EPSILON)) {
	  	settingsChanged = !autoScale;
		exponent--;
	}
	powerScale = pow(10.0, -exponent);
	positionScale = (maxPosition - minPosition) / (maxValue - minValue) /
	  powerScale;
	valueScale = RoundedValue(baseScale * powerScale);
	minValueScaled = minValue * powerScale;
	maxValueScaled = maxValue * powerScale;
	minValueScaledRounded = RoundedValue(minValueScaled);
	if (minValueScaledRounded < minValueScaled)
		tickOffset++;
	while (GetTickValue(numTicks - 1) > maxValueScaled) {
	  	settingsChanged = !autoScale;
		numTicks--;
	}
	while ((maxValueScaled - GetTickValue(numTicks - 1)) > valueScale) {
	  	settingsChanged = !autoScale;
		numTicks++;
	}
	return(TRUE);

}

/****************************** RoundedValue **********************************/

/*
 * This function returns the number rounded to the specified number of 
 * decimal places, using the respective scalers.
 */

double
AxisScale::RoundedValue(double value)
{
	return(floor(value * roundingScaler + 0.5) / roundingScaler);

}

/****************************** GetTickValue **********************************/

/*
 * This function returns the number represented by a tick point.
 */

double
AxisScale::GetTickValue(int i)
{
	return(minValueScaledRounded + valueScale * (i + tickOffset));

}

/****************************** GetTickPosition *******************************/

/*
 * This function returns the number represented by a value.
 */

int
AxisScale::GetTickPosition(double tickValue)
{
	return((int) (minPosition + positionScale * (tickValue - minValueScaled)));

}

/****************************** GetFormatString *******************************/

/*
 * This function returns the format string for the current settings.
 */

wxString
AxisScale::GetFormatString(char formatChar)
{
	int		i;
	wxString	format;

	for (i = 0; i < sigDigits; i++)
		format += formatChar;
	if (decPlaces)
		format += '.';
	for (i = 0; i < decPlaces; i++)
		format += formatChar;
	format.Printf("%se%d", (char *) format.GetData(), exponent);
	return(format);

}
