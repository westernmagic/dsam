/******************
 *
 * File:		GeCommon.c
 * Purpose:		This file contains the common system routines:
 * Comments:	25-03-98 LPO: In graphics support mode the error and warning
 *				messages are sent to a window.
 *				It wasn't possible to create a single Notify_Message routine
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
 *				dSAM.'dialogOutputFlag' flag is set to TRUE).  After the first
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
 *				15-10-99 LPO: The 'usingGUIFlag' is set when MyApp is
 *				initialised.
 * Authors:		L. P. O'Mard
 * Created:		12 Jul 1993
 * Updated:		08 Jul 1999
 * Copyright:	(c) 1999, University of Essex.
 * 
 ******************/

#ifdef HAVE_CONFIG_H
#	include "DSAMSetup.h"
#endif /* HAVE_CONFIG_H */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "GeCommon.h"
#include "StdMessage.h"
#include "GeNSpecLists.h"

/******************************************************************************/
/************************ Global variables ************************************/
/******************************************************************************/

static DSAM	dSAM = {

			FALSE,		/* appInitialisedFlag */
			FALSE,		/* segmentedMode */
			FALSE,		/* usingGUIFlag */
			FALSE,		/* lockGUIFlag */
			FALSE,		/* interruptRequestedFlag */
			NULL,		/* diagnosticsPrefix */
			VERSION,	/* version */
			NULL,		/* parsFilePath */
			UNSET_FILE_PTR,	/* warningsFile */
			UNSET_FILE_PTR,	/* errorsFile */
			UNSET_FILE_PTR,	/* parsFile */
			COMMON_CONSOLE_DIAG_MODE,	/* diagMode */
			DPrintStandard,	/* DPrint */
			NotifyStandard	/* Notify */

		};

/******************************************************************************/
/************************ Subroutines and functions ***************************/
/******************************************************************************/

/*************************** DPrintStandard ***********************************/

/*
 * This routine prints out a diagnostic message, preceded by a bell sound.
 * It is used in the same way as the vprintf statement.
 * This is the standard version for ANSI C.
 */
 
void
DPrintStandard(char *format, va_list args)
{
	CheckInitParsFile_Common();
	if (dSAM.diagnosticsPrefix)
		fprintf(dSAM.parsFile, "%s", dSAM.diagnosticsPrefix);
	vfprintf(dSAM.parsFile, format, args);
	
}

/*************************** DPrint *******************************************/

/*
 * This routine prints out a diagnostic message.
 * It is used in the same way as the printf statement.
 * This is the standard version for ANSI C.
 */
 
void
DPrint(char *format, ...)
{
	va_list	args;

	CheckInitParsFile_Common();
	if (!dSAM.parsFile)
		return;
	va_start(args, format);
	if (dSAM.usingGUIFlag && (dSAM.parsFile == stdout)) {
		(* dSAM.DPrint)(format, args);
		return;
	}
	DPrintStandard(format, args);
	va_end(args);
	
}

/*************************** DiagnosticTitle **********************************/

/*
 * This is a simple routine to return the pointer to a diagnostic title.
 */

char *
DiagnosticTitle(CommonDiagSpecifier type)
{
	static NameSpecifier	list[] = {

			{ "Error",		COMMON_ERROR_DIAGNOSTIC },
			{ "Warning",	COMMON_WARNING_DIAGNOSTIC },
			{ "Alert",		COMMON_GENERAL_DIAGNOSTIC }
		};

	return(list[type].name);
}

/***************************** NotifyStandard *********************************/

/*
 * This routine prints out a message to the respective file descriptor.
 * This is the standard version which can be used both in GUI mode and non-
 * GUI mode.
 * It does not take any further action, as responses to errors differ: some
 * errors are recoverable while others are fatal.
 */

void
NotifyStandard(const char *format, va_list args, CommonDiagSpecifier type)
{
	FILE	*fp;

	switch (type) {
	case COMMON_ERROR_DIAGNOSTIC:
		fp = dSAM.errorsFile;
		break;
	case COMMON_WARNING_DIAGNOSTIC:
		fp = dSAM.warningsFile;
		break;
	default:
		fp = stdout;
	}
	vfprintf(fp, format, args);
	fprintf(fp, "\n");

} /* NotifyStandard */

/***************************** NotifyError ************************************/

/*
 * This routine prints out an error message, preceded by a bell sound.
 * It is used in the same way as the printf statement.
 * It does not take any further action, as responses to errors differ: some
 * errors are recoverable while others are fatal.
 * This routine will be incorporated in an alert dialog in the DSAM
 * application.
 */

void
NotifyError(char *format, ...)
{
	va_list	args;

	CheckInitErrorsFile_Common();
	if (!dSAM.errorsFile)
		return;
	va_start(args, format);
	if ((dSAM.errorsFile == stderr) && (!dSAM.usingGUIFlag ||
	  (dSAM.diagMode == COMMON_DIALOG_DIAG_MODE)))
		fprintf(stderr, "\07");
	(* dSAM.Notify)(format, args, COMMON_ERROR_DIAGNOSTIC);
	va_end(args);

} /* NotifyError */

/*************************** NotifyWarning ************************************/

/*
 * This routine prints out a warning message, preceded by a bell sound.  It is
 * used in the same way as the printf statement.  It does not take any further
 * action.
 */
 
void
NotifyWarning(char *format, ...)
{
	va_list	args;
	
	CheckInitWarningsFile_Common();
	if (!dSAM.warningsFile)
		return;
	va_start(args, format);
	(* dSAM.Notify)(format, args, COMMON_WARNING_DIAGNOSTIC);
	va_end(args);

} /* NotifyWarning */

/*************************** GetFilePtr ***************************************/

/*
 * This function returns a file pointer according to a specified mode.
 * The default is the standard output, which is set by sending
 * a null string as the function argument.
 * The function returns 'stderr' if it fails.
 */

FILE *
GetFilePtr(char *outputSpecifier, FileAccessSpecifier mode)
{
	static const char *funcName = "GetFilePtr";
	char	*fileAccess;
	FILE	*fp;

	switch (Identify_NameSpecifier(outputSpecifier,
	  DiagModeList_NSpecLists(0))) {
	case GENERAL_DIAGNOSTIC_OFF_MODE:
		return(NULL);
	case GENERAL_DIAGNOSTIC_SCREEN_MODE:
		return(stdout);
	case GENERAL_DIAGNOSTIC_ERROR_MODE:
		return(stderr);
	default:
		fileAccess = (mode == APPEND)? "a": "w";
		if ((fp = fopen(outputSpecifier, fileAccess)) == NULL) {
			NotifyError("%s: Could not open file '%s' output sent to stderr.",
			  funcName, outputSpecifier);
			return(stderr); 
		}
	}
	return(fp);

}

/*************************** SetWarningsFile **********************************/

/*
 * This function sets the file to which warnings should be sent.
 * The default is the standard output, which can be reset by sending
 * a null string as the function argument.
 * The function returns TRUE if successful.
 */

void
SetWarningsFile_Common(char *outputSpecifier, FileAccessSpecifier mode)
{
	dSAM.warningsFile = GetFilePtr(outputSpecifier, mode);

}

/*************************** SetParsFile **************************************/

/*
 * This function sets the file to which parameter listings should be sent.
 * The default is the standard output, which can be reset by sending
 * a null string as the function argument.
 * The function returns TRUE if successful.
 */

void
SetParsFile_Common(char *outputSpecifier, FileAccessSpecifier mode)
{
	dSAM.parsFile = GetFilePtr(outputSpecifier, mode);

}

/*************************** SetErrorsFile ************************************/

/*
 * This function sets the file to which errors should be sent.
 * The default is the standard error, which can be reset by sending
 * a null string as the function argument.
 * The function returns TRUE if successful.
 */

void
SetErrorsFile_Common(char *outputSpecifier, FileAccessSpecifier mode)
{
	dSAM.errorsFile = GetFilePtr(outputSpecifier, mode);

}

/*************************** SetSegmentedMode ********************************/

/*
 * This routine sets the segmented mode variable. It instructs process
 * routines not to reset their parameters after the first run, or to introduce
 * the necessary time offset.
 */

void
SetSegmentedMode(BOOLN setting)
{
	dSAM.segmentedMode = setting;

}

/*************************** SetDiagnosticsPrefix *****************************/

/*
 * This routine sets the prefix to be printed before diagnostic output using
 * 'DPrint'.
 */

void
SetDiagnosticsPrefix(char *prefix)
{
	dSAM.diagnosticsPrefix = prefix;

}

/*************************** CloseFile ****************************************/

/*
 * This routine closes a file, checking that it is not any of the special
 * system files.
 */

void
CloseFile(FILE *fp)
{
	if (fp && (fp != UNSET_FILE_PTR) && (fp != stdin) && (fp != stdout) &&
	  (fp != stderr))
		fclose(fp);

}

/*************************** CloseFiles ***************************************/

/*
 * This routine closes any files opened by DSAM, i.e. diagnostic files.
 */

void
CloseFiles(void)
{
	CloseFile(dSAM.warningsFile);
	CloseFile(dSAM.errorsFile);
	CloseFile(dSAM.parsFile);

}

/*************************** ResetGUIDialogs **********************************/

/*
 * This routine sets the 'diagMode' to 'COMMON_DIALOG_DIAG_MODE' so that output
 * is sent to dialogs and not to the console.
 */

void
ResetGUIDialogs(void)
{
	dSAM.diagMode = COMMON_DIALOG_DIAG_MODE;

}

/*************************** SetDiagMode **************************************/

/*
 * This routine sets the 'dialogOutputFlag', defining if output is sent to
 * dialogs and not to the console.
 */

void
SetDiagMode(DiagModeSpecifier mode)
{
	dSAM.diagMode = mode;

}

/*************************** SetDPrintFunc ************************************/

/*
 * This routine sets the 'DPrintFunc', defining where diagnostic output is sent.
 */

void
SetDPrintFunc(void (* Func)(char *, va_list))
{
	dSAM.DPrint = Func;

}

/*************************** SetNotifyFunc ************************************/

/*
 * This routine sets the 'DPrintFunc', defining where diagnostic output is sent.
 */

void
SetNotifyFunc(void (* Func)(const char *, va_list, CommonDiagSpecifier))
{
	dSAM.Notify = Func;

}

/*************************** SetUsingGUIStatus *******************************/

/*
 * This routine sets the 'usingGUIFlag', defining if output is sent to
 * dialogs and not to the console.
 */

void
SetUsingGUIStatus(BOOLN status)
{
	dSAM.usingGUIFlag = status;

}

/*************************** SetInterruptRequestStatus ************************/

/*
 * This routine sets the 'SetInterruptRequestStatus'.  This flag is set when
 * an interrupt has been requested, and processing must be curtailed.
 */

void
SetInterruptRequestStatus_Common(BOOLN status)
{
	dSAM.interruptRequestedFlag = status;

}

/*************************** SetParsFilePath **********************************/

/*
 * This function sets the file path field of the global 'dSAM' structure.
 * At the moment this is used by the simulation script utility.
 * In use, a zero-length string should assume the current directory.
 */

void
SetParsFilePath_Common(char *name)
{
	if ((name == NULL) || (name[0] == '\0'))
		dSAM.parsFilePath = NULL;
	else
		dSAM.parsFilePath = name;

}

/*************************** FindFilePathAndName ******************************/

/*
 * This routine returns as argument pointers the pointers to the file path and
 * file name of a string.
 * It sets the path and name to a zero length string if no path or name is find
 * in the 'filePath' respectively.
 */

void
FindFilePathAndName_Common(char *filePath, char *path, char *name)
{
	char *p;

	if (filePath && (((p = strrchr(filePath, '/')) != NULL) || ((p = strrchr(
	  filePath, '\\')) != NULL))) {
		if (path) {
			strncpy(path, filePath, p - filePath);
			path[p - filePath] = '\0';
		}
		if (name)
			strcpy(name, p + 1);
		return;
	}
	if (path)
		path[0] = '\0';
	if (name) {
		if (filePath)
			strcpy(name, filePath);
		else
			name[0] = '\0';
	}

}

/*************************** GetParsFileFPath *********************************/

/*
 * This routine returns a pointer to the parameter file file-path.
 * It always assumes that any file path in 'parFile' is to be appended to
 * 'dsam.parsFilePath', unless the 'parFile' string starts with '/' or contains
 * ":\" when an absolute path is assumed.
 */

char *
GetParsFileFPath_Common(char *parFile)
{
	static const char *funcName = "GetParsFileFPath_Common";
	static char filePath[MAX_FILE_PATH];
	
	if (!dSAM.parsFilePath)
		return(parFile);
	if (strlen(parFile) >= MAX_FILE_PATH) {
		parFile[MAX_FILE_PATH - 1] = '\0';
		NotifyWarning("%s: file path is too long, truncating to '%s'", funcName,
		  parFile);
	}
	if ((parFile[0] == '/') || strstr(parFile, ":\\"))
		strcpy(filePath, parFile);
	else
		sprintf(filePath, "%s/%s", dSAM.parsFilePath, parFile);
	return (filePath);

}

/*************************** SwitchDiagnostics ********************************/

/*
 * This routine sets the specified diagnostics file point to NULL, and remembers
 * the previous value so that it can switch it back to the previous value.
 */

void
SwitchDiagnostics_Common(CommonDiagSpecifier specifier, BOOLN on)
{
	static FILE 	*oldWarningsFile = UNSET_FILE_PTR;
	static FILE 	*oldErrorsFile = UNSET_FILE_PTR;
	static FILE 	*oldParsFile = UNSET_FILE_PTR;

	switch (specifier) {
	case COMMON_ERROR_DIAGNOSTIC:
		if (on)
			dSAM.errorsFile = oldErrorsFile;
		else {
			oldErrorsFile = dSAM.errorsFile;
			dSAM.errorsFile = NULL;
		}
		break;
	case COMMON_WARNING_DIAGNOSTIC:
		if (on)
			dSAM.warningsFile = oldWarningsFile;
		else {
			oldWarningsFile = dSAM.warningsFile;
			dSAM.warningsFile = NULL;
		}
		break;
	case COMMON_GENERAL_DIAGNOSTIC:
		if (on)
			dSAM.parsFile = oldParsFile;
		else {
			oldParsFile = dSAM.parsFile;
			dSAM.parsFile = NULL;
		}
		break;
	}

}

/*************************** CheckInitErrorsFile ******************************/

/*
 * This checks that the DSAM global structure errors file pointer has been
 * initialised.
 * It will set the pointer to the default value if it is unset.
 */

void
CheckInitErrorsFile_Common(void)
{
	if (dSAM.errorsFile == UNSET_FILE_PTR)
		dSAM.errorsFile = DEFAULT_ERRORS_FILE;

}

/*************************** CheckInitParsFile ********************************/

/*
 * This checks that the DSAM global structure parameters file pointer has been
 * initialised.
 * It will set the pointer to the default value if it is unset.
 */

void
CheckInitParsFile_Common(void)
{
	if (dSAM.parsFile == UNSET_FILE_PTR)
		dSAM.parsFile = DEFAULT_PARS_FILE;

}

/*************************** CheckInitWarningsFile ****************************/

/*
 * This checks that the DSAM global structure warnings file pointer has been
 * initialised.
 * It will set the pointer to the default value if it is unset.
 */

void
CheckInitWarningsFile_Common(void)
{
	if (dSAM.warningsFile == UNSET_FILE_PTR)
		dSAM.warningsFile = DEFAULT_WARNINGS_FILE;

}

/*************************** FreeDoubleArray **********************************/

/*
 * This frees the memory for a double array, after first checking that it is not
 * set to NULL. 
 */

void
FreeDoubleArray_Common(double **p)
{
	if (*p == NULL)
		return;
	free(*p);
	*p = NULL;

}

/*************************** GetDSAMPtr ***************************************/

/*
 * This return returns the global DSAMPtr structure pointer. 
 */

DSAMPtr
GetDSAMPtr_Common(void)
{
	return(&dSAM);

}

/*************************** SwitchGUILocking *********************************/

/*
 * This return returns the global DSAMPtr structure pointer. 
 */

void
SwitchGUILocking_Common(BOOLN on)
{
	dSAM.lockGUIFlag = on;

}

