/**********************
 *
 * File:		UtBinSignal.h
 * Purpose:		This routine generates a binned histogram from the EarObject's
 *				input signal.
 * Comments:	Written using ModuleProducer version 1.9 (Feb 29 1996).
 *				It adds to previous data if the output signal has already been
 *				initialised and the data->updateProcessFlag is set to FALSE.
 *				Otherwise it will overwrite the old signal or create a new
 *				signal as required.
 *				The data->updateProcessFlag facility is useful for repeated
 *				runs.
 * Author:		L. P. O'Mard
 * Created:		21 Mar 1996
 * Updated:	
 * Copyright:	(c) 1998, University of Essex
 *
 *********************/

#ifndef _UTBINSIGNAL_H
#define _UTBINSIGNAL_H 1

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define UTILITY_BINSIGNAL_NUM_PARS			2

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef enum {

	UTILITY_BINSIGNAL_MODE,
	UTILITY_BINSIGNAL_BINWIDTH

} BinSignalParSpecifier;

typedef enum {

	UTILITY_BINSIGNAL_AVERAGE_MODE,
	UTILITY_BINSIGNAL_SUM_MODE,
	UTILITY_BINSIGNAL_MODE_NULL

} UtilityModeSpecifier;

typedef struct {

	ParameterSpecifier	parSpec;

	BOOLN	modeFlag, binWidthFlag;
	int		mode;
	double	binWidth;

	/* Private members */
	NameSpecifier	*modeList;
	UniParListPtr	parList;

} BinSignal, *BinSignalPtr;

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

extern	BinSignalPtr	binSignalPtr;

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

BOOLN	CheckData_Utility_BinSignal(EarObjectPtr data);

BOOLN	CheckPars_Utility_BinSignal(void);

BOOLN	Free_Utility_BinSignal(void);

BOOLN	InitModeList_Utility_BinSignal(void);

BOOLN	Init_Utility_BinSignal(ParameterSpecifier parSpec);

UniParListPtr	GetUniParListPtr_Utility_BinSignal(void);

BOOLN	PrintPars_Utility_BinSignal(void);

BOOLN	Process_Utility_BinSignal(EarObjectPtr data);

BOOLN	ReadPars_Utility_BinSignal(char *fileName);

BOOLN	SetBinWidth_Utility_BinSignal(double theBinWidth);

BOOLN	SetMode_Utility_BinSignal(char * theMode);

BOOLN	InitModule_Utility_BinSignal(ModulePtr theModule);

BOOLN	SetParsPointer_Utility_BinSignal(ModulePtr theModule);

BOOLN	SetPars_Utility_BinSignal(char * mode, double binWidth);

BOOLN	SetUniParList_Utility_BinSignal(void);

__END_DECLS

#endif
