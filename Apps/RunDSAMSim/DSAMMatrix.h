/**********************
 *
 * File:		DSAMMatrix.h
 * Purpose:		Utilities to set and handle the arguments usingmyArgc and
 *				myArgv.
 * Comments:	This was create for use by the scripting language interface
 *				RunDSAMSim.
 * Author:		L. P. O'Mard
 * Created:		18 Feb 2003
 * Updated:		
 * Copyright:	(c) 2003, University of Essex
 *
 *********************/

#ifndef	_DSAMMatrix_H
#define _DSAMMatrix_H	1

/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

#define	NUM_BASE_ARGUMENTS			5
#define PARAMETER_STR_DELIMITERS	" ,\t"
#define	DSAMMAT_VERSION				"DSAMMatrix: 1.1.0"
#define	DSAMMAT_NUM_PARS			2		/* No. of application parameters. */
#define	PROGRAM_NAME				"RunDSAMSim"

/******************************************************************************/
/*************************** enum Definitions *********************************/
/******************************************************************************/

enum {

	SIM_FILE,
	PARAMETER_OPTIONS,
	DIAG_MODE,
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

	DSAMMATRIX_NUMBEROFRUNS,
	DSAMMATRIX_SEGMENTDURATION

} DSAMMatrixParSpecifier;

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

void	Free_DSAMMatrix(void);

BOOLN	InitArgv_DSAMMatrix(int argc);

EarObjectPtr	RunSimulation_DSAMMatrix(char *programName, char *simFile,
				  char *diagMode, char *parameterOptions, double *inputData,
				  uShort numChannels, ChanLen length, double dt,
				  BOOLN staticTimeFlag, double outputTimeOffset);

BOOLN	SetArgvString_DSAMMatrix(int index, char *string, int size);

BOOLN	SetArgStrings_DSAMMatrix(char *programName, char *simFile,
		  char *diagMode, char *parameterOptions);

int		SetParameterOptionArgs_DSAMMatrix(int indexStart,
		  char *parameterOptions, BOOLN countOnly);

BOOLN	SetNumberOfRuns_DSAMMatrix(int theNumberOfRuns);

BOOLN	SetSegmentDuration_DSAMMatrix(double theSegmentDuration);

__END_DECLS

#endif
