/**************
 *
 * File:		UtBandwidth.c
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
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG_H */

#include "GeCommon.h"
#include "UtBandwidth.h"

/******************************************************************************/
/****************************** Global Variables ******************************/
/******************************************************************************/

NameSpecifier	bandwidthModeList[] = {

					{"ERB",			BANDWIDTH_ERB },
					{"CAT",			BANDWIDTH_CAT },
					{"CUSTOM_ERB",	BANDWIDTH_CUSTOM_ERB },
					{"GUINEA_PIG",	BANDWIDTH_GUINEA_PIG },
					{"USER",		BANDWIDTH_USER },
					{"NONLINEAR",	BANDWIDTH_NONLINEAR },
					{"INTERNAL_DYNAMIC",	BANDWIDTH_INTERNAL_DYNAMIC },
					{"INTERNAL_STATIC",	BANDWIDTH_INTERNAL_STATIC },
					{"",			BANDWIDTH_NULL }

				};

/******************************************************************************/
/****************************** Subroutines & functions ***********************/
/******************************************************************************/

/******************************** GetMode *************************************/

/*
 * This function sets the "Bandwidth" mode to be used by the
 * gamma tone filter routines, or whatever wishes to use it.
 * The band width mode defines the function for calculating the gamma tone
 * filter band width 3 dB down.
 * If the 'INTERNAL_STATIC' mode is used, then the bandwidth function will
 * already have been set, so the value needs to be preserved.
 * It returns FALSE if it fails.
 */

BOOLN
SetMode_Bandwidth(BandwidthModePtr modePtr, char *modeName)
{
	static const char *funcName = "SetMode_Bandwidth";
	int		specifier;
	double (* Func)(BandwidthModePtr, double);
	BandwidthMode modeList[] = {
			
		{ BANDWIDTH_ERB, 		K1_ERB, K2_ERB, BandwidthFromF_Bandwith},
		{ BANDWIDTH_CAT, 		K1_ERB, K2_ERB, BandwidthFromF_Bandwith},
		{ BANDWIDTH_CUSTOM_ERB, K1_ERB, K2_ERB, BandwidthFromF_Bandwith},
		{ BANDWIDTH_GUINEA_PIG, K1_ERB, K2_ERB, BandwidthFromF_Bandwith},
		{ BANDWIDTH_USER, 		K1_ERB, K2_ERB, NULL},
		{ BANDWIDTH_NONLINEAR,	K1_ERB, K2_ERB, BandwidthFromF_Bandwith},
		{ BANDWIDTH_INTERNAL_DYNAMIC,	K1_ERB, K2_ERB, NULL},
		{ BANDWIDTH_INTERNAL_STATIC,	K1_ERB, K2_ERB, NULL},
		{ BANDWIDTH_NULL,		0.0, 0.0, NULL}
				
	};

	if (modePtr == NULL) {
		NotifyError("%s: Mode pointer is not set ('%s').", funcName, modeName);
		return(FALSE);
	}
	if ((specifier = Identify_NameSpecifier(modeName, bandwidthModeList)) ==
	  BANDWIDTH_NULL) {
		NotifyError("%s: Unknown bandwidth  mode (%s).", funcName, modeName);
		return(FALSE);
	}
	if (specifier == BANDWIDTH_INTERNAL_STATIC)
		Func = modePtr->Func;
	*modePtr = modeList[specifier];
	if (specifier == BANDWIDTH_INTERNAL_STATIC)
		modePtr->Func = Func;
	return(TRUE);

}

/******************************** BandwidthFromF ******************************/

/*
 * This a general routine for calculating the bandwiths.  It calls all the other
 * routines.  Using the appropriate parameters.
 * It assumes that the 'BandwidthMode' structure has been correctly initialised.
 */

double
BandwidthFromF_Bandwith(BandwidthModePtr p, double theFrequency)
{
	static const char *funcName = "BandwidthFromF_Bandwith";
	if (p == NULL) {
		NotifyError("%s: BandwidthMode structure not initialised.", funcName);
		return(-1.0);
	}
	switch (p->specifier) {
	case BANDWIDTH_ERB:
		return(ERBFromF_Bandwidth(theFrequency));
	case BANDWIDTH_CAT:
		return(CatFromF_Bandwidth(theFrequency));
	case BANDWIDTH_CUSTOM_ERB:
		return(CustomERBFromF_Bandwidth(theFrequency, p->bwMin, p->quality));
	case BANDWIDTH_GUINEA_PIG:
		return(GuineaPigFromF_Bandwidth(theFrequency));
	case BANDWIDTH_NONLINEAR:
		return(NonLinearFromF_Bandwidth(theFrequency));
	default:
		NotifyError("%s: Bandwidth mode not implemented (%d).", funcName,
		  p->specifier);	
	}
	return(-1.0);
}

/******************************** NonLinearFromF ******************************/

/*
 * Calculate the band width of a guinea-pig's filter at a characteristic 
 * frequency, f.
 * At present it uses a very rough model fit to CFs at 18 kHz, 8 kHz and 300 Hz.
 * It was written to be used with the DRNL filter.
 * This routine has been fixed to ensure that once the maximum frequency for the
 * equation has been reached, the bandwidth remains the same.
 */

#define	BANDWIDTH(F)	(-1709.0 + 279.14 * log((F)))

double
NonLinearFromF_Bandwidth(double f)
{
	double	bandwidth;

	bandwidth = BANDWIDTH(f);
	return((bandwidth > 0.0)? bandwidth: f / 10.0);
	
}

#undef	BANDWIDTH

/******************************** GuineaPigFromF ******************************/

/*
 * Calculate the band width of a guinea-pig's filter at a characteristic 
 * frequency, f.
 * At present Alan Palmer's data (unpublished) is used.
 * N.B. The parameters have been divided by 2 to convert from 10 dB down
 * bandwidth to 3 dB down bandwidth, as used by the gamma tone filter algorithm.
 */

double
GuineaPigFromF_Bandwidth(double f)
{
	double	ff;
	
	ff = f * f;
	return(207.2 + 0.0553 * f - 3.254e-7 * ff + 2.501e-11 * (ff * f));
	
}

/******************************** CatFromF ************************************/

/*
 * Calculate the band width of a cat's filter at a characteristic 
 * frequency, f.
 * At present Kiang's data (Pickles, 2ndEd, figure 4.5) is used.
 * N.B. The parameters have been divided by 2 to convert from 10 dB down
 * bandwidth to 3 dB down bandwidth, as used by the gamma tone filter algorithm.
 */

double
CatFromF_Bandwidth(double f)
{
	double	ff;
	
	ff = f * f;
	return(112.8 + 0.0598 * f - 4.159e-7 * ff + 3.599e-11 * (ff * f));
	
}

/******************************** ERBFromF ************************************/

/*
 * Calculate the erb at frequency f (new M&G formula).
 */

double
ERBFromF_Bandwidth( double theFrequency )
{
	return ( K1_ERB * ( K2_ERB * theFrequency + 1.0 ) );

}

/******************************** CustomERBFromF ******************************/

/*
 * Calculate the erb at frequency f (new M&G formula), allowing the change of
 * the constants.
 * This routine uses the equation in a different form from ERBFromF.  In this
 * form the orignal values are bwMin = 24.7, 1/quality = 0.108.
 * This routine expects checks for quality = 0.0 to be made eslewhere.
 */

double
CustomERBFromF_Bandwidth( double theFrequency, double bwMin, double quality )
{
	return ( bwMin + 1.0 / quality * theFrequency );

}

/******************************** FFromERB ************************************/

/*
 * Calculate the frequency at the associated ERB bandwidth (new M&G formula).
 */

double
FFromERB_Bandwidth( double theERB)
{
	return ( (theERB / K1_ERB - 1.0) / K2_ERB );

}

/****************************** ERBRateFromF **********************************/

/*
 * Calculate the ERB rate (ERBs per Hz) at the specified frequency.
 * It assumes that no negative centre frequencies are used (They could cause
 * problems.
 */

double
ERBRateFromF_Bandwidth(double theFrequency)
{
	return( K3_ERB * log10(K2_ERB * theFrequency + 1.0) );

}

/****************************** FFromERBRate **********************************/

/*
 * Calculate the frequency (ERBs per Hz) at the specified ERB rate.
 */

double
FFromERBRate_Bandwidth(double theERBRate)
{
	return( (pow(10.0, theERBRate / K3_ERB) - 1.0) / K2_ERB);

}

