/**********************
 *
 * File:		UtOptions.c
 * Purpose:		This module processes unix-type  options.
 * Comments:	18-03-99 LPO: I have introduced the 'optionsPleaseLink' global
 *				variable.  By accessing this from the GeModuleMgr routine, I
 *				ensure that link problems do not occur for the graphics library
 *				linking.
 * Author:		L. P. O'Mard
 * Created:		31 Jul 1998
 * Updated:		18 Mar 1999
 * Copyright:	(c) 1999, University of Essex
 *
 *********************/

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG_H */

#include <stdio.h>
#include <string.h>

#include "GeCommon.h"
#include "UtOptions.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

int		optionsPleaseLink = 0;

/****************************** Process ***************************************/

/*
 * Decode most reasonable forms of UNIX option syntax. Takes main()-
 * style argument indices (argc/argv) and a string of valid option
 * letters. Letters denoting options with arguments must be followed
 * by colons
 * With valid options, the routine returns the option letter and a pointer to
 * the associated argument (if any).
 * It sets the letter to '?' for bad options and missing arguments.
 * Returns 0 when no options remain, 
 * leaves giving "optind" indexing the first remaining argument.
 */

int
Process_Options(int argc, register char **argv, int *optind, int *optsub,
  char **argument, char *proto)
{
	register char	*idx;
	register int	c;

	*argument = NULL;
	if (*optind >= argc)
		return (0);
	if (*optsub == 0 && (argv[*optind][0] != '-' || argv[*optind][1] == '\0'))
		return (0);
	switch (c = argv[*optind][++(*optsub)]) {
	case '\0':
		++*optind;
		*optsub = 0;
		return (Process_Options(argc, argv, optind, optsub, argument, proto));
	case '-':
		++*optind;
		*optsub = 0;
		return (0);
	case ':':
		return ('?');
	}
	if ((idx = strchr (proto, c)) == NULL)
		return ('?');
	if (idx[1] != ':')
		return (c);
	*argument = &argv[(*optind)++][++(*optsub)];
	*optsub = 0;
	if (*argument[0])
		return (c);
	if (*optind >= argc)
		return ('?');
	*argument = argv[(*optind)++];
	return (c);

}

/****************************** MarkIgnore ************************************/

/*
 * This routine marks an with the OPTIONS_IGNORE_OPTION character.
 * It expects the option to exist in the argument list.
 */

void
MarkIgnore_Options(int argc, register char **argv, char *optionString,
  OptionsArgSpecifier state)
{
	int		i, length;

	length = strlen(optionString);
	for (i = 1; (strncmp(argv[i], optionString, length) != 0) && (i < argc);
	  i++)
		;
	argv[i][1] = (state == OPTIONS_NO_ARG)? OPTIONS_IGNORE_OPTION:
	  OPTIONS_IGNORE_ARG_OPTION;

}

