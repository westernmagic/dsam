/**************
 *
 * File:		UtBandwidth.h
 * Purpose:		This module contains the bandwidth functions, including the
 *				equivalent rectangular bandwidth (ERB) freqency functions.
 * Comments:	13-10-98 LPO: Implemented changes to introduce the
 *				'Custom_ERB' mode.
 * Author:		L. P. O'Mard.
 * Created:		12 Jul 1993
 * Updated:		13 Oct 1998
 * Copyright:	(c) 1998, University of Essex
 *
 **************/

#ifndef	_UTBANDWIDTH_H
#define _UTBANDWIDTH_H	1

#include "UtNameSpecs.h"
 
/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

#define BANDWIDTH_NUM_PARS				4
#define BANDWIDTH_NUM_CONSTANT_PARS		1

#define	K1_ERB		24.7		/* k1 constant. */
#define K2_ERB		4.37e-3		/* K2 constant, frequency in Hz. */
#define K3_ERB		21.4		/* K3 constant, for ERB rate calc. */

/******************************************************************************/
/****************************** Macro definitions *****************************/
/******************************************************************************/


/* Position of cf unit; from Liberman's map.
 * This has been sited here, rather than in the CFList module, so that it
 * is with with the other experimentally derived BM code.
 */

#define BANDWIDTH_CARNEY_X(CF)	(11.9e-3 * log10(0.80 + (CF) / 456.0))

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

typedef enum {

	BANDWIDTH_ERB,
	BANDWIDTH_CAT,
	BANDWIDTH_CUSTOM_ERB,
	BANDWIDTH_GUINEA_PIG,
	BANDWIDTH_USER,
	BANDWIDTH_NONLINEAR,
	BANDWIDTH_INTERNAL_DYNAMIC,
	BANDWIDTH_INTERNAL_STATIC,
	BANDWIDTH_DISABLED,
	BANDWIDTH_NULL
	
} BandwidthModeSpecifier;

typedef enum {

	BANDWIDTH_PAR_MODE,
	BANDWIDTH_PAR_MIN,
	BANDWIDTH_PAR_QUALITY,
	BANDWIDTH_PAR_BANDWIDTH

} BandwidthParSpecifier;

typedef struct BandwidthMode {

	BandwidthModeSpecifier specifier;
	double	bwMin;
	double	quality;
	double	(* Func)(struct BandwidthMode *, double);

} BandwidthMode, *BandwidthModePtr;

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Function Prototypes ******************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

/* formulae.c calls: ERB as a function of frequency etc, Glasberg and Moore 1990
 * versions only.
 */
 
double	BandwidthFromF_Bandwith(BandwidthModePtr p, double theFrequency);

double	CatFromF_Bandwidth(double f);

double	CustomERBFromF_Bandwidth(double x, double bwMin, double quality);

double	ERBFromF_Bandwidth(double x);

double	FFromERB_Bandwidth(double x);

double	ERBRateFromF_Bandwidth(double theFrequency);

double	FFromERBRate_Bandwidth(double theERBRate);

double	GuineaPigFromF_Bandwidth(double f);

double	NonLinearFromF_Bandwidth(double f);

NameSpecifier *	ModeList_Bandwidth(int index);

BOOLN	SetMode_Bandwidth(BandwidthModePtr modePtr, char *modeName);

__END_DECLS

#endif
