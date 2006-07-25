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
#include "UtRandom.h"

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

#define	_WorldTime_EarObject(EAROBJ)	((!GetDSAMPtr_Common()->segmentedMode \
  || (EAROBJ)->outSignal->staticTimeFlag || ((EAROBJ)->timeIndex < \
  (EAROBJ)->outSignal->length))? PROCESS_START_TIME: (EAROBJ)->timeIndex - \
  (EAROBJ)->outSignal->length)

#define _GetSample_EarObject(EAROBJ, CHAN, SAMPLE) \
  ((EAROBJ)->outSignal->channel[CHAN][SAMPLE])

#define _GetResult_EarObject(EAROBJ, CHAN) \
  ((EAROBJ)->outSignal->channel[CHAN][0])

#define _InSig_EarObject(EAROBJ, NUM)	((EAROBJ)->inSignal[(NUM)])

#define _OutSig_EarObject(EAROBJ)	((EAROBJ)->outSignal)

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

typedef unsigned int	EarObjHandle;		/* For customer management. */

typedef struct refNode *EarObjRefPtr;	/* Pre-referencing of customer node.  */

#ifndef MODULE_PTR
#	define MODULE_PTR

	typedef struct moduleStruct  *ModulePtr;/* Pre-referencing of module.  */
	
#endif

typedef struct _EarObject {

	BOOLN		localOutSignalFlag;	/* EarObjects must only destroy their*/
									/* own signals. */
	BOOLN		externalDataFlag;	/* Set when channel pointers are set */
									/* using external data. */
	BOOLN		initThreadRunFlag;	/* For initialising thread run. */
	BOOLN		threadRunFlag;		/* Set when running thread. */
	BOOLN		useThreadsFlag;		/* Set when multiple channels processed. */
	BOOLN		chainInitRunFlag;	/* For chain initialisation run. */
	BOOLN		updateCustomersFlag;	/* This is set when an update is */
										/* required */
	BOOLN		updateProcessFlag;	/* Set when dt is changed for a signal. */
	BOOLN			firstSectionFlag;/* Set for first signal section. */
	EarObjHandle	handle;			/* Reference handle for manager. */
	WChar			*processName;	/* The origins of the output stimulus */
	int				numInSignals;	/* Count of input signals. */
	int				numThreads;		/* No. of thread process using EarObject */
	int				threadIndex;	/* Identifies the thread process. */
	int				numSubProcesses;/* No. of associated. sub-processes. */
	ChanLen			timeIndex;		/* used in segmented mode processing */
	RandParsPtr		randPars;		/* Used to store random number pars. */
	SignalDataPtr	*inSignal;		/* The original signals */
	SignalDataPtr	outSignal;		/* The signal set by the process stages */
	EarObjRefPtr	customerList;	/* Pointer to list of customers. */
	EarObjRefPtr	supplierList;	/* Pointer to list of suppliers. */
#	ifndef _NO_MODULEMGR
	ModulePtr		module;			/* Module linked with process. */
#	endif
	void			*clientData;	/* Used to store misc. handling info. */
	struct _EarObject	*threadProcs;	/* Pointer to thread EarObject copies */
	struct _EarObject	**subProcessList;	/* Pointers to subProcesses. */

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

__BEGIN_DECLS

extern BOOLN (* ResetProcess_EarObject)(EarObjectPtr);

__END_DECLS

/******************************************************************************/
/*************************** Function Prototypes ******************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

/*************************** External Functions *******************************/

ModulePtr	Init_ModuleMgr(WChar *name);

void		Free_ModuleMgr(ModulePtr *theModule);

/*************************** Main Functions ***********************************/

BOOLN	AddEarObjRef_EarObject(EarObjRefPtr *theList, EarObjectPtr theCustomer);
			
BOOLN	AddInSignal_EarObject(EarObjectPtr data);

BOOLN	CheckInSignal_EarObject(EarObjectPtr data, const WChar 
		  *callingFuncName);

BOOLN	ConnectOutSignalToIn_EarObject(EarObjectPtr supplier,
		  EarObjectPtr customer);
			
EarObjRefPtr	CreateEarObjRef_EarObject(EarObjectPtr theObject);		
			
BOOLN	DelInSignal_EarObject(EarObjectPtr data, SignalDataPtr signal);

BOOLN	DisconnectOutSignalFromIn_EarObject(EarObjectPtr supplier,
		  EarObjectPtr customer);

void	FreeAll_EarObject(void);	
			
void	FreeEarObjRefList_EarObject(EarObjRefPtr *theList);	
			
int		FreeEarObjRef_EarObject(EarObjRefPtr *theList, EarObjHandle theHandle);

void	FreeSubProcessList_EarObject(EarObjectPtr p);

void	FreeThreadProcs_EarObject(EarObjectPtr p);

void	FreeThreadSubProcs_EarObject(EarObjectPtr p);

void	FreeOutSignal_EarObject(EarObjectPtr data);

void	Free_EarObject(EarObjectPtr *theObject);	
			
EarObjectPtr	Init_EarObject(WChar *moduleName);		

void	InitOutDataFromInSignal_EarObject(EarObjectPtr data);

BOOLN	InitOutFromInSignal_EarObject(EarObjectPtr data, uShort numChannels);	
			
BOOLN	InitOutTypeFromInSignal_EarObject(EarObjectPtr data, uShort
		  numChannels);

BOOLN	InitOutSignal_EarObject(EarObjectPtr data, uShort numChannels, 	
		  ChanLen length, double samplingInterval);
			
void	InitThreadRandPars_EarObject(EarObjectPtr p, EarObjectPtr baseP);

BOOLN	InitSubProcessList_EarObject(EarObjectPtr p, int numSubProcesses);

BOOLN	InitThreadProcs_EarObject(EarObjectPtr p);

BOOLN	InitThreadSubProcs_EarObject(EarObjectPtr p, EarObjectPtr baseP);

double	GetResult_EarObject(EarObjectPtr data, uShort channel);

double	GetSample_EarObject(EarObjectPtr data, uShort channel, ChanLen sample);

void	PrintProcessName_EarObject(WChar *message, EarObjectPtr data);	

void	RemoveEarObjRefs_EarObject(EarObjectPtr theObject);	
			
void	ResetOutSignal_EarObject(EarObjectPtr data);

BOOLN	ResetProcessStandard_EarObject(EarObjectPtr theObject);

void	ResetSignalContinuity_EarObject(EarObjectPtr data,
		  SignalDataPtr oldOutSignal);

BOOLN	SetNewOutSignal_EarObject(EarObjectPtr data, uShort numChannels,
		  ChanLen length, double samplingInterval);
			
void	SetProcessContinuity_EarObject(EarObjectPtr data);

void	SetProcessForReset_EarObject(EarObjectPtr theObject);

void	SetProcessName_EarObject(EarObjectPtr theObject, WChar *format, ...);
			
BOOLN	SetRandPars_EarObject(EarObjectPtr p, long ranSeed,
		  const WChar *callingFunc);

void	SetResetProcess_EarObject(BOOLN (* Func)(EarObjectPtr));

void	SetThreadRunFlag_EarObject(EarObjectPtr theObject, BOOLN setting);

void	SetTimeContinuity_EarObject(EarObjectPtr data);

void	SetUpdateProcessFlag_EarObject(EarObjectPtr theObject, BOOLN setting);

void	SetUtilityProcessContinuity_EarObject(EarObjectPtr data);

BOOLN	TempInputConnection_EarObject(EarObjectPtr base,
		  EarObjectPtr supporting, int numInSignals);

void	UpdateCustomers_EarObject(EarObjectPtr theObject);	
			
__END_DECLS

#endif
