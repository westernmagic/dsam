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

#define	DIAGRAM_DEFAULT_SHAPE_WIDTH		90.0
#define	DIAGRAM_DEFAULT_SHAPE_HEIGHT	50.0
#define DIAGRAM_DEFAULT_INITIAL_X		60.0
#define DIAGRAM_DEFAULT_INITIAL_Y		60.0
#define DIAGRAM_DEFAULT_X_SEPARATION	20.0
#define DIAGRAM_DEFAULT_Y_SEPARATION	20.0
#define	DIAGRAM_LABEL_WIDTH_MARGIN		10.0
#define	DIAGRAM_LABEL_HEIGHT_MARGIN		5.0
#define	DIAGRAM_ARROW_TEXT				"Arrow head connection"
#define DIAGRAM_ARROW_SIZE				10.0
#define	DIAGRAM_REPEAT_ARROW_TEXT		"Arrow head 'repeat' connection"
#define DIAGRAM_REPEAT_ARROW_SIZE		10.0
#define DIAGRAM_ENABLED_BRUSH			wxCYAN_BRUSH
#define DIAGRAM_DISENABLED_BRUSH		wxGREY_BRUSH

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
/*************************** Pre reference definitions ************************/
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
	bool	ok, loadIDsFromFile;
	int		direction;
	double	x, y;
	double	xScale, yScale;
	EarObjectPtr	simProcess;

  public:
	SDIDiagram(void);
	
	wxShape *	AddLineShape(wxShape *fromShape, wxShape *toShape,
				  int lineType);
	void	AddShape(wxShape *shape);
	wxShape *	CreateBasicShape(wxClassInfo *shapeInfo, int type,
				  wxBrush *brush);
	wxShape *	CreateLoadShape(DatumPtr pc, wxClassInfo *shapeInfo,
				  wxBrush *brush);
	void	DrawDefaultConnection(DatumPtr pc, wxShape *shape);
	void	DrawSimConnections(void);
	void	DrawSimShapes(void);
	void	DrawSimulation(void);
	DatumPtr	FindShapeDatum(uInt id);
	EarObjectPtr	GetSimProcess(void)	{ return simProcess; }
	double	GetXScale(void)		{ return xScale; }
	double	GetYScale(void)		{ return yScale; }
	void	RedrawShapeLabel(wxShape *shape);
	void	Rescale(double theXScale, double theYScale);
	bool	SaveFile(const wxString& filename);
	void	SetOk(bool status)		{ ok = status; }
	void	SetLoadIDsFromFile(bool status)		{ loadIDsFromFile = status; }
	void	SetSimProcess(EarObjectPtr process)	{ simProcess = process; }
	bool	VerifyDiagram(void);
	bool	UnselectAllShapes(void);

};

#endif
  // _OGLSAMPLE_DOC_H_
