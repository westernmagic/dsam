/******************
 *
 * File:		GeCommon.h
 * Purpose:		This file contains the common system routines:
 * Comments:	25-03-98 LPO: In graphics support mode the error and warning
 *				messages are sent to a window.
 *				It wasn't possible to create a single NotifyMessage routine
 *				that could be called by "NotifyError" and "NotifyWarning"
 *				because, I think, of the way that the stdarg macros work.
 *				26-09-98 LPO: Introduced the minus sign convention for formats
 *				e.g. "-10s".  I have also introduced the "CloseFiles" routine.
 *				At present this is only being used by the simulation manager
 *				so that files are closed after the simulation has been run.
 *				28-09-98 LPO: Introduced the MAX_FILE_PATH constant definition
 *				and used in in all modules which need to store file names.
 *				29-01-99 LPO: Under the GRAPHICS mode the 'NotifyError' routine
 *				will produce a dialog the first time (if the 
 *				dSAM.'dontUseGUIFlag' flag is set to false).  After the first
 *				dialog is created, then subsequent calls will send error
 *				messages to the console.
 *				04-02-99 LPO: Introduced the 'CloseFile' routine to prevent
 *				attempts to close the system files, such as 'stdin'.  It is now
 *				being used by all the respective 'Fi..' modules.
 *				30-04-99 LPO: Added the 'version' field to the 'dSAM' global
 *				structure.  It will mean that programs can report the current
 *				run-time library version rather than the compile-time version.
 *				I also introduced the 'parsFilePath' so that. This can be set
 *				to read parameters from a specified file path.
 *				09-05-99 LPO: Introduced the 'COMMON_GENERAL_DIAGNOSTIC' option
 *				for printing diagnostics.
 *				12-05-99 LPO: Introduced the 'GetParsFileFPath_Common'.
 *				08-07-99 LPO: Introduced the UNSET_FILE_PTR constant for the
 *				DSAM global structure file pointers.  This was necessary because
 *				these pointers cannot be used as initialisers under
 *				egcs 2.91.66 (gcc) because they are not constants.  The
 *				alternative would be to call a routine to initialise the dSAM
 *				global structure, but I do not want to do that.
 * Authors:		L. P. O'Mard
 * Created:		12 Jul 1993
 * Updated:		08 Jul 1999
 * Copyright:	(c) 1999, University of Essex.
 * 
 ******************/

#ifndef	_GECOMMON_H
#define _GECOMMON_H	1

#include <stdarg.h>

/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

/*
 * These following lines are required to turn on the appropriate values for the
 * cccrlx scripts, regardless of the settings in the DSAMSetup.h file - which is
 * useful for other things too.
 */

#ifdef CCDSAM
#	undef GRAPHICS_SUPPORT
#	undef MPI_SUPPORT
#endif

#ifdef CCDSAMG
#	define GRAPHICS_SUPPORT	1
#	undef MPI_SUPPORT
#endif

#ifdef CCDSAMM
#	define MPI_SUPPORT		1
#	undef GRAPHICS_SUPPORT
#endif

#ifdef CCDSAMMG
#	define MPI_SUPPORT		1
#	define GRAPHICS_SUPPORT	1
#endif

/*
 * Normal definitions
 */

#ifndef PI
# define	PI	3.141592653589792434		/* From MacEar */
#endif
#define PIx2	6.283185307179584868
#define	SQRT_2	1.4142135623730951
#ifndef LN_2
#	define LN_2	0.69314718055994529
#endif

#define MAXLINE				80		/* Max. No. of characters in a line. */
#define SMALL_STRING		10		/* For small string operations. */
#define LONG_STRING			255		/* For very long strings. */
#define MAX_FILE_PATH		255		/* For file names - can have long paths */
#define UNSET_STRING		"<unset>" /* initial string value for arrays. */
#define TAB_SPACES			4		/* Spaces per tab with GRAPHICS_SUPPORT */
#define	NO_FILE				"no_file"	/* -for when this must be indicated. */
#define DEFAULT_ERRORS_FILE		stderr
#define DEFAULT_WARNINGS_FILE	stdout
#define DEFAULT_PARS_FILE		stdout
#define UNSET_FILE_PTR			(FILE *) -1

#ifndef TRUE
#	define	TRUE		0xFFFF
#endif
#ifndef FALSE
#	define	FALSE		0
#endif
#ifndef DBL_MAX
#	define	DBL_MAX		((double) 1.0e37)
#endif
#ifndef FLT_MAX
#	define	FLT_MAX		((float) 1.0e37)
#endif

#ifndef GRAPHICS_SUPPORT
#	define MainSimulation	main
#endif
#define	PROCESS_START_TIME		1

/******************************************************************************/
/*************************** Macro definitions ********************************/
/******************************************************************************/

#define RMS_AMP(DBVALUE)		(20.0 * pow(10.0, (DBVALUE) / 20.0))

#define DB_SPL(RMS_AMPLITUDE)	(20.0 * log10((RMS_AMPLITUDE) / 20.0))

#define MSEC(SECONDS)			((SECONDS) * 1.0e+3)
#define MILLI(VALUE)			((VALUE) * 1.0e+3)
#define NANO(VALUE)				((VALUE) * 1.0e+9)
#define PICO(VALUE)				((VALUE) * 1.0e+12)

#define SQR(X) 					((X) * (X))

#define	DEGREES_TO_RADS(DEGREES)	((DEGREES) / 57.29577951)

#define	MINIMUM(A, B)			(((A) < (B))? (A): (B))

#define	MAXIMUM(A, B)			(((A) > (B))? (A): (B))

/*
 * __BEGIN_DECLS should be used at the beginning of all C declarations,
 * so that C++ compilers don't mangle their names.  Use __END_DECLS at
 * the end of C declarations.
 * This example was taken from the GNU libtool-1.12 documentation as a method
 * for ensuring that C libraries can be safely used with C++ code.
 */

#undef __BEGIN_DECLS
#undef __END_DECLS
#ifdef __cplusplus
# define __BEGIN_DECLS extern "C" {
# define __END_DECLS }
#else
# define __BEGIN_DECLS /* empty */
# define __END_DECLS /* empty */
#endif

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

#ifndef	BOOLN 
#	define BOOLN_ALGEBRA
	typedef	int		BOOLN;
#endif

typedef	unsigned long	ChanLen;	/* For the channel indices. */
typedef	double			ChanData;	/* Data type for channel data. */
typedef	unsigned short	uShort;
typedef	unsigned short	uInt;

typedef enum {
	
	LOCAL,
	GLOBAL

} ParameterSpecifier;

typedef enum {
	
	COMMON_ERROR_DIAGNOSTIC,
	COMMON_WARNING_DIAGNOSTIC,
	COMMON_GENERAL_DIAGNOSTIC

} CommonDiagSpecifier;
	
typedef struct {

	BOOLN	segmentedMode;		/* TRUE, when in segmented mode. */
	BOOLN	dontUseGUIFlag;		/* TRUE, when forcing output to stdout. */
	char	*diagnosticsPrefix;	/* Printed before diagnostics output. */
	char	*version;			/* Global version; shared library will show */
	char	*parsFilePath;		/* File path for parameter files. */
	FILE	*warningsFile;		/* File to which warnings should be sent. */
	FILE	*errorsFile;		/* File to which errors should be sent. */
	FILE	*parsFile;			/* File for parameter listings. */

} DSAM, *DSAMPtr;

typedef	enum {

	APPEND,
	OVERWRITE

} FileAccessSpecifier;

/******************************************************************************/
/*************************** Preference type definitions **********************/
/******************************************************************************/

typedef struct UniParListNode	*UniParListPtr;

/******************************************************************************/
/*************************** External Variables *******************************/
/******************************************************************************/

extern DSAM	dSAM;

/******************************************************************************/
/*************************** Global Subroutines *******************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

void	CheckInitErrorsFile_Common(void);

void	CheckInitParsFile_Common(void);

void	CheckInitWarningsFile_Common(void);

void	CloseFile(FILE *fp);

void	CloseFiles(void);

char *	DiagnosticTitle(CommonDiagSpecifier type);

void	DPrint(char *format, ...);

void	DPrintStandard(char *format, va_list args);

void	DPrintStandard_Message(char *format, va_list args);

void	FindFilePathAndName_Common(char *filePath, char *path, char *name);

FILE *	GetFilePtr(char *outputSpecifier, FileAccessSpecifier mode);

char *	GetParsFileFPath_Common(char *parFile);

void	NotifyError(char *format, ...);

void	NotifyStandard(const char *format, va_list args,
		  CommonDiagSpecifier type);

void	NotifyWarning(char *format, ...);

void	ReadParsFromFile(char *fileName);		/* Used in test programs. */

void	ResetGUIDialogs(void);

void	SetDiagnosticsPrefix(char *prefix);

void	SetErrorsFile_Common(char *outputSpecifier, FileAccessSpecifier mode);

void	SetParsFile_Common(char *outputSpecifier, FileAccessSpecifier mode);

void	SetParsFilePath_Common(char *name);

void	SetSegmentedMode(BOOLN setting);

void	SetWarningsFile_Common(char *outputSpecifier, FileAccessSpecifier mode);

void	SwitchDiagnostics_Common(CommonDiagSpecifier specifier, BOOLN on);

__END_DECLS

#endif