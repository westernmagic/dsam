/**********************
 *
 * File:		UtSSSymbols.h
 * Purpose:		This is the header file for the simulation script symbols
 *				module.
 * Comments:	23-08-99 LPO: This routine was extracted from the UtSimScript
 *				module.
 * Author:		L. P. O'mard
 * Created:		23 Aug 1999
 * Updated:		
 * Copyright:	(c) 1999, University of Essex.
 *
 *********************/

#ifndef _UTSSSYMOBLS_H
#define _UTSSSYMOBLS_H 1

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef struct	Symbol	{	/* Symbol table entry. */

	char	*name;
	short	type;			/* IDENTIFIER, KEYWORD */
	struct Symbol	*next;	/* To link to next symbol */

} Symbol, *SymbolPtr;

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

extern	SymbolPtr	sSSymList;

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

void		FreeSymbols_Utility_SSSymbols(SymbolPtr *list);

void		InitKeyWords_Utility_SSSymbols(SymbolPtr *list);

SymbolPtr	InstallSymbol_Utility_SSSymbols(SymbolPtr *list, char *s, int type);

SymbolPtr	LookUpSymbol_Utility_SSSymbols(SymbolPtr list, char *s);

void		PrintSymbols_Utility_SSSymbols(SymbolPtr list);

__END_DECLS

#endif
