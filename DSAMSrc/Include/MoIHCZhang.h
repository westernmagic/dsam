/**********************
 *
 * File:		MoIHCZhang.h
 * Purpose:		Incorporates the Zhang et al. 'hppi' and 'syn' IHC synapse code.
 * Comments:	Written using ModuleProducer version 1.4.0 (Mar 14 2002).
 *				This code was revised from the ARLO matlab code.
 * Author:		Revised by L. P. O'Mard
 * Created:		19 Aug 2002
 * Updated:	
 * Copyright:	(c) 2002, CNBH, University of Essex
 *
 *********************/

#ifndef _MOIHCZHANG_H
#define _MOIHCZHANG_H 1

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define IHC_ZHANG_NUM_PARS			10

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef enum {

	IHC_ZHANG_SPONT,
	IHC_ZHANG_AASS,
	IHC_ZHANG_TAUST,
	IHC_ZHANG_TAUR,
	IHC_ZHANG_AAROVERAST,
	IHC_ZHANG_PIMAX,
	IHC_ZHANG_KS,
	IHC_ZHANG_VS1,
	IHC_ZHANG_VS2,
	IHC_ZHANG_VS3

} ZhangHCParSpecifier;

typedef struct TSynapse {

  double 	(*Run)(struct TSynapse *p,double x);
  void		(*Run2)(struct TSynapse *p, const double *in, double *out,
  			  const int length);
  double	cf,tdres;
  double	spont;  
  double	PTS, Ass, Ar_over_Ast, Pimax, tauR, tauST;
  double	Prest,PPIlast,PL,PG,CIrest,CIlast,CLrest,CLlast,CG,VI,VL;
  double	Vsat;

} TSynapse, *TSynapsePtr;

typedef struct {

	ParameterSpecifier	parSpec;

	BOOLN	spontFlag, aAssFlag, tauSTFlag, tauRFlag, aAROverASTFlag, pIMaxFlag;
	BOOLN	ksFlag, vs1Flag, vs2Flag, vs3Flag;
	BOOLN	updateProcessVariablesFlag;
	double	spont;
	double	aAss;
	double	tauST;
	double	tauR;
	double	aAROverAST;
	double	pIMax;
	double	ks;
	double	vs1;
	double	vs2;
	double	vs3;

	/* Private members */
	UniParListPtr	parList;
	TNonLinear	*iHCPPI;
	TSynapse	*synapse;

} ZhangHC, *ZhangHCPtr;

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

extern	ZhangHCPtr	zhangHCPtr;

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
* macros, to allow the safe use of C libraries with C++ libraries - defined
* in GeCommon.h.
*/
__BEGIN_DECLS

BOOLN	CheckData_IHC_Zhang(EarObjectPtr data);

BOOLN	CheckPars_IHC_Zhang(void);

BOOLN	FreeProcessVariables_IHC_Zhang(void);

BOOLN	Free_IHC_Zhang(void);

UniParListPtr	GetUniParListPtr_IHC_Zhang(void);

BOOLN	InitModule_IHC_Zhang(ModulePtr theModule);

BOOLN	InitProcessVariables_IHC_Zhang(EarObjectPtr data);

void	InitSynapse_IHC_Zhang(TSynapse *pthis);

BOOLN	Init_IHC_Zhang(ParameterSpecifier parSpec);

BOOLN	PrintPars_IHC_Zhang(void);

BOOLN	RunModel_IHC_Zhang(EarObjectPtr data);

void	RunSynapseDynamic_IHC_Zhang(TSynapse *pthis, const double *in,
		  double *out, const int length);

BOOLN	SetAAROverAST_IHC_Zhang(double theAAROverAST);

BOOLN	SetAAss_IHC_Zhang(double theAAss);

BOOLN	SetKs_IHC_Zhang(double theKs);

BOOLN	SetPIMax_IHC_Zhang(double thePIMax);

BOOLN	SetParsPointer_IHC_Zhang(ModulePtr theModule);

BOOLN	SetSpont_IHC_Zhang(double theSpont);

BOOLN	SetTauR_IHC_Zhang(double theTauR);

BOOLN	SetTauST_IHC_Zhang(double theTauST);

BOOLN	SetUniParList_IHC_Zhang(void);

BOOLN	SetVs1_IHC_Zhang(double theVs1);

BOOLN	SetVs2_IHC_Zhang(double theVs2);

BOOLN	SetVs3_IHC_Zhang(double theVs3);

__END_DECLS

#endif
