/**********************
 *
 * File:		UtRandom.h
 * Purpose:		This is the random number routines module.
 * Comments:	22-03-98 LPO Added support for seeds set the time for
 *				completely random numbers.
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

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

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

int		GeomDist_Random(double probability, int numTrials);

double	Ran01_Random(long *seed);

void	SetGlobalSeed_Random(long theSeed);

__END_DECLS

#endif
