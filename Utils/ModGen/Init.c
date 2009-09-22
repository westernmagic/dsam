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
	char	*altName;
	int		type;

} keywords[] = {

	{"void",		DEFAULT_ALT_NAME_PTR,		VOID},
	{"char",		DEFAULT_ALT_NAME_PTR,		CHAR},
	{"WChar",		DEFAULT_ALT_NAME_PTR,		WCHAR},
	{"short",		DEFAULT_ALT_NAME_PTR,		SHORT},
	{"int",			DEFAULT_ALT_NAME_PTR,		INT},
	{"long",		DEFAULT_ALT_NAME_PTR,		LONG},
	{"float",		DEFAULT_ALT_NAME_PTR,		FLOAT},
	{"Float",		DEFAULT_ALT_NAME_PTR,		DFLOAT},
	{"double",		DEFAULT_ALT_NAME_PTR,		DOUBLE},
	{"signed",		DEFAULT_ALT_NAME_PTR,		SIGNED},
	{"unsigned",	DEFAULT_ALT_NAME_PTR,		UNSIGNED},
	{"typedef",		DEFAULT_ALT_NAME_PTR,		TYPEDEF},
	{"enum",		DEFAULT_ALT_NAME_PTR,		ENUM},
	{"const",		DEFAULT_ALT_NAME_PTR,		CONST},
	{"volatile",	DEFAULT_ALT_NAME_PTR,		VOLATILE},
	{"struct",		DEFAULT_ALT_NAME_PTR,		STRUCT},
	{"union",		DEFAULT_ALT_NAME_PTR,		UNION},
	{"BoolSpecifier", DEFAULT_ALT_NAME_PTR,	BOOLSPECIFIER},
	{"BOOLN",		DEFAULT_ALT_NAME_PTR,		BOOLEAN_VAR},
	{"mod_name",	DEFAULT_ALT_NAME_PTR,		MOD_NAME},
	{"qualifier",	DEFAULT_ALT_NAME_PTR,		QUALIFIER},
	{"proc_routine",	DEFAULT_ALT_NAME_PTR,	PROC_ROUTINE},
	{"int_al",		"int",						INT_AL},
	{"NameSpecifier",	DEFAULT_ALT_NAME_PTR,	NAMESPECIFIER},
	{"FileName",	DEFAULT_ALT_NAME_PTR,		FILENAME},
	{"DatumPtr",	DEFAULT_ALT_NAME_PTR,		DATUMPTR},
	{"CFListPtr",	DEFAULT_ALT_NAME_PTR,		CFLISTPTR},
	{"ParArrayPtr",	DEFAULT_ALT_NAME_PTR,		PARARRAY},
	{"process_vars",	DEFAULT_ALT_NAME_PTR,	PROCESS_VARS},
	{0,				0,							0}

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
		install(keywords[i].name, keywords[i].altName, keywords[i].type);

}

