/*
 *	UtCmplxm.c: routines to handle complex numbers in C.
 *	M.A.Stone, Cambridge, Sept 1992.
 *	23-02-93 L.P.O'Mard: Incorporated into the DSAM core routines library.
 *
 * Unless otherwise stated all code was written by L. P. O'Mard.
 *
 *	E-mail: L.P.OMard@uk.ac.lut
 *
 * COPYRIGHT ©
 *
 *	Speech and Hearing Laboratory,
 *	Department of Human Sciences,
 *	Loughborough University of Technology,
 *	Loughborough,
 *	Leicestershire LE11 3TU.
 *	United Kingdom.
 *
 *	12th July 1993.
 *
 *	DISCLAIMER OF WARRANTY
 *
 * Although every effort has been made to ensure the integrity of this code,
 * the author, Loughborough University and its Laboratories disclaim all
 * liability for direct or consequential damages resulting from its use.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG_H */

#include "GeCommon.h"
#include "UtCmplxM.h"

/*----------------------------------------------------------------------------*/

/*
 * Complex multiply c = a * b
 */

void
Mult_CmplxM(ComplexPtr a, ComplexPtr b, ComplexPtr c)
{
	double SumRe,SumIm;
	
	SumRe = (a->re * b->re) - (a->im * b->im);
	SumIm = (a->re * b->im) + (a->im * b->re);
	(*c).re = SumRe;
	(*c).im = SumIm;
}

/*----------------------------------------------------------------------------*/

/* Complex divide c = a/b */

BOOLN
Div_CmplxM(ComplexPtr a, ComplexPtr b, ComplexPtr c)
{
	Complex Num;
	double Denom;
	int div_succs;
	
	Denom  = (b->re * b->re) + (b->im * b->im);
	Num.re = (a->re * b->re) + (a->im * b->im);
	Num.im = (a->im * b->re) - (a->re * b->im);
	if(Denom != 0.0) {
		(*c).re = Num.re/Denom;
		(*c).im = Num.im/Denom;
		div_succs = TRUE;
	}
	else {
		(*c).re = 0.0;
		(*c).im = 0.0;
		div_succs = FALSE;
	}
	return(div_succs); /* return whether divide happened or not */
}

/*----------------------------------------------------------------------------*/

/* Complex addition c = a + b */

void
Add_CmplxM(ComplexPtr a, ComplexPtr b, ComplexPtr c)
{
	double SumRe,SumIm;
	
	SumRe = (a->re + b->re);
	SumIm = (a->im + b->im);
	(*c).re = SumRe;
	(*c).im = SumIm;
}

/*----------------------------------------------------------------------------*/

/* Complex subtraction c = a - b */

void Subt_CmplxM(ComplexPtr a, ComplexPtr b, ComplexPtr c)
{
	double SumRe,SumIm;
	
	SumRe = (a->re - b->re);
	SumIm = (a->im - b->im);
	(*c).re = SumRe;
	(*c).im = SumIm;
}

/*----------------------------------------------------------------*/

void Convert_CmplxM(ComplexPtr a,ComplexPtr b) /* Complex magnitude b = |a| */
{
	double SumRe, SumIm;
	
	SumRe = (a->re * a->re) + (a->im * a->im);;
	SumRe = sqrt(SumRe);
	if(a->re != 0.0)
		SumIm = a->im / a->re;
	else
		SumIm = 0.0;
	b->re = SumRe;
	b->im = SumIm; /* puts tan(theta) in .im */
}

/*--------------------------------------------------------------*/

void Copy_CmplxM(ComplexPtr a,ComplexPtr b) /* copy contents of a to b */
{
	b->re = a->re;
	b->im = a->im;
}

/*-------------------------------------------------*/

void Power_CmplxM(ComplexPtr a, double n) /* calculate */
/*-------------------(*a) to power n, n fractional */
{
	double r, theta, real, imag;
	
	real = a->re; imag = a->im;
	r = sqrt( (real*real) + (imag*imag) );
	if(r != 0.0)
		theta = acos(real/r);
	else
		return;
	if((real > 0.0) && (imag > 0.0))
		 ; /* get quadrant correct */
	else 
		if((real > 0.0) && (imag < 0.0))
			theta = TWOPI - theta;
	else
		if((real < 0.0) && (imag < 0.0))
			theta += M_PI;
	else 
		if((real < 0.0) && (imag > 0.0))
			theta = M_PI - theta;
			
	theta *= n;	r = pow(r,n); /* perform power on r*exp(i*theta) */
	real = r*cos(theta); imag = r*sin(theta);
	a->re = real; a->im = imag;
}

/*************************** Modulus ******************************************/

/*
 * This function returns with the modulus of a complex number.
 */

double
Modulus_CmplxM(ComplexPtr z)
{
	return (sqrt(z->re * z->re + z->im * z->im));
	
}

/*************************** Phase ********************************************/

/*
 * This function returns with the phase (radians) of a complex number.
 * This needs to be changed.
 */

double
Phase_CmplxM(ComplexPtr z)
{
	return(atan2(z->im, z->re));
	
}

/*************************** RThetaSet ****************************************/

/*
 * This function sets a complex number using the z = R * exp(I * theta) format,
 * where R and Theta are passed as arguments.
 */

void
RThetaSet_CmplxM(double r, double theta, ComplexPtr z)
{
	z->re = r * cos(theta);
	z->im = r * sin(theta);

}

/*************************** ScalerMult ***************************************/

/*
 * This routine multiplies a complex number by a scaler value
 */

void
ScalerMult_CmplxM(ComplexPtr z, double scaler)
{
	z->re *= scaler;
	z->im *= scaler;

}


