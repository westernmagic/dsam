/**********************
 *
 * File:		GrSDIDiagram.h
 * Purpose: 	Diagram class for Simulation Design Interface.
 * Comments:	Revised from Julian Smart's Ogledit/doc.h
 * Author:		L.P.O'Mard
 * Created:		13 Nov 2002
 * Updated:		
 * Copyright:	(c) 2002, CNBH, University of Essex
 *
 **********************/

#ifndef _GRSDIDIAGRAM_H
#define _GRSDIDIAGRAM_H 1

#ifdef __GNUG__
// #pragma interface
#endif

#include <wx/docview.h>

#include <wx/ogl/ogl.h>

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

/*************************** SDIDiagram ***************************************/

/*
 * Override a few members for this application
 */
 
class SDIDiagram: public wxDiagram
{
  public:
	SDIDiagram(void) {}
	
	wxShape *	CreateShape(wxClassInfo *shapeInfo, int type, wxBrush *brush);
	void	DrawSimulation(DatumPtr start);
	bool	OnShapeSave(wxExprDatabase& db, wxShape& shape, wxExpr& expr);
	bool	OnShapeLoad(wxExprDatabase& db, wxShape& shape, wxExpr& expr);

};

#endif
  // _OGLSAMPLE_DOC_H_
