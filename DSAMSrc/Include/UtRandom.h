/**********************
 *
 * File:		UtRandom.h
 * Purpose:		This is the random number routines module.
 * Comments:	22-03-98 LPO Added support for seeds set the time for
 *				completely random numbers.
 *				17-11-05 LPO Added the offset for setting seeds in thread mode.
 * Author:		L. P. O'Mard
 * Created:		29 Mar 1993
 * Updated:		22 Mar 1998
 * Copyright:	(c) 1998, University of Essex
 *
 **********************/

#ifndef	_UTRANDOM_H
#define _UTRANDOM_H	1
 
/******************************************************************************/
/************************** Constant definitions ******************************/
/******************************************************************************/

#define RANDOM_INITIAL_SEED		3141L
#define RANDOM_NTAB				32

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/
typedef struct {

	long	idum;
	long	offset;
	long	iy;
	long	iv[RANDOM_NTAB];

} RandPars, *RandParsPtr;

/******************************************************************************/
/*************************** External Variables *******************************/
/******************************************************************************/

extern long	randomNumSeed;			/* Seed for the random number generator */

/******************************************************************************/
/*************************** Function Prototypes ******************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

void	FreePars_Random(RandParsPtr *p);

int		GeomDist_Random(double probability, int numTrials, RandParsPtr p);

double	GaussRan01_Random(RandParsPtr p);

RandParsPtr	InitPars_Random(long idum, long offset);

double	Ran_Random(RandParsPtr p, double minValue, double maxValue);

double	Ran01_Random(RandParsPtr p);

BOOLN	SetSeed_Random(RandParsPtr p, long ranSeed, long offset);

__END_DECLS

#endif
