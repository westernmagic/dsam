/**********************
 *
 * File:		UtAccumulate.h
 * Purpose:		This general utility accumulates its input signal into it
 *				output signal.
 * 				It adds to previous data if the output signal has already been
 * 				initialised and the data->updateProcessFlag is set to FALSE.
 * 				Otherwise it will overwrite the old signal or create a new
 *				signal as required.
 * 				The data->updateProcessFlag facility is useful for repeated
 *				runs.  It is set to FALSE before the routine returns.
 * Comments:	Written using ModuleProducer version 1.8.
 *				It does not have any parameters.
 *				06-09-96 LPO: Now can accumulate output from more than one
 *				EarObject.
 *				27-01-99 LPO: 
 * Author:		L. P. O'Mard
 * Created:		Dec 21 1995
 * Updated:		27 Jan 1998
 * Copyright:	(c) 1999, University of Essex.
 *
 *********************/

#ifndef _UTACCUMULATE_H
#define _UTACCUMULATE_H 1

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

BOOLN	CheckData_Utility_Accumulate(EarObjectPtr data);

BOOLN	Process_Utility_Accumulate(EarObjectPtr data);

__END_DECLS

#endif
