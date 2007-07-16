/**********************
 *
 * File:		Utils.h
 * Purpose:		This module contains the common utilities for the filter test
 *				modules.
 * Comments:	It was created to be used with the feval program.
 *				Routines were copied from the AutoTest.c program
 * Author:		L. P. O'Mard
 * Created:		28 Jun 2000
 * Updated:		
 * Copyright:	(c) 2000, University of Essex
 *
 *********************/

#ifndef UTILS_H
#define UTILS_H 1

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define CHANNEL				0				/* Channel number. */

#define ANA_FINDBIN_PROCESS_NAME	wxT("Ana_Findbin")
#define ANA_INTENSITY_PROCESS_NAME	wxT("Ana_Intensity")
#define ANA_FOURIERT_PROCESS_NAME	wxT("Ana_FourierT")
#define DATAFILE_IN_PROCES_NAME		wxT("DataFile_In")
#define STIM_CLICK_PROCESS_NAME		wxT("Stim_Click")
#define STIM_PURETONE_PROCESS_NAME	wxT("Stim_PureTone")
#define STIM_PURETONE_MULTI_PROCESS_NAME	wxT("Stim_PureTone_Multi")
#define STIM_STEPFUN_PROCESS_NAME	wxT("Stim_StepFun")
#define TRANS_GATE_PROCESS_NAME		wxT("Trans_Gate")
#define TRANS_SETDBSPL_PROCES_NAME	wxT("Trans_SetDBSPL")
#define UTIL_SIMSCRIPT_PROCES_NAME	wxT("Util_SimScript")

#define MAX_GRADIENT_RATIO		0.5

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

typedef enum {

	UTILS_PHASE_ANALYSISOUTPUT_DEGREES_MODE,
	UTILS_PHASE_ANALYSISOUTPUT_RADIANS_MODE,
	UTILS_PHASE_ANALYSISOUTPUT_CYCLES_MODE,
	UTILS_PHASE_ANALYSISOUTPUTMODE_NULL

} PhaseOutputModeSpecifier;

typedef enum {

	UTILS_NO_TIME_COLUMN,
	UTILS_NORMAL_SIGNAL

} OutputStyleSpecifier;

/******************************************************************************/
/*************************** External Variables *******************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Subroutine declarations **************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

double	CalcPhase_Utils(double phase, double dF,
		  PhaseOutputModeSpecifier outputMode, BOOLN reset);

double	CalcVectorAngle_Utils(double a1, double a2, double b1, double b2);

double	GetFreqIndexSample_Utils(EarObjectPtr data, double frequency);

double	GetPhase_Utils(double phase, PhaseOutputModeSpecifier outputMode);

NameSpecifierPtr	InitPhaseOutputModeList_Utils(void);

EarObjectPtr	InitResultEarObject_Utils(int numChannels, ChanLen length,
				  double initialValue, double increment,
				  const WChar *processName);

BOOLN	OutputResults_Utils(EarObjectPtr data, NameSpecifier *outputMod,
		  OutputStyleSpecifier style);

EarObjectPtr	RunSimulation_Utils(void);

__END_DECLS

#endif
