/**********************
 *
 * File:		WriteHeaderCode.c
 * Purpose: 	Module parser header code writing routines source file.
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#include "MGGeneral.h"
#include "Symbol.h"
#include "Parser.h"
#include "Token.h"
#include "Strings.h"
#include "WriteCodeUtils.h"
#include "WriteHeaderCode.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

static	char	*routineDeclarations[MAX_ROUTINES];
int		numRoutines = 0;

/****************************** Function Declaration List *********************/

/******************************************************************************/
/****************************** Function Declarations *************************/
/******************************************************************************/

/****************************** PrintHeaderIncludes ***************************/

/*
 * This routine prints the Module's early includes needed in the header file.
 */

void
PrintHeaderIncludes(FILE *fp)
{
	Token	*p, *type, *identifierList[MAX_IDENTIFIERS], **list;
	
	/* Name specifier header */
	p = FindTokenType(STRUCT, pc);
	for (p = p->next; p = GetType_IdentifierList(&type, identifierList, p); )
		for (list = identifierList; *list != 0; list++)
			if (type->sym->type == NAMESPECIFIER) {
				fprintf(fp, "#include \"UtNameSpecs.h\"\n\n");
				return;
			}

}

/****************************** PrintConstantDefinitions **********************/

/*
 * This routine prints the Module's constant definitions.
 */

void
PrintConstantDefinitions(FILE *fp)
{
	int		count;
	Token	*p, *type, *identifierList[MAX_IDENTIFIERS], **list;
	
	/* Number of parameters - required by the Universal parameter handling. */
	p = FindTokenType(STRUCT, pc);
	for (p = p->next, count = 0; p = GetType_IdentifierList(&type,
	  identifierList, p); )
		for (list = identifierList; *list != 0; list++)
			count++;
	fprintf(fp, "#define %s_NUM_PARS\t\t\t%d\n\n", CreateBaseModuleName(module,
	  qualifier, TRUE), count);

}

/****************************** PrintParSpecifierEnumDefinition ***************/

/*
 * This routine prints the Module's parameter specifier enum definition.
 * The 'placeComma' boolean variable is used to ensure that there is no comma
 * after the last parameter of the definition.
 */

void
PrintParSpecifierEnumDefinition(FILE *fp)
{
	BOOLN	placeComma = FALSE;
	char	*baseModuleName;
	Token	*p, *type, *identifierList[MAX_IDENTIFIERS], **list;
	
	baseModuleName = CreateBaseModuleName(module, qualifier, TRUE);
	fprintf(fp, "typedef enum {\n\n");
	p = FindTokenType(STRUCT, pc);
	for (p = p->next; p = GetType_IdentifierList(&type, identifierList, p); ) {
		if (placeComma) {
			fprintf(fp, ",\n");
			placeComma = FALSE;
		}
		for (list = identifierList; *list != 0; list++) {
			if (placeComma) {
				fprintf(fp, ",\n");
				placeComma = FALSE;
			}
			fprintf(fp, "\t%s_%s", baseModuleName,
			  UpperCase(DT_TO_SAMPLING_INTERVAL((*list)->sym->name)));
			placeComma = (*(list + 1) != 0);
				
		}
		placeComma = TRUE;
	}
	fprintf(fp, "\n\n} %sParSpecifier;\n\n", structure);

}

/****************************** PrintNameSpecifierEnumDefinition **************/

/*
 * This routine prints the Module's parameter specifier enum definition.
 * The 'placeComma' boolean variable is used to ensure that there is no comma
 * after the last parameter of the definition.
 * Note that because of the way 'Capital' works it cannot appear more than once
 * in a single 'fprintf' instruction.
 */

void
PrintNameSpecifierEnumDefinition(FILE *fp)
{
	char	nameSpecBase[MAXLINE];
	Token	*p, *type, *identifierList[MAX_IDENTIFIERS], **list;
	
	p = FindTokenType(STRUCT, pc);
	for (p = p->next; p = GetType_IdentifierList(&type, identifierList, p); )
		for (list = identifierList; *list != 0; list++)
			if (type->sym->type == NAMESPECIFIER) {
				sprintf(nameSpecBase, "%s_%s_", CreateBaseModuleName(module,
				  qualifier, TRUE), UpperCase((*list)->sym->name));
				fprintf(fp, "typedef enum {\n\n");
				fprintf(fp, "\t%s\n", nameSpecBase);
				fprintf(fp, "\t%sNULL\n", nameSpecBase);
				fprintf(fp, "\n} %s", Capital(module));
				fprintf(fp, "%sSpecifier;\n\n", Capital((*list)->sym->name));
			}

}

/****************************** PrintExpandedStructure ************************/

/*
 * This routine prints the expanded structure for the module
 */

void
PrintExpandedStructure(FILE *fp)
{
	char	line[MAXLINE], variable[MAXLINE];
	Token	*p, *type, *identifierList[10], **list;

	fprintf(fp, "typedef struct {\n\n");
	fprintf(fp, "\tParameterSpecifier	parSpec;\n\n");

	/* Flags first */
	line[0] = '\0';
	p = FindTokenType(STRUCT, pc);
	for (p = p->next; p = GetType_IdentifierList(&type, identifierList, p); )
		for (list = identifierList; *list != 0; list++)
			if (((*list)->inst != POINTER) && (type->sym->type !=
			  BOOLEAN_VAR) && (type->sym->type != CFLISTPTR)) {
				sprintf(variable, "%sFlag", (*list)->sym->name);
				if (line[0] == '\0')
					sprintf(line, "\tBOOLN\t%s", variable);
				else if (RealStringLength(line) + strlen(variable) +
				  strlen(", ;\n") > MAXLINE - 1) {
					fprintf(fp, "%s;\n", line);
					sprintf(line, "\tBOOLN\t%s", variable);
				} else
					sprintf(line, "%s, %s", line, variable);
			}
	if (line[0] != '\0')
		fprintf(fp, "%s;\n", line);

	/* Parameters */
	p = FindTokenType(STRUCT, pc);
	for (p = p->next; p = GetType_IdentifierList(&type, identifierList, p); ) {
		fprintf(fp, "\t%s\t", NAMESPECIFIER_TO_INT(type->sym));
		if ((strlen(type->sym->name) < 4) || (type->sym->type == NAMESPECIFIER))
			fprintf(fp, "\t");
		for (list = identifierList; *list != 0; list++) {
			if (list != identifierList)
				fprintf(fp, ", ");
			if ((*list)->inst == POINTER)
				fprintf(fp, "*");
			fprintf(fp, "%s", (*list)->sym->name);
		}
		fprintf(fp, ";\n");
	}
	fprintf(fp, "\n");

	/* Private members */
	fprintf(fp, "\t/* Private members */\n");
	/* Name Specifiers */
	p = FindTokenType(STRUCT, pc);
	for (p = p->next; p = GetType_IdentifierList(&type, identifierList, p); )
		for (list = identifierList; *list != 0; list++)
			if (type->sym->type == NAMESPECIFIER)
				fprintf(fp, "\tNameSpecifier\t*%sList;\n", (*list)->sym->name);

	fprintf(fp, "\tUniParListPtr	parList;\n");
	fprintf(fp, "\n");
	fprintf(fp, "} %s, *%sPtr;\n\n", structure, structure);

}

/****************************** PrintExternalVariables ************************/

/*
 * This routine prints the external variables for the
 * Module header file.
 */

void
PrintExternalVariables(FILE *fp)
{
	
	fprintf(fp, "extern\t%s\t", ptrType);
	fprintf(fp, "%s;\n\n", ptrVar);

}

/****************************** AddRoutine ************************************/

/*
 * This routine adds a routine declaration to the list of pointers in the
 * global array, it also increments the 'numRoutines' counter.
 */

BOOLN
AddRoutine(char *declaration)
{
	if (numRoutines == MAX_ROUTINES)
		execerror("AddRoutines: Could not add routine", declaration);
	routineDeclarations[numRoutines++] = declaration;
	return(TRUE);

}

/****************************** PrintFuncPrototypes ***************************/

/*
 * This routine prints the function prototypes for the header file.
 * It uses the global list of function declarations.
 * It first sorts the function declarations into alphabetical order.
 */

void
PrintFuncPrototypes(FILE *fp)
{
	int		i;
	char	*p;

	fprintf(fp,
	  "/* C Declarations.  Note the use of the '__BEGIN_DECLS' and "
	  "'__BEGIN_DECLS'\n"
	  "* macros, to allow the safe use of C libraries with C++ libraries - "
	  "defined\n"
	  "* in GeCommon.h.\n"
	  "*/\n"
	  "__BEGIN_DECLS\n\n");
	QuickSortNP(routineDeclarations, 1, "\n\t ", 0, numRoutines - 1);
	for (i = 0; i < numRoutines; i++) {
		/* Find first newline and change it to a tab */
		for (p = routineDeclarations[i]; (*p != '\0') && (*p != '\n'); p++)
			;
		if (*p == '\n')
			*p = '\t';
		Print(fp, "\t\t  ", routineDeclarations[i]);
		Print(fp, "\t\t  ", ";\n\n");
		Print(fp, "", "");
	}
	fprintf(fp, "__END_DECLS\n\n");

}
