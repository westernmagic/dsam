/**********************
 *
 * File:		sams.h
 * Purpose:		This program generates a simulation response using the
 *				simulation specification utility.
 * Comments:	22-04-98 LPO:  Amended to allow users to override the stimulus
 *				parameters.
 *				To allow the parameter file to be specified as an option,
 *				flags have to be used to ensure that general parameters set by
 *				options are not overwritten.
 *				28-09-1998 LPO: The simulation is now declared globally, and it
 *				is only initialised if the simulation specification file was
 *				modified since the last time the simulation was initialised.
 *				In this mode the EarObjects are only deallocated at the end of
 *				of the program (by default), or if re-initialisation occurs.
 *				05-10-98 LPO: Removed override parameters, as this is no longer
 *				necessary because of the universal parameter option handling.
 *				07-10-98 LPO: Introduced '-l' option for listings.
 *				09-10-1998 LPO: Introduced the parameter file mode.  If it is
 *				'on' then parameters for the simulation will be read from a
 *				file: A simulation parameter file (SPF) - resource file.
 *				13-10-98 LPO: Added the 'CFLIST' list option.
 *				07-01-99 LPO: Added support for ignoring options with arguments.
 *				26-01-99 LPO: Added overridefunction for printing the parameters
 *				from the simulation manager.
 *				I also removed the dependence on the main parameter file.  The
 *				main parameters will now be set to defaults.
 *				09-02-99 LPO: The main parameters are now only read when the
 *				program is first started.
 *				20-05-99 LPO: I have introduced the "DialogList::
 *				SetUpdateParent" Routine which will allow the changing if the
 *				simulation script and the simulation parameter file to signal
 *				a re-initialisation of the simulation and or parameters.
 *				19-07-99 LPO: The CygWin port of wxwin did not have the program
 *				name as the first argv[] string.  This was corrected in the
 *				version that I am using.
 *				27-07-99 LPO: The "SetSimScriptFile" routine now free's
 *				the "audModel" simulation script, to ensure that the new
 *				script is initialised, and the old one is not used.
 *				18-08-99 LPO: Introduced the file-locking option.
 * Author:		L. P. O'Mard
 * Created:		19 Mar 1995
 * Updated:		18 Aug 1999
 * Copyright:	(c) 1999, University of Essex.
 *
 *********************/

#include <stdlib.h> 
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <string.h>

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define	PROGRAM_NAME				wxT("sams")
#ifndef SAMS_VERSION
#	define	SAMS_VERSION					wxT("?.?.?")
#endif

#define	SAMS_NUM_PARS				3		/* No. of application parameters. */
#define	LOCK_FILE					wxT(".") PROGRAM_NAME wxT("_LCK")
#define SAMS_HELP					wxT("SAMSHelp")
#define SAMS_TUTORIAL_HELP			wxT("SAMSTutorials")

#ifndef SAMS_DATA_INSTALL_DIR
#	define	SAMS_DATA_INSTALL_DIR	wxT(".") /* To find the DSAM help files...*/
#endif

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

typedef enum {

	SAMS_FILELOCKINGMODE,
	SAMS_AUTONUMRUNSMODE,
	SAMS_NUMBEROFRUNS

} SAMSParSpecifier;

/******************************************************************************/
/*************************** Class definitions ********************************/
/******************************************************************************/

/******************************************************************************/
/*************************** External Variables *******************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Subroutine declarations **************************/
/******************************************************************************/

BOOLN	AutoSetNumberOfRuns(void);

BOOLN	Init(void);

BOOLN	PostInitFunc(void);

void	PrintInitialDiagnostics(void);

void	PrintUsage(void);

BOOLN	ProcessOptions(int argc, char **argv, int *optInd);

BOOLN	SetAppInterfacePars(void);

BOOLN	SetAutoNumRunsMode(WChar *theAutoNumRunsMode);

BOOLN	SetFileLockingMode(WChar *theFileLockingMode);

BOOLN	SetLockFile(BOOLN on);

BOOLN	SetNumberOfRuns(int theNumberOfRuns);

BOOLN	SetUniParList(UniParListPtr *parList);
