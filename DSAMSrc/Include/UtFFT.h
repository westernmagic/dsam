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
/*************************** Macro Definitions ********************************/
/******************************************************************************/

#if DSAM_USE_FLOAT
#	define DSAM_FFTW_NAME(NAME)	fftwf_ ## NAME
#else
#	define DSAM_FFTW_NAME(NAME)	fftw_ ## NAME
#endif

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

#if HAVE_FFTW3
	typedef DSAM_FFTW_NAME(complex)	Complx, *ComplxPtr;
	typedef DSAM_FFTW_NAME(plan)	Fftw_plan;

	typedef struct {		/* Used so that logical lengths for the arrays can be set */

		int		numPlans;
		Fftw_plan	*plan;
		Float	*data;
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

void	Calc_FFT(Float *data, unsigned long nn, int isign);

void	CalcComplex_FFT(Complex data[], unsigned long nn, int isign);

BOOLN	CalcReal_FFT(SignalDataPtr signal, Float *fT, int direction);

void	CreateNoiseBand_FFT(FFTArrayPtr fTInv, int plan, RandParsPtr randPars,
		  int kLow, int kUpp);

#if HAVE_FFTW3
	FFTArrayPtr	InitArray_FFT(unsigned long dataLen, BOOLN forInPlaceFFT, int numPlans);

	void	FreeArray_FFT(FFTArrayPtr *p);
#endif /* HAVE_FFTW3 */

unsigned long	Length_FFT(unsigned long length);

__END_DECLS

#endif
