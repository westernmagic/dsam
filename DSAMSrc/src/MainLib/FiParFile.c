/**********************
 *
 * File:		FiParFile.c
 * Purpose:		This module contains the general parameter file handling
 *				routines for the DSAM Core Routines library.
 * Comments:	07-05-99 LPO: I have changed the 'IdentifyFormat_' routine so
 *				that it now returns any identified extra format characters using
 *				the 'extraFmt' header argument.
 *				29-05-99 LPO: Introduced the 'ExtractQuotedString_' routine so
 *				that strings can now be quoted, so that multiple words can be
 *				read.
 * Author:		L. P. O'Mard
 * Created:		12 Jul 1993
 * Updated:		29 May 1999
 * Copyright:	(c) 1999,  University of Essex.
 *
 **********************/

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG_H */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>

#include "GeCommon.h"
#include "UtString.h"
#include "FiParFile.h"
 
/******************************************************************************/
/*************************** Global Variables *********************************/
/******************************************************************************/

ParFilePtr		parFile = NULL;

/******************************************************************************/
/*************************** Functions and subroutines ************************/
/******************************************************************************/

/*
 * This routine initialises the parFile data structure.  It allocates its
 * memory dynamically, and sets the respective fields to their default.
 * values.
 */

/****************************** Init ******************************************/

BOOLN
Init_ParFile(void)
{
	static const char *funcName = "Init_ParFile";
	
	if (parFile != NULL) {
		NotifyError("%s: Module already initialised!", funcName);
		return(FALSE);
	}
	if ((parFile = (ParFile *) malloc(sizeof(ParFile))) == NULL) {
		NotifyError("%s: Cannot allocate memory for parameters.", funcName);
		return(FALSE);
	}
	parFile->diagnosticsFlag = FALSE;
	parFile->emptyLineMessageFlag = TRUE;
	parFile->parLineCount = 0;
	return(TRUE);

}

/********************************* CheckInit **********************************/

/*
 * This routine checks that the the module) has been initialised.
 * It prevents attempts to use uninitailised pointers which will cause
 * the module to crash the system.
 * It returns FALSE if the gobal variable has not been initialised.
 */
 
BOOLN
CheckInit_ParFile(void)
{
	static const char *funcName = "CheckInit_ParFile";

	if (parFile == NULL) {
		NotifyError("%s: General parameter file routines module not "
		  "initialised.", funcName);
		return(FALSE);
	}
	return(TRUE);
	
}

/********************************* Free ***************************************/

/*
 * This routine frees the space allocated for the global variable parFile.
 */

BOOLN
Free_ParFile(void)
{
	static const char *funcName = "";

	if (!CheckInit_ParFile()) {
		NotifyError("%s: Module not initialised!", funcName);
		return(FALSE);
	}
	free(parFile);
	parFile = NULL;
	return(TRUE);
	
}

/****************************** GetLine ***************************************/

/*
 * This function reads a line from a file.  If the maximum number of characters
 * is exceeded, then it flushes the rest of the line in the file.
 * It returns a pointer to the string store if successful, and a NULL if not.
 */

char *
GetLine_ParFile(char *line, int maxChars, FILE *fp)
{
	int		i, c;
	
	for (i = 0; ((c = fgetc(fp)) != EOF) && (i < maxChars -1) && (c != '\n');
	  i++)
		line[i] = c;
	line[i] = '\0';
	if (c == EOF && i == 0)
		return(NULL);
	if (c != '\n')
		while ((c = fgetc(fp)) != '\n' && c != EOF)
			;
	return(line);

}	

/****************************** CommentOrBlankLine ****************************/

/*
 * This routine checks if a line read is a comment line;
 * it returns TRUE if it is.
 * It also checks for blank lines.
 * A comment line is any line that begins with white space followed by a '#'.
 */

BOOLN
CommentOrBlankLine_ParFile(char *line)
{
	char	*p;
	
	for (p = line; (*p != '\0') && isspace(*p); p++)
		;
	if (strlen(p) == 0)
		return(TRUE);
	if (*p == '#')
		return(TRUE);
	return(FALSE);

}
			
/****************************** IdentifyFormat ********************************/

/*
 * This subroutine returns a type character as defined by the format string
 * passed as an argument.  Only a single value is looked for.
 * It assumes that only doubles are used.
 * It returns EOF if a '%' is not found at the start of the string, and a '\0'
 * if an invalid character is found.
 */

int
IdentifyFormat_ParFile(char *fmt, char *extraFmt)
{
	static const char *funcName = "IdentifyFormat_ParFile";
	char	*p;

	p = fmt;
	*extraFmt = '\0';
	if (*p++ == '%') {
		if (*p == 'l')
			*extraFmt = *p++;
		switch (*p) {
			case 'u':
			case 'd':
			case 'f':
			case 's':
			case 'i':	/* Identifier. */
				return((int) *p);
			default:
				NotifyError("%s: Illegal format string, '%s'.", funcName, fmt);
				return('\0');
		}
	} else
		return(EOF);

}

/****************************** SetEmptyLineMessage ***************************/

/*
 * This routine turns on of off the "GetPars" empty line error message.
 */

void
SetEmptyLineMessage_ParFile(BOOLN status)
{
	parFile->emptyLineMessageFlag = status;

}

/****************************** ExtractQuotedString ***************************/

/*
 * This routine extracts a quoted string from a string, missing out the quotes.
 * If there is only one quote, then the rest of the line will be read.
 * Adjacent quotes, \" will be intepreted as a single quote.
 * It returns false if it does not find a quoted string.
 */

BOOLN
ExtractQuotedString_ParFile(char *string, char *source)
{
	char	*p, *s = string;

	for (p = source; *p && isspace(*p); p++)
		;
	if (*p++ != '"')
		return(FALSE);
	while (*p && (*p != '"')) {
		if ((*p == '\\') && (*(p + 1) == '"'))
			p++;
		*s++ = *p++;
	}
	*s = '\0';
	return(TRUE);

}

/****************************** GetPars ***************************************/

/*
 * This function reads respective variables from a line in a file.  It uses a
 * format similar to the standard C printf statement.
 * If it fails then it returns FALSE.
 */
 
BOOLN
GetPars_ParFile(FILE *fp, char *fmt, ...)
{
	static const char *funcName = "GetPars_ParFile";
	static char	line[LONG_STRING], fmtScanLine[MAXLINE], extraFmt[MAXLINE];
	char	c, *formatToken, *restOfLine;
	int		formatType;
	va_list	args;
	
	if (!CheckInit_ParFile())
		exit(1);
	do {
		if (GetLine_ParFile(line, LONG_STRING, fp) == NULL) {
			c = (parFile->parLineCount > 1)? 's': ' ';
			if (parFile->emptyLineMessageFlag)
				NotifyError("%s: Premature end of parameter file!  Only %d "
				  "line%c read.", funcName, parFile->parLineCount, c);
			return(FALSE);
		}
	} while(CommentOrBlankLine_ParFile(line));
	for (restOfLine = line; isspace(*restOfLine); restOfLine++)
		; 
	parFile->parLineCount++;
	va_start(args, fmt);

	/* This next line is needed because strtok bashes its string argument. */
	CopyAndTrunc_Utility_String(fmtScanLine, fmt, MAXLINE);
	formatToken = strtok(fmtScanLine, FORMAT_DELIMITERS);
	while ((formatToken != NULL) && (restOfLine != NULL) &&
	  ((formatType = IdentifyFormat_ParFile(formatToken, extraFmt)) != EOF)) {
		switch (formatType) {
			case 'u':
				sscanf(restOfLine, formatToken, va_arg(args, unsigned long *));
				break;
			case 'd':
				sscanf(restOfLine, formatToken, va_arg(args, int *));
				break;
			case 'f':
				sscanf(restOfLine, formatToken, va_arg(args, double *));
				break;
			case 's':
				if (extraFmt[0] == 'l')
					strcpy(va_arg(args, char *), restOfLine);
				else {
					char	*string = va_arg(args, char *);
					if (!ExtractQuotedString_ParFile(string, restOfLine))
						sscanf(restOfLine, formatToken, string);
				}
				break;
			default:
				NotifyError("%s: Could not identify format string.", funcName);
				return(FALSE);
		}
		formatToken = strtok(NULL, FORMAT_DELIMITERS);
		/* This next section gets rid of the white space */
		for (restOfLine = strpbrk(restOfLine, FORMAT_DELIMITERS);
		  (restOfLine != NULL) && isspace(*restOfLine); restOfLine++)
			;
	}
	va_end(args);
	if (parFile->diagnosticsFlag)
		DPrint("GetPars_ParFile: Read line: '%s'\n", line);
	return(TRUE);

}
