/**********************
 *
 * File:		MatMainApp.h
 * Purpose:		This routine contains the DSAM matlab/octave specific class
 *				derived from the MainApp class for handling the DSAM extensions
 *				library.
 * Comments:	This was create for use by the scripting language interface
 *				RunDSAMSim.
 *				It was revised from the previous DSAMMatrix code module.
 * Author:		L. P. O'Mard
 * Created:		07 Jan 2004
 * Updated:
 * Copyright:	(c) 2004, L. P. O'Mard
 *
 *********************/

#ifndef	_MATMAINAPP_H
#define _MATMAINAPP_H	1

#include <ExtMainApp.h>

/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

#define	DSAMMAT_NUM_PARS			6		/* No. of application parameters. */
#define	DSAMMAT_AUTO_INTERLEAVE_LEVEL	-1
#define DSAMMAT_COMMAND_PREFIX	wxT("SERVER_")
#define	PROGRAM_NAME				wxT("RunDSAMSim")

/******************************************************************************/
/*************************** enum Definitions *********************************/
/******************************************************************************/

enum {

	SIM_FILE,
	PARAMETER_OPTIONS,
	INPUT_SIGNAL,
	INFO_STRUCT

};

/******************************************************************************/
/*************************** Macro definitions ********************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

typedef enum {

	DSAMMATRIX_AUTONUMRUNSMODE,
	DSAMMATRIX_NUMBEROFRUNS,
	DSAMMATRIX_SEGMENTDURATION,
	DSAMMATRIX_SERVERMODE,
	DSAMMATRIX_SERVERHOST,
	DSAMMATRIX_SERVERPORT

} DSAMMatrixParSpecifier;

/******************************************************************************/
/*************************** Class definitions ********************************/
/******************************************************************************/

/********************************** Pre-references ****************************/

class MainApp;

/*************************** MatMainApp ***************************************/

class MATLIB_API MatMainApp: public MainApp {

	UniParListPtr	parList;
	wxInitializer initializer;
	MatInSignal	*inputSignal;

  public:
	bool	updateProcessVariablesFlag;
	int		numberOfRuns;
	int		autoNumRunsMode;
	int		serverMode;
	Float	segmentDuration;

	MatMainApp(wxChar *programName, const wxChar *simFile,
	  const wxChar * parameterOptions, MatInSignal *inputSignal,
	  bool setLocalNotifyFlag = true);
	~MatMainApp(void);

	bool	AutoSetNumberOfRuns(void);
	EarObjectPtr	GetSimProcess(void);
	bool	InitRun(void);
	int		Main(void);
	bool	RunSimulationLocal(void);
	bool	RunSimulationRemote(void);
	bool	SetArgStrings(wxChar *programName, const wxChar *simFile,
			  const wxChar *parameterOptions);

};

/******************************************************************************/
/*************************** Preference type definitions **********************/
/******************************************************************************/

/******************************************************************************/
/*************************** External Variables *******************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Global Subroutines *******************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

MATLIB_API void	DPrint_MatMainApp(const WChar *format, va_list args);

BOOLN	InitApp_MatMainApp(void);

MATLIB_API void	Notify_MatMainApp(const wxChar *message, CommonDiagSpecifier type);

BOOLN	RegisterUserModules_MatMainApp(void);

EarObjectPtr	RunSimulation_MatMainApp(void);

EarObjectPtr	RunSimulationLocal_MatMainApp(void);

BOOLN	SetAutoNumRunsMode_MatMainApp(wxChar *theAutoNumRunsMode);

BOOLN	SetArgvString_MatMainApp(int index, wxChar *string, int size);

BOOLN	SetArgStrings_MatMainApp(wxChar *programName, wxChar *simFile,
		  wxChar *parameterOptions);

void	SetInputProcessData_MatMainApp(EarObjectPtr process, ChanLen
		  signalLength, Float *data);

int		SetParameterOptionArgs_MatMainApp(int indexStart,
		  wxChar *parameterOptions, BOOLN countOnly);

BOOLN	SetNumberOfRuns_MatMainApp(int theNumberOfRuns);

BOOLN	SetSegmentDuration_MatMainApp(Float theSegmentDuration);

BOOLN	SetServerHost_MatMainApp(const wxChar *theServerHost);

BOOLN	SetServerMode_MatMainApp(wxChar *theServerMode);

BOOLN	SetServerPort_MatMainApp(int theServerPort);

BOOLN	SetUniParList_MatMainApp(UniParListPtr *parList);

__END_DECLS

#endif
