/**********************
 *
 * File:		GrPrintDisp.h
 * Purpose: 	Routines for printing the display window.
 * Comments:	Created to use the WxWin 2 printing routines.
 * Created:		20 Sep 1999
 * Updated:		
 * Copyright:	(c) 1999, University of Essex
 *
 **********************/

#ifndef _GRPRINTDISP_H
#define _GRPRINTDISP_H 1

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define	PRINTDISP_PS_X_OFFSET		50.0
#define	PRINTDISP_PS_Y_OFFSET		50.0
#define PRINTDISP_HEADER_OFFSET		20

#if defined(__WXMSW__)
#	define	PRINTDISP_X_SCALE				7.0
#	define	PRINTDISP_Y_SCALE				7.0
#else
#	define	PRINTDISP_X_SCALE				1.0
#	define	PRINTDISP_Y_SCALE				1.0
#endif /* __WXMSW__ */

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Class definitions *****************************/
/******************************************************************************/

/********************************** PrintDisp *********************************/

class PrintDisp: public wxPrintout
{
	MyCanvas	*canvas;

  public:
	PrintDisp(MyCanvas *theCanvas, const wxChar *title = wxT("DSAM Print-out"));
	bool OnPrintPage(int page);
	bool HasPage(int page);
	bool OnBeginDocument(int startPage, int endPage);
	void GetPageInfo(int *minPage, int *maxPage, int *selPageFrom,
	  int *selPageTo);

};

/******************************************************************************/
/*************************** External Variables *******************************/
/******************************************************************************/

extern wxPrintData *printData;

#endif
