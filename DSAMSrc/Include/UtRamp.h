/***********************
 *
 * UtRamp.h
 *
 * This is the header file for the signal ramp module.
 *
 * 29-3-93 L. P. O'Mard
 *
 ***********************/

#ifndef	_UTRAMP_H
#define _UTRAMP_H	1
 
/******************************************************************************/
/*************************** Type definitions *********************************/
/******************************************************************************/

/******************************************************************************/
/*************************** Function Prototypes ******************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

void	RampUpOutSignal_Ramp(EarObjectPtr data,
		  double (* RampFunction)(ChanLen, ChanLen), double timeInterval);

void	RampDownOutSignal_Ramp(EarObjectPtr data,
		   double (* RampFunction)(ChanLen, ChanLen), double timeInterval);

BOOLN	CheckPars_Ramp(SignalData *theSignal, double timeInterval);

double	Sine_Ramp(ChanLen step, ChanLen intervalIndex);

double	Linear_Ramp(ChanLen step, ChanLen intervalIndex);

__END_DECLS

#endif
