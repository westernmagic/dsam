/******************
 *		
 * File:		ExtProcThread.h
 * Purpose: 	Process thread class module.
 * Comments:	
 * Author:		L. P. O'Mard
 * Created:		23 Sep 2004
 * Updated:		
 * Copyright:	(c) 2004, CNBH, University of Essex
 *
 ******************/

#ifndef	_EXTPROCTHREAD_H
#define _EXTPROCTHREAD_H	1

/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

/*#define DEBUG 1*/

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Class definitions ********************************/
/******************************************************************************/

/*************************** ProcThread ***************************************/

class ProcThread: public wxThread {
	int		*threadCount, index, offset, numChannels;
	wxCondition	*myCondition;
	wxMutex		*myMutex;

  public:

	ProcThread(int theIndex, int theOffset, int theNumChannels, wxMutex *mutex,
	  wxCondition *condition, int *theThreadCount);

	virtual void OnExit();

	int		GetIndex(void)		{ return index; }
	int		GetOffset(void)		{ return offset; }
	EarObjectPtr	ConfigProcess(EarObjectPtr theDataPtr);
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
