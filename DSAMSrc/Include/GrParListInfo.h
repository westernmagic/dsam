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

#define	PARLISTINFO_ID_NOTEBOOK			2000

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

/******************************************************************************/
/*************************** Class definitions ********************************/
/******************************************************************************/

/*************************** ParControl.***************************************/

class ParControl {

	enum	Tag { CHECK_BOX, TEXT_CTRL, CHOICE, COMBO_BOX } tag;
	bool	updateFlag;
	union {

		wxCheckBox	*checkBox;
		wxComboBox	*comboBox;
		wxTextCtrl	*textCtrl;
		wxChoice	*choice;

	};
	wxButton	*button;

	void	Check(Tag t)	{ if (tag != t) wxMessageBox("GrParListInfo: "
							  "Illegal ParControl"); }

  public:

	ParControl(wxCheckBox *cB)	{ Init(); tag = CHECK_BOX; checkBox = cB; }
	ParControl(wxTextCtrl *tC)	{ Init(); tag = TEXT_CTRL; textCtrl = tC; }
	ParControl(wxChoice *c)		{ Init(); tag = CHOICE; choice = c; }
	ParControl(wxComboBox *cB)	{ Init(); tag = COMBO_BOX; comboBox = cB; }

	void	Init(void)	{ updateFlag = FALSE; }

	wxButton *		GetButton(void)		{ return button; }
	wxCheckBox *	GetCheckBox(void)	{ Check(CHECK_BOX); return checkBox; }
	wxChoice *		GetChoice(void)		{ Check(CHOICE); return choice; }
	wxComboBox *	GetComboBox(void)	{ Check(COMBO_BOX); return comboBox; }
	Tag				GetTag(void)		{ return tag; }
	wxTextCtrl *	GetTextCtrl(void)	{ Check(TEXT_CTRL); return textCtrl; }

	void	SetButton(wxButton *theButton)	{ button = theButton; }

};
		
/*************************** ParListInfo.**************************************/

class ParListInfo {

	bool		*subParList;
	char		*prefix;
	int			offset;
	DatumPtr	pc;
	DialogList	*parent;
	ParControl	*controlList;
	wxWindow	*lastControl;
	wxNotebook	*notebook;
	wxStringList	rothmanHeadings;

  public:
  	UniParListPtr	parList;

	ParListInfo(DialogList *theParent, DatumPtr thePC, UniParListPtr theParList,
	  int theOffset = 0, char *thePrefix = "");
	~ParListInfo(void);

	void	AddModuleListBoxEntries(DatumPtr pc, wxListBox& listBox);
	bool	CheckChangedValues(void);
	bool	SubParList(int i)		{ return subParList[i]; };
	int		GetOffset(void)	{ return offset; };
	wxWindow *	GetLastControl(void)	{ return lastControl; }
	ParControl& GetParControl(int i)	{ return controlList[i]; }
	void	SetICListListBox(IonChanListPtr theICs, char *prefix);
	void	SetModuleListBox(DatumPtr simulation);
	void	SetParBoolean(int index);
	void	SetParNameList(int index);
	void	SetParNameListWithText(int index);
	void	SetParStandard(wxPanel *parent, int index);
	void	SetParListStandard(wxPanel *parent);
	void	SetParListIonChannel(void);
	bool	SetParValue(int parNum, char *value);
//T	void	SetUpdateFlag(int i, bool state)	{ updateFlagList[i] = state; };

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

