/******************
 *
 * File:		StdSimThread.h
 * Purpose:		This file contains the standard messaging routine prototypes
 *				for both the GUI and non-GUI message libraries.
 * Comments:	It was take from the GeCommon module.
 * Authors:		L. P. O'Mard
 * Created:		21 Feb 1999
 * Updated:		
 * 
 ******************/

#ifndef	_STDSIMTHREAD_H
#define _STDSIMTHREAD_H	1

#include <stdarg.h>

/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Macro definitions ********************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

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

BOOLN	TestDestroy_SimThread(void);

__END_DECLS

#endif
