dnl Process this file with autoconf to produce a configure script.
AC_REVISION($Revision: 2.61)

AC_INIT(ogl, 2.4.2)
AC_CONFIG_AUX_DIR(CfgTools)
AC_CONFIG_MACRO_DIRS([m4])
AC_CANONICAL_TARGET

AM_INIT_AUTOMAKE([-Wall -Werror])
AM_CONFIG_HEADER(OGLSetup.h:OGLSetup.hin )
AM_MAINTAINER_MODE

dnl ============================================================================
dnl 'Global' variable declaration
dnl ============================================================================

WX_CONFIG=wx-config

dnl ============================================================================
dnl Preliminary checks for programs.
dnl ============================================================================

AC_PROG_AWK
 
dnl ============================================================================
dnl Version declarations
dnl ============================================================================

AC_MSG_CHECKING(for WXWIN environment ($WX_CONFIG))
if eval "$WX_CONFIG --version 2> config.log > /dev/null"; then
	wxwinAvailable=yes
else
	wxwinAvailable=no
fi
AC_MSG_RESULT($wxwinAvailable)

if test x$wxwinAvailable = xno; then
	AC_MSG_ERROR(wxWin library must be installed)
fi

dnl OGL_VERSION=`$WX_CONFIG --version`
AC_DEFINE_UNQUOTED(OGL_VERSION,"$OGL_VERSION", "ogl version")
AC_SUBST(OGL_VERSION)

PREFIX=`$WX_CONFIG --prefix`
AC_PREFIX_DEFAULT($PREFIX)

LIBVERSION_CURRENT=6
AC_SUBST(LIBVERSION_CURRENT)
LIBVERSION_REVISION=2
AC_SUBST(LIBVERSION_REVISION)
LIBVERSION_AGE=6
AC_SUBST(LIBVERSION_AGE)
LT_RELEASE=`$WX_CONFIG --version | awk -F . '{print $1 "." $2}'`
AC_SUBST(LT_RELEASE)

dnl ============================================================================
dnl Libtool
dnl ============================================================================

AM_DISABLE_SHARED
AM_PROG_LIBTOOL

dnl ============================================================================
dnl Checks for programs.
dnl ============================================================================

AC_PROG_CXX
AC_PROG_CPP
AC_PROG_MAKE_SET
dnl AC_PROG_RANLIB
AC_PROG_INSTALL
AC_PROG_LN_S
 
dnl ----------------------------------------------------------------------------
dnl wxport
dnl ----------------------------------------------------------------------------

dnl AC_ARG_ENABLE(wxport,
dnl 	[  --enable-wxport=ARG     Specify WxWin port [ARG=gtk]],
dnl 	[case ${enableval} in
dnl 		motif)	AC_DEFINE(wx_motif) ;;
dnl 		msw)	AC_DEFINE(wx_msw) ;;
dnl 		xview)	AC_DEFINE(wx_xview) ;;
dnl 		gtk)	AC_DEFINE(wx_gtk) ;;
dnl 		*)	AC_MSG_ERROR(bad value ${enableval} for --enable-wxport) ;;
dnl 	esac], wxport=gtk)

AC_MSG_CHECKING(for WXWIN environment ($WX_CONFIG))
if eval "$WX_CONFIG --version 2> config.log > /dev/null"; then
	wxwinAvailable=yes
else
	wxwinAvailable=no
fi

AC_MSG_RESULT($wxwinAvailable)

if test x$wxwinAvailable = xyes; then
	wx_inc=`$WX_CONFIG --cxxflags`
	wx_ldflags=`$WX_CONFIG --libs`
	wx_resflags="--include-dir `$WX_CONFIG --exec-prefix`/include \
	  --include-dir `$WX_CONFIG --exec-prefix`/$host_alias/include"
fi

XINCLUDE=$wx_xinclude
AC_SUBST(XINCLUDE)
WXINC=$wx_inc
AC_SUBST(WXINC)
WXBASEINC=$wx_baseinc
AC_SUBST(WXBASEINC)
WXLDFLAGS=$wx_ldflags
AC_SUBST(WXLDFLAGS)

AC_SUBST(WXLDLIBS)

dnl ---------------
dnl ogl specific
dnl ---------------


dnl -----------
dnl File output


AC_CONFIG_FILES([
	Makefile
	include/Makefile
	src/Makefile
	ogl.spec
	samples/Makefile
	samples/ogledit/Makefile
	samples/ogledit/bitmaps/Makefile
	samples/studio/Makefile
	samples/studio/bitmaps/Makefile
	samples/studio/manual/Makefile
])

AC_OUTPUT

