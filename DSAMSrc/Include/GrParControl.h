/**********************
 *
 * File:		GrParControl.h
 * Purpose:		Parameter control structure classes.
 * Comments:	.
 * Author:		L. P. O'Mard
 * Created:		29 Oct 1999
 * Updated:		
 * Copyright:	(c) 1999, University of Essex
 *
 **********************/
 
#ifndef _GRPARCONTROL_H
#define _GRPARCONTROL_H 1

/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

#define PARCONTROL_VGAP				3
#define PARCONTROL_HGAP				3
#define PARCONTROL_BUTTON_MARGIN	3

/******************************************************************************/
/*************************** Macro Definitions ********************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

typedef enum {

	PARCONTROL_SUB_LIST

} ParControlTypeSpecifier;

/******************************************************************************/
/*************************** Class definitions ********************************/
/******************************************************************************/

/*************************** ParControl ***************************************/

class ParControl {

  public:
	enum	Tag { UNSET, SPECIAL, CHECK_BOX, TEXT_CTRL, CHOICE, COMBO_BOX,
				  LIST_BOX } tag;

  private:
	bool	updateFlag;
	int		infoNum;
	UniParPtr	par;
	wxSizer	*sizer;
	union {

		wxCheckBox	*checkBox;
		wxChoice	*choice;
		wxComboBox	*comboBox;
		wxListBox	*listBox;
		wxTextCtrl	*textCtrl;

		ParControlTypeSpecifier	type;

	};
	wxButton		*button;
	wxSlider		*slider;
	wxStaticText	*label;

	void	Check(Tag t)	{ if (tag != t) wxMessageBox("GrParListInfo: "
							  "Illegal ParControl"); }

  public:

	ParControl(UniParPtr thePar, int theInfoNum, wxSizer *theSizer,
	  wxCheckBox *cB);
	ParControl(UniParPtr thePar, int theInfoNum, wxSizer *theSizer,
	  wxTextCtrl *tC, wxStaticText  *labelTC);
	ParControl(UniParPtr thePar, int theInfoNum, wxSizer *theSizer, wxChoice *c,
	  wxStaticText *labelTC);
	ParControl(UniParPtr thePar, int theInfoNum, wxSizer *theSizer,
	  wxComboBox *cB, wxStaticText *labelTC);
	ParControl(UniParPtr thePar, int theInfoNum, wxSizer *theSizer,
	  wxListBox *lB, wxStaticText *labelTC);
	ParControl(UniParPtr thePar, int theInfoNum, wxSizer *theSizer,
	  ParControlTypeSpecifier t);

	void	Init(Tag theTag, int theInfoNum, wxSizer *theSizer,
			  UniParPtr thePar);

	wxButton *		GetButton(void)		{ return button; }
	wxCheckBox *	GetCheckBox(void)	{ Check(CHECK_BOX); return checkBox; }
	wxChoice *		GetChoice(void)		{ Check(CHOICE); return choice; }
	wxComboBox *	GetComboBox(void)	{ Check(COMBO_BOX); return comboBox; }
	wxControl *		GetControl(void);
	int				GetInfoNum(void)	{ return infoNum; }
	wxListBox *		GetListBox(void)	{ Check(LIST_BOX); return listBox; }
	UniParPtr		GetPar(void)		{ return par; }
	wxSize			GetSize(void) const;
	wxSizer *		GetSizer(void)		{ return sizer; }
	wxSlider *		GetSlider(void)		{ return slider; }
	ParControlTypeSpecifier	GetSpecialType(void)
										{ Check(SPECIAL); return type; }
	Tag				GetTag(void)		{ return tag; }
	wxTextCtrl *	GetTextCtrl(void)	{ Check(TEXT_CTRL); return textCtrl; }
	bool			GetUpdateFlag(void)	{ return updateFlag; }

	void	PostInit(void);
	void	ResetValue(void);
	void	SetEnable(void);
	void	SetButton(wxButton *theButton);
	void	SetSlider(wxSlider *theSlider);
	void	SetUpdateFlag(bool state)			{ updateFlag = state; }

};
		
/*************************** ParControlHandle *********************************/

class ParControlHandle {

	ParControl	*parControl;

  public:

	ParControlHandle(ParControl *theParControl)	{ parControl = theParControl; }

	ParControl *	GetPtr(void)	{ return parControl; }

};

/******************************************************************************/
/*************************** External Variables *******************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Subroutine declarations **************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Call back prototypes *****************************/
/******************************************************************************/

#endif

