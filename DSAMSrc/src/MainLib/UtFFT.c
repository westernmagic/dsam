/**********************
 *
 * File:		UtFFT.c
 * Purpose:		This module contains the fast fourier transform analysis
 *				routines.
 * Comments:	This was renamed from the old AnFourierT module.
 * Author:		L. P. O'Mard
 * Created:		17-07-00
 * Updated:
 * Copyright:	(c) 2000, 2010 Lowel P. O'Mard
 *
 *********************
 *
 *  This file is part of DSAM.
 *
 *  DSAM is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  DSAM is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with DSAM.  If not, see <http://www.gnu.org/licenses/>.
 *
 *********************/

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

/****************************** Length ****************************************/

/*
 * This function returns the nearest length which is a power of two for use
 * with the FFT algorithm.
 */

unsigned long
Length_FFT(unsigned long length)
{
	unsigned long	ln;

	ln = (unsigned long) ceil(log(length) / log(2.0));
	return((unsigned long) pow(2.0, (Float) ln));

}

#if HAVE_FFTW3

/****************************** InitArray *************************************/

/*
 * Initialise an FFTArray structure.
 */

FFTArrayPtr
InitArray_FFT(unsigned long dataLen, BOOLN forInPlaceFFT, int numPlans)
{
	static const WChar *funcName = wxT("InitArray_FFT");
	FFTArrayPtr	p;

	if (dataLen < 2) {
		NotifyError(wxT("%s: The data length must be greater than 2 (%d)."),
		  funcName, dataLen);
		return(NULL);
	}
	if ((p = (FFTArrayPtr) malloc(sizeof(FFTArray))) == NULL) {
		NotifyError(wxT("%s: Out of memory for structure."), funcName);
		return(NULL);
	}
	if (numPlans < 1)
		p->plan = NULL;
	else {
		if ((p->plan = (Fftw_plan *) calloc(numPlans, sizeof(Fftw_plan))) == NULL) {
			NotifyError(wxT("%s: output of memory for %d fftw plans."), funcName,
			  numPlans);
			free (p);
			return(NULL);
		}
		p->numPlans = numPlans;
	}
	p->fftLen = Length_FFT(dataLen);
	p->arrayLen = (forInPlaceFFT)? (p->fftLen << 1) + 2: p->fftLen;
	if ((p->data = (Float *) DSAM_FFTW_NAME(malloc)(p->arrayLen * sizeof(Float))) == NULL) {
		NotifyError(wxT("%s: output of memory for physical array length: %lu."),
		  funcName, p->arrayLen);
		free (p);
		return(NULL);
	}
	p->dataLen = dataLen;
	return(p);

}

/****************************** FreeArray *************************************/

void
FreeArray_FFT(FFTArrayPtr *p)
{
	int		i;

	if (!*p)
		return;
	if ((*p)->data)
		DSAM_FFTW_NAME(free)((*p)->data);
	if ((*p)->plan) {
		for (i = 0; i < (*p)->numPlans; i++)
			DSAM_FFTW_NAME(destroy_plan)((*p)->plan[i]);
		free((*p)->plan);
	}
	free(*p);
	*p = NULL;

}

/****************************** CreateNoiseBand ********************************/

/*
 * Routine to create a band of noise given the half upper frequency band in
 * k-space.
 * It expects the FFTW plan to have been correctly set up for a reverse FFT.
 * A value of kLow = 1 is required for no DC.
 */

void
CreateNoiseBand_FFT(FFTArrayPtr fTInv, int plan, RandParsPtr randPars,
  int kLow, int kUpp)
{
	int		k;
	ChanLen	j;
	Float	phase;
	Complx	*c1;

	c1 = (Complx *) fTInv->data;
	for (k = 0; k < kLow; k++)
		CMPLX_RE(c1[k]) = CMPLX_IM(c1[k]) = 0.0;
	for (k = kLow; k < (kUpp + 1); k++) {
		phase = Ran01_Random(randPars) * PIx2;
		CMPLX_RE(c1[k]) = cos(phase);
		CMPLX_IM(c1[k]) = sin(phase);
	}
	for (j = kUpp + 1; j < fTInv->fftLen / 2 + 1; j++)
		CMPLX_RE(c1[j]) = CMPLX_IM(c1[j]) = 0.0;
	DSAM_FFTW_NAME(execute)(fTInv->plan[plan]);

}

#endif /* HAVE_FFTW3 */

