/******************
 *
 * File:		MoNCMcGregor.h
 * Purpose:		This module implements  the MacGregor Model.
 * Comments:	It was revised from code written by Trevor Shackleton and
 *				M. J. Hewitt.
 * Authors:		L. P. O'Mard.
 * Created:		Oct 1993
 * Updated:		24 May 1996
 * Copyright:	(c) 1998, University of Essex.
 *
 ******************/

#ifndef	_MONCMCGREGOR_H
#define _MONCMCGREGOR_H	1

/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

#define NEURON_MCGREGOR_NUM_PARS	9
#define	MCGREGOR_DE_LIMIT			0.7	/* -limits the value of G/tm => dE. */

/*
 * ns = nano siemens - unit of conductance.
 * The default values are for the Stellate model.
 */

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

typedef enum {

	NEURON_MCGREGOR_MEMBRANETCONST_TM,
	NEURON_MCGREGOR_KDECAYTCONST_TGK,
	NEURON_MCGREGOR_THRESHOLDTCONST_TTH,
	NEURON_MCGREGOR_ACCOMCONST_C,
	NEURON_MCGREGOR_DELAYEDRECTKCOND_B,
	NEURON_MCGREGOR_RESTINGTHRESHOLD_TH0,
	NEURON_MCGREGOR_ACTIONPOTENTIAL,
	NEURON_MCGREGOR_KEQUILIBRIUMPOT_EK,
	NEURON_MCGREGOR_CELLRESTINGPOT_ER

} McGregorParSpecifier;

typedef struct {

	int		lastSpikeState;
	double	potential_E;
	double	kConductance_Gk;
	double	threshold_Th;

} McGregorState, *McGregorStatePtr;

typedef struct  {

	ParameterSpecifier parSpec;
	
	BOOLN	membraneTConstFlag, thresholdTConstFlag, accomConstFlag;
	BOOLN	delayedRectKCondFlag, restingThresholdFlag, actionPotentialFlag;
	BOOLN	kDecayTConstFlag, kEquilibriumPotFlag, cellRestingPotFlag;
	BOOLN	updateProcessVariablesFlag;

	double	membraneTConst_Tm;	/* Membrane time constant (s). */	
	double	kDecayTConst_TGk;	/* Potassium conductance decay constant (s). */
	double	thresholdTConst_TTh;/* Threshold rise time constant (s). */
	double	accomConst_c;		/* Accomdation constant (dimensionless). */
	double	delayedRectKCond_b;	/* Delayed Rectifier K Cond. strength (ns). */
	double	restingThreshold_Th0;/* Resting threshold of cell (mV). */
	double	actionPotential;	/* Action potential of spike (mv). */
	double	kEquilibriumPot_Ek;	/* Reversal pot. of the K conductance (mV). */
	double	cellRestingPot_Er;	/* Resting potential for K conductance (mV). */

	/* Private members */
	UniParListPtr	parList;
	McGregorState *state;

} McGregor, *McGregorPtr;

/******************************************************************************/
/*************************** External Variables *******************************/
/******************************************************************************/

extern	McGregorPtr	mcGregorPtr;

/******************************************************************************/
/*************************** Function Prototypes ******************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

BOOLN		CheckPars_Neuron_McGregor(void);

BOOLN		Free_Neuron_McGregor(void);

void		FreeProcessVariables_Neuron_McGregor(void);

double		GetPotentialResponse_Neuron_McGregor(double potential);

double		GetRestingResponse_Neuron_McGregor(void);

UniParListPtr	GetUniParListPtr_Neuron_McGregor(void);

BOOLN		Init_Neuron_McGregor(ParameterSpecifier parSpec);

BOOLN		InitProcessVariables_Neuron_McGregor(EarObjectPtr data);

BOOLN		PrintPars_Neuron_McGregor(void);

BOOLN		ReadPars_Neuron_McGregor(char *fileName);

BOOLN		RunModel_Neuron_McGregor(EarObjectPtr data);

BOOLN		SetAccomConst_Neuron_McGregor(double theAccomConst);

BOOLN		SetActionPotential_Neuron_McGregor(double theActionPotential);

BOOLN		SetDelayedRectKCond_Neuron_McGregor(double theDelayedRectKCond);

BOOLN		SetKDecayTConst_Neuron_McGregor(double theKDecayTConst);

BOOLN		SetKEquilibriumPot_Neuron_McGregor(double theKEquilibriumPot);

BOOLN		SetCellRestingPot_Neuron_McGregor(double theCellRestingPot);

BOOLN		SetMembraneTConst_Neuron_McGregor(double theMembraneTConst);

BOOLN		SetPars_Neuron_McGregor(double tm, double tGk, double tTh,
			  double c, double b, double tH0, double aP, double ek, double er);

BOOLN		SetRestingThreshold_Neuron_McGregor(double theRestingThreshold);

BOOLN		SetThresholdTConst_Neuron_McGregor(double theThresholdTConst);

BOOLN	SetUniParList_Neuron_McGregor(void);

__END_DECLS

#endif
