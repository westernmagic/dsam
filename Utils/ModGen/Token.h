/**********************
 *
 * File:		Token.h
 * Purpose: 	Module parser token handling header file.
 * Comments:	04-11-98 LPO: Added the "NameSpecifier" type.
 * Author:		L.P.O'Mard
 * Created:		7 Nov 1995
 * Updated:		04 Nov 1998
 * Copyright:	(c) 1998, University of Essex.
 *
 **********************/

#ifndef _TOKEN_H
#define _TOKEN_H	1

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define	STOP	0
#define	MAX_IDENTIFIERS		20

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef struct	Token	{	/* Symbol table entry. */

	Symbol	*sym;
	int		inst;		/* Denontes instructions, i.e. pointer, end dec., etc.*/
	struct Token	*next;	/* To link to next token. */
	struct Token	*back;	/* To link to previous token. */

} Token, *TokenPtr;

	
/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

extern	Token	*pc;

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

TokenPtr	FindArrayLimit(TokenPtr pc, char *variableName);

Token *		FindTokenInst(int inst, Token *p);

Token *		FindTokenType(int type, Token *p);

Token *		GetType_IdentifierList(Token **type, Token *identifier[], Token *p);

Token *		InstallToken(Symbol *sym, int inst);

void		SetTokenInstRelCurrent(int inst, unsigned int steps);

void		PrintTokens(Token *p);

#endif

