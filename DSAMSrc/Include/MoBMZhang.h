/**********************
 *
 * File:		MoBMZhang.h
 * Purpose:		Incorporates the Zhang et al. BM code
 * Comments:	Written using ModuleProducer version 1.4.0 (Mar 14 2002).
 *				This code was revised from the ARLO matlab code.
 * Author:		Revised by L. P. O'Mard
 * Created:		13 Jun 2002
 * Updated:	
 * Copyright:	(c) 2002, CNBH, University of Essex.
 *
 *********************/

#ifndef _MOBMZHANG_H
#define _MOBMZHANG_H 1

#include "UtNameSpecs.h"

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define BASILARM_ZHANG_NUM_PARS			17
#define BASILARM_ZHANG_ABS_DB			20.0

/******************************************************************************/
/****************************** Macro definitions *****************************/
/******************************************************************************/

#define BASILARM_ZHANG_GAIN(MPA)	((MPA)? 1e-6: 1.0)

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef enum {

	BASILARM_ZHANG_MODEL,
	BASILARM_ZHANG_SPECIES,
	BASILARM_ZHANG_MICROPAINPUT,
	BASILARM_ZHANG_NBORDER,
	BASILARM_ZHANG_WBORDER,
	BASILARM_ZHANG_CORNERCP,
	BASILARM_ZHANG_SLOPECP,
	BASILARM_ZHANG_STRENGHCP,
	BASILARM_ZHANG_X0CP,
	BASILARM_ZHANG_S0CP,
	BASILARM_ZHANG_X1CP,
	BASILARM_ZHANG_S1CP,
	BASILARM_ZHANG_SHIFTCP,
	BASILARM_ZHANG_CUTCP,
	BASILARM_ZHANG_KCP,
	BASILARM_ZHANG_R0,
	BASILARM_ZHANG_CFLIST

} BMZhangParSpecifier;

typedef enum {

	BASILARM_ZHANG_MODEL_FEED_FORWARD_NL,
	BASILARM_ZHANG_MODEL_FEED_BACK_NL,
	BASILARM_ZHANG_MODEL_SHARP_LINEAR,
	BASILARM_ZHANG_MODEL_BROAD_LINEAR,
	BASILARM_ZHANG_MODEL_BROAD_LINEAR_HIGH,
	BASILARM_ZHANG_MODEL_NULL

} BasilarMModelSpecifier;

typedef struct TGammaTone {
	/*//input one signal to the filter and get the output */
	double (*Run)(struct TGammaTone *p, double x);
	/*//input multiple signal to the filter and get the multiple output */
	void (*Run2)(struct TGammaTone *p,const double *in, double *out,
	  const int length);

	double phase;
	/* Cutoff Freq(tau), Shift Freq, ... */
	double tdres,tau;
	double F_shift,delta_phase;
	double gain,c1LP,c2LP;
	Complex gtf[MAX_ORDER],gtfl[MAX_ORDER];
	int Order;

	/*// Set the tau of the gammatone filter, this is useful for time-varying
	 * filter */
	void (*SetTau)(struct TGammaTone *p, double _tau);

} TGammaTone, *TGammaTonePtr;

typedef struct { /* class of basilar membrane */

  /* double (*run)(TBasilarMembrane *p, double x); */
  /* void (*run2)(TBasilarMembrane *p, const double *in, double *out,
   * const int length); */

  int bmmodel; /* determine if the bm is broad_linear, sharp_linear or other */
  double tdres;
  int bmorder,wborder;

  double tau,TauMax,TauMin;
  double TauWB,TauWBMin;
  double A,B; 
  /* --------Model -------------- */
  TGammaTone bmfilter; /*/NonLinear Filter */
  TGammaTone gfagain; /*/Linear Filter */
  TGammaTone wbfilter; /*/Control Path filter */
  THairCell ohc;
  TNonLinear afterohc;

} TBasilarMembrane, *TBasilarMembranePtr;

typedef struct {

	ParameterSpecifier	parSpec;

	BOOLN	modelFlag, speciesFlag, microPaInputFlag, nbOrderFlag, wbOrderFlag;
	BOOLN	cornerCPFlag, slopeCPFlag, strenghCPFlag, x0CPFlag, s0CPFlag;
	BOOLN	x1CPFlag, s1CPFlag, shiftCPFlag, cutCPFlag, kCPFlag, r0Flag;
	BOOLN	updateProcessVariablesFlag;
	int		model;
	int		species;
	int		microPaInput;
	int		nbOrder;
	int		wbOrder;
	double	cornerCP;
	double	slopeCP;
	double	strenghCP;
	double	x0CP;
	double	s0CP;
	double	x1CP;
	double	s1CP;
	double	shiftCP;
	double	cutCP;
	int		kCP;
	double	r0;
	CFListPtr	cFList;

	/* Private members */
	NameSpecifier	*modelList;
	UniParListPtr	parList;
	int		numChannels;
	TBasilarMembrane *bM;

} BMZhang, *BMZhangPtr;

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

extern	BMZhangPtr	bMZhangPtr;

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
* macros, to allow the safe use of C libraries with C++ libraries - defined
* in GeCommon.h.
*/
__BEGIN_DECLS

BOOLN	CheckData_BasilarM_Zhang(EarObjectPtr data);

BOOLN	CheckPars_BasilarM_Zhang(void);

BOOLN	FreeProcessVariables_BasilarM_Zhang(void);

BOOLN	Free_BasilarM_Zhang(void);

CFListPtr	GetCFListPtr_BasilarM_Zhang(void);

UniParListPtr	GetUniParListPtr_BasilarM_Zhang(void);

BOOLN	InitBasilarMembrane_BasilarM_Zhang(TBasilarMembranePtr bm,
		  int model, int species, double tdres, double cf);

BOOLN	InitModelList_BasilarM_Zhang(void);

BOOLN	InitModule_BasilarM_Zhang(ModulePtr theModule);

BOOLN	InitProcessVariables_BasilarM_Zhang(EarObjectPtr data);

BOOLN	Init_BasilarM_Zhang(ParameterSpecifier parSpec);

BOOLN	PrintPars_BasilarM_Zhang(void);

void	Run2BasilarMembrane_BasilarM_Zhang(TBasilarMembrane *bm,
		  const double *in, double *out, const int length);

BOOLN	RunModel_BasilarM_Zhang(EarObjectPtr data);

BOOLN	SetCFList_BasilarM_Zhang(CFListPtr theCFList);

BOOLN	SetCornerCP_BasilarM_Zhang(double theCornerCP);

BOOLN	SetCutCP_BasilarM_Zhang(double theCutCP);

BOOLN	SetKCP_BasilarM_Zhang(int theKCP);

BOOLN	SetMicroPaInput_BasilarM_Zhang(WChar * theMicroPaInput);

BOOLN	SetModel_BasilarM_Zhang(WChar * theModel);

BOOLN	SetNbOrder_BasilarM_Zhang(int theNbOrder);

BOOLN	SetParsPointer_BasilarM_Zhang(ModulePtr theModule);

BOOLN	SetR0_BasilarM_Zhang(double theR0);

BOOLN	SetS0CP_BasilarM_Zhang(double theS0CP);

BOOLN	SetS1CP_BasilarM_Zhang(double theS1CP);

BOOLN	SetShiftCP_BasilarM_Zhang(double theShiftCP);

BOOLN	SetSlopeCP_BasilarM_Zhang(double theSlopeCP);

BOOLN	SetSpecies_BasilarM_Zhang(WChar * theSpecies);

BOOLN	SetStrenghCP_BasilarM_Zhang(double theStrenghCP);

BOOLN	SetUniParList_BasilarM_Zhang(void);

BOOLN	SetWbOrder_BasilarM_Zhang(int theWbOrder);

BOOLN	SetX0CP_BasilarM_Zhang(double theX0CP);

BOOLN	SetX1CP_BasilarM_Zhang(double theX1CP);

__END_DECLS

#endif
