/**********************
 *
 * File:		ExtXMLNode.h
 * Purpose: 	DSAM simulation base XML Node.
 * Comments:	Adds addition methods for setting "Properties".
 * Author:		L.P.O'Mard
 * Created:		24 Mar 2008
 * Updated:		
 * Copyright:	(c) 2008, Lowel P. O'Mard
 *
 **********************/

#ifndef _EXTXMLNODE_H
#define _EXTXMLNODE_H 1

#ifdef __GNUG__
// #pragma interface
#endif

#include <wx/string.h>
#include <wx/xml/xml.h>

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
/*************************** Pre reference definitions ************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Class definitions ********************************/
/******************************************************************************/

/*************************** DSAMXMLNode **************************************/

class DSAMXMLNode: public wxXmlNode
{

  public:
  
	DSAMXMLNode(wxXmlNodeType type, const wxString& name, const wxString&
	  content = wxEmptyString): wxXmlNode(type, name, content) { ; }
	~DSAMXMLNode() { ;}

	void	AddProperty(const wxString& name, const wxString& x) { wxXmlNode::
			  AddProperty(name, x); }
	void	AddProperty(const wxString& name, int x) { wxXmlNode::AddProperty(
			  name, wxString::Format(wxT("%d"), x)); }
	void	AddProperty(const wxString& name, long x) { wxXmlNode::AddProperty(
			  name, wxString::Format(wxT("%ld"), x)); }
	void	AddProperty(const wxString& name, double x) { wxXmlNode::AddProperty(
			  name, wxString::Format(wxT("%g"), x)); }
	
};

#endif
