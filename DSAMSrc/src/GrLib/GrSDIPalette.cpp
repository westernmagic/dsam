/**********************
 *
 * File:		GrSDIPalette.cpp
 * Purpose: 	Simulation Process palette.
 * Comments:	Revised from Julian Smart's Ogledit/Palette.cpp
 * Author:		L.P.O'Mard
 * Created:		04 Nov 2002
 * Updated:		
 * Copyright:	(c) 2002, University of Essex
 *
 **********************/

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG_H */

#ifdef HAVE_WX_OGL_OGL_H

#ifdef __GNUG__
// #pragma implementation
#endif

// For compilers that support precompilation, includes "wx.h".
#include <wx/wxprec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/toolbar.h>

#include <ctype.h>
#include <stdlib.h>
#include <math.h>

#include "GeCommon.h"
#include "GrSimMgr.h"
//#include "GrSDIDoc.h"
#include "GrSDIPalette.h"

/******************************************************************************/
/****************************** Bitmaps ***************************************/
/******************************************************************************/

// Include pixmaps
#if defined(__WXGTK__) || defined(__WXMOTIF__)
#include "Bitmaps/arrow.xpm"
#include "Bitmaps/tool1.xpm"
#include "Bitmaps/tool2.xpm"
#include "Bitmaps/tool3.xpm"
#include "Bitmaps/tool4.xpm"
#endif

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Methods (Subroutines) *************************/
/******************************************************************************/

/*
 * Object editor tool palette
 *
 */

EditorToolPalette::EditorToolPalette(wxWindow* parent, const wxPoint& pos,
  const wxSize& size, long style): TOOLPALETTECLASS(parent, -1, pos, size,
  style)
{
	currentlySelected = -1;
	Set();

	#if 1 // ndef __WXGTK__
	SetMaxRowsCols(1000, 1);
	#endif
}

/******************************************************************************/
/*************************** Set **********************************************/
/******************************************************************************/

void
EditorToolPalette::Set(void)
{
	// Load palette bitmaps
	#ifdef __WXMSW__
	wxBitmap PaletteAnaTool("TOOL1");
	wxBitmap PaletteCtrlTool("TOOL2");
	wxBitmap PaletteFiltTool("TOOL3");
	wxBitmap PaletteIOTool("TOOL4");
	wxBitmap PaletteArrow("ARROWTOOL");
	wxBitmap PaletteModelTool("TOOL1");
	wxBitmap PaletteStimTool("TOOL2");
	wxBitmap PaletteTransTool("TOOL3");
	wxBitmap PaletteUserTool("TOOL4");
	wxBitmap PaletteUtilTool("TOOL1");
	wxBitmap PaletteArrow("ARROWTOOL");
	#elif defined(__WXGTK__) || defined(__WXMOTIF__)
	wxBitmap PaletteAnaTool(tool1_xpm);
	wxBitmap PaletteCtrlTool(tool2_xpm);
	wxBitmap PaletteFiltTool(tool3_xpm);
	wxBitmap PaletteIOTool(tool4_xpm);
	wxBitmap PaletteModelTool(tool1_xpm);
	wxBitmap PaletteStimTool(tool2_xpm);
	wxBitmap PaletteTransTool(tool3_xpm);
	wxBitmap PaletteUserTool(tool4_xpm);
	wxBitmap PaletteUtilTool(tool1_xpm);
	wxBitmap PaletteArrow(arrow_xpm);
	#endif

	SetMargins(2, 2);
	SetToolBitmapSize(wxSize(22, 22));

	AddTool(PALETTE_ARROW, PaletteArrow, wxNullBitmap, TRUE, 0, -1, NULL,
	  "Pointer");
	AddTool(PALETTE_ANALYSIS, PaletteAnaTool, wxNullBitmap, TRUE, 0, -1, NULL,
	  "Analysis Process");
	AddTool(PALETTE_CONTROL, PaletteCtrlTool, wxNullBitmap, TRUE, 0, -1, NULL,
	  "Control Process");
	AddTool(PALETTE_FILTERS, PaletteFiltTool, wxNullBitmap, TRUE, 0, -1, NULL,
	  "Filter Process");
	AddTool(PALETTE_IO, PaletteIOTool, wxNullBitmap, TRUE, 0, -1, NULL,
	  "Input/Output Process");
	AddTool(PALETTE_MODELS, PaletteModelTool, wxNullBitmap, TRUE, 0, -1, NULL,
	  "Model Process");
	AddTool(PALETTE_STIMULI, PaletteStimTool, wxNullBitmap, TRUE, 0, -1, NULL,
	  "Stimulus Process");
	AddTool(PALETTE_TRANSFORMS, PaletteTransTool, wxNullBitmap, TRUE, 0, -1,
	  NULL, "Transform Process");
	AddTool(PALETTE_USER, PaletteUserTool, wxNullBitmap, TRUE, 0, -1,
	  NULL, "User defined Process");
	AddTool(PALETTE_UTILITIES, PaletteUtilTool, wxNullBitmap, TRUE, 0, -1,
	  NULL, "Utility Process");

	Realize();

	ToggleTool(PALETTE_ARROW, TRUE);
	currentlySelected = PALETTE_ARROW;

}

/******************************************************************************/
/*************************** OnLeftClick **************************************/
/******************************************************************************/

bool
EditorToolPalette::OnLeftClick(int toolIndex, bool toggled)
{
	// BEGIN mutual exclusivity code
	if (toggled && (currentlySelected != -1) && (toolIndex !=
	  currentlySelected))
		ToggleTool(currentlySelected, FALSE);

	if (toggled)
		currentlySelected = toolIndex;
	else if (currentlySelected == toolIndex)
		currentlySelected = -1;
	//  END mutual exclusivity code

	return TRUE;
}

/******************************************************************************/
/*************************** OnMouseEnter *************************************/
/******************************************************************************/

void
EditorToolPalette::OnMouseEnter(int toolIndex)
{
}

/******************************************************************************/
/*************************** SetSize ******************************************/
/******************************************************************************/

void
EditorToolPalette::SetSize(int x, int y, int width, int height, int sizeFlags)
{
	TOOLPALETTECLASS::SetSize(x, y, width, height, sizeFlags);
}

#endif /* HAVE_WX_OGL_OGL_H */
