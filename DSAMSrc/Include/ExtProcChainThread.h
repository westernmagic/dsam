/******************
 *		
 * File:		ExtProcChainThread.h
 * Purpose: 	Chain Process thread class module.  It processes a chain of
 *				equal channelled processes.
 * Comments:	
 * Author:		L. P. O'Mard
 * Created:		23 Sep 2004
 * Updated:		
 * Copyright:	(c) 2004, CNBH, University of Essex
 *
 ******************/

#ifndef	_EXTPROCCHAINTHREAD_H
#define _EXTPROCCHAINTHREAD_H	1

/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Class definitions ********************************/
/******************************************************************************/

/*************************** ProcThread ***************************************/

class ProcChainThread: public ProcThread {
	uInt	origNumChannels;
	wxCondition	*myCondition;
	wxMutex		*myMutex;
	DatumPtr	simulation;

  public:

	ProcChainThread(int theIndex, int offset, int numChannels,
	  DatumPtr start, wxMutex *mutex, wxCondition *condition,
	  int *theThreadCount);

	virtual void *Entry();
	virtual void OnExit();

};

/******************************************************************************/
/*************************** External variables *******************************/
/******************************************************************************/

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


__END_DECLS

#endif
