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
#if 0 // def __WXGTK__
#include <wx/toolbar.h>
#else
#include <wx/tbarsmpl.h>
#endif

/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Enum definitions *********************************/
/******************************************************************************/

enum {

	PALETTE_ANALYSIS = 1,
	PALETTE_CONTROL,
	PALETTE_FILTERS,
	PALETTE_IO,
	PALETTE_MODELS,
	PALETTE_STIMULI,
	PALETTE_TRANSFORMS,
	PALETTE_USER,
	PALETTE_UTILITIES,
	PALETTE_ARROW

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

// TODO for wxWin: wxToolBar95 cannot be moved to a non-0,0 position!
// Needs to have a parent window...
// So use a simple toolbar at present.
#if 0 // def __WXGTK__
#define TOOLPALETTECLASS    wxToolBar
#else
#define TOOLPALETTECLASS    wxToolBarSimple
#endif

class EditorToolPalette: public TOOLPALETTECLASS
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