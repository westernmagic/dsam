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
#include "GeEarObject.h"
#include "UtFilters.h"
#include "UtFFT.h"
#include "UtBandwidth.h"
#include "UtGCFilters.h"


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
	static const WChar *funcName = wxT("InitAsymCmpCoeffs_GCFilters");
	int    stateVectorLength, cascadeAC, coeffsLength;
	AsymCmpCoeffsPtr p;

	cascadeAC = GCFILTERS_NUM_CASCADE_ACF_FILTER;	
	coeffsLength = cascadeAC * GCFILTERS_NUM_ACF_STATE_VARS_PER_FILTER + 1;

	if ((p = (AsymCmpCoeffs *) malloc(sizeof(AsymCmpCoeffs))) == NULL) {
		NotifyError(wxT("%s: Out of memory!"), funcName);
		exit(1);
    }
	if ((p->numCoeffs = (double *) calloc(coeffsLength, sizeof(double))) ==
	  NULL) {
		NotifyError(wxT("%s: Cannot allocate space for numCoeffs."), funcName);
		free(p);
		exit(1);
	}
	if ((p->denomCoeffs = (double *) calloc(coeffsLength, sizeof(double))) ==
	  NULL) {
		NotifyError(wxT("%s: Cannot allocate space for denomCoeffs."),
		  funcName);
		free(p);
		exit(1);
	}

	stateVectorLength = cascadeAC * GCFILTERS_NUM_ACF_STATE_VARS_PER_FILTER + 1;
	if ((p->stateFVector = (double *) calloc(stateVectorLength, sizeof(
	  double))) == NULL) {
		NotifyError(wxT("%s: Cannot allocate space for state Forward-vector."),
		  funcName);
		free(p);
		exit(1);
	}
	if ((p->stateBVector = (double *) calloc(stateVectorLength, sizeof(
	  double))) == NULL) {
		NotifyError(wxT("%s: Cannot allocate space for state Backward-vector."),
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
	static const WChar *funcName = wxT("CalcAsymCmpCoeffs_GCFilters");
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
		NotifyError(wxT("%s: Memory for coefficients have not been allocated."),
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
			NotifyError(wxT("%s: Filter failed to initialise."), funcName);
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
	static const WChar *funcName = wxT("InitERBGammaToneCoeffs_GCFilters");
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
		NotifyError(wxT("%s: Out of memory!"), funcName);
		exit(1);
	}
	if ((p->a0 = (double *) calloc(cascade, sizeof(double))) == NULL) {
		NotifyError(wxT("%s: Cannot allocate space for a0."), funcName);
		exit(1);
	}
	if ((p->a1 = (double *) calloc(cascade, sizeof(double))) == NULL) {
		NotifyError(wxT("%s: Cannot allocate space for a1."), funcName);
		exit(1);
	}
	if ((p->a2 = (double *) calloc(cascade, sizeof(double))) == NULL) {
		NotifyError(wxT("%s: Cannot allocate space for a2."), funcName);
		exit(1);
	}
	if ((p->b1 = (double *) calloc(cascade, sizeof(double))) == NULL) {
		NotifyError(wxT("%s: Cannot allocate space for b1."), funcName);
		exit(1);
	}
	if ((p->b2 = (double *) calloc(cascade, sizeof(double))) == NULL) {
		NotifyError(wxT("%s: Cannot allocate space for b2."), funcName);
		exit(1);
	}
	stateVectorLength = cascade * FILTERS_NUM_GAMMAT_STATE_VARS_PER_FILTER;
	if ((p->stateVector = (double *) calloc(stateVectorLength,
			sizeof(double))) == NULL) {
		NotifyError(wxT("%s: Cannot allocate space for state vector."), 
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
			NotifyError(wxT("%s: Filter failed to initialise."), funcName);
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
	static const WChar *funcName = wxT("InitLeakyIntCoeffs_GCFilters");
	int		stateVectorLength;
	double k0num;	/* numerator coefficients */
	double k1denom;	/* denominator coefficients */
        OnePoleCoeffsPtr p;

	k1denom=-exp(-1.0/(sampleClk*Tcnst/1000.0));
	k0num=1.0+k1denom;

	if ((p = (OnePoleCoeffs *) malloc(sizeof(OnePoleCoeffs))) == NULL) {
		NotifyError(wxT("%s: Out of memory!"), funcName);
		exit(1);
    }
	stateVectorLength = GCFILTERS_NUM_LI_STATE_VARS_PER_FILTER;
	if ((p->stateVector = (double *) calloc(stateVectorLength, sizeof(
	  double))) == NULL) {
		NotifyError(wxT("%s: Cannot allocate space for state vector."),
		  funcName);
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
	static const WChar *funcName = wxT("InitHammingWindow");
	int	nsmpl;
        double 	*win;

	if ((win = (double *) calloc(winLength,sizeof(double))) == NULL) {
		NotifyError(wxT("%s: Out of memory!"), funcName);
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
	static const WChar *funcName = wxT("InitERBWindow_GCFilters");
	int	winLength, winMtrxLength, nch, mch, nwin, nee, strPos, endPos;
	double 	*p, *win, sumWin, diffERB;


	if (numChannels > (int) winSizeERB) {
		diffERB = 1.0 / eRBDensity;
		nee = (int) ceil((double) winSizeERB / diffERB);
		winLength = nee + 1 - (nee % 2);	/* Making odd number */
		winMtrxLength = numChannels * numChannels;

		if ((p = (double *) calloc(winMtrxLength,sizeof(double))) == NULL) {
			NotifyError(wxT("%s: Out of memory!"), funcName);
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
			NotifyError(wxT("%s: Out of memory!"), funcName);
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
	/* static const WChar *funcName = wxT("AsymCmp_GCFilters"); */
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
	static const WChar *funcName = wxT("ERBGammaTone_GCFilters");
	int			nch, kth;
	ChanLen		nsmpl;
	register	double      *ptr1, *ptr2, wn;   /* Inner loop variables */
	register	ChanData    *data;

	if (!CheckPars_SignalData(theSignal)) {
		NotifyError(wxT("%s: Signal not correctly initialised."), funcName);
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
	static const WChar *funcName = wxT("LeakyInt_GCFilters");
	int			nch;
	register	double	*ptr1;    /* Inner loop variables */

	if (!CheckCntlInit_GCFilters(p)) {
		NotifyError(wxT("%s: cntlGammaC not set in %s."), funcName);
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
	static const WChar *funcName = wxT("CheckPars_CntlGammaC");

	if (cntlGammaC == NULL) {
		NotifyError(wxT("%s: cntlGammaC not set in %s."), funcName);
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
	/* static const WChar *funcName = wxT("SetpsEst_GCFilters"); */
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
		NotifyError(wxT("%s: Estimated Ps is too large."), funcName);
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
	/* static const WChar *funcName = wxT("SetcEst_GCFilters"); */
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
	/* static const WChar *funcName = wxT("SetaEst_GCFilters"); */
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
	/* static const WChar *funcName = wxT("CntlGammaChirp_GCFilters"); */
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
	static const WChar *funcName = wxT("InitCntlGammaChirp_GCFilters");
        CntlGammaCPtr p;

	if ((p = (CntlGammaC *) malloc(sizeof(CntlGammaC))) == NULL) {
		NotifyError(wxT("%s: Out of memory!"), funcName);
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

/****************************** PGCCarrierList ********************************/

/*
 * This routine returns a name specifier for the passive dynamic gamma chirp
 * carrier options list.
 */

NameSpecifier *
PGCCarrierList_GCFilters(int index)
{
	static NameSpecifier	modeList[] = {

					{ wxT("COMPLEX"),	GCFILTER_CARRIER_MODE_COMPLEX},
					{ wxT("COS"),		GCFILTER_CARRIER_MODE_COS},
					{ wxT("ENVELOPE"),	GCFILTER_CARRIER_MODE_ENVELOPE},
					{ wxT("SIN"),		GCFILTER_CARRIER_MODE_SIN},
					{ NULL,				GCFILTER_CARRIER_MODE_NULL},
				
				};
	return (&modeList[index]);

}

/*************************** FindPeakGainFreq **********************************/

/*
 * This function returns the peak gain frequency index.
 * i.e. the 'freqz' calculation. 
 * I need to sort out the case when no FFTW3 is available.
 * This use of fftw_plan is safe because this code is not run threaded.
 */

double
FindPeakGainFreq_GCFilters(double *b, size_t len, double sR, double freqPeak)
{
	static const WChar	*funcName = wxT("FindPeakGainFreq_GCFilters");
	register size_t	i, peakIndex;
	register double	*p;
	double	*peakFFT, peakFFTModulus;
	FFTArrayPtr	fT;
	fftw_plan	plan;

	if ((fT = (FFTArrayPtr) InitArray_FFT(len * 2, TRUE, 1)) == NULL) {
		NotifyError(wxT("%s: Couldn't allocate memory for FFT array structure."),
		  funcName);
		return(FALSE);
	}
	for (i = 0, p = fT->data; i < len; i++)
		*p++ = *b++;
	for (; i < fT->fftLen; i++)
		*p++ = 0.0;

	plan = fftw_plan_dft_r2c_1d(fT->dataLen, fT->data, (fftw_complex *) fT->data,
	  FFTW_ESTIMATE);
	fftw_execute(plan);
	fftw_destroy_plan(plan);

	peakIndex = (size_t) floor(freqPeak / sR * fT->dataLen + 0.5);
	peakFFT = fT->data + peakIndex * 2;
	peakFFTModulus = sqrt(SQR(*peakFFT) + SQR(*(peakFFT + 1)));
	FreeArray_FFT(&fT);
	return(peakFFTModulus);

}


/*************************** GammaChirpAmpFreqResp ****************************/

/*
 * This routine calculates the frequency response of a gamma chirp
 * It was revised from the aim2007b GammaChirpFrsp.m matlab script.
 */

void
GammaChirpAmpFreqResp_GCFilters(double *ampFrsp, double frs, double eRBw,
  double sR, double orderG, double coefERBw, double coefC, double phase)
{
	int		i;
	double	*p, fExpr, f;
	double	bh = coefERBw * eRBw, cn = coefC / orderG, nOver2 = orderG / 2.0;
	double	cnSqrPlus1 = 1.0 + SQR(cn), atan_cn = atan(cn);
	double	deltaFreq = sR / GCFILTERS_NUM_FRQ_RSL / 2;

	for (i = 0, p = ampFrsp, f = 0.0; i < GCFILTERS_NUM_FRQ_RSL; i++, f +=
	  deltaFreq) {
	  	fExpr = (f - frs) / bh;
		*p++ = pow(cnSqrPlus1 / (1 + SQR(fExpr)), nOver2) *
		   exp(coefC * (atan(fExpr) - atan_cn));
	}
}

/*************************** InitPGammaChirpCoeffs *************************/

/*
 * This function returns a set of FFT Filter coeffients for the passive GammaChirp to be used
 * in a FFT filter.
 * It is revised from the "GammaChirp.m" matlab script.
 * This routine actually calculates the first stage of the fft Filter, which was
 * was originally done in fftfilt.m.
 */

GammaChirpCoeffsPtr
InitPGammaChirpCoeffs_GCFilters(double cF, double bw, double sR, double orderG, double 
  coefERBw, double coefC, double phase, int swCarr, int swNorm, SignalDataPtr inSignal)
{
	static const WChar *funcName = wxT("InitPGammaChirpCoeffs_GCFilters");
	register double *pp, t, gammaEnv, maxGammaEnv;
	double	lenGC1kHz, peakGain;
	ChanLen	i, lenGC;
	GammaChirpCoeffsPtr	p;

	if ((p = (GammaChirpCoeffsPtr) malloc(sizeof(GammaChirpCoeffs))) == NULL) {
	 	NotifyError(wxT("%s: Out of memory."), funcName);
	 	return(NULL);
	}
	p->pGC = p->pGCOut = NULL;
	lenGC1kHz = (40.0 * orderG / coefERBw + 200.0) * sR / 16000.0;
	if (swCarr == GCFILTER_CARRIER_MODE_SIN)
		phase -= PI_2;
	phase += coefC * log(cF / GCFILTERS_REF_FREQ);		/* Relative phase to 1 kHz */
	lenGC = (ChanLen)floor(lenGC1kHz * ERBFromF_Bandwidth(GCFILTERS_REF_FREQ) / bw);
	if ((p->pGC = InitArray_FFT(lenGC + inSignal->length, TRUE, 1)) == NULL) {
		NotifyError(wxT("%s: Out of memory for coeffients array (CF = %g Hz)."), funcName,
		  cF);
		FreePGammaChirpCoeffs_GCFilters(&p);
		return(NULL);
	}
	p->pGC->plan[0] = fftw_plan_dft_r2c_1d(p->pGC->fftLen,
	  p->pGC->data, (fftw_complex *) p->pGC->data, FFTW_ESTIMATE);
	if ((p->pGCOut = InitArray_FFT(p->pGC->dataLen, TRUE, 2)) == NULL) {
		NotifyError(wxT("%s: Could not initialise pGC work FFT array."), funcName);
		FreePGammaChirpCoeffs_GCFilters(&p);
	}
	p->pGCOut->plan[GCFILTERS_PGC_FORWARD_PLAN] = fftw_plan_dft_r2c_1d(
	  p->pGCOut->fftLen, p->pGCOut->data, (fftw_complex *) p->pGCOut->data,
	  FFTW_ESTIMATE);
	p->pGCOut->plan[GCFILTERS_PGC_BACKWARD_PLAN] = fftw_plan_dft_c2r_1d(
	  p->pGCOut->fftLen, (fftw_complex *) p->pGCOut->data, p->pGCOut->data,
	  FFTW_ESTIMATE);
	*p->pGC->data = 0.0;
	for (i = 1, pp = p->pGC->data + 1, maxGammaEnv = 0.0; i < lenGC; i++, pp++) {
		t = i / sR;
		gammaEnv = pow(t, orderG - 1) * exp(-PIx2 * coefERBw * bw * t);
		switch (swCarr) {
		case GCFILTER_CARRIER_MODE_ENVELOPE:
			*pp = gammaEnv;
			break;
		case GCFILTER_CARRIER_MODE_COMPLEX:
			*pp = gammaEnv * exp(i * (PIx2 * cF * t + coefC * log(t)+ phase));
			break;
		default:
			*pp = gammaEnv * cos(PIx2 * cF * t + coefC * log(t) + phase);
		}
		if (gammaEnv > maxGammaEnv)
			maxGammaEnv = gammaEnv;
	}
	for (i = 0, pp = p->pGC->data; i < lenGC; i++)
		*pp++ /= maxGammaEnv;

	if (swNorm) {
		peakGain = FindPeakGainFreq_GCFilters(p->pGC->data, lenGC, sR, cF + coefC *
		  bw * coefERBw / orderG);
		for (i = 0, pp = p->pGC->data; i < lenGC; i++)
			*pp++ /= peakGain;
		for ( ; i < p->pGC->fftLen; i++)
			*pp++ = 0.0;
	}
	/* This next calculation was originally the first part of fftfilt.m */
	fftw_execute(p->pGC->plan[0]);
	return (p);

}

/*************************** FreePGammaChirpCoeffs ****************************/

void
FreePGammaChirpCoeffs_GCFilters(GammaChirpCoeffsPtr *p)
{
	if (!*p)
		return;
	if ((*p)->pGC)
		FreeArray_FFT(&(*p)->pGC);
	if ((*p)->pGCOut)
		FreeArray_FFT(&(*p)->pGCOut);
	free(*p);
	*p = NULL;

}
 
/*************************** PassiveGCFilter **********************************/

/*
 * This functions filters "signal" with the FIR coefficients "fIR" using the FFT.
 * This is based on the fftfilt.m script, but the first fft on the pGC array has already
 * been done.
 */

void
PassiveGCFilter_GCFilters(EarObjectPtr data, GammaChirpCoeffsPtr *pGCoeffs)
{
	register double	*p1, *p2;
	register fftw_complex	*c1, *c2;
	int		chan;
	ChanLen		i;
	fftw_complex	tempC;
	FFTArrayPtr	pGC, pGCOut;
	SignalDataPtr	inSignal, outSignal;

	inSignal = _InSig_EarObject(data, 0);
	outSignal = _OutSig_EarObject(data);
	for (chan = outSignal->offset; chan < outSignal->numChannels; chan++) {
		pGC = pGCoeffs[chan]->pGC;
		pGCOut = pGCoeffs[chan]->pGCOut;
		for (i = 0, p1 = pGCOut->data, p2 = inSignal->channel[chan % inSignal->
		  interleaveLevel]; i < inSignal->length; i++)
			*p1++ = *p2++;
		for ( ; i < pGC->fftLen; i++)
			*p1++ = 0.0;
		fftw_execute(pGCOut->plan[GCFILTERS_PGC_FORWARD_PLAN]);
	
		for (i = 0, c1 = (fftw_complex *) pGCOut->data, c2 = (fftw_complex *) pGC->data;
		  i < pGC->fftLen; i++, c1++, c2++) {
			CMPLX_MULT(tempC, *c1, *c2);
			CMPLX_COPY(*c1, tempC);
		}
	
		fftw_execute(pGCOut->plan[GCFILTERS_PGC_BACKWARD_PLAN]);
		for (i = 0, p1 = outSignal->channel[chan], p2 = pGCOut->data; i <
		  outSignal->length; i++)
			*p1++ = *p2++ / pGCOut->fftLen;
	}

}

/*************************** FreeAsymCmpCoeffs2 ***************************/

void
FreeAsymCmpCoeffs2_GCFilters(AsymCmpCoeffs2Ptr *p)
{
	if (!*p)
		return;
	if ((*p)->ap)
		free((*p)->ap);
	if ((*p)->bz)
		free((*p)->bz);
	if ((*p)->sigInPrev)
		free((*p)->sigInPrev);
	if ((*p)->sigOutPrev)
		free((*p)->sigOutPrev);
	free(*p);
	*p = NULL;

}

/*************************** InitAsymCmpCoeffs2 ***************************/

/*
 * This function initialises the Asymmetric Compensation Filters coefficients
 * memory space and sets the static coefficients
 */

AsymCmpCoeffs2Ptr
InitAsymCmpCoeffs2_GCFilters(int cascade, double fs, double b, double c,
  BandwidthModePtr bMode)
{
	const static WChar	*funcName = wxT("InitAsymCmpCoeffs2_GCFilters");
	int		totalCoeffs = cascade * GCFILTERS_ACF_NUM_COEFFS;
	AsymCmpCoeffs2Ptr p;

	if ((p = (AsymCmpCoeffs2Ptr) malloc(sizeof(AsymCmpCoeffs2))) == NULL) {
		NotifyError(wxT("%s: Out of memory for coeffient structure."), funcName);
		return(NULL);
	}
	p->ap = p->bz = NULL;
	p->sigInPrev = p->sigOutPrev = NULL;

	if ((p->ap = (double *) calloc(totalCoeffs, sizeof(double))) == NULL) {
		NotifyError(wxT("%s: Out of memory for 'ap' coeffients (%d."), funcName,
		  totalCoeffs);
		FreeAsymCmpCoeffs2_GCFilters(&p);
		return(NULL);
	}
	if ((p->bz = (double *) calloc(totalCoeffs, sizeof(double))) == NULL) {
		NotifyError(wxT("%s: Out of memory for 'bz' coeffients (%d."), funcName,
		  totalCoeffs);
		FreeAsymCmpCoeffs2_GCFilters(&p);
		return(NULL);
	}
	if ((p->sigInPrev = (double *) calloc(GCFILTERS_ACF_LEN_STATE_VECTOR,
	  sizeof(double))) == NULL) {
		NotifyError(wxT("%s: Out of memory for sigInPrev state vector (%d."), funcName,
		  totalCoeffs);
		FreeAsymCmpCoeffs2_GCFilters(&p);
		return(NULL);
	}
	if ((p->sigOutPrev = (double *) calloc(cascade * GCFILTERS_ACF_LEN_STATE_VECTOR,
	  sizeof(double))) == NULL) {
		NotifyError(wxT("%s: Out of memory for sigOutPrev state vector (%d."), funcName,
		  totalCoeffs);
		FreeAsymCmpCoeffs2_GCFilters(&p);
		return(NULL);
	}
	p->p0 = GCFILTERS_ACF_P0;
	p->p1 = GCFILTERS_P1(b, c);
	p->p2 = GCFILTERS_P2(b, c);
	p->p3 = GCFILTERS_P3(b, c);
	p->p4 = GCFILTERS_ACF_P4;
	p->numFilt = cascade;
	p->fs = fs;
	p->b = b;
	p->c = c;
	p->bMode = bMode;
	return(p);

}

/*************************** SetAsymCmpCoeffs2 *****************************/

/*
 * This function sets the Asymmetric Compensation Filters coefficients.
 */

#define	MAX_ANGLE(A)	(PIx2 * (((sumF = A) > 0)? sumF: 0) / p->fs)

void
SetAsymCmpCoeffs2_GCFilters(AsymCmpCoeffs2Ptr p, double frs)
{
	const static WChar	*funcName = wxT("SetAsymCmpCoeffs2_GCFilters");
	int		i, j;
	double	*ap, *bz, r, eRBw, delFrs, phi, psy, sumF, nrm;
	double	*apStart, *bzStart;
	Complex	vwrs[GCFILTERS_ACF_NUM_COEFFS], vwrApSum, vwrBzSum, workVar;

	eRBw = BandwidthFromF_Bandwith(p->bMode, frs);
	for (i = 0, apStart = p->ap, bz = bzStart = p->bz; i < p->numFilt; i++,
	  apStart += GCFILTERS_ACF_NUM_COEFFS, bzStart += GCFILTERS_ACF_NUM_COEFFS) {
		r = exp(-p->p1 * pow((p->p0 / p->p4), i) * PIx2 * p->b * eRBw / p->fs);
		delFrs = pow((p->p0 * p->p4), i) * p->p2 * p->c * p->b * eRBw;
		phi = MAX_ANGLE(frs + delFrs);
		psy = MAX_ANGLE(frs - delFrs);
		ap = apStart;
		bz = bzStart;
		*ap++ = *bz++ = 1.0;
		*ap++ = -2.0 * r * cos(phi);
		*bz++ = -2.0 * r * cos(psy);
		*ap++ = *bz++ = SQR(r);
		/* Normalisation calculations */
		EXP_CMPLXM(vwrs[0], 0.0);
		EXP_CMPLXM(vwrs[1], PIx2 * frs / p->fs);
		MULT_CMPLXM(vwrs[2], vwrs[1], vwrs[1]);
		SET_CMPLXM(vwrApSum, 0.0, 0.0);
		SET_CMPLXM(vwrBzSum, 0.0, 0.0);
		for (j = 0, ap = apStart, bz = bzStart; j < GCFILTERS_ACF_NUM_COEFFS;
		  j++, ap++, bz++) {	/* Mustn't use the pointer decrement in the macros! */
			SCALER_MULT_CMPLXM(workVar, vwrs[j], *ap);
			ADD_CMPLXM(vwrApSum, vwrApSum, workVar);
			SCALER_MULT_CMPLXM(workVar, vwrs[j], *bz);
			ADD_CMPLXM(vwrBzSum, vwrBzSum, workVar);
		}
		if (!Div_CmplxM(&vwrApSum, &vwrBzSum, &workVar)) {
			NotifyError(wxT("%s: Failed complex division term (frs = %g)."), funcName,
			  frs);
			return;
		}
		nrm = MODULUS_CMPLX(workVar);
		for (j = 0, bz = bzStart; j < GCFILTERS_ACF_NUM_COEFFS; j++)
			*bz++ *= nrm;
	}

}

/*************************** ACFilterBank *********************************/

/*
 * IIR ACF time-slice filtering for time-varing filter
 * Revised from ACFilterBank.m from the aim2007b matlab code.
 */

#define UPDATE_STATE_VECTOR(SV, DATA)	\
		{ \
			for (j = 1, pp = (SV); j < GCFILTERS_ACF_LEN_STATE_VECTOR; j++, pp++) \
				*pp = *(pp + 1); \
			*pp = (DATA); \
		}
			
void
ACFilterBank_GCFilters(AsymCmpCoeffs2Ptr *aCFCoeffs, EarObjectPtr data,
  ChanLen sample)
{
	register ChanData	**dataPtr, *pp, *p1, forwardSum, backwardSum, y;
	int		chan, i, j;
	ChanData	*apStart, *bzStart, *x, *sigOutPrev;

	dataPtr = _OutSig_EarObject(data)->channel + _OutSig_EarObject(data)->offset;
	for (chan = _OutSig_EarObject(data)->offset; chan < _OutSig_EarObject(
	  data)->numChannels; chan++, dataPtr++, aCFCoeffs++) {
		UPDATE_STATE_VECTOR((*aCFCoeffs)->sigInPrev, *(*dataPtr + sample));
	  	x = (*aCFCoeffs)->sigInPrev;
		for (i = 0, apStart = (*aCFCoeffs)->ap, bzStart = (*aCFCoeffs)->bz,
		  sigOutPrev = (*aCFCoeffs)->sigOutPrev; i < (*aCFCoeffs)->numFilt;
		   i++, apStart += GCFILTERS_ACF_NUM_COEFFS, bzStart +=
		   GCFILTERS_ACF_NUM_COEFFS, sigOutPrev += GCFILTERS_ACF_LEN_STATE_VECTOR) {
			for (j = 0, forwardSum = 0.0, p1 = bzStart, pp = x +
			  GCFILTERS_ACF_LEN_STATE_VECTOR; j < GCFILTERS_ACF_LEN_STATE_VECTOR; j++)
				forwardSum += *p1++ * *(--pp);
			for (j = 1, backwardSum = 0.0, p1 = apStart + 1, pp = sigOutPrev +
			  GCFILTERS_ACF_LEN_STATE_VECTOR; j < GCFILTERS_ACF_LEN_STATE_VECTOR; j++)
				backwardSum += *p1++ * *(--pp);
			y = (forwardSum - backwardSum) / *apStart;
			UPDATE_STATE_VECTOR(sigOutPrev, y);
			x = sigOutPrev;
		}
		*(*dataPtr + sample) = y;
	}

}
#undef UPDATE_STATE_VECTOR

/*************************** ASympCmpFreqResp ******************************/

/*
 * This routine calculates the frequency response of an asymetric compensation
 * filter.
 * It was revised from the aim2007b AsymCmpFrspV2.m matlab script.
 */

void
ASympCmpFreqResp_GCFilters(double *asymFunc, double frs, double fs, double b,
  double c, BandwidthModePtr bMode)
{
	int		i;
	double	*p, f;
	double	deltaFreq = fs / GCFILTERS_NUM_FRQ_RSL / 2.0, be;

	be = b * BandwidthFromF_Bandwith(bMode, frs);
	for (i = 0, p = asymFunc, f = 0.0; i < GCFILTERS_NUM_FRQ_RSL; i++, f +=
	  deltaFreq)
		*p++ = exp(c * atan2(f - frs, be));
}
