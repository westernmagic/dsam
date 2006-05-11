/**********************
 *
 * File:		GrAxisScale.h
 * Purpose: 	Axis scale class for organising scales in a rational manner.
 * Comments:	
 * Author:		L.P.O'Mard
 * Created:		12 Feb 2002
 * Updated:		
 * Copyright:	(c) 2002, CNBH, University of Essex
 *
 **********************/

#ifndef _GRAXISSCALE_H
#define _GRAXISSCALE_H 1

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define AXIS_SCALE_DEFAULT_SIG_DIGITS	1
#define AXIS_SCALE_DEFAULT_DEC_PLACES	0
#define AXIS_SCALE_DEFAULT_NUM_TICKS	5
#define AXIS_SCALE_DELTA_EXPONENT		3

/******************************************************************************/
/****************************** Macro definitions *****************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Class definitions *****************************/
/******************************************************************************/

/********************************** AxisScale *********************************/

class AxisScale
{
	bool	settingsChanged, autoScale;
	int		exponent, dataExponent, decPlaces, numTicks, minPosition;
	int		tickOffset, sigDigits, maxPosition;
	double	valueScale, positionScale, roundingScaler, minValue, maxValue;
	double	minValueScaled, minValueScaledRounded, minTickValue, powerScale;
	double	maxValueScaled;
	wxString	outputFormat;

  public:

	bool	CalculateScales(void);
	double	GetDecPlaces(void)		{ return decPlaces; }
	int		GetExponent(void)		{ return exponent; }
	wxString	GetFormatString(wxChar formatChar);
	int		GetNumTicks(void)		{ return numTicks; }
	bool	GetSettingsChanged()	{ return settingsChanged; }
	wxChar *	GetOutputFormatString(void)	{ return (wxChar *) outputFormat.
				  GetData(); }
	double	GetTickValue(int i);
	int		GetTickPosition(double tickValue);
	double	GetValueScale(void)			{ return valueScale; }
	bool	ParseNumberFormat(const wxString &numberFormat);
	double	RoundedValue(double value);
	bool	Set(wxChar *numberFormat, double minVal, double maxVal, int minPos,
			  int maxPos, int theNumTicks, bool theAutoScale);
	void	SetExponent(int requestedExp, int deltaExp);

};

/******************************************************************************/
/*************************** External Variables *******************************/
/******************************************************************************/

#endif

