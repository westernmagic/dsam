/**********************
 *
 * File:		UtDatum.h
 * Purpose:		This module contains the process control, "datum" management
 *				code.
 * Comments:	Written using ModuleProducer version 1.9 (May 27 1996).
 *				11-03-97 LPO: This code has been amended to allow the numbers
 *				of inputs and outputs for an EarObject to be specified.
 *				The number of inputs defines how many of the previous
 *				EarObjects should be connected to a particular EarObject.
 *				The number of outputs defines how many of the following
 *				EarObjects should be connected accordingly.
 *				21-08-97 LPO: Removed rampInterval parameter - use new ramp
 *				module.
 *				19-11-98 LPO: Corrected the "General parameter list' bug, that
 *				meant that the 'array_index' general parameter was not being
 *				found for a particular process.
 *				10-12-98 LPO: Introduced handling of NULL parLists, i.e. for 
 *				modules with no parameters.
 *				27-01-99 LPO: The static 'stepCount' in the 'InstallInst_'
 *				routine was not being reset to zero when installing 'datum'
 *				nodes at the head of the list.  This occurred with the 
 *				GUI mode, where simulations needed to be recreated.
 *				02-02-99 LPO: The 'FindModuleUniPar_' routine now also returns
 *				the simulation 'DatumPtr' at which the universal parameter was
 *				found.
 *				The 'SetUniParValue_' routine has been moved here from the
 *				UtSimScript module, as it needs to use the
 *				'SetParsPointer_ModuleMgr' routine.
 *				05-05-99 LPO: The 'InitialiseModules_' routine now sets the
 *				display signal's window title using the 'stepNumber' field, if
 *				it has not already been set.
 *				24-08-99 LPO: Implemented nicer output for the
 *				'PrintInstructions_" routine.
 * Author:		L. P. O'Mard
 * Created:		01 Jul 1996
 * Updated:		24 Aug 1999
 * Copyright:	(c) 1999, University of Essex
 *
 *********************/

#ifndef _UTDATUM_H
#define _UTDATUM_H 1

#include "UtDynaList.h"
#include "UtDynaBList.h"

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define	DATUM_DEFAULT_EAROBJECT_CONNECTIONS		1
#define DATUM_INITIAL_INDENT_LEVEL				0
#define DATUM_DEFAULT_LABEL_PREFIX				"p"

/******************************************************************************/
/****************************** Macro definitions *****************************/
/******************************************************************************/

#define DATUM_IN_SIMULATION(PC)	((PC)->previous || (PC)->next)

#define GET_DATUM_CLIENT_DATA(PC)	(((PC)->type == PROCESS)? \
		  (PC)->data->clientData: (PC)->clientData)

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef struct Datum {

	BOOLN	onFlag;
	int		type;
	uInt	stepNumber;
	char	*label;
	union {
		char	*string;
		struct {
			char	*parFile;
			char	*moduleName;
			DynaListPtr		inputList;
			DynaListPtr		outputList;
		} proc;
		struct {
			BOOLN	stopPlaced;
			int		count;
		} loop;
	} u;
	EarObjectPtr	data;
	void			*clientData;
	struct Datum	*previous;			/* To link to previous datum */
	struct Datum	*next;				/* To link to next datum */

} Datum, *DatumPtr;

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

extern uInt	datumStepCount;

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

void	AppendInst_Utility_Datum(DatumPtr *head, DatumPtr pos, DatumPtr datum);

BOOLN	CheckInputConnections_Utility_Datum(DatumPtr pc, DynaBListPtr
		  labelBList);

BOOLN	CheckParLists_Utility_Datum(DatumPtr start);

int		CmpLabel_Utility_Datum(void *a, void *b);

int		CmpProcessLabel_Utility_Datum(void *a, void *b);

int		CmpProcessLabels_Utility_Datum(void *a, void *b);

void	ConnectInst_Utility_Datum(DatumPtr *head, DatumPtr from, DatumPtr to);

void	DisconnectInst_Utility_Datum(DatumPtr *head, DatumPtr from,
		  DatumPtr to);

DatumPtr	Execute_Utility_Datum(DatumPtr start);

DatumPtr	FindLabelledInst_Utility_Datum(DatumPtr start, char *label);

EarObjectPtr	FindLabelledProcess_Utility_Datum(DatumPtr start, char *label);

DatumPtr	FindLabelledProcessInst_Utility_Datum(DatumPtr start, char *label);

DatumPtr	FindModuleProcessInst_Utility_Datum(DatumPtr start,
			  char *moduleName);

BOOLN	FindModuleUniPar_Utility_Datum(UniParListPtr *parList, uInt *index,
		  DatumPtr *pc, char *parSpecifier, BOOLN diagnosticsOn);

BOOLN	FindNearestProcesses_Utility_Datum(DatumPtr *fromPc, DatumPtr *toPc);

EarObjectPtr	FindProcess_Utility_Datum(DatumPtr pc, char *processSpecifier);

void	FreeInstFromSim_Utility_Datum(DatumPtr *start, DatumPtr pc);

void	FreeInstruction_Utility_Datum(DatumPtr *pc);

void	FreeInstructions_Utility_Datum(DatumPtr *pc);

BOOLN	FreeEarObjects_Utility_Datum(DatumPtr start);

EarObjectPtr	GetFirstProcess_Utility_Datum(DatumPtr start);

int		GetInstIntVal_Utility_Datum(DatumPtr start, char *label);

DatumPtr	GetLastInst_Utility_Datum(DatumPtr head);

EarObjectPtr	GetLastProcess_Utility_Datum(DatumPtr start);

char *	GetProcessName_Utility_Datum(DatumPtr pc);

UniParListPtr	GetUniParListPtr_ModuleMgr(EarObjectPtr data);

BOOLN	InitialiseEarObjects_Utility_Datum(DatumPtr start,
		  DynaBListPtr *labelBList);

BOOLN	InitialiseModules_Utility_Datum(DatumPtr start);

DatumPtr	InitInst_Utility_Datum(int type);

void	InsertInst_Utility_Datum(DatumPtr *head, DatumPtr pos, DatumPtr datum);

DatumPtr	InstallInst_Utility_Datum(DatumPtr *head, int type);

void	PrintConnections_Utility_Datum(DynaListPtr list);

void	PrintIndentAndLabel_Utility_Datum(DatumPtr pc, int indentLevel);

void	PrintInstructions_Utility_Datum(DatumPtr pc, char *scriptName,
		  int indentLevel, char *prefix);

BOOLN	PrintParListModules_Utility_Datum(DatumPtr start, char *prefix);

BOOLN	PrintParsModules_Utility_Datum(DatumPtr start);

BOOLN	PrintSimScript_Utility_Datum(DatumPtr pc, char *scriptName,
		  int indentLevel, char *prefix, BOOLN checkForSubSimScripts);

char *	NameAndLabel_Utility_Datum(DatumPtr pc);

void	ResetSimulation_Utility_Datum(DatumPtr start);

void	ResetStepCount_Utility_Datum(void);

BOOLN	ResolveInstLabels_Utility_Datum(DatumPtr start, DynaBListPtr
		  labelBList);

BOOLN	SetDefaultConnections_Utility_Datum(DatumPtr start);

BOOLN	SetDefaultLabel_Utility_Datum(DatumPtr pc);

BOOLN	SetDefaultLabels_Utility_Datum(DatumPtr start);

BOOLN	SetOutputConnections_Utility_Datum(DatumPtr pc, DynaBListPtr
		  labelBList);

BOOLN	SetUniParValue_Utility_Datum(DatumPtr start, char *parName,
		  char *parValue);

__END_DECLS

#endif
