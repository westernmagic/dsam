/******************
 *
 * File:		GeModuleReg.h
 * Comments:	This module handles the register of all the library and user
 *				process modules.
 *				It is necssary to use a module specifier to identify
 *				non-standard process modules such as SIMSCRIPT_MODULE and the
 *				NULL_MODULE
 * Authors:		L. P. O'Mard
 * Created:		29 Mar 1993
 * Updated:		
 * Copyright:	(c) 2001, University of Essex.
 * 
 ******************/

#ifndef	_MODULEREG_H
#define _MODULEREG_H	1

#include "UtNameSpecs.h"
 
/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

#define	MODULE_REG_DEFAAULT_USER_MODULES	10
#define	NULL_MODULE_NAME		"NULL"
#define BM_MODULE_PREFIX		"BM_"

/******************************************************************************/
/*************************** Macro Definitions ********************************/
/******************************************************************************/

/*************************** Misc. Macros *************************************/

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

typedef struct {

	char	*name;
	ModuleSpecifier		specifier;
	BOOLN	(* InitModule )(ModulePtr);

} ModRegEntry, *ModRegEntryPtr;

/******************************************************************************/
/*************************** External Variables *******************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Function Prototypes ******************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

ModRegEntryPtr	Identify_ModuleReg(ModRegEntryPtr list, char *name);

BOOLN	InitUserModuleList_ModuleReg(int theMaxUserModules);

ModRegEntryPtr	GetRegEntry_ModuleReg(char *name);

BOOLN	RegEntry_ModuleReg(const char *name, BOOLN (* InitModuleFunc)(
		  ModulePtr));

__END_DECLS

#endif
