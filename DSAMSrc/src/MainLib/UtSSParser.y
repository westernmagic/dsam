/**********************
 *
 * File:		UtSSParser.y
 * Purpose: 	Module parser.
 * Comments:	28-08-97 LPO: Added the UTSSPARSER_H symbol so that it would
 *				appear in the header file and allow the use of pre-compiled
 *				headers.
 * Author:		L.P.O'Mard
 * Created:		06 Nov 1995
 * Updated:		28 Aug 1997
 * Copyright:	(c) 1998,University of Essex
 *
 **********************/
 
/******************************************************************************/
/****************************** Grammar ***************************************/
/******************************************************************************/

%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG_H */

#include "GeCommon.h"
#include "GeSignalData.h"
#include "GeEarObject.h"
#include "UtString.h"
#include "UtSSSymbols.h"
#include "UtDynaList.h"
#include "UtSimScript.h"

int		yyerror(char *);
#define	yylex	yylex
int		yylex(void);

#if defined(CRL_mac) && !defined(alloca) 
#	define	alloca	malloc
#endif
#if defined(BORLANDC) && !defined(alloca) 
#	define	alloca	malloc
#endif

%}
%union {				/* stack type */
	Symbol	*sym;
	int		num;
	Datum	*inst;
}
%token	<sym>	_UTSSPARSER_H REPEAT BEGIN STOP STRING QUOTED_STRING RESET
%token	<num>	NUMBER PROCESS
%type	<inst>	process_specifier process statement_specifier reset
%type	<inst>	simulation simulation_name
%%
list:		/* nothing */
		|	list '\n'
		|	list simulation_list '\n'
		|	list error '\n'			{ yyerrok; }
	;
simulation_list:
			simulation	{ ;}
		|	simulation_list simulation
	;
simulation:
			BEGIN '{' statement_list '}'
			{ if (!InitialiseEarObjects_Utility_SimScript()) {
				NotifyError("parser: error for simulation.");
				return 1;
			  }
			  if (!simScriptPtr->subSimList)
				return 0;
			}
		|	BEGIN simulation_name '{' statement_list '}'
			{ $$ = $2;
			  if (!InitialiseEarObjects_Utility_SimScript()) {
				NotifyError("parser: error for simulation.");
				return 1;
			  }
			  if (!simScriptPtr->subSimList)
				return 0;
			}
	;
simulation_name:
			STRING
			{  $$ = (DatumPtr) Pull_Utility_DynaList(&simScriptPtr->subSimList);
			  if (strcmp($$->u.proc.parFile, $1->name) != 0) {
			    NotifyError_Utility_SimScript("parser: '%s' simulation script "
				  "does not correspond with '%s' .", $$->u.proc.parFile,
				  $1->name);
			    return 1;
			  }
			  simScriptPtr->simPtr = GetSimScriptSimulation_Utility_SimScript(
			   $$);
			}
		;
statement_list:
			statement
		|	statement_list statement
	;
statement:
			statement_specifier
			{ $1->u.proc.parFile = InitString_Utility_String(NO_FILE); }
		|	statement_specifier '<' STRING
			{ $1->u.proc.parFile = InitString_Utility_String($3->name); }
		|	statement_specifier '<' '(' STRING ')'
			{ $1->u.proc.parFile = InitString_Utility_String($4->name);
			  Append_Utility_DynaList(&simScriptPtr->subSimList, $1);
			}
		|	statement_specifier '<' STRING QUOTED_STRING
			{ $1->u.proc.parFile = InitString_Utility_String($3->name); }
			/* Ignore the QUOTED_STRING at present. */
		|	statement_specifier NUMBER statement
			{ $1->u.var = $2; InstallInst_Utility_Datum(simScriptPtr->simPtr,
			    STOP); }
		|	reset STRING
			{ $1->u.string = InitString_Utility_String($2->name); }
		|	'{' statement_list '}'
	;
process:
			STRING
			{	$$ = InstallInst_Utility_Datum(simScriptPtr->simPtr, PROCESS);
				$$->u.proc.moduleName = InitString_Utility_String($1->name);
			}
		| '@' STRING
			{	$$ = InstallInst_Utility_Datum(simScriptPtr->simPtr, PROCESS);
				$$->u.proc.moduleName = InitString_Utility_String($2->name);
				$$->onFlag = FALSE;
			}
process_specifier:
			process
		|	process '(' NUMBER '-' '>' ')'
				{ $$ = $1;
				  $1->u.proc.numInputs = $3;
				}
		|	process '(' '-' '>' NUMBER ')'
				{ $$ = $1;
				  $1->u.proc.numOutputs = $5;
				}
		|	process '(' NUMBER '-' '>' NUMBER ')'
				{ $$ = $1;
				  $1->u.proc.numInputs = $3;
				  $1->u.proc.numOutputs = $6;
				}
		;
statement_specifier:
			process_specifier
		|	STRING '%' process_specifier
				{ $$ = $3;
				  $3->u.proc.label = InitString_Utility_String($1->name); }
		|	REPEAT { $$ = InstallInst_Utility_Datum(simScriptPtr->simPtr,
					   REPEAT);}
		|	STRING '%' REPEAT
				{ $$ = InstallInst_Utility_Datum(simScriptPtr->simPtr, REPEAT);
				  $$->u.proc.label = InitString_Utility_String($1->name); }
reset:
			RESET { $$ = InstallInst_Utility_Datum(simScriptPtr->simPtr, RESET);
				}
	;
%%

/******************************************************************************/
/****************************** Includes **************************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Macro Definitions *****************************/
/******************************************************************************/

#define	IS_FILE_PATH_CHAR(C)	((isalnum ((C)) ||(C) == '.') || ((C) == \
								'_') || ((C) == '/') || ((C) == ':'))

/******************************************************************************/
/****************************** Gobal variables *******************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Function Declarations *************************/
/******************************************************************************/

/****************************** yyerror ***************************************/

/*
 * This function is called for yacc syntax error.
 */

int
yyerror(char *s)
{
	NotifyError_Utility_SimScript("%s", s);
	return 1;
}

/****************************** yylex *****************************************/

/*
 * This routine is the lexical analyser.
 * It will install multiple symbols if the first instance of the symbol
 * is a custom reserved word.
 */

int
yylex(void)
{
	static const char *funcName = "yylex";
	int		c;
	
	while (isspace((c = fgetc(simScriptPtr->fp))))
		if (c == '\n')
			simScriptPtr->lineNumber++;
	while (c == '#') {				 /* Check for comments. */
		while (((c = fgetc(simScriptPtr->fp)) != '\n') && (c != EOF))
			;
		for ( ; isspace(c); c = fgetc(simScriptPtr->fp))
			if (c == '\n')
				simScriptPtr->lineNumber++;
	}
	if (c == EOF)
		return 0;
	if (isdigit(c)) {	/* number */
		int		d;
		ungetc(c, simScriptPtr->fp);
		fscanf(simScriptPtr->fp, "%d", &d);
		yylval.num = d;
		return NUMBER;
	}
	if (c == '"') {
		Symbol *s;
		char	sbuf[LONG_STRING], *p = sbuf;
		while ((c = fgetc(simScriptPtr->fp)) != '"' && (c != EOF)) {
			if (p >= sbuf + LONG_STRING - 1) {
				*p = '\0';
				NotifyError("%s: String in quotes is too long (%s)", funcName,
				  sbuf);
				exit(1);
			}
			*p++ = c;
		}
		*p = '\0';
		if (c == EOF) {
			NotifyError("%s: File ends before terminating quotes.", funcName);
			exit(1);
		}
		s = InstallSymbol_Utility_SSSymbols(&simScriptPtr->symList, sbuf,
		  QUOTED_STRING);
		yylval.sym = s;
		return QUOTED_STRING;
	}		
	if (IS_FILE_PATH_CHAR(c)) {
		Symbol *s;
		char	sbuf[LONG_STRING], *p = sbuf;
		do {
			if (p >= sbuf + LONG_STRING - 1) {
				*p = '\0';
				NotifyError("%s: Name too long (%s)", funcName, sbuf);
				exit(1);
			}
			*p++ = c;
		} while ((c = fgetc(simScriptPtr->fp)) != EOF && IS_FILE_PATH_CHAR(c) );
		ungetc(c, simScriptPtr->fp);
		*p = '\0';
		if ((s = LookUpSymbol_Utility_SSSymbols(simScriptPtr->symList, sbuf)) ==
		  0)
			s = InstallSymbol_Utility_SSSymbols(&simScriptPtr->symList, sbuf,
			  STRING);
		else
			if ((s->type == STRING) || (s->type == QUOTED_STRING))
				s = InstallSymbol_Utility_SSSymbols(&simScriptPtr->symList,
				  sbuf, STRING);
		yylval.sym = s;
		return s->type;
	}
	return c;

}
