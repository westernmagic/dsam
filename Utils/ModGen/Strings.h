/**********************
 *
 * File:		Strings.h
 * Purpose: 	Module parser string handling routines header file.
 * Comments:	
 * Author:		L.P.O'Mard
 * Created:		7 Nov 1995
 * Updated:	
 * Copyright:	(c) 1995, Loughborough University of Technology
 *
 **********************/

#ifndef _STRINGS_H
#define _STRINGS_H	1

/******************************************************************************/
/****************************** Constant definitions **************************/
/******************************************************************************/

#define	SPACES_PER_TAB			4
 
/******************************************************************************/
/****************************** Macro definitions *****************************/
/******************************************************************************/

#define	TAB_SPACES(POS)		(SPACES_PER_TAB - ((POS) % SPACES_PER_TAB))

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

char *	AddChar(char *string, char c, int *pos, BOOLN stringControlChars);

char *	Capital(char *string);

char *	GetExtension(char *fileName);

char *	GetFileNameFPath(char *fileName);

char *	PluralToSingular(char *string);

void	Print(FILE *fp, char *prefix, char *string);

int		PrintAndAdjustLine(FILE *fp, char *prefix, char *string,
		  char *eol, BOOLN clearLeadingSpace);

void	QuickSortNP(char *v[], int n, const char *delimiters, int left,
		  int right);

void	QuickSortP(char *v[], int left, int right);

int		RealStringLength(char *string);

char *	StripFileExtension(char *fileName, char *extension);

char *	StrBreak(char *string, int n, const char *delimiters);

void	SwapP(char *v[], int i, int j);

char *	UpperCase(char *string);

#endif

