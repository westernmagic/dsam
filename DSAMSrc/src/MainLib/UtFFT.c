/**********************
 *
 * File:		UtFFT.c
 * Purpose:		This module contains the fast fourier transform analysis
 *				routines.
 * Comments:	This was renamed from the old AnFourierT module.
 * Author:		L. P. O'Mard
 * Created:		17-07-00
 * Updated:		
 * Copyright:	(c) 2000, University of Essex
 *
 **********************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG_H */

#include "GeCommon.h"
#include "GeSignalData.h"
#include "GeEarObject.h"
#include "UtCmplxM.h"
#include "UtFFT.h"

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/ 

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Functions and subroutines *********************/
/******************************************************************************/

/****************************** FFT...Complex *********************************/

/*
 * This routine carries out an FFT, returning the result to the same
 * array of complex data.
 * It uses the Danielson-Lanczos Lemma, and is based upon (but not copied
 * from) the Numerical Recipes algorithm.
 */

#define	SWAP(a, b)		tempVal = (a); (a) = (b); (b) = tempVal;

void
CalcComplex_FFT(Complex data[], unsigned long nn, int isign)
{
	unsigned long	mmax,m,j,istep,i;
	double		wtemp,wr,wpr,wpi,wi,theta, tempVal;
	Complex		temp;

	j = 0;
	for (i = 0; i < nn; i++) {
		if (j > i) {
			SWAP(data[j].re, data[i].re);
			SWAP(data[j].im, data[i].im);
		}
		m = nn >> 1;
		while (m >= 2 && j >= m) {
			j -= m;
			m >>= 1;
		}
		j += m;
	}
	for (mmax = 1; nn > mmax; mmax <<= 1) {
		istep = mmax << 1;
		theta = 3.141592653589793 / (isign * mmax);
		wtemp = sin(0.5 * theta);
		wpr = -2.0 * wtemp * wtemp;
		wpi = sin(theta);
		wr = 1.0;
		wi = 0.0;
		for (m = 0; m < mmax; m++) {
			for (i = m; i < nn; i += istep) {
				j = i + mmax;
				temp.re = wr * data[j].re - wi * data[j].im;
				temp.im = wr * data[j].im + wi * data[j].re;
				data[j].re = data[i].re - temp.re;
				data[j].im = data[i].im - temp.im;
				data[i].re += temp.re;
				data[i].im += temp.im;
			}
			wr = (wtemp = wr) * wpr - wi * wpi + wr;
			wi = wi * wpr + wtemp * wpi + wi;
		}
	}

}

#undef SWAP

/****************************** FFT *******************************************/

/*
 * This routine carries out an FFT, returning the result to the same
 * data array.
 * It uses the Danielson-Lanczos Lemma, and is based upon (but not copied
 * from) the Numerical Recipes algorithm, i.e. it uses pointers.
 * The data is in the form *data = real, *(data + 1) = imaginary.
 */

#define SWAP(a, b)	tempr = (a); (a) = (b); (b) = tempr

void
Calc_FFT(double *data, unsigned long nn, int isign)
{
	unsigned long	n, mmax, m, j, istep, i;
	double	wtemp, wr, wpr, wpi, wi, theta;
	double	tempr, tempi, *dm, *di, *dj;

	n = nn << 1;
	for (i = 0, j = 0; i < n; i += 2) {
		if (j > i) {
			SWAP(data[j], data[i]);
			SWAP(data[j + 1], data[i + 1]);
		}
		m = n >> 1;
		while (m >= 2 && j >= m) {
			j -= m;
			m >>= 1;
		}
		j += m;
	}
	for (mmax = 2; n > mmax; mmax <<= 1) {
		istep = mmax << 1;
		theta = isign * (6.28318530717959 / mmax);
		wtemp = sin(0.5 * theta);
		wpr = -2.0*wtemp*wtemp;
		wpi = sin(theta);
		wr=1.0;
		wi=0.0;
		for (dm = data; dm < data + mmax; dm += 2) {
			for (di = dm; di < data + n; di += istep) {
				dj = di + mmax;
				tempr = wr * *dj - wi * *(dj + 1);
				tempi = wr * *(dj + 1) + wi * *dj;
				*dj = *di - tempr;
				*(dj + 1) = *(di + 1) - tempi;
				*di += tempr;
				*(di + 1) += tempi;
			}
			wr = (wtemp = wr) * wpr - wi * wpi + wr;
			wi = wi * wpr + wtemp * wpi + wi;
		}
	}
}

#undef SWAP

/****************************** RealFT ****************************************/

/*
 * This function calculates the normalised Fourier transformation of a real data
 * set.
 * It returns FALSE if the routine fails in any way.
 * N.B. dfn = n / (dt * length * 2).
 */

#define	C1	0.5

BOOLN
CalcReal_FFT(SignalDataPtr signal, int direction)
{
	static const char *funcName = "CalcReal_FFT";
	ChanLen	i, k, n, ln;
	double	h1r, h1i, h2r, h2i;
	double	wr, wi, wpr, wpi, wtemp, theta, *fT;
	register double		c2, c3, *fT1, *fT2;
	register ChanData	*ptr;
	
	if (!CheckPars_SignalData(signal)) {
		NotifyError("%s: Signal not correctly set.", funcName);		
		return(FALSE);
	}
	/* This next bit ensures there is a power of two of data */
	ln = (ChanLen) ceil(log(signal->length) / log(2.0));
	n = (ChanLen) pow(2.0, (double) ln);
	if ((fT = (double *) calloc(n, sizeof(double))) == NULL) {
		NotifyError("%s: Couldn't allocate memory for complex data array.",
		  funcName);
		return(FALSE);
	}
	SetSamplingInterval_SignalData(signal, 1.0 / (signal->dt * n * 2.0));
	theta = M_PI / (double) (n >> 1);
	for (k = 0; k < signal->numChannels; k++) {
		fT[0] = 0.0;
		ptr = signal->channel[k] + 1;
		for (i = 1; i < signal->length; i++)
			fT[i] = *ptr++;
		if (direction == FORWARD_FT) {
			c2 = -0.5;
			c3 = 1.0;
			Calc_FFT(fT, n >> 1, 1);
		} else {
			c2 = 0.5;
			theta = -theta;
			c3 = 2.0 / n;
		}
		wtemp = sin(0.5 * theta);
		wpr = -2.0 * wtemp * wtemp;
		wpi = sin(theta);
		wr = 1.0 + wpr;
		wi = wpi;
		for (fT1 = fT + 2, fT2 = fT + n - 2; fT1 < fT + (n >> 1);
		  fT1 += 2, fT2 -= 2) {
			h1r = C1 * (*fT1 + *fT2);
			h1i = C1 * (*(fT1 + 1) - *(fT2 + 1));
			h2r = -c2 * (*(fT1 + 1) + *(fT2 + 1));
			h2i = c2 * (*fT1 - *fT2);
			*fT1 = h1r + wr * h2r - wi * h2i;
			*(fT1 + 1) = h1i + wr * h2i + wi * h2r;
			*fT2 = h1r - wr * h2r + wi * h2i;
			*(fT2 + 1) = -h1i + wr * h2i + wi * h2r;
			wr = (wtemp = wr) * wpr - wi * wpi + wr;
			wi = wi * wpr + wtemp * wpi + wi;
		}
		if (direction == FORWARD_FT) {
			*fT = (h1r = *fT) + *(fT + 1);
			*(fT + 1) = h1r - *(fT + 1);
		} else {
			*fT = C1 * ((h1r = *fT) + *(fT + 1));
			*(fT + 1) = C1 * (h1r - *(fT + 1));
			Calc_FFT(fT, n >> 1, -1);
		}
		ptr = signal->channel[k];
		for (fT1 = fT; fT1 < fT + signal->length; fT1++)
			*ptr++ = *fT1 * c3;
	}
	free(fT);
	return(TRUE);

}

#undef C1

