/******************
 *
 * File:		GeUniParMgr.c
 * Purpose:		This file contains the universal parameter manager routines
 * Comments:	Note that the "UNIPAR_STRING" values only have the initial
 *				values.  I have got to check this.
 *				08-10-98 LPO: Fixed bug with name specifiers, by introducing
 *				the 'UNIPAR_NAME_SPEC' type: 'LINEAR' mode was being selected
 *				by the system when 'SINE' was asked for - I hadn't noticed that.
 *				12-10-98 LPO: Parameter lists that return structures, such as
 *				the CFList, will only carry out the setting function when all
 *				parameters have been set.
 *				06-11-98 LPO: Implemented the array handling for the likes of
 *				the StMPTone module.
 *				10-12-98 LPO: Introduced handling of NULL parLists, i.e. for 
 *				modules with no parameters.
 *				11-12-98 LPO: Introduced the second array index for the IC list.
 *				19-01-99 LPO: The 'arrayIndex[0]' always refers to the last
 *				(lowest?) index level, i.e. for the 'ICList' mode
 *				'arrayIndex[1]' refers to the ion ionChannel, and
 *				'arrayIndex[0]' refers to the respective ion channel parameter
 *				array element.
 *				27-01-99 LPO: Corrected problem in 'FindUniPar_' where it was
 *				returning before checking the rest of a parameter list after a
 *				sub module's parameter list.
 *				29-04-99 LPO: The 'FindUniPar_' routine can now find the 
 *				'CFLIST' abbreviation so that the 'CFListPtr' pointer can be
 *				accessed.
 *				19-05-99 LPO: I have changed the 'UNIPAR_FILE_NAME' code so that
 *				I can save the 'defaultExtension' to be used with the GUI.
 *				30-05-99 LPO: The strings and file names are now printed
 *				surrounded by speech marks, '"'.
 *				02-06-99 LPO: I have updated the 'FindUniPar_' routine so that
 *				it now will find the parameters within a simulation script.
 *				03-06-99 LPO: Corrected the problem with 'SetParValue_' not
 *				treating the general list parameters correctly.
 *				08-09-99 LPO: In the 'FindUniPar_' routine only the sub-
 *				parameters for a module would be set, and not the module
 *				parameter file name itself.  This has been fixed.
 *				26-10-99 LPO: Introduced the 'enabled' flag for the 'UniPar'
 *				structure.  Printing, setting operations and such like will not
 *				be carried out on 'disabled' parameters.
 * Authors:		L. P. O'Mard
 * Created:		24 Sep 1998
 * Updated:		08 Sep 1999
 * Copyright:	(c) 1999, University of Essex.
 * 
 ******************/

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG_H */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "GeCommon.h"
#include "GeSignalData.h"
#include "GeEarObject.h"
#include "GeNSpecLists.h"
#include "UtNameSpecs.h"
#include "UtBandwidth.h"
#include "UtCFList.h"
#include "UtIonChanList.h"
#include "UtParArray.h"
#include "UtString.h"
#include "UtDatum.h"
#include "GeUniParMgr.h"

/******************************************************************************/
/************************ Global variables ************************************/
/******************************************************************************/

/******************************************************************************/
/************************ Subroutines and functions ***************************/
/******************************************************************************/

/************************ InitList ********************************************/

/*
 * This routine initialises a universal parameter list structure.
 */

UniParListPtr
InitList_UniParMgr(UniParModeSpecifier mode, int numPars, void *handlePtr)
{
	static const char *funcName = "IniList_UniParMgr";
	int		i;
	UniParListPtr	p;

	if ((p = (UniParListPtr) malloc(sizeof(UniParList))) == NULL) {
		NotifyError("%s: Could not allocate UniParList.", funcName);
		return(NULL);
	}
	if ((p->pars = (UniPar *) calloc(numPars, sizeof(UniPar))) == NULL) {
		NotifyError("%s: Could not allocate %d UniPars.", funcName, numPars);
		FreeList_UniParMgr(&p);
		return(NULL);
	}
	p->updateFlag = FALSE;
	p->notebookPanel = -1;
	p->mode = mode;
	p->numPars = numPars;
	switch (mode) {
	case UNIPAR_SET_CFLIST:
		p->handlePtr.cFs = (CFListPtr) handlePtr;
		break;
	case UNIPAR_SET_IC:
	case UNIPAR_SET_ICLIST:
		p->handlePtr.iCs = (IonChanListPtr) handlePtr;
		break;
	case UNIPAR_SET_PARARRAY:
		p->handlePtr.parArray.ptr = (ParArrayPtr) handlePtr;
		p->handlePtr.parArray.SetFunc = NULL;
		break;
	default:
		;
	}
	for (i = 0; i < numPars; i++) {
		p->pars[i].mode = mode;
		p->pars[i].index = i;
	}
	p->GetPanelList = NULL;
	return(p);
	
}

/************************ FreeList ********************************************/

/*
 * This routine frees the memory allocated for a universal parameter list
 * structure.
 * It also sets the pointer, passed by reference, to NULL;
 */

void
FreeList_UniParMgr(UniParListPtr *list)
{
	if (*list == NULL)
		return;
	if ((*list)->pars)
		free((*list)->pars);
	free(*list);
	*list = NULL;

}

/************************ SetPar **********************************************/

/*
 * This routine sets the members of a universal parameter structure.
 * The 'mode' member is set elsewhere e.g., InitList_UniParMgr.
 */

void
SetPar_UniParMgr(UniParPtr par, char *abbreviation, char *description,
  UniParTypeSpecifier type, void *ptr1, void *ptr2, void * (* Func))
{
	static const char	*funcName = "SetPar_UniParMgr";

	par->enabled = TRUE;
	par->type = type;
	par->abbr = abbreviation;
	par->desc = description;
	switch (type) {
	case UNIPAR_INT:
	case UNIPAR_BOOL:
		par->valuePtr.i = (int *) ptr1;
		break;
	case UNIPAR_INT_ARRAY:
		par->valuePtr.array.index = 0;
		par->valuePtr.array.pPtr.i = (int **) ptr1;
		par->valuePtr.array.numElements = (int *) ptr2;
		break;
	case UNIPAR_LONG:
		par->valuePtr.l = (long *) ptr1;
		break;
	case UNIPAR_REAL:
		par->valuePtr.r = (double *) ptr1;
		break;
	case UNIPAR_REAL_ARRAY:
		par->valuePtr.array.index = 0;
		par->valuePtr.array.pPtr.r = (double **) ptr1;
		par->valuePtr.array.numElements = (int *) ptr2;
		break;
	case UNIPAR_STRING:
		par->valuePtr.s = (char *) ptr1;
		break;
	case UNIPAR_FILE_NAME:
		par->valuePtr.file.name = (char *) ptr1;
		par->valuePtr.file.defaultExtension = (char *) ptr2;
		break;
	case UNIPAR_MODULE:
		par->valuePtr.module.parFile = (char *) ptr1;
		par->valuePtr.module.parList = (UniParListPtr) ptr2;
		break;
	case UNIPAR_CFLIST:
		par->valuePtr.cFPtr = (CFListPtr *) ptr1;
		break;
	case UNIPAR_PARARRAY:
		par->valuePtr.pAPtr = (ParArrayPtr *) ptr1; 
		break;
	case UNIPAR_ICLIST:
		par->valuePtr.iCPtr = (IonChanListPtr *) ptr1;
		break;
	case UNIPAR_NAME_SPEC:
	case UNIPAR_NAME_SPEC_WITH_FILE:
	case UNIPAR_NAME_SPEC_WITH_FPATH:
		par->valuePtr.nameList.specifier = (int *) ptr1;
		par->valuePtr.nameList.list = (NameSpecifier *) ptr2;
		break;
	case UNIPAR_SIMSCRIPT:
		par->valuePtr.simScript.simulation = (DatumPtr *) ptr1;
		par->valuePtr.simScript.fileName = (char *) ptr2;
		break;
	case UNIPAR_PARLIST:
		par->valuePtr.parList = (UniParListPtr *) ptr1;
		break;
	default:
		NotifyError("%s: Universal parameter not yet implemented (%d).",
		  funcName, type);
	}
	switch (par->mode) {
	case UNIPAR_SET_GENERAL:
	case UNIPAR_SET_SIMSPEC:
		switch (type) {
		case UNIPAR_INT:
			par->FuncPtr.SetInt = (BOOLN (*)(int)) Func;
			break;
		case UNIPAR_INT_ARRAY:
			par->FuncPtr.SetIntArrayElement = (BOOLN (*)(int , int)) Func;
			break;
		case UNIPAR_LONG:
			par->FuncPtr.SetLong = (BOOLN (*)(long)) Func;
			break;
		case UNIPAR_REAL:
			par->FuncPtr.SetReal = (BOOLN (*)(double)) Func;
			break;
		case UNIPAR_REAL_ARRAY:
			par->FuncPtr.SetRealArrayElement = (BOOLN (*)(int, double)) Func;
			break;
		case UNIPAR_BOOL:
		case UNIPAR_STRING:
		case UNIPAR_MODULE:
		case UNIPAR_NAME_SPEC:
		case UNIPAR_NAME_SPEC_WITH_FILE:
		case UNIPAR_NAME_SPEC_WITH_FPATH:
		case UNIPAR_FILE_NAME:
		case UNIPAR_PARLIST:
			par->FuncPtr.SetString = (BOOLN (*)(char *)) Func;
			break;
		case UNIPAR_CFLIST:
			par->FuncPtr.SetCFList = (BOOLN (*)(CFListPtr)) Func;
			break;
		case UNIPAR_PARARRAY:
			if (!*par->valuePtr.pAPtr || !(*par->valuePtr.pAPtr)->parList)
				NotifyError("%s: Could not set par array handle function.",
				  funcName);			
			(*par->valuePtr.pAPtr)->parList->handlePtr.parArray.SetFunc =
			  (BOOLN (*)(ParArrayPtr)) Func;
			break;
		case UNIPAR_ICLIST:
			par->FuncPtr.SetICList = (BOOLN (*)(IonChanListPtr)) Func;
			break;
		case UNIPAR_SIMSCRIPT:
			par->FuncPtr.SetDatumPtr = (BOOLN (*)(DatumPtr)) Func;
			break;
		default:
			NotifyError("%s: Universal parameter not yet implemented (%d).",
			  funcName, type);
		}
		break;
	case UNIPAR_SET_CFLIST:
		switch (type) {
		case UNIPAR_INT:
			par->FuncPtr.SetCFListInt = (BOOLN (*)(CFListPtr, int)) Func;
			break;
		case UNIPAR_REAL:
			par->FuncPtr.SetCFListReal = (BOOLN (*)(CFListPtr, double)) Func;
			break;
		case UNIPAR_REAL_ARRAY:
			par->FuncPtr.SetCFListRealArrayElement = (BOOLN (*)(CFListPtr, int,
			  double)) Func;
			break;
		case UNIPAR_BOOL:
		case UNIPAR_STRING:
		case UNIPAR_MODULE:
		case UNIPAR_NAME_SPEC:
		case UNIPAR_NAME_SPEC_WITH_FILE:
		case UNIPAR_NAME_SPEC_WITH_FPATH:
			par->FuncPtr.SetCFListString = (BOOLN (*)(CFListPtr, char *)) Func;
			break;
		default:
			NotifyError("%s: Universal parameter (CFList) not yet implemented "
			  "(%d).", funcName, type);
		}
		break;
	case UNIPAR_SET_PARARRAY:
		switch (type) {
		case UNIPAR_INT:
			par->FuncPtr.SetParArrayInt = (BOOLN (*)(ParArrayPtr, int)) Func;
			break;
		case UNIPAR_REAL_ARRAY:
			par->FuncPtr.SetParArrayRealArrayElement = (BOOLN (*)(ParArrayPtr,
			  int, double)) Func;
			break;
		case UNIPAR_BOOL:
		case UNIPAR_STRING:
		case UNIPAR_MODULE:
		case UNIPAR_NAME_SPEC:
		case UNIPAR_NAME_SPEC_WITH_FILE:
		case UNIPAR_NAME_SPEC_WITH_FPATH:
			par->FuncPtr.SetParArrayString = (BOOLN (*)(ParArrayPtr,
			  char *)) Func;
			break;
		default:
			NotifyError("%s: Universal parameter (ParArray) not yet "
			  "implemented (%d).", funcName, type);
		}
		break;
	case UNIPAR_SET_ICLIST:
		switch (type) {
		case UNIPAR_INT:
			par->FuncPtr.SetICListInt = (BOOLN (*)(IonChanListPtr, int)) Func;
			break;
		case UNIPAR_REAL:
			par->FuncPtr.SetICListReal = (BOOLN (*)(IonChanListPtr,
			  double)) Func;
			break;
		case UNIPAR_BOOL:
		case UNIPAR_STRING:
		case UNIPAR_MODULE:
		case UNIPAR_NAME_SPEC:
		case UNIPAR_NAME_SPEC_WITH_FILE:
		case UNIPAR_NAME_SPEC_WITH_FPATH:
			par->FuncPtr.SetICListString = (BOOLN (*)(IonChanListPtr,
			  char *)) Func;
			break;
		default:
			NotifyError("%s: Universal parameter (IonChanList) not yet "
			  "implemented (%d).", funcName, type);
		}
		break;
	case UNIPAR_SET_IC:
		switch (type) {
		case UNIPAR_INT:
			par->FuncPtr.SetICInt = (BOOLN (*)(IonChannelPtr, int)) Func;
			break;
		case UNIPAR_REAL:
			par->FuncPtr.SetICReal = (BOOLN (*)(IonChannelPtr, double)) Func;
			break;
		case UNIPAR_REAL_ARRAY:
			par->FuncPtr.SetICRealArrayElement = (BOOLN (*)(IonChannelPtr, int,
			  double)) Func;
			break;
		case UNIPAR_BOOL:
		case UNIPAR_STRING:
		case UNIPAR_MODULE:
		case UNIPAR_NAME_SPEC:
		case UNIPAR_NAME_SPEC_WITH_FILE:
		case UNIPAR_NAME_SPEC_WITH_FPATH:
		case UNIPAR_FILE_NAME:
			par->FuncPtr.SetICString = (BOOLN (*)(IonChannelPtr, char *)) Func;
			break;
		default:
			NotifyError("%s: Universal parameter (IonChannel) not yet "
			  "implemented (%d).", funcName, type);
		}
		break;
	default:
		;
	}

}

/************************ SetPanelListFunction ********************************/

/*
 * Sets the function that returns the panel list for displaying the parameters
 * of a module on different notebook pages.
 */

BOOLN
SetGetPanelListFunc_UniParMgr(UniParListPtr list, NameSpecifier * (* Func)(int))
{
	static const char *funcName = "SetPanelListFunc_UniParMgr";

	if (list == NULL) {
		NotifyError("%s: List not initialised.", funcName);
		return(FALSE);
	}
	list->GetPanelList = Func;
	return(TRUE);

}

/************************ PrintValue ******************************************/

/*
 * Print the value in a universal parameter.
 */

BOOLN
PrintValue_UniParMgr(UniParPtr p)
{
	static const char	*funcName = "PrintValue_UniParMgr";

	switch (p->type) {
	case UNIPAR_BOOL:
		DPrint("%-10s\t", BooleanList_NSpecLists(*p->valuePtr.i)->name);
		break;
	case UNIPAR_INT:
		DPrint("%-10d\t", *p->valuePtr.i);
		break;
	case UNIPAR_LONG:
		DPrint("%-10ld\t", *p->valuePtr.l);
		break;
	case UNIPAR_REAL:
		DPrint("%-10g\t", *p->valuePtr.r);
		break;
	case UNIPAR_STRING:
		DPrint("%-10s\t", QuotedString_Utility_String(p->valuePtr.s));
		break;
	case UNIPAR_FILE_NAME:
		DPrint("%-10s\t", QuotedString_Utility_String(p->valuePtr.file.name));
		break;
	case UNIPAR_NAME_SPEC:
	case UNIPAR_NAME_SPEC_WITH_FILE:
	case UNIPAR_NAME_SPEC_WITH_FPATH:
		DPrint("%-10s\t", QuotedString_Utility_String(p->valuePtr.nameList.list[
		  *p->valuePtr.nameList.specifier].name));
		break;
	default:
		NotifyError("%s: Universal parameter not yet implemented (%d).",
		  funcName, p->type);
		return(FALSE);
	}
	return(TRUE);

}

/************************ PrintArray ******************************************/

/*
 * Print the array values in a universal parameter.
 * It returns FALSE if it fails in anry way.
 */

BOOLN
PrintArray_UniParMgr(UniParPtr p, char *suffix)
{
	static const char	*funcName = "PrintArray_UniParMgr";
	int		i;

	for (i = 0; i < *p->valuePtr.array.numElements; i++)
		switch (p->type) {
		case UNIPAR_INT_ARRAY:
			DPrint("\t\t%s%s\t%3d:%-10d\n", p->abbr, suffix, i,
			  (*p->valuePtr.array.pPtr.i)[i]);
			break;
		case UNIPAR_REAL_ARRAY:
			DPrint("\t\t%s%s\t%3d:%-10g\n", p->abbr, suffix, i,
			  (*p->valuePtr.array.pPtr.r)[i]);
			break;
		default:
			NotifyError("%s: Universal parameter not yet implemented (%d).",
			  funcName, p->type);
			return(FALSE);
		}
	return(TRUE);

}

/************************ PrintPars *******************************************/

/*
 * Print the parameters in a universal parameter list.
 */

BOOLN
PrintPars_UniParMgr(UniParListPtr list, char *prefix, char *suffix)
{
	static const char *funcName = "PrintPars_UniParMgr";
	BOOLN	ok = TRUE;
	char	string[LONG_STRING];
	int		i;
	UniParPtr	p;
	DynaListPtr	node;

	if (list == NULL) {
		NotifyError("%s: List not initialised.", funcName);
		return(FALSE);
	}
	for (i = 0; i < list->numPars; i++) {
		p = &list->pars[i];
		switch (p->type) {
		case UNIPAR_BOOL:
		case UNIPAR_INT:
		case UNIPAR_LONG:
		case UNIPAR_REAL:
		case UNIPAR_STRING:
		case UNIPAR_NAME_SPEC:
		case UNIPAR_NAME_SPEC_WITH_FILE:
		case UNIPAR_NAME_SPEC_WITH_FPATH:
		case UNIPAR_FILE_NAME:
			if ((strlen(p->abbr) + strlen(suffix)) >= LONG_STRING) {
				NotifyError("%s: Combined string '%s%s' is greater than %d.",
				  funcName, p->abbr, suffix, MAXLINE);
				return(FALSE);
			}
			snprintf(string, LONG_STRING, "%s%s", p->abbr, suffix);
			DPrint("\t%s%-25s\t", prefix, string);
			PrintValue_UniParMgr(p);
			DPrint("%s\n", p->desc);
			break;
		case UNIPAR_MODULE:
			DPrint("\t# Module parameter file: %s\n",
			  p->valuePtr.module.parFile);
			ok = PrintPars_UniParMgr(p->valuePtr.module.parList,
			  UNIPAR_SUB_PAR_LIST_MARKER, suffix);
			break;
		case UNIPAR_PARLIST:
			if (!*(p->valuePtr.parList))
				break;
			DPrint("\t# Sub-parameter list: \n");
			ok = PrintPars_UniParMgr(*(p->valuePtr.parList),
			  UNIPAR_SUB_PAR_LIST_MARKER, suffix);
			break;
		case UNIPAR_CFLIST:
			DPrint("\t# CFList parameters:\n");
			if (!PrintPars_UniParMgr((*p->valuePtr.cFPtr)->cFParList,
			  UNIPAR_SUB_PAR_LIST_MARKER, suffix))
				ok = FALSE;
			if ((*p->valuePtr.cFPtr)->bandwidth && !PrintPars_UniParMgr(
			  (*p->valuePtr.cFPtr)->bParList, UNIPAR_SUB_PAR_LIST_MARKER,
			  suffix))
				ok = FALSE;
			break;
		case UNIPAR_PARARRAY:
			DPrint("\t# %s parameters:\n", (*p->valuePtr.pAPtr)->name);
			if (!PrintPars_UniParMgr((*p->valuePtr.pAPtr)->parList,
			  UNIPAR_SUB_PAR_LIST_MARKER, suffix))
				ok = FALSE;
			break;
		case UNIPAR_ICLIST:
			DPrint("\t# IonChanList parameters:\n");
			if (!PrintPars_UniParMgr((*p->valuePtr.iCPtr)->parList,
			  UNIPAR_SUB_PAR_LIST_MARKER, suffix))
				ok = FALSE;
			for (node = (*p->valuePtr.iCPtr)->ionChannels; node; node =
			  node->next) {
				IonChannelPtr iC = (IonChannelPtr) node->data;
				DPrint("\t# <---- Ion channel %s ---->\n", iC->description);
				if (!PrintPars_UniParMgr(iC->parList,
				  UNIPAR_SUB_PAR_LIST_MARKER, suffix))
					ok = FALSE;
			}
			break;
		case UNIPAR_SIMSCRIPT:
			DPrint("\t# Parameters for '%s' simulation:\n",
			  p->valuePtr.simScript.fileName);
			PrintParListModules_Utility_Datum(*p->valuePtr.simScript.simulation,
			  UNIPAR_SUB_PAR_LIST_MARKER);
			break;
		case UNIPAR_INT_ARRAY:
		case UNIPAR_REAL_ARRAY:
			DPrint("\t# %s:\n", p->desc);
			PrintArray_UniParMgr(p, suffix);
			break;
		default:
			NotifyError("%s: Universal parameter not yet implemented (%d).",
			  funcName, p->type);
			ok = FALSE;
		}
	}
	return(ok);

}

/************************ ResetCFList *****************************************/

/*
 * This routine resets the CFList pointer handle for a universal parameter list.
 * The re-creation of the CFList structure's lists will will ensure that the
 * all the list 'updateFlag' flags will be set to FALSE.
 * The 'bandwidthMode' structure must be preserved, as it will already
 * have been reset.
 */

BOOLN
ResetCFList_UniParMgr(UniParListPtr parList)
{
	static const char *funcName = "ResetCFListPointer_UniParMgr";
	CFListPtr	theCFs;
	
	theCFs = parList->handlePtr.cFs;
	if (!RegenerateList_CFList(theCFs)) {
		NotifyError("%s: Could not regenerate CFList.", funcName);
		return(FALSE);
	}
	theCFs->bParList->updateFlag = FALSE;
	return(TRUE);

}

/************************ CheckParList ****************************************/

/*
 * This routine checks the universal parameter list's 'updateFlag' and tries to
 * update the associate parameter list handle if necessary.
 * It does this recursively for modules or structures such as CFLists.
 * It returns FALSE if it fails in any way.
 */

BOOLN
CheckParList_UniParMgr(UniParListPtr list)
{
	static const char *funcName = "CheckParList_UniParMgr";
	BOOLN	ok = TRUE;
	int		i;
	UniParPtr	p;

	if (list == NULL) {
		NotifyError("%s: List not initialised.", funcName);
		return(FALSE);
	}
	switch (list->mode) {
	case UNIPAR_SET_GENERAL:
	case UNIPAR_SET_SIMSPEC:
		for (i = 0; i < list->numPars; i++) {
			p = &list->pars[i];
			switch (p->type) {
			case UNIPAR_MODULE:
				ok = CheckParList_UniParMgr(p->valuePtr.module.parList);
				break;
			case UNIPAR_PARLIST:
				if (!*(p->valuePtr.parList))
					break;
				ok = CheckParList_UniParMgr(*(p->valuePtr.parList));
				break;
			case UNIPAR_CFLIST:
				if (!*p->valuePtr.cFPtr) {
					ok = FALSE;
					break;
				}
				if (!CheckParList_UniParMgr((*p->valuePtr.cFPtr)->cFParList))
					ok = FALSE;
				if (!CheckParList_UniParMgr((*p->valuePtr.cFPtr)->bParList))
					ok = FALSE;
				break;
			case UNIPAR_PARARRAY:
				if (!*p->valuePtr.pAPtr) {
					ok = FALSE;
					break;
				}
				if (!CheckParList_UniParMgr((*p->valuePtr.pAPtr)->parList))
					ok = FALSE;
				break;
			case UNIPAR_ICLIST:
				if (!*p->valuePtr.iCPtr || !CheckParList_UniParMgr(
				  (*p->valuePtr.iCPtr)->parList)) {
					ok = FALSE;
					break;
				}
				break;
			default:
				;
			}
		}
		break;
	case UNIPAR_SET_CFLIST:
		if (list->updateFlag)
			ok = ResetCFList_UniParMgr(list);
		break;
	case UNIPAR_SET_PARARRAY:
		break;
	case UNIPAR_SET_ICLIST: {
		DynaListPtr	node;
		IonChanListPtr	theICs = list->handlePtr.iCs;
		IonChannelPtr	iC;

		if (list->updateFlag)
			ok = PrepareIonChannels_IonChanList(theICs);
		for (node = theICs->ionChannels; ok && node; node = node->next) {
			iC = (IonChannelPtr) node->data;
			if (iC->parList->updateFlag)
				ok = ResetIonChannel_IonChanList(theICs, iC);
		}
		break; }
	default:
		;
	}
	list->updateFlag = !ok;
	return(ok);

}

/************************ GetParString ****************************************/

/*
 * This function returns a pointer to string version of a universal parameter.
 */

char *
GetParString_UniParMgr(UniParPtr p)
{
	static const char *funcName = "GetParString_UniParMgr";
	static char string[LONG_STRING];

	if (p == NULL) {
		NotifyError("%s: Parameter not initialised.", funcName);
		return(NULL);
	}
	if (((p->type == UNIPAR_INT_ARRAY) || (p->type == UNIPAR_REAL_ARRAY)) &&
	  (*p->valuePtr.array.numElements == 0))
		return("");
		
	switch (p->type) {
	case UNIPAR_BOOL:
		sprintf(string, "%s", BooleanList_NSpecLists(*p->valuePtr.i)->name);
		break;
	case UNIPAR_INT:
		sprintf(string, "%d", *p->valuePtr.i);
		break;
	case UNIPAR_INT_ARRAY:
		sprintf(string, "%d", (*p->valuePtr.array.pPtr.i)[
		  p->valuePtr.array.index]);
		break;
	case UNIPAR_LONG:
		sprintf(string, "%ld", *p->valuePtr.l);
		break;
	case UNIPAR_REAL:
		sprintf(string, "%g", *p->valuePtr.r);
		break;
	case UNIPAR_REAL_ARRAY:
		sprintf(string, "%g", (*p->valuePtr.array.pPtr.r)[p->valuePtr.array.
		  index]);
		break;
	case UNIPAR_STRING:
		snprintf(string, LONG_STRING, "%s", p->valuePtr.s);
		break;
	case UNIPAR_FILE_NAME:
		snprintf(string, LONG_STRING, "%s", p->valuePtr.file.name);
		break;
	case UNIPAR_MODULE:
		snprintf(string, LONG_STRING, "%s", p->valuePtr.module.parFile);
		break;
	case UNIPAR_NAME_SPEC:
	case UNIPAR_NAME_SPEC_WITH_FILE:
	case UNIPAR_NAME_SPEC_WITH_FPATH:
		snprintf(string, LONG_STRING, "%s", p->valuePtr.nameList.list[
		  *p->valuePtr.nameList.specifier].name);
		break;
	default:
		NotifyError("%s: Universal parameter not yet implemented (%d).",
		  funcName, p->type);
		return("");
	}
	return(string);

}

/************************ ParseArrayValue *************************************/

/*
 * This function parses a value, separating the string into index, parameter
 * values if the UNIPAR_INDEX_SEPARATOR appears in the string.
 * The values are returned in the respective arguments.
 * The index is set to the parameter's index field if no index is specified.
 * The function returns FALSE if it fails in any way.
 */

BOOLN
ParseArrayValue_UniParMgr(UniParPtr par, char *parValue, char **parValuePtr,
  int *index)
{
	static const char *funcName = "ParseArrayValue_UniParMgr";
	char	*p, *s, string[MAXLINE];
	int		i, pos;

	if (!par) {
		NotifyError("%s: Universal parameter not initalised.\n", funcName);
		return(FALSE);
	}
	if ((par->type != UNIPAR_INT_ARRAY) && (par->type != UNIPAR_REAL_ARRAY)) {
		NotifyError("%s: Universal parameter is not array type (%d).\n",
		  funcName, par->type);
		return(FALSE);
	}
	index[0] = par->valuePtr.array.index;
	for (i = 0, s = parValue; (i < UNIPAR_MAX_ARRAY_INDEX) && (p = strchr(s,
	  UNIPAR_INDEX_SEPARATOR)); i++) {
		pos = p - s;
		strncpy(string, s, pos);
		string[pos] = '\0';
		index[i] = atoi(string);
		s = p + 1;
	}
	if (index[0] >= *par->valuePtr.array.numElements) {
		NotifyError("%s: Index range must be 0 - %d for '%s' (%d).", funcName,
		  *par->valuePtr.array.numElements - 1, par->abbr, index[0]);
		return(FALSE);
	}
	*parValuePtr = s;
	if (i == 0)
		index[0] = par->valuePtr.array.index;
	return(TRUE);

}
	
/************************ SetGeneralParValue **********************************/

/*
 * Set a general parameter in a universal parameter list, where the value is in
 * string form.
 */

BOOLN
SetGeneralParValue_UniParMgr(UniParListPtr parList, uInt index, char *parValue)
{
	static const char *funcName = "SetGeneralParValue_UniParMgr";
	BOOLN	ok = FALSE;
	char	*arrayValue;
	int		arrayIndex[UNIPAR_MAX_ARRAY_INDEX];
	UniParPtr	p;

	p = &parList->pars[index];
	switch (p->type) {
	case UNIPAR_INT:
		ok = (* p->FuncPtr.SetInt)(atoi(parValue));
		break;
	case UNIPAR_INT_ARRAY:
		if (!ParseArrayValue_UniParMgr(p, parValue, &arrayValue, arrayIndex)) {
			NotifyError("%s: Could not set array value.", funcName);
			return(FALSE);
		}
		ok = (* p->FuncPtr.SetIntArrayElement)(arrayIndex[0], atoi(arrayValue));
		break;
	case UNIPAR_LONG:
		ok = (* p->FuncPtr.SetLong)(atol(parValue));
		break;
	case UNIPAR_REAL:
		ok = (* p->FuncPtr.SetReal)(atof(parValue));
		break;
	case UNIPAR_REAL_ARRAY:
		if (!ParseArrayValue_UniParMgr(p, parValue, &arrayValue, arrayIndex)) {
			NotifyError("%s: Could not set array value.", funcName);
			return(FALSE);
		}
		ok = (* p->FuncPtr.SetRealArrayElement)(arrayIndex[0],
		  atof(arrayValue));
		break;
	case UNIPAR_BOOL:
	case UNIPAR_STRING:
	case UNIPAR_MODULE:
	case UNIPAR_NAME_SPEC:
	case UNIPAR_NAME_SPEC_WITH_FILE:
	case UNIPAR_NAME_SPEC_WITH_FPATH:
	case UNIPAR_FILE_NAME:
		ok = (* p->FuncPtr.SetString)(parValue);
		break;
	case UNIPAR_CFLIST:
		ok = (* p->FuncPtr.SetCFList)((CFListPtr) parValue);
		break;
	case UNIPAR_ICLIST:
		ok = (* p->FuncPtr.SetICList)((IonChanListPtr) parValue);
		break;
	case UNIPAR_PARARRAY:
		break;
	default:
		NotifyError("%s: Universal parameter type not yet implemented (%d).",
		  funcName, p->type);
		ok = FALSE;
	}
	return(ok);

}

/************************ SetCFListParValue ***********************************/

/*
 * Set a CFList parameter in a universal parameter list, where the value is in
 * string form.
 */

BOOLN
SetCFListParValue_UniParMgr(UniParListPtr *parList, uInt index, char *parValue)
{
	static const char *funcName = "SetCFListParValue_UniParMgr";
	BOOLN	ok = FALSE;
	char	*arrayValue;
	int		arrayIndex[UNIPAR_MAX_ARRAY_INDEX];
	UniParPtr	p;

	p = &(*parList)->pars[index];
	switch (p->type) {
	case UNIPAR_INT:
		ok = (* p->FuncPtr.SetCFListInt)((*parList)->handlePtr.cFs,
		  atoi(parValue));
		break;
	case UNIPAR_REAL:
		ok = (* p->FuncPtr.SetCFListReal)((*parList)->handlePtr.cFs,
		  atof(parValue));
		break;
	case UNIPAR_REAL_ARRAY:
		if (!ParseArrayValue_UniParMgr(p, parValue, &arrayValue, arrayIndex)) {
			NotifyError("%s: Could not set array value.", funcName);
			return(FALSE);
		}
		ok = (* p->FuncPtr.SetCFListRealArrayElement)(
		  (*parList)->handlePtr.cFs, arrayIndex[0], atof(arrayValue));
		break;
	case UNIPAR_BOOL:
	case UNIPAR_STRING:
	case UNIPAR_MODULE:
	case UNIPAR_NAME_SPEC:
	case UNIPAR_NAME_SPEC_WITH_FILE:
	case UNIPAR_NAME_SPEC_WITH_FPATH:
		ok = (* p->FuncPtr.SetCFListString)((*parList)->handlePtr.cFs,
		  parValue);
		break;
	default:
		NotifyError("%s: Universal parameter type not yet implemented (%d).",
		  funcName, p->type);
		ok = FALSE;
	}
	if (ok && (*parList)->updateFlag)
		ok = RegenerateList_CFList((*parList)->handlePtr.cFs);
	return(ok);

}

/************************ SetParArrayParValue *********************************/

/*
 * Set a ParArray parameter in a universal parameter list, where the value is
 * in string form.
 */

BOOLN
SetParArrayParValue_UniParMgr(UniParListPtr *parList, uInt index,
  char *parValue)
{
	static const char *funcName = "SetParArrayParValue_UniParMgr";
	BOOLN	ok = FALSE;
	char	*arrayValue;
	int		arrayIndex[UNIPAR_MAX_ARRAY_INDEX];
	UniParPtr	p;

	p = &(*parList)->pars[index];
	switch (p->type) {
	case UNIPAR_INT:
		ok = (* p->FuncPtr.SetParArrayInt)((*parList)->handlePtr.parArray.ptr,
		  atoi(parValue));
		break;
	case UNIPAR_REAL_ARRAY:
		if (!ParseArrayValue_UniParMgr(p, parValue, &arrayValue, arrayIndex)) {
			NotifyError("%s: Could not set array value.", funcName);
			return(FALSE);
		}
		ok = (* p->FuncPtr.SetParArrayRealArrayElement)(
		  (*parList)->handlePtr.parArray.ptr, arrayIndex[0], atof(arrayValue));
		break;
	case UNIPAR_BOOL:
	case UNIPAR_STRING:
	case UNIPAR_MODULE:
	case UNIPAR_NAME_SPEC:
	case UNIPAR_NAME_SPEC_WITH_FILE:
	case UNIPAR_NAME_SPEC_WITH_FPATH:
		ok = (* p->FuncPtr.SetParArrayString)((*parList)->handlePtr.parArray.
		  ptr, parValue);
		break;
	default:
		NotifyError("%s: Universal parameter type not yet implemented (%d).",
		  funcName, p->type);
		ok = FALSE;
	}
	if (ok)
		(* (*parList)->handlePtr.parArray.SetFunc)((*parList)->handlePtr.
		  parArray.ptr);
	return(ok);

}

/************************ SetCurrentIC ****************************************/

/*
 * This routine sets the IC index for an IC list structure.
 * It assumes that the arguments are correctly initialised.
 * It returns FALSE if the ion channel is not found and there are no free un-set
 * ion channels which it can use.
 */

BOOLN
SetCurrentIC_UniParMgr(IonChanListPtr theICs, char *description)
{
	DynaListPtr	node;
	IonChannelPtr	iC;

	for (node = theICs->ionChannels; node; node = node->next) {
		iC = (IonChannelPtr) node->data;
		if ((iC->description[0] == '\0') || (StrNCmpNoCase_Utility_String(
		  iC->description, description) == 0)) {
			theICs->currentIC = iC;
			return(TRUE);
		}
	}
	return(FALSE);

}

/************************ SetICParValue ***************************************/

/*
 * Set a Ion channel parameter in a universal parameter list, where the
 * value is in string form.
 */

BOOLN
SetICParValue_UniParMgr(UniParListPtr parList, uInt index, char *parValue)
{
	static const char *funcName = "SetICParValue_UniParMgr";
	BOOLN	ok = FALSE;
	char	*arrayValue;
	int		arrayIndex[UNIPAR_MAX_ARRAY_INDEX];
	UniParPtr	p;
	IonChanListPtr	theICs = parList->handlePtr.iCs;

	p = &parList->pars[index];
	switch (p->type) {
	case UNIPAR_INT:
		ok = (* p->FuncPtr.SetICInt)(theICs->currentIC, atoi(parValue));
		break;
	case UNIPAR_REAL:
		ok = (* p->FuncPtr.SetICReal)(theICs->currentIC, atof(parValue));
		break;
	case UNIPAR_REAL_ARRAY:
		if (!ParseArrayValue_UniParMgr(p, parValue, &arrayValue, arrayIndex)) {
			NotifyError("%s: Could not set array value.", funcName);
			return(FALSE);
		}
		ok = (* p->FuncPtr.SetICRealArrayElement)(theICs->currentIC,
		  arrayIndex[0], atof(arrayValue));
		break;
	case UNIPAR_BOOL:
	case UNIPAR_STRING:
	case UNIPAR_MODULE:
	case UNIPAR_NAME_SPEC:
	case UNIPAR_NAME_SPEC_WITH_FILE:
	case UNIPAR_NAME_SPEC_WITH_FPATH:
	case UNIPAR_FILE_NAME:
		if ((index == ICLIST_IC_DESCRIPTION) && !SetCurrentIC_UniParMgr(theICs,
		  parValue)) {
			NotifyError("%s: Could not find '%s' ion channel.", funcName,
			  parValue);
			return(FALSE);
		}
		ok = (* p->FuncPtr.SetICString)(theICs->currentIC, parValue);
		break;
	default:
		NotifyError("%s: Universal parameter type not yet implemented (%d).",
		  funcName, p->type);
		ok = FALSE;
	}
	return(ok);

}

/************************ SetICListParValue ***********************************/

/*
 * Set a Ion channel List parameter in a universal parameter list, where the
 * value is in string form.
 */

BOOLN
SetICListParValue_UniParMgr(UniParListPtr *parList, uInt index, char *parValue)
{
	static const char *funcName = "SetICListParValue_UniParMgr";
	BOOLN	ok = FALSE;
	UniParPtr	p = &(*parList)->pars[index];
	IonChanListPtr	theICs = (*parList)->handlePtr.iCs;

	switch (p->type) {
	case UNIPAR_INT:
		ok = (* p->FuncPtr.SetICListInt)(theICs, atoi(parValue));
		break;
	case UNIPAR_REAL:
		ok = (* p->FuncPtr.SetICListReal)(theICs, atof(parValue));
		break;
	case UNIPAR_BOOL:
	case UNIPAR_STRING:
	case UNIPAR_MODULE:
	case UNIPAR_NAME_SPEC:
	case UNIPAR_NAME_SPEC_WITH_FILE:
	case UNIPAR_NAME_SPEC_WITH_FPATH:
		ok = (* p->FuncPtr.SetICListString)(theICs, parValue);
		break;
	default:
		NotifyError("%s: Universal parameter type not yet implemented "
		  "(%d).", funcName, p->type);
		ok = FALSE;
	}
	return(ok);

}

/************************ SetParValue *****************************************/

/*
 * Set a parameter in a universal parameter list, where the value is in string
 * form.
 */

BOOLN
SetParValue_UniParMgr(UniParListPtr *parList, uInt index, char *parValue)
{
	static const char *funcName = "SetParValue_UniParMgr";
	BOOLN	ok = TRUE;

	if ((*parList) == NULL) {
		NotifyError("%s: universal parameter not initialised.", funcName);
		return(FALSE);
	}
	switch ((*parList)->mode) {
	case UNIPAR_SET_GENERAL:
	case UNIPAR_SET_SIMSPEC:
		ok = SetGeneralParValue_UniParMgr(*parList, index, parValue);
		break;
	case UNIPAR_SET_CFLIST:
		ok = SetCFListParValue_UniParMgr(parList, index, parValue);
		break;
	case UNIPAR_SET_PARARRAY:
		ok = SetParArrayParValue_UniParMgr(parList, index, parValue);
		break;
	case UNIPAR_SET_IC:
		ok = SetICParValue_UniParMgr(*parList, index, parValue);
		break;
	case UNIPAR_SET_ICLIST:
		ok = SetICListParValue_UniParMgr(parList, index, parValue);
		break;
	default:
		NotifyError("%s: Universal parameter mode not yet implemented (%d).",
		  funcName, (*parList)->mode);
		ok = FALSE;
	}
	(*parList)->updateFlag = ok;
	return(ok);

}

/************************ FindUniPar ******************************************/

/*
 * This function returns a pointer to a universal parameter specified by an
 * abbreviation within a list.
 * It searches recursively down lists for 'UNIPAR_MODULE' types.
 * It searches recursively down lists for 'UNIPAR_PARLIST' types.
 * It searches through simulation scripts, i.e. 'UNIPAR_SIMSCRIPT' types.
 * It assumes that 'parName' points to a valid string.
 * It removes any strings after a period, "." including the period.
 * It returns NULL if it fails in any way.
 * The function sets the argument 'parList' so the recursive searches return
 * the correct parList.
 */

UniParPtr
FindUniPar_UniParMgr(UniParListPtr *parList, char *parName)
{
	static const char *funcName = "FindUniPar_UniParMgr";
	int		i;
	UniParPtr	p, par = NULL;
	UniParListPtr	tempParList;

	if (*parList == NULL) {
		NotifyError("%s: Parameter list not initialised.", funcName);
		return(NULL);
	}
	for (i = 0; (i < (*parList)->numPars) && !par; i++) {
		p = &(*parList)->pars[i];
		switch (p->type) {
		case UNIPAR_MODULE:
			if (StrNCmpNoCase_Utility_String(p->abbr, parName) == 0) {
				par = p;
				break;
			}
			tempParList = p->valuePtr.module.parList;
			if ((par = FindUniPar_UniParMgr(&tempParList, parName)) != NULL) {
				*parList = tempParList;
				break;	/* here in case anything is added under this. */
			}
		   break;
		case UNIPAR_PARLIST:
			if (StrNCmpNoCase_Utility_String(p->abbr, parName) == 0) {
				par = p;
				break;
			}
			if ((tempParList = *(p->valuePtr.parList)) == NULL)
				break;
			if ((par = FindUniPar_UniParMgr(&tempParList, parName)) != NULL) {
				*parList = tempParList;
				break;	/* here in case anything is added under this. */
			}
		   break;
		case UNIPAR_CFLIST:
			if (StrNCmpNoCase_Utility_String(p->abbr, parName) == 0) {
				par = p;
				break;
			}
			if (!*p->valuePtr.cFPtr)
				return(NULL);
			tempParList = (*p->valuePtr.cFPtr)->cFParList;
			if ((par = FindUniPar_UniParMgr(&tempParList, parName)) != NULL) {
				*parList = tempParList;
				break;
			}
			tempParList = (*p->valuePtr.cFPtr)->bParList;
			if ((par = FindUniPar_UniParMgr(&tempParList, parName)) != NULL) {
				*parList = tempParList;
				break;
			}
			break;
		case UNIPAR_PARARRAY:
			if (StrNCmpNoCase_Utility_String(p->abbr, parName) == 0) {
				par = p;
				break;
			}
			if (!*p->valuePtr.pAPtr)
				break;
			tempParList = (*p->valuePtr.pAPtr)->parList;
			if ((par = FindUniPar_UniParMgr(&tempParList, parName)) != NULL) {
				*parList = tempParList;
				break;
			}
			break;
		case UNIPAR_ICLIST: {
			DynaListPtr	node;
			IonChanListPtr	theICs = *p->valuePtr.iCPtr;

			if (StrNCmpNoCase_Utility_String(p->abbr, parName) == 0) {
				par = p;
				break;
			}
			if (!theICs)
				return(NULL);
			tempParList = theICs->parList;
			if ((par = FindUniPar_UniParMgr(&tempParList, parName)) != NULL) {
				*parList = tempParList;
				break;
			}
			if (!theICs->ionChannels) {
				if (!PrepareIonChannels_IonChanList(theICs)) {
					NotifyError("%s: Could not set the ion channels.",
					  funcName);
					return(FALSE);
				}
			}
			for (node = theICs->ionChannels; node; node =
			  node->next) {
				tempParList = ((IonChannelPtr) node->data)->parList;
				if ((par = FindUniPar_UniParMgr(&tempParList, parName)) !=
				  NULL) {
					*parList = tempParList;
					break;
				}
			}
			break; }
		case UNIPAR_SIMSCRIPT: {
			uInt	index;
			DatumPtr	pc = *p->valuePtr.simScript.simulation;

			tempParList = *parList;
			if (FindModuleUniPar_Utility_Datum(&tempParList, &index, &pc,
			  parName, TRUE) && (index < tempParList->numPars)) {
				par = &tempParList->pars[index];
				break;
			}
			break; }
		default:
			if (StrNCmpNoCase_Utility_String(p->abbr, parName) == 0) {
				par = p;
				break;
			}
		}
	}
	return(par);

}
