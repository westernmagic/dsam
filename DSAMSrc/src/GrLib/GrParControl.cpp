/**********************
 *
 * File:		GrParControl.cpp
 * Purpose:		Parameter control structure classes.
 * Comments:	The client data set should be free'd by the "wxControl"
 *				destructors (I think).  There was a problem with the client
 *				data being de-allocated somewhere.
 * Author:		L. P. O'Mard
 * Created:		29 Oct 1999
 * Updated:		
 * Copyright:	(c) 1999, University of Essex
 *
 **********************/

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG_H */

#include <stdio.h>

#if defined(GRAPHICS_SUPPORT) && defined(__cplusplus)
// For compilers that support precompilation, includes "wx.h".
#	include <wx/wxprec.h>

#	ifdef __BORLANDC__
	    #pragma hdrstop
#	endif

// Any files you want to include if not precompiling by including
// the whole of <wx/wx.h>
#	ifndef WX_PRECOMP
#		include <wx/checkbox.h>
#		include <wx/combobox.h>
#		include <wx/textctrl.h>
#		include <wx/button.h>
#		include <wx/choice.h>
#		include <wx/listbox.h>
#		include <wx/slider.h>
#		include <wx/stattext.h>
#		include <wx/msgdlg.h>
#	endif

// Any files included regardless of precompiled headers
#endif

#include "GeCommon.h"
#include "GeUniParMgr.h"

#include "GrParControl.h"

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
/****************************** ParControl Methods ****************************/
/******************************************************************************/

/****************************** Constructors **********************************/

ParControl::ParControl(UniParPtr thePar, int theInfoNum, wxCheckBox *cB)
{
	Init(CHECK_BOX, theInfoNum, thePar);
	checkBox = cB;
	PostInit();
}

ParControl::ParControl(UniParPtr thePar, int theInfoNum, wxTextCtrl *tC,
  wxStaticText  *labelTC)
{
	Init(TEXT_CTRL, theInfoNum, thePar);
	textCtrl = tC;
	label = labelTC;
	PostInit();

}

ParControl::ParControl(UniParPtr thePar, int theInfoNum, wxChoice *c,
  wxStaticText *labelTC)
{
	Init(CHOICE, theInfoNum, thePar);
	choice = c;
	label = labelTC;
	PostInit();

}

ParControl::ParControl(UniParPtr thePar, int theInfoNum, wxComboBox *cB,
  wxStaticText *labelTC)
{
	Init(COMBO_BOX, theInfoNum, thePar);
	comboBox = cB;
	label = labelTC;
	PostInit();

}

ParControl::ParControl(UniParPtr thePar, int theInfoNum, wxListBox *lB,
  wxStaticText *labelTC)
{
	Init(LIST_BOX, theInfoNum, thePar);
	listBox = lB;
	label = labelTC;
	PostInit();

}

ParControl::ParControl(UniParPtr thePar, int theInfoNum,
  ParListInfoTypeSpecifier t)
{
	Init(SPECIAL, theInfoNum, thePar);
	type = t;

}

/****************************** Init ******************************************/

/*
 * This routine intialises the values for a control.
 */

void
ParControl::Init(Tag theTag, int theInfoNum, UniParPtr thePar)
{
	tag = theTag;
	infoNum = theInfoNum;
	par = thePar;
	updateFlag = FALSE;
	label = NULL;
	button = NULL;
	slider = NULL;
	firstSlider = FALSE;

}

/****************************** PostInit **************************************/

/*
 * This routine intialises the values for a control which need to be done after
 * the control has been initialised.
 * The client data must of a global context.  This is why I have used 'par'
 * instead of 'this'.
 */

void
ParControl::PostInit(void)
{
	switch (tag) {
	case UNSET:
	case SPECIAL:
		break;
	case CHOICE:
		choice->SetClientData(0, new ParControlHandle(this));
		break;
	case COMBO_BOX:
		comboBox->SetClientData(0, new ParControlHandle(this));
		break;
	case LIST_BOX:
		listBox->SetClientData(0, new ParControlHandle(this));
		break;
	default: {
		wxControl *control = GetControl();

		if (control)
			control->SetClientData(new ParControlHandle(this));
		break; }
	}

}

/****************************** GetControl ************************************/

/*
 * This function returns a pointer to the base wxControl for a ParControl.
 */

wxControl *
ParControl::GetControl(void)
{
	static char *funcName = "ParControl::GetControl";

	switch (tag) {
	case UNSET:
	case SPECIAL:
		return(NULL);
	case CHECK_BOX:
		return(checkBox);
	case TEXT_CTRL:
		return(textCtrl);
	case CHOICE:
		return(choice);
	case COMBO_BOX:
		return(comboBox);
	case LIST_BOX:
		return(listBox);
	default:
		NotifyError("%s: Illegal tag. (%d);\n", funcName, tag);
		return(NULL);
	}

}

/****************************** SetEnable *************************************/

/*
 * This routine enables or disable a control and its label.
 * Note that the buttons for the special file controls is controlled by the
 * selection
 */

void
ParControl::SetEnable(void)
{
	if (!par)
		return;

	wxControl *control = GetControl();

	if (control)
		control->Enable(par->enabled);

	if (label)
		label->Enable(par->enabled);

	if (button)
		switch (par->type) {
		case UNIPAR_NAME_SPEC_WITH_FPATH:
		case UNIPAR_NAME_SPEC_WITH_FILE:
			button->Enable(par->enabled && (comboBox->GetSelection() ==
			  comboBox->Number() - 1));
			break;
		default:	
			button->Enable(par->enabled);
		}
	if (!par->enabled)
		updateFlag = FALSE;

}

/****************************** SetButton *************************************/

/*
 * This routine sets the extra button for a control.
 */

void
ParControl::SetButton(wxButton *theButton)
{
	button = theButton;
	button->SetClientData(new ParControlHandle(this));

}

/****************************** SetSlider ************************************/

/*
 * This routine sets the extra Slider for a control.
 */

void
ParControl::SetSlider(wxSlider *theSlider)
{
	slider = theSlider;
	slider->SetClientData(new ParControlHandle(this));

}

/****************************** ResetValue ************************************/

/*
 * This routine resets the panel values for a control.
 * The update flag needs to be preserved for the TEXT_CTRL because the action
 * of setting the text causes 'updateFlag' to be set.
 */

void
ParControl::ResetValue(void)
{
	static const char *funcName = "ParControl::ResetValue";

	switch (tag) {
	case UNSET:
	case SPECIAL:
	case CHOICE:
	case COMBO_BOX:
	case LIST_BOX:
		break;
	case CHECK_BOX:
		checkBox->SetValue(*par->valuePtr.i);
		break;
	case TEXT_CTRL: {
		bool	oldUpdateFlag = updateFlag;
		textCtrl->SetValue(GetParString_UniParMgr(par));
		updateFlag = oldUpdateFlag;
		} break;
	default:
		NotifyError("%s: Illegal tag. (%d);\n", funcName, tag);
		return;
	}

}

/****************************** GetSize ***************************************/

/*
 * This routine returns a wxSize class for a control.
 * It returns 'wxDefaultSize' if the ParControl does not have a wxControl.
 */

wxSize
ParControl::GetSize(void) const
{
	switch (tag) {
	case CHECK_BOX:
		return(checkBox->GetSize());
	case TEXT_CTRL:
		return(textCtrl->GetSize());
	case CHOICE:
		return(choice->GetSize());
	case COMBO_BOX:
		return(comboBox->GetSize());
	case LIST_BOX:
		return(listBox->GetSize());
	default:
		return(wxDefaultSize);
	}

}