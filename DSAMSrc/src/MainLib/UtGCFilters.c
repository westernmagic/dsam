/**********************
 *
 * File:		UtGCFilters.c
 * Purpose:		This module contains the functions that initialise, generate,
 *				control various filters.
 * Comments:	This module is based on subroutine of Package of GCFBv1.04a.
 * Author:		Masashi Unoki
 * Created:		16 Nov 2000
 * Updated:		30 Jan 2001
 * Copyright:	(c) 2000, 2001, CNBH Univ. of Cambridge
 *
 **********************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG_H */

#include "GeCommon.h"
#include "GeSignalData.h"
#include "UtFilters.h"
#include "UtGCFilters.h"
#include "UtCmplxM.h"

/******************************************************************************/
/*************************** Global Variables *********************************/
/******************************************************************************/

/*  Coefficients for the Asymmetric Compensastion filter */

double	Filters_AsymCmpCoef0[6] = {1.35, -0.19, 0.29, -0.004, 0.23, 0.0072};


/**************************** InitAsymCmpFiltersCoeffs ************************/

/*
 * The function allocates a pointer to the coefficients structure of Asymmetric
 * compensation filter. This routine consists with the fourth cascade of 
 * biquadratic filer.
 */

AsymCmpCoeffsPtr
InitAsymCmpCoeffs_GCFilters(void)
{
	static const char *funcName = "InitAsymCmpCoeffs_GCFilters";
	int    stateVectorLength, cascadeAC, coeffsLength;
	AsymCmpCoeffsPtr p;

	cascadeAC = GCFILTERS_NUM_CASCADE_ACF_FILTER;	
	coeffsLength = cascadeAC * GCFILTERS_NUM_ACF_STATE_VARS_PER_FILTER + 1;

	if ((p = (AsymCmpCoeffs *) malloc(sizeof(AsymCmpCoeffs))) == NULL) {
		NotifyError("%s: Out of memory!", funcName);
		exit(1);
    }
	if ((p->numCoeffs = (double *) calloc(coeffsLength, sizeof(double))) ==
	  NULL) {
		NotifyError("%s: Cannot allocate space for numCoeffs.", funcName);
		free(p);
		exit(1);
	}
	if ((p->denomCoeffs = (double *) calloc(coeffsLength, sizeof(double))) ==
	  NULL) {
		NotifyError("%s: Cannot allocate space for denomCoeffs.", funcName);
		free(p);
		exit(1);
	}

	stateVectorLength = cascadeAC * GCFILTERS_NUM_ACF_STATE_VARS_PER_FILTER + 1;
	if ((p->stateFVector = (double *) calloc(stateVectorLength, sizeof(
	  double))) == NULL) {
		NotifyError("%s: Cannot allocate space for state Forward-vector.",
		  funcName);
		free(p);
		exit(1);
	}
	if ((p->stateBVector = (double *) calloc(stateVectorLength, sizeof(
	  double))) == NULL) {
		NotifyError("%s: Cannot allocate space for state Backward-vector.",
		  funcName);
		free(p);
		exit(1);
	}

	p->cascade = cascadeAC;
	return(p);

} /* InitAsymCmpCoeffs_GCFilters */

/**************************** CalcAsymCmpFiltersCoeffs ************************/

/*
 * The function returns a pointer to the coefficients structure of Asymmetric
 * compensation filter. This routine consists with the fourth cascade of 
 * biquadratic filer.
 */

void
CalcAsymCmpCoeffs_GCFilters(AsymCmpCoeffsPtr p, double centreFreq, 
double bWidth3dB, double bCoeff, double cCoeff, int cascade, double sampleClk)
{
	static const char *funcName = "CalcAsymCmpCoeffs_GCFilters";
	int    kth, nth, coeffsLength;
	double coef_r, coef_th, coef_fn, r, th, fn;
	double k0num, k1num, k2num;				/* numerator coefficients */
	double k0denom, k1denom, k2denom;		/* denominator coefficients */
	double forward[2*GCFILTERS_NUM_CASCADE_ACF_FILTER+1];
	double feedback[2*GCFILTERS_NUM_CASCADE_ACF_FILTER+1];
	double numCoeffs[2*GCFILTERS_NUM_CASCADE_ACF_FILTER+1];
	double denomCoeffs[2*GCFILTERS_NUM_CASCADE_ACF_FILTER+1];
	Complex cf_num, cf_denom, Tf, var1, var2, var3;

	if (p == NULL) {
		NotifyError("%s: Memory for coefficients have not been allocated.", 
					funcName);
		exit(1);
	}
	coeffsLength = 2 * GCFILTERS_NUM_CASCADE_ACF_FILTER +1;
	forward[0] = 1.0;
	feedback[0] = 1.0;
	for (kth = 1; kth < coeffsLength; kth++) {
		forward[kth] = 0.0;
		feedback[kth] = 0.0;
	}

	for (kth = 0; kth < p->cascade; kth++) {

		for (nth = 0; nth < coeffsLength; nth++) {
			numCoeffs[nth] = 0.0;
			denomCoeffs[nth] = 0.0;
		}

        coef_r   = (Filters_AsymCmpCoef0[0] + 
						Filters_AsymCmpCoef0[1]*fabs(cCoeff));
        coef_r  *= (double)(kth+1);
        coef_th  = (Filters_AsymCmpCoef0[2] + 
						Filters_AsymCmpCoef0[3]*fabs(cCoeff));
        coef_th *= pow(2.0,(double)kth);
		coef_fn  = (Filters_AsymCmpCoef0[4] + 
						Filters_AsymCmpCoef0[5]*fabs(cCoeff));
        coef_fn *= (double)(kth+1);

        r  = exp(-coef_r*TwoPi*bCoeff*bWidth3dB/sampleClk);
        th = coef_th*cCoeff*bCoeff*bWidth3dB/centreFreq;
        fn = centreFreq+coef_fn*cCoeff*bCoeff*bWidth3dB/cascade;

        k0num 	= 1.0;
		k1num   = -2.0*r*cos(TwoPI*(1.0-th)*centreFreq/sampleClk); 
        k2num	= pow(r,2.0);
        k0denom	= 1.0;
		k1denom = -2.0*r*cos(TwoPI*(1.0+th)*centreFreq/sampleClk); 
  		k2denom = pow(r,2.0);

		/* normalise filter to unity gain at centre frequency 
													(needs Complex maths) */
	
		var2.re = k1num; var2.im = 0.0;	
		var3.re =  cos(TwoPi*fn/sampleClk);
		var3.im = -sin(TwoPi*fn/sampleClk);
		Mult_CmplxM(&var2,&var3,&var2);		/* a1*exp(-sT) */
		var1.re = k0num; var1.im = 0.0;
		Add_CmplxM (&var1,&var2,&var1); 	/* a0+a1*exp(-sT) */
		var2.re = k2num; var2.im = 0.0;	
		var3.re =  cos(2.0*TwoPi*fn/sampleClk);
		var3.im = -sin(2.0*TwoPi*fn/sampleClk);
		Mult_CmplxM(&var2,&var3,&var2); 	/* a2*exp(-2sT) */
		Add_CmplxM (&var1,&var2,&cf_num);	/* sigma(ai*exp(-sT) */

		var2.re = k1denom; var2.im = 0.0;
		var3.re =  cos(TwoPi*fn/sampleClk);
		var3.im = -sin(TwoPi*fn/sampleClk);
		Mult_CmplxM(&var2,&var3,&var2); 	/* b1*exp(-sT) */
		var1.re = k0denom; var1.im = 0.0;
		Add_CmplxM (&var1,&var2,&var1); 	/* b0+b1*exp(-sT) */
		var2.re = k2denom; var2.im = 0.0;
		var3.re =  cos(2.0*TwoPi*fn/sampleClk);
		var3.im = -sin(2.0*TwoPi*fn/sampleClk);
		Mult_CmplxM(&var2,&var3,&var2);		/* b2*exp(-2sT) */
		Add_CmplxM(&var1,&var2,&cf_denom); 	/* sigma(bi*exp(-sT) */

		if(!Div_CmplxM(&cf_denom,&cf_num,&Tf)){
					/* invert to get normalised values */
			NotifyError("%s: Filter failed to initialise.", funcName);
			exit(1);
		}
		Convert_CmplxM(&Tf,&Tf);
		k2num *= Tf.re; k1num *= Tf.re; k0num *= Tf.re;	/* Normalised */

		for (nth =0; nth < 2 * kth + 1; nth++) {
			numCoeffs[nth]   += k0num * forward[nth];
			numCoeffs[nth+1] += k1num * forward[nth];
			numCoeffs[nth+2] += k2num * forward[nth];
			denomCoeffs[nth]   += k0denom * feedback[nth];
			denomCoeffs[nth+1] += k1denom * feedback[nth];
			denomCoeffs[nth+2] += k2denom * feedback[nth];
		}
		for (nth =0; nth < 2 * (kth + 1) + 1; nth++) {
			forward[nth]  = numCoeffs[nth];
			feedback[nth] = denomCoeffs[nth];
		}
	}
	for (kth = 0; kth < coeffsLength; kth++) {
		p->numCoeffs[kth]   = numCoeffs[kth];
		p->denomCoeffs[kth] = denomCoeffs[kth];
	}

} /* CalcAsymCmpCoeffs_GCFilters */

/**************************** FreeAsymCmpCoeffs *****************************/

/*
 * This routine releases the momory allocated for the Asymmetric compensation 
 * filter coefficients. A custom routine is required because memory is 
 * allocated dynamically for the state vector, according to the filter cascade. 
 */

void
FreeAsymCmpCoeffs_GCFilters(AsymCmpCoeffsPtr *p)
{
	if (*p == NULL)
		return;
	FreeDoubleArray_Common(&(*p)->numCoeffs);
	FreeDoubleArray_Common(&(*p)->denomCoeffs);
	FreeDoubleArray_Common(&(*p)->stateFVector);
	FreeDoubleArray_Common(&(*p)->stateBVector);
	free(*p);
	*p = NULL;

}

/******************** InitERBGammaToneCoeffs **********************************/

ERBGammaToneCoeffsPtr
InitERBGammaToneCoeffs_GCFilters(double centreFreq, double bWidth3dB, 
  double bCoeff, int cascade, double sampleClk)
{
	static const char *funcName = "InitERBGammaToneCoeffs_GCFilters";
	int	   stateVectorLength, kth;
	double bERBw, sin_theta, cos_theta, dt;
/*	double k0num, k1num[GCFILTERS_NUM_CASCADE_ERBGT_FILTER], k2num;	*/
	double k0num, k1num[4], k2num;	
											/* numerator coefficients */
	double k0denom, k1denom, k2denom;		/* denominator coefficients */
	Complex cf_num, cf_denom, Tf, var1, var2, var3;
	ERBGammaToneCoeffsPtr p;
   
	bERBw = (TwoPi * bCoeff * bWidth3dB);
	cos_theta = cos(TwoPi * centreFreq/sampleClk);
	sin_theta = sin(TwoPi * centreFreq/sampleClk);
	dt =  1.0 / sampleClk;

	k1num[0]  = cos_theta - ((1.0 + sqrt(2.0)) * sin_theta);
	k1num[0] *= -dt * exp(-bERBw / sampleClk);
	k1num[1]  = cos_theta + ((1.0 + sqrt(2.0)) * sin_theta);
	k1num[1] *= -dt * exp(-bERBw / sampleClk);
	k1num[2]  = cos_theta - (1.0 - sqrt(2.0)) * sin_theta;
	k1num[2] *= -dt * exp(-bERBw / sampleClk);
	k1num[3]  = cos_theta + (1.0 - sqrt(2.0)) * sin_theta;
	k1num[3] *= -dt * exp(-bERBw / sampleClk);
	k0denom = 1.0;
	k1denom = -2.0 * exp(-bERBw / sampleClk) * cos_theta; 
	k2denom = exp(-2.0 * bERBw / sampleClk);;

	if ((p = (ERBGammaToneCoeffs *) malloc(sizeof(GammaToneCoeffs))) == NULL) {
		NotifyError("%s: Out of memory!", funcName);
		exit(1);
	}
	if ((p->a0 = (double *) calloc(cascade, sizeof(double))) == NULL) {
		NotifyError("%s: Cannot allocate space for a0.", funcName);
		exit(1);
	}
	if ((p->a1 = (double *) calloc(cascade, sizeof(double))) == NULL) {
		NotifyError("%s: Cannot allocate space for a1.", funcName);
		exit(1);
	}
	if ((p->a2 = (double *) calloc(cascade, sizeof(double))) == NULL) {
		NotifyError("%s: Cannot allocate space for a2.", funcName);
		exit(1);
	}
	if ((p->b1 = (double *) calloc(cascade, sizeof(double))) == NULL) {
		NotifyError("%s: Cannot allocate space for b1.", funcName);
		exit(1);
	}
	if ((p->b2 = (double *) calloc(cascade, sizeof(double))) == NULL) {
		NotifyError("%s: Cannot allocate space for b2.", funcName);
		exit(1);
	}
	stateVectorLength = cascade * FILTERS_NUM_GAMMAT_STATE_VARS_PER_FILTER;
	if ((p->stateVector = (double *) calloc(stateVectorLength,
			sizeof(double))) == NULL) {
		NotifyError("%s: Cannot allocate space for state vector.", 
						funcName);
		free(p);
		exit(1);
	}

	for (kth = 0; kth <2 * GCFILTERS_NUM_CASCADE_ERBGT_FILTER ; kth++)
		p->stateVector[kth] = 0.0;

	for (kth = 0; kth <GCFILTERS_NUM_CASCADE_ERBGT_FILTER ; kth++) {

		k0num   = dt;
		k2num   = 0.0; 
		var2.re = k1num[kth]; var2.im = 0.0;
		var3.re =  cos_theta; var3.im = -sin_theta; 
		Mult_CmplxM(&var2,&var3,&var2);			/* a1*exp(-sT) */
		var1.re = k0num; var1.im = 0.0;
		Add_CmplxM (&var1,&var2,&cf_num);		/* a0+a1*exp(-sT) */

		var2.re = k1denom; var2.im = 0.0;
		Mult_CmplxM(&var2,&var3,&var2);			/* b1*exp(-sT) */
		var1.re = k0denom; var1.im = 0.0;
		Add_CmplxM (&var1,&var2,&var1);			/* b0+b1*exp(-sT) */
        var2.re = k2denom; var2.im = 0.0;
		var3.re = cos(2.*TwoPi*centreFreq/sampleClk);
		var3.im = -sin(2.*TwoPi*centreFreq/sampleClk);
		Mult_CmplxM(&var2,&var3,&var2); 		/* b2*exp(-2sT) */
		Add_CmplxM (&var1,&var2,&cf_denom); 	/* sigma(bi*exp(-sT) */

		if(!Div_CmplxM(&cf_denom,&cf_num,&Tf)) {/* invert to get normalised 
																	values */
			NotifyError("%s: Filter failed to initialise.", funcName);
			exit(1);
		}
		Convert_CmplxM(&Tf,&Tf);
		k0num      *= Tf.re;
		k1num[kth] *= Tf.re;

		p->a0[kth] = k0num;
		p->a1[kth] = k1num[kth];
		p->a2[kth] = k2num;
		p->b1[kth] = k1denom;
		p->b2[kth] = k2denom;
	}
	p->cascade = cascade;
	return(p);

} /* InitERBGammaToneCoeffs_GCFilters */

/************************** FreeERBGammaToneCoeffs ****************************/

/*
 * This routine releases the memory allocated for the gamma tone coefficients.
 * A custom routine is required because memory is allocated dynamically for
 * the state vector, according to the filter cascade.
 */

void
FreeERBGammaToneCoeffs_GCFilters(ERBGammaToneCoeffsPtr *p)
{
    if (*p == NULL)
        return;
    free((*p)->a0);
    free((*p)->a1);
    free((*p)->a2);
    free((*p)->b1);
    free((*p)->b2);
    free((*p)->stateVector);
    free(*p);
    *p = NULL;
}


/********************* InitLeakyIntCoeffs *************************************/

/*
 * The function returns a pointer to the coefficients structure of Leaky
 * Integrator. This routine consists with the One-pole filter. 
 */

OnePoleCoeffsPtr 
InitLeakyIntCoeffs_GCFilters(double Tcnst, double sampleClk)
{
	static const char *funcName = "InitLeakyIntCoeffs_GCFilters";
	int		stateVectorLength;
	double k0num;	/* numerator coefficients */
	double k1denom;	/* denominator coefficients */
        OnePoleCoeffsPtr p;

	k1denom=-exp(-1.0/(sampleClk*Tcnst/1000.0));
	k0num=1.0+k1denom;

	if ((p = (OnePoleCoeffs *) malloc(sizeof(OnePoleCoeffs))) == NULL) {
		NotifyError("%s: Out of memory!", funcName);
		exit(1);
    }
	stateVectorLength = GCFILTERS_NUM_LI_STATE_VARS_PER_FILTER;
	if ((p->stateVector = (double *) calloc(stateVectorLength, sizeof(
	  double))) == NULL) {
		NotifyError("%s: Cannot allocate space for state vector.", funcName);
		free(p);
		exit(1);
	}
	p->a0 = k0num; 
	p->b1 = k1denom;
	p->cascade = 1;
	return(p);

} /* InitLeakyIntCoeffs_GCFilters */

/**************************** FreeLeakyIntCoeffs ******************************/

/*
 * This routine releases the momory allocated for the Leaky Integrator
 * coefficients. A custom routine is required because memory is allocated 
 * dynamically for the state vector, according to the filter cascade. 
 */

void
FreeLeakyIntCoeffs_GCFilters(OnePoleCoeffsPtr *p)
{
	if (*p == NULL)
		return;
	free((*p)->stateVector);
	free(*p);
	*p = NULL;

}

/***************************** InitHammingWindow ******************************/

/*
 * The function returns a pointer to the coefficients structure of the Hamming
 * window function.
 */

double *
InitHammingWindow_GCFilters(int winLength)
{
	static const char *funcName = "InitHammingWindow";
	int	nsmpl;
        double 	*win;

	if ((win = (double *) calloc(winLength,sizeof(double))) == NULL) {
		NotifyError("%s: Out of memory!", funcName);
		exit(1);
	}

	if (winLength == 1) 
		win[0] = 1.0;
	else
		for (nsmpl = 0; nsmpl < winLength; nsmpl++) 
			win[nsmpl] = 0.54 - 0.46 * 
						cos(TwoPI*(double)nsmpl / (double)(winLength -1));
 
	return(win);

} /* InitHammingWindow */

/************************** InitERBWindow *************************************/

/*
 * This function returns a pointer to the coefficients structure of the weight-
 * ing function. This routine consists with the Hamming window with the band-
 * width of 3-ERB. 
 */

double *
InitERBWindow_GCFilters(double eRBDensity, int numChannels)
{
	static const char *funcName = "InitERBWindow_GCFilters";
	int	winLength, winMtrxLength, nch, mch, nwin, nee, strPos, endPos;
	double 	*p, *win, sumWin, diffERB;


	if (numChannels > (int) winSizeERB) {
		diffERB = 1.0 / eRBDensity;
		nee = (int) ceil((double) winSizeERB / diffERB);
		winLength = nee + 1 - (nee % 2);	/* Making odd number */
		winMtrxLength = numChannels * numChannels;

		if ((p = (double *) calloc(winMtrxLength,sizeof(double))) == NULL) {
			NotifyError("%s: Out of memory!", funcName);
			exit(1);
		}

		win = InitHammingWindow_GCFilters(winLength);

		sumWin = 0.0;
		for (mch = 0; mch < winLength; mch++)
			sumWin += win [mch];
		for (mch = 0; mch < winLength; mch++)
			win[mch] *= 1.0 / sumWin;

		for (nch = 0; nch < numChannels; nch++) {
			mch = MAX(0, nch - winLength / 2);
			strPos = MAX(winLength / 2 - nch, 0);
			endPos = MIN(winLength, numChannels - nch + winLength / 2);
			for (nwin = strPos; nwin < endPos; nwin++) {
				p[nch + numChannels * mch] = win[nwin];
       			mch++;
			}
		}

		FreeDoubleArray_Common(&win);
	}
	else {							/*	making an eye-matrix	*/	

		if ((p = (double *) calloc(numChannels * numChannels,
				sizeof(double))) == NULL) {
			NotifyError("%s: Out of memory!", funcName);
			exit(1);
		}

 
		for (mch = 0; mch < numChannels; mch++) 
			for (nch = 0; nch < numChannels; nch++)
				if (mch == nch)
					p[mch + numChannels * nch] = 1.0;
	}

	return(p);

} /* InitERBWindow_GCFilters */

/********************* AsymCmp ************************************************/

/*
 * Asymmetric Compensation Filterbank
 *
 * This routine filters the signal, returning the result in the same signal
 * passed to the routine.
 * No checks are made here as to whether or not the asymmetric compensation
 * filter coefficitnts, paased as an argument array, have been initialised. 
 * Each channel has its own filter (coefficients).
 * It is assumed that the 'theSignal' has been correctly initialised.
 */

void
AsymCmp_GCFilters(SignalDataPtr theSignal, ChanLen nsmpl, AsymCmpCoeffs *p[])
{
	/* static const char *funcName = "AsymCmp_GCFilters"; */
	int			kth, nch;
	register	double		*ptr1, *ptr2, wn; /* Inner loop variables */
	register	ChanData	*data;

	for (nch = theSignal->offset; nch < theSignal->numChannels; nch++) {
		data = &theSignal->channel[nch][nsmpl];
		wn = *data;		/* Temp variable */
		*data *= p[nch]->numCoeffs[0];
		ptr1 = p[nch]->stateFVector;
		for (kth = 1; kth < 2 * p[nch]->cascade + 1; kth++)
			*data += p[nch]->numCoeffs[kth] * *(ptr1++);
		ptr2 = p[nch]->stateBVector;
		for (kth = 1; kth < 2 * p[nch]->cascade + 1; kth++)
			*data -= p[nch]->denomCoeffs[kth] * *(ptr2++);
		for (kth = 2 * p[nch]->cascade; kth > 0; kth--)
			p[nch]->stateFVector[kth] = p[nch]->stateFVector[kth - 1];
		for (kth = 2 * p[nch]->cascade; kth > 0; kth--)
			p[nch]->stateBVector[kth] = p[nch]->stateBVector[kth - 1];
		p[nch]->stateFVector[0] = wn;
		p[nch]->stateBVector[0] = *data;
	}

}

/************************** ERBGammaTone **************************************/

/*
 * This routine filters the signal, returning the result in the same signal
 * passed to the routine.
 * No checks are made here as to whether or not the gamma tone coefficients,
 * passed as an argument array, have been initialised.
 * Each channel has its own filter (coefficients).
 */

void
ERBGammaTone_GCFilters(SignalDataPtr theSignal, ERBGammaToneCoeffs *p[])
{ 
	static const char *funcName = "ERBGammaTone_GCFilters";
	int			nch, kth;
	ChanLen		nsmpl;
	register	double      *ptr1, *ptr2, wn;   /* Inner loop variables */
	register	ChanData    *data;

	if (!CheckPars_SignalData(theSignal)) {
		NotifyError("%s: Signal not correctly initialised.", funcName);
		exit(1);
	}

	/* For the allocation of space to the state vector, the filter for all
	 * channels are assumed to have the same cascade as the first. */
    
	for (nch = theSignal->offset; nch < theSignal->numChannels; nch++) {
		data = theSignal->channel[nch];
		for (nsmpl = 0; nsmpl < theSignal->length; nsmpl++, data++) {
			ptr1 = p[nch]->stateVector;
			for (kth = 0; kth < p[nch]->cascade; kth++) {
				ptr2 = ptr1;
				*data -= p[nch]->b1[kth] * *(ptr1++);
				*data -= p[nch]->b2[kth] * *ptr1;
				wn = *data;     						/* Temp variable w(n) */
				*data *= p[nch]->a0[kth];
				*data += p[nch]->a1[kth] * *(ptr2); 	/* Final w(n) */
/*				*data += p[nch]->a2[kth] * *(ptr2--); */	/* Final w(n) */
				*ptr1 = *ptr2;  					/* Update w(n-1) to w(n-2)*/
				*ptr2 = wn;
				ptr1++;
			}
		}
	}
}


/***************************** LeakyInt ***************************************/

/*
 * This routine filters the signal, returning the result in the same signal
 * passed to the routine.
 * No checks are made here as to whether or not the Leaky Integrator
 * coefficitnts, paased as an argument array, have been initialised. 
 * Each channel has its own filter (coefficients).
 */

void
LeakyInt_GCFilters(CntlGammaCPtr *p, OnePoleCoeffsPtr *q, int numChannels)
{
	static const char *funcName = "LeakyInt_GCFilters";
	int			nch;
	register	double	*ptr1;    /* Inner loop variables */

	if (!CheckCntlInit_GCFilters(p)) {
		NotifyError("%s: cntlGammaC not set in %s.", funcName);
		exit(1);
	}	
	 
	for (nch = 0; nch < numChannels; nch++) {
		ptr1 = q[nch]->stateVector;
		p[nch]->outSignalLI *= q[nch]->a0;
		p[nch]->outSignalLI -= q[nch]->b1 * *ptr1;
		*ptr1 = p[nch]->outSignalLI;
	}

}

/**************************** CheckInit ***************************************/

/*
 * This function checks the module by setting module's parameter pointer
 * structure.
 */

BOOLN
CheckCntlInit_GCFilters(CntlGammaCPtr *cntlGammaC)
{
	static const char *funcName = "CheckPars_CntlGammaC";

	if (cntlGammaC == NULL) {
		NotifyError("%s: cntlGammaC not set in %s.", funcName);
		return(FALSE);
	}
	return(TRUE);

}

/************************ SetpsEst ********************************************/

/*
 * This function sets the parameter of psEst for the gammachirp filters.
 */

void
SetpsEst_GCFilters(CntlGammaCPtr *cntlGammaC, int numChannels,
  double *winPsEst, double coefPsEst)
{
	/* static const char *funcName = "SetpsEst_GCFilters"; */
	int	mch, nch;
	double	winOut, maxPsEst;

	maxPsEst = 0.0;
	for (mch = 0; mch < numChannels; mch++) {
		winOut = 0.0;
      	for (nch = 0; nch < numChannels; nch++) 
		winOut += winPsEst[mch + nch * numChannels] * cntlGammaC[
		  nch]->outSignalLI;
		winOut = MAX(winOut, pow(10.0, -10.0));	
					/* Limit minimum value for log10 */
		cntlGammaC[mch]->psEst = MIN(20.0 * log10(coefPsEst * winOut), 120.0 );
		if (maxPsEst < cntlGammaC[mch]->psEst)
			maxPsEst = cntlGammaC[mch]->psEst;
	}
/*	if (maxPsEst > 120) {
		NotifyError("%s: Estimated Ps is too large.", funcName);
		exit(1);
	}	
*/
}

/************************* SetcEst ********************************************/

/*
 * This function sets the parameter of cEst for the gammachirp filters.
 */

void
SetcEst_GCFilters(CntlGammaCPtr *cntlGammaC, int numChannels,
  double cCoeff0, double cCoeff1, double cLowerLim, double cUpperLim)
{
	/* static const char *funcName = "SetcEst_GCFilters"; */
	int	nch;

	for (nch = 0; nch < numChannels; nch++) 
		cntlGammaC[nch]->cEst = cCoeff0+cCoeff1*cntlGammaC[nch]->psEst;

	if (fabs(cUpperLim - cLowerLim) > 1.0) {
		for (nch = 0; nch < numChannels; nch++) {
			if (cntlGammaC[nch]->cEst < cLowerLim) 
				cntlGammaC[nch]->cEst = cLowerLim;
			if (cntlGammaC[nch]->cEst > cUpperLim) 
				cntlGammaC[nch]->cEst = cUpperLim;
		}	
	}
	else {
		for (nch = 0; nch < numChannels; nch++)
			cntlGammaC[nch]->cEst = cLowerLim + cUpperLim *  
								atan(-cntlGammaC[nch]->cEst + cLowerLim);
	}

}

/************************* SetaEst ********************************************/

/*
 * This function sets the parameter of aEst for the gammachirp filters.
 */

void
SetaEst_GCFilters(CntlGammaCPtr *cntlGammaC, int numChannels, double cmprs)
{
	/* static const char *funcName = "SetaEst_GCFilters"; */
	int	nch;

	if (cmprs == 1.0) {
		for (nch = 0; nch < numChannels; nch++)
			cntlGammaC[nch]->aEst = 1.0;	
	}
	else {
		for (nch = 0; nch < numChannels; nch++)
			cntlGammaC[nch]->aEst =  pow(10.0, (-cntlGammaC[nch]->aEst /
												20.0 * cmprs));
						/* 0.5 NG, 0.3 OK */
	}
}


/****************** CntlGammaChirp ********************************************/

/*
 * Control block of gammachirp filterbank
 * This routine controls the parameters of the gammachirp filterbank
 * in order to estimate the power level and then deterime parameter c, related 
 * as the asymmetry of gammachirp.
 * No checks are made here as to whether or not the parameters of Control block,
 * paased as an argument array, have been initialised. 
 * Each channel has its own filter (coefficients).
 * It is assumed that the 'theSignal' has been correctly initialised.
 */

void
CntlGammaChirp_GCFilters(SignalDataPtr theSignal, ChanLen nsmpl, 
  CntlGammaCPtr *cntlGammaC, double cCoeff0, double cCoeff1, double cLowerLim,
  double cUpperLim, double *winPsEst, double coefPsEst, double cmprs, 
  OnePoleCoeffsPtr *coefficientsLI)
{
	/* static const char *funcName = "CntlGammaChirp_GCFilters"; */
	int nch;

	
	for (nch = theSignal->offset; nch < theSignal->numChannels; nch++) 
		cntlGammaC[nch]->outSignalLI = (MAX(theSignal->channel[nch][nsmpl], 0.0)
							+ MAX(-theSignal->channel[nch][nsmpl], 0.0)) / 2.0;

	LeakyInt_GCFilters(cntlGammaC, coefficientsLI, theSignal->numChannels);

	SetpsEst_GCFilters(cntlGammaC, theSignal->numChannels, winPsEst, 
								coefPsEst);

	SetcEst_GCFilters(cntlGammaC, theSignal->numChannels, cCoeff0,
							cCoeff1, cLowerLim, cUpperLim);

	SetaEst_GCFilters(cntlGammaC, theSignal->numChannels, cmprs);

}

/********************* InitCntlGammaChirp *************************************/

/*
 * The function returns a pointer to the coefficients structure of CntlGamma-
 * Chirp_GCFilters.
 */

CntlGammaCPtr 
InitCntlGammaChirp_GCFilters(void)
{
	static const char *funcName = "InitCntlGammaChirp_GCFilters";
        CntlGammaCPtr p;

	if ((p = (CntlGammaC *) malloc(sizeof(CntlGammaC))) == NULL) {
		NotifyError("%s: Out of memory!", funcName);
		exit(1);
    }
	p->outSignalLI = 0.0; 
	p->aEst = 0.0; 
	p->cEst = 0.0; 
	p->psEst = 0.0; 
	return(p);

} /* InitCntlGammaChirp_GCFilters */

/************************* FreeCntlGammaChirp *********************************/

/*
 * This routine releases the momory allocated for the Control block of the 
 * GammaChirp filter. A custom routine is required because memory is 
 * allocated dynamically for the state vector, according to the filter cascade. 
 */

void
FreeCntlGammaChirp_GCFilters(CntlGammaCPtr *p)
{
	if (*p == NULL)
		return;
	free(*p);
	*p = NULL;

}


