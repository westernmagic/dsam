/**********************
 *
 * File:		GrSDIEvtHandler.h
 * Purpose: 	Event handler class for Simulation Design Interface.
 * Comments:	Revised from Julian Smart's Ogledit/doc.h
 * Author:		L.P.O'Mard
 * Created:		13 Nov 2002
 * Updated:		
 * Copyright:	(c) 2002, CNBH, University of Essex
 *
 **********************/

#ifndef _GRSDIEVTHANDLER_H
#define _GRSDIEVTHANDLER_H 1

#ifdef __GNUG__
// #pragma interface
#endif

#include <wx/ogl/ogl.h>

/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Enum definitions *********************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

/********************************** Pre-references ****************************/

class ModuleParDialog;

/******************************************************************************/
/*************************** Class definitions ********************************/
/******************************************************************************/

/*************************** SDIEvtHandler ************************************/

/*
 * All shape event behaviour is routed through this handler, so we don't
 * have to derive from each shape class. We plug this in to each shape.
 */
 
class SDIEvtHandler: public wxShapeEvtHandler
{
  public:
	int		processType;
	wxString	label;
	DatumPtr	pc;
	ModuleParDialog	*dialog;

	SDIEvtHandler(wxShapeEvtHandler *prev = NULL, wxShape *shape = NULL,
	  const wxString& lab = "", int theProcessType = -1);
	~SDIEvtHandler(void);

	wxStringList *	GetProcessList(void);
	bool	EditInstruction(void);
	void	FreeInstruction(void);
	bool	InitInstruction(void);
	void	SetSelectedShape(wxClientDC &dc);

	void	OnLeftClick(double x, double y, int keys = 0, int attachment = 0);
	void	OnRightClick(double x, double y, int keys = 0, int attachment = 0);
	void	OnBeginDragRight(double x, double y, int keys = 0, int attachment =
			  0);
	void	OnDragRight(bool draw, double x, double y, int keys = 0,
			  int attachment = 0);
	void	OnEndDragRight(double x, double y, int keys = 0, int attachment =
			  0);
	void	OnEndSize(double x, double y);
	void	SetDialog(ModuleParDialog *dlg) { dialog = dlg; }

};

#endif
