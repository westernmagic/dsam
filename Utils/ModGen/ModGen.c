/**********************
 *
 * File:		ModuleProducer.c
 * Purpose: 	Module generator main process routines.
 * Comments:	
 * Author:		L.P.O'Mard
 * Created:		13 Nov 1995
 * Updated:	
 * Copyright:	(c) 1995, Loughborough University of Technology
 *
 **********************/

#ifdef HAVE_CONFIG_H
#	include "MGSetup.h"
#endif /* HAVE_CONFIG_H */

#include <stdio.h>
#include <ctype.h>

#include "MGGeneral.h"
#include "Symbol.h"
#include "Init.h"
#include "Token.h"
#include "Parser.h"
#include "WriteCodeUtils.h"
#include "WriteHeaderCode.h"
#include "WriteSrcCode.h"
#include "Strings.h"
#include "ModGen.h"

/******************************************************************************/
/****************************** Gobal variables *******************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Function Declarations *************************/
/******************************************************************************/

/****************************** TestProcess ***********************************/

/* 
 * This routine was written to test this application.
 */

void
TestProcess(char *baseName)
{
	static char	*moduleName = "PureTone", *qualifier = "Multi";
	static char	*structure = "MPureTone", *headerFile = "StMPTone.h";
	int		i;

	PrintTokens(pc);
	PrintFileHeader(stdout, headerFile);
	PrintCommentHeading(stdout, "Constant definitions");
	PrintCommentHeading(stdout, "Type definitions");
	PrintExpandedStructure(stdout);
	PrintCommentHeading(stdout, "External variables");
	PrintExternalVariables(stdout);
	PrintCommentHeading(stdout, "Function Prototypes");
	Print(stdout, "\t", "A short long line which should be over eighty "
	  "characters long.  At least it will be if I keep typing.");
	Print(stdout, "\t", "A short long line which should be over eighty "
	  "characters long.  At least it will be if I keep typing.");
	Print(stdout, "", "");
	printf("\n");
	Print(stdout, "\t", "\"A short long line which should be over eighty "
	  "characters long.  At least it will be if I keep typing.\"");
	Print(stdout, "", "");
	printf("\n");
	Print(stdout, "\t", "\"A short long line which should be over "
	  "eighty characters long.  At least it _\"will_\" be if I keep typing.\"");
	Print(stdout, "", "");
	printf("\n");
	Print(stdout, "\t", "\"A short long line which should be_\"\n_\""
	  "over eighty characters long.  At least it will be if I keep typing.\"");
	Print(stdout, "", "");
	printf("\n");
	Print(stdout, "\t\t  ", "BOOLN	SetPars_PureTone_Multi(int "
	  "theNumPTones, double *theFrequencies, double *theIntensities, double "
	  "*thePhases, double theDuration, double theSamplingInterval);");
	Print(stdout, "", "");
	printf("\n");
	PrintIncludeFiles(stdout, headerFile);
	PrintGlobalVariables(stdout);
	for (i = 0; standardFunctions[i].name != NULL; i++)
		(*standardFunctions[i].func)(stdout);
	PrintFuncPrototypes(stdout);

}

/****************************** Process ***************************************/

/* 
 * This routine was written to test this application.
 * If a qualifier is found, then the name of the next token is used.
 * This is because the qualifier might have the same name as the typedef
 * structure, and hence will have been re-typed to TYPEDEF_NAME and will
 * not be found using GetType_IdentifierList().
 * The above also applies to the MOD_NAME.  This is not a good method, and
 * should be reviewed.
 */

void
Process(char *baseName)
{
	char	headerFileNamePath[MAX_FILE_PATH], mainFileNamePath[MAX_FILE_PATH];
	char	*p, *up, *headerFileName = NULL, *mainFileName = NULL;
	char	upperCaseBaseName[MAXLINE];
	int		i;
	FILE	*headerFP, *mainFP;

	snprintf(headerFileNamePath, MAX_FILE_PATH, "%s.h", baseName);
	if ((headerFP = fopen(headerFileNamePath, "w")) == NULL)
		execerror("ModGen: Process: Cannot open file", headerFileName);
	headerFileName = GetFileNameFPath(headerFileNamePath);
	snprintf(mainFileNamePath, MAX_FILE_PATH, "%s.c", baseName);
	if ((mainFP = fopen(mainFileNamePath, "w")) == NULL)
		execerror("ModGen: Process: Cannot open file", mainFileName);
	mainFileName = GetFileNameFPath(mainFileNamePath);
	
	Init_WriteCodeUtils();

	/* Main file. */
	PrintFileHeader(mainFP, mainFileName);
	PrintIncludeFiles(mainFP, headerFileName);
	PrintCommentHeading(mainFP, "Global variables");
	PrintGlobalVariables(mainFP);
	PrintCommentHeading(mainFP, "Subroutines and functions");
	for (i = 0; standardFunctions[i].name != NULL; i++)
		(*standardFunctions[i].func)(mainFP);
	fclose(mainFP);

	/* Header file. */
	
	PrintFileHeader(headerFP, headerFileName);
	for (p = GetFileNameFPath(baseName), up = upperCaseBaseName; *p != '\0';
	  p++, up++)
		*up = toupper(*p);
	*up = '\0';
	fprintf(headerFP, "#ifndef _%s_H\n", upperCaseBaseName);
	fprintf(headerFP, "#define _%s_H 1\n\n", upperCaseBaseName);
	PrintHeaderIncludes(headerFP);
	PrintCommentHeading(headerFP, "Constant definitions");
	PrintConstantDefinitions(headerFP);
	PrintCommentHeading(headerFP, "Type definitions");
	PrintParSpecifierEnumDefinition(headerFP);
	PrintNameSpecifierEnumDefinition(headerFP);
	PrintExpandedStructure(headerFP);
	PrintCommentHeading(headerFP, "External variables");
	PrintExternalVariables(headerFP);
	PrintCommentHeading(headerFP, "Function Prototypes");
	PrintFuncPrototypes(headerFP);
	fprintf(headerFP, "#endif\n");
	fclose(headerFP);

	printf("Module files written to '%s.c' and '%s.h'.\n", baseName, baseName);
	
}

/******************************************************************************/
/****************************** Main function *********************************/
/******************************************************************************/

int
main(int argc, char * argv[])
{
	int		yyparse(void);
	char	*baseName, fileName[MAXLINE];

	progname = argv[0];
	setjmp(begin);
	signal(SIGFPE, fpecatch);
	Init();
	if (argc >= 2)
		sprintf(fileName, "%s", argv[1]);
	else {
		printf("Input module file name: ");
		scanf("%s", fileName);
	}
	if ((baseName = StripFileExtension(fileName, "mod")) == NULL) {
		fprintf(stderr, "ModuleProducer: file must have '.mod' extension.\n");
		return 1;
	}
	if ((fpIn = fopen(fileName, "r")) == NULL) {
		fprintf(stderr, "ModuleProducer: could not open module file\n",
		  fileName);
		 return 1;
	}
	if (yyparse() == 1) {
		fprintf(stderr, "ModuleProducer: exit with error\n", fileName);
		return 1;
	}
	fclose(fpIn);
	printf("%s version %s (%s):\n", progname, MODGEN_VERSION, __DATE__);
	Process(baseName);
	return 0;

}

