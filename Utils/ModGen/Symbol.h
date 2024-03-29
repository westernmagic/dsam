/**********************
 *
 * File:		Symbol.h
 * Purpose: 	Header file for module parser symbol handling.
 * Comments:	04-11-98 LPO: Added the "NameSpecifier" type.
 * Author:		L.P.O'Mard
 * Created:		06 Nov 1995
 * Updated:		04 Nov 1998
 * Copyright:	(c) 1998, University of Essex.
 *
 **********************/

#ifndef _SYMBOL_H
#define _SYMBOL_H	1

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define	DEFAULT_ALT_NAME_PTR	0

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef struct	Symbol	{	/* Symbol table entry. */

	char	*name;
	char	*altName;		/* Alternative for type e.g. 'int' for 'int_al'. */
	short	type;			/* VAR, BLTIN, UNDEF */
	struct Symbol	*next;	/* To link to next symbol */

} Symbol, *SymbolPtr;

/******************************************************************************/
/****************************** Function prototypes ***************************/
/******************************************************************************/

Symbol	*install(char *, char *, int type);
Symbol	*lookup(char *);
char *	GetName(Symbol *sp);
Symbol	*GetSymbolStackPtr(void);

#endif

