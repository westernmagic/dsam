/**********************
 *
 * File:		GrParListInfoList.h
 * Purpose:		Module parameter information list handling routines.
 * Comments:	
 * Author:		L. P. O'Mard
 * Created:		11 Feb 1999
 * Updated:		
 * Copyright:	(c) 1999, University of Essex
 *
 **********************/
 
#ifndef _GRPARLISTINFOLIST_H
#define _GRPARLISTINFOLIST_H 1

/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Class definitions ********************************/
/******************************************************************************/

/*************************** ParListInfoList **********************************/

class ParListInfoList {

	BOOLN		labelsFlag;
	DialogList	*parent;

  public:
	DSAMList<ParListInfo>	list;

	ParListInfoList(DialogList *theParent, DatumPtr pc, UniParListPtr parList,
	  BOOLN theLabelsFlag);
	~ParListInfoList(void);

	void	AddInfo(ParListInfo *theInfo)	{ list.Append(list.Number(),
			  (wxObject *) theInfo); };
	int		GetNumDialogs(void)	{ return list.Number(); };
	void	SetInfo(DatumPtr pc, UniParListPtr parList, int offset,
			  char *prefix);

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

