/**********************
 *
 * File:		WriteSrcCode.c
 * Purpose: 	Module parser source code writing routines source file.
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

#ifdef HAVE_CONFIG_H
#	include "MGSetup.h"
#endif /* HAVE_CONFIG_H */

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
#include "WriteSrcCode.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

/****************************** Standard Functions ****************************/

StandardFunctions standardFunctions[] = {
	
	{"Free",			PrintFreeRoutine},
	{"InitList",		PrintNameSpecInitListRoutines},
	{"GetNumXPars",		PrintGetNumXParsRoutines},
	{"SetDefaultArrays",PrintSetDefaultArraysRoutine},
	{"Init",			PrintInitRoutine},
	{"SetUniParList",	PrintSetUniParListRoutine},
	{"GetUniParListPtr",PrintGetUniParListPtrRoutine},
	{"Alloc",			PrintAllocArraysRoutines},
	{"Set",				PrintSetFunctions},
	{"Get",				PrintGetFunctions},
	{"PrintPars",		PrintPrintParsRoutine},
	{"SetParsPointer",	PrintSetParsPointerRoutine},
	{"InitModule",		PrintInitModuleRoutine},
	{"CheckData",		PrintCheckDataRoutine},
	{"InitProcessVariables",	PrintInitProcessVariablesRoutine},
	{"FreeProcessVariables",	PrintFreeProcessVariablesRoutine},
	{"Process",			PrintProcessRoutine},
	{0,					0}
};

/******************************************************************************/
/****************************** Function Declarations *************************/
/******************************************************************************/

/****************************** PrintIncludeFiles *****************************/

/*
 * This routine prints the files for the module file.
 */

void
PrintIncludeFiles(FILE *fp, char *headerFileName)
{
	BOOLN		parArrayHeaderPlaced = FALSE, nameSpecListsHeaderPlaced = FALSE;
	TokenPtr	p, type, identifierList[MAX_IDENTIFIERS], *list;

	fprintf(fp, "#ifdef HAVE_CONFIG_H\n");
	fprintf(fp, "#\tinclude \"DSAMSetup.h\"\n");
	fprintf(fp, "#endif /* HAVE_CONFIG */\n");
	fprintf(fp, "\n");
	fprintf(fp, "#include <stdio.h>\n");
	fprintf(fp, "#include <stdlib.h>\n");
	fprintf(fp, "#include <math.h>\n");
	fprintf(fp, "\n");
	fprintf(fp, "#include \"GeCommon.h\"\n");
	fprintf(fp, "#include \"GeSignalData.h\"\n");
	fprintf(fp, "#include \"GeEarObject.h\"\n");
	fprintf(fp, "#include \"GeUniParMgr.h\"\n");
	fprintf(fp, "#include \"GeModuleMgr.h\"\n");
	fprintf(fp, "#include \"FiParFile.h\"\n");

	p = FindTokenType(STRUCT, pc);
	for (p = p->next; p = GetType_IdentifierList(&type, identifierList, p); )
		for (list = identifierList; *list != 0; list++) {
			if ((type->sym->type == BOOLSPECIFIER) &&
			  !nameSpecListsHeaderPlaced) {
				fprintf(fp, "#include \"GeNSpecLists.h\"\n");
				nameSpecListsHeaderPlaced = TRUE;
			}
			if (type->sym->type == CFLISTPTR) {
				fprintf(fp, "#include \"UtBandwidth.h\"\n");
				fprintf(fp, "#include \"UtCFList.h\"\n");
			}
			if ((type->sym->type == PARARRAY) && !parArrayHeaderPlaced) {
				fprintf(fp, "#include \"UtParArray.h\"\n");
				parArrayHeaderPlaced = TRUE;
			}
		}

	fprintf(fp, "#include \"%s\"\n", headerFileName);
	fprintf(fp, "\n");

}

/****************************** PrintGlobalVariables ************************/

/*
 * This routine prints the global variables for the module  file.
 */

void
PrintGlobalVariables(FILE *fp)
{
	fprintf(fp, "%s\t", ptrType);
	fprintf(fp, "%s = NULL;\n\n", ptrVar);

}

/****************************** PrintStaticFuncName ***************************/

/*
 * Prints the static declaration for the function name, to be use by error
 * routines.
 */

void
PrintStaticFuncNameDeclaration(FILE *fp, char *funcName)
{
	fprintf(fp, "\tstatic const WChar\t*funcName = wxT(\"%s\");\n", funcName);

}

/****************************** PrintNameSpecInitListRoutines *****************/

/*
 * This routine prints the Module's name specifier list Init_ routines
 * It adds the function declarations to the main list.
 */

void
PrintNameSpecInitListRoutines(FILE *fp)
{
	char	*funcName, *funcDeclaration, function[MAXLINE];
	char	nameSpecBase[MAXLINE], strVariable[MAXLINE];
	TokenPtr	p, type, identifierList[MAX_IDENTIFIERS], *list;
	
	p = FindTokenType(STRUCT, pc);
	for (p = p->next; p = GetType_IdentifierList(&type, identifierList, p); )
		for (list = identifierList; *list != 0; list++)
			if (type->sym->type == NAMESPECIFIER) {
				sprintf(strVariable, "%s%s", GetName((*list)->sym),
				(type->sym->type == PARARRAY)? "Mode": "");
				sprintf(function, "Init%sList", Capital(strVariable));
				PrintLineCommentHeading(fp, function);
				fprintf(fp,
				  "/*\n"
				  " * This function initialises the '%s' list array\n"
				  " */\n\n", strVariable
				  );
				funcName = CreateFuncName(function, module, qualifier);
				funcDeclaration = CreateFuncDeclaration("BOOLN\n", funcName,
				  "void");
				sprintf(nameSpecBase, "%s_%s_", CreateBaseModuleName(module,
				  qualifier, TRUE), UpperCase(GetName((*list)->sym)));
				fprintf(fp, "%s\n{\n", funcDeclaration);
				fprintf(fp, "\tstatic NameSpecifier\tmodeList[] = {\n\n");
				fprintf(fp, "\t\t\t{ wxT(\"\"),\t%s },\n", nameSpecBase);
				fprintf(fp, "\t\t\t{ wxT(\"\"),\t%sNULL },\n", nameSpecBase);
				fprintf(fp, "\t\t};\n");
				fprintf(fp, "\t%s->%sList = modeList;\n", ptrVar, strVariable);
				fprintf(fp, "\treturn(TRUE);\n");
				fprintf(fp, "\n}\n\n");
				AddRoutine(funcDeclaration);
			}

}

/****************************** SetDefaultArraysRoutine ***********************/

/*
 * This routine prints the Module SetDefaultArrays routine if required.
 * It adds the function declaration to the main list.
 */

void
PrintSetDefaultArraysRoutine(FILE *fp)
{
	char	function[MAXLINE];
	char	*funcDeclaration, *funcName;
	Token	*p, *identifierList[MAX_IDENTIFIERS], **list, *pStart, *arrayLimit;
	Token	*type, *arrayType, *arrayIdentifierList[MAX_IDENTIFIERS];
	Symbol	*limitSym;

	for (arrayLimit = pc; (arrayLimit = FindTokenType(INT_AL, arrayLimit)); 
	  arrayLimit = arrayLimit->next) {
		pStart = GetType_IdentifierList(&arrayType, arrayIdentifierList,
		  arrayLimit);
		limitSym = arrayIdentifierList[0]->sym;
		sprintf(function, "SetDefault%sArrays", Capital(GetName(limitSym)));
		PrintLineCommentHeading(fp, function);
		fprintf(fp,
		  "/*\n"
		  " * This function sets the default arrays and array values for the\n"
		  " * '%s' variable.\n"
		  " * It returns FALSE if it fails in any way.\n"
		  " */\n\n",
		  GetName(limitSym)
		  );
		funcName = CreateFuncName(function, module, qualifier);
		funcDeclaration = CreateFuncDeclaration("BOOLN\n", funcName, "void");
		fprintf(fp, "%s\n{\n", funcDeclaration);
		PrintStaticFuncNameDeclaration(fp, funcName);

		fprintf(fp, "\tint\t\ti;\n");
		for (p = pStart; (p = GetType_IdentifierList(&type, identifierList,
		  p)) && ((type->sym->type != INT_AL)); )
			for (list = identifierList; *list != 0; list++)
				if ((*list)->inst == POINTER)
					fprintf(fp, "\t%s\t%s[] = {/* default values here */};\n",
					  GetName(type->sym), GetName((*list)->sym));
		fprintf(fp, "\n");

		fprintf(fp, "\tif (!Alloc%s(/* Def. no. elements */)) {\n",
		  CreateFuncName(Capital(GetName(limitSym)), module, qualifier));
		fprintf(fp, "\t\tNotifyError(wxT(\"%%s: Could not allocate default "
		  "arrays.\"), funcName);\n");
		fprintf(fp, "\t\treturn(FALSE);\n");
		fprintf(fp, "\t}\n");
	
		fprintf(fp, "\tfor (i = 0; i < %s->%s; i++) {\n", ptrVar,
		  GetName(limitSym));
		for (p = pStart; (p = GetType_IdentifierList(&type, identifierList,
		  p)) && ((type->sym->type != INT_AL)); )
			for (list = identifierList; *list != 0; list++)
				if ((*list)->inst == POINTER)
					fprintf(fp, "\t\t%s->%s[i] = %s[i];\n",  ptrVar,
					  GetName((*list)->sym), GetName((*list)->sym));
		fprintf(fp, "\t}\n");

		fprintf(fp, "\treturn(TRUE);\n");
		fprintf(fp, "\n}\n\n");
		AddRoutine(funcDeclaration);
	}

}

/****************************** PrintFreeRoutine ******************************/

/*
 * This routine prints the Module Free routine.
 * It adds the function declaration to the main list.
 */

void
PrintFreeRoutine(FILE *fp)
{
	static char	*function = "Free";
	char	*funcName, *funcDeclaration;
	TokenPtr	p, type, identifierList[MAX_IDENTIFIERS], *list;
	
	PrintLineCommentHeading(fp, function);
	fprintf(fp,
	  "/*\n"
	  " * This function releases of the memory allocated for the process\n"
	  " * variables. It should be called when the module is no longer in use.\n"
	  " * It is defined as returning a BOOLN value because the generic\n"
	  " * module interface requires that a non-void value be returned.\n"
	  " */\n\n"
	  );
	funcName = CreateFuncName(function, module, qualifier);
	funcDeclaration = CreateFuncDeclaration("BOOLN\n", funcName, "void");
	fprintf(fp, "%s\n{\n", funcDeclaration);
	fprintf(fp, "\t/* static const WChar\t*funcName = wxT(\"%s\"); */\n",
	  funcName);
	fprintf(fp, "\n");
	fprintf(fp, "\tif (%s == NULL)\n", ptrVar);
	fprintf(fp, "\t\treturn(FALSE);\n");
	if (processVarsFlag)
		fprintf(fp, "\t%s();\n", CreateFuncName("FreeProcessVariables", module,
		  qualifier));
	/* Free pointers. */
	p = FindTokenType(STRUCT, pc);
	for (p = p->next; p = GetType_IdentifierList(&type, identifierList, p); )
		for (list = identifierList; *list != 0; list++)
			if (((*list)->inst == POINTER) && (type->sym->type != CHAR)) {
				fprintf(fp, "\tif (%s->%s) {\n", ptrVar, GetName((*list)->sym));
				fprintf(fp, "\t\tfree(%s->%s);\n", ptrVar, GetName((*list)->
				  sym));
				fprintf(fp, "\t\t%s->%s = NULL;\n", ptrVar, GetName((*list)->
				  sym));
				fprintf(fp, "\t}\n");
			} else if (type->sym->type == CFLISTPTR)
				fprintf(fp, "\tFree_CFList(&%s->%s);\n", ptrVar,
				  GetName((*list)->sym));
			else if (type->sym->type == PARARRAY)
				fprintf(fp, "\tFree_ParArray(&%s->%s);\n", ptrVar,
				  GetName((*list)->sym));
			else if (type->sym->type == DATUMPTR)
				fprintf(fp, "\tFreeInstructions_Utility_Datum(&%s->%s);\n",
				  ptrVar, GetName((*list)->sym));
	fprintf(fp, "\tif (%s->parList)\n", ptrVar);
	fprintf(fp, "\t\tFreeList_UniParMgr(&%s->parList);\n", ptrVar);
	fprintf(fp, "\tif (%s->parSpec == GLOBAL) {\n", ptrVar);
	fprintf(fp, "\t\tfree(%s);\n", ptrVar);
	fprintf(fp, "\t\t%s = NULL;\n", ptrVar);
	fprintf(fp, "\t}\n");
	fprintf(fp, "\treturn(TRUE);\n");
	fprintf(fp, "\n}\n\n");
	AddRoutine(funcDeclaration);

}

/****************************** PrintInitRoutine ******************************/

/*
 * This routine prints the Module Init routine.
 * It adds the function declaration to the main list.
 */

void
PrintInitRoutine(FILE *fp)
{
	static char	*function = "Init";
	BOOLN	firstParArrayFlag = TRUE, printSetDefaults = FALSE;
	char	*funcName, *funcDeclaration;
	char	initListFunc[MAXLINE], defaultsFunction[MAXLINE];
	TokenPtr	p, type, identifierList[MAX_IDENTIFIERS], *list, arrayLimit;
	TokenPtr	pStart, arrayType, arrayIdentifierList[MAX_IDENTIFIERS];
	Symbol	*limitSym;
	
	PrintLineCommentHeading(fp, function);
	fprintf(fp,
	  "/*\n"
	  " * This function initialises the module by setting module's parameter\n"
	  " * pointer structure.\n"
	  " * The GLOBAL option is for hard programming - it sets the module's\n"
	  " * pointer to the global parameter structure and initialises the\n"
	  " * parameters. The LOCAL option is for generic programming - it\n"
	  " * initialises the parameter structure currently pointed to by the\n"
	  " * module's parameter pointer.\n"
	  " */\n\n"
	  );
	funcName = CreateFuncName(function, module, qualifier);
	funcDeclaration = CreateFuncDeclaration("BOOLN\n", funcName,
	  "ParameterSpecifier parSpec");
	fprintf(fp, "%s\n{\n", funcDeclaration);
	PrintStaticFuncNameDeclaration(fp, funcName);
	fprintf(fp, "\n");
	fprintf(fp, "\tif (parSpec == GLOBAL) {\n");
	fprintf(fp, "\t\tif (%s != NULL)\n", ptrVar);
	fprintf(fp, "\t\t\t%s();\n", CreateFuncName("Free", module, qualifier));
	fprintf(fp, "\t\tif ((%s = (%s) malloc(sizeof(%s))) == NULL) {\n",
	  ptrVar, ptrType, structure);
	fprintf(fp, "\t\t\tNotifyError(wxT(\"%%s: Out of memory for 'global' "
	  "pointer\"), funcName);\n");
	fprintf(fp, "\t\t\treturn(FALSE);\n");
	fprintf(fp, "\t\t}\n");
	fprintf(fp, "\t} else { /* LOCAL */\n");
	fprintf(fp, "\t\tif (%s == NULL) {\n", ptrVar);
	fprintf(fp, "\t\t\tNotifyError(wxT(\"%%s:  'local' pointer not set.\"), "
	  "funcName);\n");
	fprintf(fp, "\t\t\treturn(FALSE);\n");
	fprintf(fp, "\t\t}\n");
	fprintf(fp, "\t}\n");

	fprintf(fp, "\t%s->parSpec = parSpec;\n", ptrVar);
	if (processVarsFlag)
		fprintf(fp, "\t%s->updateProcessVariablesFlag = TRUE;\n", ptrVar);

	/* Public parameters */
	p = FindTokenType(STRUCT, pc);
	for (p = p->next; p = GetType_IdentifierList(&type, identifierList, p); )
		for (list = identifierList; *list != 0; list++) {
			if (type->sym->type == PARARRAY) {
				Print(fp, "\t  ", "\tif ((");
				Print(fp, "\t  ", ptrVar);
				Print(fp, "\t  ", "->");
				Print(fp, "\t  ", GetName((*list)->sym));
				Print(fp, "\t  ", " = Init_ParArray(\"");
				Print(fp, "\t  ", GetName((*list)->sym));
				Print(fp, "\t  ", "\", FitFuncModeList_NSpecLists(0), ");
				Print(fp, "\t  ", CreateFuncName("GetFitFuncPars", module,
				  qualifier));
				Print(fp, "\t  ", ")) == NULL) {\n");
				Print(fp, "", "");
				fprintf(fp, "\t\tNotifyError(wxT(\"%%s: Could not initialise %s "
				  "parArray structure\"),\n\t\t  funcName);\n", GetName((*list)->
				  sym));
				fprintf(fp, "\t\t%s();\n", CreateFuncName("Free", module,
				  qualifier));
				fprintf(fp, "\t\treturn(FALSE);\n");
				fprintf(fp, "\t}\n");
			} else if (type->sym->type == CFLISTPTR) {
				Print(fp, "\t  ", "\tif ((");
				Print(fp, "\t  ", ptrVar);
				Print(fp, "\t  ", "->");
				Print(fp, "\t  ", GetName((*list)->sym));
				Print(fp, "\t  ", " = GenerateDefault_CFList(\n");
				Print(fp, "\t  ", "\t  CFLIST_DEFAULT_MODE_NAME, "
				  "CFLIST_DEFAULT_CHANNELS,\n");
				Print(fp, "\t  ", "\t  CFLIST_DEFAULT_LOW_FREQ, "
				  "CFLIST_DEFAULT_HIGH_FREQ,\n");
				Print(fp, "\t  ", "\t  CFLIST_DEFAULT_BW_MODE_NAME, "
				  "CFLIST_DEFAULT_BW_MODE_FUNC)) == NULL) {\n");
				Print(fp, "", "");
				fprintf(fp, "\t\tNotifyError(wxT(\"%%s: Could not set default "
				  "CFList.\"), funcName);\n");
				fprintf(fp, "\t\treturn(FALSE);\n");
				fprintf(fp, "\t}\n");
			} else if (type->sym->type == FILENAME)
				fprintf(fp, "\tstrcpy(%s->%s, NO_FILE);\n", ptrVar, GetName(
				  (*list)->sym));
			else {
				fprintf(fp, "\t%s->%s = ", ptrVar, GetName((*list)->sym));
				if ((*list)->inst == POINTER)
					fprintf(fp, "NULL");
				else
					switch(type->sym->type) {
					case DOUBLE:
						fprintf(fp, "0.0");
						break;
					case BOOLEAN_VAR:
						fprintf(fp, "FALSE");
						break;
					case BOOLSPECIFIER:
						fprintf(fp, "GENERAL_BOOLEAN_OFF");
						break;
					case PARARRAY:
					case DATUMPTR:
						fprintf(fp, "NULL");
						break;
					default:
						fprintf(fp, "0");
					} /* switch */
				fprintf(fp, ";\n");
			}
		}
	fprintf(fp, "\n");

	/* Default array settings. */
	for (arrayLimit = pc; (arrayLimit = FindTokenType(INT_AL, arrayLimit)); 
	  arrayLimit = arrayLimit->next) {
		printSetDefaults = TRUE;
		pStart = GetType_IdentifierList(&arrayType, arrayIdentifierList,
		  arrayLimit);
		limitSym = arrayIdentifierList[0]->sym;
		sprintf(defaultsFunction, "SetDefault%sArrays", Capital(
		  GetName(limitSym)));
		fprintf(fp, "\tif (!%s()) {\n", CreateFuncName(defaultsFunction, module,
		  qualifier));
		Print(fp, "\t\t  ", "\t\tNotifyError(wxT(\"%s: Could not set the default "
		  "'");
		Print(fp, "\t\t  ", GetName(limitSym));
		Print(fp, "\t\t  ", "' arrays.\"), funcName);\n");
		Print(fp, "", "");
		fprintf(fp, "\t\treturn(FALSE);\n");
		fprintf(fp, "\t}\n");
	}
	if (printSetDefaults)
		fprintf(fp, "\n");

	/* Name specifier list initialisation */
	p = FindTokenType(STRUCT, pc);
	for (p = p->next; p = GetType_IdentifierList(&type, identifierList, p); )
		for (list = identifierList; *list != 0; list++)
			if (type->sym->type == NAMESPECIFIER) {
				sprintf(initListFunc, "Init%s%sList", Capital(
				  GetName((*list)->sym)), (type->sym->type == PARARRAY)? "Mode":
				  "");
				fprintf(fp, "\t%s();\n", CreateFuncName(initListFunc, module,
				  qualifier));
			} else if (type->sym->type == DATUMPTR)
				fprintf(fp, "\tsprintf(%s->%s%s, NO_FILE);\n", ptrVar,
				  GetName((*list)->sym), SIM_SCRIPT_NAME_SUFFIX);
	
	fprintf(fp, "\tif (!%s()) {\n", CreateFuncName("SetUniParList", module,
	  qualifier));
	fprintf(fp, "\t\tNotifyError(wxT(\"%%s: Could not initialise parameter list.\""
	  "), funcName);\n");
	fprintf(fp, "\t\t%s();\n", CreateFuncName("Free", module, qualifier));
	fprintf(fp, "\t\treturn(FALSE);\n");
	fprintf(fp, "\t}\n");
	fprintf(fp, "\treturn(TRUE);\n");
	fprintf(fp, "\n}\n\n");
	AddRoutine(funcDeclaration);

}

/****************************** PrintModuleInitCheck **************************/

/*
 * This routine prints the code which checks that the module is initialised.
 */

void
PrintModuleInitCheck(FILE *fp)
{
	fprintf(fp, "\tif (%s == NULL) {\n", ptrVar);
	fprintf(fp, "\t\tNotifyError(wxT(\"%%s: Module not initialised.\"), funcName);"
	  "\n");
	fprintf(fp, "\t\treturn(FALSE);\n");
	fprintf(fp, "\t}\n");

}

/****************************** PrintSetUniParListRoutine *********************/

/*
 * This routine prints the Module's SetUniParList routine.
 * It adds the function declaration to the main list.
 */

void
PrintSetUniParListRoutine(FILE *fp)
{
	static char	*function = "SetUniParList";
	char	*funcName, *baseModuleName, *funcDeclaration;
	char	setFunctionBase[MAXLINE];
	TokenPtr	p, type, identifierList[MAX_IDENTIFIERS], *list, arrayLimit;
	
	PrintLineCommentHeading(fp, function);
	fprintf(fp,
	  "/*\n"
	  " * This function initialises and sets the module's universal parameter\n"
	  " * list. This list provides universal access to the module's\n"
	  " * parameters.  It expects to be called from the 'Init_' routine.\n"
	  " */\n\n"
	  );
	funcName = CreateFuncName(function, module, qualifier);
	baseModuleName = CreateBaseModuleName(module, qualifier, TRUE);
	funcDeclaration = CreateFuncDeclaration("BOOLN\n", funcName, "void");
	fprintf(fp, "%s\n{\n", funcDeclaration);
	PrintStaticFuncNameDeclaration(fp, funcName);
	fprintf(fp, "\tUniParPtr\tpars;\n");
	fprintf(fp, "\n");
	fprintf(fp, "\tif ((%s->parList = InitList_UniParMgr(UNIPAR_SET_GENERAL,\n"
	  "\t  %s_NUM_PARS, NULL)) == NULL) {\n", ptrVar, baseModuleName);
	fprintf(fp, "\t\tNotifyError(wxT(\"%%s: Could not initialise parList.\""
	  "), funcName);\n");
	fprintf(fp, "\t\treturn(FALSE);\n");
	fprintf(fp, "\t}\n");
	fprintf(fp, "\tpars = %s->parList->pars;\n", ptrVar);
	p = FindTokenType(STRUCT, pc);
	for (p = p->next; p = GetType_IdentifierList(&type, identifierList, p); )
		for (list = identifierList; *list != 0; list++) {
			fprintf(fp, "\tSetPar_UniParMgr(&pars[%s_%s], wxT(\"\"),\n",
			  baseModuleName, UpperCase(DT_TO_SAMPLING_INTERVAL(
			  GetName((*list)->sym))));
			fprintf(fp, "\t  wxT(\"\"),\n");
			fprintf(fp, "\t  %s,\n", GetOutputUniParTypeFormatStr((*list),
			  type));
			fprintf(fp, "\t  &%s->%s, ", ptrVar, GetName((*list)->sym));
			if ((*list)->inst == POINTER) {
				if ((arrayLimit = FindArrayLimit(pc,
				  GetName((*list)->sym))) == NULL)
					fprintf(fp, "NULL,\n");
				else
					fprintf(fp, "&%s->%s,\n", ptrVar, GetName(FindArrayLimit(pc,
					  GetName((*list)->sym))->sym));
			} else if (type->sym->type == NAMESPECIFIER)
				fprintf(fp, "%s->%sList,\n", ptrVar, GetName((*list)->sym));
			else if (type->sym->type == DATUMPTR)
				fprintf(fp, "%s->%s%s,\n", ptrVar, GetName((*list)->sym),
				  SIM_SCRIPT_NAME_SUFFIX);
			else if (type->sym->type == FILENAME)
				fprintf(fp, "\"*.*\",\n");
			else
				fprintf(fp, "NULL,\n");
			if (((*list)->inst == POINTER) && (type->sym->type != CHAR))
				sprintf(setFunctionBase, "Individual%s", Capital(
				  PluralToSingular(GetName((*list)->sym))));
			else if (type->sym->type == CFLISTPTR)
				strcpy(setFunctionBase, "CFList");
			else
				sprintf(setFunctionBase, "%s", Capital(DT_TO_SAMPLING_INTERVAL(
				  GetName((*list)->sym))));
			fprintf(fp, "\t  (void * (*)) Set%s);\n",
			  CreateFuncName(setFunctionBase, module, qualifier));
		}
	fprintf(fp, "\treturn(TRUE);\n");
	fprintf(fp, "\n}\n\n");
	AddRoutine(funcDeclaration);

}

/****************************** PrintGetUniParListPtrRoutine ******************/

/*
 * This routine prints the Module's GetUniParListPtr routine.
 * It adds the function declaration to the main list.
 */

void
PrintGetUniParListPtrRoutine(FILE *fp)
{
	static char	*function = "GetUniParListPtr";
	char	*funcName, *funcDeclaration;
	
	PrintLineCommentHeading(fp, function);
	fprintf(fp,
	  "/*\n"
	  " * This function returns a pointer to the module's universal parameter\n"
	  " * list.\n"
	  " */\n\n"
	  );
	funcName = CreateFuncName(function, module, qualifier);
	funcDeclaration = CreateFuncDeclaration("UniParListPtr\n", funcName,
	  "void");
	fprintf(fp, "%s\n{\n", funcDeclaration);
	PrintStaticFuncNameDeclaration(fp, funcName);
	fprintf(fp, "\n");
	PrintModuleInitCheck(fp);
	fprintf(fp, "\tif (%s->parList == NULL) {\n", ptrVar);
	fprintf(fp, "\t\tNotifyError(wxT(\"%%s: UniParList data structure has not been "
	  "initialised. \"\n\t\t  \"NULL returned.\"), funcName);"
	  "\n");
	fprintf(fp, "\t\treturn(NULL);\n");
	fprintf(fp, "\t}\n");
	fprintf(fp, "\treturn(%s->parList);\n", ptrVar);

	fprintf(fp, "\n}\n\n");
	AddRoutine(funcDeclaration);

}

/****************************** CreateSetParsArguments ************************/

/*
 * This routine creates a list of arguments for the SetPars routine.
 * It allocates space dynamically for the string.
 * Note the special treatment for "dt" variables, i.e. it is always expanded
 * to "samplingInterval" using the DT_TO_SAMPLING_INTERVAL macro.
 */

char *
CreateSetParsArguments(BOOLN justNames)
{
	char	*string;
	int		length, extraChars;
	TokenPtr	p, type, identifierList[MAX_IDENTIFIERS], *list;

	length = 0;
	extraChars = strlen(",  ");
	p = FindTokenType(STRUCT, pc);
	for (p = p->next; p = GetType_IdentifierList(&type, identifierList, p); )
		for (list = identifierList; *list != 0; list++) {
			length += strlen(GetTypeFormatStr(type->sym, TRUE)) + strlen(
			  DT_TO_SAMPLING_INTERVAL(GetName((*list)->sym))) + extraChars;
			if ((*list)->inst == POINTER)
				length++;
		}
	string = (char *) emalloc(length + 1);
	*string = '\0';
	p = FindTokenType(STRUCT, pc);
	for (p = p->next; p = GetType_IdentifierList(&type, identifierList, p); )
		for (list = identifierList; *list != 0; list++) {
			 if (*string != '\0')
				strcat(string, ", ");
			if (!justNames) {
				strcat(string, GetTypeFormatStr(type->sym, TRUE));
				strcat(string, " ");
				if ((*list)->inst == POINTER)
					strcat(string, "*");
			} else {
				if (((*list)->inst == POINTER) && (type->sym->type != CHAR)) {
					strcat(string, ptrVar);
					strcat(string, "->");
				}
					
			}
			strcat(string, DT_TO_SAMPLING_INTERVAL(GetName((*list)->sym)));
		}
	return(string);

}

/****************************** PrintArrayListHeader **************************/

/*
 * This routine prints the 'PrintPars' headers for an array of parameters.
 * It expects the p pointer to be pointer at the array limit.
 * It first counts how many array titles are required.
 */

void
PrintArrayListHeader(FILE *fp, Token *arrayLimit)
{
	char	formatString[MAXLINE];
	int		i, titleCount = 0;
	TokenPtr	p, type, identifierList[MAX_IDENTIFIERS], *list;
	
	for (p = arrayLimit; (p = GetType_IdentifierList(&type, identifierList,
	  p)) && ((p->sym->type != INT_AL)); )
		for (list = identifierList; *list != 0; list++)
			if ((*list)->inst == POINTER)
				titleCount++;
	sprintf(formatString, "_\"");
	for (i = 0; i < titleCount; i++)
		strcat(formatString, PRINTPARS_ARRAY_STRING_FORMAT);
	strcat(formatString, "\n_\"");
	Print(fp, "\t  ", "\tDPrint(");
	Print(fp, "\t  ", formatString);
	for (p = arrayLimit; (p = GetType_IdentifierList(&type, identifierList,
	  p)) && ((p->sym->type != INT_AL)); )
		for (list = identifierList; *list != 0; list++)
			if ((*list)->inst == POINTER) {
				Print(fp, "\t  ", ", _\"");
				Print(fp, "\t  ", GetName((*list)->sym));
				Print(fp, "\t  ", "_\"");
			}
	Print(fp, "\t  ", ");\n");
	Print(fp, "", "");
	Print(fp, "\t  ", "\tDPrint(");
	Print(fp, "\t  ", formatString);
	for (i = 0; i < titleCount; i++)
		Print(fp, "\t  ", ", _\"(\?\?)_\"");
	Print(fp, "\t  ", ");\n");
	Print(fp, "", "");

}

/****************************** PrintArrayListVars ****************************/

/*
 * This routine prints the 'PrintPars' headers for an array of parameters.
 * It expects the p pointer to be pointer at the array limit.
 * It first counts how many array titles are required.
 * It doesn't expect the format string to be greater than MAXLINE characters.
 */

void
PrintArrayListVars(FILE *fp, Token *arrayLimit)
{
	char	formatString[MAXLINE];
	int		i, titleCount = 0;
	TokenPtr	p, type, identifierList[MAX_IDENTIFIERS], *list;
	
	for (p = arrayLimit; (p = GetType_IdentifierList(&type, identifierList,
	  p)) && ((p->sym->type != INT_AL)); )
		for (list = identifierList; *list != 0; list++)
			if ((*list)->inst == POINTER)
				titleCount++;
	sprintf(formatString, "_\"");
	for (i = 0; i < titleCount; i++)
		strcat(formatString, PRINTPARS_ARRAY_VARS_FORMAT);
	strcat(formatString, "\n_\"");
	Print(fp, "\t\t  ", "\t\tDPrint(");
	Print(fp, "\t\t  ", formatString);
	for (p = arrayLimit; (p = GetType_IdentifierList(&type, identifierList,
	  p)) && ((p->sym->type != INT_AL)); )
		for (list = identifierList; *list != 0; list++)
			if ((*list)->inst == POINTER) {
				Print(fp, "\t\t  ", ", ");
				Print(fp, "\t\t  ", ptrVar);
				Print(fp, "\t\t  ", "->");
				Print(fp, "\t\t  ", GetName((*list)->sym));
				Print(fp, "\t\t  ", "[i]");
			}
	Print(fp, "\t\t  ", ");\n");
	Print(fp, "\t\t  ", "");

}

/****************************** PrintPrintParsRoutine *************************/

/*
 * This routine prints the Module PrintPars routine.
 * It adds the function declaration to the main list.
 * It does not expect the single parameter lines to have greater than LONG_LINE
 * characters.
 */

void
PrintPrintParsRoutine(FILE *fp)
{
	static char	*function = "PrintPars";
	char	*funcName, *funcDeclaration;
	TokenPtr	p, type, identifierList[MAX_IDENTIFIERS], *list, arrayLimit;
	
	PrintLineCommentHeading(fp, function);
	fprintf(fp,
	  "/*\n"
	  " * This routine prints all the module's parameters to the file stream.\n"
	  " * specified by the lUTEar.parsFile file pointer.\n"
	  " */\n\n"
	  );
	funcName = CreateFuncName(function, module, qualifier);
	funcDeclaration = CreateFuncDeclaration("BOOLN\n", funcName, "void");
	fprintf(fp, "%s\n{\n", funcDeclaration);
	PrintStaticFuncNameDeclaration(fp, funcName);
	if (FindTokenType(INT_AL, pc))
		fprintf(fp, "\tint\t\ti;\n");
	fprintf(fp, "\n");
	fprintf(fp, "\tDPrint(wxT(\"?? %s Module Parameters:-"
	  "\\n\"));\n", module);
	for (arrayLimit = pc; (arrayLimit = FindTokenType(INT_AL, arrayLimit)); 
	  arrayLimit = arrayLimit->next) {
		PrintArrayListHeader(fp, arrayLimit);
		GetType_IdentifierList(&type, identifierList, arrayLimit);
		Print(fp, "\t  ", "\tfor (i = 0; i < ");
		Print(fp, "\t  ", ptrVar);
		Print(fp, "\t  ", "->");
		Print(fp, "\t  ", GetName(identifierList[0]->sym));
		Print(fp, "\t  ", "; i++)\n");
		Print(fp, "\t  ", "");
		PrintArrayListVars(fp, arrayLimit);
	}
	p = FindTokenType(STRUCT, pc);
	for (p = p->next; p = GetType_IdentifierList(&type, identifierList, p); )
		for (list = identifierList; *list != 0; list++)
			if (type->sym->type == CFLISTPTR)
				fprintf(fp, "\tPrintPars_CFList(%s->%s);\n", ptrVar,
				  GetName((*list)->sym));
			else if (type->sym->type == PARARRAY)
				fprintf(fp, "\tPrintPars_ParArray(%s->%s);\n", ptrVar,
				  GetName((*list)->sym));
			else if (type->sym->type == DATUMPTR) {
				Print(fp, "\t  ", "\tDPrint(wxT(\"\\tSimulation instruction list "
				  "(%%s):-\\n\"), ");
				Print(fp, "\t  ", ptrVar);
				Print(fp, "\t  ", "->");
				Print(fp, "\t  ", GetName((*list)->sym));
				Print(fp, "\t  ", SIM_SCRIPT_NAME_SUFFIX ");\n");
				Print(fp, "\t  ", "");
				Print(fp, "\t  ", "\tPrintInstructions_Utility_Datum(");
				Print(fp, "\t  ", ptrVar);
				Print(fp, "\t  ", "->");
				Print(fp, "\t  ", GetName((*list)->sym));
				Print(fp, "\t  ", ", DATUM_INITIAL_INDENT_LEVEL, \"\\t\");\n");
				Print(fp, "\t  ", "");
			} else if (((*list)->inst != POINTER) || (type->sym->type ==
			  CHAR)) {
				Print(fp, "\t  ", "\tDPrint(wxT(\"\\t");
				Print(fp, "\t  ", DT_TO_SAMPLING_INTERVAL(GetName((*list)->
				  sym)));
				Print(fp, "\t  ", " = ");
				Print(fp, "\t  ", GetOutputTypeFormatStr(type->sym));
				switch (type->sym->type) {
				case BOOLEAN_VAR:
					Print(fp, "\t  ", " \\n\"), (");
					Print(fp, "\t  ", ptrVar);
					Print(fp, "\t  ", "->");
					Print(fp, "\t  ", GetName((*list)->sym));
					Print(fp, "\t  ", ")? \"TRUE\": \"FALSE\"");
					break;
				case BOOLSPECIFIER:
					Print(fp, "\t  ", "\\n\"), ");
					Print(fp, "\t  ", "BooleanList_NSpecLists(");
					Print(fp, "\t  ", ptrVar);
					Print(fp, "\t  ", "->");
					Print(fp, "\t  ", GetName((*list)->sym));
					Print(fp, "\t  ", ")->name");
					break;
				case NAMESPECIFIER:
					Print(fp, "\t  ", " \\n\"), ");
					Print(fp, "\t  ", ptrVar);
					Print(fp, "\t  ", "->");
					Print(fp, "\t  ", GetName((*list)->sym));
					Print(fp, "\t  ", "List[");
					Print(fp, "\t  ", ptrVar);
					Print(fp, "\t  ", "->");
					Print(fp, "\t  ", GetName((*list)->sym));
					Print(fp, "\t  ", "].name");
					break;
				case FILENAME:
					Print(fp, "\t  ", ".\\n\"), ");
					Print(fp, "\t  ", ptrVar);
					Print(fp, "\t  ", "->");
					Print(fp, "\t  ", GetName((*list)->sym));
					break;
				default:
					Print(fp, "\t  ", " \?\?\\n\"), ");
					Print(fp, "\t  ", ptrVar);
					Print(fp, "\t  ", "->");
					Print(fp, "\t  ", GetName((*list)->sym));
				}
				Print(fp, "\t  ", ");\n");
				Print(fp, "\t  ", "");
			}
	fprintf(fp, "\treturn(TRUE);\n");
	fprintf(fp, "\n}\n\n");
	AddRoutine(funcDeclaration);

}

/****************************** PrintAllocArraysRoutines **********************/

/*
 * This routine prints the 'Alloc...' array allocation routines.
 * It adds the function declaration to the main list.
 * It doesn't expect the 'line' string to be larger than LONG_LINE.
 */

void
PrintAllocArraysRoutines(FILE *fp)
{
	char	function[MAXLINE], funcArguments[MAXLINE], arrayPtrVar[MAXLINE];
	char	*funcDeclaration, *funcName;
	Token	*p, *identifierList[MAX_IDENTIFIERS], **list, *pStart, *arrayLimit;
	Token	*type, *arrayType, *arrayIdentifierList[MAX_IDENTIFIERS];
	Symbol	*limitSym;

	for (arrayLimit = pc; (arrayLimit = FindTokenType(INT_AL, arrayLimit)); 
	  arrayLimit = arrayLimit->next) {
		pStart = GetType_IdentifierList(&arrayType, arrayIdentifierList,
		  arrayLimit);
		limitSym = arrayIdentifierList[0]->sym;
		sprintf(function, "Alloc%s", Capital(GetName(limitSym)));
		PrintLineCommentHeading(fp, function);
		fprintf(fp,
		  "/*\n"
		  " * This function allocates the memory for the module arrays.\n"
		  " * It will assume that nothing needs to be done if the '%s' \n"
		  " * variable is the same as the current structure member value.\n"
		  " * To make this work, the function needs to set the structure '%s'\n"
		  " * parameter too.\n"
		  " * It returns FALSE if it fails in any way.\n"
		  " */\n\n",
		  GetName(limitSym), GetName(limitSym)
		  );
		funcName = CreateFuncName(function, module, qualifier);
		sprintf(funcArguments, "int %s", GetName(limitSym));
		funcDeclaration = CreateFuncDeclaration("BOOLN\n", funcName,
		  funcArguments);
		fprintf(fp, "%s\n{\n", funcDeclaration);
		PrintStaticFuncNameDeclaration(fp, funcName);
		fprintf(fp, "\n");

		fprintf(fp, "\tif (%s == %s->%s)\n", GetName(limitSym), ptrVar,
		  GetName(limitSym));
		fprintf(fp, "\t\treturn(TRUE);\n");

		for (p = pStart; (p = GetType_IdentifierList(&type, identifierList,
		  p)) && ((type->sym->type != INT_AL)); )
			for (list = identifierList; *list != 0; list++)
				if ((*list)->inst == POINTER) {
					sprintf(arrayPtrVar, "%s->%s", ptrVar, GetName((*list)->
					  sym));
					fprintf(fp, "\tif (%s)\n", arrayPtrVar);
					fprintf(fp, "\t\tfree(%s);\n", arrayPtrVar);
					Print(fp, "\t  ", "\tif ((");
					Print(fp, "\t  ", arrayPtrVar);
					Print(fp, "\t  ", " = (");
					Print(fp, "\t  ", GetName(type->sym));
					Print(fp, "\t  ", " *) calloc(");
					Print(fp, "\t  ", GetName(limitSym));
					Print(fp, "\t  ", ", sizeof(");
					Print(fp, "\t  ", GetName(type->sym));
					Print(fp, "\t  ", "))) == NULL) {\n");
					Print(fp, "\t  ", "");
					Print(fp, "\t\t  ", "\t\tNotifyError(wxT(_\"%s: Cannot "
					  "allocate memory for '%d' ");
					Print(fp, "\t\t  ", GetName((*list)->sym));
					Print(fp, "\t\t  ", "._\"), funcName, ");
					Print(fp, "\t\t  ", GetName(limitSym));
					Print(fp, "\t\t  ", ");\n");
					Print(fp, "\t\t  ", "");
					fprintf(fp, "\t\treturn(FALSE);\n");
					fprintf(fp, "\t}\n");
				}

		fprintf(fp, "\t%s->%s = %s;\n", ptrVar, GetName(limitSym), GetName(
		  limitSym));
		fprintf(fp, "\treturn(TRUE);\n");
		fprintf(fp, "\n}\n\n");
		AddRoutine(funcDeclaration);
	}

}

/****************************** PrintArrayCode ********************************/

/*
 * This routine prints the memory array initialisation code.
 * It assumes that such lines will not be over LONG_LINE characters long.
 */

void
PrintArrayCode(FILE *fp, Token *arrayLimit)
{
	BOOLN	addSpace;
	char	formatString[MAXLINE];
	Token	*p, *identifierList[MAX_IDENTIFIERS], **list, *pStart;
	Token	*type, *arrayType, *arrayIdentifierList[MAX_IDENTIFIERS];
	Symbol	*limitSym;

	sprintf(formatString, "_\"");
	pStart = GetType_IdentifierList(&arrayType, arrayIdentifierList,
	  arrayLimit);
	limitSym = arrayIdentifierList[0]->sym;
	fprintf(fp, "\tif (!GetPars_ParFile(fp, \"%s\", ",
	  GetInputTypeFormatStr(arrayType->sym));
	fprintf(fp, "&%s))\n", GetName(limitSym));
	fprintf(fp, "\t\tok = FALSE;\n");

	fprintf(fp, "\tif (!Alloc%s_%s(%s)) {\n", Capital(GetName(limitSym)),
	  CreateBaseModuleName(module, qualifier, FALSE), GetName(limitSym));
	Print(fp, "\t\t  ", "\t\tNotifyError(wxT(_\"%%s: Cannot allocate memory for "
	  "the '");
	Print(fp, "\t\t  ", GetName(limitSym));
	Print(fp, "\t\t  ", "' arrays._\"), funcName);\n");
	Print(fp, "\t\t  ", "");
	fprintf(fp, "\t\treturn(FALSE);\n");
	fprintf(fp, "\t}\n");

	addSpace = FALSE;
	for (p = pStart; (p = GetType_IdentifierList(&type, identifierList, p)) &&
	  ((type->sym->type != INT_AL)); )
		for (list = identifierList; *list != 0; list++)
			if ((*list)->inst == POINTER) {
				if (addSpace)
					strcat(formatString, " ");
				strcat(formatString, (type->sym->type == DOUBLE)? "%lf":
				  GetOutputTypeFormatStr(type->sym));
				addSpace = TRUE;
			}
	strcat(formatString, "_\"");
	fprintf(fp, "\tfor (i = 0; i < %s; i++)\n", GetName(limitSym));
	Print(fp, "\t  ", "\t\tif (!GetPars_ParFile(fp, ");
	Print(fp, "\t\t  ", formatString);
	for (p = pStart; (p = GetType_IdentifierList(&type, identifierList, p)) &&
	  ((type->sym->type != INT_AL)); )
		for (list = identifierList; *list != 0; list++)
			if ((*list)->inst == POINTER) {
				if (type->sym->type != CHAR)
					Print(fp, "\t\t  ", ", &");
				Print(fp, "\t\t  ", ptrVar);
				Print(fp, "\t\t  ", "->");
				Print(fp, "\t\t  ", GetName((*list)->sym));
				Print(fp, "\t\t  ", "[i]");
			}
	Print(fp, "\t\t  ", "))\n");
	Print(fp, "\t\t  ", "");
	fprintf(fp, "\t\t\tok = FALSE;\n");

}

/****************************** PrintSetParsPointerRoutine ********************/

/*
 * This routine prints the Module SetParsPointerRoutine routine.
 * It adds the function declaration to the main list.
 */

void
PrintSetParsPointerRoutine(FILE *fp)
{
	static 	char	*function = "SetParsPointer";
	char	*funcName, *funcDeclaration;
	
	PrintLineCommentHeading(fp, function);
	fprintf(fp,
	  "/*\n"
	  " * This function sets the global parameter pointer for the module to\n"
	  " * that associated with the module instance.\n"
	  " */\n\n"
	  );
	funcName = CreateFuncName(function, module, qualifier);
	funcDeclaration = CreateFuncDeclaration("BOOLN\n", funcName,
	  "ModulePtr theModule");
	fprintf(fp, "%s\n{\n", funcDeclaration);
	PrintStaticFuncNameDeclaration(fp, funcName);
	fprintf(fp, "\n");

	fprintf(fp, "\tif (!theModule) {\n");
	fprintf(fp, "\t\tNotifyError(wxT(\"%%s: The module is not set.\"), "
	  "funcName);\n");
	fprintf(fp, "\t\treturn(FALSE);\n");
	fprintf(fp, "\t}\n");
	fprintf(fp, "\t%s = (%s) theModule->parsPtr;\n", ptrVar, ptrType);
	fprintf(fp, "\treturn(TRUE);\n");
	fprintf(fp, "\n}\n\n");
	AddRoutine(funcDeclaration);

}

/****************************** PrintInitModuleRoutine ************************/

/*
 * This routine prints the Module InitModule routine.
 * It adds the function declaration to the main list.
 */

void
PrintInitModuleRoutine(FILE *fp)
{
	static 	char	*function = "InitModule";
	char	*funcName, *funcDeclaration;
	
	PrintLineCommentHeading(fp, function);
	fprintf(fp,
	  "/*\n"
	  " * This routine sets the function pointers for this process module.\n"
	  " * It also initialises the process structure.\n"
	  " */\n\n"
	  );
	funcName = CreateFuncName(function, module, qualifier);
	funcDeclaration = CreateFuncDeclaration("BOOLN\n", funcName,
	  "ModulePtr theModule");
	fprintf(fp, "%s\n{\n", funcDeclaration);
	PrintStaticFuncNameDeclaration(fp, funcName);
	fprintf(fp, "\n");

	fprintf(fp, "\tif (!%s(theModule)) {\n", CreateFuncName("SetParsPointer",
	  module, qualifier));
	fprintf(fp, "\t\tNotifyError(wxT(\"%%s: Cannot set parameters pointer.\"), "
	  "funcName);\n");
	fprintf(fp, "\t\treturn(FALSE);\n");
	fprintf(fp, "\t}\n");
	fprintf(fp, "\tif (!%s(GLOBAL)) {\n", CreateFuncName("Init", module,
	  qualifier));
	fprintf(fp, "\t\tNotifyError(wxT(\"%%s: Could not initialise process "
	  "structure.\"), funcName);\n");
	fprintf(fp, "\t\treturn(FALSE);\n");
	fprintf(fp, "\t}\n");
	fprintf(fp, "\ttheModule->parsPtr = %s;\n", ptrVar);
	fprintf(fp, "\ttheModule->threadMode = MODULE_THREAD_MODE_SIMPLE;\n");
	fprintf(fp, "\ttheModule->Free = %s;\n", CreateFuncName("Free",
	  module, qualifier));
	fprintf(fp, "\ttheModule->GetUniParListPtr = %s;\n", CreateFuncName(
	  "GetUniParListPtr", module, qualifier));
	fprintf(fp, "\ttheModule->PrintPars = %s;\n", CreateFuncName("PrintPars",
	  module, qualifier));
	fprintf(fp, "\ttheModule->RunProcess = %s;\n", CreateProcessFuncName(pc,
	  module, qualifier));
	fprintf(fp, "\ttheModule->SetParsPointer = %s;\n", CreateFuncName(
	  "SetParsPointer", module, qualifier));
	fprintf(fp, "\treturn(TRUE);\n");
	fprintf(fp, "\n}\n\n");
	AddRoutine(funcDeclaration);

}

/****************************** PrintCheckDataRoutine *************************/

/*
 * This routine prints the Module CheckData routine.
 * It adds the function declaration to the main list.
 */

void
PrintCheckDataRoutine(FILE *fp)
{
	static 	char	*function = "CheckData";
	char	*funcName, *funcDeclaration;
	
	PrintLineCommentHeading(fp, function);
	fprintf(fp,
	  "/*\n"
	  " * This routine checks that the 'data' EarObject and input signal are\n"
	  " * correctly initialised.\n"
	  " * It should also include checks that ensure that the module's\n"
	  " * parameters are compatible with the signal parameters, i.e. dt is\n"
	  " * not too small, etc...\n"
	  " * The 'CheckRamp_SignalData()' can be used instead of the\n"
	  " * 'CheckInit_SignalData()' routine if the signal must be ramped for\n"
	  " * the process.\n"
	  " */\n\n"
	  );
	funcName = CreateFuncName(function, module, qualifier);
	funcDeclaration = CreateFuncDeclaration("BOOLN\n", funcName,
	  "EarObjectPtr data");
	fprintf(fp, "%s\n{\n", funcDeclaration);
	PrintStaticFuncNameDeclaration(fp, funcName);
	fprintf(fp, "\n");

	fprintf(fp, "\tif (data == NULL) {\n");
	fprintf(fp, "\t\tNotifyError(wxT(\"%%s: EarObject not initialised.\"), "
	  "funcName);\n");
	fprintf(fp, "\t\treturn(FALSE);\n");
	fprintf(fp, "\t}\n");
	fprintf(fp, "\tif (!CheckInSignal_EarObject(data, funcName))\n");
	fprintf(fp, "\t\treturn(FALSE);\n");
	fprintf(fp, "\t/*** Put additional checks here. ***/\n");
	fprintf(fp, "\treturn(TRUE);\n");
	fprintf(fp, "\n}\n\n");
	AddRoutine(funcDeclaration);

}

/****************************** PrintInitProcessVariablesRoutine **************/

/*
 * This routine prints the Module CheckData routine.
 * It adds the function declaration to the main list.
 */

void
PrintInitProcessVariablesRoutine(FILE *fp)
{
	static 	char	*function = "InitProcessVariables";
	char	*funcName, *funcDeclaration;

	if (!processVarsFlag)
		return;
	PrintLineCommentHeading(fp, function);
	fprintf(fp,
	  "/*\n"
	  " * This function allocates the memory for the process variables.\n"
	  " * It assumes that all of the parameters for the module have been\n"
	  " * correctly initialised.\n"
	  " */\n\n"
	  );
	funcName = CreateFuncName(function, module, qualifier);
	funcDeclaration = CreateFuncDeclaration("BOOLN\n", funcName,
	  "EarObjectPtr data");
	fprintf(fp, "%s\n{\n", funcDeclaration);
	PrintStaticFuncNameDeclaration(fp, funcName);
	fprintf(fp, "\t%s\tp = %s;\n", ptrType, ptrVar);
	fprintf(fp, "\n");

	Print(fp, "\t  ", "\tif (");
	Print(fp, "\t  ", "p->updateProcessVariablesFlag || "
	  "data->updateProcessFlag) {\n");
	Print(fp, "", "");
	fprintf(fp, "\t\t/*** Additional update flags can be added to above line "
	  "***/\n");
	fprintf(fp, "\t\t%s();\n", CreateFuncName("FreeProcessVariables", module,
	  qualifier));
	fprintf(fp, "\t\t/*** Put memory allocation etc here ***/\n");
	fprintf(fp, "\t\tp->updateProcessVariablesFlag = FALSE;\n");
	fprintf(fp, "\t}\n");
	fprintf(fp, "\tif (data->timeIndex == PROCESS_START_TIME) {\n");
	fprintf(fp, "\t\t/*** Put reset (to zero ?) code here ***/\n");
	fprintf(fp, "\t}\n");
	fprintf(fp, "\treturn(TRUE);\n");
	fprintf(fp, "\n}\n\n");
	AddRoutine(funcDeclaration);

}

/****************************** PrintFreeProcessVariablesRoutine **************/

/*
 * This routine prints the Module FreeProcessVariabls routine.
 * It adds the func;tion declaration to the main list.
 */

void
PrintFreeProcessVariablesRoutine(FILE *fp)
{
	static 	char	*function = "FreeProcessVariables";
	char	*funcName, *funcDeclaration;
	
	if (!processVarsFlag)
		return;
	PrintLineCommentHeading(fp, function);
	fprintf(fp,
	  "/*\n"
	  " * This routine releases the memory allocated for the process "
	  "variables\n"
	  " * if they have been initialised.\n"
	  " */\n\n"
	  );
	funcName = CreateFuncName(function, module, qualifier);
	funcDeclaration = CreateFuncDeclaration("BOOLN\n", funcName,
	  "void");
	fprintf(fp, "%s\n{\n", funcDeclaration);
	PrintStaticFuncNameDeclaration(fp, funcName);
	fprintf(fp, "\n");

	fprintf(fp, "\t/** Put memory deallocation code here.    ***/\n");
	fprintf(fp, "\t/** Remember to set the pointers to NULL. ***/\n");

	fprintf(fp, "\treturn(TRUE);\n");
	fprintf(fp, "\n}\n\n");
	AddRoutine(funcDeclaration);

}

/****************************** PrintProcessRoutine ***************************/

/*
 * This routine prints the Module Process routine.
 * It adds the function declaration to the main list.
 */

void
PrintProcessRoutine(FILE *fp)
{
	char	*function, *funcName, *funcDeclaration;
	Token	*p, *type, *identifierList[MAX_IDENTIFIERS];
	
	if ((p = FindTokenType(PROC_ROUTINE, pc)) == NULL)
		function = "Process";
	else {
		GetType_IdentifierList(&type, identifierList, p);
		function = GetName(identifierList[0]->sym);
	}
	PrintLineCommentHeading(fp, function);
	fprintf(fp,
	  "/*\n"
	  " * This routine allocates memory for the output signal, if necessary,\n"
	  " * and generates the signal into channel[0] of the signal data-set.\n"
	  " * It checks that all initialisation has been correctly carried out by\n"
	  " * calling the appropriate checking routines.\n"
	  " * It can be called repeatedly with different parameter values if\n"
	  " * required.\n"
	  " * Stimulus generation only sets the output signal, the input signal\n"
	  " * is not used.\n"
	  " * With repeated calls the Signal memory is only allocated once, then\n"
	  " * re-used.\n"
	  " */\n\n"
	  );
	funcName = CreateFuncName(function, module, qualifier);
	funcDeclaration = CreateFuncDeclaration("BOOLN\n", funcName,
	  "EarObjectPtr data");
	fprintf(fp, "%s\n{\n", funcDeclaration);
	PrintStaticFuncNameDeclaration(fp, funcName);
	fprintf(fp, "\tregister ChanData\t *inPtr, *outPtr;\n");
	fprintf(fp, "\tint\t\ti, chan;\n");
	fprintf(fp, "\tSignalDataPtr\tinSignal, outSignal;\n");
	fprintf(fp, "\t%s\tp = %s;\n", ptrType, ptrVar);
	fprintf(fp, "\n");

	fprintf(fp, "\tinSignal = _InSig_EarObject(data, 0);\n");
	fprintf(fp, "\tif (!data->threadRunFlag) {\n");
	fprintf(fp, "\t\tif (!%s(data)) {\n", CreateFuncName("CheckData", module,
	  qualifier));
	fprintf(fp, "\t\t\tNotifyError(wxT(\"%%s: Process data invalid.\"), funcName);\n");
	fprintf(fp, "\t\t\treturn(FALSE);\n");
	fprintf(fp, "\t\t}\n");

	fprintf(fp, "\t\tSetProcessName_EarObject(data, wxT(\"Module process \?\?\"));\n");
	fprintf(fp, "\t\n");

	fprintf(fp, "\t\t/*** Example Initialise output signal - ammend/change if "
	  "required. ***/\n");
	fprintf(fp, "\t\tif (!InitOutSignal_EarObject(data, inSignal->numChannels, ");
	fprintf(fp, "inSignal->length,\n");
	fprintf(fp, "\t\t  inSignal->dt)) {\n");
	fprintf(fp, "\t\t\tNotifyError(wxT(\"%%s: Cannot initialise output channels.\"), "
	  "funcName);\n");
	fprintf(fp, "\t\t\treturn(FALSE);\n");
	fprintf(fp, "\t\t}\n");
	fprintf(fp, "\n");

	if (processVarsFlag) {
		fprintf(fp, "\t\tif (!%s(data)) {\n", CreateFuncName(
		  "InitProcessVariables", module, qualifier));
		fprintf(fp, "\t\t\tNotifyError(wxT(\"%%s: Could not initialise the process "
		  "variables.\"),\n\t\t  funcName);\n");
		fprintf(fp, "\t\t\treturn(FALSE);\n");
		fprintf(fp, "\t\t}\n");
		fprintf(fp, "\t\tif (data->initThreadRunFlag)\n");
		fprintf(fp, "\t\t\treturn(TRUE);\n");
		fprintf(fp, "\t}\n");
	}

	fprintf(fp, "\toutSignal = _OutSig_EarObject(data);\n");
	fprintf(fp, "\tfor (chan = outSignal->offset; chan < outSignal->numChannels; "
	  "chan++) {\n");
	fprintf(fp, "\t\tinPtr = inSignal->channel[chan];\n");
	fprintf(fp, "\t\toutPtr = outSignal->channel[chan];\n");
	fprintf(fp, "\t\t/*** Put your code here to process output signal. ***/\n");
	fprintf(fp, "\t\t/*** The following 'for' loop is an example only. ***/\n");
	fprintf(fp, "\t\tfor (i = 0; i < data->outSignal->length; i++)\n");
	fprintf(fp, "\t\t\t*outPtr++ = *inPtr++;\n");
	fprintf(fp, "\t}\n");

	fprintf(fp, "\n");
	fprintf(fp, "\tSetProcessContinuity_EarObject(data);\n");
	fprintf(fp, "\treturn(TRUE);\n");
	fprintf(fp, "\n}\n\n");
	AddRoutine(funcDeclaration);

}

/****************************** PrintSetFunctionComment ***********************/

/*
 * This routine prints the comment for the set function.
 */

void
PrintSetFunctionComment(FILE *fp, TokenPtr token, TokenPtr type,
  SetFunctionTypeSpecifier functionType, char *variableName)
{
	if (functionType == SET_BANDWIDTHS_ROUTINE)
		fprintf(fp,
		  "/*\n"
		  " * This function sets the band width mode for the gamma tone "
		    "filters.\n"
		  " * The band width mode defines the function for calculating the "
		    "gamma tone\n"
		  " * filter band width 3 dB down.\n"
		  " * No checks are made on the correct length for the bandwidth "
		    "array.\n"
		  " */\n\n");
	else if (type->sym->type == CFLISTPTR)
		fprintf(fp,
		  "/*\n"
		  " * This function sets the CFList data structure for the filter "
		    "bank.\n"
		  " * It returns TRUE if the operation is successful.\n"
		  " */\n\n");
	else if (type->sym->type == PARARRAY)
		fprintf(fp,
		  "/*\n"
		  " * This function sets the ParArray data structure for the module.\n"
		  " * It returns TRUE if the operation is successful.\n"
		  " */\n\n");
	else {
		fprintf(fp,
		  "/*\n"
		  " * This function sets the module's %s %s%s.\n"
		  " * It returns TRUE if the operation is successful.\n", variableName,
		  (token->inst == POINTER)? "array":
		  "parameter", (functionType == SET_ARRAY_ELEMENT_ROUTINE)? " element":
		  "");
		if (type->sym->type == INT_AL) {
			Print(fp, " * ", " * The '");
			Print(fp, " * ", variableName);
			Print(fp, " * ", "' variable is set by the 'Alloc");
			Print(fp, " * ", Capital(variableName));
			Print(fp, " * ", "_");
			Print(fp, " * ", CreateBaseModuleName(module, qualifier, FALSE));
			Print(fp, " * ", "' routine.\n");
			Print(fp, " * ", "");
		}
		fprintf(fp, " * Additional checks should be added as required.\n"
		  " */\n\n");
	}

}

/****************************** PrintSetFunction ******************************/

/*
 * This routine prints the top section of a Module parameter setting routine.
 */

void
PrintSetFunction(FILE *fp, TokenPtr token, TokenPtr type,
  SetFunctionTypeSpecifier functionType)
{
	char	function[MAXLINE], *funcName, *funcDeclaration, *variableName;
	char	funcArguments[MAXLINE];
	char	assignmentString[MAXLINE];
	TokenPtr	arrayLimit;
	
	switch (functionType) {
	case SET_ARRAY_ELEMENT_ROUTINE:
		variableName = PluralToSingular(GetName(token->sym));
		sprintf(function, "SetIndividual%s", Capital(variableName));
		sprintf(funcArguments, "int theIndex, %s the%s",
		  GetTypeFormatStr(type->sym, TRUE), Capital(variableName));
		sprintf(assignmentString, "\t%s->%s[theIndex] = ", ptrVar,
		  GetName(token->sym));
		break;
	case SET_BANDWIDTHS_ROUTINE:
		variableName = PluralToSingular(GetName(token->sym));
		sprintf(function, "SetBandWidths", Capital(variableName));
		sprintf(funcArguments, "WChar *theBandwidthMode, double *theBandwidths");
		assignmentString[0] = '\0';
		break;
	default:
		variableName = DT_TO_SAMPLING_INTERVAL(GetName(token->sym));
		switch (type->sym->type) {
		case CFLISTPTR:
			strcpy(function, "SetCFList");
			sprintf(funcArguments, "CFListPtr theCFList");
			break;
		default:
			sprintf(function, "Set%s", Capital(variableName));
			sprintf(funcArguments, "%s %sthe%s", GetTypeFormatStr(type->sym,
			  TRUE), (token->inst == POINTER)? "*": "",
			  Capital(variableName));
			if (type->sym->type == FILENAME)
				sprintf(assignmentString, "\tsnprintf(%s->%s, MAX_FILE_PATH, "
				  "\"%%s\", the%s);\n", ptrVar, GetName(token->sym), Capital(
				  variableName));
			else
				sprintf(assignmentString, "\t%s->%s = ", ptrVar, GetName(token->
				  sym));
		}
	}
	PrintLineCommentHeading(fp, function);
	PrintSetFunctionComment(fp, token, type, functionType, variableName); 
	funcName = CreateFuncName(function, module, qualifier);
	funcDeclaration = CreateFuncDeclaration("BOOLN\n", funcName,
	  funcArguments);
	fprintf(fp, "%s\n{\n", funcDeclaration);
	Print(fp, "\t  ", "\tstatic const WChar\t*funcName = wxT(_\"");
	Print(fp, "\t  ", funcName);
	Print(fp, "\t  ", "_\");\n");
	Print(fp, "\t  ", "");
	if ((type->sym->type == NAMESPECIFIER) || (type->sym->type ==
	  BOOLSPECIFIER))
		fprintf(fp, "\tint\t\tspecifier;\n");
	fprintf(fp, "\n");
	PrintModuleInitCheck(fp);
	if (functionType == SET_ARRAY_ELEMENT_ROUTINE) {
		fprintf(fp, "\tif (%s->%s == NULL) {\n", ptrVar, GetName(token->sym));
		fprintf(fp, "\t\tNotifyError(wxT(\"%%s: %s not set.\"), "
		  "funcName);\n", Capital(GetName(token->sym)));
		fprintf(fp, "\t\treturn(FALSE);\n");
		fprintf(fp, "\t}\n");
		arrayLimit = FindArrayLimit(pc, GetName(token->sym));
		fprintf(fp, "\tif (theIndex > %s->%s - 1) {\n", ptrVar,
		  GetName(arrayLimit->sym));
		fprintf(fp, "\t\tNotifyError(wxT(\"%%s: Index value must be in the "
		  "range 0 - %%d (%%d).\"),\n");
		fprintf(fp, "\t\t  funcName, %s->%s - 1, theIndex);\n", ptrVar,
		  GetName(arrayLimit->sym));
		fprintf(fp, "\t\treturn(FALSE);\n");
		fprintf(fp, "\t}\n");
	}
	if (type->sym->type == BOOLSPECIFIER) {
		fprintf(fp, "\tif ((specifier = Identify_NameSpecifier(the%s,\n"
		  "\t\tBooleanList_NSpecLists(0))) == GENERAL_BOOLEAN_NULL) {\n",
		  Capital(variableName));
		fprintf(fp, "\t\tNotifyError(wxT(\"%%s: Illegal switch state (%%s).\"), "
		  "funcName, the%s);\n", Capital(variableName));
		fprintf(fp, "\t\treturn(FALSE);\n");
		fprintf(fp, "\t}\n");
	}
	if (type->sym->type == NAMESPECIFIER) {
		fprintf(fp, "\tif ((specifier = Identify_NameSpecifier(the%s,\n"
		  "\t\t%s->%sList)) == %s_%s_NULL) {\n", Capital(variableName),
		  ptrVar, variableName, CreateBaseModuleName(module, qualifier, TRUE),
		  UpperCase(GetName(token->sym)));
		fprintf(fp, "\t\tNotifyError(wxT(\"%%s: Illegal name (%%s).\"), "
		  "funcName, the%s);\n", Capital(variableName));
		fprintf(fp, "\t\treturn(FALSE);\n");
		fprintf(fp, "\t}\n");
	}
	if (type->sym->type == INT_AL) {
		fprintf(fp, "\tif (the%s < 1) {\n", Capital(variableName));
		Print(fp, "\t\t  ", "\t\tNotifyError(wxT(\"%s: Value must be greater then "
		  "zero (%d).\"), funcName, the");
		Print(fp, "\t\t  ", Capital(variableName));
		Print(fp, "\t\t  ", ");\n");
		Print(fp, "\t\t  ", "");
		fprintf(fp, "\t\treturn(FALSE);\n");
		fprintf(fp, "\t}\n");
		fprintf(fp, "\tif (!Alloc%s_%s(the%s)) {\n", Capital(variableName),
		  CreateBaseModuleName(module, qualifier, FALSE), Capital(
		  variableName));
		Print(fp, "\t\t  ", "\t\tNotifyError(wxT(_\"%s: Cannot allocate memory "
		  "for the '");
		Print(fp, "\t\t  ", variableName);
		Print(fp, "\t\t  ", "' arrays._\"), funcName);\n");
		Print(fp, "\t\t  ", "");
		fprintf(fp, "\t\treturn(FALSE);\n");
		fprintf(fp, "\t}\n");
	}

	fprintf(fp, "\t/*** Put any other required checks here. ***/\n");

	if ((type->sym->type == CFLISTPTR) && (functionType !=
	  SET_BANDWIDTHS_ROUTINE)) { 
		fprintf(fp, "\tif (!CheckPars_CFList(theCFList)) {\n");
		fprintf(fp, "\t\tNotifyError(wxT(\"%%s: Centre frequency structure not "
		  "correctly set.\"),\n\t\t  funcName);\n");
		fprintf(fp, "\t\treturn(FALSE);\n");
		fprintf(fp, "\t}\n");
		fprintf(fp, "\tif (%s->%s != NULL)\n", ptrVar, GetName(token->sym));
		fprintf(fp, "\t\tFree_CFList(&%s->%s);\n", ptrVar, GetName(token->sym));
		sprintf(assignmentString, "\t%s->%s = theCFList;\n", ptrVar,
		  GetName(token->sym));
	}

	if (type->sym->type == PARARRAY) { 
		fprintf(fp, "\tif (!CheckInit_ParArray(the%s, funcName)) {\n", Capital(
		  GetName(token->sym)));
		fprintf(fp, "\t\tNotifyError(wxT(\"%%s: ParArray structure not correctly "
		  "set.\"),  funcName);\n");
		fprintf(fp, "\t\treturn(FALSE);\n");
		fprintf(fp, "\t}\n");
		fprintf(fp, "\tif (%s->%s != NULL)\n", ptrVar, GetName(token->sym));
		fprintf(fp, "\t\tFree_ParArray(&%s->%s);\n", ptrVar, GetName(token->
		  sym));
	}

	if (type->sym->type == DATUMPTR) { 
		fprintf(fp, "\tif (!InitSimulation_Utility_SimScript((the%s)) {\n",
		  Capital(GetName(token->sym)));
		fprintf(fp, "\t\tNotifyError(wxT(\"%%s: Simulation not correctly "
		  "initialised.\"),  funcName);\n");
		fprintf(fp, "\t\treturn(FALSE);\n");
		fprintf(fp, "\t}\n");
	}

	if (functionType == SET_BANDWIDTHS_ROUTINE) {
		fprintf(fp, "\tif (!SetBandwidths_CFList(%s->%s, theBandwidthMode,\n",
		  ptrVar, GetName(token->sym));
		fprintf(fp, "\t  theBandwidths)) {\n");
		fprintf(fp, "\t\tNotifyError(wxT(\"%%s: Failed to set bandwidth mode.\"), "
		  "funcName);\n");
		fprintf(fp, "\t\treturn(FALSE);\n");
		fprintf(fp, "\t}\n");
	}
	
	if (processVarsFlag)
		fprintf(fp, "\t%s->updateProcessVariablesFlag = TRUE;\n", ptrVar);

	if (type->sym->type != INT_AL) {
		fprintf(fp, "%s", assignmentString);
		switch (type->sym->type) {
		case CFLISTPTR:
		case FILENAME:
			break;
		case BOOLSPECIFIER:
		case NAMESPECIFIER:
			fprintf(fp, "specifier;\n");
			break;
		default:
			fprintf(fp, "the%s;\n", Capital(variableName));
		}
	}
	fprintf(fp, "\treturn(TRUE);\n");
	fprintf(fp, "\n}\n\n");
	AddRoutine(funcDeclaration);

}

/****************************** PrintSetFunctions *****************************/

/*
 * This routine prints the Module parameter setting routines.
 * It adds the function declaration to the main list.
 */

void
PrintSetFunctions(FILE *fp)
{
	TokenPtr	p, type, identifierList[MAX_IDENTIFIERS], *list;
	
	p = FindTokenType(STRUCT, pc);
	for (p = p->next; p = GetType_IdentifierList(&type, identifierList, p); )
		for (list = identifierList; *list != 0; list++) {
			PrintSetFunction(fp, *list, type, SET_STANDARD_ROUTINE);
			if (((*list)->inst == POINTER) && FindArrayLimit(pc,
			  GetName((*list)->sym)))
				PrintSetFunction(fp, *list, type, SET_ARRAY_ELEMENT_ROUTINE);
			if (type->sym->type == CFLISTPTR)
				PrintSetFunction(fp, *list, type, SET_BANDWIDTHS_ROUTINE);
				
		}

}

/****************************** PrintGetFunctions *****************************/

/*
 * This routine prints the Module parameter get routines.
 * It adds the function declaration to the main list.
 */

void
PrintGetFunctions(FILE *fp)
{
	char	function[MAXLINE], *funcName, *funcDeclaration;
	TokenPtr	p, type, identifierList[MAX_IDENTIFIERS], *list;

	p = FindTokenType(STRUCT, pc);
	for (p = p->next; p = GetType_IdentifierList(&type, identifierList, p); )
		for (list = identifierList; *list != 0; list++) {
			if (type->sym->type == CFLISTPTR) {
				strcpy(function, "GetCFListPtr");
				PrintLineCommentHeading(fp, function);
				fprintf(fp,
				  "/*\n"
				  " * This routine returns a pointer to the module's CFList "
				    "data pointer.\n"
				  " */\n\n"
	  			);
				funcName = CreateFuncName(function, module, qualifier);
				funcDeclaration = CreateFuncDeclaration("CFListPtr\n", funcName,
				  "void");
				fprintf(fp, "%s\n{\n", funcDeclaration);
				PrintStaticFuncNameDeclaration(fp, funcName);
				PrintModuleInitCheck(fp);
				fprintf(fp, "\tif (%s->%s == NULL) {\n", ptrVar,
				  GetName((*list)->sym));
				fprintf(fp, "\t\tNotifyError(wxT(\"%%s: CFList data structure has "
				  "not been correctly set.  \"\n\t\t  \"NULL returned.\"), "
				  "funcName);\n");
				fprintf(fp, "\t\treturn(NULL);\n");
				fprintf(fp, "\t}\n");
				fprintf(fp, "\treturn(%s->%s);\n\n", ptrVar,
				  GetName((*list)->sym));
				fprintf(fp, "}\n\n");
				AddRoutine(funcDeclaration);
			}
				
		}

}

/****************************** PrintGetNumXParsRoutines **********************/

/*
 * This routine prints the Module parameter GetNum routines.
 * It adds the function declaration to the main list.
 */

void
PrintGetNumXParsRoutines(FILE *fp)
{
	char	function[MAXLINE], *funcName, *funcDeclaration, *baseModuleName;
	TokenPtr	p, type, identifierList[MAX_IDENTIFIERS], *list;

	p = FindTokenType(STRUCT, pc);
	for (p = p->next; p = GetType_IdentifierList(&type, identifierList, p); )
		for (list = identifierList; *list != 0; list++) {
			if (type->sym->type == PARARRAY) {
				sprintf(function, "GetNum%sPars", Capital(GetName((*list)->
				  sym)));
				PrintLineCommentHeading(fp, function);
				fprintf(fp, "/*\n");
				Print(fp, " * ", " * This function returns the number of "
				  "parameters for the respective ");
				Print(fp, " * ", GetName((*list)->sym));
				Print(fp, " * ", " parameter array structure. Using it helps "
				  "maintain the correspondence between the mode names.\n");
				Print(fp, " * ", "");
				fprintf(fp, " */\n\n");
				funcName = CreateFuncName(function, module, qualifier);
				funcDeclaration = CreateFuncDeclaration("int\n", funcName,
				  "int mode");
				fprintf(fp, "%s\n{\n", funcDeclaration);
				PrintStaticFuncNameDeclaration(fp, funcName);
				baseModuleName = CreateBaseModuleName(module, qualifier, TRUE);
				fprintf(fp, "\tswitch (mode) {\n");
				fprintf(fp, "\tcase %s_%s_XXX_MODE:\n", baseModuleName,
				  UpperCase(GetName((*list)->sym)));
				fprintf(fp, "\t\tbreak;\n");
				fprintf(fp, "\tdefault:\n");
				fprintf(fp, "\t\tNotifyError(wxT(\"%%s: Mode not listed (%%d), "
				  "returning zero.\"), funcName,\n");
				fprintf(fp, "\t\t  mode);\n");
				fprintf(fp, "\t\treturn(0);\n");
				fprintf(fp, "\t}\n");
				fprintf(fp, "}\n\n");
				AddRoutine(funcDeclaration);
			}
				
		}

}

