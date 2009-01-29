/**********************
 *
 * File:		MoNCHHuxley.h
 * Purpose:		This module is Ray's general purpose neuron model, based on the
 *				Hodgkin Huxley implementation.
 * Comments:	Written using ModuleProducer version 1.10 (Oct 13 1996).
 *				18-12-96 LPO: debug operationMode added.  The output channels
 *				are in the order: V, leakage current, conductance, current,
 *				conductance, current,....
 *				03-01-97 LPO: The module has been amended so that up to three
 *				EarObjects can be connect to the module for (1) current
 *				injections; (2) excitatory conductance and (3) inhibitory
 *				conductance.
 *				If the current injection mode is "ON" then the first signal is
 *				always the current injection.
 *				15-01-97 LPO: Currected the activation (proportion of channels
 *				open) for the conductance calculations.
 *				18-01-97 The leakage conductance and potential are now part of
 *				of the IonChanList module - so the leakage conductance can be
 *				changed using the temperature scaling in the "Rothman" mode.
 *				27-01-98 LPO: Re-introduced the "restingPotential" parameter.
 *				If this is set to a non-physical value (>=0) then a calibration
 *				run is carried out to calculate the resting potential.
 *				30-01-98 LPO: Introducing a separate "diagnostic" (debug) mode.
 *				02-02-98 LPO: Fixed memory leak: iCList was not being free'd.
 * Author:		L. P. O'Mard
 * Created:		14 Oct 1996
 * Updated:		02 Feb 1998
 * Copyright:	(c) 1998, University of Essex
 *
 *********************/

#ifndef _MONCHHUXLEY_H
#define _MONCHHUXLEY_H 1

#include "UtNameSpecs.h"

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define NEURON_HHUXLEY_NUM_PARS			11
#define	HHUXLEY_FIND_REST_MAX_LOOP		500

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef enum {

	NEURON_HHUXLEY_DIAGNOSTICMODE,
	NEURON_HHUXLEY_OPERATIONMODE,
	NEURON_HHUXLEY_INJECTIONMODE,
	NEURON_HHUXLEY_EXCITATORYREVERSALPOT,
	NEURON_HHUXLEY_INHIBITORYREVERSALPOT,
	NEURON_HHUXLEY_SHUNTINHIBITORYREVERSALPOT,
	NEURON_HHUXLEY_CELLCAPACITANCE,
	NEURON_HHUXLEY_RESTINGPOTENTIAL,
	NEURON_HHUXLEY_RESTINGSIGNALDURATION,
	NEURON_HHUXLEY_RESTINGCRITERIA,
	NEURON_HHUXLEY_ICLIST

} HHuxleyNCParSpecifier;

typedef enum {

	HHUXLEYNC_OPERATION_NORMAL_MODE,
	HHUXLEYNC_OPERATION_VOLTAGE_CLAMP_MODE,
	HHUXLEYNC_OPERATION_NULL

} HHuxleyNCOperationModeSpecifier;

typedef enum {

	HHUXLEYNC_INJECTION_OFF,
	HHUXLEYNC_INJECTION_ON,
	HHUXLEYNC_INJECTION_NULL

} HHuxleyNCInjectionModeSpecifier;

typedef struct {

	double	*y, *z, *x;
	double	potential_V;

} HHuxleyState, *HHuxleyStatePtr;

typedef struct {

	ParameterSpecifier	parSpec;

	BOOLN	diagnosticModeFlag, operationModeFlag, injectionModeFlag;
	BOOLN	excitatoryReversalPotFlag, inhibitoryReversalPotFlag;
	BOOLN	shuntInhibitoryReversalPotFlag, cellCapacitanceFlag;
	BOOLN	restingPotentialFlag, restingSignalDurationFlag;
	BOOLN	restingCriteriaFlag, iCListFlag;
	BOOLN	updateProcessVariablesFlag;

	int		diagnosticMode;
	int		operationMode;
	int		injectionMode;
	double	excitatoryReversalPot;
	double	inhibitoryReversalPot;
	double	shuntInhibitoryReversalPot;
	double	cellCapacitance;
	double	restingPotential;
	double	restingSignalDuration;
	double	restingCriteria;
	IonChanListPtr	iCList;

	/* Private Members */
	NameSpecifier	*diagnosticModeList;
	NameSpecifier	*operationModeList;
	NameSpecifier	*injectionModeList;
	UniParListPtr	parList;
	WChar			diagFileName[MAX_FILE_PATH];
	FILE			*fp;
	BOOLN	restingRun, debug;
	int		numChannels;
	double	restingPot, dt, dtOverC;
	HHuxleyState	*state;

} HHuxleyNC, *HHuxleyNCPtr;

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

extern	HHuxleyNCPtr	hHuxleyNCPtr;

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

BOOLN	CheckData_Neuron_HHuxley(EarObjectPtr data);

BOOLN	CheckPars_Neuron_HHuxley(void);

double	FindRestingPotential_Neuron_HHuxley(double restingCriteria,
		  double duration, double dt);

BOOLN	Free_Neuron_HHuxley(void);

void	FreeProcessVariables_Neuron_HHuxley(void);

IonChanListPtr	GetICListPtr_Neuron_HHuxley(void);

UniParListPtr	GetUniParListPtr_Neuron_HHuxley(void);

BOOLN	Init_Neuron_HHuxley(ParameterSpecifier parSpec);

BOOLN	InitOperationModeList_Neuron_HHuxley(void);

BOOLN	InitProcessVariables_Neuron_HHuxley(EarObjectPtr data);

BOOLN	PrintPars_Neuron_HHuxley(void);

BOOLN	ReadPars_Neuron_HHuxley(WChar *fileName);

BOOLN	RunModel_Neuron_HHuxley(EarObjectPtr data);

BOOLN	SetCellCapacitance_Neuron_HHuxley(double theCellCapacitance);

BOOLN	SetDiagnosticMode_Neuron_HHuxley(WChar *theDiagnosticMode);

BOOLN	SetExcitatoryReversalPot_Neuron_HHuxley(double
		  theExcitatoryReversalPot);

void *	GetPotentialResponse_Neuron_HHuxley(void *potentialPtr);

BOOLN	SetICList_Neuron_HHuxley(IonChanListPtr theICList);

BOOLN	SetInhibitoryReversalPot_Neuron_HHuxley(double
			  theInhibitoryReversalPot);

BOOLN	SetInjectionMode_Neuron_HHuxley(WChar *theInjectionMode);

BOOLN	InitModule_Neuron_HHuxley(ModulePtr theModule);

BOOLN	SetOperationMode_Neuron_HHuxley(WChar *theOperationMode);

BOOLN	SetParsPointer_Neuron_HHuxley(ModulePtr theModule);

BOOLN	SetPars_Neuron_HHuxley(IonChanListPtr iCList, WChar *diagnosticMode,
		  WChar *operationMode, WChar *injectionMode,
		  double excitatoryReversalPot, double inhibitoryReversalPot,
		  double shuntInhibitoryReversalPot, double cellCapacitance,
		  double restingPotential, double restingSignalDuration,
		  double restingCriteria);

BOOLN	SetRestingCriteria_Neuron_HHuxley(double theRestingCriteria);

BOOLN	SetRestingPotential_Neuron_HHuxley(double theRestingPotential);

BOOLN	SetRestingSignalDuration_Neuron_HHuxley(
		  double theRestingSignalDuration);

BOOLN	SetShuntInhibitoryReversalPot_Neuron_HHuxley(
		  double theShuntInhibitoryReversalPot);

BOOLN	SetUniParList_Neuron_HHuxley(void);

__END_DECLS

#endif
