/**********************
 *
 * File:		UtANSGDist.h
 * Purpose:		This file contains the auditory nerve spike distribution
 * 				management routines.
 * Comments:
 * Author:		L. P. O'Mard
 * Created:		12 Jun 2009
 * Updated:
 * Copyright:	(c) 2009, L. P. O'Mard
 *
 *********************/

#ifndef UTANSGDIST_H_
#define UTANSGDIST_H_

#include "UtNameSpecs.h"

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Macro definitions *****************************/
/******************************************************************************/

#define ANSGDIST_GAUSSIAN(V, X, M)	(exp(-SQR((X) - (M)) / (2.0 * (V))) / \
		  sqrt((V) * PIx2))

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef enum {

	ANSGDIST_DISTRIBUTION_STANDARD_MODE,
	ANSGDIST_DISTRIBUTION_GAUSSIAN_MODE,
	ANSGDIST_DISTRIBUTION_DBL_GAUSSIAN_MODE,
	ANSGDIST_DISTRIBUTION_NULL

} ANSGDistSpecifier;

typedef enum {

	ANSGDIST_GAUSS_NUM_FIBRES,
	ANSGDIST_GAUSS_MEAN,
	ANSGDIST_GAUSS_VAR1,
	ANSGDIST_GAUSS_VAR2

} ANSGDistGaussianSpecifier;

typedef struct {

	int		numChannels;
	int		*numFibres;

} ANSGDist, *ANSGDistPtr;

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

BOOLN	CheckFuncPars_ANSGDist(ParArrayPtr p, SignalDataPtr signal);

BOOLN	Init_ANSGDist(ANSGDistPtr *p, int numChannels);

void	Free_ANSGDist(ANSGDistPtr *p);

int		GetDistFuncValue_ANSGDist(ParArrayPtr p, int numChannels, int chan);

int		GetNumDistributionPars_ANSGDist(int mode);

NameSpecifier *	ModeList_ANSGDist(int index);

void	PrintFibres_ANSGDist(FILE *fp, const WChar *prefix, int *fibres,
		  Float *frequencies, int numChannels);

void	SetDefaultDistribution_ANSGDist(ParArrayPtr distribution);

BOOLN	SetFibres_ANSGDist(ANSGDistPtr *aNDist, ParArrayPtr p, Float *frequencies,
		  int numChannels);

void	SetStandardNumFibres_ANSGDist(ParArrayPtr distribution, int numFibres);

__END_DECLS

#endif /* UTANSGDIST_H_ */
