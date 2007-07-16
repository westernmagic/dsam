/**********************
 *
 * File:		feval.h
 * Purpose:		This program runs a series of test on a filter simulation.
 * Comments:	
 * Author:		L. P. O'Mard
 * Created:		28 Jun 2000
 * Updated:		
 * Copyright:	(c) 2000, University of Essex
 *
 *********************/

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define	PROGRAM_NAME			wxT("feval")
#ifndef FEVAL_VERSION
#	define	FEVAL_VERSION		wxT("?.?.?")
#endif

#define	FEVAL_NUM_PARS			1	/* No. of main application pars. */
#define	FEVAL_TEST_MOD_PREFIX	wxT("FT_")

#define DEFAULT_ERRORS_MODE		wxT("screen")
#define	LOCK_FILE				wxT("."PROGRAM_NAME"_LCK")

#ifndef FEVAL_DATA_INSTALL_DIR
#	define	FEVAL_DATA_INSTALL_DIR	wxT(".")	/* -find DSAM help files, etc.*/
#endif

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

typedef enum {

	FEVAL_FILELOCKINGMODE

} fevalParSpecifier;

/******************************************************************************/
/*************************** External Variables *******************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Subroutine declarations **************************/
/******************************************************************************/

BOOLN	Init(void);

NameSpecifier *	GetPanelList(int index);

void	PrintInitialDiagnostics(void);

void	PrintUsage(void);

BOOLN	ProcessOptions(WChar option, WChar **argv, int *optInd);

BOOLN	ReadTuningCurvePars(FILE *fp);

BOOLN	SetAppInterfacePars(void);

BOOLN	SetFileLockingMode(WChar *theFileLockingMode);

BOOLN	SetLockFile(BOOLN on);

BOOLN	SetUniParList(UniParListPtr *parList);
