/**********************
 *
 * File:		WriteHeaderCode.h
 * Purpose: 	Module parser header code writing routines header file.
 * Comments:	04-11-98 LPO: Incorporated the universal parameter list code
 *				generation, plus other changes.
 *				05-11-98 LPO: Separated 'WriteCode.[ch]' into
 *				'WriteCodeUtils.[ch]', 'WriteHeaderCode.[ch]' and
 *				'WriteSrcCode.[ch]'.
 * Author:		L.P.O'Mard
 * Created:		07 Nov 1995
 * Updated:		05 Nov 1998
 * Copyright:	(c) 1998, University of Essex.
 *
 **********************/

#ifndef _WRITEHEADERCODE_H
#define _WRITEHEADERCODE_H	1

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Macro definitions *****************************/
/******************************************************************************/

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

BOOLN	AddRoutine(char *declaration);

void	PrintConstantDefinitions(FILE *fp);

void	PrintExpandedStructure(FILE *fp);

void	PrintExternalVariables(FILE *fp);

void	PrintHeaderIncludes(FILE *fp);

void	PrintNameSpecifierEnumDefinition(FILE *fp);

void	PrintParSpecifierEnumDefinition(FILE *fp);

#endif

