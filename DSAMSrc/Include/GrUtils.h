/**********************
 *
 * File:		GrUtils.h
 * Purpose: 	Routines for handling simple boxes.
 * Comments:	14-05-99 LPO: Added the 'FileSelector_' routine to cope with the
 *				deficiencies (!) of the 'wxFileSelector' routine's 'path'
 *				variable treatment.
 * Author:		L.P.O'Mard
 * Created:		6 Jan 1995
 * Updated:		14 May 199
 * Copyright:	(c) 1999, University of Essex.
 *
 **********************/

#ifndef _GRUTILS_H
#define _GRUTILS_H 1

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define	GRUTILS_HASHSIZE		UINT_MAX

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Class definitions *****************************/
/******************************************************************************/

/********************************** Box ***************************************/

class Box
{

  public:
	double	left, right, top, bottom;

	Box(double theLeft = 0.0, double theRight = 0.0, double theTop = 0.0,
	  double theBottom = 0.0);

	void	 Reset(double theLeft, double theRight, double theTop,
			  double theBottom);
	void	Offset(double x, double y) {bottom += y; top += y; left += x;
			  right += x;}
	void	ShiftVertical(double shift) {bottom += shift; top += shift;}
	void	ShiftHorizontal(double shift) {left += shift; right += shift;}
	double	Height(void) {return (bottom - top);}
	double	Width(void) {return (right - left);}

};

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

wxString	FileSelector_Utils(char *message, char *defaultPath,
			  char *defaultFileName, char *defaultExtension, char *wildcard,
			  int flags, wxWindow *parent);

uInt	Hash_Utils(const char *s);


#endif

