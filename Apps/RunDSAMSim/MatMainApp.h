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
 * Copyright:	(c) 2004, CNBH, University of Essex
 *
 *********************/

#ifndef	_MATMAINAPP_H
#define _MATMAINAPP_H	1

#include "ExtMainApp.h"

/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

#define	NUM_BASE_ARGUMENTS			3
#define	DSAMMAT_VERSION				"MatMainApp: 1.1.0"
#define	DSAMMAT_NUM_PARS			6		/* No. of application parameters. */
#define	DSAMMAT_AUTO_INTERLEAVE_LEVEL	-1
#define	PROGRAM_NAME				"RunDSAMSim"

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

#define MATMAINAPP_COMMAND_PREFIX	"SERVER_"

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

class MatMainApp: public MainApp {

	bool	staticTimeFlag;
	int		numChannels, interleaveLevel;
	ChanLen	length;
	double	*inputData, dt, outputTimeOffset;
	IPCClient	*myClient;
	UniParListPtr	parList;
	EarObjectPtr	inputProcess;
	wxInitializer initializer;

  public:
	bool	updateProcessVariablesFlag;
	char	serverHost[MAXLINE];
	int		numberOfRuns;
	int		autoNumRunsMode;
	int		serverMode;
	int		serverPort;
	double	segmentDuration;

	MatMainApp(char *programName, char *simFile, char *parameterOptions,
	  double *theInputData, int theNumChannels, int theInterleaveLevel,
	  ChanLen theLength, double theDt, bool theStaticTimeFlag,
	  double theOutputTimeOffset);
	~MatMainApp(void);

	bool	AutoSetNumberOfRuns(ChanLen inputLength, double dt);
	EarObjectPtr	GetSimProcess(void);
	bool	InitInputEarObject(ChanLen segmentLength);
	bool	InitRun(void);
	int		Main(void);
	bool	RunSimulationLocal(void);
	bool	RunSimulationRemote(void);
	bool	SetArgStrings(char *programName, char *simFile,
			  char *parameterOptions);
	void	SetInputProcessData(EarObjectPtr process, ChanLen signalLength,
			  double *data);

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

BOOLN	InitApp_MatMainApp(void);

BOOLN	RegisterUserModules_MatMainApp(void);

EarObjectPtr	RunSimulation_MatMainApp(void);

EarObjectPtr	RunSimulationLocal_MatMainApp(void);

BOOLN	SetAutoNumRunsMode_MatMainApp(char *theAutoNumRunsMode);

BOOLN	SetArgvString_MatMainApp(int index, char *string, int size);

BOOLN	SetArgStrings_MatMainApp(char *programName, char *simFile,
		  char *parameterOptions);

void	SetInputProcessData_MatMainApp(EarObjectPtr process, ChanLen
		  signalLength, double *data);

int		SetParameterOptionArgs_MatMainApp(int indexStart,
		  char *parameterOptions, BOOLN countOnly);

BOOLN	SetNumberOfRuns_MatMainApp(int theNumberOfRuns);

BOOLN	SetSegmentDuration_MatMainApp(double theSegmentDuration);

BOOLN	SetServerHost_MatMainApp(char *theServerHost);

BOOLN	SetServerMode_MatMainApp(char *theServerMode);

BOOLN	SetServerPort_MatMainApp(int theServerPort);

BOOLN	SetUniParList_MatMainApp(UniParListPtr *parList);

__END_DECLS

#endif
