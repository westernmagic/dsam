/**********************
 *
 * File:		Init.c
 * Purpose: 	Initialises the symbol table for the Module parser.
 * Comments:	
 * Author:		L.P.O'Mard
 * Created:		6 Nov 1995
 * Updated:	
 * Copyright:	(c) 1997, University of Essex
 *
 **********************/

#ifdef HAVE_CONFIG_H
#	include "MGSetup.h"
#endif /* HAVE_CONFIG_H */

#include "Symbol.h"
#include "Parser.h"
#include "Init.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

/****************************** Constants *************************************/


static struct {
	char	*name;
	double	cval;
} consts[] = {
	{"PI",		3.14159265358979323846},
	{"E",		2.71828182845904523536},
	{"GAMMA",	0.57721566490153286060},	/* Euler */
	{"DEG",		57.29577951308232087680},/* deg/radian */
	{"PHI",		1.61803398874989484820},	/* golden ratio */
	{0,			0.0}
};


/****************************** keywords **************************************/

static struct {

	char	*name;
	int		type;

} keywords[] = {

	{"void",		VOID},
	{"char",		CHAR},
	{"short",		SHORT},
	{"int",			INT},
	{"long",		LONG},
	{"float",		FLOAT},
	{"double",		DOUBLE},
	{"signed",		SIGNED},
	{"unsigned",	UNSIGNED},
	{"typedef",		TYPEDEF},
	{"enum",		ENUM},
	{"const",		CONST},
	{"volatile",	VOLATILE},
	{"struct",		STRUCT},
	{"union",		UNION},
	{"BOOLN",		BOOLEAN_VAR},
	{"mod_name",	MOD_NAME},
	{"qualifier",	QUALIFIER},
	{"proc_routine",	PROC_ROUTINE},
	{"int_al",		INT_AL},
	{"NameSpecifier",	NAMESPECIFIER},
	{"FileName",	FILENAME},
	{"DatumPtr",	DATUMPTR},
	{"CFListPtr",	CFLISTPTR},
	{"ParArrayPtr",	PARARRAY},
	{"process_vars",	PROCESS_VARS},
	{0,		0}

};

/******************************************************************************/
/****************************** Subroutines and functions *********************/
/******************************************************************************/

/****************************** Init ******************************************/

/*
 * installs constanst and built-ins in table.
 */

void
Init(void)
{
	int		i;
	
	/* for (i = 0; consts[i].name; i++)
		install(consts[i].name, VAR, consts[i].cval); */
	for (i = 0; keywords[i].name; i++)
		install(keywords[i].name, keywords[i].type);

}

