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
	double	*c;
	double	*state;

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
				  int numBands, double *bands, double *desired, double *weights,
				  int type);


__END_DECLS

#endif
