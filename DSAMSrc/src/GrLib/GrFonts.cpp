/**********************
 *
 * File:		GrFonts.cpp
 * Purpose:		Global font variable and handling routines.
 * Comments:	
 * Author:		L. P. O'Mard
 * Created:		27 Oct 1998
 * Updated:		
 * Copyright:	(c) 1998, University of Essex
 *
 **********************/

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG_H */

#if defined(GRAPHICS_SUPPORT) && defined(__cplusplus)
// For compilers that support precompilation, includes "wx.h".
#	include <wx/wxprec.h>

#	ifdef __BORLANDC__
	    #pragma hdrstop
#	endif

// Any files you want to include if not precompiling by including
// the whole of <wx/wx.h>
#	ifndef WX_PRECOMP
#		include <wx/wx.h>
#	endif

// Any files included regardless of precompiled headers
#endif

#include "GrFonts.h"

/******************************************************************************/
/****************************** Bitmaps ***************************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

// Must initialise these in OnInit, not statically
wxFont    *diagnosticsWindowFont;
wxFont    *itemFont;
wxFont    *labelFont;
wxFont    *textWindowFont;

/******************************************************************************/
/****************************** Methods (Subroutines) *************************/
/******************************************************************************/

void
Init_Fonts(void)
{
	diagnosticsWindowFont = new wxFont(10, wxSWISS, wxNORMAL, wxNORMAL);
	itemFont = new wxFont(10, wxROMAN, wxNORMAL, wxNORMAL);
	labelFont = new wxFont(12, wxROMAN, wxITALIC, wxBOLD);
	textWindowFont = new wxFont(12, wxSWISS, wxNORMAL, wxNORMAL);
}
