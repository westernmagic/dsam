/******************
 *		
 * File:		ExtRunThreadedProc.h
 * Purpose: 	This module runs a processes using threads of possible.
 * Comments:	It was deceided to create this module rather than creating
 *				a replacement ExecuteSimulation routine.
 * Author:		L. P. O'Mard
 * Created:		30 Sep 2004
 * Updated:		
 * Copyright:	(c) 2004, CNBH, University of Essex
 *
 ******************/

#ifndef	_EXTRUNTHREADEDPROC_H
#define _EXTRUNTHREADEDPROC_H	1

#include "GeSignalData.h"
#include "GeEarObject.h"

/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Class definitions ********************************/
/******************************************************************************/

class RunThreadedProc {

	int		numThreads;

  public:
	RunThreadedProc(void);

	bool	PreThreadProcessInit(EarObjectPtr data);
	bool	RunThreadEnabled(EarObjectPtr data);
	bool	RunProcess(EarObjectPtr data);
	void	SetNumThreads(int theNumThreads);

};

/******************************************************************************/
/*************************** External variables *******************************/
/******************************************************************************/

extern RunThreadedProc	*runThreadedProc;

/******************************************************************************/
/*************************** Subroutine declarations **************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Call back prototypes *****************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

BOOLN	RunProcess_RunThreadedProc(EarObjectPtr data);

__END_DECLS

#endif
