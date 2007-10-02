/******************
 *
 * File:		FiSndFile.h
 * Purpose:		This Filing reads sound format files using the libsndfile
 * 				library.
 * Comments:	
 * Authors:		L. P. O'Mard
 * Created:		07 Nov 2006
 * Updated:		
 * Copyright:	(c) 2006, Lowel P. O'Mard
 * 
 ******************/

#ifndef	_FISNDFILE_H
#define _FISNDFILE_H	1

#if HAVE_SNDFILE

#include <sndfile.h>

/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

#define SND_FILE_FORMAT_DELIMITERS		" "
#define SND_FILE_FORMAT_PAR_SEPARATOR	':'
#define SND_FILE_TEST_FILE_MEMORY_SIZE	512

/******************************************************************************/
/*************************** Macro definitions ********************************/
/******************************************************************************/

#define SND_FILE_ARRAY_LEN(x)	((int) (sizeof (x)) / (sizeof ((x) [0])))
#define SND_FILE_VIO_PTR(earObj)	(((DataFilePtr) (earObj)->module->parsPtr)->vIOPtr)


/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

typedef struct {
	
	sf_count_t	offset;
	sf_count_t	length;
	sf_count_t	maxLength;
	unsigned char *data;

} DFVirtualIO, *DFVirtualIOPtr ;

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

void	Free_SndFile(void);

void	FreeVirtualIOMemory_SndFile(DFVirtualIOPtr *p);

double	GetDuration_SndFile(WChar *fileName);

BOOLN	InitVirtualIOMemory_SndFile(DFVirtualIOPtr *p, sf_count_t maxLength);

BOOLN	ReadFile_SndFile(WChar *fileName, EarObjectPtr data);

BOOLN	WriteFile_SndFile(WChar *fileName, EarObjectPtr data);

__END_DECLS

#endif /* HAVE_SNDFILE */
#endif /* _FISNDFILE_H */
