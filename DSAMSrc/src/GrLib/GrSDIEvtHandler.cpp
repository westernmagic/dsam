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
			return(true);
		}
		break;
	default:
		NotifyError("%s: Unknown process type (%d).\n", funcName, processType);
		return(true);
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
		printf("%s: Debug: This routine also needs to ask for a process "
		  "label\n", funcName);
		switch (pc->type) {
		case REPEAT: {
			long	count;

			label.ToLong(&count);
			if (count >= 1)
				pc->u.loop.count = count;
			label.Printf("repeat %d", pc->u.loop.count);
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

void
SDIEvtHandler::FreeInstruction(void)
{
	if (pc)
		FreeInstruction_Utility_Datum(&pc, pc);

}

/******************************************************************************/
/****************************** SetSelectedShape ******************************/
/******************************************************************************/

void
SDIEvtHandler::SetSelectedShape(wxClientDC &dc)
{
	if (GetShape()->Selected()) {
		GetShape()->Select(FALSE, &dc);
		// Redraw because bits of objects will be are missing
		GetShape()->GetCanvas()->Redraw(dc);
	} else {
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
		SetSelectedShape(dc);
	} else 
		if (keys & KEY_CTRL) {
			printf("Debug: ctrl left click.\n");
		} else {
			wxGetApp().GetFrame()->SetStatusText(label);
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
	wxClientDC dc(GetShape()->GetCanvas());
	GetShape()->GetCanvas()->PrepareDC(dc);

	if (keys == 0) {
		SetSelectedShape(dc);
		if (!dialog && pc) {
			switch (pc->type) {
			case PROCESS: {
				UniParListPtr	parList = GetUniParListPtr_ModuleMgr(pc->data);
				wxString	title;

				switch (parList->mode) {
				case UNIPAR_SET_SIMSPEC: {
					printf("SDIEvtHandler::OnRightClick: Open child SDI "
					  "window.\n");
					break; }
				default:
					title = (pc->data->module->specifier == DISPLAY_MODULE)?
					  parList->pars[DISPLAY_WINDOW_TITLE].valuePtr.s:
					  NameAndLabel_Utility_Datum(pc);
					dialog = new ModuleParDialog(((SDICanvas *) GetShape()->
					  GetCanvas())->parent, title, pc, parList, this,
					  300, 300, 500, 500, wxDEFAULT_DIALOG_STYLE);
					dialog->SetNotebookSelection();
					dialog->Show(TRUE);
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
			SetSelectedShape(dc);
			SDICanvas *canvas = (SDICanvas *) GetShape()->GetCanvas();
			((SDIView *) canvas->view)->ProcessListDialog();
		} else {
			wxGetApp().GetFrame()->SetStatusText(label);
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

	if (otherShape && !otherShape->IsKindOf(CLASSINFO(wxLineShape))) {
		canvas->view->GetDocument()->GetCommandProcessor()->Submit(
		new SDICommand("wxLineShape", SDIFRAME_ADD_LINE, (SDIDocument *)
		  canvas->view->GetDocument(), CLASSINFO(wxLineShape), -1, 0.0, 0.0,
		  FALSE, NULL, GetShape(), otherShape));
	}

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
