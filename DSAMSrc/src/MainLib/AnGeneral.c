/**********************
 *
 * File:		AnGeneral.c
 * Purpose:		This module contains various general analysis routines.
 * Comments:	
 * Author:		L. P. O'Mard
 * Created:		12 Jul 1993
 * Updated:		
 * Copyright:	(c) 1998, University of Essex
 *
 **********************/
 
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG_H */

#include "GeCommon.h"
#include "GeSignalData.h"
#include "GeEarObject.h"
#include "AnGeneral.h"

/**************************** LinearRegression ********************************/

/*
 * This subroutine calculates the best fit line parameters for a set of data
 * points, x,y, where x = dx * i.
 * It returns the parameters as arguments.
 */

BOOLN
LinearRegression_GenAnalysis(double *y0, double *gradient, double *y,
  double dx, ChanLen startIndex, ChanLen length)
{
	static const char *funcName = "LinearRegression_GenAnalysis";
	double  a, b, c, e, f, x;
	ChanLen	i, endIndex;

	if (length < 2) {
		NotifyError("%s: Illegal calculation length (%lu).", funcName, length);
		return(FALSE);
	}
	b = c = e = f = 0.0;
	a = (double) length;
	endIndex = length + startIndex;
	y += startIndex;
	for (i = startIndex; i < endIndex; i++) {
		x = dx * i;	
		b += x;
		c += x * x;
		e += *y;
		f += x * *(y++);
	}
	*gradient = (f - b * e / a) / (c - b * b / a);
	*y0 = (e - b * *gradient) / a;
	return(TRUE);

} /* LinearRegression_GenAnalysis */

/****************************** EuclideanDistance *****************************/

/*
 * This function compares two arrays using a Euclidean distance
 * measure: m = sum[(ti - si)^2].
 * The smaller the return value, m, the better the match between the two
 * arrays.
 * This routine assumes that startIndex and endIndex are within the bounds
 * of both arrays.
 */

double
EuclideanDistance_GenAnalysis(double *arrayA, double *arrayB,
  ChanLen startIndex, ChanLen endIndex)
{
	static const char *funcName = "Euclidean_GenAnalysis";
	double		sum, difference;
	ChanLen		i;
	double		*ptr1, *ptr2;

	if (arrayA == NULL) {
		NotifyError("%s: Array A not initialised.", funcName);
		exit(1);
	}
	if (arrayB == NULL) {
		NotifyError("%s: Array B not initialised.", funcName);
		exit(1);
	}
	if (endIndex < startIndex) {
		NotifyError("%s: Start ", funcName);
		exit(1);
	}
	ptr1 = arrayA + startIndex;
	ptr2 = arrayB + startIndex;
	for (i = startIndex, sum = 0.0; i < endIndex; i++) {
		difference = *ptr1++ - *ptr2++;
		sum += difference * difference;
	}
	return(sum);

}
