/**********************
 *
 * File:		UtCmplxm.h
 * Purpose:		routines to handle complex numbers in C
 * Comments:	23-02-93 L.P.O'Mard: Incorporated into the DSAM core routines
 *				  library.
 * Author:		M.A.Stone L. P. O'Mard
 * Created:		?? Sep 1992
 * Updated:		31 Jul 1997
 * Copyright:	(c) 1997,  University of Essex
 *
 **********************/

#ifndef	_UTCMPLXM_H
#define _UTCMPLXM_H	1

/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

#ifndef M_PI
#	define M_PI		3.1415926535897932
#endif
#define TWOPI		(2.0 * M_PI)
#define	PI_BY_2		(M_PI / 2.0)
#define	SCALE_RADIANS_TO_DEGREES		(360.0 / TWOPI)

#ifndef TRUE
#	define	TRUE		0xFFFF
#endif
#ifndef FALSE
#	define	FALSE		0
#endif

/******************************************************************************/
/*************************** Type Definitions *********************************/
/******************************************************************************/

#if !defined(BOOLEAN_ALGEBRA)
#	define BOOLEAN_ALGEBRA
	typedef	int	BOOLN;
#endif

typedef struct {

	double	re, im;

} Complex, *ComplexPtr;

/******************************************************************************/
/*************************** Function Prototypes ******************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

void	Add_CmplxM(ComplexPtr a, ComplexPtr b, ComplexPtr c);

void	Convert_CmplxM(ComplexPtr a, ComplexPtr b);	/* .re has magnit.,
												 * .im has tan(theta) */

void	Copy_CmplxM(ComplexPtr a,ComplexPtr b); /*move contents of a to b */

BOOLN	Div_CmplxM(ComplexPtr a, ComplexPtr b, ComplexPtr c);

double	Modulus_CmplxM(ComplexPtr z);

void	Mult_CmplxM(ComplexPtr a, ComplexPtr b, ComplexPtr c);

double	Phase_CmplxM(ComplexPtr z);

void	Power_CmplxM(ComplexPtr a, double n);

void	RThetaSet_CmplxM(double r, double theta, ComplexPtr z);

void	ScalerMult_CmplxM(ComplexPtr z, double scaler);

void	Subt_CmplxM(ComplexPtr a, ComplexPtr b, ComplexPtr c);

__END_DECLS

#endif
