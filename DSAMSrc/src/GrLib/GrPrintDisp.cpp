/**********************
 *
 * File:		GrPrintDisp.cpp
 * Purpose: 	Routines for printing the display window.
 * Comments:	Created to use the WxWin 2 printing routines.
 *				The global 'printData' is initialised in 'MyApp::OnInit'.
 * Created:		20 Sep 1999
 * Updated:		
 * Copyright:	(c) 1999, University of Essex
 *
 **********************/

#ifdef __GNUG__
#pragma implementation
#pragma interface
#endif

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
#	include "wx/metafile.h"
#	include "wx/print.h"
#	include "wx/printdlg.h"

#	if wxTEST_POSTSCRIPT_IN_MSW
#		include "wx/generic/printps.h"
#		include "wx/generic/prntdlgg.h"
#	endif
#endif

#include "GeCommon.h"
#include "GeSignalData.h"
#include "GeEarObject.h"
#include "GrUtils.h"
#include "GrBrushes.h"
#include "GrSignalDisp.h"
#include "GrDisplayS.h"
#include "GrCanvas.h"
#include "GrPrintDisp.h"

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

wxPrintData *printData = (wxPrintData*) NULL ;

/******************************************************************************/
/*************************** Event tables *************************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Methods ***************************************/
/******************************************************************************/

/****************************** Constructor ***********************************/

/*
 * Define a constructor for my canvas.
 */

PrintDisp::PrintDisp(MyCanvas *theCanvas, char *title): wxPrintout(title)
{
	canvas = theCanvas;

}

/****************************** OnPrintPage ***********************************/

bool
PrintDisp::OnPrintPage(int page)
{
	wxDC *dc = GetDC();

	if (dc) {
		char buf[200];
		sprintf(buf, "%s: %s", canvas->GetSignalDispPtr()->title,
		  wxNow().GetData());
		dc->DrawText(buf, (int) PRINTDISP_PS_X_OFFSET, (int)
		  PRINTDISP_PS_Y_OFFSET);

		//canvas->SetUseTextAdjust(TRUE); - not needed at present.
		dc->SetUserScale(PRINTDISP_X_SCALE, PRINTDISP_Y_SCALE);
		canvas->DrawGraph(*dc, (int) PRINTDISP_PS_X_OFFSET, (int) (
		  PRINTDISP_PS_Y_OFFSET + PRINTDISP_HEADER_OFFSET));
		//canvas->SetUseTextAdjust(FALSE);

		return(TRUE);
	}
	return(FALSE);

}

/****************************** OnBeginDocument *******************************/

bool
PrintDisp::OnBeginDocument(int startPage, int endPage)
{
    if (!wxPrintout::OnBeginDocument(startPage, endPage))
        return FALSE;
    
    return TRUE;

}

/****************************** GetPageInfo ***********************************/

void
PrintDisp::GetPageInfo(int *minPage, int *maxPage, int *selPageFrom,
  int *selPageTo)
{
    *minPage = 1;
    *maxPage = 1;
    *selPageFrom = 1;
    *selPageTo = 1;

}

/****************************** HasPage ***************************************/

bool
PrintDisp::HasPage(int pageNum)
{
    return (pageNum == 1);

}

