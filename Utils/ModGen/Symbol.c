/**********************
 *
 * File:		Symbol.c
 * Purpose: 	Module parser symbol handling.
 * Comments:	04-11-98 LPO: Added the "NameSpecifier" type.
 * Author:		L.P.O'Mard
 * Created:		06 Nov 1995
 * Updated:		04 Nov 1998
 * Copyright:	(c) 1998, University of Essex.
 *
 **********************/

/******************************************************************************/
/****************************** Includes **************************************/
/******************************************************************************/

#ifdef HAVE_CONFIG_H
#	include "MGSetup.h"
#endif /* HAVE_CONFIG_H */

#include <stdio.h>
#include <string.h>

#include "MGGeneral.h"
#include "Symbol.h"
#include "Parser.h"

/******************************************************************************/
/****************************** Gobal variables *******************************/
/******************************************************************************/

static Symbol	*symlist = 0;	/* Symbol table: linked list */

/******************************************************************************/
/****************************** Function Declarations *************************/
/******************************************************************************/

/****************************** lookup ****************************************/

/*
 * Find s in symbol table.
 */

Symbol *
lookup(char *s)
{
	Symbol	*sp;
	
	for (sp = symlist; sp != (Symbol *) 0; sp = sp->next)
		if (strcmp(sp->name, s) == 0)
			return sp;
	return 0;

}

/****************************** install ***************************************/

/*
 * Install tokens in symbol table.
 */

Symbol *
install(char *s, int type)
{
	Symbol	*sp;
	void	*emalloc(unsigned n);
	
	sp = (Symbol *) emalloc(sizeof (Symbol));
	sp->name = (char *) emalloc(strlen(s) + 1);
	strcpy(sp->name, s);
	sp->type = type;
	sp->next = symlist;		/* put at front of list */
	symlist = sp;
	return sp;

}
