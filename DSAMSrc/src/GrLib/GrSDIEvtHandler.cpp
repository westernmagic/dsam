/**********************
 *
 * File:		GrSDIEvtHandler.cpp
 * Purpose: 	Diagram class Simulation Design Interface.
 * Comments:	Revised from Julian Smart's Ogledit/doc.h
 * Author:		L.P.O'Mard
 * Created:		13 Nov 2002
 * Updated:		
 * Copyright:	(c) 2002, CNBH, University of Essex
 *
 **********************/

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG_H */

#ifdef HAVE_WX_OGL_OGL_H

#include <stdio.h>
#include <stdlib.h>

#include "DSAM.h"
#include "UtSSParser.h"

/******************************************************************************/
/****************************** Bitmaps ***************************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Methods (Subroutines) *************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Constructor ***********************************/
/******************************************************************************/

SDIEvtHandler::SDIEvtHandler(wxShapeEvtHandler *prev, wxShape *shape,
  const wxString& lab, int theProcessType): wxShapeEvtHandler(prev, shape)
{
	label = lab;
	processType = theProcessType;
	pc = NULL;
	dialog = NULL;

}

/******************************************************************************/
/****************************** Destructor ************************************/
/******************************************************************************/

SDIEvtHandler::~SDIEvtHandler(void)
{

}

/******************************************************************************/
/****************************** GetProcList ***********************************/
/******************************************************************************/

wxStringList *
SDIEvtHandler::GetProcessList(void)
{
	static const char	*funcName = "SDIEvtHandler::GetProcessList";

	switch (processType) {
	case PALETTE_ANALYSIS:
		return(&wxGetApp().anaList);
	case PALETTE_CONTROL:
		return(&wxGetApp().ctrlList);
	case PALETTE_FILTERS:
		return(&wxGetApp().filtList);
	case PALETTE_IO:
		return(&wxGetApp().ioList);
	case PALETTE_MODELS:
		return(&wxGetApp().modelsList);
	case PALETTE_STIMULI:
		return(&wxGetApp().stimList);
	case PALETTE_TRANSFORMS:
		return(&wxGetApp().transList);
	case PALETTE_USER:
		return(&wxGetApp().userList);
	case PALETTE_UTILITIES:
		return(&wxGetApp().utilList);
	default:
		NotifyError("%s: Unknown process type (%d).\n", funcName, processType);
		return(NULL);
	}

}

/******************************************************************************/
/****************************** ResetLabel ************************************/
/******************************************************************************/

/*
 * This routine resets the label using the various respective positions for
 * the different elements.
 */

void
SDIEvtHandler::ResetLabel(void)
{
	label.Empty();
	if (!pc->defaultLabelFlag)
		label.Printf("%s\n", pc->label);
	switch (pc->type) {
	case REPEAT: {
		wxString strCount;
		strCount.Printf("repeat\n%d", pc->u.loop.count);
		label += strCount;
		break; }
	case RESET: {
		wxString str;
		str.Printf("reset\n%s", pc->u.string);
		label += str;
		break; }
	case PROCESS:
		label += pc->u.proc.moduleName;
		break;
	default:
		label += "undefined";
	}
	
}

/******************************************************************************/
/****************************** InitInstruction *******************************/
/******************************************************************************/

bool
SDIEvtHandler::InitInstruction(void)
{
	static const char	*funcName = "SDIEvtHandler::InitInstruction";

	if (pc) {
		NotifyError("%s: datum already initialised!", funcName);
		return(false);
	}
	switch (processType) {
	case PALETTE_CONTROL: {
		SymbolPtr	sp = LookUpSymbol_Utility_SSSymbols(simScriptPtr->symList,
	  	  (char *) label.GetData());
		if ((pc = InitInst_Utility_Datum(sp->type)) == NULL) {
			NotifyError("%s: Could not create '%s' control intruction for "
			  "process '%s'.", funcName, (char *) label.GetData());
			return(false);
		}
		break; }
	case PALETTE_ANALYSIS:
	case PALETTE_FILTERS:
	case PALETTE_IO:
	case PALETTE_MODELS:
	case PALETTE_STIMULI:
	case PALETTE_TRANSFORMS:
	case PALETTE_USER:
	case PALETTE_UTILITIES:
		if ((pc = InitInst_Utility_Datum(PROCESS)) == NULL) {
			NotifyError("%s: Could not create new intruction for process '%s'.",
			  funcName, (char *) label.GetData());
			return(false);
		}
		break;
	default:
		NotifyError("%s: Unknown process type (%d).\n", funcName, processType);
		return(false);
	}
	if (!SetDefaultLabel_Utility_Datum(pc)) {
		NotifyError("%s: Could not create set the default process label.",
		  funcName);
		return(false);
	}
	return(true);

}

/******************************************************************************/
/****************************** EditInstruction *******************************/
/******************************************************************************/

bool
SDIEvtHandler::EditInstruction(void)
{
	static const char	*funcName = "SDIEvtHandler::EditInstruction";

	if (label.IsEmpty())
		return(true);
	switch (processType) {
	case PALETTE_CONTROL: {
		switch (pc->type) {
		case REPEAT: {
			long	count;

			label.ToLong(&count);
			if (count >= 1)
				pc->u.loop.count = count;
			ResetLabel();
			break; }
		default:
			;
		} /* switch */
		break; }
	case PALETTE_ANALYSIS:
	case PALETTE_FILTERS:
	case PALETTE_IO:
	case PALETTE_MODELS:
	case PALETTE_STIMULI:
	case PALETTE_TRANSFORMS:
	case PALETTE_USER:
	case PALETTE_UTILITIES:
		if (*pc->u.proc.moduleName != '\0')
			free(pc->u.proc.moduleName);
		pc->u.proc.moduleName = InitString_Utility_String((char *) label.
		  GetData());
		if (!pc->data)
			pc->data = Init_EarObject(pc->u.proc.moduleName);
		else {
			ResetProcess_EarObject(pc->data);
			Free_ModuleMgr(&(pc->data->module));
			pc->data->module = Init_ModuleMgr(pc->u.proc.moduleName);
		}
		break;
	default:
		NotifyError("%s: Unknown process type (%d).\n", funcName, processType);
		return(false);
	}
	return(true);

}

/******************************************************************************/
/****************************** FreeInstruction *******************************/
/******************************************************************************/

/*
 * This routine free's the memory for an instruction.
 * It does not manage the simulation connections, as these are taken care of by
 * the 'line' connection code.
 */

void
SDIEvtHandler::FreeInstruction(void)
{
	if (pc)
		FreeInstruction_Utility_Datum(&pc);

}

/******************************************************************************/
/****************************** SetSelectedShape ******************************/
/******************************************************************************/

/*
 * This routine returns TRUE if the shape was already selected.
 */

bool
SDIEvtHandler::SetSelectedShape(wxClientDC &dc)
{
	bool	alreadySelected = false;

	if (GetShape()->Selected())
		alreadySelected = true;
	else {
		// Ensure no other shape is selected, to simplify Undo/Redo code
		bool redraw = FALSE;
		wxNode *node = GetShape()->GetCanvas()->GetDiagram()->GetShapeList(
		  )->First();
		while (node) {
			wxShape *eachShape = (wxShape *)node->Data();
			if (eachShape->GetParent() == NULL) {
				if (eachShape->Selected()) {
					eachShape->Select(FALSE, &dc);
					redraw = TRUE;
				}
			}
			node = node->Next();
		}
		GetShape()->Select(TRUE, &dc);
		if (redraw)
			GetShape()->GetCanvas()->Redraw(dc);
	}
	return(alreadySelected);

}

/******************************************************************************/
/****************************** Callbacks *************************************/
/******************************************************************************/

/******************************************************************************/
/****************************** OnLeftClick ***********************************/
/******************************************************************************/

/*
 * SDIEvtHandler: an event handler class for all shapes
 */
 
void
SDIEvtHandler::OnLeftClick(double x, double y, int keys, int attachment)
{
	wxClientDC dc(GetShape()->GetCanvas());
	GetShape()->GetCanvas()->PrepareDC(dc);

	if (keys == 0) {
		if (SetSelectedShape(dc)) {
			GetShape()->Select(FALSE, &dc);
			// Redraw because bits of objects will be are missing
			GetShape()->GetCanvas()->Redraw(dc);
		}
	} else 
		if (keys & KEY_CTRL) {
			printf("SDIEvtHandler::OnLeftClick: Debug: ctrl left click.\n");
		} else {
			printf("SDIEvtHandler::OnLeftClick: Debug: shift left click.\n");
		}

}

/******************************************************************************/
/****************************** OnLeftDoubleClick *****************************/
/******************************************************************************/

/*
 * SDIEvtHandler: an event handler class for all shapes
 */

/*
 * If this routine is called, and the instruction is unset, then call the set
 * instruction dialog, otherwiase edit the instruction parameters.
 * Any subsequent changes of instruction needs to be done using "right click".
 */

void
SDIEvtHandler::OnLeftDoubleClick(double x, double y, int keys, int attachment)
{
	wxClientDC dc(GetShape()->GetCanvas());
	GetShape()->GetCanvas()->PrepareDC(dc);

	if (keys == 0) {
		SetSelectedShape(dc);
		if (!pc) {
			SDICanvas *canvas = (SDICanvas *) GetShape()->GetCanvas();
			((SDIView *) canvas->view)->ProcessListDialog();
		} else if (!dialog) {
			switch (pc->type) {
			case PROCESS: {
				UniParListPtr	parList = GetUniParListPtr_ModuleMgr(pc->data);
				wxString	title;

				switch (parList->mode) {
				case UNIPAR_SET_SIMSPEC: {
					printf("SDIEvtHandler::OnLeftDoubleClick: Open child SDI "
					  "window.\n");
					break; }
				default: {
					int		winX, winY;
					SDICanvas	*canvas = (SDICanvas *) GetShape()->GetCanvas();
					canvas->parent->GetPosition(&winX, &winY);

					title = (pc->data->module->specifier == DISPLAY_MODULE)?
					  parList->pars[DISPLAY_WINDOW_TITLE].valuePtr.s:
					  NameAndLabel_Utility_Datum(pc);
					dialog = new ModuleParDialog(canvas->parent, title, pc,
					  parList, this, (int) (winX + x), (int) (winY + y), 500,
					  500, wxDEFAULT_DIALOG_STYLE);
					dialog->SetNotebookSelection();
					dialog->Show(TRUE);
					}
				} /* switch */
				break; }
			case REPEAT: {
				SDICanvas *canvas = (SDICanvas *) GetShape()->GetCanvas();
				((SDIView *) canvas->view)->EditCtrlProperties();
				break; }
			default:
				;
			} /* switch */
		}
	} else 
		if (keys & KEY_CTRL) {
 		   // Do something for CONTROL
		} else {
		   // Do something for for SHIFT ?
		}

}

/******************************************************************************/
/****************************** OnRightClick **********************************/
/******************************************************************************/

/*
 * SDIEvtHandler: an event handler class for all shapes
 */
 
void
SDIEvtHandler::OnRightClick(double x, double y, int keys, int attachment)
{
	SDICanvas *canvas = (SDICanvas *)GetShape()->GetCanvas();
	wxClientDC dc(canvas);
	canvas->PrepareDC(dc);

	if (keys == 0) {
		SetSelectedShape(dc);
		wxMenu menu("Edit Process");
		menu.Append(SDIFRAME_EDIT_MENU_ENABLE, "&Enable", "Enable/disable "
		  "process", TRUE);
		menu.Append(SDIFRAME_EDIT_MENU_CHANGE_PROCESS, "&Change process",
		  "Change process");
		menu.AppendSeparator();
		menu.Append(SDIFRAME_EDIT_MENU_PROPERTIES, "&Properties...");

		canvas->PopupMenu(&menu, (int) x, (int) y);
		
	} else 
		if (keys & KEY_CTRL) {
			printf("SDIEvtHandler::OnRightClick: Debug: ctrl left click.\n");
		} else {
			printf("SDIEvtHandler::OnRightClick: Debug: shift left click.\n");
		}

}

/******************************************************************************/
/****************************** OnBeginDragRight ******************************/
/******************************************************************************/

/*
 * Implement connection of two shapes by right-dragging between them.
 */

void
SDIEvtHandler::OnBeginDragRight(double x, double y, int keys, int attachment)
{
	// Force attachment to be zero for now. Eventually we can deal with
	// the actual attachment point, e.g. a rectangle side if attachment mode
	// is on.
	attachment = 0;

	wxClientDC dc(GetShape()->GetCanvas());
	GetShape()->GetCanvas()->PrepareDC(dc);

	wxPen dottedPen(wxColour(0, 0, 0), 1, wxDOT);
	dc.SetLogicalFunction(OGLRBLF);
	dc.SetPen(dottedPen);
	double xp, yp;
	GetShape()->GetAttachmentPosition(attachment, &xp, &yp);
	dc.DrawLine((long) xp, (long) yp, (long) x, (long) y);
	GetShape()->GetCanvas()->CaptureMouse();

}

/******************************************************************************/
/****************************** OnDragRight ***********************************/
/******************************************************************************/

void
SDIEvtHandler::OnDragRight(bool draw, double x, double y, int keys,
  int attachment)
{
	// Force attachment to be zero for now
	attachment = 0;

	wxClientDC dc(GetShape()->GetCanvas());
	GetShape()->GetCanvas()->PrepareDC(dc);

	wxPen dottedPen(wxColour(0, 0, 0), 1, wxDOT);
	dc.SetLogicalFunction(OGLRBLF);
	dc.SetPen(dottedPen);
	double xp, yp;
	GetShape()->GetAttachmentPosition(attachment, &xp, &yp);
	dc.DrawLine((long) xp, (long) yp, (long) x, (long) y);

}

/******************************************************************************/
/****************************** OnEndDragRight ********************************/
/******************************************************************************/

void
SDIEvtHandler::OnEndDragRight(double x, double y, int keys, int attachment)
{
	GetShape()->GetCanvas()->ReleaseMouse();
	SDICanvas *canvas = (SDICanvas *)GetShape()->GetCanvas();

	// Check if we're on an object
	int new_attachment;
	wxShape *otherShape = canvas->FindFirstSensitiveShape(x, y, &new_attachment,
	  OP_DRAG_RIGHT);

	if (!otherShape || otherShape->IsKindOf(CLASSINFO(wxLineShape)))
		return;

	DatumPtr	pc = SHAPE_PC(GetShape());
	if (pc && pc->next) {
		switch (pc->type) {
		case REPEAT:
			if (pc->u.loop.stopPlaced)	/* Existing repeat connection */
				break;
			canvas->view->GetDocument()->GetCommandProcessor()->Submit(
			  new SDICommand("'repeat' connection", SDIFRAME_ADD_REPEAT_LINE,
			  (SDIDocument *) canvas->view->GetDocument(), CLASSINFO(
			  wxLineShape), -1, 0.0, 0.0, FALSE, NULL, GetShape(), otherShape));
			return;
		case RESET:
			canvas->view->GetDocument()->GetCommandProcessor()->Submit(
			  new SDICommand("Set 'reset'", SDIFRAME_SET_RESET, (SDIDocument *)
			  canvas->view->GetDocument(), NULL,
			  -1, 0.0, 0.0, FALSE, NULL, GetShape(), otherShape));
			return;
		default:
			;
		}
		
	}
	canvas->view->GetDocument()->GetCommandProcessor()->Submit(new SDICommand(
	  "process connection", SDIFRAME_ADD_LINE, (SDIDocument *) canvas->view->
	  GetDocument(), CLASSINFO(wxLineShape), -1, 0.0, 0.0, FALSE, NULL,
	  GetShape(), otherShape));

}

/******************************************************************************/
/****************************** OnEndSize *************************************/
/******************************************************************************/

void
SDIEvtHandler::OnEndSize(double x, double y)
{
	wxClientDC dc(GetShape()->GetCanvas());
	GetShape()->GetCanvas()->PrepareDC(dc);

	GetShape()->FormatText(dc, (char*) (const char*) label);

}

#endif /* HAVE_WX_OGL_OGL_H */
