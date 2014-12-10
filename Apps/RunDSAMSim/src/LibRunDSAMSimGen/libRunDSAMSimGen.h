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

/******************************************************************************/
/*************************** Constant Definitions *****************************/
/******************************************************************************/

/* ----------------------------------------------------------------------------
 * Making or using LIBRUNDSAMSIMGEN as a Windows DLL
 * ----------------------------------------------------------------------------
 */

#if defined(_WINDOWS)

	/* __declspec works in BC++ 5 and later, as well as VC++ and gcc */
#if defined(__MSVISUALC__) || defined(__BORLANDC__) || defined(__GNUC__)
#  ifdef LIBRUNDSAMSIMGEN_MAKING_DLL
#    define LIBRUNDSAMSIMGEN_API __declspec( dllexport )
#  else
#    define LIBRUNDSAMSIMGEN_API
#  endif
#else
#    define LIBRUNDSAMSIMGEN_API
#endif

#elif defined(__PM__)

#  if (!(defined(__VISAGECPP__) && (__IBMCPP__ < 400 || __IBMC__ < 400 )))

#    ifdef LIBRUNDSAMSIMGEN_MAKING_DLL
#      define LIBRUNDSAMSIMGEN_API _Export
#    else
#      define LIBRUNDSAMSIMGEN_API
#    endif

#  else

#    define LIBRUNDSAMSIMGEN_API

#  endif

#else  /* !(MSW or OS2) */

#  define LIBRUNDSAMSIMGEN_API

#endif /* __WINDOWS */

/******************************************************************************/
/****************************** Type definitions ******************************/
/******************************************************************************/

/******************************************************************************/
/****************************** External variables ****************************/
/******************************************************************************/

extern SignalDataPtr	outSignal;

/******************************************************************************/
/****************************** Function Prototypes ***************************/
/******************************************************************************/

/* C Declarations.  Note the use of the '__BEGIN_DECLS' and '__BEGIN_DECLS'
 * macros, to allow the safe use of C libraries with C++ libraries - defined
 * in GeCommon.h.
 */
__BEGIN_DECLS

LIBRUNDSAMSIMGEN_API SignalDataPtr	RunDSAMSim(WChar *simFile,
										WChar *parameterOptions = (WChar *) wxT(""),
										SignalDataPtr inSignal = NULL);

__END_DECLS

#endif /* _LIBRUNDSAMSIMGEN_H */
