/**********************
 *
 * File:		PaUtilities.h
 * Purpose:		This module contains various useful utilities for parallel
 *				processing.
 * Comments:	
 * Author:		L. P. O'Mard
 * Created:		18 Mar 1997
 * Updated:		
 * Copyright:	(c) 1997, University of Essex
 *
 **********************/

#ifndef _PAUTILITIES_H
#define _PAUTILITIES_H 1

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

BOOLN	ProcessBatch_MPI_Utility(EarObjectPtr audModel, BOOLN (* SetPars)(int),
		  void (* CalcResult)(EarObjectPtr, int), int numSettings);

__END_DECLS

#endif
