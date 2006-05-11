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
* Revision 1.1  2001/07/13 14:31:49  lowel
* Changes: I have moved the respective files into the 'MainLib', "StdLib' and
* 'GrLib' directories.
*
* Revision 1.5  2001/06/12 12:47:23  lowel
* Cosmetic changes.
*
* Revision 1.4  2000/04/10 06:49:53  lowel
* Changed the names of the Ieee routines to IEEE.
*
* Revision 1.3  1999/12/21 16:29:54  lowel
* Corrected som casts and typing issues complained about by the SGI C compiler
*
* Revision 1.2  1999/10/28 07:13:44  lowel
* Various changes which now allow the correct appearance of module parameter
* files.
* The ShowSignal_SignalDisp routine now calls the 'Refresh()' ruotine rather than
* the 'OnPaint' routine otherwise the wxMSW port complains during running.
*
* Revision 1.1.1.1  1999/09/15 14:48:56  lowel
* Imported DSAM source
*
* Revision 1.1.1.1  1999/09/14 15:52:28  lowel
* Imported DSAM source 
*
* Revision 1.1 1992/12/11 18:16:05 malcolm 
* Initial revision
*
*/

#include	<stdio.h>
#include	<math.h>

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG_H */

#include	"GeCommon.h"
#include	"UtUIEEEFloat.h"

/**************************************************************** 
* The following two routines make up for deficiencies in many 
* compilers to convert properly between unsigned integers and 
* floating-point. Some compilers which have this bug are the 
* THINK_C compiler for the Macintosh and the C compiler for the 
* Silicon Graphics MIPS-based Iris.
****************************************************************/ 

#if defined(applec) | defined(BORLANDC)	/* The Apple C compiler works */
# define FloatToUnsigned(f)	((unsigned long)(f))
# define UnsignedToFloat(u)	((DefDouble)(u))
#else /* applec */
# define FloatToUnsigned(f)	((unsigned long)(((long)((f) - 2147483648.0)) + \
  2147483647L + 1))
# define UnsignedToFloat(u)	(((DefDouble)((long)((u) - 2147483647L - 1))) + \
  2147483648.0)
#endif /* applec */


/**************************************************************** 
* Single precision IEEE floating-point conversion routines ****************************************************************/ 

#define SEXP_MAX		255
#define SEXP_OFFSET		127L
#define SEXP_SIZE		8
#define SEXP_POSITION	(32-SEXP_SIZE-1)


DefDouble
ConvertFromIEEESingle(char *bytes)
{
	DefDouble	f;
	unsigned long	mantissa, expon;
	unsigned long	bits;

	bits = ((unsigned long)(bytes[0] & 0xFF) << 24) | ((unsigned long)(bytes[
	  1] & 0xFF) << 16) | ((unsigned long)(bytes[2] & 0xFF) << 8) |
	 (unsigned long)(bytes[3] & 0xFF);		/* Assemble bytes into a long */

	if ((bits & 0x7FFFFFFFL) == 0) {
		f = 0;
	}

	else {
		expon = (bits & 0x7F800000L) >> SEXP_POSITION;
		if (expon == SEXP_MAX) {		/* Infinity or NaN */
			f = HUGE_VAL;		/* Map NaN's to infinity */
		} else {
			if (expon == 0) {	/* Denormalized number */
				mantissa = (bits & 0x7fffffL);
				f = ldexp((DefDouble)mantissa, (int) (expon - SEXP_OFFSET -
				SEXP_POSITION + 1.0));
			} else {				/* Normalized number */
				mantissa = (bits & 0x7fffffL) + 0x800000L;	/* Insert hidden bit */
				f = ldexp((DefDouble)mantissa, (int) (expon - SEXP_OFFSET -
				  SEXP_POSITION));
			}
		}
	}

	if (bits & 0x80000000L)
		return -f;
	else
		return f;
}


/****************************************************************/


void
ConvertToIEEESingle(DefDouble num, char *bytes)
{
	long	sign;
	register long bits;

	if (num < 0) {	/* Can't distinguish a negative zero */
		sign = 0x80000000L;
		num *= -1;
	} else {
		sign = 0;
	}

	if (num == 0) {
		bits = 0;
	}

	else {
		DefDouble fMant;
		int expon;

		fMant = frexp(num, &expon);

		if ((expon > (SEXP_MAX-SEXP_OFFSET+1)) || !(fMant < 1)) {
			/* NaN's and infinities fail second test */
			bits = sign | 0x7F800000L;		/* +/- infinity */
		} else {
			long mantissa;

			if (expon < -(SEXP_OFFSET - 2L)) {	/* Smaller than normalized */
				int shift = (SEXP_POSITION+1) + (SEXP_OFFSET-2) + expon;
				if (shift < 0) {	/* Way too small: flush to zero */
					bits = sign;
				} else {			/* Nonzero denormalized number */
					mantissa = (long)(fMant * (1L << shift));
					bits = sign | mantissa;
				}
			} else {				/* Normalized number */
				mantissa = (long)floor(fMant * (1L << (SEXP_POSITION+1)));
				mantissa -= (1L << SEXP_POSITION);			/* Hide MSB */
				bits = sign | ((long)((expon + SEXP_OFFSET - 1)) << SEXP_POSITION) |
				mantissa;
			}
		}
	}

	bytes[0] = (char) (bits >> 24);	/* Copy to byte string */ 
	bytes[1] = (char) (bits >> 16);
	bytes[2] = (char) (bits >> 8);
	bytes[3] = (char) bits;

}


/**************************************************************** 
* Double precision IEEE floating-point conversion routines ****************************************************************/ 

#define DEXP_MAX		2047
#define DEXP_OFFSET		1023
#define DEXP_SIZE		11
#define DEXP_POSITION	(32-DEXP_SIZE-1)


DefDouble
ConvertFromIEEEDouble(char *bytes)
{
	DefDouble	f;
	long	mantissa, expon;
	unsigned long first, second;

	first = ((unsigned long)(bytes[0] & 0xFF) << 24) |
	  ((unsigned long)(bytes[1] & 0xFF) << 16) |
	  ((unsigned long)(bytes[2] & 0xFF) << 8) |
	  (unsigned long)(bytes[3] & 0xFF);
	second = ((unsigned long)(bytes[4] & 0xFF) << 24) |
	  ((unsigned long)(bytes[5] & 0xFF) << 16) |
	  ((unsigned long)(bytes[6] & 0xFF) << 8) |
	  (unsigned long)(bytes[7] & 0xFF);
	
	if (first == 0 && second == 0) {
		f = 0;
	}

	else {
		expon = (long) ((first & 0x7FF00000L) >> DEXP_POSITION);
		if (expon == DEXP_MAX) {		/* Infinity or NaN */
			f = HUGE_VAL;		/* Map NaN's to infinity */
		} else {
			if (expon == 0) {	/* Denormalized number */
				mantissa = (long) (first & 0x000FFFFFL);
				f = ldexp((DefDouble)mantissa, expon - DEXP_OFFSET -
				  DEXP_POSITION + 1);
				f += ldexp(UnsignedToFloat(second), expon - DEXP_OFFSET -
				  DEXP_POSITION + 1 - 32);
			} else {				/* Normalized number */
				mantissa = (long) (first & 0x000FFFFFL) + 0x00100000L;	/* Insert hidden bit */
				f = ldexp((DefDouble)mantissa, expon - DEXP_OFFSET - DEXP_POSITION);
				f += ldexp(UnsignedToFloat(second), expon - DEXP_OFFSET -
				  DEXP_POSITION - 32); 			}
		}
	}

	if (first & 0x80000000L)
		return -f;
	else
		return f;
}


/****************************************************************/ 


void
ConvertToIEEEDouble(DefDouble num, char *bytes)
{
	long	sign;
	unsigned long	first, second;

	if (num < 0) {	/* Can't distinguish a negative zero */
		sign = 0x80000000L;
		num *= -1;
	} else {
		sign = 0;
	}

	if (num == 0) {
		first = 0;
		second = 0;
	}

	else {
		DefDouble fMant, fsMant;
		int expon;

		fMant = frexp(num, &expon);

		if ((expon > (DEXP_MAX-DEXP_OFFSET+1)) || !(fMant < 1)) {
			/* NaN's and infinities fail second test */
			first = sign | 0x7FF00000L;		/* +/- infinity */
			second = 0;
		} else {
			long mantissa;

			if (expon < -(DEXP_OFFSET-2)) {	/* Smaller than normalized */
				int shift = (DEXP_POSITION+1) + (DEXP_OFFSET-2) + expon;
				if (shift < 0) {	/* Too small for something in the MS word */
					first = sign;
					shift += 32;
					if (shift < 0) {	/* Way too small: flush to zero */
						second = 0;
					} else {			/* Pretty small demorn */
						second = FloatToUnsigned(floor(ldexp(fMant, shift)));
					}
				} else {			/* Nonzero denormalized number */
					fsMant = ldexp(fMant, shift);
					mantissa = (long)floor(fsMant);
					first = sign | mantissa;
					second = FloatToUnsigned(floor(ldexp(fsMant - mantissa, 32)));
				}
			} else {				/* Normalized number */
				fsMant = ldexp(fMant, DEXP_POSITION+1);
				mantissa = (long)floor(fsMant);
				mantissa -= (1L << DEXP_POSITION);			/* Hide MSB */
				fsMant -= (1L << DEXP_POSITION);
				first = sign | ((long)((expon + DEXP_OFFSET - 1)) <<
				  DEXP_POSITION) | mantissa;
				second = FloatToUnsigned(floor(ldexp(fsMant - mantissa, 32)));
			}
		}
	}
	
	bytes[0] = (char) (first >> 24);
	bytes[1] = (char) (first >> 16);
	bytes[2] = (char) (first >> 8);
	bytes[3] = (char) first;
	bytes[4] = (char) (second >> 24);
	bytes[5] = (char) (second >> 16);
	bytes[6] = (char) (second >> 8);
	bytes[7] = (char) second;
}


/**************************************************************** 
* Extended precision IEEE floating-point conversion routines ****************************************************************/ 

DefDouble
ConvertFromIEEEExtended(char *bytes)
{
	DefDouble	f;
	long	expon;
	unsigned long hiMant, loMant;
	
	expon = ((bytes[0] & 0x7F) << 8) | (bytes[1] & 0xFF);
	hiMant = ((unsigned long)(bytes[2] & 0xFF) << 24) |
	  ((unsigned long)(bytes[3] & 0xFF) << 16) |
	  ((unsigned long)(bytes[4] & 0xFF) << 8) |
	  ((unsigned long)(bytes[5] & 0xFF));
	loMant = ((unsigned long)(bytes[6] & 0xFF) << 24) |
	  ((unsigned long)(bytes[7] & 0xFF) << 16) |
	  ((unsigned long)(bytes[8] & 0xFF) << 8) |
	  ((unsigned long)(bytes[9] & 0xFF));

	if (expon == 0 && hiMant == 0 && loMant == 0) {
		f = 0;
	} else {
		if (expon == 0x7FFF) {	/* Infinity or NaN */
			f = HUGE_VAL;
		} else {
			expon -= 16383;
			f = ldexp(UnsignedToFloat(hiMant), expon-=31);
 			f += ldexp(UnsignedToFloat(loMant), expon-=32);
 		}
	}

	if (bytes[0] & 0x80)
		return -f;
	else
		return f;
}


/****************************************************************/


void
ConvertToIEEEExtended(DefDouble num, char *bytes)
{
	int	sign;
	int expon;
	DefDouble fMant, fsMant;
	unsigned long hiMant, loMant;

	if (num < 0) {
		sign = 0x8000;
		num *= -1;
	} else {
		sign = 0;
	}

	if (num == 0) {
		expon = 0; hiMant = 0; loMant = 0;
	}
	else {
		fMant = frexp(num, &expon);
		if ((expon > 16384) || !(fMant < 1)) {	/* Infinity or NaN */
			expon = sign|0x7FFF;
			hiMant = 0;
			loMant = 0; /* infinity */
		} else {	/* Finite */
			expon += 16382;
			if (expon < 0) {	/* denormalized */
				fMant = ldexp(fMant, expon);
				expon = 0;
			}
			expon |= sign;
			fMant = ldexp(fMant, 32);
			fsMant = floor(fMant);
			hiMant = FloatToUnsigned(fsMant);
			fMant = ldexp(fMant - fsMant, 32);
			fsMant = floor(fMant);
			loMant = FloatToUnsigned(fsMant);
		}
	}

	bytes[0] = (expon >> 8) & 0xff;
	bytes[1] = expon & 0xff;
	bytes[2] = (unsigned char) ((hiMant >> 24) & 0xff);
	bytes[3] = (unsigned char) ((hiMant >> 16) & 0xff);
	bytes[4] = (unsigned char) ((hiMant >> 8) & 0xff);
	bytes[5] = (unsigned char) (hiMant & 0xff);
	bytes[6] = (unsigned char) ((loMant >> 24) & 0xff);
	bytes[7] = (unsigned char) ((loMant >> 16) & 0xff);
	bytes[8] = (unsigned char) ((loMant >> 8) & 0xff);
	bytes[9] = (unsigned char) (loMant & 0xff);
}

/****************************************************************
* Testing routines for the floating-point conversions. ****************************************************************/


#ifdef applec
# define IEEE
#endif /* applec */
#ifdef THINK_C
# define IEEE
#endif /* THINK_C */
#ifdef sgi
# define IEEE
#endif /* sgi */
#ifdef sequent
# define IEEE
# define LITTLE_ENDIAN
#endif /* sequent */
#ifdef sun
# define IEEE
#endif /* sun */
#ifdef NeXT
# define IEEE
#endif /* NeXT */

#ifdef MAIN

union SParts {
	Single s;
	long i;
};
union DParts {
	Double d;
	long i[2];
};
union EParts {
	DefDouble e;
	short i[6];
};


int
GetHexValue(register int x)
{
	x &= 0x7F;

	if ('0' <= x && x <= '9')
		x -= '0';
	else if ('a' <= x && x <= 'f')
		x = x - 'a' + 0xA;
	else if ('A' <= x && x <= 'F')
		x = x - 'A' + 0xA;
	else
		x = 0;

	return(x);
}


void
Hex2Bytes(register char *hex, char *bytes)
{
	for ( ; *hex; hex += 2) {
		*bytes++ = (GetHexValue(hex[0]) << 4) | GetHexValue(hex[1]);
		if (hex[1] == 0)
			break;	/* Guard against odd bytes */
	}
}


int
GetHexSymbol(register int x)
{
	x &= 0xF;
	if (x <= 9)
		x += '0';
	else
		x += 'A' - 0xA;
	return(x);
}


void
Bytes2Hex(register char *bytes, char *hex, register int nBytes)
{
	for ( ; nBytes--; bytes++) {
		*hex++ = GetHexSymbol(*bytes >> 4);
		*hex++ = GetHexSymbol(*bytes);
	}
	*hex = 0;
}


void
MaybeSwapBytes(char *bytes, int nBytes)
{
#ifdef LITTLE_ENDIAN
	register char *p, *q, t;
	for (p = bytes, q = bytes+nBytes-1; p < q; p++, q--) { 		t = *p;
		*p = *q;
		*q = t;
	}
#else
	if (bytes, nBytes);		/* Just so it's used */
#endif /* LITTLE_ENDIAN */
	
}


float
MachineIEEESingle(char *bytes)
{
	float t;
	MaybeSwapBytes(bytes, 4);
	t = *((float*)(bytes));
	MaybeSwapBytes(bytes, 4);
	return (t);
}


double
MachineIEEEDouble(char *bytes)
{
	Double t;
	MaybeSwapBytes(bytes, 8);
	t = *((Double*)(bytes));
	MaybeSwapBytes(bytes, 8);
	return (t);
}


void
TestFromIEEESingle(char *hex)
{
	DefDouble f;
	union SParts p;
	char bytes[4];

	Hex2Bytes(hex, bytes);
	f = ConvertFromIEEESingle(bytes);
	p.s = f;

#ifdef IEEE
	DSAM_fprintf(stderr, "IEEE(%g) [%s] --> float(%g) [%08lX]\n",
	  MachineIEEESingle(bytes),
	hex, f, p.i);
#else /* IEEE */
	DSAM_fprintf(stderr, "IEEE[%s] --> float(%g) [%08lX]\n", hex, f, p.i);
#endif /* IEEE */
}


void
TestToIEEESingle(DefDouble f)
{
	union SParts p;
	char bytes[4];
	char hex[8+1];

	p.s = f;

	ConvertToIEEESingle(f, bytes);
	Bytes2Hex(bytes, hex, 4);
#ifdef IEEE
	DSAM_fprintf(stderr, "float(%g) [%08lX] --> IEEE(%g) [%s]\n", 		f, p.i,
		MachineIEEESingle(bytes),
		hex
	);
#else /* IEEE */
	DSAM_fprintf(stderr, "float(%g) [%08lX] --> IEEE[%s]\n", f, p.i, hex);
#endif /* IEEE */
}


void
TestFromIEEEDouble(char *hex)
{
	DefDouble f;
	union DParts p;
	char bytes[8];
	
	Hex2Bytes(hex, bytes);
	f = ConvertFromIEEEDouble(bytes);
	p.d = f;

#ifdef IEEE
	DSAM_fprintf(stderr, "IEEE(%g) [%.8s %.8s] --> double(%g) [%08lX %08lX]\n",
	  MachineIEEEDouble(bytes),
	hex, hex+8, f, p.i[0], p.i[1]);
#else /* IEEE */
	DSAM_fprintf(stderr, "IEEE[%.8s %.8s] --> double(%g) [%08lX %08lX]\n",
	  hex, hex+8, f, p.i[0], p.i[1]);
#endif /* IEEE */

}

void
TestToIEEEDouble(DefDouble f)
{
	union DParts p;
	char bytes[8];
	char hex[16+1];

	p.d = f;

	ConvertToIEEEDouble(f, bytes);
	Bytes2Hex(bytes, hex, 8);
#ifdef IEEE
	DSAM_fprintf(stderr, "double(%g) [%08lX %08lX] --> IEEE(%g) [%.8s %.8s]\n",
	  f, p.i[0], p.i[1], MachineIEEEDouble(bytes), hex, hex+8 );
#else /* IEEE */
	DSAM_fprintf(stderr, "double(%g) [%08lX %08lX] --> IEEE[%.8s %.8s]\n", f, p.i[0],
	  p.i[1], hex, hex+8 );
#endif /* IEEE */

}


void
TestFromIEEEExtended(char *hex)
{
	DefDouble f;
	union EParts p;
	char bytes[12];

	Hex2Bytes(hex, bytes);
	f = ConvertFromIEEEExtended(bytes);
	p.e = f;

	bytes[11] = bytes[9];
	bytes[10] = bytes[8];
	bytes[9] = bytes[7];
	bytes[8] = bytes[6];
	bytes[7] = bytes[5];
	bytes[6] = bytes[4];
	bytes[5] = bytes[3];
	bytes[4] = bytes[2];
	bytes[3] = 0;
	bytes[2] = 0;

#if defined(applec) || defined(THINK_C)
	DSAM_fprintf(stderr, "IEEE(%g) [%.4s %.8s %.8s] --> extended(%g) "\
	  "[%04X %04X%04X %04X%04X]\n", *((DefDouble*)(bytes)), hex, hex+4, hex+12,
	  f, p.i[0]&0xFFFF, p.i[2]&0xFFFF, p.i[3]&0xFFFF, p.i[4]&0xFFFF,
	  p.i[5]&0xFFFF );
#else /* !Macintosh */
	DSAM_fprintf(stderr, "IEEE[%.4s %.8s %.8s] --> extended(%g) "\
	  "[%04X %04X%04X %04X%04X]\n", hex, hex+4, hex+12, f, p.i[0]&0xFFFF,
	  p.i[2]&0xFFFF, p.i[3]&0xFFFF, p.i[4]&0xFFFF, p.i[5]&0xFFFF);
#endif /* Macintosh */
}


void
TestToIEEEExtended(DefDouble f)
{
	char bytes[12];
	char hex[24+1];

	ConvertToIEEEExtended(f, bytes);
	Bytes2Hex(bytes, hex, 10);

	bytes[11] = bytes[9];
	bytes[10] = bytes[8];
	bytes[9] = bytes[7];
	bytes[8] = bytes[6];
	bytes[7] = bytes[5];
	bytes[6] = bytes[4];
	bytes[5] = bytes[3];
	bytes[4] = bytes[2];
	bytes[3] = 0;
	bytes[2] = 0;

#if defined(applec) || defined(THINK_C)
	DSAM_fprintf(stderr, "extended(%g) --> IEEE(%g) [%.4s %.8s %.8s]\n", f,
	  *((DefDouble*)(bytes)), hex, hex+4, hex+12);
#else /* !Macintosh */
	DSAM_fprintf(stderr, "extended(%g) --> IEEE[%.4s %.8s %.8s]\n", f, hex, hex+4,
	  hex+12);
#endif /* Macintosh */
}

#include	<signal.h>

void
SignalFPE(int i)
{
	printf("[UtUIEEFloat: Floating Point Interrupt Caught.]\n");
 	signal(SIGFPE, SignalFPE);
}
	
void
main()
{
	long d[3];
	char bytes[12];

	signal(SIGFPE, SignalFPE);

	TestFromIEEESingle("00000000");
	TestFromIEEESingle("80000000");
	TestFromIEEESingle("3F800000");
	TestFromIEEESingle("BF800000");
	TestFromIEEESingle("40000000");
	TestFromIEEESingle("C0000000");
	TestFromIEEESingle("7F800000");
	TestFromIEEESingle("FF800000");
	TestFromIEEESingle("00800000");
	TestFromIEEESingle("00400000");
	TestFromIEEESingle("00000001");
	TestFromIEEESingle("80000001");
	TestFromIEEESingle("3F8FEDCB");
	TestFromIEEESingle("7FC00100");	/* Quiet NaN(1) */
	TestFromIEEESingle("7F800100");	/* Signalling NaN(1) */

	TestToIEEESingle(0.0);
	TestToIEEESingle(-0.0);
	TestToIEEESingle(1.0);
	TestToIEEESingle(-1.0);
	TestToIEEESingle(2.0);
	TestToIEEESingle(-2.0);
	TestToIEEESingle(3.0);
	TestToIEEESingle(-3.0);
#if !(defined(sgi) || defined(NeXT))
	TestToIEEESingle(HUGE_VAL);
	TestToIEEESingle(-HUGE_VAL);
#endif

#ifdef IEEE
	/* These only work on big-endian IEEE machines */
	d[0] = 0x00800000L; MaybeSwapBytes(d,4); TestToIEEESingle(*((float*)(&d[0])));		/* Smallest normalized */
 	d[0] = 0x00400000L; MaybeSwapBytes(d,4); TestToIEEESingle(*((float*)(&d[0])));		/* Almost largest denormalized */
 	d[0] = 0x00000001L; MaybeSwapBytes(d,4); TestToIEEESingle(*((float*)(&d[0])));		/* Smallest denormalized */
 	d[0] = 0x00000001L; MaybeSwapBytes(d,4); TestToIEEESingle(*((float*)(&d[0])) * 0.5);	/* Smaller than smallest denorm */
 	d[0] = 0x3F8FEDCBL; MaybeSwapBytes(d,4); TestToIEEESingle(*((float*)(&d[0])));
#if !(defined(sgi) || defined(NeXT))
	d[0] = 0x7FC00100L; MaybeSwapBytes(d,4); TestToIEEESingle(*((float*)(&d[0])));		/* Quiet NaN(1) */
	d[0] = 0x7F800100L; MaybeSwapBytes(d,4); TestToIEEESingle(*((float*)(&d[0])));		/* Signalling NaN(1) */
#endif /* sgi */
#endif /* IEEE */



	TestFromIEEEDouble("0000000000000000"); 	TestFromIEEEDouble("8000000000000000"); 	TestFromIEEEDouble("3FF0000000000000"); 	TestFromIEEEDouble("BFF0000000000000"); 	TestFromIEEEDouble("4000000000000000"); 	TestFromIEEEDouble("C000000000000000"); 	TestFromIEEEDouble("7FF0000000000000"); 	TestFromIEEEDouble("FFF0000000000000"); 	TestFromIEEEDouble("0010000000000000"); 	TestFromIEEEDouble("0008000000000000"); 	TestFromIEEEDouble("0000000000000001"); 	TestFromIEEEDouble("8000000000000001"); 	TestFromIEEEDouble("3FFFEDCBA9876543"); 	TestFromIEEEDouble("7FF8002000000000");	/* Quiet NaN(1) */ 	TestFromIEEEDouble("7FF0002000000000");	/* Signalling NaN(1) */ 

	TestToIEEEDouble(0.0);
	TestToIEEEDouble(-0.0);
	TestToIEEEDouble(1.0);
	TestToIEEEDouble(-1.0);
	TestToIEEEDouble(2.0);
	TestToIEEEDouble(-2.0);
	TestToIEEEDouble(3.0);
	TestToIEEEDouble(-3.0);
#if !(defined(sgi) || defined(NeXT))
	TestToIEEEDouble(HUGE_VAL);
	TestToIEEEDouble(-HUGE_VAL);
#endif

#ifdef IEEE
	/* These only work on big-endian IEEE machines */
	Hex2Bytes("0010000000000000", bytes); MaybeSwapBytes(d,8); TestToIEEEDouble(*((Double*)(bytes)));	/* Smallest normalized */
	Hex2Bytes("0010000080000000", bytes); MaybeSwapBytes(d,8); TestToIEEEDouble(*((Double*)(bytes)));	/* Normalized, problem with unsigned */
	Hex2Bytes("0008000000000000", bytes); MaybeSwapBytes(d,8); TestToIEEEDouble(*((Double*)(bytes)));	/* Almost largest denormalized */
	Hex2Bytes("0000000080000000", bytes); MaybeSwapBytes(d,8); TestToIEEEDouble(*((Double*)(bytes)));	/* Denorm problem with unsigned */
	Hex2Bytes("0000000000000001", bytes); MaybeSwapBytes(d,8); TestToIEEEDouble(*((Double*)(bytes)));	/* Smallest denormalized */
	Hex2Bytes("0000000000000001", bytes); MaybeSwapBytes(d,8); TestToIEEEDouble(*((Double*)(bytes)) * 0.5);	/* Smaller than smallest denorm */
	Hex2Bytes("3FFFEDCBA9876543", bytes); MaybeSwapBytes(d,8); TestToIEEEDouble(*((Double*)(bytes)));	/* accuracy test */
#if !(defined(sgi) || defined(NeXT))
	Hex2Bytes("7FF8002000000000", bytes); MaybeSwapBytes(d,8); TestToIEEEDouble(*((Double*)(bytes)));	/* Quiet NaN(1) */
	Hex2Bytes("7FF0002000000000", bytes); MaybeSwapBytes(d,8); TestToIEEEDouble(*((Double*)(bytes)));	/* Signalling NaN(1) */
	#endif /* sgi */
#endif /* IEEE */

	TestFromIEEEExtended("00000000000000000000");	/* +0 */ 	TestFromIEEEExtended("80000000000000000000");	/* -0 */ 	TestFromIEEEExtended("3FFF8000000000000000");	/* +1 */ 	TestFromIEEEExtended("BFFF8000000000000000");	/* -1 */ 	TestFromIEEEExtended("40008000000000000000");	/* +2 */ 	TestFromIEEEExtended("C0008000000000000000");	/* -2 */ 	TestFromIEEEExtended("7FFF0000000000000000");	/* +infinity */ 	TestFromIEEEExtended("FFFF0000000000000000");	/* -infinity */ 	TestFromIEEEExtended("7FFF8001000000000000");	/* Quiet NaN(1) */ 	TestFromIEEEExtended("7FFF0001000000000000");	/* Signalling NaN(1) */ 	TestFromIEEEExtended("3FFFFEDCBA9876543210");	/* accuracy test */ 

	TestToIEEEExtended(0.0);
	TestToIEEEExtended(-0.0);
	TestToIEEEExtended(1.0);
	TestToIEEEExtended(-1.0);
	TestToIEEEExtended(2.0);
	TestToIEEEExtended(-2.0);
#if !(defined(sgi) || defined(NeXT))
	TestToIEEEExtended(HUGE_VAL);
	TestToIEEEExtended(-HUGE_VAL);
#endif /* sgi */

#if defined(applec) || defined(THINK_C)
	Hex2Bytes("7FFF00008001000000000000", bytes); TestToIEEEExtended(*((long double*)(bytes)));	/* Quiet NaN(1) */
	Hex2Bytes("7FFF00000001000000000000", bytes); TestToIEEEExtended(*((long double*)(bytes)));	/* Signalling NaN(1) */
	Hex2Bytes("7FFE00008000000000000000", bytes); TestToIEEEExtended(*((long double*)(bytes)));
	Hex2Bytes("000000008000000000000000", bytes); TestToIEEEExtended(*((long double*)(bytes)));
	Hex2Bytes("000000000000000000000001", bytes); TestToIEEEExtended(*((long double*)(bytes)));
	Hex2Bytes("3FFF0000FEDCBA9876543210", bytes); TestToIEEEExtended(*((long double*)(bytes)));
#endif
}

#endif /* MAIN */
