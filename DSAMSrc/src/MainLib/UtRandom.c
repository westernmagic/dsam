/**********************
 *
 * File:		UtRandom.c
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

/******************************************************************************/
/************************** Subroutines and functions *************************/
/******************************************************************************/

/************************** InitPars ******************************************/

/*
 * This routine initialises the a RandPars structure.
 * It returns NULL if it fails in any way.
 */

RandParsPtr
InitPars_Random(long idum, long offset)
{
	static const WChar *funcName = wxT("InitPars_Random");
	RandParsPtr	p;

	if ((p = (RandParsPtr) malloc(sizeof(RandPars))) == NULL) {
		NotifyError(wxT("%s: Could not initialises structure."), funcName);
		return(NULL);
	}
	p->idum = idum;
	p->offset = offset;
	p->iy = 0;
	return(p);

}

/************************** FreePars ******************************************/

/*
 * This routine frees the memory allocated for a RandPars structure.
 */

void
FreePars_Random(RandParsPtr *p)
{
	if (!*p)
		return;
	free(*p);
	*p = NULL;

}

/************************** SetSeed *******************************************/

/*
 * This function sets the random number seed.
 * The offset is used for setting different seeds in thread processing mode.
 * It returns FALSE if it fails in any way.
 */

BOOLN
SetSeed_Random(RandParsPtr p, long ranSeed, long offset)
{
	static const WChar *funcName = wxT("SetSeed_Random");
	if (!p) {
		NotifyError(wxT("%s: Structure is not initialised."), funcName);
		return(FALSE);
	}
	p->idum = ranSeed;
	p->offset = offset;
	return(TRUE);

}

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
#define NDIV (1 + (IM - 1) / RANDOM_NTAB)
#define EPS 1.2e-7
#define RNMX (1.0 - EPS)

double
Ran01_Random(RandParsPtr p)
{
	int j;
	long k;
	double temp;

	if (p->idum <= 0 || !p->iy) {
		if (p->idum == 0)
			p->idum = (long) time(NULL);
		else
			p->idum = (-(p->idum) < 1)? 1: -(p->idum);
		p->idum += p->offset;
		for (j = RANDOM_NTAB + 7; j >= 0; j--) {
			k = (p->idum) / IQ;
			p->idum = IA * (p->idum - k * IQ) - IR * k;
			if (p->idum < 0)
				p->idum += IM;
			if (j < RANDOM_NTAB)
				p->iv[j] = p->idum;
		}
		p->iy = p->iv[0];
	}
	k = (p->idum) / IQ;
	p->idum = IA * (p->idum - k * IQ) - IR * k;
	if (p->idum < 0)
		p->idum += IM;
	j = p->iy / NDIV;
	p->iy = p->iv[j];
	p->iv[j] = p->idum;
	return( ((temp = AM * p->iy) > RNMX)? RNMX: temp);

}
#undef IA
#undef IM
#undef AM
#undef IQ
#undef IR
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
GeomDist_Random(double probability, int numTrials, RandParsPtr p)
{
	int		i;
	double	sum, logNotProbable;
	
	#ifdef DEBUG
	static const WChar *funcName = wxT("GeomDist_Random");
		if (probability >= 1.0) {
			NotifyError(wxT("%s: probability = %g: greater than 1."), funcName,
			  probability);
			exit(1);
		}
	#endif
	if ((numTrials <= 0) || (probability < DBL_EPSILON))
		return(0);
	logNotProbable = log(1.0 - probability);
	for (i = 0, sum = 0.0; ((sum += log(Ran01_Random(p)) / logNotProbable) <
	  numTrials); i++)
		;
	if (i > numTrials)
		i = numTrials;
	return (i);
	
}

/************************** GaussRan01 ****************************************/

/*
 * This routine is a Gaussian (normally) distributed random number generator.
 * It has been revised from the "Numerical Recipes 2nd ed." Ran1 (p.289).
 */

double
GaussRan01_Random(RandParsPtr p)
{
	static BOOLN	isSet = FALSE;
	static double	gSet;
	double	fac, rSquared, v1, v2;

	if (!isSet) {
		do {
			v1 = 2.0 * Ran01_Random(p) - 1.0;
			v2 = 2.0 * Ran01_Random(p) - 1.0;
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

/************************** Ran ***********************************************/

/*
 * This routine calculates a random number between a set of limits.
 * Set "seed" to any negative value to initialise or re-intialise the sequence.
 */

double
Ran_Random(RandParsPtr p, double minValue, double maxValue)
{
	return(Ran01_Random(p) * (maxValue - minValue) + minValue);

}
