/**********************
 *
 * File:		MatInSignal.h
 * Purpose:		This routine contains the DSAM matlab/octave input signal class
 *				that initialises and converts the input signal.
 * Comments:	This was create for use by the scripting language interface
 *				RunDSAMSim.
 * Author:		L. P. O'Mard
 * Created:		05 Jul 2010
 * Updated:
 * Copyright:	(c) 2010, L. P. O'Mard
 *
 *********************/

#ifndef	MATINSIGNAL_H
#define MATINSIGNAL_H	1

#include <ExtMainApp.h>

/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

/******************************************************************************/
/*************************** enum Definitions *********************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Macro definitions ********************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Class definitions ********************************/
/******************************************************************************/

/*************************** InputSignal **************************************/

class MatInSignal {

	bool	vectorModeFlag;
	bool	staticTimeFlag;
	int		numChannels;
	int		interleaveLevel;
	ChanLen	length;
	Float	dt;
	Float	outputTimeOffset;
	Float	*inVector;
	Float	**inPtrs;
	EarObjectPtr	inputProcess;

public:
	MatInSignal(Float *inVector, int numChannels, int interleaveLevel,
		ChanLen length, Float dt, bool staticTimeFlag, Float outputTimeOffset);
	MatInSignal(Float **inPtrs, int numChannels, int interleaveLevel,
		ChanLen length, Float dt, bool staticTimeFlag, Float outputTimeOffset);
	~MatInSignal(void);

	Float	GetDt(void)	{ return dt; }
	EarObjectPtr	GetInputProcess(void)	{ return inputProcess; }
	ChanLen	GetLength(void)	{ return length; }
	bool	InitInputEarObject(ChanLen segmentLength);
	void	SetInputProcessData(ChanLen segmentLength);
	void	SetSignalPars(int numChannels, int interleaveLevel, ChanLen length,
			Float dt, bool staticTimeFlag, Float outputTimeOffset);
};


/******************************************************************************/
/*************************** Preference type definitions **********************/
/******************************************************************************/

/******************************************************************************/
/*************************** External Variables *******************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Global Subroutines *******************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS


__END_DECLS

#endif /* MATINSIGNAL_H */
