/**********************
 *
 * File:		UtZhang.c
 * Purpose:		Incorporates the Zhang et al. AN code utilities
 * Comments:	This code was revised from the ARLO matlab code.
 * Author:		Edited by L. P. O'Mard
 * Created:		13 Jun 2002
 * Updated:	
 * Copyright:	(c) 2002, CNBH, University of Essex.
 *
 *********************/

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "GeCommon.h"

#include "UtCmplxM.h"
#include "UtZhang.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Subroutines and functions *********************/
/******************************************************************************/

/****************************** InitSpeciesList *******************************/

/*
 * This function initialises the 'species' list array
 */

NameSpecifier *
SpeciesList_Utility_Zhang(int index)
{
	static NameSpecifier	modeList[] = {

			{ "HUMAN",		UT_ZHANG_SPECIES_HUMAN },
			{ "CAT",		UT_ZHANG_SPECIES_CAT },
			{ "UNIVERSAL",	UT_ZHANG_SPECIES_UNIVERSAL },
			{ "",			UT_ZHANG_SPECIES_NULL },
		};
	return (&modeList[index]);

}

/****************************** init_boltzman *********************************/

/*
 * Extracted from the cmpa.c code (LPO).
 * asym is the ratio of positive Max to negative Max.
 */

BOOLN
init_boltzman(TNonLinear* p,double _corner, double _slope, double _strength,
  double _x0, double _s0, double _x1, double _s1, double _asym)
{

	p->p_corner = _corner;
	p->p_slope = _slope;
	p->p_strength = _strength;
	p->x0 = _x0;
	p->s0 = _s0;
	p->x1 = _x1;
	p->s1 = _s1;
	if (_asym<0)
		p->shift = 1.0/(1.0+exp(_x0/_s0)*(1+exp(_x1/_s1)));
	else {
		p->shift = 1.0/(1.0+_asym);
		p->x0 = _s0*log((1.0/p->shift-1)/(1+exp(_x1/_s1)));
		p->Bcp = p->p_slope/p->p_strength;
		p->Acp = exp((-p->p_corner-20*log10(20e-6))*p->p_strength);
		p->Ccp = 20*p->p_strength/log(10);
	}
	p->Run = RunBoltzman;
	p->Run2 = NULL;
	return(TRUE);

}

/****************************** RunBoltzman ***********************************/

/*
 * Extracted from the hc.c code (LPO).
 */

double
RunBoltzman(TNonLinear *p, double x)
{
	/*// get the output of the first nonlinear function */
	double xx,out;
	xx = fabs(x);
	if (x>0)
		xx = p->Bcp*log(1+p->Acp*pow(xx,p->Ccp));
	else if (x<0)
		xx = -p->Bcp*log(1+p->Acp*pow(xx,p->Ccp));
	else
		xx = 0;

	/*// get the output of the second nonlinear function(Boltzman Function) */
	out = 1.0 / (1.0 + exp(-(xx - p->x0) / p->s0) * (1.0 + exp(-(xx - p->x1) /
	  p->s1))) - p->shift;
	return(out / (1.0 - p->shift));

}

/****************************** RunHairCell ***********************************/

/*
 * Original name: initLowPass
 * Extracted from the hc.c code (LPO).
 */

double
RunHairCell(THairCell *p, double x)
{
	double y;

	y = p->hcnl.Run(&(p->hcnl), x);
	return(p->hclp.Run(&(p->hclp), y));

}

/****************************** RunAfterOhcNL *********************************/

/*
 * Original name: runAfterOhcNL
 * Extracted from the hc.c code (LPO).
 * output of the nonlinearity
 * out = TauMax*(minR+(1.0-minR)*exp(-x1/s0));\\
 * if the input is zero, the output is TauMax,\\
 * if the input is dc, the output is TauMin,\\
 * if the input is too large, the output is pushed to TauMax*minR
 * if the input is negative, the output is the 2*TauMax-out (keep 
 * the control signal continuous)
 */

double
RunAfterOhcNL_Utility_Zhang(TNonLinear* p, double x)
{
	double out;
	double x1 = fabs(x);

	out = p->TauMax * (p->minR + (1.0 - p->minR) * exp(-x1 / p->s0));
	return(out);

};

/****************************** RunAfterOhcNL2 ********************************/

/*
 * Original name: runAfterOhcNL
 * Extracted from the hc.c code (LPO).
 */

void
RunAfterOhcNL2_Utility_Zhang(TNonLinear* p, const double *in, double *out,
  const int length)
{
    int register i;
    double x1;

    for (i=0;i<length;i++){
	x1 = fabs(in[i]);
    	out[i] = p->TauMax*(p->minR+(1.0-p->minR)*exp(-x1/p->s0));
    };

}

/****************************** InitLowPass ***********************************/

/*
 * Original name: initLowPass
 * Original file: filters.c
 */

void
InitLowPass_Utility_Zhang(TLowPass* res, double _tdres, double _Fc,
  double _gain,int _LPorder)
{
	double c;
	int i;
	res->tdres = _tdres;
	c = 2.0/_tdres;
	res->Fc = _Fc;
	res->Order = _LPorder;
	res->c1LP = ( c - TWOPI*_Fc ) / ( c + TWOPI*_Fc );
	res->c2LP = TWOPI*_Fc / (TWOPI*_Fc + c);
	for(i=0; i<=res->Order; i++) res->hc[i] = res->hcl[i] = 0.0;
	res->gain = _gain;

	res->Run = RunLowPass_Utility_Zhang;
	res->Run2 = RunLowPass2_Utility_Zhang;

}

/****************************** RunLowPass ************************************/

/*
 * Original name: runLowPass2
 * Original file: filters.c
 */

/**
  This function runs the low-pass filter
   @author Xuedong Zhang
 */
double
RunLowPass_Utility_Zhang(TLowPass *p, double x)
{
	register int i;
	register int pOrder = p->Order;

	p->hc[0] = x*p->gain;

	for (i=0; i< pOrder; i++)
		p->hc[i+1] = p->c1LP * p->hcl[i+1] + p->c2LP * (p->hc[i] + p->hcl[i]);

	for (i=0; i <= pOrder; i++)
		p->hcl[i] = p->hc[i];

	return(p->hc[pOrder]);

}

/****************************** RunLowPass2 ***********************************/

/*
 * Original name: runLowPass2
 * Original file: filters.c
 */

/**
  This function runs the low-pass filter
   @author Xuedong Zhang
 */

void
RunLowPass2_Utility_Zhang(TLowPass *p,const double *in, double *out,
  const int length)
{
	register int loopSig,loopLP;
	int pOrder = p->Order;
	double *hc,*hcl,c1LP,c2LP;
	double gain;
	gain = p->gain;
	c1LP = p->c1LP;
	c2LP = p->c2LP;
	hc = p->hc;
	hcl = p->hcl;
	for(loopSig = 0; loopSig < length; loopSig++) {
		hc[0] = in[loopSig]*gain;
		for(loopLP = 0; loopLP < pOrder; loopLP++)
			hc[loopLP+1] = c1LP * hcl[loopLP+1] + c2LP*(hc[loopLP]+hcl[loopLP]);

		for(loopLP = 0; loopLP <= pOrder;loopLP++)
			hcl[loopLP] = hc[loopLP];

		out[loopSig] = hc[pOrder];
	}
}
