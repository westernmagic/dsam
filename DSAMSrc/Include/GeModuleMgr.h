/******************
 *
 * File:		GeModuleMgr.h
 * Comments:	These routines carry out various functions such as freeing
 *				all module memory space, and so on.
 *				17-04-98 LPO: Added the NULL_MODULE_NAME string constant.
 *				22-04-98 LPO: Changed all the stimulus module names so that they
 *				start with "Stim_".
 *				10-12-98 LPO: Changed the old 'NullFunction_...' to
 *				'TrueFunction_...' which alwaysreturns a TRUE value.  I have
 *				created a new 'NullFunction_...' which always returns the
 *				correct NULL (zero) value.
 *				05-03-99 LPO: The 'SetParsPointer_' routine now checks for a
 *				NULL module passed as an argument.
 *				18-03-99 LPO: I have added the 'LinkGlueRoutine'.  This routine
 *				is not meant to be used, but it is to access global variables
 *				of modules that wouldn't otherwise be linked.
 *				20-04-99 LPO: The "Free_ModuleMgr" routine was not setting the
 *				appropriate module parameters pointer using the 
 *				"SetParsPointer_" routine.
 *				27-04-99 LPO:  The universal parameter lists have allowed me
 *				to introduce the 'ReadPar_', 'PrintPar_' and 'RunProcess'
 *				'SetPar_' commands.  These commands replace the previous
 *				generic programming interface.  It will also mean that the
 *				ModuleMgr module can be automatically generated.
 *				10-05-99 LPO: Introduced the 'ReadSimParFile_' routine.  There
 *				is only one module which uses this routine, but since it may
 *				be used quite alot in programs, it is best to put it in.
 *				02-06-99 LPO: In line with my drive to remove the 'DoFun'
 *				interface I have created the 'SetRealPar_', 'GetCFListPtr_' and
 *				'GetSimulation_' routines.  These routines will only work with
 *				modules that have these functions.
 *				22-10-99 LPO: The global 'nullModule' is initialised by the
 *				first call to 'Init_'.  Only one instance is ever initialised.
 * Authors:		L. P. O'Mard
 * Created:		29 Mar 1993
 * Updated:		02 Jun 1999
 * Copyright:	(c) 1999, University of Essex.
 * 
 ******************/

#ifndef	_MODULEMGR_H
#define _MODULEMGR_H	1

#include "UtNameSpecs.h"
 
/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

#define	LOCAL_PARAMETERS		TRUE	/* For setting mod. parameter pointer */
#define	GLOBAL_PARAMETERS		FALSE	/* For setting mod. parameter pointer */
#define MAX_MODULE_NAME			30	/* Max. characters in module name. */
#define	NULL_MODULE_NAME		"NULL"
#define BM_MODULE_PREFIX		"BM_"

/******************************************************************************/
/*************************** Macro Definitions ********************************/
/******************************************************************************/

/*************************** Misc. Macros *************************************/

#define MODULE_SPECIFIER(EAROBJ)	((EAROBJ)->module->specifier)

#define MODULE_NAME(EAROBJ)			((EAROBJ)->module->name)

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

typedef enum {

	ANA_SAI_MODULE,
	DISPLAY_MODULE,
	PROCESS_MODULE,
	NULL_MODULE,
	SIMSCRIPT_MODULE,

	MODULE_SPECIFIER_NULL

} ModuleSpecifier;

typedef unsigned int	ModuleHandle;	/* For module management. */

typedef struct moduleStruct {

	BOOLN	onFlag;
	char	name[MAX_MODULE_NAME];
	ModuleSpecifier	specifier;
	ModuleHandle	handle;				/* Reference handle for manager. */
	
	/* General, accessible functions. */
	BOOLN	(* CheckData)(EarObjectPtr data);
	BOOLN	(* CheckPars)(void);
	BOOLN	(* Free)(void);
	double	(* GetPotentialResponse)(double potential);
	double	(* GetRestingResponse)(void);
	UniParListPtr	(* GetUniParListPtr)(void);
	BOOLN	(* PrintPars)(void);
#	ifdef _PAMASTER1_H
		BOOLN (* QueueCommand)(void *parameter, int parCount,
		  TypeSpecifier type, char *label, CommandSpecifier command,
		  ScopeSpecifier scope);
		BOOLN (* SendQueuedCommands)(void);
#	endif
	BOOLN	(* RunProcess)(EarObjectPtr theObject);
	BOOLN	(* ReadPars)(char *fileName);
	BOOLN	(* ReadSignal)(char *fileName, EarObjectPtr data);
	BOOLN	(* InitModule )(struct moduleStruct *);
	BOOLN	(* SetParsPointer)(struct moduleStruct *);
	
	void	*parsPtr;

} Module;

#ifndef MODULE_PTR
#	define MODULE_PTR

	typedef struct moduleStruct  *ModulePtr;
	
#endif

typedef struct moduleNode {

	ModulePtr			module;
	struct moduleNode	*next;
	
} ModuleRef, *ModuleRefPtr;

/******************************************************************************/
/*************************** External Variables *******************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Function Prototypes ******************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

BOOLN		AddModuleRef_ModuleMgr(ModuleRefPtr *theList, ModulePtr theModule);

BOOLN		CheckData_ModuleMgr(EarObjectPtr data, const char *callingFunction);

ModuleRefPtr	CreateModuleRef_ModuleMgr(ModulePtr theModule);

BOOLN		Enable_ModuleMgr(EarObjectPtr data, BOOLN on);

void		Free_ModuleMgr(ModulePtr *theModule);

void		FreeAll_ModuleMgr(void);

void		FreeModuleRef_ModuleMgr(ModuleRefPtr *theList, ModuleHandle
			  theHandle);

double		GetPotentialResponse_ModuleMgr(EarObjectPtr data, double potential);

EarObjectPtr	GetProcess_ModuleMgr(EarObjectPtr data, char *processSpecifier);

double		GetRestingResponse_ModuleMgr(EarObjectPtr data);

DatumPtr	GetSimulation_ModuleMgr(EarObjectPtr data);

UniParPtr	GetUniParPtr_ModuleMgr(EarObjectPtr data, char *parName);

ModulePtr	Init_ModuleMgr(char *name);

void		LinkGlueRoutine_ModuleMgr(void);

void *		NoFunction_ModuleMgr(void);

void *		NullFunction_ModuleMgr(void);

BOOLN		PrintPars_ModuleMgr(EarObjectPtr data);

BOOLN		PrintSimParFile_ModuleMgr(EarObjectPtr data);

BOOLN		ReadPars_ModuleMgr(EarObjectPtr data, char *fileName);

BOOLN		ReadSimParFile_ModuleMgr(EarObjectPtr data, char *simParFileName);

BOOLN		RunProcess_ModuleMgr(EarObjectPtr data);

BOOLN		RunModel_ModuleMgr_Null(EarObjectPtr data);

void		SetDefault_ModuleMgr(ModulePtr module, void *(* DefaultFunc)(void));

BOOLN		SetNull_ModuleMgr(ModulePtr module);

BOOLN		SetPar_ModuleMgr(EarObjectPtr data, char *parName, char *value);

BOOLN		SetRealArrayPar_ModuleMgr(EarObjectPtr data, char *name, int index,
			  double value);

BOOLN		SetRealPar_ModuleMgr(EarObjectPtr data, char *name, double value);

void *		TrueFunction_ModuleMgr(void);

__END_DECLS

#endif
