/**********************
 *
 * File:		Strings.c
 * Purpose: 	Module parser string handling routines.
 * Comments:	
 * Author:		L.P.O'Mard
 * Created:		7 Nov 1995
 * Updated:	
 * Copyright:	(c) 1998, University of Essex
 *
 **********************/

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "MGGeneral.h"
#include "Strings.h"

/******************************************************************************/
/****************************** Gobal variables *******************************/
/******************************************************************************/

/******************************************************************************/
/****************************** Function Declarations *************************/
/******************************************************************************/

/****************************** RealStringLength ******************************/

/*
 * This calculates the real length of a string, taking into account any
 * tab characters which may be present.
 */

int
RealStringLength(char *string)
{
	char 	*p;
	int		length;
	
	for (p = string, length = 0; *p != '\0'; p++)
		length += (*p == '\t')? TAB_SPACES(length): 1;
	return(length);

}

/****************************** AddChar ***************************************/

/*
 * This routine adds a character to a string.
 * It will expand tabs as required.
 * It expects the string to have the necessary space allocated.
 * The position pointer, passed as an argument, is incremented.
 * The pointer position at which the character was placed is returned.
 */

char *
AddChar(char *string, char c, int *pos, BOOLN stringControlChars)
{
	char	*p, *cPtr;

	for (p = string; *p != '\0'; p++)
		;
	cPtr = p;
	if (!iscntrl(c)) {
		*p++ = c;
		*p = '\0';
		(*pos)++;
		return(cPtr);
	}
	if (!stringControlChars) {
		*p++ = c;
		(*pos) += (c == '\t')? TAB_SPACES(*pos): 1;
	} else {
		*p++ = '\\';
		switch (c) {
		case '\t':
			*p++ = 't';
			break;
		case '\n':
			*p++ = 'n';
			break;
		default:
			execerror("AddChar: Unknown control char", "");
		}
		(*pos) += 2;
	}
	*p = '\0';
	return(cPtr);

}

/****************************** PrintAndAdjustLine ****************************/

/*
 * This prints a specified number of characters from a string, starting from
 * the first position. It then moves the remaining characters up to the start,
 * after adding any required prefix
 * It returns the number of characters remaining in the string;
 */

int
PrintAndAdjustLine(FILE *fp, char *prefix, char *string, char *eol,
  BOOLN clearLeadingSpace)
{
	int	numChars = 0;
	char	*p, *pp;

	for (p = string; (*p != '\0') && (p < eol); p++)
		fputc(*p, fp);
	if (*p == '\0')
		return 0;
	if (clearLeadingSpace)
		for (; (*p != '\0') && isspace(*p); p++)
			;
	for (*string = '\0'; *prefix != '\0'; prefix++)
		pp = AddChar(string, *prefix, &numChars, FALSE);
	for (; *p != '\0'; p++)
			AddChar(string, *p, &numChars, FALSE);
	return(numChars);

}

/****************************** Print *****************************************/

/*
 * This routine concatinates and prints lines.  If a line is over MAXLINE
 * characters long then the line will be truncated at an appropriate place and
 * excessive length is put onto successive lines.
 * If the string is "" then any remaining lines in the buffer will be printed
 * (flushed).
 * At present, lines are separated at spaces, commas, open brackets ("(") or 
 * equals ("=").
 * Special consideration is given to quoted lines which are divided over
 * successive lines.
 * Quoted strings within quoted string must use the sequence '_\"'f for quotes
 " and '_\n' for newline.
 * The prefix is any string which should preceed each succesive line, e.g. tabs.
 * At present page breaks should not occur over lines.
 */

void
Print(FILE *fp, char *prefix, char *string)
{
	static BOOLN	firstLine, nestedQuotes;
	static char	*p, *lp, *eol, *quote, *punctuation, line[MAXLINE] = "";
	static char	temp[MAXLINE];
	static int	quoteCount = 0, numChars, space;

	if ((*string == '\0') && (*line != '\0')) {
		fprintf(fp, "%s", line);
		*line = '\0';
		return;
	}
	if (*line == '\0') {
		punctuation = eol = quote = 0;
		numChars = quoteCount = 0;
		firstLine = TRUE;
	}
	for (p = string; (*p != '\0'); p++) {
		switch (*p) {
		case '_':
			if (*(p + 1) == '"') {
				nestedQuotes = !nestedQuotes;
				p++;
			}
			break;
		case '"':
			quoteCount++;
			break;
		default:
			;
		} /* Switch */
		if (!nestedQuotes )
			switch (*p) {
			case '\n':	space = 0; break;
			case '\t':	space = TAB_SPACES(numChars); break;
			default:	space = 1; break;
			} /* switch */
		else if (iscntrl(*p))
				space = 2;
		if (numChars + space < MAXLINE - 2) {
			lp = AddChar(line, *p, &numChars, nestedQuotes);
			switch (*p) {
			case '"':
				quote = lp;
				break;
			case '=':
				if (*(p - 1) == '=')
					punctuation = lp;
				break;
			case ',':
			case '(':
			case '[':
			case '>':
				punctuation = lp;
				break;
			case '.':
			case ' ':
			case '\t':
				eol = lp;
				break;
			default:
				;
			} /* Switch */
		} else {
			if (((quoteCount % 2) == 1) && (quote < lp)) {
				sprintf(temp, "%s\"", (firstLine)? "": prefix);
				numChars = PrintAndAdjustLine(fp, temp, line, eol, FALSE);
				fprintf(fp, "\"");
			} else {
				if (punctuation && (*(punctuation + 1) != '\0'))
					eol = punctuation + 1;
				sprintf(temp, "%s", (firstLine)? "": prefix);
				numChars = PrintAndAdjustLine(fp, temp, line, eol, TRUE);
			}
			fprintf(fp, "\n");
			AddChar(line, *p, &numChars, nestedQuotes);
		}
		firstLine = FALSE;
	}

}

/****************************** SwapP *****************************************/

/*
 * This routine interchanges two pointers.
 */

void
SwapP(char *v[], int i, int j)
{
	char	*temp;
	
	temp = v[i];
	v[i] = v[j];
	v[j] = temp;

}

/****************************** StrBreak **************************************/

/*
 * This routine returns a pointer to the point in the character after the Nth
 * delimitered space.
 */

char *
StrBreak(char *string, int n, const char *delimiters)
{
	static char *funcName = "StrBreak";
	char	*p, *copy, *pos, *result;
	int		i;

	if (n == 0)
		return (string);
	if ((copy = strdup(string)) == NULL)
		execerror("Out of memory for string copy", funcName);
	for (i = 0, p = strtok(copy, delimiters); (i < n) && (p != NULL); i++)
		p = strtok(NULL, delimiters);
	result = (p == NULL)? string: string + (p - copy);
	free(copy);
	return(result);

}

/****************************** QuickSortP ************************************/

/*
 * This routine does a quick sort of an array of pointers to strings.
 * It sorts v[left]...v[right] into increasing order.
 */

void
QuickSortP(char *v[], int left, int right)
{
	int		i, last;
	
	if (left >= right)	/* do nothing if  less than two elements. */
		return;
	SwapP(v, left, (left + right) / 2);
	last = left;
	for (i = left + 1; i <= right; i++)
		if (strcmp(v[i], v[left]) < 0)
			SwapP(v, ++last, i);
	SwapP(v, left, last);
	QuickSortP(v, left, last - 1);
	QuickSortP(v, last + 1, right);

}

/****************************** QuickSortNP ***********************************/

/*
 * This routine does a quick sort of an array of pointers to strings.
 * It sorts v[left]...v[right] into increasing order.
 * The string after the Nth delimitered 'space' is used for sorting.
 */

void
QuickSortNP(char *v[], int n, const char *delimiters, int left, int right)
{
	char	*vI, *vLeft;
	int		i, last;
	
	if (left >= right)	/* do nothing if  less than two elements. */
		return;
	SwapP(v, left, (left + right) / 2);
	last = left;
	vLeft = StrBreak(v[left], n, delimiters);
	for (i = left + 1; i <= right; i++) {
		vI = StrBreak(v[i], n, delimiters);
		if (strcmp(vI, vLeft) < 0)
			SwapP(v, ++last, i);
	}
	SwapP(v, left, last);
	QuickSortNP(v, n, delimiters, left, last - 1);
	QuickSortNP(v, n, delimiters, last + 1, right);

}

/**************************** GetExtension ************************************/

/*
 * This routine returns the suffix of a file name i.e. any characters
 * after a ".".
 * It returns the entire file name if no suffix is returned.
 * 
 */
 
char *
GetExtension(char *fileName)
{
	static char scanLine[MAXLINE]; /* - must be static so that memory is kept.*/
	char	*token;
	
	strcpy(scanLine, fileName);	/* Remember, strtok clobbers scanLine. */
	token = strtok(scanLine, ".");
	token = strtok(NULL, " ");
	if (token != NULL)
		return(token);
	else 
		return(fileName);
		
}

/****************************** StripFileExtension ****************************/

/*
 * This routine removes the specified extension from a file name.
 * It returns NULL if it does not find the extension.
 */

char *
StripFileExtension(char *fileName, char *extension)
{
	static	char baseFileName[MAXLINE];
	char	*p, *pp;
	
	if (strcmp(GetExtension(fileName), extension) != 0)
		return(NULL);
	for (p = fileName, pp = baseFileName; (*p != '\0') && (*p != '.'); )
		*pp++ = *p++;
	*pp = '\0';
	return(baseFileName);

}

/****************************** UpperCase *************************************/

/*
 * This function routines a pointer to an uppercase copy of the string passed
 * as an argument.
 */

char *
UpperCase(char *string)
{
	static char* funcName = "UpperCase";
	static char	newString[MAXLINE];
	char	*p1, *p2;

	if (strlen(string) >= MAXLINE)
		execerror("String too long", funcName);
	for (p1 = newString, p2 = string; *p2 != '\0'; )
		*p1++ = toupper(*p2++);
	*p1 = '\0';
	return newString;

}

/****************************** Capital ***************************************/

/*
 * This function routines a pointer to a capitalised copy of the string passed
 * as an argument.
 */

char *
Capital(char *string)
{
	static char* funcName = "Capital";
	static char	newString[MAXLINE];
	char	*p1, *p2;

	if (strlen(string) >= MAXLINE)
		execerror("String too long", funcName);
	strcpy(newString, string);
	newString[0] = toupper(newString[0]);
	return newString;

}

/****************************** PluralToSingular ******************************/

/*
 * This routine converts a plural suffix to singular.
 */

char *
PluralToSingular(char *string)
{
	static char* funcName = "Capital";
	static char	newString[MAXLINE];
	char	*stringSuffix;
	int		i, stringLen, suffixLen;
	struct	conversion {
		char *plural, *singular;
	} convTable[] = {
		"IES", "Y",
		"ies", "y",
		"S", "",
		"s", "",
		"", ""
	};
	
	if (strlen(string) >= MAXLINE)
		execerror("String too long", funcName);
	strcpy(newString, string);
	if ((stringLen = strlen(string)) == 0)
		return(newString);
	for (i = 0; convTable[i].plural[0] != '\0'; i++) {
		stringSuffix = newString + (stringLen - strlen(convTable[i].plural));
		if (strcmp(stringSuffix, convTable[i].plural) == 0) {
			strcpy(stringSuffix, convTable[i].singular);
			break;
		}
	}
	return(newString);

}

/**************************** GetFileNameFPath ********************************/

/*
 * This routine returns the filename with the path removed.
 * It returns the entire file name if there is no file path.
 * 
 */
 
char *
GetFileNameFPath(char *fileName)
{
	char	*p;

	p = strrchr(fileName, '/');
	if (!p)
		p = strrchr(fileName, '\\');
	if (p == NULL)
		return(fileName);
	return(++p);

}

