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
#include <ctype.h>

#include "GeCommon.h"
#include "GeSignalData.h"
#include "GeEarObject.h"
#include "GeUniParMgr.h"
#include "GeModuleMgr.h"

#include "UtSSSymbols.h"
#include "UtSimScript.h"
#include "UtSSParser.h"
#include "UtString.h"

#include "UtDatum.h"

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

/****************************** InitInst **************************************/

/*
 * Initialises a datum instruction. The space is allocated and the basic 
 * settings assigned.
 */

DatumPtr
InitInst_Utility_Datum(int type)
{
	static const char *funcName = "InitInst_Utility_Datum";
	DatumPtr	datum;
	
	if ((datum = (DatumPtr) malloc(sizeof (Datum))) == NULL) {
		NotifyError("%s: Out of memory for Datum.", funcName);
		return(NULL);
	}
	datum->onFlag = TRUE;
	datum->type = type;
	datum->label = NULL_STRING;
	switch (type) {
	case PROCESS:
		datum->u.proc.parFile = NULL_STRING;
		datum->u.proc.moduleName = NULL_STRING;
		datum->u.proc.inputList = NULL;
		datum->u.proc.outputList = NULL;
		break;
	case RESET:
		datum->u.string = NULL_STRING;
		break;
	case REPEAT:
		datum->u.loop.count = 1;
		datum->u.loop.stopPC = NULL;
		break;
	default:
		;
	} /* Switch */
	datum->data = NULL;
	datum->clientData = NULL;
	datum->next = NULL;
	datum->previous = NULL;
	datum->stepNumber = datumStepCount++;
	return datum;

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
	
	if ((datum = InitInst_Utility_Datum(type)) == NULL) {
		NotifyError("%s: Could not initialise datum instruction.", funcName);
		return(NULL);
	}
	if ((pc = GetLastInst_Utility_Datum(*head)) == NULL) {
		*head = datum;
		datum->previous = NULL;
	} else {
		pc->next = datum;
		datum->previous = pc;
	}
	return datum;

}

/****************************** InsertInst ************************************/

/*
 * Insert a code datum instruction into a list before the current position.
 * It expects to the datum to have been already created elsewhere.
 */

void
InsertInst_Utility_Datum(DatumPtr *head, DatumPtr pos, DatumPtr datum)
{
	if (!*head) {	/* Start of simulation list */
		*head = datum;
		return;
	}
	if (!pos) {
		*head = datum;
		return;
	}
	if (pos == *head)
		*head = datum;
	if (pos->previous)
		pos->previous->next = datum;
	datum->previous = pos->previous;
	pos->previous = datum;
	datum->next = pos;

}

/****************************** AppendInst ************************************/

/*
 * Append a code datum instruction into a list after the current position.
 * It expects to the datum to have been already created elsewhere.
 */

void
AppendInst_Utility_Datum(DatumPtr *head, DatumPtr pos, DatumPtr datum)
{
	if (!*head) {	/* Start of simulation list */
		*head = datum;
		return;
	}
	datum->previous = pos;
	datum->next = pos->next;
	pos->next = datum;

}

/****************************** DisconnectInst ********************************/

/*
 * Disconnect datum instructions in a list.
 */

void
DisconnectInst_Utility_Datum(DatumPtr *head, DatumPtr from, DatumPtr to)
{
	from->next = NULL;
	to->previous = NULL;
	if (!DATUM_IN_SIMULATION(from) && (*head == from))
		*head = (DATUM_IN_SIMULATION(to))? to: NULL;
	if (!DATUM_IN_SIMULATION(to) && (*head == to))
		*head = NULL;

}

/****************************** ConnectInst ***********************************/

/*
 * Connect datum instructions in a list.
 */

void
ConnectInst_Utility_Datum(DatumPtr *head, DatumPtr from, DatumPtr to)
{
	if (!DATUM_IN_SIMULATION(from)) {
		if (DATUM_IN_SIMULATION(to)) {
			InsertInst_Utility_Datum(head, to, from);
		} else {
			InsertInst_Utility_Datum(head, NULL, from);
			AppendInst_Utility_Datum(head, from, to);
		}
	} else if (!DATUM_IN_SIMULATION(to))
		AppendInst_Utility_Datum(head, from, to);
	else {
		from->next = to;
		to->previous = from;
	}

}

/****************************** FreeInstruction *******************************/

/*
 * This routine frees a single Datum instructions.
 * It does not manage the simulation connections.
 * Note that all of the strings where initialised to 'NULL_STRING', but if
 * they were set, then the string was allocated space.
 */

void
FreeInstruction_Utility_Datum(DatumPtr *pc)
{
	if (!*pc)
		return;

	if (*(*pc)->label != '\0')
		free((*pc)->label);
	switch ((*pc)->type) {
	case PROCESS:
		if (*(*pc)->u.proc.parFile != '\0')
			free((*pc)->u.proc.parFile);
		if (*(*pc)->u.proc.moduleName != '\0')
			free((*pc)->u.proc.moduleName);
		if ((*pc)->u.proc.inputList)
			FreeList_Utility_DynaList(&(*pc)->u.proc.inputList);
		if ((*pc)->u.proc.outputList)
			FreeList_Utility_DynaList(&(*pc)->u.proc.outputList);
		Free_EarObject(&(*pc)->data);
		break;
	case RESET:
		if (*(*pc)->u.string != '\0')
			free((*pc)->u.string);
		break;
	case REPEAT:
		break;
	default:
		;
	} /* switch */
	free((*pc));
	*pc = NULL;

}

/****************************** FreeInstFromSim *******************************/

/*
 * This routine frees a single Datum instructions from the simulation.
 * Note that all of the strings where initialised to 'NULL_STRING', but if
 * they were set, then the string was allocated space.
 */

void
FreeInstFromSim_Utility_Datum(DatumPtr *start, DatumPtr pc)
{
	if (!pc)
		return;

	if (!pc->previous) {
		*start = pc->next;
		if (*start)
			(*start)->previous = NULL;
	} else
		pc->previous->next = pc->next;
	FreeInstruction_Utility_Datum(&pc);

}

/****************************** FreeInstructions ******************************/

/*
 * This routine frees the Datum instructions in the simulation.
 */

void
FreeInstructions_Utility_Datum(DatumPtr *pc)
{
	while (*pc)
		FreeInstFromSim_Utility_Datum(pc, *pc);

}

/****************************** PrintIndentAndLabel ***************************/

/*
 * This routine prints a specified number of tabs, with the label at the
 * previous tab level.
 */

void
PrintIndentAndLabel_Utility_Datum(DatumPtr pc, int indentLevel)
{
	int		i;

	for (i = 0; i < indentLevel; i++)
		DPrint("\t");
	if (((pc->type == PROCESS) || (pc->type == REPEAT)) && pc->label &&
	  (pc->label[0] != '\0'))
		DPrint("%s%%", pc->label);
	DPrint("\t");

}

/****************************** PrintConnections ******************************/

/*
 * This routine prints the connection labels in a dynamic array list.
 */

void
PrintConnections_Utility_Datum(DynaListPtr list)
{
	DynaListPtr	node;

	for (node = list; node != NULL; node = node->next) {
		if (node != list)
			DPrint(", ");
		DPrint("%s", (char *) node->data);
	}

}

/****************************** GetProcessName ********************************/

/*
 * This routine returns the process name.
 */

char *
GetProcessName_Utility_Datum(DatumPtr pc)
{
	static const char *funcName = "GetProcessName_Utility_Datum";
	static char		string[MAXLINE];

	if (!pc) {
		NotifyError("%s: Null process pointer given.", funcName);
		return(NULL);
	}
	switch (pc->type) {
	case PROCESS:
		return(pc->u.proc.moduleName);
	case REPEAT:
		return("repeat");
	case RESET:
		return("reset");
	default:
		snprintf(string, MAXLINE, "default %d", pc->type);
		return(string);
	} /* switch */

}

/****************************** PrintSimScript ********************************/

/*
 * This routine prints the supplied simulation script simulation script.
 * It returns 'TRUE' if there are subsimulation scripts to print.
 */

BOOLN
PrintSimScript_Utility_Datum(DatumPtr pc, char *scriptName, int indentLevel,
  char *prefix, BOOLN checkForSubSimScripts)
{
	BOOLN	subSimScripts = FALSE;
	char	*p, *scriptNameNoPath;
	int		i;

	if (pc == NULL)
		return(subSimScripts);
	if ((p = strrchr(scriptName, '/')) != NULL)
		scriptNameNoPath = p + 1;
	else if ((p = strrchr(scriptName, '\\')) != NULL)
		scriptNameNoPath = p + 1;
	else
		scriptNameNoPath = scriptName;
	DPrint("%sbegin %s {\n\n", prefix, scriptNameNoPath);
	for ( ; pc != NULL; pc = pc->next) {
		DPrint("%s", prefix);
		switch (pc->type) {
		case PROCESS:
			if (pc->data->module->specifier == SIMSCRIPT_MODULE)
				subSimScripts = TRUE;
			PrintIndentAndLabel_Utility_Datum(pc, indentLevel);
			if (!pc->data->module->onFlag)
				DPrint("%c ", SIMSCRIPT_DISABLED_MODULE_CHAR);
			DPrint("%-16s", pc->u.proc.moduleName);
			if (pc->u.proc.inputList || pc->u.proc.outputList) {
				DPrint("(");
				PrintConnections_Utility_Datum(pc->u.proc.inputList);
				DPrint("->");
				PrintConnections_Utility_Datum(pc->u.proc.outputList);
				DPrint(")");
			}
			if ((*pc->u.proc.parFile == '\0') || strcmp(pc->u.proc.parFile,
			  NO_FILE) == 0)
				DPrint("\n");
			else if (checkForSubSimScripts && (pc->data->module->specifier ==
			  SIMSCRIPT_MODULE))
				DPrint("\t< (%s)\n",  pc->u.proc.parFile);
			else
				DPrint("\t< %s\n",  pc->u.proc.parFile);
			break;
		case REPEAT:
			PrintIndentAndLabel_Utility_Datum(pc, indentLevel++);
			DPrint("%s %d {\n", GetProcessName_Utility_Datum(pc), pc->u.loop.
			  count);
			break;
		case RESET:
			PrintIndentAndLabel_Utility_Datum(pc, indentLevel);
			DPrint("%s\t%s\n", GetProcessName_Utility_Datum(pc), pc->u.string);
			break;
		case STOP:
			PrintIndentAndLabel_Utility_Datum(pc, --indentLevel);
			DPrint("}\n");
			break;
		default:
			PrintIndentAndLabel_Utility_Datum(pc, indentLevel);
			DPrint("%s\tvar = %d\n", GetProcessName_Utility_Datum(pc), pc->u.
			  loop.count);
			break;
		} /* switch */
	}
	for (i = 0; i < indentLevel; i++)
		DPrint("\t");
	DPrint("\n%s}\n", prefix);
	return(subSimScripts);

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
	DatumPtr	start = pc;

	if (pc == NULL)
		return;
	subSimScripts = PrintSimScript_Utility_Datum(pc, scriptName, indentLevel,
	  prefix, TRUE);
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

/****************************** CmpProcessLabels ******************************/

/*
 * This function cmpares the labels of two process nodes.
 * It returns a negative, zero, or postive integer values according to whether
 * the labels are less than, equal or greater than respectively.
 */

int
CmpProcessLabels_Utility_Datum(void *a, void *b)
{
	DatumPtr	aPtr = (DatumPtr) a, bPtr = (DatumPtr) b;

	return (StrCmpNoCase_Utility_String(aPtr->label, bPtr->label));

}

/****************************** CmpProcessLabel *******************************/

/*
 * This function cmpares the labels of two process nodes.
 * It returns a negative, zero, or postive integer values according to whether
 * the labels are less than, equal or greater than respectively.
 */

int
CmpProcessLabel_Utility_Datum(void *processNode, void *labelPtr)
{
	char	*label = (char *) labelPtr;
	DatumPtr	ptr = (DatumPtr) processNode;

	return (StrCmpNoCase_Utility_String(ptr->label, label));

}

/****************************** CmpLabel **************************************/

/*
 * This function cmpares a two string labels, passed as void pointers.
 * It returns a negative, zero, or postive integer values according to whether
 * the labels are less than, equal or greater than respectively.
 */

int
CmpLabel_Utility_Datum(void *a, void *b)
{
	char	*aLabel = (char *) a;
	char	*bLabel = (char *) b;

	return (StrCmpNoCase_Utility_String(aLabel, bLabel));

}

/****************************** SetOutputConnections **************************/

/*
 * This function sets up the specified connections for a simulation.
 * It assumes that the list has been properly initialised.
 */

BOOLN
SetOutputConnections_Utility_Datum(DatumPtr pc, DynaBListPtr labelBList)
{
	static const char *funcName = "SetOutputConnections_Utility_Datum";
	char	*label;
	DatumPtr	foundPC;
	DynaListPtr	p;
	DynaBListPtr	foundPtr;

	for (p = pc->u.proc.outputList; p != NULL; p = p->next) {
		label = (char *) p->data;
		if ((foundPtr = FindElement_Utility_DynaBList(labelBList,
		  CmpProcessLabel_Utility_Datum, label)) == NULL) {
			NotifyError("%s: Could not find process '%s%% %s.%lu' input "
			  "labelled '%s' in the simulation script.", funcName, pc->label,
			  pc->data->module->name, pc->data->handle, label);
			return(FALSE);
		}
		foundPC = (DatumPtr) foundPtr->data;
		if (!FindElement_Utility_DynaList(foundPC->u.proc.inputList,
		  CmpLabel_Utility_Datum, pc->label)) {
			NotifyError("%s: Could not find label '%s' in\nprocess "
			  "'%s%% %s.%lu' inputs.", funcName, pc->label, foundPC->label,
			  foundPC->data->module->name, foundPC->data->handle);
			return(FALSE);
		}
		if (!ConnectOutSignalToIn_EarObject(pc->data, foundPC->data)) {
			NotifyError("%s: Could not set '%s' connection for process %s.%lu.",
			  funcName, label, pc->data->module->name, pc->data->handle);
			return(FALSE);
		}
	}
	return(TRUE);

}

/****************************** CheckInputConnections *************************/

/*
 * This function checks that specified input connections are correctly resolved.
 */

BOOLN
CheckInputConnections_Utility_Datum(DatumPtr pc, DynaBListPtr labelBList)
{
	static const char *funcName = "CheckInputConnections_Utility_Datum";
	char	*label;
	DatumPtr	foundPC;
	DynaListPtr	p;
	DynaBListPtr	foundPtr;

	for (p = pc->u.proc.inputList; p != NULL; p = p->next) {
		label = (char *) p->data;
		if ((foundPtr = FindElement_Utility_DynaBList(labelBList,
		  CmpProcessLabel_Utility_Datum, label)) == NULL)  {
			NotifyError("%s: Could not find process '%s%% %s.%lu' input "
			  "labelled '%s' in the simulation script.", funcName, pc->label,
			  pc->data->module->name, pc->data->handle, label);
			return(FALSE);
		}
		foundPC = (DatumPtr) foundPtr->data;
		if (!FindElement_Utility_DynaList(foundPC->u.proc.outputList,
		  CmpLabel_Utility_Datum, pc->label)) {
			NotifyError("%s: Could not find label '%s' in\nprocess "
			  "'%s%% %s.%lu' outputs.", funcName, pc->label, foundPC->label,
			  foundPC->data->module->name, foundPC->data->handle);
			return(FALSE);
		}
	}
	return(TRUE);

}

/****************************** ResolveInstLabels *****************************/

/*
 * This routine checks that the labels used in instructions actually exist
 * within the simulation script instructions (such as reset, process connections
 * etc.).
 * It also sets the EarObject data pointer to that corresponding to the label,
 * sets up the connections or whatever is the appropriate action.
 * It must be used after the EarObjects have been initialised, if the data
 * pointers are to be valid.
 * It returns FALSE if it fails in any way.
 * Any errors here must always be reported, even if the error diagnostics had
 * been turned off.
 */

BOOLN
ResolveInstLabels_Utility_Datum(DatumPtr start, DynaBListPtr labelBList)
{
	static const char *funcName = "ResolveInstLabels_Utility_Datum";
	BOOLN		ok;
	FILE		*savedErrorsFileFP = GetDSAMPtr_Common()->errorsFile;
	DatumPtr	pc;
	DynaBListPtr	p;

	SetErrorsFile_Common("screen", OVERWRITE);
	for (pc = start, ok = TRUE; pc != NULL; pc = pc->next)
		switch (pc->type) {
		case RESET:
			if ((p = FindElement_Utility_DynaBList(labelBList,
			  CmpProcessLabel_Utility_Datum, pc->u.string)) == NULL) {
				NotifyError("%s: Could not find label '%s' in the simulation "
				  "script.", funcName, pc->u.string);
				ok = FALSE;
			} else
				pc->data = ((DatumPtr) p->data)->data;
			break;
		case PROCESS:
			if (pc->u.proc.outputList && !SetOutputConnections_Utility_Datum(
			  pc, labelBList)) {
				NotifyError("%s: Failed to set output connections.", funcName);
				ok = FALSE;
			}
			if (pc->u.proc.inputList && !CheckInputConnections_Utility_Datum(
			  pc, labelBList)) {
				NotifyError("%s: Failed to resolve input connections.",
				  funcName);
				ok = FALSE;
			}
			break;
		default:
			;
		}
	GetDSAMPtr_Common()->errorsFile = savedErrorsFileFP;
	return(ok);

}

/****************************** SetDefaultConnections *************************/

/*
 * This routine sets up the default forward connection of EarObjects.
 * It assumes that the list has been properly initialised.
 * It assumes that if the next process has its input specified, then no
 * connection should be made.
 */

BOOLN
SetDefaultConnections_Utility_Datum(DatumPtr start)
{
	/* static const char *funcName = "SetDefaultConnections_Utility_Datum"; */
	DatumPtr	pc1, pc2;

	for (pc1 = start; (pc1 != NULL); pc1 = pc1->next)
		if ((pc1->type == PROCESS) && !pc1->u.proc.outputList) {
			for (pc2 = pc1->next; (pc2 != NULL) && (pc2->type != PROCESS);
			  pc2 = pc2->next)
				;
			if ((pc2 != NULL) && !pc2->u.proc.inputList)
				ConnectOutSignalToIn_EarObject( pc1->data, pc2->data );
		}
	return(TRUE);

}

/****************************** SetDefaultLabel ******************************/

/*
 * This routine sets the default label for a process.
 * If a label has not been defined for a process, then by default it is set to
 * the process' step number.
 * It returns FALSE if it fails in any way.
 */

BOOLN
SetDefaultLabel_Utility_Datum(DatumPtr pc)
{
	static const char *funcName = "SetDefaultLabel_Utility_Datum";
	char	label[MAXLINE];

	if (((pc->type == PROCESS) || (pc->type == REPEAT) || (pc->type ==
	  RESET)) && (pc->label[0] == '\0')) {
		snprintf(label, MAXLINE, "%s%d", DATUM_DEFAULT_LABEL_PREFIX,
		  pc->stepNumber);
		if ((pc->label = InitString_Utility_String(label)) == NULL) {
			NotifyError("%s: Out of memory for label '%s'.", funcName, label);
			return(FALSE);
		}
	}
	return(TRUE);

}

/****************************** SetDefaultLabels ******************************/

/*
 * This routine sets the default labels.
 * If a label has not been defined for a process, then by default it is set to
 * the process step number.
 */

BOOLN
SetDefaultLabels_Utility_Datum(DatumPtr start)
{
	static const char *funcName = "SetDefaultLabels_Utility_Datum";
	DatumPtr	pc;

	for (pc = start; (pc != NULL); pc = pc->next)
		if (!SetDefaultLabel_Utility_Datum(pc)) {
			NotifyError("%s: Could not set default label '%s' for process "
			  "'%s'.", funcName, pc->stepNumber, pc->u.proc.moduleName);
			return(FALSE);
		}
	return(TRUE);

}

/****************************** InitialiseEarObjects **************************/

/*
 * This routine initialises and connects the EarObjects for the simulation
 * script.
 */

BOOLN
InitialiseEarObjects_Utility_Datum(DatumPtr start, DynaBListPtr *labelBList)
{
	static const char	*funcName = "InitialiseEarObjects_Utility_Datum";
	BOOLN	ok = TRUE;
	DatumPtr	pc;

	if (start == NULL) {
		NotifyError("%s: Simulation not initialised.\n", funcName);
		return(FALSE);
	}
	if (!SetDefaultLabels_Utility_Datum(start)) {
		NotifyError("%s: Could not set default labels.", funcName);
		return(FALSE);
	}
	for (pc = start; pc != NULL; pc = pc->next)
		if (pc->type == PROCESS) {
			if ((pc->data = Init_EarObject(pc->u.proc.moduleName)) == NULL) {
				NotifyError("%s: Could not initialise process with '%s'.",
				  funcName, pc->u.proc.moduleName);
				ok = FALSE;
			}
			if ((pc->label[0] != '\0') && !Insert_Utility_DynaBList(labelBList,
			  CmpProcessLabels_Utility_Datum, pc)) {
				NotifyError("%s: Cannot insert process labelled '%s' into "
				  "simulation.", funcName, pc->label);
				ok = FALSE;
			}
		}
	if (ok)
		ok = ResolveInstLabels_Utility_Datum(start, *labelBList);

	if (ok && !SetDefaultConnections_Utility_Datum(start)) {
		NotifyError("%s Could not set default foward connections.", funcName);
		ok = FALSE;
	}
	return (ok);

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



/****************************** NameAndLabel **********************************/

/*
 * This routine returns a pointer to a string containing the proces name and
 * label for a Datum structure.
 * A copy of this information should be used if a permanent string is required.
 */

char *
NameAndLabel_Utility_Datum(DatumPtr pc)
{
	static const char	*funcName = "NameAndLabel_Utility_Datum";
	static char	string[MAXLINE];

	if (!pc) {
		NotifyError("%s: Pointer not initialised.", funcName);
		return(NULL);
	}
	snprintf(string, MAXLINE, "%s.%s", GetProcessName_Utility_Datum(pc), pc->
	  label);
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
	
	for (pc = start, ok = TRUE; (pc != NULL) && ok; pc = pc->next)
		if (pc->type == PROCESS ) {
			if (GetDSAMPtr_Common()->usingGUIFlag && (pc->data->module->
			  specifier == DISPLAY_MODULE) && (GetUniParPtr_ModuleMgr(pc->data,
			  "win_title")->valuePtr.s[0] == '\0')) {
				SetPar_ModuleMgr(pc->data, "win_title",
				  NameAndLabel_Utility_Datum(pc));
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
	char	suffix[MAXLINE], fmtParFileName[MAXLINE];
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
				snprintf(fmtParFileName, MAXLINE, "(%s)", pc->u.proc.parFile);
				DPrint("##----- %-20s %20s -----##\n",
				  NameAndLabel_Utility_Datum(pc), fmtParFileName);
				snprintf(suffix, MAXLINE, ".%s", NameAndLabel_Utility_Datum(
				  pc));
				ok = (parList)? PrintPars_UniParMgr(parList, prefix, suffix):
				  TRUE;
				DPrint("\n");
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
			if (!RunProcess_ModuleMgr(pc->data)) {
				NotifyError("%s: Could not run process '%s'.", funcName,
				  pc->label);
				return(NULL);
			}
			break;
		case RESET:
			ResetProcess_EarObject(pc->data);
			break;
		case STOP:
			return (pc);
		case REPEAT:
			if (!pc->u.loop.count) {
				NotifyError("%s: Illegal zero 'repeat' count.", funcName);
				return(NULL);
			}
			if (!pc->u.loop.stopPC) {
				NotifyError("%s: Repeat has no end point.", funcName);
				return(NULL);
			}
			for (i = 0; i < pc->u.loop.count; i++)
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
		if ((pc->type == PROCESS) && ((label[0] == '*') || (strcmp(label, pc->
		  label) == 0)))
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
		if ((pc->type == PROCESS) && ((label[0] == '*') || (strcmp(label, pc->
		  label) == 0)))
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
		  '*') || (strcmp(label, pc->label) == 0)))
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
 * If the process label starts with a digit, then it is assumed that an older
 * SPF file is being read.
 */

BOOLN
FindModuleUniPar_Utility_Datum(UniParListPtr *parList, uInt *index,
  DatumPtr *pc, char *parSpecifier, BOOLN diagnosticsOn)
{
	static const char *funcName = "FindModuleUniPar_Utility_Datum";
	char		*p, parName[MAXLINE], processName[MAXLINE], processStr[MAXLINE];
	char		processLabel[MAXLINE];
	DatumPtr	tempPc;
	UniParPtr		par;
	UniParListPtr	pList = NULL;

	if (parSpecifier == NULL) {
		NotifyError("%s: Illegal parameter name '%s'!", funcName, parSpecifier);
		return(FALSE);
	}
	snprintf(parName, MAXLINE, "%s", parSpecifier);
	if ((p = strchr(parName, UNIPAR_NAME_SEPARATOR)) == NULL) {
		processName[0] = '\0';
		processLabel[0] = '\0';
	} else {
		snprintf(processName, MAXLINE, "%s", p + 1);
		if ((p = strchr(processName, UNIPAR_NAME_SEPARATOR)) != NULL) {
			snprintf(processLabel, MAXLINE, "%s%s", (isdigit(*(p + 1)))?
			  DATUM_DEFAULT_LABEL_PREFIX: "", p + 1);
			*p = '\0';
		} else
			processLabel[0] = '\0';
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
			  GetUniParListPtr_ModuleMgr((*pc)->data)) != NULL) &&
			  ((processLabel[0] == '\0') || (StrCmpNoCase_Utility_String(
			  (*pc)->label, processLabel) == 0))) {
				if ((par = FindUniPar_UniParMgr(&pList, parName,
				  UNIPAR_SEARCH_ABBR)) != NULL) {
			  		*parList = pList;
					*index = par->index;
					return(TRUE);
				}
			}
		}
	if (diagnosticsOn && pList) {
		if (processLabel[0] == '\0')
			processStr[0] = '\0';
		else
			snprintf(processStr, MAXLINE, " for process [%s] ", processLabel);
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
	char	*p, processName[MAXLINE], processStr[MAXLINE];
	char	processLabel[MAXLINE];

	if (processSpecifier == NULL) {
		NotifyError("%s: Illegal parameter name '%s'!", funcName,
		  processSpecifier);
		return(FALSE);
	}
	snprintf(processName, MAXLINE, "%s", processSpecifier);
	if ((p = strchr(processName, UNIPAR_NAME_SEPARATOR)) == NULL)
		processLabel[0] = '\0';
	else
		snprintf(processLabel, MAXLINE, "%s", p + 1);
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
			  pc->data->module->name, processName) == 0)) && ((processLabel[
			  0] == '\0') || (StrNCmpNoCase_Utility_String(pc->label,
			  processLabel) == 0)))
				return(pc->data);
		}
	if (processLabel[0] == '\0')
		processStr[0] = '\0';
	else
		snprintf(processStr, MAXLINE, ", label [%s] ", processLabel);
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
	return (pc->u.loop.count);

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

/****************************** FindNearestProcesses **************************/

/*
 * This routine ensures that process instructions are connected 'though' non-
 * process instructions like 'reset' and 'repeat'.
 * It changes the pointer arguments to the nearest process found.
 * It return's TRUE if a nearest process is found.
 */

BOOLN
FindNearestProcesses_Utility_Datum(DatumPtr *fromPc, DatumPtr *toPc)
{
	if (((*fromPc)->type != PROCESS) && ((*toPc)->type != PROCESS))
		return(FALSE);
	while (*fromPc && ((*fromPc)->type != PROCESS))
		*fromPc = (*fromPc)->previous;
	if (!*fromPc)
		return(FALSE);
	while (*toPc && ((*toPc)->type != PROCESS))
		*toPc = (*toPc)->next;
	if (!*toPc)
		return(FALSE);
	return(TRUE);

}

/****************************** SetDefaultProcessFileName *********************/

/*
 * This routine sets the process file name to the module number with the
 * step number as a suffix, but only if the file name has not already been set.
 */

void
SetDefaultProcessFileName_Utility_Datum(DatumPtr pc)
{
	char	fileName[MAXLINE];

	if (pc->type != PROCESS)
		return;
	if ((*pc->u.proc.parFile == '\0') || (strcmp(pc->u.proc.parFile, NO_FILE)
	  == 0)) {
		snprintf(fileName, MAXLINE, "%s_%d.par", pc->u.proc.moduleName,
		  pc->stepNumber);
		pc->u.proc.parFile = InitString_Utility_String(fileName);
	}

}

/*************************** WriteSimFiles ************************************/

/*
 * This function prints a simulation as a ".sim" file with the associated
 * "*.par"files.
 * It returns FALSE if it fails in any way.
 */

BOOLN
WriteSimFiles_Datum(char  *fileName, DatumPtr start)
{
	static const char *funcName = "PrintSimFile_Datum";
	DatumPtr	pc;
	FILE *oldFp = GetDSAMPtr_Common()->parsFile;
	UniParListPtr	parList;

	if (!start) {
		NotifyError("%s: Simulation not initialised.", funcName);
		return(FALSE);
	}
	for (pc = start; pc; pc = pc->next) {
		if (pc->type != PROCESS)
			continue;
		if (pc->data->module->specifier == SIMSCRIPT_MODULE) {
			WriteSimFiles_Datum(pc->u.proc.parFile, GetSimulation_ModuleMgr(pc->
			  data));
			continue;
		}
		SET_PARS_POINTER(pc->data);
		if ((parList = (* pc->data->module->GetUniParListPtr)()) == NULL)
			return(TRUE);
		SetDefaultProcessFileName_Utility_Datum(pc);
		SetParsFile_Common(pc->u.proc.parFile, OVERWRITE);
		PrintParList_UniParMgr(parList);
		fclose(GetDSAMPtr_Common()->parsFile);
	}
	SetParsFile_Common(fileName, OVERWRITE);
	PrintSimScript_Utility_Datum(start, fileName, 0, "", FALSE);
	fclose(GetDSAMPtr_Common()->parsFile);
	GetDSAMPtr_Common()->parsFile = oldFp;
	return(TRUE);

}
