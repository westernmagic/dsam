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

#if defined(wx_motif) || defined(wx_msw)
#	define	PARLISTINFO_MAX_LABEL			40
#else
#	define	PARLISTINFO_MAX_LABEL			80
#endif

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Class definitions ********************************/
/******************************************************************************/

/*************************** ParListInfo.**************************************/

class ParListInfo {

	BOOLN		*updateFlagList, *subParList, labelsFlag;
	char		*prefix;
	int			offset;
	DatumPtr	pc;
	DialogList	*parent;
	MyText		**parTextList;
	MySlider	**parSliderList;
	MyButton	**parButtonList;
	wxStringList	oldPars, labels, rothmanHeadings;

  public:
  	UniParListPtr	parList;

	ParListInfo(DialogList *theParent, DatumPtr thePC, UniParListPtr theParList,
	  int theOffset = 0, char *thePrefix = "", BOOLN theLabelsFlag = TRUE);
	~ParListInfo(void);

	BOOLN	CheckChangedValues(void);
	BOOLN	SubParList(int i)		{ return subParList[i]; };
	int		GetOffset(void)	{ return offset; };
	MySlider *	SetSlider(int index);
	MySlider *	GetSlider(int i)	{ return parSliderList[i]; };
	char *	GetOldValue(int i)	{ return (char *) oldPars.Nth(i)->Data(); };
	MyText *	GetParText(int i)	{ return parTextList[i]; };
	void	ResetOldParText(int index);
	void	ResetOldValue(int index);
	void	ResetOldValues(void);
	void	SetICListListBox(IonChanListPtr theICs, char *prefix);
	void	SetModuleListBox(DatumPtr simulation);
	void	SetParStandard(int index);
	void	SetParListStandard(void);
	void	SetParListIonChannel(void);
	BOOLN	SetParValue(int parNum, char *value);
	void	SetUpdateFlag(int i, BOOLN state)	{ updateFlagList[i] = state; };

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

