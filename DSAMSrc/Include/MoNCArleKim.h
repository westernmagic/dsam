/**********************
 *
 * File:		MoNCArleKim.h
 * Purpose:		This module implements Arle & Kim's version of the MacGregor
 *				Model.
 * Comments:	The changes implemented in "Neural Modeling of instrinsic and
 *				spike-discharge properties of cochlear nucleus neurons",
 *				J. E. Erle and D. O. Kim, Biological Cybernetics, Springer
 *				Verlag 1991. - have been added to this module version of the
 *				McGregor model.
 * Author:		Trevor Shackleton and M. J. Hewitt. (Revised L. P. O'Mard).
 * Created:		12 Jul 1993
 * Updated:		
 * Copyright:	(c) 1998, University of Essex.
 *
 **********************/

#ifndef	_MONCARLEKIM_H
#define _MONCARLEKIM_H	1

/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

#define NEURON_ARLEKIM_NUM_PARS	12
#define	ARLEKIM_DE_LIMIT		0.7	/* This limits the value of G/tm => dE. */

/*
 * ns = nano siemens - unit of conductance.
 * The default values are for the Stellate model.
 */

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

typedef enum {

	NEURON_ARLEKIM_MEMBRANETCONST_TM,
	NEURON_ARLEKIM_KDECAYTCONST_TGK,
	NEURON_ARLEKIM_THRESHOLDTCONST_TTH,
	NEURON_ARLEKIM_ACCOMCONST_C,
	NEURON_ARLEKIM_DELAYEDRECTKCOND_B,
	NEURON_ARLEKIM_RESTINGTHRESHOLD_TH0,
	NEURON_ARLEKIM_ACTIONPOTENTIAL,
	NEURON_ARLEKIM_NONLINEARVCONST_VNL,
	NEURON_ARLEKIM_KREVERSALPOTEN_EK,
	NEURON_ARLEKIM_BREVERSALPOTEN_EB,
	NEURON_ARLEKIM_KRESTINGCOND_GK,
	NEURON_ARLEKIM_BRESTINGCOND_GB

} ArleKimParSpecifier;

typedef struct {

	int		lastSpikeState;
	double	potential_V;
	double	kConductance_Gk;
	double	bConductance_Gb;
	double	threshold_Th;

} ArleKimState, *ArleKimStatePtr;

typedef struct  {

	ParameterSpecifier parSpec;
	
	BOOLN	membraneTConstFlag, thresholdTConstFlag, accomConstFlag;
	BOOLN	delayedRectKCondFlag, restingThresholdFlag, actionPotentialFlag;
	BOOLN	kDecayTConstFlag, nonLinearVConstFlag, kReversalPotenFlag;
	BOOLN	bReversalPotenFlag, kRestingCondFlag, bRestingCondFlag;
	BOOLN	updateProcessVariablesFlag;

	double	membraneTConst_Tm;	/* Membrane time constant (s). */	
	double	kDecayTConst_TGk;	/* Potassium conductance decay constant (s). */
	double	thresholdTConst_TTh;/* Threshold rise time constant (s). */
	double	accomConst_c;		/* Accomdation constant (dimensionless). */
	double	delayedRectKCond_b;	/* Delayed Rectifier K Cond. strength (ns). */
	double	restingThreshold_Th0;/* Resting threshold of cell (mV). */
	double	actionPotential;	/* Action potential of spike (mv). */
	double	nonLinearVConst_Vnl;/* Non-linear voltage constant (mV). */
	double	kReversalPoten_Ek;	/* Reversal pot. of the K conductance (mV). */
	double	bReversalPoten_Eb;	/* Rev. pot. of all other conductances (mV) */
	double	kRestingCond_gk;	/* Resting potassium conductance (nS). */
	double	bRestingCond_gb;	/* Resting component for all other conds.(nS).*/	

	/* Private members */
	UniParListPtr	parList;
	ArleKimState	*state;

} ArleKim, *ArleKimPtr;

/******************************************************************************/
/*************************** External Variables *******************************/
/******************************************************************************/

extern	ArleKimPtr		arleKimPtr;

/******************************************************************************/
/*************************** Function Prototypes ******************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

BOOLN	CheckData_Neuron_ArleKim(EarObjectPtr data);

BOOLN	CheckPars_Neuron_ArleKim(void);

BOOLN	Free_Neuron_ArleKim(void);

void	FreeProcessVariables_Neuron_ArleKim(void);

double	GetPotentialResponse_Neuron_ArleKim(double potential);

double	GetRestingResponse_Neuron_ArleKim(void);

UniParListPtr	GetUniParListPtr_Neuron_ArleKim(void);

BOOLN	Init_Neuron_ArleKim(ParameterSpecifier parSpec);

BOOLN	InitProcessVariables_Neuron_ArleKim(EarObjectPtr data);

BOOLN	PrintPars_Neuron_ArleKim(void);

BOOLN	ReadPars_Neuron_ArleKim(char *fileName);

BOOLN	RunModel_Neuron_ArleKim(EarObjectPtr data);

BOOLN	SetAccomConst_Neuron_ArleKim(double theAccomConst);

BOOLN	SetActionPotential_Neuron_ArleKim(double theActionPotential);

BOOLN	SetBRestingCond_Neuron_ArleKim(double theBRestingCond);

BOOLN	SetBReversalPoten_Neuron_ArleKim(double theBReversalPoten);

BOOLN	SetCellRestingPot_Neuron_ArleKim(double theCellRestingPot);

BOOLN	SetDelayedRectKCond_Neuron_ArleKim(double theDelayedRectKCond);

BOOLN	SetKDecayTConst_Neuron_ArleKim(double theKDecayTConst);

BOOLN	SetKRestingCond_Neuron_ArleKim(double theKRestingCond);

BOOLN	SetKReversalPoten_Neuron_ArleKim(double theKReversalPoten);

BOOLN	SetMembraneTConst_Neuron_ArleKim(double theMembraneTConst);

BOOLN	InitModule_Neuron_ArleKim(ModulePtr theModule);

BOOLN	SetNonLinearVConst_Neuron_ArleKim(double theNonLinearVConst);

BOOLN	SetParsPointer_Neuron_ArleKim(ModulePtr theModule);

BOOLN	SetPars_Neuron_ArleKim(double tm, double tGk, double tTh,
		  double c, double b, double tH0, double aP, double vnl,
		  double ek, double eb, double gk, double gb);

BOOLN	SetRestingThreshold_Neuron_ArleKim(double theRestingThreshold);

BOOLN	SetThresholdTConst_Neuron_ArleKim(double theThresholdTConst);

BOOLN	SetUniParList_Neuron_ArleKim(void);

__END_DECLS

#endif
