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
#define PARLISTINFO_DEFAULT_X_MARGIN	4
#define PARLISTINFO_DEFAULT_Y_MARGIN	4
#define PARLISTINFO_BROWSE_BUTTON_TEXT	"Browse"
#define PARLISTINFO_BROWSE_BUTTON_WIDTH	50

#if defined(wx_motif) || defined(wx_msw)
#	define	PARLISTINFO_MAX_LABEL			40
#else
#	define	PARLISTINFO_MAX_LABEL			80
#endif

/******************************************************************************/
/*************************** Macro Definitions ********************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Class definitions ********************************/
/******************************************************************************/

/*************************** ParListInfo.**************************************/

class ParListInfo {

	int			infoNum, offset, numPars, maxHeight, maxWidth;
	DatumPtr	pc;
	wxPanel		*parent;
	ParControl	**controlList;
	wxWindow	*lastControl;

  public:
  	UniParListPtr	parList;

	ParListInfo(wxPanel *theParent, DatumPtr thePC, UniParListPtr theParList,
	  int theInfoNum, int theOffset = 0, int theNumPars = -1);
	~ParListInfo(void);

	void	AddModuleListBoxEntries(wxListBox *listBox);
	bool	CheckChangedValues(void);
	void	CheckInterDependence(void);
	int		GetInfoNum(void)			{ return infoNum; }
	wxWindow *	GetLastControl(void)	{ return lastControl; }
	wxSize	GetSize(void) const			{ return wxSize(maxWidth, maxHeight); }
	int		GetNumPars(void)			{ return numPars; }
	int		GetOffset(void)				{ return offset; }
	ParControl * GetParControl(int i);
	wxPanel *	GetParent(void)			{ return parent; }
	UniParListPtr	GetParList(void)	{ return parList; }
	DatumPtr	GetPC(void)			{ return pc; }
	void	ResetControlValues(void);
	void	SetEnabledControls(void);
	void	SetModuleListBox(void);
	void	SetParBoolean(UniParPtr par, int index);
	void	SetParFileName(UniParPtr par, int index);
	void	SetParGeneral(UniParPtr par, int index);
	void	SetParNameList(UniParPtr par, int index);
	void	SetParNameListWithText(UniParPtr par, int index);
	void	SetParStandard(UniParPtr par, int index);
	void	SetParListStandard(void);
	void	SetParListIonChannel(void);
	bool	SetParValue(ParControl *control);
	void	SetSlider(int index);
	void	UpdateControlValues(void);

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

