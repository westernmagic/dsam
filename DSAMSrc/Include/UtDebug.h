/*******
 *
 * UtDebug.h
 *
 * This header file is for the debug module used in testing.
 *
 * by Lowel O'Mard 26-10-93.
 *
 ********/

/*
 * The following include files enable me to use WriteOutSignal_DataFile
 * while only adding the UtDebug.h header.
 */

/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/
 

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

/******************************************************************************/
/*************************** External Variables *******************************/
/******************************************************************************/

extern EarObjectPtr	debug;		/* This is used for module debugging. */

/******************************************************************************/
/*************************** Global Subroutines *******************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

void		Free_Debug(void);

void		Init_Debug(void);

BOOLN	CheckInit_Debug(void);

BOOLN	SetChannelValue_Debug(int channel, ChanLen sample, double value);

BOOLN	SetPars_Debug(int numChannels, ChanLen length,
			  double samplingInterval);

__END_DECLS

BOOLN	WriteOutSignal_Debug(char *fileName);
