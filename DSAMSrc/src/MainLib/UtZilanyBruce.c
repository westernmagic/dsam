/**********************
 *
 * File:		UtZilanyBruce.c
 * Purpose:		This is the Zilany and Bruce BM filter utilities module.
 *				It was revised from code provided by I. C. Bruce
 * 				(zbcatmodel2007v2.tar.gz).
 *				Zilany, M. S. A. and Bruce, I. C. (2006). "Modeling auditory-
 * 				nerve responses for high sound pressure levels in the normal
 * 				and impaired auditory periphery," Journal of the Acoustical
 * 				Society of America 120(3):1446-1466.
 * 				Zilany, M. S. A. and Bruce, I. C. (2007). "Representation of
 * 				the vowel /eh/ in normal and impaired auditory nerve fibers:
 * 				Model predictions of responses in cats," Journal of the
 * 				Acoustical Society of America 122(1):402-417.
  * Author:		Revised by L. P. O'Mard
 * Comments:	This code was revised from the ARLO matlab code.
 * Author:		Revised by L. P. O'Mard
 * Created:		05 Mar 2008
 * Updated:	
 * Copyright:	(c) 2008, CNBH, University of Essex.
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
#include "UtZilanyBruce.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Subroutines and functions *********************/
/******************************************************************************/

/****************************** Get_tauwb *************************************/

/*
 * Get (set) TauMax, TauMin for the tuning filter. The TauMax is determined by the
 * bandwidth/Q10 of the tuning filter at low level. The TauMin is determined by
 * the gain change between high and low level
 */

double
Get_tauwb_Utility_ZilanyBruce(double cf, double CAgain, int order,
  double *taumax, double *taumin)
{

  double Q10,bw,ratio;
    
  ratio = pow(10, (-CAgain / (20.0 * order))); /* ratio of TauMin/TauMax according to the gain, order */
  
  /* Q10 = pow(10,0.4708*log10(cf/1e3)+0.5469); */ /* 75th percentile */
    Q10 = pow(10,0.4708*log10(cf/1e3)+0.4664); /* 50th percentile */
  /* Q10 = pow(10,0.4708*log10(cf/1e3)+0.3934); */ /* 25th percentile */
  
  bw     = cf / Q10;
  *taumax = 2.0 / (TWOPI * bw);   

  *taumin   = *taumax * ratio;
  return 0;
}

/****************************** Get_taubm ***************************************/

double
Get_taubm_Utility_ZilanyBruce(double cf, double CAgain, double taumax,
  double *bmTaumax, double *bmTaumin, double *ratio)
{
  double factor,bwfactor;
    
  bwfactor = 0.7;
  factor   = 2.5;

  *ratio  = pow(10,(-CAgain / (20.0 * factor))); 

  *bmTaumax = taumax/bwfactor;
  *bmTaumin = *bmTaumax * *ratio;     
  return 0;
}

/****************************** Set_tau *****************************************/

/*
 * This routine sets the tau parameter.
 * This is an addition to the original code formalism.
 * It assumes that the "ZBWBGTCoeffsPtr" argument is correctly initialised.
 */

void
Set_tau_Utility_ZilanyBruce(ZBWBGTCoeffsPtr p, double tau)
{
    p->tau = p->TauWBMax + (tau - p->bmTaumax) * (p->TauWBMax -
      p->TauWBMin) / (p->bmTaumax - p->bmTaumin);
	
}

/****************************** gain_groupdelay *********************************/

/** Calculate the gain and group delay for the Control path Filter */

double
gain_groupdelay_Utility_ZilanyBruce(double binwidth,double centerfreq, double cf,
  double tau, int *grdelay)
{ 
  double tmpcos,dtmp2,c1LP,c2LP,tmp1,tmp2,wb_gain;

  tmpcos = cos(TWOPI*(centerfreq-cf)*binwidth);
  dtmp2 = tau*2.0/binwidth;
  c1LP = (dtmp2-1)/(dtmp2+1);
  c2LP = 1.0/(dtmp2+1);
  tmp1 = 1+c1LP*c1LP-2*c1LP*tmpcos;
  tmp2 = 2*c2LP*c2LP*(1+tmpcos);
  
  wb_gain = pow(tmp1/tmp2, 1.0/2.0);
  
  *grdelay = (int)floor((0.5-(c1LP*c1LP-c1LP*tmpcos)/(1+c1LP*c1LP-2*c1LP*tmpcos)));

  return(wb_gain);
  
}

/****************************** FreeeZBWBGTCoeffs *******************************/

/*
 * This function frees the memory for  the coefficients of the Zilany Bruce
 * Gammatone code.
 */

void
FreeZBWBGTCoeffs_Utility_ZilanyBruce(ZBWBGTCoeffsPtr *p)
{
	if (!*p)
		return;
	if ((*p)->fl)
		free((*p)->fl);
	if ((*p)->f)
		free((*p)->f);
	*p = NULL;

}

/****************************** ResetZBWBGTCoeffs ********************************/

/*
 * This function resets the coefficients for the Zilany Bruce Gammatone code to
 * starting values.
 * It expects the order to have a valid, non-zero value.
 */

void
ResetZBWBGTCoeffs_Utility_ZilanyBruce(ZBWBGTCoeffsPtr p)
{
	int		i;
	
	for (i = 0; i < p->order + 1; i++) {
		SET_CMPLXM(p->fl[i], 0.0, 0.0); 
		SET_CMPLXM(p->f[i], 0.0, 0.0); 
	}
	p->phase = 0;
}

/****************************** InitZBWBGTCoeffs ********************************/

/*
 * This function initialises the coefficients for the Zilany Bruce Gammatone code.
 * It expects the order to have a valid, non-zero value.
 */

ZBWBGTCoeffsPtr
InitZBWBGTCoeffs_Utility_ZilanyBruce(int order, double dt, double cF,
  double CAgain, double cohc)
{
	static const WChar	*funcName = wxT("InitZBWBGTCoeffs_Utility_ZilanyBruce");
	ZBWBGTCoeffsPtr	p;
	int		i, numComplex;
	double	Taumax, Taumin, bmTaubm, bmplace;

	if ((p = (ZBWBGTCoeffsPtr) malloc(sizeof(ZBWBGTCoeffs))) == NULL) {
		NotifyError(wxT("%s: Out of memory for ZBWBGTCoeffs structure"), funcName);
		return(NULL);
	}
	p->fl = NULL;
	p->f = NULL;
	numComplex = order + 1;
	if ((p->fl = (Complex *) calloc(numComplex, sizeof(Complex))) == NULL) {
		NotifyError(wxT("%s: Out of memory for fl array (%d elements)"),
				funcName, order);
		FreeZBWBGTCoeffs_Utility_ZilanyBruce(&p);
		return(NULL);
	}
	if ((p->f = (Complex *) calloc(numComplex, sizeof(Complex))) == NULL) {
		NotifyError(wxT("%s: Out of memory for f array (%d elements)"),
				funcName, order);
		FreeZBWBGTCoeffs_Utility_ZilanyBruce(&p);
		return(NULL);
	}
	p->order = order;
	
	/** Calculate the location on basilar membrane from CF */	
	bmplace = 11.9 * log10(0.80 + cF / 456.0); 
    
	/** Calculate the center frequency for the control-path wideband filter
	    from the location on basilar membrane */
	p->cF = 456.0 * (pow(10.0, (bmplace + 1.2) / 11.9) - 0.80); /* shift the center freq */

	p->delta_phase = -TWOPI * p->cF * dt;

	Get_tauwb_Utility_ZilanyBruce(cF, CAgain, p->order, &Taumax, &Taumin);
	Get_taubm_Utility_ZilanyBruce(cF, CAgain, Taumax, &p->bmTaumax, &p->bmTaumin,
	  &p->ratiobm);
	bmTaubm  = cohc * (p->bmTaumax - p->bmTaumin) + p->bmTaumin;
	p->TauWBMax = Taumin + 0.2 * (Taumax - Taumin);
	p->TauWBMin = p->TauWBMax / Taumax * Taumin;
    Set_tau_Utility_ZilanyBruce(p, bmTaubm);
    p->gain = gain_groupdelay_Utility_ZilanyBruce(dt, p->cF, cF, p->tau,
    	  &p->grdelay);
    p->lasttmpgain = p->gain;
    ResetZBWBGTCoeffs_Utility_ZilanyBruce(p);
	return(p);

}

/****************************** WbGammaTone ***********************************/

/*
 * Pass the signal through the Control path Third Order Nonlinear Gammatone Filter.
 * This code now uses the complex math code used in DSAM.  This passes complex numbers
 * by pointer rather then copying structures too and fro.
 */

double
WbGammaTone_Utility_ZilanyBruce(double x, ZBWBGTCoeffsPtr p, double dt)
{
  double	dtmp, c1LP, c2LP;
  Complex	zwbphase, zSum, zC1Scaled, zC2Scaled, zProd;
  
  int i,j;
  
  p->phase += p->delta_phase;
  
  dtmp = p->tau * 2.0 / dt;
  c1LP = (dtmp - 1) / (dtmp + 1);
  c2LP = 1.0 / (dtmp + 1);

  EXP_CMPLXM(zwbphase, p->phase);
  SCALER_MULT_CMPLXM(p->f[0], zwbphase, x);                 /* FREQUENCY SHIFT */
  
  for (j = 1; j <= p->order; j++) {                           /* IIR Bilinear transformation LPF */
	  ADD_CMPLXM(zSum, p->f[j - 1], p->fl[j - 1]);
	  SCALER_MULT_CMPLXM(zC2Scaled, zSum, c2LP * p->gain);
	  SCALER_MULT_CMPLXM(zC1Scaled, p->fl[j], c1LP);
	  ADD_CMPLXM(p->f[j], zC2Scaled, zC1Scaled);
  }
  EXP_CMPLXM(zwbphase, -p->phase);
  MULT_CMPLXM(zProd, zwbphase, p->f[p->order]); /* FREQ SHIFT BACK UP */
  
  for(i = 0; i <= p->order; i++)
	  p->fl[i] = p->f[i];
  return(zProd.re);

}

/****************************** Boltzman **************************************/

/*
 * Get the output of the OHC Nonlinear Function (Boltzman Function).
 */

double
Boltzman_Utility_ZilanyBruce(double x, double asym, double s0, double s1, double x1)
  {
	double shift,x0,out1,out;

    shift = 1.0/(1.0+asym);  /* asym is the ratio of positive Max to negative Max*/
    x0    = s0*log((1.0/shift-1)/(1+exp(x1/s1)));
	    
    out1 = 1.0/(1.0+exp(-(x-x0)/s0)*(1.0+exp(-(x-x1)/s1)))-shift;
	out = out1/(1-shift);

    return(out);
  }  /* output of the nonlinear function, the output is normalized with maximum value of 1 */


/****************************** NLafterohc ************************************/

/* 
 * Get the output of the Control path using Nonlinear Function after OHC
 */

double
NLafterohc_Utility_ZilanyBruce(double x,double taumin, double taumax, double asym)
{    
	double R,dc,R1,s0,x1,out,minR;

	minR = 0.05;
    R  = taumin/taumax;
    
	if (R<minR)
		minR = 0.5 * R;
    else
    	minR = minR;
    
    dc = (asym-1)/(asym+1.0)/2.0-minR;
    R1 = R-minR;

    /* This is for new nonlinearity */
    s0 = -dc/log(R1/(1-minR));
	
    x1  = fabs(x);
    out = taumax*(minR+(1.0-minR)*exp(-x1/s0));
	if (out<taumin) out = taumin;
	if (out>taumax) out = taumax;
    return(out);
}

/****************************** ResetZBGCCoeffs *******************************/

/*
 * This routine resets the Gamma chirp filter's coefficients.
 * It assumes that the 'ZBGCCoeffs' structure has been correctly initialised.
 * As this structure uses fixed array sizes (at present) the structure pointer
 * can be set without extra memory allocation code.
 */

void
ResetZBGCCoeffs_Utility_ZilanyBruce(ZBGCCoeffsPtr p, double binwidth, double cf,
  double taumax, ZBGCTypeSpecifier type)
{
    double  rzero, norm_gain, phase;
	int		i,r,order_of_pole;
	Complex	*z;

	p->type = type;
	/* Defining initial locations of the poles and zeros */
	/*======== setup the locations of poles and zeros =======*/
	  p->sigma0 = 1.0 / taumax;
	  p->ipw    = 1.01*cf*TWOPI-50;
	  p->ipb    = 0.2343*TWOPI*cf-1104;
	  p->rpa    = pow(10, log10(cf)*0.9 + 0.55)+ 2000;
	  p->pzero  = pow(10,log10(cf)*0.7+1.6)+500;

	/*===============================================================*/     
         
     order_of_pole    = 10;             
     p->half_order_pole  = order_of_pole / 2;

	 p->fs_bilinear = TWOPI*cf/tan(TWOPI*cf*binwidth/2);
    rzero       = -p->pzero;
	p->CF          = TWOPI*cf;
   
	p->p[1].re = -p->sigma0;     
    p->p[1].im = p->ipw;
	p->p[5].re = p->p[1].re - p->rpa;
	p->p[5].im = p->p[1].im - p->ipb;
    p->p[3].re = (p->p[1].re + p->p[5].re) * 0.5;
    p->p[3].im = (p->p[1].im + p->p[5].im) * 0.5;
    COPY_COMPCONJ_CMPLX(p->p[2], p->p[1]);
    COPY_COMPCONJ_CMPLX(p->p[4], p->p[3]);
    COPY_COMPCONJ_CMPLX(p->p[6], p->p[5]);
    p->p[7]   = p->p[1];
    p->p[8] = p->p[2];
    p->p[9] = p->p[5];
    p->p[10]= p->p[6];

    p->initphase = 0.0;
	for (i = 1, z = p->p + 1; i <= p->half_order_pole; i++, z += 2)
		p->initphase += atan(p->CF / (-rzero)) - atan((p->CF - z->im) /
		  (-z->re)) - atan((p->CF + z->im) / (-z->re));

	/*===================== Initialize p->input & p->output =====================*/

	for (i=1;i<=(p->half_order_pole+1);i++) {
		p->input[i][3] = 0; 
		p->input[i][2] = 0; 
		p->input[i][1] = 0;
		p->output[i][3] = 0; 
		p->output[i][2] = 0; 
		p->output[i][1] = 0;
	}

	/*===================== normalize the gain =====================*/
    
      p->gain_norm = 1.0;
      for (r=1; r<=order_of_pole; r++)
		   p->gain_norm *= (pow((p->CF - p->p[r].im),2) + p->p[r].re*p->p[r].re);
	
}

/****************************** ChirpFilt *************************************/

/*
 * Pass the signal through the signal-path C1 Tenth Order Nonlinear
 * Chirp-Gammatone Filter
 */

double
ChirpFilt_Utility_ZilanyBruce(double x, ZBGCCoeffsPtr p, double rsigma_fcohc)
{
    static const WChar *funcName = wxT("ChirpFilt_Utility_ZilanyBruce");
    int		i, order_of_zero, index;
    double	pimg, preal, phase, rzero, norm_gain;
    double	filterout, dy;
    Complex	*z;

    order_of_zero = p->half_order_pole;
    rzero = -p->pzero;
    norm_gain = sqrt(p->gain_norm) / pow(sqrt(p->CF * p->CF + rzero * rzero),
      order_of_zero);
	p->p[1].re = (p->type == ZB_GC_C1)? -p->sigma0 - rsigma_fcohc: -p->sigma0 *
	  rsigma_fcohc;
	if (p->p[1].re>0.0) {
		NotifyError(wxT("%s: The system becomes unstable.\n"), funcName);
		exit(1);
	}
	p->p[1].im = p->ipw;
	p->p[5].re = p->p[1].re - p->rpa;
	p->p[5].im = p->p[1].im - p->ipb;
    p->p[3].re = (p->p[1].re + p->p[5].re) * 0.5;
    p->p[3].im = (p->p[1].im + p->p[5].im) * 0.5;
    COPY_COMPCONJ_CMPLX(p->p[2], p->p[1]);
    COPY_COMPCONJ_CMPLX(p->p[4], p->p[3]);
    COPY_COMPCONJ_CMPLX(p->p[6], p->p[5]);
    p->p[7] = p->p[1];
    p->p[8] = p->p[2];
    p->p[9] = p->p[5];
    p->p[10]= p->p[6];
    
    phase = 0.0;
	for (i = 1, z = p->p + 1; i <= p->half_order_pole; i++, z += 2)
		phase += -atan((p->CF - z->im) / (-z->re)) - atan((p->CF + z->im) / -z->re);

	rzero = -p->CF / tan((p->initphase - phase) / order_of_zero);

    if (rzero>0.0) {
    	NotifyError(wxT("%s: The zeros are in the right-half plane.\n"), funcName);
    	exit(1);
    }
	 
   /*%==================================================  */
	/*each loop below is for a pair of poles and one zero */
   /*%      time loop begins here                         */
   /*%==================================================  */
 
       p->input[1][3]=p->input[1][2]; 
	   p->input[1][2]=p->input[1][1]; 
	   p->input[1][1]= x;

	for (i = 1, z = p->p + 1; i<= p->half_order_pole; i++, z += 2){
		dy = p->input[i][1] * (p->fs_bilinear - rzero) - 2 * rzero *
		  p->input[i][2] - (p->fs_bilinear + rzero) * p->input[i][3] +
		  2 * p->output[i][1] * (p->fs_bilinear * p->fs_bilinear -
		  z->re * z->re - z->im * z->im) - p->output[i][2] * ((p->fs_bilinear +
		  z->re) * (p->fs_bilinear + z->re) + z->im * z->im);
		dy /= pow((p->fs_bilinear - z->re), 2) + pow(z->im, 2);
	
		p->input[i+1][3] = p->output[i][2];
		p->input[i+1][2] = p->output[i][1];
		p->input[i+1][1] = dy;
	
		p->output[i][2] = p->output[i][1];
		p->output[i][1] = dy;
	}

	dy = p->output[p->half_order_pole][1] * norm_gain;  /* don't forget the gain term */
	filterout = dy / 4.0;   /* signal path output is divided by 4 to give correct C1 filter gain */
	return (filterout);
	
}  

/****************************** NLogarithm ************************************/

double
NLogarithm_Utility_ZilanyBruce(double x, double slope, double asym)
{
	double corner,strength,xx,splx,asym_t;

    corner    = 80;
    strength  = 20.0e6/pow(10,corner/20); 
            
    xx = log(1.0+strength*fabs(x))*slope;
    if(x<0)
	{
		splx   = 20*log10(-x/20e-6);
		asym_t = asym-(asym-1)/(1+exp(splx/5.0));
		xx = -1/asym_t*xx;
	};
    
    return(xx);

}

/****************************** delay_cat *************************************/

/*
 * Calculate the delay (basilar membrane, synapse, etc. for cat)
 */

double
delay_cat_Utility_ZilanyBruce(double cf)
{
  /* DELAY THE WAVEFORM (delay buf1, tauf, ihc for display purposes)  */
  /* Note: Latency vs. CF for click responses is available for Cat only (not human) */
  /* Use original fit for Tl (latency vs. CF in msec) from Carney & Yin '88
     and then correct by .75 cycles to go from PEAK delay to ONSET delay */
	/* from Carney and Yin '88 */
  double A0,A1,x,delay;

  A0    = 3.0;  
  A1    = 12.5;
  x     = 11.9 * log10(0.80 + cf / 456.0);      /* cat mapping */
  delay = A0 * exp( -x/A1 ) * 1e-3;
  
  return(delay);
}
