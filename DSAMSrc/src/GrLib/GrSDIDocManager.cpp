/**********************
 *
 * File:		GrSDIDocManager.cpp
 * Purpose: 	Simulation Design Interface base document manager class.
 * Comments:	This class allows me to define the "SaveAs" behaviour, etc.
 * Author:		L.P.O'Mard
 * Created:		10 Mar 2004
 * Updated:		
 * Copyright:	(c) 2004, University of Essex
 *
 **********************/

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG_H */

#include <stdio.h>
#include <stdlib.h>

#include "ExtCommon.h"

#include "GeCommon.h"
#include "GrSDIDocManager.h"

/******************************************************************************/
/****************************** Bitmaps ***************************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Global variables ******************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Methods (Subroutines) *************************/
/******************************************************************************/

IMPLEMENT_DYNAMIC_CLASS(SDIDocManager, wxDocManager)

BEGIN_EVENT_TABLE(SDIDocManager, wxDocManager)
    EVT_MENU(wxID_SAVEAS, SDIDocManager::OnFileSaveAs)
END_EVENT_TABLE()

/******************************************************************************/
/****************************** Constructor ***********************************/
/******************************************************************************/

SDIDocManager::SDIDocManager(long flags, bool initialize): wxDocManager(flags,
  initialize)
{

}

/******************************************************************************/
/****************************** Destructor ************************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Callbacks *************************************/
/******************************************************************************/

/****************************** OnFileSaveAs **********************************/

/*
 * A replacement for the default 'OnFileSaveAs'.
 * The conditional "#if" allows the use of multiple filters for the platforms
 * that allow it.
 */

void
SDIDocManager::OnFileSaveAs(wxCommandEvent& event)
{
#	if defined(__WXMSW__) || defined(__WXGTK__) || defined(__WXMAC__)
	unsigned int	i;
	wxString		descrBuf;
	wxDocTemplate	*docTemplate, *temp;

	docTemplate = GetCurrentDocument()->GetDocumentTemplate();
	wxString	oldFileFilter = docTemplate->GetFileFilter();
	for (i = 0; i < m_templates.GetCount(); i++) {
		temp = (wxDocTemplate *) (m_templates.Item(i)->GetData());
		if (temp->IsVisible()) {
		// add a '|' to separate this filter from the previous one
		if ( !descrBuf.IsEmpty() )
    		descrBuf << wxT('|');
		descrBuf << temp->GetDescription() << wxT(" (") << temp->GetFileFilter(
		  ) << wxT(") |") << temp->GetFileFilter();
		}
	}
	docTemplate->SetFileFilter(descrBuf);
	wxDocManager::OnFileSaveAs(event);
	docTemplate->SetFileFilter(oldFileFilter);
#	else
	wxDocManager::OnFileSaveAs(event);
#	endif

}

