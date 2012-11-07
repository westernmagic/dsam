/**********************
 *
 * File:		MatLib.h
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

#ifndef	MATLIB_H
#define MATLIB_H	1

/* ----------------------------------------------------------------------------
 * Making or using MatLib as a Windows DLL
 * ----------------------------------------------------------------------------
 */

#if defined(_WINDOWS)

	/* __declspec works in BC++ 5 and later, as well as VC++ and gcc */
#if defined(__MSVISUALC__) || defined(__BORLANDC__) || defined(__GNUC__)
#  ifdef MATLIB_MAKING_DLL
#    define MATLIB_API __declspec( dllexport )
#  elif defined(MATLIB_USING_DLL)
#    define MATLIB_API __declspec( dllimport )
#  else
#    define MATLIB_API
#  endif
#else
#    define MATLIB_API
#endif

#elif defined(__PM__)

#  if (!(defined(__VISAGECPP__) && (__IBMCPP__ < 400 || __IBMC__ < 400 )))

#    ifdef MATLIB_MAKING_DLL
#      define MATLIB_API _Export
#    elif defined(MATLIB_USING_DLL)
#      define MATLIB_API _Export
#    else
#      define MATLIB_API
#    endif

#  else

#    define MATLIB_API

#  endif

#else  /* !(MSW or OS2) */

#  define MATLIB_API

#endif /* __WINDOWS */

#endif /* MATLIB_H */