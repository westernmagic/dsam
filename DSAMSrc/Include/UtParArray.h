/**********************
 *
 * File:		UtParArray.h
 * Purpose:		This Module contains the parameter array structure, where the
 *				length of the array is defined by the mode.
 * Comments:	Originally created for use in the MoDRNL filter.
 * Author:		L. P. O'Mard
 * Created:		01 Sep 2000
 * Updated:		
 * Copyright:	(c) 2000, University of Essex.
 *
 *********************/

#ifndef UTPARARRAY_H
#define	UTPARARRAY_H 1

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define PARARRAY_NUM_PARS			2

/******************************************************************************/
/****************************** Bitmaps ***************************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

typedef enum {

	PARARRAY_MODE,
	PARARRAY_PARAMETER,
	PARARRAY_NULL

} ParArrayParSpecifier;

typedef struct {

	BOOLN	updateFlag;
	char	name[MAXLINE];
	int		mode;
	int		numParams;
	double	*params;
	NameSpecifier	*modeList;

	int (* GetNumPars)(int);

	UniParListPtr	parList;
	char	desc[PARARRAY_NUM_PARS][MAXLINE];
	char	abbr[PARARRAY_NUM_PARS][MAXLINE];

} ParArray, *ParArrayPtr;

/******************************************************************************/
/*************************** Class definitions ********************************/
/******************************************************************************/

/******************************************************************************/
/*************************** External Variables *******************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Subroutine declarations **************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

BOOLN	CheckInit_ParArray(ParArrayPtr parArray, const char *callingFunction);

void	Free_ParArray(ParArrayPtr *parArray);

ParArrayPtr	Init_ParArray(char *name, NameSpecifier *modeList,
			  int (* GetNumPars)(int));

void	PrintPars_ParArray(ParArrayPtr parArray);

BOOLN	ReadPars_ParArray(FILE *fp, ParArrayPtr parArray);

BOOLN	SetIndividualPar_ParArray(ParArrayPtr parArray, int theIndex,
		  double parValue);

BOOLN	SetMode_ParArray(ParArrayPtr parArray, char *modeName);

BOOLN	SetUniParList_ParArray(ParArrayPtr parArray);

__END_DECLS

#endif
