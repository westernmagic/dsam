/**********************
 *
 * File:		MGGeneral.c
 * Purpose: 	General file for module parser symbol handling.
 * Comments:	
 * Author:		L.P.O'Mard
 * Created:		6 Nov 1995
 * Updated:	
 * Copyright:	(c) 1995, Loughborough University of Technology
 *
 **********************/

#include <stdio.h>
#include <stdlib.h>

#include "MGGeneral.h"

/******************************************************************************/
/****************************** Gobal variables *******************************/
/******************************************************************************/

char	*progname;		/* for error messages */
int		lineno = 1;
jmp_buf	begin;
FILE	*fpIn;


/******************************************************************************/
/****************************** Function Declarations *************************/
/******************************************************************************/

/****************************** execerror *************************************/

/*
 * Recover from run-time error
 */

void
execerror(char *s, char *t)
{
	warning(s, t);
	longjmp(begin, 0);

}

/****************************** warning ***************************************/

/*
 * Print warning message.
 */

void
warning(char *s, char *t)
{
	fprintf(stderr, "%s: %s", progname, s);
	if (t)
		fprintf(stderr, " %s", t);
	fprintf(stderr, " near line %d\n", lineno);

}

/****************************** fpecatch **************************************/

/*
 * Catch floating point exceptions.
 */

void
fpecatch(int dummy)
{
	dummy++;
	execerror("Floating point exeception", (char *) 0);

}

/****************************** emalloc ***************************************/

/*
 * Check return from malloc.
 */

void *
emalloc(unsigned n)
{
	char	*p;
	
	p = (char *) malloc(n);
	if (p == 0)
		execerror("Out of memory", (char *) 0);
	return((void *) p);

}


