/**********************
 *
 * File:		UtRandom.c
 * Purpose:		This is the random number routines module.
 * Comments:	22-03-98 LPO Added support for seeds set the time for
 *				completely random numbers.
 * Author:		L. P. O'Mard
 * Created:		29 Mar 1993
 * Updated:		22 Mar 1998
 * Copyright:	(c) 1998, University of Essex
 *
 **********************/
 
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <time.h>

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG_H */

#include "GeCommon.h"
#include "UtRandom.h"

/******************************************************************************/
/************************** Global Variables **********************************/
/******************************************************************************/

long	randomNumSeed = RANDOM_INITIAL_SEED; 

/******************************************************************************/
/************************** Subroutines and functions *************************/
/******************************************************************************/

/************************** Ran01 *********************************************/

/*
 * This routine is a uniformally distributed random number generator.
 * It has been revised from the "Numerical Recipes 2nd ed." Ran1 (p.280).
 * Set "seed" to any negative value to initialise or re-intialise the sequence.
 */

#define IA 16807
#define IM 2147483647L
#define AM (1.0 / IM)
#define IQ 127773L
#define IR 2836
#define NTAB 32
#define NDIV (1 + (IM - 1) / NTAB)
#define EPS 1.2e-7
#define RNMX (1.0 - EPS)

double
Ran01_Random(long *idum)
{
	int j;
	long k;
	static long iy=0;
	static long iv[NTAB];
	double temp;

	if (*idum <= 0 || !iy) {
		if (*idum == 0)
			*idum = (long) time(NULL);
		else
			*idum = (-(*idum) < 1)? 1: -(*idum);
		for (j = NTAB + 7; j >= 0; j--) {
			k = (*idum) / IQ;
			*idum = IA * (*idum - k * IQ) - IR * k;
			if (*idum < 0)
				*idum += IM;
			if (j < NTAB)
				iv[j] = *idum;
		}
		iy=iv[0];
	}
	k = (*idum) / IQ;
	*idum = IA * (*idum - k * IQ) - IR * k;
	if (*idum < 0)
		*idum += IM;
	j = iy / NDIV;
	iy = iv[j];
	iv[j] = *idum;
	return( ((temp = AM * iy) > RNMX)? RNMX: temp);

}
#undef IA
#undef IM
#undef AM
#undef IQ
#undef IR
#undef NTAB
#undef NDIV
#undef EPS
#undef RNMX

/************************** GeomDist ******************************************/

/*
 * This routine calculates the number of times that an event occurs with 
 * a set probability and number of trials.
 * It uses the gemotric distribution method, but the probability must be
 * small.
 * The zero numTrials check speeds up processing when many such conditions
 * are processed.
 */

int
GeomDist_Random(double probability, int numTrials)
{
	int		i;
	double	sum, logNotProbable;
	
	#ifdef DEBUG
	static const char *funcName = "GeomDist_Random";
		if (probability >= 1.0) {
			NotifyError("%s: probability = %g: greater than 1.", funcName,
			  probability);
			exit(1);
		}
	#endif
	if ((numTrials <= 0) || (probability < DBL_EPSILON))
		return(0);
	logNotProbable = log(1.0 - probability);
	for (i = 0, sum = 0.0; ((sum += log(Ran01_Random(&randomNumSeed)) /
	  logNotProbable) < numTrials); i++)
		;
	if (i > numTrials)
		i = numTrials;
	return (i);
	
}

/************************** SetGlobalSeed *************************************/

/*
 * This routine sets the global random number seed for the module.
 */

void
SetGlobalSeed_Random(long theSeed)
{
	randomNumSeed = theSeed;

}

/************************** GaussRan01 ****************************************/

/*
 * This routine is a Gaussian (normally) distributed random number generator.
 * It has been revised from the "Numerical Recipes 2nd ed." Ran1 (p.289).
 */

double
GaussRan01_Random(long *seed)
{
	static BOOLN	isSet = FALSE;
	static double	gSet;
	double	fac, rSquared, v1, v2;

	if (!isSet) {
		do {
			v1 = 2.0 * Ran01_Random(seed) - 1.0;
			v2 = 2.0 * Ran01_Random(seed) - 1.0;
			rSquared = SQR(v1) + SQR(v2);
		} while ((rSquared >= 1.0) || (rSquared == 0.0));
		fac = sqrt(-2.0 * log(rSquared) / rSquared);
		gSet = v1 * fac;
		isSet = TRUE;
		return (v2 * fac);
	} else {
		isSet = FALSE;
		return (gSet);
	}
}
