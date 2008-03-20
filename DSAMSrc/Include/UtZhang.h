/**********************
 *
 * File:		UtZhang.h
 * Purpose:		Incorporates the Zhang et al. AN code utilities
 * Comments:	This code was revised from the ARLO matlab code.
 * Author:		Edited by L. P. O'Mard
 * Created:		13 Jun 2002
 * Updated:	
 * Copyright:	(c) 2002, CNBH, University of Essex.
 *
 *********************/

#ifndef _UTZHANG_H
#define _UTZHANG_H 1

#include "UtNameSpecs.h"

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define Broad_ALL		0x80
#define Linear_ALL		0x40
#define NonLinear_ALL		0x20

#define FeedBack_NL		(NonLinear_ALL|0x01)
#define FeedForward_NL		(NonLinear_ALL|0x02)
#define Broad_Linear		(Broad_ALL|Linear_ALL|0x01)
#define Sharp_Linear		(Linear_ALL|0x02)
#define Broad_Linear_High	(Broad_ALL|Linear_ALL|0x03)

#define MAX_ORDER 10

/******************************************************************************/
/****************************** Macro definitions *****************************/
/******************************************************************************/

#define UT_ZHANG_GAIN(MPA)	((MPA)? 1e-6: 1.0)

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef enum {

	UT_ZHANG_SPECIES_HUMAN,
	UT_ZHANG_SPECIES_CAT,
	UT_ZHANG_SPECIES_UNIVERSAL,
	UT_ZHANG_SPECIES_NULL

} BasilarMSpeciesSpecifier;

typedef struct TLowPass {
	/*//input one signal to the filter and get the output */
	double (*Run)(struct TLowPass *p, double x);
	/*//input multiple signal to the filter and get the multiple output */
	void (*Run2)(struct TLowPass *p,const double *in, double *out,
	  const int length);

	/*/time-domain resolution,cut-off frequency,gain, filter order */
	double tdres, Fc, gain;
	int Order;
	/*/ parameters used in calculation */
	double c1LP,c2LP,hc[MAX_ORDER],hcl[MAX_ORDER];
} TLowPass, *TLowPassPtr;

typedef struct TNonLinear {
	double	(* Run)(struct TNonLinear *p, double x);
	void	(* Run2)(struct TNonLinear *p, const double *in, double *out,
			  const int length);
	/*/For OHC Boltzman */
	double	p_corner,p_slope,p_strength,x0,s0,x1,s1,shift;
	double	Acp,Bcp,Ccp;
	/*/For AfterOHCNL */
	double	dc,minR,A,TauMax,TauMin; /*/s0 also used by AfterOHCNL */
	/*/For IHC nonlinear function */
	double	A0,B,C,D;
	/*/For IHCPPI nonlinear */
	double	psl,pst,p1,p2;

} TNonLinear, *TNonLinearPtr;

typedef struct THairCell {
	double	(* Run)(struct THairCell *p,double x);
	void	(* Run2)(struct THairCell* p,const double *in, double *out,
			  const int length);
	TLowPass hclp;
	TNonLinear hcnl;
	/*/Boltzman Like NonLinear */
	/* void (*setOHC_NL)(THairCell*,double,double,double,double,double,double,double,double); */
	/* void (*setIHC_NL)(); */
} THairCell, *THairCellPtr;

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

BOOLN	init_boltzman(TNonLinear* p,double _corner, double _slope,
		  double _strength, double _x0, double _s0, double _x1, double _s1,
		  double _asym);

void	InitLowPass_Utility_Zhang(TLowPass* res, double _tdres, double _Fc,
		  double _gain,int _LPorder);

NameSpecifier *	SpeciesList_Utility_Zhang(int index);

double	RunAfterOhcNL_Utility_Zhang(TNonLinear* p, double x);

void	RunAfterOhcNL2_Utility_Zhang(TNonLinear* p, const double *in,
		  double *out, const int length);

double	RunBoltzman(TNonLinear *p, double x);

double	RunHairCell(THairCell *p, double x);

double	RunLowPass_Utility_Zhang(TLowPass *p, double x);

void	RunLowPass2_Utility_Zhang(TLowPass *p,const double *in,
		  double *out, const int length);

__END_DECLS

#endif
