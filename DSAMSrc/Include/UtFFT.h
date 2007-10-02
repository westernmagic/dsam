/**********************
 *
 * File:		UtFFT.h
 * Purpose:		This module contains the fast fourier transform analysis
 *				routines.
 * Comments:	This was renamed from the old AnFourierT module.
 * Author:		L. P. O'Mard
 * Created:		17-07-00
 * Updated:		
 * Copyright:	(c) 2000, University of Essex
 *
 **********************/

#ifndef	_UTFFT_H
#define _UTFFT_H	1

#if HAVE_FFTW3
#	include <fftw3.h>
#endif

#include "UtCmplxM.h"

/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

#define	FORWARD_FT		1			/* Forward FT */
#define	BACKWARD_FT		-1			/* Backward FT */

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

#if HAVE_FFTW3
	typedef fftw_complex	Complx, *ComplxPtr;

	typedef struct {		/* Used so that logical lengths for the arrays can be set */

		int		numPlans;
		fftw_plan	*plan;
		double	*data;
		unsigned long	arrayLen;
		unsigned long	fftLen;
		unsigned long	dataLen;

	} FFTArray, *FFTArrayPtr;
#else
#	define Complx	Complex
#	define ComplxPtr	ComplexPtr
#endif

/******************************************************************************/
/*************************** Function Prototypes ******************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

void	Calc_FFT(double *data, unsigned long nn, int isign);

void	CalcComplex_FFT(Complex data[], unsigned long nn, int isign);

BOOLN	CalcReal_FFT(SignalDataPtr signal, double *fT, int direction);

#if HAVE_FFTW3
	FFTArrayPtr	InitArray_FFT(unsigned long dataLen, BOOLN forInPlaceFFT, int numPlans);

	void	FreeArray_FFT(FFTArrayPtr *p);
#endif /* HAVE_FFTW3 */

unsigned long	Length_FFT(unsigned long length);

__END_DECLS

#endif
