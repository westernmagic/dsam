/**********************
 *
 * File:		GrBrushes.h
 * Purpose: 	Routines for the globally used brushes.
 * Comments:	
 * Author:		L.P.O'Mard
 * Created:		26 Nov 1996
 * Updated:		
 * Copyright:	(c) 1996, University of Essex
 *
 **********************/

#ifndef _GRBRUSHES_H
#define _GRBRUSHES_H 1

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define	MAX_BRUSHES						20
#define	BRUSHES_GREYSCALE_MAX_RED		255
#define	BRUSHES_GREYSCALE_MAX_BLUE		255
#define	BRUSHES_GREYSCALE_MAX_GREEN		255

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Class definitions *****************************/
/******************************************************************************/

class GrBrushes
{
	int		numBrushes;
	wxBrush	*brush[MAX_BRUSHES];

  public:
	GrBrushes(void);
	~GrBrushes(void);
	
	void	SetGreyScales(int theNumGreyScales);
	wxBrush	*GetBrush(int theBrush);

};


/******************************************************************************/
/*************************** External Variables *******************************/
/******************************************************************************/

extern GrBrushes	*greyBrushes;

#endif

