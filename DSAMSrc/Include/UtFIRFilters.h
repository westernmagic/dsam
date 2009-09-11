/**********************
 *
 * File:		UtFIRFilter.h
 * Purpose:		This contains the structures for the FIR Filter.
 * Comments:	This module is to be put into UtFilters.
 * Author:		L. P. O'Mard.
 * Created:		04 Dec 2000
 * Updated:
 * Copyright:	(c) 2000, CNBH University of Essex
 *
 *********************/

#ifndef _UTFIRFILTERS_H
#define _UTFIRFILTERS_H 1

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef struct {

	int		numChannels;
	int		m;
	Float	*c;
	Float	*state;

} FIRCoeffs, *FIRCoeffsPtr;

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

void	FIR_FIRFilters(EarObjectPtr data, FIRCoeffsPtr p);

void	FreeFIRCoeffs_FIRFilters(FIRCoeffsPtr *p);

FIRCoeffsPtr	InitFIRCoeffs_FIRFilters(int numChannels, int numTaps,
				  int numBands, Float *bands, Float *desired, Float *weights,
				  int type);

void	ProcessBuffer_FIRFilters(EarObjectPtr data, FIRCoeffsPtr p);

__END_DECLS

#endif
