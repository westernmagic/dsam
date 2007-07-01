/**********************
 *
 * File:		UtCFList.h
 * Purpose:		This file contains the centre frequency management routines.
 * Comments:	The "bandWidth" field is set using the UtBandWidth module.
 *				06-10-98 LPO: The CFList type is now saved as the specifier,
 *				rather than as a string.  The 'minCf', 'maxCF' and 'eRBDensity'
 *				values are now saved with the CFList, so that the parameter list
 *				can always know these values.
 *				12-10-98 LPO: Introduced the 'SetParentCFList_CFList' and the
 *				'parentPtr' so that a CFList can always set the pointer which is
 *				pointing to it.
 *				19-11-98 LPO: Changed code so that a NULL array for the user
 *				modes causes the frequency and bandwidth arrays to have space
 *				allocated for them.  This was needed for the universal
 *				parameters implementation.
 *				26-11-98 LPO: Added the 'oldNumChannels' private parameter so
 *				that the user mode can use the parameters of the old frequency
 *				array if it exists.
 *				17-06-99 LPO: The 'SetIndividualFreq' routine now sets the
 *				CFList's 'updateFlag' field when a frequency is set.  This is
 *				needed because modules, such as 'BM_Carney' need to know if the
 *				bandwidths need recalculating.
 *				06-11-00 LPO: Implented the Greenwood function with the
 *				functions and values provided by Steve D. Holmes.
 * Author:		L. P. O'Mard
 * Created:		12 Jul 1993
 * Updated:		17 Jun 1999
 * Copyright:	(c) 1999, University of Essex
 *
 *********************/

#ifndef	_UTCFLIST_H
#define _UTCFLIST_H	1

#include "UtNameSpecs.h"

/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

#define CFLIST_NUM_PARS					9
#define CFLIST_NUM_CONSTANT_PARS		2
#define CFLIST_DEFAULT_MODE_NAME		wxT("log")
#define CFLIST_DEFAULT_CHANNELS			30
#define CFLIST_DEFAULT_LOW_FREQ			100
#define CFLIST_DEFAULT_HIGH_FREQ		10000
#define CFLIST_DEFAULT_BW_MODE_NAME		wxT("erb")
#define CFLIST_DEFAULT_BW_MODE_FUNC		NULL

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

typedef enum {

	CFLIST_SINGLE_MODE,		/* User single frequency mode. */
	CFLIST_USER_MODE,		/* User centre frequency list mode. */
	CFLIST_ERB_MODE,		/* Freqs. equally spaced on ERB scale. */
	CFLIST_ERBN_MODE,		/* -as above, no. of CF's specifier, not ERB dens.*/
	CFLIST_LOG_MODE,		/* Freqs. spaced on a log10 scale. */
	CFLIST_FOCAL_LOG_MODE,	/* Freqs. spaced on a log10 scale. */
	CFLIST_LINEAR_MODE,		/* Freqs. spaced on a linear scale. */
	CFLIST_CAT_MODE,		/* Freqs. spaced on BM for cats (Greenwood). */
	CFLIST_CHINCHILLA_MODE,	/* - spaced on BM for chinchilla (Greenwood). */
	CFLIST_GPIG_MODE,		/* - spaced on BM for guinea pig (Greenwood). */
	CFLIST_HUMAN_MODE,		/* - spaced on BM for humans (Greenwood). */
	CFLIST_MACAQUEM_MODE,	/* - spaced on BM for macaque monkey (Greenwood). */
	CFLIST_IDENTICAL_MODE,	/* - all frequencies set to the same value. */
	CFLIST_NULL

} CFListSpecifier;

typedef enum {

	CFLIST_DIAG_MODE,
	CFLIST_PARAMETERS_DIAG_MODE,
	CFLIST_DIAG_NULL

} CFListDiagModeSpecifier;

typedef enum {

	CFLIST_CF_DIAGNOSTIC_MODE,
	CFLIST_CF_MODE,
	CFLIST_CF_SINGLE_FREQ,
	CFLIST_CF_FOCAL_FREQ,
	CFLIST_CF_MIN_FREQ,
	CFLIST_CF_MAX_FREQ,
	CFLIST_CF_NUM_CHANNELS,
	CFLIST_CF_ERB_DENSITY,
	CFLIST_CF_FREQUENCIES

} CFListCFParSpecifier;

typedef struct {

	int		species;
	double	aA;
	double	k;
	double	a;

} GreenwoodPars, *GreenwoodParsPtr;

typedef struct {

	BOOLN	minCFFlag, maxCFFlag;
	BOOLN	updateFlag;
	CFListSpecifier			centreFreqMode;
	CFListDiagModeSpecifier	diagnosticMode;
	int		numChannels;
	double	minCF;
	double	maxCF;
	double	focalCF;
	double	eRBDensity;
	double	*frequency, *bandwidth;
	BandwidthMode	bandwidthMode;

	/* Private members */
	int		oldNumChannels;
	UniParListPtr	cFParList;
	UniParListPtr	bParList;

} CFList, *CFListPtr;

/******************************************************************************/
/*************************** External Variables *******************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Function Prototypes ******************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

BOOLN		AllocateFrequencies_CFList(CFListPtr theCFs);

NameSpecifier *	CFModeList_CFList(int index);

double		CFRateFromF_CFList(CFListPtr theCFs, double frequency);

double		CFSpace_CFList(CFListPtr theCFs);

BOOLN		CheckInit_CFList(CFList *theCFs, const WChar *callingFunction);

BOOLN		CheckPars_CFList(CFListPtr theCFs);

void		Free_CFList(CFListPtr *theCFs);

CFListPtr	GenerateDefault_CFList(WChar *modeName, int numberOfCFs,
			  double minCF, double maxCF, WChar *bwModeName,
			  double (* BWidthFunc)(struct BandwidthMode *, double));

BOOLN		GenerateERB_CFList(CFListPtr theCFs);

BOOLN		GenerateERBn_CFList(CFListPtr theCFs);

BOOLN		GenerateFocalLog_CFList(CFListPtr theCFs);

BOOLN		GenerateGreenwood_CFList(CFListPtr theCFs);

BOOLN		GenerateIdentical_CFList(CFListPtr theCFs);

BOOLN		GenerateLinear_CFList(CFListPtr theCFs);

CFListPtr	GenerateList_CFList(WChar *modeName, WChar *diagModeName,
			  int numberOfCFs, double minCF, double maxCF, double focalCF,
			  double eRBDensity, double *frequencies);

BOOLN		GenerateLog_CFList(CFListPtr theCFs);

BOOLN		GenerateUser_CFList(CFListPtr theCFs);

double		GetBandwidth_CFList(CFListPtr theCFs, int channel);

double		GetCF_CFList(CFListPtr theCFs, int channel);

GreenwoodParsPtr	GetGreenwoodPars_CFList(int	species);

CFListPtr	Init_CFList(const WChar *callingFunctionName);

void		PrintList_CFList(CFListPtr theCFs);

void		PrintPars_CFList(CFListPtr theCFs);

BOOLN		RatifyPars_CFList(CFListPtr theCFs);

BOOLN		ReadBandwidths_CFList(FILE *fp, CFListPtr theCFs);

CFListPtr	ReadPars_CFList(FILE *fp);

BOOLN		RegenerateList_CFList(CFListPtr theCFs);

BOOLN		ResetBandwidth_CFList(CFListPtr theCFs, int channel,
			  double theBandwidth);

BOOLN		ResetCF_CFList(CFListPtr theCFs, int channel, double theFrequency);

BOOLN		SetBandwidthArray_CFList(CFListPtr theCFs, double *theBandwidths);

BOOLN		SetBandwidthMin_CFList(CFListPtr theCFs, double bwMin);

BOOLN		SetBandwidthQuality_CFList(CFListPtr theCFs, double quality);

BOOLN		SetBandwidthSpecifier_CFList(CFListPtr theCFs,
			  WChar *specifierName);

BOOLN		SetBandwidthsWithFunc_CFList(CFListPtr theCFs, WChar *modeName,
			  double (* Func)(BandwidthModePtr, double));

BOOLN		SetBandwidths_CFList(CFListPtr theCFs, WChar *modeName,
			  double *theBandwidths);

BOOLN		SetBandwidthUniParList_CFList(CFListPtr theCFs);

BOOLN		SetBandwidthUniParListMode_CFList(CFListPtr theCFs);

BOOLN		SetCFUniParList_CFList(CFListPtr theCFs);

BOOLN		SetCFUniParListMode_CFList(CFListPtr theCFs);

BOOLN		SetCFMode_CFList(CFListPtr theCFs, WChar *modeName);

BOOLN		SetDiagnosticMode_CFList(CFListPtr theCFs, WChar *modeName);

BOOLN		SetERBDensity_CFList(CFListPtr theCFs, double eRBDensity);

BOOLN		SetFocalCF_CFList(CFListPtr theCFs, double focalCF);

BOOLN		SetGeneratedPars_CFList(CFListPtr theCFs);

BOOLN		SetIndividualBandwidth_CFList(CFListPtr theCFs, int theIndex,
			  double theBandwidth);

BOOLN		SetIndividualFreq_CFList(CFListPtr theCFs, int theIndex,
			  double theFrequency);

BOOLN		SetMaxCF_CFList(CFListPtr theCFs, double maxCF);

BOOLN		SetMinCF_CFList(CFListPtr theCFs, double minCF);

BOOLN		SetNumChannels_CFList(CFListPtr theCFs, int numChannels);

BOOLN		SetSingleFrequency_CFList(CFListPtr theCFs, double theFrequency);

__END_DECLS

#endif
