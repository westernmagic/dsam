/**********************
 *
 * File:		GrParListInfo.cpp
 * Purpose:		Module parameter list handling routines.
 * Author:		L. P. O'Mard
 * Created:		23 Oct 1998
 * Updated:		13 Jan 1999
 * Copyright:	(c) 1999, University of Essex
 *
 **********************/

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG_H */

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
/****************************** ModuleParDialog Methods ***********************/
/******************************************************************************/

/****************************** Constructor ***********************************/

ParListInfo::ParListInfo(wxPanel *theParent, DatumPtr thePC,
  UniParListPtr theParList, int theInfoNum, int theoffset, int theNumPars)
{
	static char *funcName = "ParListInfo::ParListInfo";

	parent = theParent;
	pc = thePC;
	parList = theParList;
	infoNum = theInfoNum;
	offset = theoffset;
	maxWidth = maxHeight = -1;
	numPars = (theNumPars < 0)? parList->numPars: theNumPars;
	lastControl = NULL;

	// Set Up parameters list.
	if ((controlList = (ParControl **) calloc(numPars, sizeof(ParControl *))) ==
	  NULL) {
		NotifyError("%s: Out of memory for controlList", funcName);
		return;
	}
	switch (parList->mode) {
	case UNIPAR_SET_IC:
		SetParListIonChannel();
		break;
	default:
		SetParListStandard();
	}

}

/****************************** Destructor ************************************/

/*
 * Note that the individual 'MyText' items will be deleted when the dialog panel
 * is deleted.
 */

ParListInfo::~ParListInfo(void)
{
	int		i;

	if (controlList) {
		for (i = 0; i < numPars; i++)
			if (controlList[i])
				delete controlList[i];
			else
				wxMessageBox("ParListInfo::~ParListInfo unset control!");
		free(controlList);
	}

}

/****************************** SetParBoolean *********************************/

/*
 * This routine sets the boolean parameter check box.
 */

void
ParListInfo::SetParBoolean(UniParPtr par, int index)
{
	wxLayoutConstraints	*c;

	wxCheckBox *checkBox = new wxCheckBox(parent, DL_ID_CHECK_BOX + index,
	  par->abbr);
	checkBox->SetValue(CXX_BOOL(*par->valuePtr.i));
	checkBox->SetToolTip(par->desc);
	controlList[index] = new ParControl(par, infoNum, checkBox);

	c = new wxLayoutConstraints;
	c->left.SameAs(parent, wxLeft, 4);
	if (!lastControl)
		c->top.SameAs(parent, wxTop, PARLISTINFO_DEFAULT_Y_MARGIN);
	else
		c->top.SameAs(lastControl, wxBottom, PARLISTINFO_DEFAULT_Y_MARGIN);
	c->width.AsIs();
	c->height.AsIs();
	checkBox->SetConstraints(c);

	lastControl = checkBox;

}

/****************************** SetParNameList ********************************/

/*
 * This routine sets the boolean parameter check box.
 */

void
ParListInfo::SetParNameList(UniParPtr par, int index)
{
	wxLayoutConstraints	*c;
	NameSpecifierPtr list; 

	wxChoice *choice = new wxChoice(parent, DL_ID_CHOICE + index,
	  wxDefaultPosition, wxDefaultSize, 0, NULL);
	for (list = par->valuePtr.nameList.list; list->name[0] != '\0'; list++)
		choice->Append(list->name);
	choice->SetSelection(*par->valuePtr.nameList.specifier);
	choice->SetToolTip(par->desc);

	wxStaticText *labelText = new wxStaticText(parent, -1, par->abbr);

	controlList[index] = new ParControl(par, infoNum, choice, labelText);

	c = new wxLayoutConstraints;
	c->left.SameAs(parent, wxLeft, 4);
	if (!lastControl)
		c->top.SameAs(parent, wxTop, PARLISTINFO_DEFAULT_Y_MARGIN);
	else
		c->top.SameAs(lastControl, wxBottom, PARLISTINFO_DEFAULT_Y_MARGIN);
	c->width.PercentOf(parent, wxWidth, 45);
	c->height.AsIs();
	choice->SetConstraints(c);

	c = new wxLayoutConstraints;
	c->left.RightOf(choice, 4);
	c->centreY.SameAs(choice, wxCentreY);
	c->width.AsIs();
	c->height.AsIs();
	labelText->SetConstraints(c);

	lastControl = labelText;
}

/****************************** SetParNameListWithText ************************/

/*
 * This routine sets the combo box with a button for browsing the file or
 * path.
 * The button text must have its own memory space, or when the dialog is
 * closed it can cause a crash.
 */

void
ParListInfo::SetParNameListWithText(UniParPtr par, int index)
{
	wxLayoutConstraints	*c;
	NameSpecifierPtr list; 

	wxComboBox *cBox = new wxComboBox(parent, DL_ID_COMBO_BOX + index, "",
	  wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY);
	for (list = par->valuePtr.nameList.list; list->name[0] != '\0'; list++)
		cBox->Append(list->name);
	cBox->SetSelection(*par->valuePtr.nameList.specifier);
	cBox->SetInsertionPointEnd();
	cBox->SetToolTip(par->desc);

	wxButton *browseBtn = new wxButton(parent, DL_ID_BUTTON + index,
	  PARLISTINFO_BROWSE_BUTTON_TEXT, wxDefaultPosition, wxSize(
	  PARLISTINFO_BROWSE_BUTTON_WIDTH, -1));
	browseBtn->SetToolTip("Click this button to browse for a file (path).");
	browseBtn->Enable(cBox->GetSelection() == (cBox->Number() - 1));

	wxStaticText *labelText = new wxStaticText(parent, index, par->abbr);

	controlList[index] = new ParControl(par, infoNum, cBox, labelText);
	controlList[index]->SetButton(browseBtn);

	c = new wxLayoutConstraints;
	c->left.SameAs(parent, wxLeft, 4);
	if (!lastControl)
		c->top.SameAs(parent, wxTop, PARLISTINFO_DEFAULT_Y_MARGIN);
	else
		c->top.SameAs(lastControl, wxBottom, PARLISTINFO_DEFAULT_Y_MARGIN);
	c->width.PercentOf(parent, wxWidth, 40);
	c->height.AsIs();
	cBox->SetConstraints(c);

	c = new wxLayoutConstraints;
	c->left.RightOf(cBox, 4);
	c->centreY.SameAs(cBox, wxCentreY);
	c->width.AsIs();
	c->height.AsIs();
	browseBtn->SetConstraints(c);

	c = new wxLayoutConstraints;
	c->left.RightOf(browseBtn, 4);
	c->centreY.SameAs(browseBtn, wxCentreY);
	c->width.AsIs();
	c->height.AsIs();
	labelText->SetConstraints(c);

	lastControl = labelText;
}

/****************************** SetParFileName ********************************/

/*
 * This routine sets the text box for the file name with a browse button.
 * and description for the parameter.
 * The button text must have its own memory space, or when the dialog is
 * closed it can cause a crash.
 */

void
ParListInfo::SetParFileName(UniParPtr par, int index)
{
	wxLayoutConstraints	*c;

	wxTextCtrl	*textCtrl = new wxTextCtrl(parent, DL_ID_TEXT,
	  GetParString_UniParMgr(par), wxDefaultPosition, wxSize(
	  PARLISTINFO_TEXT_ITEM_WIDTH, -1), wxHSCROLL);
	textCtrl->SetToolTip(par->desc);
	textCtrl->SetInsertionPointEnd();

	wxButton *browseBtn = new wxButton(parent, DL_ID_BUTTON + index,
	  PARLISTINFO_BROWSE_BUTTON_TEXT, wxDefaultPosition, wxSize(
	  PARLISTINFO_BROWSE_BUTTON_WIDTH, -1));
	browseBtn->SetToolTip("Click this button to browse for a file.");

	wxStaticText *labelText = new wxStaticText(parent, index, par->abbr);

	controlList[index] = new ParControl(par, infoNum, textCtrl, labelText);
	controlList[index]->SetButton(browseBtn);

	c = new wxLayoutConstraints;
	c->left.SameAs(parent, wxLeft, 4);
	if (!lastControl)
		c->top.SameAs(parent, wxTop, PARLISTINFO_DEFAULT_Y_MARGIN);
	else
		c->top.SameAs(lastControl, wxBottom, PARLISTINFO_DEFAULT_Y_MARGIN);
	c->width.AsIs();
	c->height.AsIs();
	textCtrl->SetConstraints(c);

	c = new wxLayoutConstraints;
	c->left.RightOf(textCtrl, 4);
	c->centreY.SameAs(textCtrl, wxCentreY);
	c->width.AsIs();
	c->height.AsIs();
	browseBtn->SetConstraints(c);

	c = new wxLayoutConstraints;
	c->left.RightOf(browseBtn, 4);
	c->centreY.SameAs(browseBtn, wxCentreY);
	c->width.AsIs();
	c->height.AsIs();
	labelText->SetConstraints(c);

	lastControl = labelText;

}

/****************************** SetParStandard ********************************/

/*
 * This routine sets the standard parameter text box and description for a
 * parameter.
 */

void
ParListInfo::SetParStandard(UniParPtr par, int index)
{
	wxLayoutConstraints	*c;

	wxTextCtrl	*textCtrl = new wxTextCtrl(parent, DL_ID_TEXT,
	  GetParString_UniParMgr(par), wxDefaultPosition, wxSize(
	  PARLISTINFO_TEXT_ITEM_WIDTH, -1));
	textCtrl->SetToolTip(par->desc);
	textCtrl->SetInsertionPointEnd();

	wxStaticText *labelText = new wxStaticText(parent, index, par->abbr);

	controlList[index] = new ParControl(par, infoNum, textCtrl, labelText);

	if ((par->type == UNIPAR_INT_ARRAY) || (par->type == UNIPAR_REAL_ARRAY) ||
	  (par->type == UNIPAR_STRING_ARRAY) || (par->type ==
	  UNIPAR_NAME_SPEC_ARRAY))
		SetSlider(index);

	if (controlList[index]->FirstSlider()) {
		c = new wxLayoutConstraints;
		c->left.SameAs(parent, wxLeft, 4);
		if (!lastControl)
			c->top.SameAs(parent, wxTop, PARLISTINFO_DEFAULT_Y_MARGIN);
		else
			c->top.SameAs(lastControl, wxBottom, PARLISTINFO_DEFAULT_Y_MARGIN);
		c->width.AsIs();
		c->height.AsIs();
		controlList[index]->GetSlider()->SetConstraints(c);
		lastControl = controlList[index]->GetSlider();
	}
	c = new wxLayoutConstraints;
	c->left.SameAs(parent, wxLeft, 4);
	if (!lastControl)
		c->top.SameAs(parent, wxTop, 4);
	else
		c->top.SameAs(lastControl, wxBottom, 4);
	c->width.AsIs();
	c->height.AsIs();
	textCtrl->SetConstraints(c);

	c = new wxLayoutConstraints;
	c->left.RightOf(textCtrl, 4);
	c->centreY.SameAs(textCtrl, wxCentreY);
	c->width.AsIs();
	c->height.AsIs();
	labelText->SetConstraints(c);

	lastControl = labelText;

}

/****************************** SetParGeneral *********************************/

/*
 * This routine sets the general parameter format.  It was separated from the
 * SetParListStandard routine so that other custon routines could use it too.
 */

void
ParListInfo::SetParGeneral(UniParPtr par, int index)
{
	switch (par->type) {
	case UNIPAR_CFLIST:
	case UNIPAR_ICLIST:
	case UNIPAR_SIMSCRIPT:
	case UNIPAR_MODULE:
	case UNIPAR_PARLIST:
	case UNIPAR_PARARRAY:
		controlList[index] = new ParControl(par, infoNum, PARLISTINFO_SUB_LIST);
		break;
	case UNIPAR_BOOL:
		SetParBoolean(par, index);
		break;
	case UNIPAR_NAME_SPEC:
		SetParNameList(par, index);
		break;
	case UNIPAR_NAME_SPEC_WITH_FILE:
	case UNIPAR_NAME_SPEC_WITH_FPATH:
		SetParNameListWithText(par, index);
		break;
	case UNIPAR_FILE_NAME:
		SetParFileName(par, index);
		break;
	default:
		SetParStandard(par, index);
	}
	controlList[index]->SetEnable();

}

/****************************** SetParListStandard ****************************/

/*
 * This routine sets the standard parameter list format.
 */

void
ParListInfo::SetParListStandard(void)
{
	int		i;

	for (i = 0; i < numPars; i++)
		SetParGeneral(&parList->pars[i + offset], i);

}

/****************************** SetParListIonChannel **************************/

/*
 * This routine sets the ion channel parameter list.
 * The 'ALPHA' and 'BETA' controls must be done in separate loops so that the
 * "SetSlider" routine can be used.
 */

void
ParListInfo::SetParListIonChannel(void)
{
	const int	numHHCols = ICLIST_IC_ALPHA_K - ICLIST_IC_ALPHA_A + 1;
	int		i, index;
	UniParPtr	par;
	wxString	heading;
	wxTextCtrl	*hHuxleyAlphaTC[numHHCols], *hHuxleyBetaTC[numHHCols];
	wxStaticText *hHuxleyLabel[numHHCols];
	wxLayoutConstraints	*c;

	maxHeight = 0;
	for (i = 0; i <= ICLIST_IC_FILE_NAME; i++) {
		SetParGeneral(&parList->pars[i], i);
		maxHeight += GetParControl(i)->GetSize().GetHeight() +
		  PARLISTINFO_DEFAULT_Y_MARGIN;
	}
	
	for (i = 0; i < numHHCols; i++) {
		heading.sprintf("    %c", 'A' + i);
		hHuxleyLabel[i] = new wxStaticText(parent, -1, heading,
		  wxDefaultPosition, wxSize(PARLISTINFO_IC_TEXT_ITEM_WIDTH, -1));

		index = i + ICLIST_IC_ALPHA_A;
		par = &parList->pars[index];
		hHuxleyAlphaTC[i] = new wxTextCtrl(parent, DL_ID_TEXT,
		  GetParString_UniParMgr(par), wxDefaultPosition, wxSize(
		  PARLISTINFO_IC_TEXT_ITEM_WIDTH, -1));
		hHuxleyAlphaTC[i]->SetToolTip(par->desc);
		controlList[index] = new ParControl(par, infoNum, hHuxleyAlphaTC[i],
		  hHuxleyLabel[i]);
		controlList[index]->SetEnable();
		SetSlider(index);
	}
	for (i = 0; i < numHHCols; i++) {
		index = i + ICLIST_IC_BETA_A;
		par = &parList->pars[index];
		hHuxleyBetaTC[i] = new wxTextCtrl(parent, DL_ID_TEXT,
		  GetParString_UniParMgr(par), wxDefaultPosition, wxSize(
		  PARLISTINFO_IC_TEXT_ITEM_WIDTH, -1));
		hHuxleyBetaTC[i]->SetToolTip(par->desc);
		controlList[index] = new ParControl(par, infoNum, hHuxleyBetaTC[i],
		  hHuxleyLabel[i]);
		SetSlider(index);
		controlList[index]->SetEnable();

	}
	int  width, height;
	GetParControl(ICLIST_IC_ALPHA_A)->GetSlider()->GetSize(&width, &height);
	maxHeight += height + PARLISTINFO_DEFAULT_Y_MARGIN + 3 * 
	  (GetParControl(ICLIST_IC_ALPHA_A)->GetSize().GetHeight() +
	  PARLISTINFO_DEFAULT_Y_MARGIN);
	maxWidth = numHHCols * (GetParControl(ICLIST_IC_ALPHA_A)->GetSize(
	  ).GetWidth() + PARLISTINFO_DEFAULT_X_MARGIN) + 2 *
	  PARLISTINFO_DEFAULT_X_MARGIN;
	for (i = 0; i < numHHCols; i++) {
		c = new wxLayoutConstraints;
		if (i == 0)
			c->left.SameAs(parent, wxLeft, 4);
		else
			c->left.RightOf(hHuxleyLabel[i - 1], 4);
		if (!lastControl)
			c->top.SameAs(parent, wxTop, 4);
		else
			c->top.SameAs(lastControl, wxBottom, 4);
		c->width.AsIs();
		c->height.AsIs();
		hHuxleyLabel[i]->SetConstraints(c);

		c = new wxLayoutConstraints;
		c->left.SameAs(hHuxleyLabel[i], wxLeft);
		c->top.Below(hHuxleyLabel[i], 4);
		c->width.AsIs();
		c->height.AsIs();
		hHuxleyAlphaTC[i]->SetConstraints(c);

		c = new wxLayoutConstraints;
		c->left.SameAs(hHuxleyAlphaTC[i], wxLeft);
		c->top.Below(hHuxleyAlphaTC[i], 4);
		c->width.AsIs();
		c->height.AsIs();
		hHuxleyBetaTC[i]->SetConstraints(c);
	}
	lastControl = hHuxleyBetaTC[numHHCols - 1];
	
}

/****************************** SetSlider *************************************/

/*
 * This routine sets the slider for a particular parameter array index.
 * It assumes that any previous sliders having the same maximum value, will
 * use the same slider.
 * It returns "TRUE" if a new slider is set.
 */

void
ParListInfo::SetSlider(int index)
{
	int		i, minElement, maxElement;

//	Start a wxSaticBox here too?
	for (i = index - 1; i >= 0; i--)
		if (controlList[i]->GetSlider() && (controlList[i]->GetPar(
		  )->valuePtr.array.numElements == controlList[index]->GetPar(
		  )->valuePtr.array.numElements)) {
			controlList[index]->SetSlider(controlList[i]->GetSlider());
			return;
		}
#	ifdef __WXMSW__
		minElement = 0;
		maxElement = *(controlList[index]->GetPar()->valuePtr.array.
		  numElements) - 1;
#	else
		minElement = 1;
		maxElement = *(controlList[index]->GetPar()->valuePtr.array.
		  numElements);
#	endif
	wxSlider *slider = new wxSlider(parent, DL_ID_SLIDER + index, minElement,
	  minElement, maxElement, wxDefaultPosition, wxSize(
	  PARLISTINFO_SILDER_ITEM_WIDTH, -1), wxSL_LABELS | wxSL_HORIZONTAL);

	controlList[index]->SetSlider(slider);
	controlList[index]->SetFirstSlider();

}

/****************************** SetEnabledControls ****************************/

/*
 * This routine enables or disables the controls for this info structure.
 * It is called when the enabled controls can have been changed.
 */

void
ParListInfo::SetEnabledControls(void)
{
	int		i;

	for (i = 0; i < numPars; i++)
		if (controlList[i])
			controlList[i]->SetEnable();
		else
			wxMessageBox("ParListInfo::SetEnabledControls: Unset control!");

}

/****************************** SetParValue ***********************************/

/*
 * This routine sets a parameter value.
 */

bool
ParListInfo::SetParValue(ParControl *control)
{
	wxString	value;

	switch (control->GetTag()) {
	case ParControl::CHECK_BOX:
		value = (control->GetCheckBox()->GetValue())? BooleanList_NSpecLists(
		  GENERAL_BOOLEAN_ON)->name: BooleanList_NSpecLists(
		  GENERAL_BOOLEAN_OFF)->name;
		break;
	case ParControl::CHOICE:
		value = control->GetChoice()->GetStringSelection();
		break;
	case ParControl::COMBO_BOX:
		value = control->GetComboBox()->GetStringSelection();
		break;
	case ParControl::TEXT_CTRL:
		value = control->GetTextCtrl()->GetValue();
		break;
	default:
		value = "Value not yet being set!";
	}
	if (pc)
		(* pc->data->module->SetParsPointer)(pc->data->module);
	if (SetParValue_UniParMgr(&parList, control->GetPar()->index, (char *)
	  value.c_str())) {
		control->SetUpdateFlag(FALSE);
		return(TRUE);
	}
	return(FALSE);

}

/****************************** CheckChangedValues ****************************/

/*
 * This routine checks the changed values and returns false if any of them
 * cannot be set.
 * If the parameter list has been updated, then it is assumed that all the
 * the parameters must be checked.
 */

bool
ParListInfo::CheckChangedValues(void)
{
	bool	ok = TRUE;
	int		i;

	for (i = 0; i < numPars; i++)
		if (controlList[i]->GetUpdateFlag()) {
			if (!SetParValue(controlList[i]))
				ok = FALSE;
		}
	return(ok);

}

/****************************** AddModuleListBoxEntries ***********************/

/*
 * This routine adds the module list box entries.
 * It puts a 'x' infront of any module which is disabled.
 */

void
ParListInfo::AddModuleListBoxEntries(wxListBox *listBox)
{
	DatumPtr	p;

	for (p = pc; p != NULL; p = p->next)
		if (p->type == PROCESS) {
			wxString moduleEntry;
			if (p->data->module->onFlag)
				moduleEntry.sprintf("  %s", NameAndLabel_Utility_Datum(p));
			else
				moduleEntry.sprintf("%c %s", SIMSCRIPT_DISABLED_MODULE_CHAR,
				  NameAndLabel_Utility_Datum(p));
			listBox->Append(moduleEntry.GetData());
		}

}

/****************************** GetParControl *********************************/

/*
 * This function returns a parameter control.
 */

ParControl *
ParListInfo::GetParControl(int i)
{
	static const char *funcName = "ParListInfo::GetParControl";
	if (i < numPars)
		return(controlList[i]);
	NotifyError("%s: Control index %d is out of bound.", funcName, i);
	return(NULL);

}

/****************************** ResetControlValues ****************************/

/*
 * This routine resets all of the controls to their original values.
 */

void
ParListInfo::ResetControlValues(void)
{
	int		i;

	for (i = 0; i < numPars; i++)
		controlList[i]->ResetValue();

}

/****************************** UpdateControlValues ***************************/

/*
 * This routine updates all of the controls to their original values, unless
 * they have been set.
 */

void
ParListInfo::UpdateControlValues(void)
{
	int		i;

	for (i = 0; i < numPars; i++)
		if (!controlList[i]->GetUpdateFlag())
			controlList[i]->ResetValue();

}

/****************************** CheckInterDependence **************************/

/*
 * This routine checks the interdependence of parameters.  If a dependent
 * parameter is changed, then the parList->updateFlag will be set.
 */

void
ParListInfo::CheckInterDependence(void)
{
	if (parList->updateFlag) {
		SetEnabledControls();
		UpdateControlValues();
	}
}
