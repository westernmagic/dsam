/**********************
 *
 * File:		WriteCodeUtils.h
 * Purpose: 	Module parser code writing utility routines source file.
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

#ifdef HAVE_CONFIG_H
#	include "MGSetup.h"
#endif /* HAVE_CONFIG_H */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#include "MGGeneral.h"
#include "Symbol.h"
#include "Parser.h"
#include "Token.h"
#include "Strings.h"
#include "WriteCodeUtils.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

BOOLN	processVarsFlag;
char	*module, *qualifier, *structure, ptrType[MAXLINE], ptrVar[MAXLINE];

/******************************************************************************/
/****************************** Function Declarations *************************/
/******************************************************************************/

/****************************** Init_WriteCodeUtils ***************************/

/*
 * This routine initialises the global variables required for writting the
 * module code.
 */

void
Init_WriteCodeUtils(void)
{
	Token	*tp, *type, *identifierList[MAX_IDENTIFIERS];

	if ((tp = FindTokenType(MOD_NAME, pc)) == 0)
		execerror("ModGen: Process: Module name not set (mod_name)",
		  (char *) 0);
	else
		module = tp->next->sym->name;
	qualifier = ((tp = FindTokenType(QUALIFIER, pc)) != 0)? tp->next->sym->name:
	  "";
	/* Find the last typedef identifier */
	for (tp = FindTokenType(TYPEDEF_NAME, pc); tp != 0; tp =
	  FindTokenType(TYPEDEF_NAME, tp->next))
		structure = tp->sym->name;
	if (islower(structure[0]))
		execerror("Process: structure name must begin with upper case char.",
		  "");

	sprintf(ptrType, "%sPtr", structure);
	sprintf(ptrVar, "%s", ptrType);
	ptrVar[0] = tolower(ptrType[0]);
	
	if ((tp = FindTokenType(PROCESS_VARS, pc)) == NULL)
		processVarsFlag = FALSE;
	else {
		GetType_IdentifierList(&type, identifierList, tp);
		processVarsFlag = (strcmp(UpperCase(identifierList[0]->sym->name),
		  "TRUE") == 0);
	}

}

/****************************** PrintFileHeader *******************************/

/*
 * Print the standard header information for a module file.
 */

void
PrintFileHeader(FILE *fp, char *fileName)
{
	char	dateStrn[MAXLINE], yearStrn[MAXLINE];
	time_t	theTime;

	theTime = time(NULL);
	strftime(dateStrn, MAXLINE, "%d %b %Y", localtime(&theTime));
	strftime(yearStrn, MAXLINE, "%Y", localtime(&theTime));
	fprintf(fp,
	  "/**********************\n"
	  " *\n"
	  " * File:\t\t%s\n"
	  " * Purpose:\t\t\n"
	  " * Comments:\tWritten using ModuleProducer version %s ("__DATE__").\n"
	  " * Author:\t\t\n"
	  " * Created:\t\t%s\n"
	  " * Updated:\t\n"
	  " * Copyright:\t(c) %s, \n"
	  " *\n"
	  " *********************/\n"
	  "\n", fileName, MODGEN_VERSION, dateStrn, yearStrn);

}

/****************************** PrintCommentHeading ***************************/

/*
 * This routine prints out a three line comment heading.
 */

void
PrintCommentHeading(FILE *fp, char *title)
{
	int		i;

	fprintf(fp,
	  "/**********************************************************************"
	  "********/\n"
	  "/****************************** ");
	fprintf(fp, "%s ", title);
	for (i = 35 + strlen(title); i < MAXLINE - 1; i++)
	fprintf(fp, "*");
	fprintf(fp, "/\n");
	fprintf(fp, "/************************************************************"
	  "******************/\n\n");

}

/****************************** CreateFuncName ********************************/

/*
 * This routine creates a function name.
 * It returns a pointer to a static string, or 0 if it fails.
 */

char *
CreateFuncName(char *function, char *moduleName, char *qualifier)
{
	static	char funcName[MAXLINE];

	if ((strlen(function) + strlen(moduleName) + strlen(qualifier)) >= MAXLINE)
		execerror("Function name too long", function);
	sprintf(funcName, "%s_%s%s%s", function, moduleName, (*qualifier)? "_": "",
	  qualifier);
	return funcName;

}

/****************************** CreateProcessFuncName *************************/

/*
 * This routine creates a process function name.
 * It returns a pointer to a static string, or 0 if it fails.
 */

char *
CreateProcessFuncName(Token *pc, char *moduleName, char *qualifier)
{
	static	char funcName[MAXLINE];
	char	*function;

	Token	*p, *type, *identifierList[MAX_IDENTIFIERS];
	
	if ((p = FindTokenType(PROC_ROUTINE, pc)) == NULL)
		function = "Process";
	else {
		GetType_IdentifierList(&type, identifierList, p);
		function = identifierList[0]->sym->name;
	}
	if ((strlen(function) + strlen(moduleName) + strlen(qualifier)) >= MAXLINE)
		execerror("Function name too long", function);
	sprintf(funcName, "%s_%s%s%s", function, moduleName, (*qualifier)? "_": "",
	  qualifier);
	return funcName;

}

/****************************** CreateBaseModuleName **************************/

/*
 * This routine creates the base module name, i.e. the module name and any
 * qualifier.
 * It returns a pointer to a static string, or 0 if it fails.
 * If the 'upperCase' flag is set, then the output is converted to upper case.
 */

char *
CreateBaseModuleName(char *moduleName, char *qualifier, BOOLN upperCase)
{
	static	char string[MAXLINE];
	char	*p;

	if ((strlen(moduleName) + strlen(qualifier)) >= MAXLINE)
		execerror("Base name too long", moduleName);
	sprintf(string, "%s%s%s", moduleName, (*qualifier)? "_": "", qualifier);
	if (upperCase)
		for (p = string; *p != '\0'; p++)
			*p = toupper(*p);
	return string;

}

/****************************** CreateFuncDeclaration *************************/

/*
 * This routine creates a function declaration.
 * It returns a pointer to the string, or 0 if it fails.
 * It allocates space for the string.
 */

char *
CreateFuncDeclaration(char *type, char *funcName, char *arguments)
{
	char	*p;
	int		length;
	
	length = strlen(type) + strlen(funcName) + strlen(arguments);
	p = (char *) emalloc(length + strlen("()") + 1);
	sprintf(p, "%s%s(%s)", type, funcName, arguments);
	return p;

}

/****************************** PrintLineCommentHeading ***********************/

/*
 * This routine prints out a single line comment heading.
 */

void
PrintLineCommentHeading(FILE *fp, char *title)
{
	int		i;

	fprintf(fp,
	  "/****************************** ");
	fprintf(fp, "%s ", title);
	for (i = 35 + strlen(title); i < MAXLINE - 1; i++)
	fprintf(fp, "*");
	fprintf(fp, "/\n\n");

}

/****************************** GetOutputTypeFormatStr ************************/

/*
 * This routine returns the output type format descriptor string for a symbol,
 * for printing purposes.
 * If an unknown type is found, then it will return 0.
 */

char *
GetOutputTypeFormatStr(Symbol *p)
{

	switch (p->type) {
	case BOOLEAN_VAR:	return("%s");
	case CHAR:			return("%s");
	case INT:			return("%d");
	case LONG:			return("%ld");
	case FLOAT:			return("%f");
	case DOUBLE:		return("%g");
	case NAMESPECIFIER:	return("%s");
	case FILENAME:		return("%s");
	default:			return("\"_to be defined_");
	} /* switch */

}

/****************************** GetInputTypeFormatStr *************************/

/*
 * This routine returns the type format descriptor string for a symbol,
 * for reading (scanf) purposes.
 */

char *
GetInputTypeFormatStr(Symbol *p)
{

	switch (p->type) {
	case BOOLEAN_VAR:	return("%d");
	case CHAR:			return("%s");
	case INT:			return("%d");
	case LONG:			return("%ld");
	case FLOAT:			return("%f");
	case DOUBLE:		return("%lf");
	case NAMESPECIFIER:	return("%s");
	case FILENAME:		return("%s");
	default:			return("\"_to be defined_");
	} /* switch */

}

/****************************** GetOutputUniParTypeFormatStr ******************/

/*
 * This routine returns the universal parameter type format descriptor
 * string for a 'token', for printing purposes.
 */

char *
GetOutputUniParTypeFormatStr(TokenPtr p, TokenPtr type)
{

	if ((p->inst != POINTER) || (type->sym->type == CHAR))
		switch (type->sym->type) {
		case CHAR:			return("UNIPAR_STRING");
		case INT:			return("UNIPAR_INT");
		case LONG:			return("UNIPAR_LONG");
		case DOUBLE:		return("UNIPAR_REAL");
		case NAMESPECIFIER:	return("UNIPAR_NAME_SPEC");
		case FILENAME:		return("UNIPAR_FILE_NAME");
		case CFLISTPTR:		return("UNIPAR_CFLIST");
		case PARARRAY:		return("UNIPAR_PARARRAY");
		case DATUMPTR:		return("UNIPAR_SIMSCRIPT");
		default:			return("UNIPAR_UNKNOWN");
		} /* switch */
	else
		switch (type->sym->type) {
		case INT:			return("UNIPAR_INT_ARRAY");
		case DOUBLE:		return("UNIPAR_REAL_ARRAY");
		default:			return("UNIPAR_UNKNOWN_ARRAY");
		} /* switch */

}

/****************************** GetTypeFormatStr ******************************/

/*
 * This routine returns the "C" type associated with a particular
 * symbol.
 */

char *
GetTypeFormatStr(SymbolPtr sym, BOOLN isPointer)
{

	switch (sym->type) {
	case FILENAME:
	case NAMESPECIFIER:
		if (isPointer)
			return("char *");
		else
			return("char");
		break;
	case DATUMPTR:
		return("DatumPtr");
		break;
	default:
		return(sym->name);
	}

}

