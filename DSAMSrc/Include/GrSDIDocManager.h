/**********************
 *
 * File:		GrSDIDocManager.h
 * Purpose: 	Simulation Design Interface base document manager class.
 * Comments:	This class allows me to define the "SaveAs" behaviour, etc.
 * Author:		L.P.O'Mard
 * Created:		10 Mar 2004
 * Updated:		
 * Copyright:	(c) 2004, University of Essex
 *
 **********************/

#ifndef _GRSDIDOCMANAGER_H
#define _GRSDIDOCMANAGER_H 1

#ifdef __GNUG__
// #pragma interface
#endif

#include <wx/docview.h>

/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Enum definitions *********************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Class definitions ********************************/
/******************************************************************************/

/*************************** SDIDocManager ************************************/

class SDIDocManager: public wxDocManager
{
	DECLARE_DYNAMIC_CLASS(SDIDocManager)

  private:

  public:
	SDIDocManager(long flags = wxDEFAULT_DOCMAN_FLAGS, bool initialize = TRUE);

    void OnFileSaveAs(wxCommandEvent& event);

    DECLARE_EVENT_TABLE()

};

#endif
