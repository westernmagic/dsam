/**********************
 *
 * File:		UtCompress.h
 * Purpose:		Compresses an input signal using log or power compression.
 * Comments:	Written using ModuleProducer version 1.9 (May 27 1996).
 * Author:		L. P. O'Mard revised from AIM code
 * Created:		28 Jun 1996
 * Updated:	
 * Copyright:	(c) Loughborough University
 *
 *********************/

#ifndef _UTCOMPRESS_H
#define _UTCOMPRESS_H 1

#include "UtNameSpecs.h"

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define UTILITY_COMPRESSION_NUM_PARS			4

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef enum {

	UTILITY_COMPRESSION_MODE,
	UTILITY_COMPRESSION_SIGNALMULTIPLIER,
	UTILITY_COMPRESSION_POWEREXPONENT,
	UTILITY_COMPRESSION_MINRESPONSE

} CompressionParSpecifier;

typedef enum {

	COMPRESS_LOG_MODE,
	COMPRESS_POWER_MODE,
	COMPRESS_NULL
	
} CompressModeSpecifier;

typedef struct {

	ParameterSpecifier	parSpec;

	BOOLN	modeFlag, signalMultiplierFlag, powerExponentFlag, minResponseFlag;
	int		mode;
	double	signalMultiplier;
	double	powerExponent;
	double	minResponse;

	/* Private members */
	NameSpecifier	*modeList;
	UniParListPtr	parList;

} Compression, *CompressionPtr;

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

extern	CompressionPtr	compressionPtr;

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

BOOLN	CheckData_Utility_Compression(EarObjectPtr data);

BOOLN	CheckPars_Utility_Compression(void);

BOOLN	Free_Utility_Compression(void);

UniParListPtr	GetUniParListPtr_Utility_Compression(void);

BOOLN	Init_Utility_Compression(ParameterSpecifier parSpec);

BOOLN	InitModeList_Utility_Compression(void);

BOOLN	PrintPars_Utility_Compression(void);

BOOLN	Process_Utility_Compression(EarObjectPtr data);

BOOLN	ReadPars_Utility_Compression(char *fileName);

BOOLN	SetMinResponse_Utility_Compression(double theMinResponse);

BOOLN	SetMode_Utility_Compression(char *theMode);

BOOLN	SetPars_Utility_Compression(char *mode, double signalMultiplier,
		  double powerExponent, double minResponse);

BOOLN	SetPowerExponent_Utility_Compression(double thePowerExponent);

BOOLN	SetSignalMultiplier_Utility_Compression(double theSignalMultiplier);

BOOLN	SetUniParList_Utility_Compression(void);

__END_DECLS

#endif
