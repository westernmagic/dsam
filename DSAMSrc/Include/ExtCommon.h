/**********************
 *
 * File:		ExtCommon.h
 * Purpose:		GUI General header
 * Comments:	23-06-98 LPO: Introduced manager window resizing:
 * Author:		L. P. O'Mard
 * Created:		12 Dec 2003
 * Updated:		
 * Copyright:	(c) 2003 CNBH, University of Essex
 *
 **********************/

#ifndef	_EXTCOMMON_H
#define _EXTCOMMON_H	1

#if defined(LIBRARY_COMPILE) && defined(GRAPHICS_SUPPORT)
#	define DSAM_DO_NOT_DEFINE_MYAPP = 1
#	define USE_GUI = 1
#endif

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
