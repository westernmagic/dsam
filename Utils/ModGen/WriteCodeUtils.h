/**********************
 *
 * File:		WriteCodeUtils.h
 * Purpose: 	Module parser code writing utility routines header file.
 * Comments:	04-11-98 LPO: Incorporated the universal parameter list code
 *				generation, plus other changes.
 *				05-11-98 LPO: Separated 'WriteCode.[ch]' into
 *				'WriteCodeUtils.[ch]', 'WriteHeaderCode.[ch]' and
 *				'WriteSrcCode.[ch]'.
 *				09-11-98 LPO: I have implemented the variables 'module',
 *				'qualifier', 'structure' and etc. as global.  I have therefore
 *				removed them from the function headers.
 * Author:		L.P.O'Mard
 * Created:		07 Nov 1995
 * Updated:		09 Nov 1998
 * Copyright:	(c) 1998, University of Essex.
 *
 **********************/

#ifndef _WRITECODEUTILS_H
#define _WRITECODEUTILS_H	1

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define	LONG_LINE						512
#define	MAX_IDENTIFIERS					20
#define	MAX_ROUTINES					200

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Macro definitions *****************************/
/******************************************************************************/

#define	DT_TO_SAMPLING_INTERVAL(NAME) ((strcmp(NAME, "dt") == 0)? \
		  "samplingInterval": NAME)

#define NAMESPECIFIER_TO_INT(SYM) (((SYM)->type == NAMESPECIFIER)? "int": \
		  (SYM)->name)

#define NAMESPECIFIER_TO_CHAR(SYM) (((SYM)->type == NAMESPECIFIER)? \
		  "char": (SYM)->name)

#define NAMESPECIFIER_TO_CHAR_PTR(SYM) (((SYM)->type == NAMESPECIFIER)? \
		  "char *": (SYM)->name)

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

extern BOOLN	processVarsFlag;
extern char		*module, *qualifier, *structure, ptrType[], ptrVar[];

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

char *	CreateBaseModuleName(char *moduleName, char *qualifier,
		  BOOLN upperCase);

char *	CreateFuncDeclaration(char *type, char *funcName, char *arguments);

char *	CreateFuncName(char *function, char *moduleName, char *qualifier);

char *	CreateProcessFuncName(Token *pc, char *moduleName, char *qualifier);

char *	GetInputTypeFormatStr(Symbol *p);

char *	GetOutputTypeFormatStr(Symbol *p);

char *	GetOutputUniParTypeFormatStr(TokenPtr p, TokenPtr type);

void	Init_WriteCodeUtils(void);

void	PrintCommentHeading(FILE *fp, char *title);

void	PrintFileHeader(FILE *fp, char *fileNameName);

void	PrintLineCommentHeading(FILE *fp, char *title);

#endif

