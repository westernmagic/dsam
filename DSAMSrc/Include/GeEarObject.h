/******************
 *
 * File:		EarObject.h
 * Purpose:		This module contains the routines for the EarObject Module.
 * Comments:	To compile without the ModuleMgr support, uncomment
 *				"#define _NO_MODULMGR" in the "GeEarObject.h" file.
 *				12-03-97 LPO: Added SetMaxInSignals routine.
 *				29-04-98 LPO: The SetProcessName_EarObject routine will now
 *				indescriminately replace any previous names.
 *				01-05-98 LPO: Created the GetInSignalRef_EarObject() routine
 *				so that it can be used by the Simulation specification module
 *				for connection to input signals.
 *				15-10-98 LPO: The 'ResetProcess_...' routine now resets the
 *				process time to the PROCESS_START_TIME.
 *				27-01-99 LPO: The 'FreeAll_' routine now sets the
 *				'earObjectCount' to zero.
 * Authors:		L. P. O'Mard
 * Created:		18 Feb 1993
 * Updated:		27 Jan 1998
 * Copyright:	(c) 1999, University of Essex
 * 
 ******************/

#ifndef	_EAROBJECT_H
#define _EAROBJECT_H	1

#include "GeSignalData.h"
 
/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

/*
#define	_NO_MODULEMGR
*/
#define	RESET_TO_ZERO_EAROBJECT	TRUE	/* Tell InitOutSignal to reset signal */

/******************************************************************************/
/*************************** Macro Definitions ********************************/
/******************************************************************************/

#define	_WorldTime_EarObject(EAROBJ)	((!dSAM.segmentedMode || \
  (EAROBJ)->outSignal->staticTimeFlag || ((EAROBJ)->timeIndex < \
  (EAROBJ)->outSignal->length))? PROCESS_START_TIME: (EAROBJ)->timeIndex - \
  (EAROBJ)->outSignal->length)

#define _GetSample_EarObject(EAROBJ, CHAN, SAMPLE) \
  ((EAROBJ)->outSignal->channel[CHAN][SAMPLE])

#define _GetResult_EarObject(EAROBJ, CHAN) \
  ((EAROBJ)->outSignal->channel[CHAN][0])

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

typedef unsigned int	EarObjHandle;		/* For customer management. */

typedef struct refNode *EarObjRefPtr;	/* Pre-referencing of customer node.  */

#ifndef MODULE_PTR
#	define MODULE_PTR

	typedef struct moduleStruct  *ModulePtr;/* Pre-referencing of module.  */
	
#endif

typedef struct {

	BOOLN		localOutSignalFlag;	/* EarObjects must only destroy their*/
									/* own signals. */
	BOOLN		updateCustomersFlag;	/* This is set when an update is */
										/* required */
	BOOLN		updateProcessFlag;	/* Set when dt is changed for a signal. */
	BOOLN			*inSignalFlag;	/* Record of active input signals. */
	BOOLN			firstSectionFlag;/* Set for first signal section. */
	EarObjHandle	handle;			/* Reference handle for manager. */
	char			*processName;	/* The origins of the output stimulus */
	int				maxInSignals;	/* Total number of input signals. */
	int				numInSignals;	/* Count of input signals. */
	ChanLen			timeIndex;		/* used in segmented mode processing */
	SignalDataPtr	*inSignal;		/* The original signals */
	SignalDataPtr	outSignal;		/* The signal set by the process stages */
	EarObjRefPtr	customerList;	/* Pointer to list of customers. */
	EarObjRefPtr	supplierList;	/* Pointer to list of suppliers. */
#	ifndef _NO_MODULEMGR
	ModulePtr		module;			/* Module linked with process. */
#	endif

} EarObject, *EarObjectPtr;

typedef struct refNode {	/* Reference node */

	int				inSignalRef;/* Reference to specific input signal. */
	EarObjectPtr	earObject;
	EarObjRefPtr	next;		/* The next customer in the list, or NULL. */
	
} EarObjRef;

/******************************************************************************/
/*************************** External Variables *******************************/
/******************************************************************************/

extern EarObjHandle	earObjectCount;
extern EarObjRefPtr	mainEarObjectList;

/******************************************************************************/
/*************************** Function Prototypes ******************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

/*************************** External Functions *******************************/

ModulePtr	Init_ModuleMgr(char *name);

void		Free_ModuleMgr(ModulePtr *theModule);

/*************************** Main Functions ***********************************/

BOOLN		AddEarObjRef_EarObject(EarObjRefPtr *theList,	
			  EarObjectPtr theCustomer, int inSignalRef);
			
BOOLN		ConnectOutSignalToIn_EarObject(EarObjectPtr supplier,	
			  EarObjectPtr customer);
			
EarObjRefPtr	CreateEarObjRef_EarObject(EarObjectPtr theObject,
				  int inSignalRef);		
			
BOOLN		DisconnectOutSignalFromIn_EarObject(EarObjectPtr supplier,
			  EarObjectPtr customer);

void		FreeAll_EarObject(void);	
			
void		FreeEarObjRefList_EarObject(EarObjRefPtr *theList);	
			
int			FreeEarObjRef_EarObject(EarObjRefPtr *theList, 	
			  EarObjHandle theHandle);

void		FreeOutSignal_EarObject(EarObjectPtr data);

void		Free_EarObject(EarObjectPtr *theObject);	
			
EarObjectPtr	Init_EarObject(char *moduleName);		

EarObjectPtr	Init_EarObject_MultiInput(char *moduleName, int maxInSignals);
			
BOOLN		InitOutFromInSignal_EarObject(EarObjectPtr data, int numChannels);	
			
BOOLN		InitOutSignal_EarObject(EarObjectPtr data, int numChannels, 	
			  ChanLen length, double samplingInterval);
			
double		GetResult_EarObject(EarObjectPtr data, uShort channel);

double		GetSample_EarObject(EarObjectPtr data, uShort channel,
			  ChanLen sample);

void		PrintProcessName_EarObject(char *message, EarObjectPtr data);	
			
void		RemoveEarObjRefs_EarObject(EarObjectPtr theObject);	
			
void		ResetProcess_EarObject(EarObjectPtr theObject);

void		ResetSignalContinuity_EarObject(EarObjectPtr data,
			  SignalDataPtr oldOutSignal);

void		SetProcessContinuity_EarObject(EarObjectPtr data);

void		SetProcessName_EarObject(EarObjectPtr theObject, char *format, ...);
			
void		SetMaxInSignals_EarObject(EarObjectPtr theObject, int maxInSignals);

BOOLN		SetNewOutSignal_EarObject(EarObjectPtr data, int numChannels,
			  ChanLen length, double samplingInterval);
			
void		SetTimeContinuity_EarObject(EarObjectPtr data);

void		UpdateCustomers_EarObject(EarObjectPtr theObject);	
			
int			GetInSignalRef_EarObject(EarObjectPtr data);

__END_DECLS

#endif
