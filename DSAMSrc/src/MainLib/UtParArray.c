/**********************
 *
 * File:		UtParArray.c
 * Purpose:		This Module contains the parameter array structure, where the
 *				length of the array is defined by the mode.
 * Comments:	Originally created for use in the MoDRNL filter.
 * Author:		L. P. O'Mard
 * Created:		01 Sep 2000
 * Updated:		
 * Copyright:	(c) 2000, University of Essex.
 *
 *********************/

#include <stdlib.h> 
#include <stdio.h>
#include <string.h>

#include "GeCommon.h"
#include "GeUniParMgr.h"
#include "FiParFile.h"
#include "UtString.h"
#include "UtParArray.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Functions and subroutines *********************/
/******************************************************************************/

/****************************** Free ******************************************/

/*
 * This function releases of the memory allocated for a parameter array
 * structure.
 */

void
Free_ParArray(ParArrayPtr *parArray)
{
	if (!*parArray)
		return;
	if ((*parArray)->params)
		free((*parArray)->params);
	if ((*parArray)->parList)
		FreeList_UniParMgr(&(*parArray)->parList);
	free(*parArray);
	*parArray = NULL;

}

/****************************** Init ******************************************/

/*
 * This function initialises a 'ParArray' structure.
 * It returns NULL if it fails in any way.
 */

ParArrayPtr
Init_ParArray(char *name, NameSpecifier *modeList, int (* GetNumPars)(int))
{
	static const char *funcName = "Init_ParArray";
	BOOLN	lastListEntryFound = FALSE;
	char	workStr[LONG_STRING];
	ParArrayPtr p;
	NameSpecifierPtr	list;

	if (!name) {
		NotifyError("%s: Name not initialised.", funcName);
		return(NULL);
	}
	if (!modeList) {
		NotifyError("%s: Mode list not initialised.", funcName);
		return(NULL);
	}
	if ((p = (ParArrayPtr) malloc(sizeof(ParArray))) == NULL) {
		NotifyError("%s: Out of memory for structure.", funcName);
		return(NULL);
	}
	snprintf(p->name, MAXLINE, "%s", name);
	p->updateFlag = TRUE;
	p->mode = PARARRAY_NULL;
	p->params = NULL;
	p->numParams = -1;
	p->GetNumPars = GetNumPars;
	p->modeList = modeList;
	p->parList = NULL;

	ToUpper_Utility_String(workStr, p->name);
	snprintf(p->abbr[PARARRAY_MODE], MAXLINE, "%s_MODE", workStr);
	snprintf(p->abbr[PARARRAY_PARAMETER], MAXLINE, "%s_PARAMETER", workStr);
	snprintf(p->desc[PARARRAY_MODE], MAXLINE, "Variable '%s' mode (", name);
	snprintf(p->desc[PARARRAY_PARAMETER], MAXLINE, "Parameters for '%s' "
	  "function", name);
	for (list = p->modeList; *list->name != '\0'; list++) {
		snprintf(workStr, LONG_STRING, "'%s'", list->name);
		strcat(p->desc[PARARRAY_MODE], workStr);
		if (!lastListEntryFound && *(list + 2)->name == '\0') {
			strcat(p->desc[PARARRAY_MODE], " or ");
			lastListEntryFound = TRUE;
		} else {
			if (*(list + 1)->name != '\0')
				strcat(p->desc[PARARRAY_MODE], ", ");
		}
	}
	strcat(p->desc[PARARRAY_MODE], ").");
	if (!SetMode_ParArray(p, p->modeList[0].name)) {
		NotifyError("Could not set initial parameter array.", funcName);
		Free_ParArray(&p);
		return(NULL);
	}
	return(p);

}

/****************************** CheckInit *************************************/

/*
 * This function checks that a ParArray structure is correctly initialised.
 * It returns FALSE if it fails in any way.
 */

BOOLN
CheckInit_ParArray(ParArrayPtr parArray, const char *callingFunction)
{
	static const char	*funcName = "CheckInit_ParArray";

	if (parArray == NULL) {
		NotifyError("%s: ParArray not set in %s.", funcName, callingFunction);
		return(FALSE);
	}
	return(TRUE);

}

/********************************* SetGeneralUniParList ***********************/

/*
 * This routine initialises and sets the IonChanList's universal parameter list.
 * This list provides universal access to the IonChanList's parameters.
 */
 
BOOLN
SetUniParList_ParArray(ParArrayPtr parArray)
{
	static const char *funcName = "SetUniParList_ParArray";
	UniParPtr	pars;

	if (!CheckInit_ParArray(parArray, funcName))
		return(FALSE);
	if (!parArray->parList && (parArray->parList = InitList_UniParMgr(
	  UNIPAR_SET_PARARRAY, PARARRAY_NUM_PARS, parArray)) == NULL) {
		NotifyError("%s: Could not initialise parList.", funcName);
		return(FALSE);
	}
	pars = parArray->parList->pars;
	SetPar_UniParMgr(&pars[PARARRAY_MODE], parArray->abbr[PARARRAY_MODE],
	  parArray->desc[PARARRAY_MODE],
	  UNIPAR_NAME_SPEC,
	  &parArray->mode, parArray->modeList,
	  (void * (*)) SetMode_ParArray);
	SetPar_UniParMgr(&pars[PARARRAY_PARAMETER], parArray->abbr[
	  PARARRAY_PARAMETER],
	  parArray->desc[PARARRAY_PARAMETER],
	  UNIPAR_REAL_ARRAY,
	  &parArray->params, &parArray->numParams,
	  (void * (*)) SetIndividualPar_ParArray);
	return(TRUE);

}

/********************************* SetMode ************************************/

/*
 * This routine sets the mode for a parameter array list.
 */

BOOLN
SetMode_ParArray(ParArrayPtr parArray, char *modeName)
{
	static const char *funcName = "SetMode_ParArray";
	int		mode, newNumParams;

	if (!CheckInit_ParArray(parArray, funcName))
		return(FALSE);
	mode = Identify_NameSpecifier(modeName, parArray->modeList);
	if (parArray->modeList[mode].name[0] == '\0') {
		NotifyError("%s: Unknown '%s' mode (%s).", funcName, parArray->name,
		  modeName);
		return(FALSE);
	}
	parArray->mode = mode;
	newNumParams = (parArray->GetNumPars)(mode);
	if (parArray->numParams != newNumParams) {
		parArray->numParams = newNumParams;
		if (parArray->params)
			free(parArray->params);
		if ((parArray->params = (double *) calloc(newNumParams, sizeof(
		  double))) == NULL) {
			NotifyError("%s: Out of memory for parameters (%d)", funcName,
			  newNumParams);
			return(FALSE);
		}
	}
	SetUniParList_ParArray(parArray);
	parArray->parList->updateFlag = TRUE;
	parArray->updateFlag = TRUE;
	return(TRUE);

}

/********************************* SetIndividualPar ***************************/

/*
 * This function sets the individual frequency values of a ParArray.
 * It first checks if the frequencies have been set.
 */

BOOLN
SetIndividualPar_ParArray(ParArrayPtr parArray, int theIndex, double parValue)
{
	static const char *funcName = "SetIndividualPar_ParArray";

	if (!CheckInit_ParArray(parArray, funcName))
		return(FALSE);
	if (parArray->params == NULL) {
		NotifyError("%s: parameters not set.", funcName);
		return(FALSE);
	}
	if (theIndex > parArray->numParams - 1) {
		NotifyError("%s: Index value must be in the\nrange 0 - %d (%d).\n",
		  funcName, parArray->numParams - 1, theIndex);
		return(FALSE);
	}
	parArray->params[theIndex] = parValue;
	parArray->updateFlag = TRUE;
	return(TRUE);

}

/****************************** ReadPars **************************************/

/*
 * This routine reads the ParArray parameters from a file stream.
 * It returns with the address of the ParArray, or NULL if it fails.
 */

BOOLN
ReadPars_ParArray(FILE *fp, ParArrayPtr parArray)
{
	static const char *funcName = "ReadPars_ParArray";
	BOOLN	ok = TRUE;
	char	modeName[MAXLINE];
	int		i;
	
	if (!parArray) {
		NotifyError("%s: The 'parArray' structure has not been initialised.",
		  funcName);
		return(FALSE);
	} 
	if (!GetPars_ParFile(fp, "%s", modeName)) {
		NotifyError("%s: Could not find '%s' mode for '%s'.", funcName,
		  modeName, parArray->name);
		return(FALSE);
	}
	
	if (!SetMode_ParArray(parArray, modeName)) {
		NotifyError("%s: Unknown '%s' mode (%s).", funcName, parArray->name,
		  modeName);
		return(FALSE);
	}
	for (i = 0; (i < parArray->numParams) && ok; i++)
		if (!GetPars_ParFile(fp, "%lf", &parArray->params[i]))
			ok = FALSE;
	if (!ok) {
		NotifyError("%s: Failed to read '%s' parameters.", funcName,
		  parArray->name);
		return(FALSE);
	}
	return(TRUE);

}

/****************************** PrintPars *************************************/

/*
 * This routine prints the parameters associated with a ParArray data structure.
 */

void
PrintPars_ParArray(ParArrayPtr parArray)
{
	static const char *funcName = "PrintPars_ParArray";
	int		i;
	
	if (!CheckInit_ParArray(parArray, funcName)) {
		NotifyError("%s: Parameter Array not correctly set.",
		  funcName);
		return;
	}
	DPrint("\tVariable '%s' parameters:-\n", parArray->name);
	DPrint("\t\tmode: %s:\n", parArray->modeList[parArray->mode].name);
	DPrint("\t\t%10s\t%10s\n", "Param No.", "Parameter");
	for (i = 0; i < parArray->numParams; i++) {
		DPrint("\t\t%10d\t%10g\n", i, parArray->params[i]);
	}

}

