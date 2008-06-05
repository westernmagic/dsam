/**********************
 *
 * File:		UtZilanyBruce.h
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

#ifndef _UTZILANYBRUCE_H
#define _UTZILANYBRUCE_H 1

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef enum {

	ZB_GC_C1,
	ZB_GC_C2

} ZBGCTypeSpecifier;

/*
 * Both the tau and gain fields are variable, dependent upon the input.
 */

typedef struct {

	int		order;
	int		grdelay;
	double	phase;
	double	delta_phase;
	double	cF;
	double	tau;
	double	gain;
	double	bmTaumin;
	double	bmTaumax;
	double	TauWBMax;
	double	TauWBMin;
	double	ratiobm;
	double	lasttmpgain;
	Complex	*fl;
	Complex	*f;

} ZBWBGTCoeffs, *ZBWBGTCoeffsPtr;

typedef struct {

	ZBGCTypeSpecifier		type;
	int		half_order_pole;
	double	ipw;
	double	pzero;
	double	CF;
	double	sigma0;
	double	ipb;
	double	rpa;
	double	fs_bilinear;
	double	gain_norm;
	double	initphase;
	double	input[12][4];
	double	output[12][4];
	Complex	p[11];

} ZBGCCoeffs, *ZBGCCoeffsPtr;

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

double	Boltzman_Utility_ZilanyBruce(double x, double asym, double s0, double s1,
		  double x1);

double	ChirpFilt_Utility_ZilanyBruce(double x, ZBGCCoeffsPtr p, double rsigma_fcohc);

double	delay_cat_Utility_ZilanyBruce(double cf);

void	FreeZBWBGTCoeffs_Utility_ZilanyBruce(ZBWBGTCoeffsPtr *p);

double	gain_groupdelay_Utility_ZilanyBruce(double binwidth,double centerfreq,
		  double cf, double tau, int *grdelay);

ZBWBGTCoeffsPtr	InitZBWBGTCoeffs_Utility_ZilanyBruce(int order, double dt,
				  double cF, double CAgain, double cohc);

void	ResetZBWBGTCoeffs_Utility_ZilanyBruce(ZBWBGTCoeffsPtr p);

void	ResetZBGCCoeffs_Utility_ZilanyBruce(ZBGCCoeffsPtr p, double binwidth,
		  double cf, double taumax, ZBGCTypeSpecifier type);

void	Set_tau_Utility_ZilanyBruce(ZBWBGTCoeffsPtr p, double tau);

double	NLafterohc_Utility_ZilanyBruce(double x,double taumin, double taumax,
		  double asym);

double	NLogarithm_Utility_ZilanyBruce(double x, double slope, double asym);

double	WbGammaTone_Utility_ZilanyBruce(double x, ZBWBGTCoeffsPtr p, double dt);

__END_DECLS

#endif
