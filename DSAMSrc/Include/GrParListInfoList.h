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
#if defined(__WXMSW__)
#	define	PARLISTINFOLIST_NOTEBOOK_ADD_HEIGHT	100
#else
#	define	PARLISTINFOLIST_NOTEBOOK_ADD_HEIGHT	80
#endif

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

/********************************** Pre-references ****************************/

class ModuleParDialog;

/******************************************************************************/
/*************************** Class definitions ********************************/
/******************************************************************************/

/*************************** wxArrayParListInfo *******************************/

WX_DEFINE_ARRAY(ParListInfo *, wxArrayParListInfo);

/*************************** ParListInfoList **********************************/

class ParListInfoList {

	bool	useNotebookControls;
	int		notebookPanel;
	ModuleParDialog	*parent;
	//wxNotebook	*notebook;

  public:
	wxNotebook	*notebook;
	wxArrayParListInfo	list;

	ParListInfoList(ModuleParDialog *theParent, DatumPtr pc,
	  UniParListPtr parList);
	~ParListInfoList(void);

	bool	UseNewNotebook(UniParPtr par);
	wxPanel *	UsingNotebook(UniParListPtr parList, const wxString& title);
	wxNotebook *	GetNotebook(void)	{ return notebook; }
	wxSize		GetNotebookSize(void) const;
	ModuleParDialog *	GetParent(void)		{ return parent; }
	bool	GetUseNotebookControls(void)	{ return useNotebookControls; }
	void	SetStandardInfo(wxPanel *panel, DatumPtr pc, UniParListPtr parList,
			  const wxString& title, int offset = 0, int numPars = -1);
	void	SetPanelledModuleInfo(wxPanel *panel, DatumPtr pc,
			  UniParListPtr parList, int offset = 0, int panelNum = 0);
	void	SetSubParListInfo(ParListInfo *info);
	void	UpdateAllControlValues(void);

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

