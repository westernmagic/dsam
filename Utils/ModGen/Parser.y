/**********************
 *
 * File:		Parser.y
 * Purpose: 	Module parser.
 * Comments:	04-11-98 LPO: Had to put in alot of '{ ; }' otherwise 'bison -y'
 *				gave fatal errors, i.e.'type clash (`' `sym') on default action'
 * Author:		L.P.O'Mard
 * Created:		06 Nov 1995
 * Updated:		04 Nov 1998
 * Copyright:	(c) 1998, University of Essex
 *
 **********************/

/******************************************************************************/
/****************************** Grammar ***************************************/
/******************************************************************************/

%{
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "MGGeneral.h"
#include "Symbol.h"
#include "Token.h"
%}
%union {				/* stack type */
	Symbol	*sym;		/* symbol table pointer */
}
%token	<sym>	IDENTIFIER CONSTANT_EXPRESSION TYPEDEF_NAME
%token	<sym>	TYPEDEF ENUM BOOLEAN_VAR
%token	<sym>	VOID CHAR SHORT INT LONG FLOAT DOUBLE SIGNED UNSIGNED
%token	<sym>	CONST VOLATILE
%token	<sym>	STRUCT UNION
%token	<sym>	MOD_NAME QUALIFIER PROC_ROUTINE PROCESS_VARS
%token	<sym>	POINTER STANDARD_TOKEN INT_AL NAMESPECIFIER CFLISTPTR PARARRAY
%token	<sym>	DATUMPTR FILENAME
%%
list:		/* nothing */
		|	list
		|	list declaration
		|	list error '\n'			{ yyerrok; }
	;
declaration:
		declaration_specifiers ';'
	|	declaration_specifiers token_declarator_list ';'
	;
declaration_list:
		declaration
	|	declaration_list declaration
	;
declaration_specifiers:
		storage_class_specifier
	|	storage_class_specifier declaration_specifiers
	|	type_specifier
	|	type_specifier declaration_specifiers
	|	type_qualifier
	|	type_qualifier declaration_specifiers
	;
storage_class_specifier:
		TYPEDEF				{ ; }
	;
type_specifier:
		VOID				{ ; }
	|	CHAR				{ ; }
	|	SHORT				{ ; }
	|	INT					{ ; }
	|	LONG				{ ; }
	|	FLOAT				{ ; }
	|	DOUBLE				{ ; }
	|	SIGNED				{ ; }
	|	UNSIGNED			{ ; }
	|	struct_or_union_specifier
	|	enum_specifier
	|	BOOLEAN_VAR			{ ; }
	|	typedef_name
	|	MOD_NAME			{ ; }
	|	QUALIFIER			{ ; }
	|	PROC_ROUTINE		{ ; }
	|	INT_AL				{	SetTokenInstRelCurrent(INT_AL, 0); }
	|	NAMESPECIFIER		{ ; }
	|	FILENAME			{ ; }
	|	DATUMPTR			{ ; }
	|	CFLISTPTR			{ ; }
	|	PARARRAY			{ ; }
	|	PROCESS_VARS		{ ; }
	;
typedef_name:
		TYPEDEF_NAME		{ ; }
	|	TYPEDEF	declaration_specifiers IDENTIFIER
			{	$3->type = TYPEDEF_NAME; }
	|	TYPEDEF	declaration_specifiers IDENTIFIER ',' pointer IDENTIFIER
			{	$3->type = TYPEDEF_NAME;
				$6->type = TYPEDEF_NAME;
			}
	;
type_qualifier:
		CONST				{ ; }
	|	VOLATILE			{ ; }
	;
struct_or_union_specifier:
		struct_or_union '{' struct_declaration_list '}'
	|	struct_or_union IDENTIFIER '{' struct_declaration_list '}'
	|	struct_or_union IDENTIFIER
	;
struct_or_union:
		STRUCT				{ ; }
	|	UNION				{ ; }
	;
struct_declaration_list:
		struct_declaration
	|	struct_declaration_list struct_declaration
	;
token_declarator_list:
		token_declarator
	|	token_declarator_list ',' token_declarator
	;
token_declarator:
		declarator
	/* |	declarator = tokenializer */
	;
struct_declaration:
		specifier_qualifier_list struct_declarator_list ';'
	;
specifier_qualifier_list:
		type_specifier
	|	type_specifier specifier_qualifier_list
	|	type_qualifier specifier_qualifier_list
	;
struct_declarator_list:
		struct_declarator
	|	struct_declarator_list ',' struct_declarator
	;
struct_declarator:
		declarator
	|	declarator ':' CONSTANT_EXPRESSION
	|	CONSTANT_EXPRESSION	{ ; }
	;
enum_specifier:
		ENUM '{' enumerator_list '}'			{ ; }
	|	ENUM IDENTIFIER '{' enumerator_list '}'	{ ; }
	|	ENUM IDENTIFIER							{ ; }
	;
enumerator_list:
		enumerator
	|	enumerator_list ',' enumerator
	;
enumerator:
		IDENTIFIER								{ ; }
	|	IDENTIFIER '=' CONSTANT_EXPRESSION		{ ; }
	;
declarator:
		direct_declarator
	|	pointer direct_declarator
			{	SetTokenInstRelCurrent(POINTER, 0); }
	;
direct_declarator:
		IDENTIFIER								{ ; }
	|	'(' declarator ')'
	|	direct_declarator '[' ']'
	|	direct_declarator '[' CONSTANT_EXPRESSION ']'
	/* |	direct_declarator '(' parameter_type_list ')' */
	|	direct_declarator '(' ')'
	|	direct_declarator '(' identifier_list ')'
	;
pointer:
		'*'
	|	'*' type_qualifier_list
	|	'*' type_qualifier_list pointer
	;
type_qualifier_list:
		type_qualifier
	|	type_qualifier_list type_qualifier
	;
identifier_list:
		IDENTIFIER								{ ; }
	|	identifier_list ',' IDENTIFIER
	;
type_name:
		specifier_qualifier_list
	|	specifier_qualifier_list abstract_declarator
	;
abstract_declarator:
		pointer
	|	direct_abstract_declarator
	|	pointer direct_abstract_declarator
	;
direct_abstract_declarator:
		'(' abstract_declarator ')'
	|	'[' ']'
	|	direct_abstract_declarator '[' ']'
	|	direct_abstract_declarator '[' CONSTANT_EXPRESSION ']'
	/* no parameter_type_list etc. */
	;		
%%
/******************************************************************************/
/****************************** Includes **************************************/
/******************************************************************************/

#include "ModGen.h"

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
	warning(s, (char *) 0);
	
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
	int		c;
	
	while (isspace((c = fgetc(fpIn))))
		if (c == '\n')
			lineno++;
	if (c == '/') {				 /* Check for comment. */
		int		cc = fgetc(fpIn);
		if (cc != '*')
			ungetc(cc, fpIn);
		else {
			for (c = ' '; ((cc = fgetc(fpIn)) != '/') || (c != '*'); c = cc) {
				switch (cc) {
				case '\n':
					lineno++;
					break;
				case EOF:
					execerror("Quote with no end", "");
				default:
					;
				} /* switch */
			}
			while (isspace((c = fgetc(fpIn))))
				if (c == '\n')
					lineno++;
		}
	}
	if (c == EOF)
		return 0;
	if (isalpha(c) || (c == '_')) {
		Symbol *s;
		char	sbuf[MAXLINE], *p = sbuf;
		do {
			if (p >= sbuf + MAXLINE - 1) {
				*p = '\0';
				execerror("Name too long", sbuf);
			}
			*p++ = c;
		} while ((c = fgetc(fpIn)) != EOF && (isalnum(c) || (c == '_')));
		ungetc(c, fpIn);
		*p = '\0';
		if ((s = lookup(sbuf)) == 0)
			s = install(sbuf, IDENTIFIER);
		InstallToken(s, STANDARD_TOKEN);
		yylval.sym = s;
		return s->type;
	}
	return c;

}

