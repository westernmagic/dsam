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

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Class definitions *****************************/
/******************************************************************************/

/********************************** AxisScale *********************************/

class AxisScale
{
	bool	numberFormatChanged, autoScale;
	int		exponent, decPlaces, numTicks, minPosition, maxPosition;
	int		tickOffset;
	double	valueScale, positionScale, roundingScaler, minValue, maxValue;
	double	minValueScaled, minValueScaledRounded, minTickValue, powerScale;
	double	maxValueScaled;
	wxString	outputFormat;

  public:

	bool	CalculateScales(void);
	double	GetDecPlaces(void)		{ return decPlaces; }
	int		GetExponent(void)		{ return exponent; }
	wxString	GetFormatString(char formatChar);
	int		GetNumTicks(void)		{ return numTicks; }
	bool	GetNumberFormatChanged()	{ return numberFormatChanged; }
	char *	GetOutputFormatString(void)	{ return (char *) outputFormat.GetData(
			  ); }
	double	GetTickValue(int i);
	int		GetTickPosition(double tickValue);
	double	GetValueScale(void)			{ return valueScale; }
	bool	ParseNumberFormat(char *numberFormat);
	double	RoundedValue(double value);
	bool	Set(char *numberFormat, double minVal, double maxVal, int minPos,
			  int maxPos, int theNumTicks, bool theAutoScale);

};

/******************************************************************************/
/*************************** External Variables *******************************/
/******************************************************************************/

#endif

