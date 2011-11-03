/**********************
 *
 * File:		LibRunDSAMSimGen.h
 * Purpose:		This is the main routine for the scripting language interface
 * Comments:
 * Author:		L. P. O'Mard
 * Created:		01 Nov 2011
 * Updated:
 * Copyright:	(c) 2011, L. P. O'Mard
 *
 *********************/

#ifndef _LIBRUNDSAMSIMGEN_H
#define _LIBRUNDSAMSIMGEN_H


/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

SignalDataPtr	RunDSAMSim(WChar *simFile, WChar *parameterOptions, SignalDataPtr inSignal);

__END_DECLS

#endif /* _LIBRUNDSAMSIMGEN_H */
