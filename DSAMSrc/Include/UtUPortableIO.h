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

#ifndef	_UTUPORTABLEIO_H
#define _UTUPORTABLEIO_H	1

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
*
	or representation, either express or implied, with respect to this 
	*	software, its quality, accuracy, merchantability, or fitness for a 
	*	particular purpose. As a result, this software is provided "as is," 
	*	and you, its user, are assuming the entire risk as to its quality 
	*	and accuracy.
*
*	Copyright (c) 1988-1993 by Apple Computer, Inc 
*		All Rights Reserved.
*
* $Header$ *
* $Log$
* Revision 1.4  2000/11/22 18:45:09  lowel
* The 'InitMemory_UPortableIO' was changed to return a pointer to the module
* structure created, rather than a BOOLN variable.
*
* Revision 1.3  2000/04/10 06:55:35  lowel
* Changed the Ieee routines to IEEE
*
* Revision 1.2  1999/10/06 16:45:08  lowel
* Removed the Makefile.in files
* The GUI interface now compiles, but there are still errors, and the
* module parameter interface is to be recoded.
*
* Revision 1.1.1.1  1999/09/15 14:49:02  lowel
* Imported DSAM source
*
* Revision 1.1.1.1  1999/09/14 15:52:33  lowel
* Imported DSAM source 
*
* Revision 1.1 1992/12/11 18:16:05 malcolm * Initial revision
*
*/
/* Copyright (C) 1988-1991 Apple Computer, Inc. 
* All Rights Reserved.
*
* Warranty Information
* Even though Apple has reviewed this software, Apple makes no warranty 
* or representation, either express or implied, with respect to this 
* software, its quality, accuracy, merchantability, or fitness for a 
* particular purpose. As a result, this software is provided "as is," 
* and you, its user, are assuming the entire risk as to its quality 
* and accuracy.
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
* pack bytes from low to high (the are little-endian). 
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

/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

#ifndef SEEK_SET

#	define SEEK_SET		0
#	define SEEK_CUR		1
#	define SEEK_END		2

#endif
#ifndef FALSE
#	define	FALSE		0
#	define	TRUE		0xFFFF
#endif

#define	kFloatLength	sizeof(float)
#define	kDoubleLength	sizeof(double)
#define kExtendedLength	10				/* Try this value from old code. */

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

#if !defined(BOOLEAN_ALGEBRA)
#	define BOOLEAN_ALGEBRA
	typedef	int	BOOLN;
#endif

typedef	 short	int16;

typedef  long	int32;

typedef struct {

	char	*memStart;			/* Points to the start of data in memory. */
	char	*memPtr;			/* Points to the current reading position. */
	int32	length;

} UPortableIO, *UPortableIOPtr;

/******************************************************************************/
/*************************** External Variables *******************************/
/******************************************************************************/

extern	UPortableIO	uPortableIO, *uPortableIOPtr;

/******************************************************************************/
/*************************** Subroutine declarations **************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

void		FreeMemory_UPortableIO(void);

int32		GetPosition_UPortableIO(FILE *fp);

UPortableIOPtr	InitMemory_UPortableIO(int32 length);

char		ReadByte(FILE *fp);

void		ReadBytes(FILE *fp, char *p, int n);

void		ReadBytesSwapped(FILE *fp, char	 *p, int	 n);

BOOLN		SetPosition_UPortableIO(FILE *fp, int32 offset, int whence);

void		Write16BitsHighLow(FILE *fp, int16 i);

void		Write16BitsLowHigh(FILE *fp, int16 i);

void		Write32Bits(FILE *fp, int32 i);

void		Write32BitsHighLow(FILE *fp, int32 i);

void		Write32BitsLowHigh(FILE *fp, int32 i);

void		Write8Bits(FILE *fp, int i);

void		WriteByte(char ch, FILE *fp);

void		WriteBytes(FILE	 *fp, char	 *p, int	 n);

void		WriteBytesSwapped(FILE	 *fp, char	 *p, int	 n);

void		WriteIEEEDoubleHighLow(FILE *fp, DefDouble num);

void		WriteIEEEDoubleLowHigh(FILE *fp, DefDouble num);

void		WriteIEEEExtendedHighLow(FILE *fp, DefDouble num);

void		WriteIEEEExtendedLowHigh(FILE *fp, DefDouble num);

void		WriteIEEEFloatHighLow(FILE *fp, DefDouble num);

void		WriteIEEEFloatLowHigh(FILE *fp, DefDouble num);

int16		Read16BitsLowHigh(FILE *fp);

int16		Read16BitsHighLow(FILE *fp);

int32		Read24BitsHighLow(FILE *fp);

int32		Read32BitsHighLow(FILE *fp);

int32		Read32BitsLowHigh(FILE *fp);

int			Read8Bits(FILE *fp);

DefDouble	ReadIEEEDoubleHighLow(FILE *fp);

DefDouble	ReadIEEEDoubleLowHigh(FILE *fp);

DefDouble	ReadIEEEExtendedHighLow(FILE *fp);

DefDouble	ReadIEEEExtendedLowHigh(FILE *fp);

DefDouble	ReadIEEEFloatHighLow(FILE *fp);

DefDouble	ReadIEEEFloatLowHigh(FILE *fp);

__END_DECLS

#endif
