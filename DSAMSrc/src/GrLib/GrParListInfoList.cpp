/**********************
 *
 * File:		GrParListInfoList.cpp
 * Purpose:		Module parameter information list handling routines.
 * Comments:	
 * Author:		L. P. O'Mard
 * Created:		11 Feb 1999
 * Updated:		
 * Copyright:	(c) 1999, University of Essex
 *
 **********************/

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG_H */

#include <stdio.h>
#include <stdlib.h>

#define DSAM_DO_NOT_DEFINE_MYAPP	1

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
/****************************** ModuleParDialog Methods ***********************/
/******************************************************************************/

/****************************** Constructor ***********************************/

ParListInfoList::ParListInfoList(DialogList *theParent, DatumPtr pc,
  UniParListPtr parList)
{
	static const char *funcName = "ParListInfoList::ParListInfoList";

	notebookPanel = 0;
	parent = theParent;
	notebook = NULL;
	useNotebookControls = FALSE;

	if ((pc == NULL) && (parList == NULL)) {
		NotifyError("%s: Both the simulation (pc) and the parameter list are "
		  "NULL.", funcName);
		return;
	}
	if (parList->GetPanelList)
		SetPanelledModuleInfo(parent, pc, parList);
	else
		SetStandardInfo(parent, pc, parList, "General");
	if (notebook) {
		if (parList->notebookPanel < 0)
			parList->notebookPanel = notebookPanel;
		notebook->SetSelection(parList->notebookPanel);
		parent->SetSize(GetNotebookSize());
	}

}

/****************************** Destructor ************************************/

ParListInfoList::~ParListInfoList(void)
{

}

/****************************** GetNotebookSize *******************************/

/*
 * This function calculates the maximum size for a notebook.
 * This is then used to calculate the height of the parent of the notebook,
 * because I cannot get constraints to work properly when notebooks are used.
 * There is a bug in MSW wxWindows in that there is no function for slider that
 * returns a wxSize element.
 */

wxSize
ParListInfoList::GetNotebookSize(void) const
{
	int		i, maxWidth = 325, maxHeight = 0, heightSum, width, height;
	wxSize	infoSize, cSize, sSize;
	wxNode	*node;
	ParListInfo	*infoPtr;
	ParControl	*control;

	node = list.First();
	while (node) {
		infoPtr = (ParListInfo *) node->Data();
		if (infoPtr->GetSize() == wxDefaultSize) {
			for (i = 0, heightSum = 0; i < infoPtr->GetNumPars(); i++) {
				control = infoPtr->GetParControl(i);
				cSize = control->GetSize();
				heightSum += cSize.GetHeight() + PARLISTINFO_DEFAULT_Y_MARGIN;
				if (control->GetSlider()) {
					// See note in function comment.
					// sSize = control->GetSlider()->GetSize(&width, &height);
					control->GetSlider()->GetSize(&width, &height);
					sSize.Set(width, height);
					heightSum += sSize.GetHeight() +
					  PARLISTINFO_DEFAULT_Y_MARGIN;
					if ((sSize.GetWidth() + PARLISTINFO_DEFAULT_X_MARGIN * 2) >
					  maxWidth)
						maxWidth = sSize.GetWidth() + 4 *
						  PARLISTINFO_DEFAULT_X_MARGIN;
				}
			}
			if (heightSum > maxHeight)
				maxHeight = heightSum;
		} else {
			infoSize = infoPtr->GetSize();
			if (infoSize.GetWidth() > maxWidth)
				maxWidth = infoSize.GetWidth();
			if (infoSize.GetHeight() > maxHeight)
				maxHeight = infoSize.GetHeight();
		}
		node = node->Next();
	}
	return(wxSize(maxWidth, maxHeight + PARLISTINFOLIST_NOTEBOOK_ADD_HEIGHT));

}

/****************************** UsingNotebook *********************************/

/*
 * This routine checks if a notebook needs to be initialised.
 */

wxPanel *
ParListInfoList::UsingNotebook(UniParListPtr parList, const wxString& title)
{
	/* static const char *funcName = "ParListInfoList::UsingNotebook"; */
	bool	useNotebook = FALSE;
	int		i;
	wxPanel	*panel = NULL;

	if (!notebook) {
		for (i = 0; (i < parList->numPars) && !useNotebook; i++)
			switch (parList->pars[i].type) {
			case UNIPAR_CFLIST:
			case UNIPAR_MODULE:
				useNotebook = TRUE;
		 		break;
			case UNIPAR_PARLIST:
				useNotebook = (*parList->pars[i].valuePtr.parList != NULL);
		 		break;
			case UNIPAR_ICLIST:
				useNotebook = TRUE;
				useNotebookControls = TRUE;
		 		break;
			default:
				;
			}
		if (parList->GetPanelList)
			useNotebook = TRUE;
	}
	if (useNotebook) {
		notebook = new wxNotebook(parent, PARLISTINFOLIST_ID_NOTEBOOK);

		wxLayoutConstraints* c = new wxLayoutConstraints;
		c->left.SameAs(parent, wxLeft, 4);
		c->right.SameAs(parent, wxRight, 4);
		c->top.SameAs(parent, wxTop, 4);
		c->bottom.SameAs(parent, wxBottom, 40);
		notebook->SetConstraints(c);

		notebook->SetAutoLayout(TRUE);
	}
	if (notebook) {
		panel = new wxPanel(notebook, -1);
		panel->SetAutoLayout(TRUE);
		if (parList->mode == UNIPAR_SET_CFLIST)
			notebookPanel = notebook->GetPageCount() - 1;
		notebook->AddPage(panel, title);
	}
	return(panel);

}

/****************************** SetPanelledModuleInfo *************************/

/*
 * This routine sets the special information for modules that have parameters
 * separated into different notebook pages.
 */

void
ParListInfoList::SetPanelledModuleInfo(wxPanel *panel, DatumPtr pc,
  UniParListPtr parList, int offset, int panelNum)
{
	int		numPars;
	wxPanel	*newPanel;
	ParListInfo	*infoPtr;
	NameSpecifierPtr	panelSpec1, panelSpec2;

	panelSpec1 = ( *parList->GetPanelList)(panelNum);
	if (panelSpec1->name[0] == '\0')
		return;

	if ((newPanel = UsingNotebook(parList, panelSpec1->name)) != NULL)
		panel = newPanel;
	panelSpec2 = ( *parList->GetPanelList)(panelNum + 1);
	numPars = panelSpec2->specifier - panelSpec1->specifier;
	infoPtr = new ParListInfo(panel, pc, parList, list.Number(), offset,
	  numPars);
	AddInfo(infoPtr);
	SetPanelledModuleInfo(panel, pc, parList, panelSpec2->specifier, panelNum +
	  1);

}

/****************************** SetSubParListInfo *****************************/

/*
 * This routine sets the sub-parameter list info information..
 */

void
ParListInfoList::SetSubParListInfo(ParListInfo *info)
{
	static const char *funcName = "ParListInfoList::SetSubParListInfo";
	int		i;
	UniParPtr	p;
	wxPanel		*panel = info->GetParent();
	DatumPtr	pc = info->GetPC();
	ParControl	*control;

	for (i = 0; i < info->GetNumPars(); i++) {
		control = info->GetParControl(i);
		if ((control->GetTag() == ParControl::SPECIAL) &&
		  (control->GetSpecialType() == PARLISTINFO_SUB_LIST)) {
			p = control->GetPar();
			switch (p->type) {
			case UNIPAR_CFLIST: {
				CFListPtr	theCFs = *p->valuePtr.cFPtr;
				SetStandardInfo(panel, pc, theCFs->cFParList, "CF List");
				if (theCFs->bandwidthMode.specifier !=
				  BANDWIDTH_INTERNAL_DYNAMIC)
					SetStandardInfo(panel, pc, theCFs->bParList, "Bandwidths");
				break; }
			case UNIPAR_ICLIST: {
				IonChanListPtr	theICs = *p->valuePtr.iCPtr;
				DynaListPtr	node;
				IonChannelPtr	iC;

				SetStandardInfo(panel, pc, theICs->parList, "IC List Gen.");
				for (node = theICs->ionChannels; node; node = node->next) {
					iC = (IonChannelPtr) node->data;
					SetStandardInfo(panel, pc, iC->parList, iC->description);
				}
				break; }
			case UNIPAR_MODULE: {
				char *moduleName;

				switch (pc->data->module->specifier) {
				case ANA_SAI_MODULE:
					moduleName = "Strobe";
					break;
				default:
					moduleName = "Sub-Module";
				}
				SetStandardInfo(panel, pc, p->valuePtr.module.parList,
				  moduleName);
				break; }
			case UNIPAR_PARLIST: {
				if (!*p->valuePtr.parList)
					break;
				if ((*p->valuePtr.parList)->GetPanelList)
					SetPanelledModuleInfo(panel, pc, *p->valuePtr.parList);
				else
					SetStandardInfo(panel, pc, *p->valuePtr.parList, p->desc);
				break; }
			case UNIPAR_PARARRAY: {
				ParArrayPtr	parArray = *p->valuePtr.pAPtr;
				SetStandardInfo(panel, pc, parArray->parList, parArray->name);
				break; }
			case UNIPAR_SIMSCRIPT: {
				DatumPtr	pc = *p->valuePtr.simScript.simulation;

				info->SetModuleListBox();
				for (; pc != NULL; pc = pc->next)
					if (pc->type == PROCESS) {
						parent->AddDialogInfo(new DialogInfo((wxObject *) pc,
						  (int) UNIPAR_SIMSCRIPT));
					}
				
				break; }
			default:
				NotifyError("%s: Unknown universal parameter type (%d).",
				  funcName, p->type);
				return;
			}
		}
	}

}

/****************************** SetStandardInfo *******************************/

/*
 * This routine sets the ParListInfo array.
 * It sets the list recursively.
 */

void
ParListInfoList::SetStandardInfo(wxPanel *panel, DatumPtr pc,
  UniParListPtr parList, const wxString& title, int offset, int numPars)
{
	/* static const char *funcName = "ParListInfoList::SetStandardInfo"; */
	wxPanel	*newPanel;
	ParListInfo	*infoPtr;

	if ((newPanel = UsingNotebook(parList, title)) != NULL)
		panel = newPanel;
	infoPtr = new ParListInfo(panel, pc, parList, list.Number(), offset,
	  numPars);
	AddInfo(infoPtr);
	parList->updateFlag = FALSE;
	SetSubParListInfo(infoPtr);

}

/****************************** GetLastControl ********************************/

/*
 * This function returns the last control so that the layout constraints can be
 * set up.
 */

wxWindow *
ParListInfoList::GetLastControl(void)
{
	if (notebook)
		return ((wxWindow *) notebook);

	ParListInfo *info = (ParListInfo *) (list.GetLast())->GetData();

	return((info)? info->GetLastControl(): NULL);

}
