/**********************
 *
 * File:		GrModParDialog.cpp
 * Purpose:		Module parameter handling routines.
 *				This is a dialog for changing the parameters of a module using
 *				the module's parameter list.
 * Comments:	04-10-98 LPO: This module has been set up so that before any
 *				attempt to run the simulation, the parameters in the dialogs
 *				are always checked to see if they have been changed (without
 *				pressing <return>).
 * 				23-10-98 LPO: This Module has been taken from the GrSimMgr
 *				module which used to contain both this and the SimModuleDialog
 *				29-01-99 LPO: The 	ResetGUIDialogs() is used by the
 *				'TextProc', 'ButtonProc' and 'ListProc' routines, to ensure
 *				that error messages are given as dialogs.
 * Author:		L. P. O'Mard
 * Created:		01 Oct 1998
 * Updated:		29 Jan 1999
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

/******************************************************************************/
/*************************** Event tables *************************************/
/******************************************************************************/

BEGIN_EVENT_TABLE(ModuleParDialog, DialogList)
	EVT_BUTTON(wxID_OK, 		ModuleParDialog::OnOk)
//	EVT_BUTTON(DL_ID_ADD_IC,	ModuleParDialog::OnICButton)
//	EVT_BUTTON(DL_ID_DELETE_IC,	ModuleParDialog::OnICButton)
	EVT_NOTEBOOK_PAGE_CHANGED(PARLISTINFOLIST_ID_NOTEBOOK,
	  ModuleParDialog::OnPageChanged)
END_EVENT_TABLE()

/****************************** Constructor ***********************************/

/*
 * I had to use a wxMessage rather than the label of the MyText to get the
 * description to the left of the value, and correctly lined up.
 * Negative tags are being used for the MyText items to avoid conflict with
 * other events.
 */

ModuleParDialog::ModuleParDialog(wxWindow *parent, const wxString& title,
  int theInfoNum, DatumPtr pc, UniParListPtr parList, int x, int y, int width,
  int height, long style): DialogList(parent, title, theInfoNum, pc, parList,
  wxPoint(x, y), wxSize(width, height), style)
{
	/* static char *funcName = "ModuleParDialog::ModuleParDialog"; */
	wxLayoutConstraints	*c;

	// Set up buttons
	okBtn = new wxButton(this,  wxID_OK, "Ok");
	cancelBtn = new wxButton(this, wxID_CANCEL, "Cancel");

	c = new wxLayoutConstraints;
	c->centreX.PercentOf(this, wxWidth, 40);
	if (lastControl)
		c->top.Below(lastControl, 12);
	else
		c->bottom.SameAs(this, wxBottom, 4);
	c->width.AsIs();
	c->height.AsIs();
	okBtn->SetConstraints(c);

	c = new wxLayoutConstraints;
	c->left.RightOf(okBtn, 4);
	c->bottom.SameAs(okBtn, wxBottom);
	c->width.AsIs();
	c->height.AsIs();
	cancelBtn->SetConstraints(c);

	if (parListInfoList->GetUseNotebookControls()) {
		addICBtn = new wxButton(this, DL_ID_ADD_IC, "Add IC");
		addICBtn->SetToolTip("Press button to add an ion channel.");

		deleteICBtn = new wxButton(this, DL_ID_DELETE_IC, "Delete IC");
		deleteICBtn->SetToolTip("Press button to delete an ion channel.");
		deleteICBtn->Enable(FALSE);

		c = new wxLayoutConstraints;
		c->left.RightOf(cancelBtn, 4);
		c->bottom.SameAs(cancelBtn, wxBottom);
		c->width.AsIs();
		c->height.AsIs();
		addICBtn->SetConstraints(c);

		c = new wxLayoutConstraints;
		c->left.RightOf(addICBtn, 4);
		c->bottom.SameAs(addICBtn, wxBottom);
		c->width.AsIs();
		c->height.AsIs();
		deleteICBtn->SetConstraints(c);

	} else {
		addICBtn = NULL;
		deleteICBtn = NULL;
	}

	SetAutoLayout(TRUE);
	Layout();
	Fit();

}

/****************************** Destructor ************************************/

ModuleParDialog::~ModuleParDialog(void)
{
}

/****************************** DeleteDialog **********************************/

void
ModuleParDialog::DeleteDialog(void)
{
	DialogList *parent = (DialogList *) GetParent();

	parent->UnsetDialog(infoNum);
	if (pc && pc->data->module->specifier == DISPLAY_MODULE)
		((SignalDispPtr) pc->data->module->parsPtr)->dialog = NULL;

}

/******************************************************************************/
/****************************** Call backs ************************************/
/******************************************************************************/

/****************************** OnICButton ************************************/

/*
 * This routine operates on the panels of a note book for the Ion channel
 * operations.
 * It assumes that the ParListInfo of the main ICList page will give access
 * to the ICList structure.
 */

void 
ModuleParDialog::OnICButton(wxCommandEvent& event)
{
	static const char *funcName = "ModuleParDialog::OnICButton";
	ParListInfo	*info = parListInfoList->GetInfo(
				  PARLISTINFOLIST_MAIN_ICLIST_PAGES - 1);
	IonChanListPtr	theICs = info->parList->handlePtr.iCs;

	ResetGUIDialogs();
	NotifyError("%s: Not yet Implemented.", funcName);
	return;
	switch (event.GetId()) {
	case DL_ID_ADD_IC: {
		IonChannelPtr	iC;
		printf("Add IC\n");
		if ((iC = InitIonChannel_IonChanList(funcName,
		  theICs->numTableEntries)) == NULL) {
			NotifyError("%s: Could not initialise ion channel.", funcName);
			return;
		}
		SetICGeneralPars_IonChanList(iC, theICs,ICLIST_BOLTZMANN_MODE,
		  "<description>", "on", 0.0, 0.0, 1);
		SetIonChannelUniParList_IonChanList(theICs, iC);
		SetIonChannelUniParListMode_IonChanList(iC);
		if (!Append_Utility_DynaList(&theICs->ionChannels, iC)) {
			NotifyError("%s: Could not add ion channel to list.", funcName);
			FreeIonChannel_IonChanList(&iC);
			return;
		}
		parListInfoList->SetStandardInfo(NULL, info->GetPC(), iC->parList,
		  iC->description);
		break; }
	case DL_ID_DELETE_IC:
		printf("Delete IC\n");
		break;
	default:
		NotifyError("%s: Unknown button ID (%d).", funcName, event.GetId());
		return;
	}

}

/****************************** OnOk ******************************************/

/*
 * This routine updates the module's parameters and closes the dialog.
 */

void 
ModuleParDialog::OnOk(wxCommandEvent& WXUNUSED(event))
{
	ResetGUIDialogs();
	if (!CheckChangedValues())
		return;
	if (IsModal())
		EndModal(wxID_OK);
	else
		Close();

}

/****************************** OnPageChanged *********************************/

void 
ModuleParDialog::OnPageChanged(wxNotebookEvent &event)
{
	int	selection = event.GetSelection();

	SetNotebookPanel(selection);
	if (deleteICBtn) {
		int	index = selection - PARLISTINFOLIST_MAIN_ICLIST_PAGES;

		if ((index < 0)) {
			deleteICBtn->Enable(FALSE);
			return;
		}
		deleteICBtn->Enable(TRUE);
		ParListInfo	*info = parListInfoList->GetInfo(index +
		  PARLISTINFOLIST_MAIN_ICLIST_PAGES);
		info->parList->handlePtr.iCs->index = index;
	}

}

