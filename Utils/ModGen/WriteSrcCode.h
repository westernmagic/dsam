/**********************
 *
 * File:		WriteSrcCode.h
 * Purpose: 	Module parser source code writing routines header file.
 * Comments:	04-11-98 LPO: Incorporated the universal parameter list code
 *				generation, plus other changes.
 *				05-11-98 LPO: Separated 'WriteCode.[ch]' into
 *				'WriteCodeUtils.[ch]', 'WriteHeaderCode.[ch]' and
 *				'WriteSrcCode.[ch]'.
 * Author:		L.P.O'Mard
 * Created:		07 Nov 1995
 * Updated:		05 Nov 1998
 * Copyright:	(c) 1998, University of Essex.
 *
 **********************/

#ifndef _WRITESRCCODE_H
#define _WRITESRCCODE_H	1

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define	PRINTPARS_ARRAY_TITLE_SIZE		10
#define	PRINTPARS_ARRAY_STRING_FORMAT	"\t%10s"
#define	PRINTPARS_ARRAY_VARS_FORMAT		"\t%10g"

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef enum {

	SET_STANDARD_ROUTINE,
	SET_ARRAY_ELEMENT_ROUTINE,
	SET_BANDWIDTHS_ROUTINE

} SetFunctionTypeSpecifier;

typedef struct {

	char	*name;
	void	(*func)(FILE *);
	
} StandardFunctions;

/******************************************************************************/
/****************************** Macro definitions *****************************/
/******************************************************************************/

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

extern StandardFunctions standardFunctions[];

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

void	PrintAllocArraysRoutines(FILE *fp);

void	PrintArrayCode(FILE *fp, Token *arrayLimit);

void	PrintArrayListHeader(FILE *fp, Token *arrayLimit);

void	PrintArrayListVars(FILE *fp, Token *arrayLimit);

void	PrintCheckDataRoutine(FILE *fp);

void	PrintCheckParsRoutine(FILE *fp);

void	PrintFreeRoutine(FILE *fp);

void	PrintFuncPrototypes(FILE *fp);

void	PrintGetFunctions(FILE *fp);

void	PrintGetUniParListPtrRoutine(FILE *fp);

void	PrintGlobalVariables(FILE *fp);

void	PrintIncludeFiles(FILE *fp, char *headerFile);

void	PrintInitRoutine(FILE *fp);

void	PrintModuleInitCheck(FILE *fp);

void	PrintNameSpecInitListRoutines(FILE *fp);

void	PrintPrintParsRoutine(FILE *fp);

void	PrintProcessRoutine(FILE *fp);

void	PrintReadParsRoutine(FILE *fp);

void	PrintSetParsRoutine(FILE *fp);

void	PrintSetFunction(FILE *fp, TokenPtr token, TokenPtr type,
		  SetFunctionTypeSpecifier functionType);

void	PrintSetFunctionComment(FILE *fp, TokenPtr token, TokenPtr type,
		  SetFunctionTypeSpecifier functionType, char *variableName);

void	PrintSetFunctions(FILE *fp);

void	PrintSetUniParListRoutine(FILE *fp);

#endif

