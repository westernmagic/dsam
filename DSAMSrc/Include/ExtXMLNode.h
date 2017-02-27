/**********************
 *
 * File:		ExtXMLNode.h
 * Purpose: 	DSAM simulation base XML Node.
 * Comments:	Adds addition methods for setting "Properties".
 * Author:		L.P.O'Mard
 * Created:		24 Mar 2008
 * Updated:		
 * Copyright:	(c) 2004, 2010 Lowel P. O'Mard
 *
 *********************
 *
 *  This file is part of DSAM.
 *
 *  DSAM is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  DSAM is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with DSAM.  If not, see <http://www.gnu.org/licenses/>.
 *
 *********************/

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

//ToDO: LPO 2017-02-16 Change name from AddProperty to AddAttribute

class DSAMXMLNode: public wxXmlNode
{

  public:
  
	DSAMXMLNode(wxXmlNodeType type, const wxString& name, const wxString&
	  content = wxEmptyString): wxXmlNode(type, name, content) { ; }
	~DSAMXMLNode() { ;}

	void	MyAddAttribute(const wxString& name, const wxString& x) { wxXmlNode::
			  AddAttribute(name, x); }
	void	MyAddAttribute(const wxString& name, int x) { wxXmlNode::AddAttribute(
			  name, wxString::Format(wxT("%d"), x)); }
	void	MyAddAttribute(const wxString& name, long x) { wxXmlNode::AddAttribute(
			  name, wxString::Format(wxT("%ld"), x)); }
	void	MyAddAttribute(const wxString& name, double x) { wxXmlNode::AddAttribute(
			  name, wxString::Format(wxT("%g"), x)); }
	
};

#endif
