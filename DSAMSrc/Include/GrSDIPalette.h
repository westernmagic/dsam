/**********************
 *
 * File:		GrSDIPalette.h
 * Purpose: 	Simulation Process palette.
 * Comments:	Revised from Julian Smart's Ogledit/palette.h
 * Author:		L.P.O'Mard
 * Created:		04 Nov 2002
 * Updated:		
 * Copyright:	(c) 2002, University of Essex
 *
 **********************/

#ifndef _GRSDIPALETTE_H
#define _GRSDIPALETTE_H 1

#ifdef __GNUG__
// #pragma interface
#endif

#include <wx/wx.h>
#include <wx/string.h>
#include <wx/toolbar.h>

/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Enum definitions *********************************/
/******************************************************************************/

/*
 * This enum is used with the ModuleClassSpecifier in GeModuleMgr.h.
 */

enum {

	PALETTE_ARROW = 1000

};

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Class definitions ********************************/
/******************************************************************************/

/*
 * Object editor tool palette
 *
 */

class EditorToolPalette: public wxToolBar
{
  public:
	int currentlySelected;

	EditorToolPalette(wxWindow *parent, const wxPoint& pos = wxDefaultPosition,
	const wxSize& size = wxDefaultSize,
	long style = wxTB_VERTICAL);

	void	Set(void);
	bool	OnLeftClick(int toolIndex, bool toggled);
	void	OnMouseEnter(int toolIndex);
	void	SetSize(int x, int y, int width, int height, int sizeFlags =
	 		 wxSIZE_AUTO);

};

#endif
    // _OGLSAMPLE_PALETTE_H_
