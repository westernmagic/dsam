/**********************
 *
 * File:		GrParListInfo.h
 * Purpose:		Module parameter list handling routines.
 * Comments:	
 * Author:		L. P. O'Mard
 * Created:		23 Oct 1998
 * Updated:		
 * Copyright:	(c) 1998, University of Essex
 *
 **********************/
 
#ifndef _GRPARLISTINFO_H
#define _GRPARLISTINFO_H 1

#include "GeUniParMgr.h"
#include "UtDatum.h"

/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

#define	PARLISTINFO_SILDER_ITEM_WIDTH	300
#define	PARLISTINFO_TEXT_ITEM_WIDTH		150
#define	PARLISTINFO_IC_TEXT_ITEM_WIDTH	50
#define	PARLISTINFO_SUB_MODULE_1		1
#define	PARLISTINFO_INDEX_OFFSET		1000

#if defined(wx_motif) || defined(wx_msw)
#	define	PARLISTINFO_MAX_LABEL			40
#else
#	define	PARLISTINFO_MAX_LABEL			80
#endif

/******************************************************************************/
/*************************** Macro Definitions ********************************/
/******************************************************************************/

#define PARLISTINFO_INDEX_TO_ID(IND)	((IND) + PARLISTINFO_INDEX_OFFSET)
#define PARLISTINFO_ID_TO_INDEX(IND)	((IND) - PARLISTINFO_INDEX_OFFSET)

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

typedef enum {

	PARLISTINFO_SUB_LIST

} ParListInfoTypeSpecifier;

/******************************************************************************/
/*************************** Class definitions ********************************/
/******************************************************************************/

/*************************** ParControl.***************************************/

class ParControl {

	enum	Tag { UNSET, SPECIAL, CHECK_BOX, TEXT_CTRL, CHOICE, COMBO_BOX } tag;
	bool	updateFlag;
	union {

		wxCheckBox	*checkBox;
		wxComboBox	*comboBox;
		wxTextCtrl	*textCtrl;
		wxChoice	*choice;

		ParListInfoTypeSpecifier	type;

	};
	wxButton		*button;
	wxStaticText	*label;

	void	Check(Tag t)	{ if (tag != t) wxMessageBox("GrParListInfo: "
							  "Illegal ParControl"); }

  public:

	ParControl(void)			{ Init(); tag = UNSET; }
	ParControl(wxCheckBox *cB)	{ Init(); tag = CHECK_BOX; checkBox = cB; }
	ParControl(wxTextCtrl *tC, wxStaticText  *labelTC)
	  { Init(); tag = TEXT_CTRL; textCtrl = tC; label = labelTC; }
	ParControl(wxChoice *c, wxStaticText *labelTC)
	  { Init(); tag = CHOICE; choice = c; label = labelTC; }
	ParControl(wxComboBox *cB, wxStaticText *labelTC)
	  { Init(); tag = COMBO_BOX; comboBox = cB; label = labelTC; }
	ParControl(ParListInfoTypeSpecifier t)	{ Init(); tag = SPECIAL; type = t; }

	void	Init(void);

	void	Enable(bool state);
	wxButton *		GetButton(void)		{ return button; }
	wxCheckBox *	GetCheckBox(void)	{ Check(CHECK_BOX); return checkBox; }
	wxChoice *		GetChoice(void)		{ Check(CHOICE); return choice; }
	wxComboBox *	GetComboBox(void)	{ Check(COMBO_BOX); return comboBox; }
	ParListInfoTypeSpecifier	GetSpecialType(void)
										{ Check(SPECIAL); return type; }
	Tag				GetTag(void)		{ return tag; }
	wxTextCtrl *	GetTextCtrl(void)	{ Check(TEXT_CTRL); return textCtrl; }
	wxWindow *		GetWindow(void);

	void	SetButton(wxButton *theButton)	{ button = theButton; }
	void	SetUpdateFlag(bool state)			{ updateFlag = state; }
	bool	Special(void)	{ return (tag == SPECIAL); }
	bool	UnSet(void)		{ return (tag == UNSET); }

};
		
/*************************** ParListInfo.**************************************/

class ParListInfo {

	char		*prefix;
	int			offset, numPars;
	DatumPtr	pc;
	wxPanel		*parent;
	ParControl	*controlList;
	wxWindow	*lastControl;
	wxStringList	rothmanHeadings;

  public:
  	UniParListPtr	parList;

	ParListInfo(wxPanel *theParent, DatumPtr thePC, UniParListPtr theParList,
	  int theOffset = 0, int theNumPars = -1, char *thePrefix = "");
	~ParListInfo(void);

	void	AddModuleListBoxEntries(DatumPtr pc, wxListBox& listBox);
	bool	CheckChangedValues(void);
	int		GetOffset(void)	{ return offset; };
	wxWindow *	GetLastControl(void)	{ return lastControl; }
	int		GetNumPars(void)			{ return numPars; }
	ParControl * GetParControl(int i)	{ return &controlList[i]; }
	wxPanel *	GetParent(void)			{ return parent; }
	UniParListPtr	GetParList(void)	{ return parList; }
	DatumPtr	GetPC(void)			{ return pc; }
	void	SetICListListBox(IonChanListPtr theICs, char *prefix);
	void	SetModuleListBox(DatumPtr simulation);
	void	SetParBoolean(UniParPtr par, int index);
	void	SetParNameList(UniParPtr par, int index);
	void	SetParNameListWithText(UniParPtr par, int index);
	void	SetParStandard(UniParPtr par, int index);
	void	SetParListStandard(void);
	void	SetParListIonChannel(void);
	bool	SetParValue(int parNum, char *value);

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

