/***********************
 *
 * UtUIEEEFloat.h
 *
 * 3-9-93 L.P.O'Mard: Converted to ANSI C format, constructed header file
 *   and defined the int32 type which is used by the respective reading routines.
 *
 ***********************/

#ifndef	_UTUIEEEFLOAT_H
#define _UTUIEEEFLOAT_H	1
 
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
* Machine-independent I/O routines for IEEE floating-point numbers. 
*
* NaN's and infinities are converted to HUGE_VAL or HUGE, which 
* happens to be infinity on IEEE machines. Unfortunately, it is 
* impossible to preserve NaN's in a machine-independent way. 
* Infinities are, however, preserved on IEEE machines. 
*
* These routines have been tested on the following machines: 
*	Apple Macintosh, MPW 3.1 C compiler
*	Apple Macintosh, THINK C compiler
*	Silicon Graphics IRIS, MIPS compiler
*	Cray X/MP and Y/MP
*	Digital Equipment VAX
*	Sequent Balance (Multiprocesor 386)
*	NeXT
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
* $Header$ *
* $Log$
* Revision 1.3  2001/06/12 13:01:32  lowel
* Cosmetic changes.
*
* Revision 1.2  2000/04/10 06:49:49  lowel
* Changed the names of the Ieee routines to IEEE.
*
* Revision 1.1.1.1  1999/09/15 14:49:03  lowel
* Imported DSAM source
*
* Revision 1.1.1.1  1999/09/14 15:52:34  lowel
* Imported DSAM source 
*
* Revision 1.1 1992/12/11 18:16:05 malcolm * Initial revision
*
*/

/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

#define	UIEEE_FLOAT_EXTENDED_LEN	10	/* No. of bytes per extended value. */

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

typedef	double	DefDouble;

typedef	double	Double;

typedef	float	Single;

/******************************************************************************/
/*************************** External variables *******************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Subroutine declarations **************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

void		Bytes2Hex(register char *bytes, char *hex, register int nBytes);

void		ConvertToIEEEDouble(DefDouble num, char *bytes);

void		ConvertToIEEEExtended(DefDouble num, char *bytes);

void		ConvertToIEEESingle(DefDouble num, char *bytes);

void		Hex2Bytes(register char *hex, char *bytes);

void		MaybeSwapBytes(char *bytes, int nBytes);

void		SignalFPE(int i);

void		TestFromIEEEDouble(char *hex);

void		TestFromIEEEExtended(char *hex);

void		TestFromIEEESingle(char *hex);

void		TestToIEEEDouble(DefDouble f);

void		TestToIEEEExtended(DefDouble f);

void		TestToIEEESingle(DefDouble f);

int			GetHexSymbol(register int x);

int			GetHexValue(register int x);

float		MachineIEEESingle(char *bytes);

double		MachineIEEEDouble(char *bytes);

DefDouble	ConvertFromIEEEDouble(char *bytes);

DefDouble	ConvertFromIEEEExtended(char *bytes);

DefDouble	ConvertFromIEEESingle(char *bytes);

__END_DECLS

#endif
