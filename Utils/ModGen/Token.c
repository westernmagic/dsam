/**********************
 *
 * File:		Token.c
 * Purpose: 	Module parser token handling routines.
 * Comments:	04-11-98 LPO: Added the "NameSpecifier" type.
 * Author:		L.P.O'Mard
 * Created:		7 Nov 1995
 * Updated:		04 Nov 1998
 * Copyright:	(c) 1998, University of Essex.
 *
 **********************/

#ifdef HAVE_CONFIG_H
#	include "MGSetup.h"
#endif /* HAVE_CONFIG_H */

#include <stdio.h>
#include <string.h>

#include "MGGeneral.h"
#include "Symbol.h"
#include "Parser.h"
#include "Token.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

Token	*pc = 0, *currentPC = 0;

/******************************************************************************/
/****************************** Function Declarations *************************/
/******************************************************************************/

/****************************** InstallToken **********************************/

/*
 * Install a code tokenruction in table.
 * Tokenructions are added to the end of the list using the currentPC pointer.
 */

Token *
InstallToken(Symbol *sym, int inst)
{
	Token	*token;
	void	*emalloc(unsigned n);
	
	token = (Token *) emalloc(sizeof (Token));
	token->sym = sym;
	token->inst = inst;
	token->next = 0;
	token->back = currentPC;
	if (pc == 0)
		currentPC = pc = token;
	else
		currentPC = currentPC->next = token;
	return token;

}

/****************************** SetTokenInstRelCurrent ************************/

/*
 * This routine sets a token, relative to the current instruction.
 * It always moves backwards.
 * It assumes that the currentPC pointer has already been set.
 */

void
SetTokenInstRelCurrent(int inst, unsigned int steps)
{
	Token	*p;
	
	for (p = currentPC; (p != pc) && (steps); p = p->back, steps--)
		;
	if ((p == pc) && (steps != 0))
		execerror("SetTokenInstRelCurrent: failed to set instruction,",
		 (char *) 0);
	p->inst = inst;

}

/****************************** PrintTokens ***********************************/

/*
 * PrintTokens.
 */

void
PrintTokens(Token *p)
{
	Token	*pc;
	
	for (pc = p; pc != STOP; pc = pc->next) {
		if (pc->sym) {
			if (pc->inst == POINTER)
				printf("<*>");
			printf("%s (", pc->sym->name);
			switch (pc->sym->type) {
			case TYPEDEF_NAME:
				printf("typedef-name");
				break;
			case IDENTIFIER:
				printf("identifier");
				break;
			default:
				printf("keyword");
				break;
			} /* switch */
			printf(")\n");
		}
	}
}

/****************************** GetType_IdentifierList ************************/

/*
 * This routine sets the argument pointer to the first keyword encountered,
 * then sets the list of pointers to the identifier list.
 * It returns 0 if it fails, otherwise it returns the position of the next type
 * specifier;
 */

Token *
GetType_IdentifierList(Token **type, Token *identifier[], Token *p)
{
	int		count;
	Token	*pc;
	
	for (pc = p; (pc != STOP); pc = pc->next)
		if ((pc->sym) && (pc->sym->type != IDENTIFIER))
			break;
	if (pc == STOP)
		return 0;
	*type = pc;
	for (pc = pc->next, count = 0; (pc != STOP); pc = pc->next)
		if ((pc->sym) && (pc->sym->type == IDENTIFIER)) {
			if (count++ == MAX_IDENTIFIERS)
				execerror("GetType_IdentifierList: exceeded maximum "
				"number of identifiers", (char *) 0);
			*identifier++ = pc;
		} else
			break;
	*identifier = 0;
	return pc;
	
}

/****************************** FindTokenType *********************************/

/*
 * This routine returns token of the first occurance of a particular token
 * type.
 * It returns 0 if it fails.
 */

Token *
FindTokenType(int type, Token *p)
{
	for (; p != 0; p = p->next)
		if ((p->sym) && (p->sym->type == type))
			return(p);
	return 0;
	
}

/****************************** FindTokenInst *********************************/

/*
 * This routine returns token of the first occurance of a particular token
 * instruction.
 * It returns 0 if it fails.
 */

Token *
FindTokenInst(int inst, Token *p)
{
	for (; p != 0; p = p->next)
		if ((p->inst == inst))
			return(p);
	return 0;

}

/****************************** FindArrayLimit ********************************/

/*
 * This routine finds the array limit token which applies to a particular
 * variable.
 */

TokenPtr
FindArrayLimit(TokenPtr pc, char *variableName)
{
	static char *funcName = "FindArrayLimit";
	TokenPtr	p, arrayLimit, arrayType, arrayIdentifierList[MAX_IDENTIFIERS];
	TokenPtr	pStart, *list, type, identifierList[MAX_IDENTIFIERS];

	for (arrayLimit = pc; (arrayLimit = FindTokenType(INT_AL, arrayLimit)); 
	  arrayLimit = arrayLimit->next) {
		pStart = GetType_IdentifierList(&arrayType, arrayIdentifierList,
		  arrayLimit);
		for (p = pStart; (p = GetType_IdentifierList(&type, identifierList,
		  p)) && ((type->sym->type != INT_AL)); )
			for (list = identifierList; *list != 0; list++)
				if (strcmp((*list)->sym->name, variableName) == 0)
					return(arrayIdentifierList[0]);
	}
	return(NULL);
		
}

