/**********************
 *
 * File:		UtDatum.c
 * Purpose:		This module contains the process control, "datum" management
 *				code.
 * Comments:	Written using ModuleProducer version 1.9 (May 27 1996).
 *				11-03-97 LPO: This code has been amended to allow the numbers
 *				of inputs and outputs for an EarObject to be specified.
 *				The number of inputs defines how many of the previous
 *				EarObjects should be connected to a particular EarObject.
 *				The number of outputs defines how many of the following
 *				EarObjects should be connected accordingly.
 *				21-08-97 LPO: Removed rampInterval parameter - use new ramp
 *				module.
 *				19-11-98 LPO: Corrected the "General parameter list' bug, that
 *				meant that the 'array_index' general parameter was not being
 *				found for a particular process.
 *				10-12-98 LPO: Introduced handling of NULL parLists, i.e. for 
 *				modules with no parameters.
 *				27-01-99 LPO: The static 'stepCount' in the 'InstallInst_'
 *				routine was not being reset to zero when installing 'datum'
 *				nodes at the head of the list.  This occurred with the 
 *				GUI mode, where simulations needed to be recreated.
 *				02-02-99 LPO: The 'FindModuleUniPar_' routine now also returns
 *				the simulation 'DatumPtr' at which the universal parameter was
 *				found.
 *				The 'SetUniParValue_' routine has been moved here from the
 *				UtSimScript module, as it needs to use the
 *				'SetParsPointer_ModuleMgr' routine.
 *				05-05-99 LPO: The 'InitialiseModules_' routine now sets the
 *				display signal's window title using the 'stepNumber' field, if
 *				it has not already been set.
 *				24-08-99 LPO: Implemented nicer output for the
 *				'PrintInstructions_" routine.
 *				07-09-99 LPO: Fixed major problem with the FindModuleUniPar
 *				routine: It was replacing the simScript simulation field when
 *				operating on sub-simulations.
 * Author:		L. P. O'Mard
 * Created:		01 Jul 1996
 * Updated:		08 Sep 1999
 * Copyright:	(c) 1999, University of Essex
 *
 *********************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DSAM_DO_NOT_DEFINE_MYAPP 1

#include "DSAM.h"
#include "UtSSParser.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

uInt	datumStepCount = 0;

/******************************************************************************/
/****************************** Subroutines and functions *********************/
/******************************************************************************/


/****************************** GetLastInst ***********************************/

/*
 * This routine returns a pointer to the last Datum instruction in a
 * simulation instruction list.
 */

DatumPtr
GetLastInst_Utility_Datum(DatumPtr head)
{
	DatumPtr	pc;

	if (head == NULL)
		return(NULL);
	for (pc = head; pc->next != NULL; pc = pc->next)
		;
	return(pc);

}

/****************************** InstallInst ***********************************/

/*
 * Install a code datum instruction in table.
 * Datum instructions are added to the end of the list.
 */

DatumPtr
InstallInst_Utility_Datum(DatumPtr *head, int type)
{
	static const char *funcName = "InstallInst_Utility_Datum";
	DatumPtr	datum, pc;
	
	if ((datum = (DatumPtr) malloc(sizeof (Datum))) == NULL) {
		NotifyError("%s: Out of memory for Datum.", funcName);
		return(NULL);
	}
	datum->connectedFlag = FALSE;
	datum->onFlag = TRUE;
	datum->type = type;
	switch (type) {
	case PROCESS:
		datum->u.proc.label = NULL_STRING;
		datum->u.proc.parFile = NULL_STRING;
		datum->u.proc.moduleName = NULL_STRING;
		datum->u.proc.numInputs = DATUM_DEFAULT_EAROBJECT_CONNECTIONS;
		datum->u.proc.numOutputs = DATUM_DEFAULT_EAROBJECT_CONNECTIONS;
		break;
	case RESET:
		datum->u.string = NULL_STRING;
		break;
	default:
		datum->u.proc.label = NULL_STRING;
		datum->u.var = 0;
		break;
	} /* Switch */
	datum->data = NULL;
	datum->next = NULL;
	if ((pc = GetLastInst_Utility_Datum(*head)) == NULL) {
		*head = datum;
		datum->previous = NULL;
	} else {
		pc->next = datum;
		datum->previous = pc;
	}
	datum->stepNumber = datumStepCount++;
	return datum;

}

/****************************** FreeInstructions ******************************/

/*
 * This routine frees the Datum instructions in the simulation.
 */

void
FreeInstructions_Utility_Datum(DatumPtr *pc)
{
	DatumPtr	temp;
	
	for ( ; *pc != NULL; ) {
		temp = *pc;
		*pc = (*pc)->next;
		switch (temp->type) {
		case PROCESS:
			if (*temp->u.proc.parFile != '\0')
				free(temp->u.proc.parFile);
			if (*temp->u.proc.moduleName != '\0')
				free(temp->u.proc.moduleName);
			if (*temp->u.proc.label != '\0')
				free(temp->u.proc.label);
			Free_EarObject(&temp->data);
			break;
		case RESET:
			free(temp->u.string);
			break;
		default:
			if (*temp->u.proc.label != '\0')
				free(temp->u.proc.label);
			break;
		} /* switch */
		free(temp);
	}
	*pc = NULL;

}

/****************************** ResolveInstLabels *****************************/

/*
 * This routine checks that the labels used in instructions (such as 'reset').
 * actually exist within the simulation script instructions.
 * It also sets the EarObject data pointer to that corresponding to the label,
 * So it must be used after the EarObjects have been initialised, if the data
 * pointers are to be valid.
 * It returns FALSE if it fails in any way.
 */

BOOLN
ResolveInstLabels_Utility_Datum(DatumPtr start)
{
	static const char *funcName = "ResolveInstLabels_Utility_Datum";
	BOOLN		ok, found;
	DatumPtr	pc1, pc2;
	
	for (pc1 = start, ok = TRUE; pc1 != NULL; pc1 = pc1->next)
		if (pc1->type == RESET) {
			for (pc2 = pc1->next, found = FALSE; pc2 != NULL; pc2 = pc2->next)
				if ((pc2->type == PROCESS) && (strcmp(pc1->u.string,
				  pc2->u.proc.label) == 0)) {
					found = TRUE;
					pc1->data = pc2->data;
					break;
				}
			if (!found) {
				NotifyError("%s: Could not find label '%s' in the simulation "
				  "script.", funcName, pc1->u.string);
				ok = FALSE;
			}
		}
	return(ok);

}

/****************************** PrintIndentAndLabel ***************************/

/*
 * This routine prints a specified number of tabs, with the label at the
 * previous tab level.
 * The minimum tab level is 1.
 */

void
PrintIndentAndLabel_Utility_Datum(DatumPtr pc, int indentLevel)
{
	int		i;

	for (i = 0; i < indentLevel - 1; i++)
		DPrint("\t");
	if ((pc->type == PROCESS) && pc->u.proc.label && pc->u.proc.label[0] !=
	  '\0')
		DPrint("%s%%", pc->u.proc.label);
	DPrint("\t");

}

/****************************** PrintInstructions *****************************/

/*
 * This routine prints the simulation process instructions.
 */

void
PrintInstructions_Utility_Datum(DatumPtr pc, char *scriptName, int indentLevel,
  char *prefix)
{
	BOOLN	subSimScripts = FALSE;
	int		i;
	DatumPtr	start = pc;

	if (pc == NULL)
		return;
	DPrint("%sbegin %s {\n", prefix, scriptName);
	for ( ; pc != NULL; pc = pc->next) {
		DPrint("%s", prefix);
		switch (pc->type) {
		case PROCESS:
			if (pc->data->module->specifier == SIMSCRIPT_MODULE)
				subSimScripts = TRUE;
			PrintIndentAndLabel_Utility_Datum(pc, indentLevel);
			if (!pc->data->module->onFlag)
				DPrint("%c ", SIMSCRIPT_DISABLED_MODULE_CHAR);
			DPrint("%s", pc->u.proc.moduleName);
			if ((pc->u.proc.numInputs > 1) || (pc->u.proc.numOutputs > 1))
				DPrint(" (%d->%d)", pc->u.proc.numInputs,
				  pc->u.proc.numOutputs);
			if (strcmp(pc->u.proc.parFile, NO_FILE) == 0)
				DPrint("\n");
			else if (pc->data->module->specifier == SIMSCRIPT_MODULE)
				DPrint("\t<\t(%s)\n",  pc->u.proc.parFile);
			else
				DPrint("\t<\t%s\n",  pc->u.proc.parFile);
			break;
		case REPEAT:
			PrintIndentAndLabel_Utility_Datum(pc, indentLevel++);
			DPrint("repeat %d {\n", pc->u.var);
			break;
		case RESET:
			PrintIndentAndLabel_Utility_Datum(pc, indentLevel);
			DPrint("reset\t%s\n", pc->u.string);
			break;
		case STOP:
			PrintIndentAndLabel_Utility_Datum(pc, --indentLevel);
			DPrint("}\n");
			break;
		default:
			PrintIndentAndLabel_Utility_Datum(pc, indentLevel);
			DPrint("default %d\tvar = %d\n", pc->type, pc->u.var);
			break;
		} /* switch */
	}
	for (i = 0; i < indentLevel - 1; i++)
		DPrint("\t");
	DPrint("%s}\n", prefix);
	if (!subSimScripts)
		return;
	DPrint("%s#Sub-simulation scripts\n", prefix);
	for (pc = start ; pc != NULL; pc = pc->next)
		if ((pc->type == PROCESS) && (pc->data->module->specifier ==
		  SIMSCRIPT_MODULE)) {
			DPrint("%s", prefix);
			PrintInstructions_Utility_Datum(((SimScriptPtr)
			  pc->data->module->parsPtr)->simulation, pc->u.proc.parFile,
			  indentLevel + 1, prefix);
		}

}

/****************************** SetBackwardConnections ************************/

/*
 * This routine sets up the backward connection of EarObjects
 * when they have multiple inputs.
 * It only connects EarObjects with greater than 1 inputs.
 * It assumes that the list has been properly initialised.
 */

BOOLN
SetBackwardConnections_Utility_Datum(DatumPtr start)
{
	static const char *funcName = "SetBackwardConnections_Utility_Datum";
	int			i;
	DatumPtr	pc1, pc2;

	for (pc1 = start; (pc1 != NULL); pc1 = pc1->next)
		if ((pc1->type == PROCESS) && (pc1->u.proc.numInputs > 1)) {
			for (i = 0, pc2 = pc1; i < pc1->u.proc.numInputs; i++)
				for (pc2 = pc2->previous; (pc2 != NULL) && (pc2->type !=
				  PROCESS); pc2 = pc2->previous)
					;
			if (pc2 == NULL) {
				NotifyError("%s: Could not find %d EarObjects for input to "
				  "EarObject (module '%s').", funcName, pc1->u.proc.numInputs,
				  pc1->u.proc.moduleName);
				return(FALSE);
			}
			for (i = 0; i < pc1->u.proc.numInputs; i++) {
				ConnectOutSignalToIn_EarObject( pc2->data, pc1->data );
				pc2->connectedFlag = TRUE;
				for (pc2 = pc2->next; (pc2 != NULL) && (pc2->type != PROCESS);
				  pc2 = pc2->next);
					;
			}
		}
	return(TRUE);

}

/****************************** SetForwardConnections ************************/

/*
 * This routine sets up the forward connection of EarObjects and takes the
 * necessary action when they output to more than one EarObject.
 * It assumes that the list has been properly initialised.
 * For multiple outputs, it assumes that if the first connection has already
 * been made, then the connection should go to the next
 */

BOOLN
SetForwardConnections_Utility_Datum(DatumPtr start)
{
	/* static const char *funcName = "SetForwardConnections_Utility_Datum"; */
	int			i;
	DatumPtr	pc1, pc2;

	for (pc1 = start; (pc1 != NULL); pc1 = pc1->next)
		if ((pc1->type == PROCESS) && !pc1->connectedFlag) {
			for (i = 0, pc2 = pc1; (pc2 != NULL) && (i <
			  pc1->u.proc.numOutputs); ) {
				for (pc2 = pc2->next; (pc2 != NULL) && (pc2->type != PROCESS);
				  pc2 = pc2->next)
					;
				if ((pc2 != NULL) && (pc2->data->numInSignals !=
				  pc2->data->maxInSignals)) {
					ConnectOutSignalToIn_EarObject( pc1->data, pc2->data );
					i++;
				}
			}
		}
	return(TRUE);

}

/****************************** InitialiseEarObjects **************************/

/*
 * This routine initialises and connects the EarObjects for the simulation
 * script.
 */

BOOLN
InitialiseEarObjects_Utility_Datum(DatumPtr start)
{
	static const char	*funcName = "InitialiseEarObjects_Utility_Datum";
	BOOLN	ok = TRUE;
	DatumPtr	pc;

	if (start == NULL) {
		NotifyError("%s: Simulation not initialised.\n", funcName);
		return(FALSE);
	}
	for (pc = start; pc != NULL; pc = pc->next)
		if ((pc->type == PROCESS) && (pc->data = Init_EarObject_MultiInput(
		  pc->u.proc.moduleName, pc->u.proc.numInputs)) == NULL) {
			NotifyError("%s: Could not initialise process with '%s'.",
			  funcName, pc->u.proc.moduleName);
			ok = FALSE;
		}
	if (ok)
		ok = ResolveInstLabels_Utility_Datum(start);
	if (!ok)
		return(FALSE);

	/* Set up main simulation EarObject connections. */
	if (!SetBackwardConnections_Utility_Datum(start)) {
		NotifyError("%s Could not set backward connections.", funcName);
		return(FALSE);
	}
	if (!SetForwardConnections_Utility_Datum(start)) {
		NotifyError("%s Could not set backward connections.", funcName);
		return(FALSE);
	}
	return (TRUE);

}

/****************************** FreeEarObjects ********************************/

/*
 * This routine frees the EarObjects for the simulation script.
 */

BOOLN
FreeEarObjects_Utility_Datum(DatumPtr start)
{
	/* static const char	*funcName = "FreeEarObjects_Utility_Datum"; */
	DatumPtr	pc;
	
	for (pc = start; pc != NULL; pc = pc->next)
		if (pc->type == PROCESS)
			Free_EarObject(&pc->data);
	return(TRUE);
}



/****************************** NameAndStepNumber *****************************/

/*
 * This routine routines a pointer to the proces name and step number for a
 * Datum structure.
 * A copy of this information should be used if a permanent string is required.
 */

char *
NameAndStepNumber_Utility_Datum(DatumPtr pc)
{
	static const char	*funcName = "NameAndStepNumber_Utility_Datum";
	static char	string[MAXLINE];

	if (!pc || (pc->type != PROCESS)) {
		NotifyError("%s: Pointer not initialised or not a 'PROCESS'.",
		  funcName);
		return(NULL);
	}
	snprintf(string, MAXLINE, "%s.%d", pc->u.proc.moduleName, pc->stepNumber);
	return(string);

}

/****************************** InitialiseModules *****************************/

/*
 * This routine initialises all the necessary modules.
 * If it encounters the 'DISPLAY_MODULE' module, then it will attempt to set the
 * window title.  It checks for a NULL 'UniParListPtr' which indicates that the
 * GUI library is not being used.
 * 
 * It assumes that all 'internal' sub-simulation scripts that have the
 * simulation already initialised only needs the other parameters to be
 * initialised.
 */

BOOLN
InitialiseModules_Utility_Datum(DatumPtr start)
{
	/* static const char	*funcName = "InitialiseModules_Utility_Datum"; */
	BOOLN	ok;
	DatumPtr	pc;
	
	for (pc = start, ok = TRUE; pc != NULL; pc = pc->next)
		if (pc->type == PROCESS ) {
			if (GetDSAMPtr_Common()->usingGUIFlag && (pc->data->module->
			  specifier == DISPLAY_MODULE) && (GetUniParPtr_ModuleMgr(pc->data,
			  "win_title")->valuePtr.s[0] == '\0')) {
				SetPar_ModuleMgr(pc->data, "win_title",
				  NameAndStepNumber_Utility_Datum(pc));
			}
			if ((strcmp(pc->u.proc.parFile, NO_FILE) != 0) &&
			  ((pc->data->module->specifier != SIMSCRIPT_MODULE) ||
			  !GetSimulation_ModuleMgr(pc->data))) {
				if (!ReadPars_ModuleMgr(pc->data, pc->u.proc.parFile))
					ok = FALSE;
			}
		}
	return(ok);

}

/****************************** PrintParsModules ******************************/

/*
 * This routine issues the PrintPars command for all the process modules.
 */

BOOLN
PrintParsModules_Utility_Datum(DatumPtr start)
{
	/* static const char	*funcName = "PrintParsModules_Utility_Datum"; */
	BOOLN	ok;
	DatumPtr	pc;
	
	for (pc = start, ok = TRUE; pc != NULL; pc = pc->next)
		if (pc->type == PROCESS)
			if ((pc->data == NULL) || !PrintPars_ModuleMgr( pc->data ))
				ok = FALSE;
	return(ok);

}

/****************************** PrintParListModules ***************************/

/*
 * This routine prints the universal parameter lists for all the process
 * modules.
 */

BOOLN
PrintParListModules_Utility_Datum(DatumPtr start, char *prefix)
{
	/* static const char	*funcName = "PrintParListModules_Utility_Datum"; */
	BOOLN	ok;
	char	suffix[MAXLINE];
	DatumPtr		pc;
	UniParListPtr	parList;
	
	for (pc = start, ok = TRUE; pc != NULL; pc = pc->next)
		if ((pc->type == PROCESS) && (pc->data->module->specifier !=
		  NULL_MODULE)) {
			if ((pc->data == NULL) || ((parList = GetUniParListPtr_ModuleMgr(
			  pc->data )) == NULL))
				ok = FALSE;
			else if (pc->data->module->specifier == SIMSCRIPT_MODULE)
				ok = PrintParListModules_Utility_Datum(GetSimulation_ModuleMgr(
				  pc->data), prefix);
			else {
				DPrint("\t##--------------- %-20s [%d] ---------------##\n",
				  pc->u.proc.moduleName, pc->stepNumber);
				snprintf(suffix, MAXLINE, ".%s.%d", pc->u.proc.moduleName,
				  pc->stepNumber);
				ok = (parList)? PrintPars_UniParMgr(parList, prefix, suffix):
				  TRUE;
			}
		}
	return(ok);

}

/****************************** CheckParLists *********************************/

/*
 * This routine checks if the universal parameters lists require updating.
 * This is particularly necessary for the CFList lists and similar structures.
 * This routine will not return an error if a module has a null parameter list.
 */

BOOLN
CheckParLists_Utility_Datum(DatumPtr start)
{
	/* static const char	*funcName = "CheckParLists_Utility_Datum"; */
	BOOLN	ok;
	DatumPtr		pc;
	UniParListPtr	parList;
	
	for (pc = start, ok = TRUE; pc != NULL; pc = pc->next)
		if ((pc->type == PROCESS) && (pc->data->module->specifier !=
		  NULL_MODULE)) {
			if (pc->data == NULL)
				ok = FALSE;
			else {
				if (((parList = GetUniParListPtr_ModuleMgr( pc->data )) !=
				  NULL) && !CheckParList_UniParMgr(parList))
					ok = FALSE;
			}
		}
	return(ok);

}

/****************************** ResetSimulation *******************************/

/*
 * This routine resets all of the simulation processes.
 */

void
ResetSimulation_Utility_Datum(DatumPtr start)
{
	/* static const char	*funcName = "ResetSimulation_Utility_Datum"; */
	DatumPtr		pc;
	
	for (pc = start; pc != NULL; pc = pc->next)
		if ((pc->type == PROCESS) && (pc->data->module->specifier !=
		  NULL_MODULE))
			if (pc->data != NULL)
				ResetProcess_EarObject(pc->data);

}

/****************************** Execute ***************************************/

/*
 * This routine executes a set of simulation instructions.
 * It continues until it reachs a STOP instructions, or a NULL instuction.
 * It returns a pointer to the STOP instruction or NULL.
 */

DatumPtr
Execute_Utility_Datum(DatumPtr start)
{
	static char *funcName = "Execute_Utility_Datum";
	int			i;
	DatumPtr	pc, lastInstruction = NULL;

	if (start == NULL)
		return NULL;
	for (pc = start; pc != NULL; pc = pc->next) {
		switch (pc->type) {
		case PROCESS:
			if (!RunProcess_ModuleMgr(pc->data))
				return(NULL);
			break;
		case RESET:
			ResetProcess_EarObject(pc->data);
			break;
		case STOP:
			return (pc);
		case REPEAT:
			if (!pc->u.var) {
				NotifyError("%s: Illegal zero 'repeat'.", funcName);
				return(NULL);
			}
			for (i = 0; i < pc->u.var; i++)
				lastInstruction = Execute_Utility_Datum(pc->next);
			pc = lastInstruction;
			break;
		default:
			break;
		} /* switch */
		if ((lastInstruction = pc) == NULL)
			break;
	}
	return(lastInstruction);

}

/****************************** GetLastProcess ********************************/

/*
 * This routine returns the last process in a simulation.
 * It returns NULL if it fails in any way.
 */

EarObjectPtr
GetLastProcess_Utility_Datum(DatumPtr start)
{
	EarObjectPtr	lastProcess = NULL;
	DatumPtr	pc;

    for (pc = start; pc != NULL; pc = pc->next)
    	if (pc->type == PROCESS)
    		lastProcess = pc->data;
	return(lastProcess);

}

/****************************** GetFirstProcess *******************************/

/*
 * This routine returns the first process in a simulation.
 * It returns NULL if it fails in any way.
 */

EarObjectPtr
GetFirstProcess_Utility_Datum(DatumPtr start)
{
	DatumPtr	pc;

    for (pc = start; pc != NULL; pc = pc->next)
    	if (pc->type == PROCESS)
    		return(pc->data);
	return(NULL);

}

/****************************** FindLabelledProcess ***************************/

/*
 * This routine returns the pointer to a process' EarObject in a simulation,
 * Specified by a label.
 * If the label is the '*' wild card, than the first process found is returned.
 * It returns NULL if it fails in any way.
 */

EarObjectPtr
FindLabelledProcess_Utility_Datum(DatumPtr start, char *label)
{
	static const char *funcName = "FindLabelledProcess_Utility_Datum";
	DatumPtr	pc;

	if (label == NULL) {
		NotifyError("%s: Illegal label!", funcName);
		return(NULL);
	}
	for (pc = start; pc != NULL; pc = pc->next)
		if ((pc->type == PROCESS) && ((label[0] == '*') || (strcmp(label,
		   pc->u.proc.label) == 0)))
			return(pc->data);
	return(NULL);

}

/****************************** FindLabelledProcessInst ***********************/

/*
 * This routine returns the pointer to a process instruction in a simulation,
 * Specified by a label.
 * If the label is the '*' wild card, than the first process found is returned.
 * It returns NULL if it fails in any way.
 * If it does not find the label, then it expects the calling routine to give
 * The correct error message.
 */

DatumPtr
FindLabelledProcessInst_Utility_Datum(DatumPtr start, char *label)
{
	static const char *funcName = "FindLabelledProcessInst_Utility_Datum";
	DatumPtr	pc;

	if (label == NULL) {
		NotifyError("%s: Illegal NULL label!", funcName);
		return(NULL);
	}
	for (pc = start; pc != NULL; pc = pc->next)
		if ((pc->type == PROCESS) && ((label[0] == '*') || (strcmp(label,
		  pc->u.proc.label) == 0)))
			return(pc);
	return(NULL);

}

/****************************** FindLabelledInst ******************************/

/*
 * This routine returns the pointer to an instruction in a simulation,
 * specified by a label.
 * If the label is the '*' wild card, than the first instruction found is
 * returned.
 * It returns NULL if it fails in any way.
 * If it does not find the label, then it expects the calling routine to give
 * The correct error message.
 */

DatumPtr
FindLabelledInst_Utility_Datum(DatumPtr start, char *label)
{
	static const char *funcName = "FindLabelledInst_Utility_Datum";
	DatumPtr	pc;

	if (label == NULL) {
		NotifyError("%s: Illegal NULL label!", funcName);
		return(NULL);
	}
	for (pc = start; pc != NULL; pc = pc->next)
		if (((pc->type == PROCESS) || (pc->type == REPEAT)) && ((label[0] ==
		  '*') || (strcmp(label, pc->u.proc.label) == 0)))
			return(pc);
	return(NULL);

}

/****************************** FindModuleProcessInst *************************/

/*
 * This routine returns the pointer to a process' instruction datum in a
 * simulation, specified by a substring of its module name.
 * It returns NULL if it fails in any way.
 * If it does not find the label, then it expects the calling routine to give
 * The correct error message.
 */

DatumPtr
FindModuleProcessInst_Utility_Datum(DatumPtr start, char *moduleName)
{
	static const char *funcName = "FindModuleProcessInst_Utility_Datum";
	char		upperName[MAXLINE];
	DatumPtr	pc;

	if (moduleName == NULL) {
		NotifyError("%s: Illegal module name!", funcName);
		return(NULL);
	}
	ToUpper_Utility_String(upperName, moduleName);
	for (pc = start; pc != NULL; pc = pc->next)
		if ((pc->type == PROCESS) && (strstr(pc->data->module->name,
		  upperName) != NULL))
			return(pc);
	NotifyError("%s: Could not find module '%s' in the simulation script.",
	  funcName, upperName);
	return(NULL);

}

/****************************** FindModuleUniPar ******************************/

/*
 * This function finds a process' universal parameter in a
 * simulation, specified by a substring of its parameter abbreviation.
 * It returns NULL if it fails in any way.
 * If it does not find the label, then it expects the calling routine to give
 * The correct error message.
 * The function will check for process numbers at the end of the parameter,
 * e.g. "freq_pt.0" name.
 * This routine returns a pointer to the parameter list, and the index within
 * the list, as both of these are needed for processing structures like CFList.
 * The general parameters are encoded by adding the number of parameters in the
 * list to the parameter specifier.
 * The 'FindUniPar_UniParMgr' changes the 'pList' value, cannot be used again
 * when a general list parameter is searched for.
 * This routine will not produce error diagnostics if the 'pList' is NULL.
 */

BOOLN
FindModuleUniPar_Utility_Datum(UniParListPtr *parList, uInt *index,
  DatumPtr *pc, char *parSpecifier, BOOLN diagnosticsOn)
{
	static const char *funcName = "FindModuleUniPar_Utility_Datum";
	char		*p, parName[MAXLINE], processName[MAXLINE], processStr[MAXLINE];
	int			procNum;
	DatumPtr	tempPc;
	UniParPtr		par;
	UniParListPtr	pList = NULL;

	if (parSpecifier == NULL) {
		NotifyError("%s: Illegal parameter name '%s'!", funcName, parSpecifier);
		return(FALSE);
	}
	CopyAndTrunc_Utility_String(parName, parSpecifier, MAXLINE);
	if ((p = strchr(parName, UNIPAR_NAME_SEPARATOR)) == NULL) {
		procNum = -1;
		processName[0] = '\0';
	} else {
		CopyAndTrunc_Utility_String(processName, p + 1, MAXLINE);
		if ((p = strchr(processName, UNIPAR_NAME_SEPARATOR)) != NULL) {
			procNum = atoi(p + 1);
			*p = '\0';
		} else
			procNum = -1;
	}
	if ((p = strchr(parName, UNIPAR_NAME_SEPARATOR)) != NULL)
		*p = '\0';
	for ( ; *pc != NULL; *pc = (*pc)->next)
		if (((*pc)->type == PROCESS) && ((*pc)->data->module->specifier !=
		  NULL_MODULE)) {
			if ((*pc)->data->module->specifier == SIMSCRIPT_MODULE) {
				tempPc = ((SimScriptPtr) (*pc)->data->module->parsPtr)->
				  simulation;
				if (FindModuleUniPar_Utility_Datum(parList, index, &tempPc,
				  parSpecifier, FALSE)) {
					*pc = tempPc;
					return(TRUE);
				}
			}
			if (((processName[0] == '\0') || (StrNCmpNoCase_Utility_String(
			  (*pc)->data->module->name, processName) == 0)) && ((pList =
			  GetUniParListPtr_ModuleMgr((*pc)->data)) != NULL) && ((procNum <
			  0) || (procNum == (*pc)->stepNumber))) {
				if ((par = FindUniPar_UniParMgr(&pList, parName)) != NULL) {
			  		*parList = pList;
					*index = par->index;
					return(TRUE);
				}
			}
		}
	if (diagnosticsOn && pList) {
		if (procNum < 0)
			processStr[0] = '\0';
		else
			snprintf(processStr, MAXLINE, " for process [%d] ", procNum);
		NotifyError("%s: Could not find parameter '%s'%s in the simulation "
		  "script.", funcName, parName, processStr);
	}
	if (!pList)
		*parList = NULL;
	return(FALSE);

}

/****************************** FindProcess ***********************************/

/*
 * This function finds in a simulation, specified by a substring of its
 * module name, with an optional step number.
 * It returns NULL if it fails in any way.
 */

EarObjectPtr
FindProcess_Utility_Datum(DatumPtr pc, char *processSpecifier)
{
	static const char *funcName = "FindProcess_Utility_Datum";
	char		*p, processName[MAXLINE], processStr[MAXLINE];
	int			procNum;

	if (processSpecifier == NULL) {
		NotifyError("%s: Illegal parameter name '%s'!", funcName,
		  processSpecifier);
		return(FALSE);
	}
	CopyAndTrunc_Utility_String(processName, processSpecifier, MAXLINE);
	if ((p = strchr(processName, UNIPAR_NAME_SEPARATOR)) == NULL)
		procNum = -1;
	else
		procNum = atoi(p + 1);
	if ((p = strchr(processName, UNIPAR_NAME_SEPARATOR)) != NULL)
		*p = '\0';
	for ( ; pc != NULL; pc = pc->next)
		if ((pc->type == PROCESS) && (pc->data->module->specifier !=
		  NULL_MODULE)) {
			if (pc->data->module->specifier == SIMSCRIPT_MODULE) {
				return(FindProcess_Utility_Datum(((SimScriptPtr) pc->data->
				  module->parsPtr)->simulation, processSpecifier));
			}
			if (((processName[0] == '\0') || (StrNCmpNoCase_Utility_String(
			  pc->data->module->name, processName) == 0)) && ((procNum <
			  0) || (procNum == pc->stepNumber)))
				return(pc->data);
		}
	if (procNum < 0)
		processStr[0] = '\0';
	else
		snprintf(processStr, MAXLINE, ", step [%d] ", procNum);
	NotifyError("%s: Could not find process '%s'%s in the simulation "
	  "script.", funcName, processName, processStr);
	return(NULL);

}

/****************************** GetInstIntVal *********************************/

/*
 * This routine returns the integer value for a labelled instruction from the
 * module's simulation script.
 */

int
GetInstIntVal_Utility_Datum(DatumPtr start, char *label)
{
	static const char *funcName = "GetInstIntVal_Utility_Datum";
	DatumPtr	pc;
	
	if (label == NULL) {
		NotifyError("%s: Illegal NULL label!", funcName);
		exit(1);
	}
	if ((pc = FindLabelledInst_Utility_Datum(start, label)) == NULL) {
		NotifyError("%s: Could not find label in simulation script.", funcName);
		exit(1);
	}
	if (pc->type != REPEAT) {
		NotifyError("%s: Labelled instruction has no associated integer value.",
		  funcName);
		exit(1);
	}
	return (pc->u.var);

}

/****************************** SetUniParValue ********************************/

/*
 * This function sets a simulation universal parameter value.
 * It returns TRUE if the operation is successful.
 * If the returned 'parList' is NULL, then no parameter is set.
 */

BOOLN
SetUniParValue_Utility_Datum(DatumPtr start, char *parName, char *parValue)
{
	static const char	*funcName = "SetUniParValue_Utility_Datum";
	uInt	index;
	UniParListPtr		parList;
	DatumPtr	pc;

	if (start == NULL) {
		NotifyError("%s: Simulation not initialised.", funcName);
		return(FALSE);
	}
	pc = start;
	if (!FindModuleUniPar_Utility_Datum(&parList, &index, &pc, parName, TRUE)) {
		if (!parList)
			return(TRUE);
		NotifyError("%s: Could not find '%s' parameter in simulation.",
		  funcName, parName);
		return(FALSE);
	}
	(* pc->data->module->SetParsPointer)(pc->data->module);
	return(SetParValue_UniParMgr(&parList, index, parValue));

}

/****************************** ResetStepCount ********************************/

/*
 * This routine resets the global datumStepCount variable.
 * It is needed because MS VC++ complains.
 */

void
ResetStepCount_Utility_Datum(void)
{
	datumStepCount = 0;

}

