/**********************
 *
 * File:		UtSimScript.h
 * Purpose:		This is the header file for the simulation script reading
 *				module.
 * Comments:	Written using ModuleProducer version 1.9 (May 27 1996).
 *				21-08-97 LPO: Removed rampInterval parameter - use new ramp
 *				module.
 *				26-11-97 LPO: Using new GeNSpecLists module for 'operationMode'
 *				parameter.
 *				01-05-98 LPO: The GetInSignalRef_EarObject routine is now used
 *				to ensure that the input signals are correctly transferred to
 *				the first process, and that the numInputs count is updated
 *				At the moment the input signals to a simulation script
 *				are always re-connected each time the process is run.  This is
 *				because to add the connection to the supplier/customer lists
 *				the process would need the previous process' EarObjectPtr.
 *				24-09-1998 LPO: Introduced the parameter file mode.  If it is
 *				'on' then parameters for the simulation will be read from a
 *				file with the same name as the simulation script, but
 *				with the ".spf" extension.  A simulation parameter file (SPF)
 *				file will be created if one does not already exist.
 *				30-09-98 LPO: I have introduced the uniParLockFlag field which
 *				is set to disallow command line changes of the universal
 *				parameters when a uniParList file is being used.
 *				10-12-98 LPO: Introduced handling of NULL parLists, i.e. for 
 *				modules with no parameters.
 *				02-02-99 LPO: The 'SetUniParValue_' has been moved to the
 *				UtDatum module.
 *				25-05-99 LPO: Introduced the 'parsFilePath' field.  All modules
 *				now read their parameter files using a path relative to the
 *				path of the simulation script file.
 *				03-06-99 LPO: Introduced 'SetUniParValue_' routine, so that it
 *				can be called from 'GeModuleMgr' module.
 *				23-08-99 LPO: The 'NotifyError_' routine was not calling the
 *				CheckInitErrorsFile_Common" routine so the 'dSAM.errorsFile'
 *				field was not set and was causing a crash.
 * Author:		L. P. O'mard
 * Created:		19 Jan 1996
 * Updated:		23 Aug 1999
 * Copyright:	(c) 1999, University of Essex
 *
 *********************/

#ifndef _UTSIMSPEC_H
#define _UTSIMSPEC_H 1

#include "GeUniParMgr.h"
#include "UtNameSpecs.h"
#include "UtDatum.h"

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define UTILITY_SIMSCRIPT_NUM_PARS			3
#define	MAX_IDENTIFIERS						20
#define	NULL_STRING							""
#define SIMSCRIPT_SIMPARFILE_DIVIDER		"<<>>"
#define SIMSCRIPT_SIMPARFILE_SDI_DIVIDER	"<()>"
#define SIMSCRIPT_DISABLED_MODULE_CHAR		'@'

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef enum {

	UTILITY_SIMSCRIPT_OPERATIONMODE,
	UTILITY_SIMSCRIPT_PARFILEPATHMODE,
	UTILITY_SIMSCRIPT_SIMULATION

} SimScriptParSpecifier;

typedef enum {

	UTILITY_SIMSCRIPT_PARFILEPATHMODE_RELATIVE,
	UTILITY_SIMSCRIPT_PARFILEPATHMODE_PATH,
	UTILITY_SIMSCRIPT_PARFILEPATHMODE_NULL

} UtilityParFilePathModeSpecifier;

typedef enum {

	UTILITY_SIMSCRIPT_SIM_FILE,
	UTILITY_SIMSCRIPT_SPF_FILE,
	UTILITY_SIMSCRIPT_UNKNOWN_FILE

} SimFileTypeSpecifier;

typedef struct {

	ParameterSpecifier	parSpec;

	BOOLN	operationModeFlag;
	BOOLN	parFilePathModeFlag;
	int		operationMode;
	int		parFilePathMode;
	DatumPtr	simulation;
	

	/* Private members */
	UniParListPtr	parList;
	NameSpecifier	*parFilePathModeList;
	SimFileTypeSpecifier	simFileType;
	char	simFileName[MAX_FILE_PATH];
	char	parsFilePath[MAX_FILE_PATH];
	int		lineNumber;
	FILE	*fp;
	SymbolPtr	symList;
	DatumPtr	*simPtr;
	DynaListPtr	subSimList;
	DynaBListPtr	*labelBListPtr, labelBList;

} SimScript, *SimScriptPtr;

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

extern SimScriptPtr	simScriptPtr;

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

BOOLN	CheckData_Utility_SimScript(EarObjectPtr data);

BOOLN	CheckPars_Utility_SimScript(void);

BOOLN	Free_Utility_SimScript(void);

BOOLN	FreeSimulation_Utility_SimScript(void);

char *	GetFilePath_Utility_SimScript(char *filePath);

SimFileTypeSpecifier	GetSimFileType_Utility_SimScript(char *suffix);

DatumPtr	GetSimulation_Utility_SimScript(void);

DatumPtr * GetSimScriptSimulation_Utility_SimScript(DatumPtr pc);

UniParListPtr	GetUniParListPtr_Utility_SimScript(void);

BOOLN	Init_Utility_SimScript(ParameterSpecifier parSpec);

BOOLN	InitialiseEarObjects_Utility_SimScript(void);

void	InitKeyWords_Utility_SimScript(void);

BOOLN	InitParFilePathModeList_Utility_SimScript(void);

BOOLN	InitSimulation_Utility_SimScript(DatumPtr simulation);

char *	InitString_Utility_SimScript(char *string);

void	NotifyError_Utility_SimScript(char *format, ...);

BOOLN	PrintPars_Utility_SimScript(void);

BOOLN	PrintSimParFile_Utility_SimScript(void);

BOOLN	Process_Utility_SimScript(EarObjectPtr data);

DatumPtr	Read_Utility_SimScript(FILE *fp);

BOOLN	ReadPars_Utility_SimScript(char *fileName);

BOOLN	ReadSimParFile_Utility_SimScript(FILE *fp);

BOOLN	ReadSimParFileOld_Utility_SimScript(FILE *fp);

BOOLN	ReadSimScript_Utility_SimScript(FILE *fp);

BOOLN	ReadSimScriptOld_Utility_SimScript(FILE *fp);

BOOLN	InitModule_Utility_SimScript(ModulePtr theModule);

BOOLN	SetLabelBListPtr_Utility_SimScript(DynaBListPtr *labelBListPtr);

BOOLN	SetOperationMode_Utility_SimScript(char *theOperationMode);

BOOLN	SetParFilePathMode_Utility_SimScript(char *theParFilePathMode);

BOOLN	SetParsFilePath_Utility_SimScript(char * parsFilePath);

BOOLN	SetParsPointer_Utility_SimScript(ModulePtr theModule);

BOOLN	SetPars_Utility_SimScript(DatumPtr simulation,
		  char *operationMode, char *parFilePathMode);

BOOLN	SetProcessSimPtr_Utility_SimScript(EarObjectPtr data);

BOOLN	SetSimFileName_Utility_SimScript(char * simFileName);

BOOLN	SetSimFileType_Utility_SimScript(SimFileTypeSpecifier simFileType);

BOOLN	SetSimulation_Utility_SimScript(DatumPtr theSimulation);

BOOLN	SetSimUniParValue_Utility_SimScript(char *parName, char *parValue);

BOOLN	SetUniParList_Utility_SimScript(void);

BOOLN	SetUniParValue_Utility_SimScript(char *parName, char *parValue);

__END_DECLS

#endif
