/**********************
 *
 * File:		UtUPortableIO.c
 * Purpose:		
 * Comments:	03-09-93 LPO: Converted to ANSI C format, constructed header
 *				file and defined the int32 type which is used by the respective
 *				reading routines.
 *				19-01-95 LPO: Revised to enable writing from and reading to
 *				memory.
 *				07-04-98 LPO: Changed the "SetPosition_..." return to return a
 *				BOOLN response.
 * Author:		Malcolm Slaney
 * Revised by:	L. P. O'Mard
 * Created:		11 Dec 1992
 * Updated:		07 Apr 1998
 * Copyright:	(c) 1998, University of Essex
 * Copyright:	(c) 1988-1993 by Apple Computer, Inc. All Rights Reserved.
 *
 **********************/

/*
*				Apple Cochlear Models
*	 			 Malcolm Slaney
*				Advanced Technology Group
*				Apple Computer, Inc.
*				 malcolm@apple.com
*					1990-1993
*
*	Warranty Information
*	Even though Apple has reviewed this software, Apple makes no warranty
*	or representation, either express or implied, with respect to this
*	software, its quality, accuracy, merchantability, or fitness for a
*	particular purpose. As a result, this software is provided "as is,"
*	and you, its user, are assuming the entire risk as to its quality
*	and accuracy.
*
*	Copyright (c) 1988-1993 by Apple Computer, Inc
*		All Rights Reserved.
*
* $Header$
*
* $Log$
* Revision 1.7  2004/04/01 09:46:54  lowel
* Fix: Corrected a signed/unsigned comparison in "SetPosition_..." - compiler
* warning.
*
* Revision 1.6  2004/02/25 14:55:19  lowel
* Bug fixes:  The configure.ac script has been changed so that the size of long
* variables is checked.  This is required because it is different for 64 bit
* systems.  These changes are now taken advantage of by the UtUPortableIO code
* module.  Various cosmetic changes were also made to get rid of compiler
* warnings.
* Change: Version change to 2.7.6.
*
* Revision 1.5  2003/12/12 14:09:04  lowel
* Changes: I have implemented a more secure server operation.
* The changes have include the addition of the new Extensions library.
* I have had to update the UtUPortableIO code module for the use of 'memory files'
* These are files that are written to memory instead of to a file.
*
* Revision 1.4  2001/09/14 17:42:20  lowel
* Changes: these where required to get rid of compiler warnings when using
* Microsoft Visual C++.  One in particular was particularly useful, as I
* discovered that the UtIonChanList was declaring a real variable as an int.
*
* Revision 1.3  2001/07/23 14:36:39  lowel
* Compile error (bcc32): Removed the 'long long' type in Read32Bits... routines as
* bcc32 doesn't know about it.  The ifdef statements seem to only be required for
* Crays.
*
* Revision 1.2  2001/07/23 11:21:24  lowel
* Bug fix: The word-reading routines were mixing upper order arithmetic with
* lower order variables.  For instance, the 'Read16BitsHighLow' routine was doing
* 32 bit arithmetic using 16 bit variables.  This was causing problems under
* windows, but not under Linux.
*
* Revision 1.1  2001/07/13 14:31:49  lowel
* Changes: I have moved the respective files into the 'MainLib', "StdLib' and
* 'GrLib' directories.
*
* Revision 1.5  2001/06/12 12:48:44  lowel
* Cosmetic changes.
*
* Revision 1.4  2001/05/14 14:49:50  lowel
* Removed the '(i & 0x80)' code from the "Read8Bits' routine.  It was causing
* eight bit files to be read in incorrectly.  I never did fully understand what
* it was for anyway.  It was in Malcom Slaney's code.
*
* Revision 1.3  2000/11/22 18:39:52  lowel
* Changed the 'InitMemory_' routine so that it returns a pointer to the structure
* pointer rather then a BOOLN variable.
*
* Revision 1.2  2000/04/10 06:49:54  lowel
* Changed the names of the Ieee routines to IEEE.
*
* Revision 1.1.1.1  1999/09/15 14:48:55  lowel
* Imported DSAM source
*
* Revision 1.1.1.1  1999/09/14 15:52:27  lowel
* Imported DSAM source 
*
* Revision 1.1 1992/12/11 18:16:05 malcolm * Initial revision
*
*/

/* 
*
* This code may be used and freely distributed as long as it includes
* this copyright notice and the warranty information.
*
* Machine-independent I/O routines for 8-, 16-, 24-, and 32-bit integers.
*
* Motorola processors (Macintosh, Sun, Sparc, MIPS, etc)
* pack bytes from high to low (they are big-endian).
* Use the HighLow routines to match the native format
* of these machines.
*
* Intel-like machines (PCs, Sequent)
* pack bytes from low to high (they are little-endian).
* Use the LowHigh routines to match the native format
* of these machines.
*
* These routines have been tested on the following machines:
*	Apple Macintosh, MPW 3.1 C compiler
*	Apple Macintosh, THINK C compiler
*	Silicon Graphics IRIS, MIPS compiler
*	Cray X/MP and Y/MP
*	Digital Equipment VAX
*
*
* Implemented by Malcolm Slaney and Ken Turkowski.
*
* Malcolm Slaney contributions during 1988-1990 include big- and little-
* endian file I/O, conversion to and from Motorola's extended 80-bit
* floating-point format, and conversions to and from IEEE single-
* precision floating-point format.
*
* In 1991, Ken Turkowski implemented the conversions to and from
* IEEE double-precision format, added more precision to the extended
* conversions, and accommodated conversions involving +/- infinity,
* NaN's, and denormalized numbers.
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG_H */

#include "GeCommon.h"
#include "UtUIEEEFloat.h"
#include "UtUPortableIO.h"

/******************************************************************************/
/*************************** Global Variables *********************************/
/******************************************************************************/

/*
 * Pointer for storing data in memory.
 */

UPortableIOPtr	uPortableIOPtr = NULL;

/******************************************************************************/
/* Big/little-endian independent I/O routines. ********************************/
/******************************************************************************/

/*************************** ReadByte *****************************************/

/*
 * This routine reads a single byte (char) from either a file pointer, or
 * dataFile's memory pointer.
 * The data is read from memory if the memory pointer is not set to NULL, in
 * which case the file pointer is ignored.
 */

char
ReadByte(FILE *fp)
{
	return((uPortableIOPtr == NULL)? getc(fp): *uPortableIOPtr->memPtr++);

}

/*************************** WriteByte ****************************************/

/*
 * This routine writes a single byte (char) to either a file pointer, or
 * pointer.
 * The data is written to memory if the memory pointer is not set to NULL, in
 * which case the file pointer is ignored.
 */

void
WriteByte(char ch, FILE *fp)
{
	if (uPortableIOPtr == NULL)
		putc(ch, fp);
	else {
		*uPortableIOPtr->memPtr++ = ch;
		if (uPortableIOPtr->memPtr > uPortableIOPtr->eOFPtr)
			uPortableIOPtr->eOFPtr = uPortableIOPtr->memPtr;
	}

}

/*************************** ReadByte *****************************************/

int
Read8Bits(FILE *fp)
{
	int	result;

	result = ReadByte(fp) & 0xff;
	return result;
}


int16
Read16BitsLowHigh(FILE *fp)
{
	int16	first, second;
	int32	result;

	first = 0xff & ReadByte(fp);
	second = 0xff & ReadByte(fp);

	result = (second << 8) + first;
#ifndef	THINK_C
	if (result & 0x8000)
		result = result - 0x10000;
#endif	/* THINK_C */
	return((int16) result);
}

int16
Read16BitsHighLow(FILE *fp)
{
	int16	first, second;
	int32	result;

	first = ReadByte(fp) & 0xff;
	second = 0xff & ReadByte(fp);

	result = (first << 8) + second;
#	ifndef	THINK_C
	if (result & 0x8000)
		result = result - 0x10000;
#	endif	/* THINK_C */
	return((int16) result);
}


void
Write8Bits(FILE *fp, int i)
{
	WriteByte((char) (i & 0xff), fp);
}

void
Write16BitsLowHigh(FILE *fp, int16 i)
{
	WriteByte((char) (i & 0xff),fp);
	WriteByte((char) ((i >> 8) & 0xff),fp);
}


void
Write16BitsHighLow(FILE *fp, int16 i)
{
	WriteByte((char) ((i >> 8) & 0xff),fp);
	WriteByte((char) (i & 0xff),fp);
}


int32
Read24BitsHighLow(FILE *fp)
{
	int32	first, second, third, result;

	first = 0xff & ReadByte(fp);
	second = 0xff & ReadByte(fp);
	third = 0xff & ReadByte(fp);

	result = (first << 16) + (second << 8) + third;
 	if (result & 0x800000)
		result = result - 0x1000000;
	return(result);
}


int32
Read32BitsLowHigh(FILE *fp)
{
	int32	first, second, result;

	first = 0xffff & Read16BitsLowHigh(fp);
 	second = 0xffff & Read16BitsLowHigh(fp); 

	result = (second << 16) + first;
#ifdef	CRAY
	if (result & 0x80000000)
		result = result - 0x100000000;
#endif	/* CRAY */
	return(result);
}


int32
Read32BitsHighLow(FILE *fp)
{
	int32	first, second, result;

	first = 0xffff & Read16BitsHighLow(fp);
 	second = 0xffff & Read16BitsHighLow(fp); 

	result = (first << 16) + second;
#ifdef	CRAY
	if (result & 0x80000000)
		result = result - 0x100000000;
#endif
	return(result);
}


void
Write32Bits(FILE *fp, int32 i)
{
	Write16BitsLowHigh(fp,(int16)(i&0xffffL));
	Write16BitsLowHigh(fp,(int16)((i>>16)&0xffffL));
}


void
Write32BitsLowHigh(FILE *fp, int32 i)
{
	Write16BitsLowHigh(fp,(int16)(i&0xffffL));
	Write16BitsLowHigh(fp,(int16)((i>>16)&0xffffL));
}


void
Write32BitsHighLow(FILE *fp, int32 i)
{
	Write16BitsHighLow(fp,(int16)((i>>16)&0xffffL));
	Write16BitsHighLow(fp,(int16)(i&0xffffL));
}

void
ReadBytes(FILE	 *fp, char	 *p, int	 n)
{
	while (!feof(fp) && (n-- > 0))
		*p++ = ReadByte(fp);

}

void ReadBytesSwapped(FILE	 *fp, char	 *p, int	 n)
{
	register char	*q = p;

	while (!feof(fp) && (n-- > 0))
		*q++ = ReadByte(fp);

	for (q--; p < q; p++, q--){
		n = *p;
		*p = *q;
		*q = n;
	}
}

void WriteBytes(FILE	 *fp, char	 *p, int	 n)
{
	while (n-- > 0)
		WriteByte(*p++, fp);
}

void WriteBytesSwapped(FILE	 *fp, char	 *p, int	 n)
{
	p += n-1;
	while (n-- > 0)
		WriteByte(*p--, fp);
}

DefDouble
ReadIEEEFloatHighLow(FILE *fp)
{
	char	bits[kFloatLength];

	ReadBytes(fp, bits, kFloatLength);
	return ConvertFromIEEESingle(bits);
}

DefDouble
ReadIEEEFloatLowHigh(FILE *fp)
{
	char	bits[kFloatLength];

	ReadBytesSwapped(fp, bits, kFloatLength);
	return ConvertFromIEEESingle(bits);
}

DefDouble
ReadIEEEDoubleHighLow(FILE *fp)
{
	char	bits[kDoubleLength];

	ReadBytes(fp, bits, kDoubleLength);
	return ConvertFromIEEEDouble(bits);
}

DefDouble
ReadIEEEDoubleLowHigh(FILE *fp)
{
	char	bits[kDoubleLength];

	ReadBytesSwapped(fp, bits, kDoubleLength);
	return ConvertFromIEEEDouble(bits);
}

DefDouble
ReadIEEEExtendedHighLow(FILE *fp)
{
	char	bits[kExtendedLength];

	ReadBytes(fp, bits, kExtendedLength);
	return ConvertFromIEEEExtended(bits);
}

DefDouble
ReadIEEEExtendedLowHigh(FILE *fp)
{
	char	bits[kExtendedLength];

	ReadBytesSwapped(fp, bits, kExtendedLength);
 	return ConvertFromIEEEExtended(bits);
}

void
WriteIEEEFloatLowHigh(FILE *fp, DefDouble num)
{
	char	bits[kFloatLength];

	ConvertToIEEESingle(num,bits);
	WriteBytesSwapped(fp,bits,kFloatLength); }

void
WriteIEEEFloatHighLow(FILE *fp, DefDouble num)
{
	char	bits[kFloatLength];

	ConvertToIEEESingle(num,bits);
	WriteBytes(fp,bits,kFloatLength);
}

void
WriteIEEEDoubleLowHigh(FILE *fp, DefDouble num)
{
	char	bits[kDoubleLength];

	ConvertToIEEEDouble(num,bits);
	WriteBytesSwapped(fp,bits,kDoubleLength); }

void
WriteIEEEDoubleHighLow(FILE *fp, DefDouble num)
{
	char	bits[kDoubleLength];

	ConvertToIEEEDouble(num,bits);
	WriteBytes(fp,bits,kDoubleLength);
}

void
WriteIEEEExtendedLowHigh(FILE *fp, DefDouble num)
{
	char	bits[kExtendedLength];

	ConvertToIEEEExtended(num,bits);
	WriteBytesSwapped(fp,bits,kExtendedLength); }


void
WriteIEEEExtendedHighLow(FILE *fp, DefDouble num)
{
	char	bits[kExtendedLength];

	ConvertToIEEEExtended(num,bits);
	WriteBytes(fp,bits,kExtendedLength);
}

/**************************** FreeMemory **************************************/

/*
 * This routine frees the memory allocated for memory reading.
 */

void
FreeMemory_UPortableIO(UPortableIOPtr *p)
{
	if (*p == NULL)
		return;
	if ((*p)->memStart != NULL)
		free((*p)->memStart);
	free((*p));
	(*p) = NULL;

}

/**************************** InitMemory **************************************/

/*
 * This routine initialises the memory pointers module parameters to values.
 * It returns a pointer to the structures memory if successful, otherwise it
 * returns NULL.
 */

BOOLN
InitMemory_UPortableIO(UPortableIOPtr *p, ChanLen length)
{
	if (!(*p)) {
		if (((*p) = (UPortableIOPtr) malloc(sizeof(UPortableIO))) == NULL) {
			fprintf(stderr, "InitMemory_UPortableIO: Cannot allocate memory "
			  "for module pointer.");
			return(FALSE);
		}
		(*p)->length = 0;
		(*p)->memStart = NULL;
	}
	if ((*p)->length != length) {
		if ((*p)->memStart) {
			free((*p)->memStart);
			(*p)->memStart = NULL;
		}
		if (((*p)->memStart = (char *) malloc(length)) == NULL) {
			fprintf(stderr, "InitMemory_UPortableIO: Cannot allocate data "
			  "memory.");
			free((*p));
			*p = NULL;
			return(FALSE);
		}
	}
	(*p)->memPtr = (*p)->eOFPtr = (*p)->memStart;
	(*p)->length = length;
	return(TRUE);

}

/**************************** GetPosition *************************************/

/*
 * This routine replaces the standard "ftell" command.
 * For a file, it returns the result from ftell.
 * For memory operations (uPortableIOPtr != NULL) the offset from the start of
 * the data in memory.
 */

int32
GetPosition_UPortableIO(FILE *fp)
{
	if (uPortableIOPtr == NULL)
		return((int32) ftell(fp));
	return(uPortableIOPtr->memPtr - uPortableIOPtr->memStart);

}

/**************************** SetPosition *************************************/

/*
 * This routine replaces the standard "fseek" command.
 * For a file, it returns the result from fseek.
 * For memory operations (uPortableIOPtr != NULL) the offset from the start of
 * the data in memory.
 * SetPosition_UPortableIO(fp, OL, SEEK_SET) == rewind(fp).
 */

BOOLN
SetPosition_UPortableIO(FILE *fp, int32 offset, int whence)
{
	static const char *funcName = "SetPosition_UPortableIO";

	if (uPortableIOPtr == NULL) {
		if (fseek(fp, offset, whence) != -1 )
			return(TRUE);
		else {
			fprintf(stderr, "%s: Could not set file position.\n", funcName);
			return(FALSE);
		}
	}
	switch (whence) {
	case SEEK_SET:
		if (offset > (int32) uPortableIOPtr->length) {
			fprintf(stderr, "%s: Attempt to read past end of memory.\n",
			  funcName);
			return(FALSE);
		}
		uPortableIOPtr->memPtr = uPortableIOPtr->memStart + offset;
		return(TRUE);
	case SEEK_CUR:
		if (uPortableIOPtr->memPtr + offset > uPortableIOPtr->memStart +
		  uPortableIOPtr->length) {
			fprintf(stderr, "%s: Attempt to read past the end of memory.\n",
			  funcName);
			fprintf(stderr, "%s: offset = %d\n", funcName, offset);
			fprintf(stderr, "%s: difference = %ld\n", funcName,
			  uPortableIOPtr->memPtr +
			  offset - uPortableIOPtr->memStart + uPortableIOPtr->length);
			return(FALSE);
		}
		uPortableIOPtr->memPtr += offset;
		return(TRUE);
	case SEEK_END:
		if ((offset + uPortableIOPtr->eOFPtr) > (uPortableIOPtr->memStart + 
		  uPortableIOPtr->length)) {
			fprintf(stderr, "%s: Attempt to read past the end of memory.\n",
			  funcName);
			return(FALSE);
		}
		if ((uPortableIOPtr->eOFPtr - offset) < uPortableIOPtr->memStart) {
			fprintf(stderr, "%s: Attempt to read past the start of memory.\n",
			  funcName);
			return(FALSE);
		}
		uPortableIOPtr->memPtr = uPortableIOPtr->eOFPtr + offset;
		return(TRUE);
	default:
		fprintf(stderr, "%s: Illegal relative position.", funcName);
		return(FALSE);
	} /* switch */

}
