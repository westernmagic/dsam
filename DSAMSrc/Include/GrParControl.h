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

/******************************************************************************/
/*************************** Macro Definitions ********************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

typedef enum {

	PARLISTINFO_SUB_LIST

} ParListInfoTypeSpecifier;

/******************************************************************************/
/*************************** Class definitions ********************************/
/******************************************************************************/

/*************************** ParControl ***************************************/

class ParControl {

  public:
	enum	Tag { UNSET, SPECIAL, CHECK_BOX, TEXT_CTRL, CHOICE, COMBO_BOX } tag;

  private:
	bool	updateFlag, firstSlider;
	int		infoNum;
	UniParPtr	par;
	union {

		wxCheckBox	*checkBox;
		wxComboBox	*comboBox;
		wxTextCtrl	*textCtrl;
		wxChoice	*choice;

		ParListInfoTypeSpecifier	type;

	};
	wxButton		*button;
	wxSlider		*slider;
	wxStaticText	*label;

	void	Check(Tag t)	{ if (tag != t) wxMessageBox("GrParListInfo: "
							  "Illegal ParControl"); }

  public:

	ParControl(UniParPtr thePar, int theInfoNum, wxCheckBox *cB);
	ParControl(UniParPtr thePar, int theInfoNum, wxTextCtrl *tC,
	  wxStaticText  *labelTC);
	ParControl(UniParPtr thePar, int theInfoNum, wxChoice *c,
	  wxStaticText *labelTC);
	ParControl(UniParPtr thePar, int theInfoNum, wxComboBox *cB,
	  wxStaticText *labelTC);
	ParControl(UniParPtr thePar, int theInfoNum, ParListInfoTypeSpecifier t);

	void	Init(Tag theTag, int theInfoNum, UniParPtr thePar);

	bool			FirstSlider(void)	{ return firstSlider; }
	wxButton *		GetButton(void)		{ return button; }
	wxCheckBox *	GetCheckBox(void)	{ Check(CHECK_BOX); return checkBox; }
	wxChoice *		GetChoice(void)		{ Check(CHOICE); return choice; }
	wxComboBox *	GetComboBox(void)	{ Check(COMBO_BOX); return comboBox; }
	wxControl *		GetControl(void);
	int				GetInfoNum(void)	{ return infoNum; }
	UniParPtr		GetPar(void)		{ return par; }
	wxSize			GetSize(void) const;
	wxSlider *		GetSlider(void)		{ return slider; }
	ParListInfoTypeSpecifier	GetSpecialType(void)
										{ Check(SPECIAL); return type; }
	Tag				GetTag(void)		{ return tag; }
	wxTextCtrl *	GetTextCtrl(void)	{ Check(TEXT_CTRL); return textCtrl; }
	bool			GetUpdateFlag(void)	{ return updateFlag; }

	void	PostInit(void);
	void	ResetValue(void);
	void	SetEnable(void);
	void	SetButton(wxButton *theButton);
	void	SetFirstSlider(void)				{ firstSlider = TRUE; }
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

