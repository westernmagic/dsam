/**********************
 *
 * File:		AnFourierT.c
 * Purpose:		This module contains the fourier analysis routines routines.
 * Comments:	
 * Author:		L. P. O'Mard
 * Created:		18-01-94
 * Updated:		
 * Copyright:	(c) 1998, University of Essex
 *
 **********************/

#ifndef	_UTFTANALYSIS_H
#define _UTFTANALYSIS_H	1

#include "UtCmplxM.h"

/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

#define	FORWARD_FT		1			/* Forward FT */
#define	BACKWARD_FT		-1			/* Backward FT */

/******************************************************************************/
/*************************** Function Prototypes ******************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

void	FFT_FourierT(double *data, unsigned long nn, int isign);

void	FFT_FourierT_Complex(Complex data[], unsigned long nn, int isign);

BOOLN	Phase_FourierT(EarObjectPtr data);

BOOLN	RealFT_FourierT(SignalDataPtr signal, int direction);

__END_DECLS

#endif
