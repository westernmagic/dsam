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

#	include <wx/notebook.h>

/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

#define	PARLISTINFOLIST_ID_NOTEBOOK			1000
#define	PARLISTINFOLIST_MAIN_ICLIST_PAGES	2
#if defined(__MINGW32__)
#	define	PARLISTINFOLIST_NOTEBOOK_ADD_HEIGHT	80
#else
#	define	PARLISTINFOLIST_NOTEBOOK_ADD_HEIGHT	56
#endif

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Class definitions ********************************/
/******************************************************************************/

/*************************** ParListInfoList **********************************/

class ParListInfoList {

	bool	useNotebookControls;
	DialogList	*parent;
	wxNotebook	*notebook;

  public:
	DSAMList<ParListInfo>	list;

	ParListInfoList(DialogList *theParent, DatumPtr pc, UniParListPtr parList);
	~ParListInfoList(void);

	void	AddInfo(ParListInfo *theInfo)	{ list.Append(list.Number(),
			  (wxObject *) theInfo); };
	wxPanel *	UsingNotebook(UniParListPtr parList, const wxString& title);
	ParListInfo *	GetInfo(int index)	{ return list.GetMember(index); }
	wxWindow *	GetLastControl(void);
	wxNotebook *	GetNotebook(void)	{ return notebook; }
	wxSize		GetNotebookSize(void) const;
	int		GetNumDialogs(void)	{ return list.Number(); };
	DialogList *	GetParent(void)		{ return parent; }
	bool	GetUseNotebookControls(void)	{ return useNotebookControls; }
	void	SetStandardInfo(wxPanel *panel, DatumPtr pc, UniParListPtr parList,
			  const wxString& title, int offset = 0, int numPars = -1);
	void	SetPanelledModuleInfo(wxPanel *panel, DatumPtr pc,
			  UniParListPtr parList, int offset = 0, int panelNum = 0);
	void	SetSubParListInfo(ParListInfo *info);

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

