/**********************
 *
 * File:		MGGeneral.h
 * Purpose: 	General header file for module parser symbol handling.
 * Comments:	
 * Author:		L.P.O'Mard
 * Created:		6 Nov 1995
 * Updated:	
 * Copyright:	(c) 1995, Loughborough University of Technology
 *
 **********************/

#ifndef _MGGENERAL_H
#define _MGGENERAL_H	1

#include <setjmp.h>
#include <signal.h>

/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

#ifndef MODGEN_VERSION
#	define	MODGEN_VERSION		"?.?.?"
#endif
#define MAXLINE				82		/* Max. characters in a line + \n\0. */
#define MAX_FILE_PATH		255		/* Max. characters in a line + \n\0. */

#ifndef TRUE
#	define	TRUE		0xFFFF
#endif
#ifndef FALSE
#	define	FALSE		0
#endif

#ifdef WIN32
#	ifndef snprintf
#		define snprintf _snprintf
#	endif
#	ifndef vsnprintf
#		define vsnprintf _vsnprintf
#	endif
#endif

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef	int	BOOLN;

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

extern	char	*progname;
extern	int		lineno;
extern	jmp_buf	begin;
extern	FILE	*fpIn;

/******************************************************************************/
/****************************** Function prototypes ***************************/
/******************************************************************************/

void	*emalloc(unsigned n);

void	execerror(char *s, char *t);

void	fpecatch(int dummy);

void	warning(char *s, char *t);

#endif

