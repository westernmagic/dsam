/**********************
 *
 * File:		MoHCRPZhang.h
 * Purpose:		Incorporates the Zhang et al. 'ihc' receptor potential code.
 * Comments:	Written using ModuleProducer version 1.4.0 (Mar 14 2002).
 *				This code was revised from the ARLO matlab code.
 * Author:		Revised by L. P. O'Mard
 * Created:		01 Aug 2002
 * Updated:	
 * Copyright:	(c) 2002, CNBH, University of Essex
 *
 *********************/

#ifndef _MOHCRPZHANG_H
#define _MOHCRPZHANG_H 1

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define IHCRP_ZHANG_NUM_PARS			6

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef enum {

	IHCRP_ZHANG_A0,
	IHCRP_ZHANG_B,
	IHCRP_ZHANG_C,
	IHCRP_ZHANG_D,
	IHCRP_ZHANG_CUT,
	IHCRP_ZHANG_K

} IHCRPZhangParSpecifier;

typedef struct {

	ParameterSpecifier	parSpec;

	BOOLN	a0Flag, bFlag, cFlag, dFlag, cutFlag, kFlag;
	BOOLN	updateProcessVariablesFlag;
	double	a0;
	double	b;
	double	c;
	double	d;
	double	cut;
	int		k;

	/* Private members */
	UniParListPtr	parList;
	THairCell *hCRP;

} IHCRPZhang, *IHCRPZhangPtr;

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

extern	IHCRPZhangPtr	iHCRPZhangPtr;

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
* macros, to allow the safe use of C libraries with C++ libraries - defined
* in GeCommon.h.
*/
__BEGIN_DECLS

BOOLN	CheckData_IHCRP_Zhang(EarObjectPtr data);

BOOLN	CheckPars_IHCRP_Zhang(void);

BOOLN	FreeProcessVariables_IHCRP_Zhang(void);

BOOLN	Free_IHCRP_Zhang(void);

UniParListPtr	GetUniParListPtr_IHCRP_Zhang(void);

BOOLN	InitModule_IHCRP_Zhang(ModulePtr theModule);

BOOLN	InitProcessVariables_IHCRP_Zhang(EarObjectPtr data);

BOOLN	Init_IHCRP_Zhang(ParameterSpecifier parSpec);

BOOLN	PrintPars_IHCRP_Zhang(void);

BOOLN	RunModel_IHCRP_Zhang(EarObjectPtr data);

BOOLN	SetA0_IHCRP_Zhang(double theA0);

BOOLN	SetB_IHCRP_Zhang(double theB);

BOOLN	SetC_IHCRP_Zhang(double theC);

BOOLN	SetCut_IHCRP_Zhang(double theCut);

BOOLN	SetD_IHCRP_Zhang(double theD);

BOOLN	SetK_IHCRP_Zhang(int theK);

BOOLN	SetParsPointer_IHCRP_Zhang(ModulePtr theModule);

BOOLN	SetUniParList_IHCRP_Zhang(void);

__END_DECLS

#endif
