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

#define	DIAGRAM_DEFAULT_SHAPE_WIDTH		60.0
#define	DIAGRAM_DEFAULT_SHAPE_HEIGHT	60.0
#define	DIAGRAM_HOLLOW_CIRCLE_TEXT		"Hollow circle connection"
#define DIAGRAM_HOLLOW_CIRCLE_SIZE		6.0
#define	DIAGRAM_ARROW_TEXT				"Arrow head 'repeat' connection"
#define DIAGRAM_ARROW_SIZE				10.0

/******************************************************************************/
/*************************** Macro Definitions ********************************/
/******************************************************************************/

#define	SHAPE_PC(SHAPE)	(((SDIEvtHandler *) (SHAPE)->GetEventHandler())->pc)

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
	double	x, y;
	DatumPtr	simulation;

  public:
	SDIDiagram(void);
	
	void	AddLineShape(wxShape *fromShape, wxShape *toShape, int lineType);
	wxShape *	CreateBasicShape(wxClassInfo *shapeInfo, int type,
				  wxBrush *brush);
	wxShape *	CreateLoadShape(DatumPtr pc, wxClassInfo *shapeInfo, int type,
				  wxBrush *brush);
	void	DrawSimConnections(void);
	void	DrawSimShapes(void);
	void	DrawSimulation(void);
	DatumPtr	FindShapeProcess(uInt id);
	bool	OnShapeSave(wxExprDatabase& db, wxShape& shape, wxExpr& expr);
	bool	OnShapeLoad(wxExprDatabase& db, wxShape& shape, wxExpr& expr);
	void	SetProcessClientData(DatumPtr pc, wxShape *shape);
	void	SetSimulation(DatumPtr theSim)	{ simulation = theSim; }
	bool	VerifyDiagram(void);

};

#endif
  // _OGLSAMPLE_DOC_H_
