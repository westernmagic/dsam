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
#include "GeSignalData.h"
#include "GeEarObject.h"
#include "GeUniParMgr.h"
#include "GeModuleMgr.h"
#include "GrSimMgr.h"
//#include "GrSDIDoc.h"
#include "GrSDIPalette.h"

/******************************************************************************/
/****************************** Bitmaps ***************************************/
/******************************************************************************/

// Include pixmaps
#if defined(__WXGTK__) || defined(__WXMOTIF__)
#include "Bitmaps/arrow.xpm"
#include "Bitmaps/analysis_tool.xpm"
#include "Bitmaps/control_tool.xpm"
#include "Bitmaps/filter_tool.xpm"
#include "Bitmaps/io_tool.xpm"
#include "Bitmaps/model_tool.xpm"
#include "Bitmaps/stimulus_tool.xpm"
#include "Bitmaps/transform_tool.xpm"
#include "Bitmaps/user_tool.xpm"
#include "Bitmaps/utility_tool.xpm"
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
	wxBitmap PaletteAnaTool("analysis_tool");
	wxBitmap PaletteCtrlTool("control_tool");
	wxBitmap PaletteFiltTool("filter_tool");
	wxBitmap PaletteIOTool("io_tool");
	wxBitmap PaletteArrow("ARROWTOOL");
	wxBitmap PaletteModelTool("model_tool");
	wxBitmap PaletteStimTool("stimulus_tool");
	wxBitmap PaletteTransTool("transform_tool");
	wxBitmap PaletteUserTool("user_tool");
	wxBitmap PaletteUtilTool("utility_tool");
	#elif defined(__WXGTK__) || defined(__WXMOTIF__)
	wxBitmap PaletteAnaTool(analysis_tool_xpm);
	wxBitmap PaletteCtrlTool(control_tool_xpm);
	wxBitmap PaletteFiltTool(filter_tool_xpm);
	wxBitmap PaletteIOTool(io_tool_xpm);
	wxBitmap PaletteModelTool(model_tool_xpm);
	wxBitmap PaletteStimTool(stimulus_tool_xpm);
	wxBitmap PaletteTransTool(transform_tool_xpm);
	wxBitmap PaletteUserTool(user_tool_xpm);
	wxBitmap PaletteUtilTool(utility_tool_xpm);
	wxBitmap PaletteArrow(arrow_xpm);
	#endif

	SetMargins(2, 2);
	SetToolBitmapSize(wxSize(22, 22));

	AddTool(PALETTE_ARROW, PaletteArrow, wxNullBitmap, TRUE, 0, -1, NULL,
	  "Pointer");
	AddTool(ANALYSIS_MODULE_CLASS, PaletteAnaTool, wxNullBitmap, TRUE, 0, -1,
	  NULL, "Analysis Process");
	AddTool(CONTROL_MODULE_CLASS, PaletteCtrlTool, wxNullBitmap, TRUE, 0, -1,
	  NULL, "Control Process");
	AddTool(FILTER_MODULE_CLASS, PaletteFiltTool, wxNullBitmap, TRUE, 0, -1,
	  NULL, "Filter Process");
	AddTool(IO_MODULE_CLASS, PaletteIOTool, wxNullBitmap, TRUE, 0, -1, NULL,
	  "Input/Output Process");
	AddTool(MODEL_MODULE_CLASS, PaletteModelTool, wxNullBitmap, TRUE, 0, -1,
	  NULL, "Model Process");
	AddTool(STIMULUS_MODULE_CLASS, PaletteStimTool, wxNullBitmap, TRUE, 0, -1,
	  NULL, "Stimulus Process");
	AddTool(TRANSFORM_MODULE_CLASS, PaletteTransTool, wxNullBitmap, TRUE, 0, -1,
	  NULL, "Transform Process");
	AddTool(USER_MODULE_CLASS, PaletteUserTool, wxNullBitmap, TRUE, 0, -1,
	  NULL, "User defined Process");
	AddTool(UTILITY_MODULE_CLASS, PaletteUtilTool, wxNullBitmap, TRUE, 0, -1,
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
