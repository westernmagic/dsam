/**********************
 *
 * File:		UtCFList.c
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

#define CFLIST_NUM_PARS				9
#define CFLIST_NUM_CONSTANT_PARS	2

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

typedef struct CFList {

	BOOLN	updateFlag;
	CFListSpecifier			centreFreqMode;
	CFListDiagModeSpecifier	diagnosticMode;
	int		numChannels;
	double	minCF;
	double	maxCF;
	double	focalCF;
	double	eRBDensity;
	double	*frequency, *bandwidth;
	BandwidthModePtr	bandwidthMode;

	/* Private members */
	int		oldNumChannels;
	UniParListPtr	cFParList;
	UniParListPtr	bParList;
	struct	CFList	**parentPtr;

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

BOOLN		CheckInit_CFList(CFList *theCFList, const char *callingFunction);

BOOLN		CheckPars_CFList(CFList *theCFList);

void		Free_CFList(CFListPtr *theCFs);

CFListPtr	GenerateERB_CFList(double minCF, double maxCF, double eRBDensity);

CFListPtr	GenerateERBn_CFList(double minCF, double maxCF, int numberOfCFs);

CFListPtr	GenerateFocalLog_CFList(double focalCF, double minCF, double maxCF,
			  int numberOfCFs);

CFListPtr	GenerateLinear_CFList(double minCF, double maxCF, int numberOfCFs);

CFListPtr	GenerateList_CFList(int mode, int diagnosticMode, int numberOfCFs,
			  double minCF, double maxCF, double focalCF, double eRBDensity,
			  double *frequencies);

CFListPtr	GenerateLog_CFList(double minCF, double maxCF, int numberOfCFs);

CFListPtr	GenerateNewList_CFList(CFListPtr oldCFs);

CFListPtr	GenerateUser_CFList(int theChannels, double *theFrequencies);

double		GetBandwidth_CFList(CFListPtr theCFs, int channel);

double		GetCF_CFList(CFListPtr theCFs, int channel);

CFListPtr	Init_CFList(const char *callingFunctionName);

void		PrintList_CFList(CFListPtr theCFs);

void		PrintPars_CFList(CFListPtr theCFs);

BOOLN		ReadBandwidths_CFList(FILE *fp, CFListPtr theCFs);

CFListPtr	ReadPars_CFList(FILE *fp);

BOOLN		ResetBandwidth_CFList(CFListPtr theCFs, int channel,
			  double theBandwidth);

BOOLN		ResetCF_CFList(CFListPtr theCFs, int channel, double theFrequency);

BOOLN		SetBandwidthArray_CFList(CFListPtr theCFs, double *theBandwidths);

BOOLN		SetBandwidthMin_CFList(CFListPtr theCFs, double bwMin);

BOOLN		SetBandwidthQuality_CFList(CFListPtr theCFs, double quality);

BOOLN		SetBandwidthSpecifier_CFList(CFListPtr theCFs, char *specifierName);

BOOLN		SetBandwidths_CFList(CFListPtr theCFs, char *modeName,
			  double *theBandwidths);

BOOLN		SetBandwidthUniParList_CFList(CFListPtr theCFs);

BOOLN		SetCFUniParList_CFList(CFListPtr theCFs);

BOOLN		SetCFUniParListMode_CFList(CFListPtr theCFs);

BOOLN		SetCFMode_CFList(CFListPtr theCFs, char *modeName);

BOOLN		SetDiagnosticMode_CFList(CFListPtr theCFs, char *modeName);

BOOLN		SetERBDensity_CFList(CFListPtr theCFs, double eRBDensity);

BOOLN		SetIndividualBandwidth_CFList(CFListPtr theCFs, int theIndex,
			  double theBandwidth);

BOOLN		SetIndividualFreq_CFList(CFListPtr theCFs, int theIndex,
			  double theFrequency);

BOOLN		SetMaxCF_CFList(CFListPtr theCFs, double maxCF);

BOOLN		SetMinCF_CFList(CFListPtr theCFs, double minCF);

BOOLN		SetNewBandwidthMode_CFList(CFListPtr theCFs,
			  BandwidthModePtr oldMode);

BOOLN		SetNumChannels_CFList(CFListPtr theCFs, int numChannels);

BOOLN		SetParentCFList_CFList(CFListPtr *cFPtr, CFListPtr theCFList);

BOOLN		SetSingleFrequency_CFList(CFListPtr theCFs, double theFrequency);

__END_DECLS

#endif
