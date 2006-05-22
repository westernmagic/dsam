/**********************
 *
 * File:		UtOptions.h
 * Purpose:		This module processes unix-type options.
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

#ifndef _UTOPTIONS_H
#define _UTOPTIONS_H 1

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define OPTIONS_IGNORE_OPTION		'@'
#define OPTIONS_IGNORE_ARG_OPTION	'#'

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

typedef enum {

	OPTIONS_NO_ARG,
	OPTIONS_WITH_ARG
	
} OptionsArgSpecifier;

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

extern int	optionsPleaseLink;

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

void	MarkIgnore_Options(int argc, register WChar **argv, WChar *optionString,
		  OptionsArgSpecifier state);

WChar	Process_Options(int ac, register WChar **av, int *optind, int *optsub,
		  WChar **argument, WChar *proto);

__END_DECLS

#endif 
