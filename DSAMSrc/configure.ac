dnl Process this file with autoconf to produce a configure script.
<<<<<<< configure.ac
dnl CVS ID $Id$
=======
dnl CVS ID $Id$
>>>>>>> 1.3
dnl Put in a check for 'bool' for CC and CXX.

AC_REVISION($Revision: 2.13)

AC_INIT(src/MainLib/GeCommon.c)
AC_CONFIG_AUX_DIR(CfgTools)

AC_CANONICAL_TARGET

dnl we need to AC_DIVERT_PUSH/AC_DIVERT_POP these variable definitions so they
dnl are available for $ac_help expansion (don't we all *love* autoconf?)
dnl AC_DIVERT_PUSH(AC_DIVERSION_NOTICE)dnl
##
# Making releases: (Adopted from gnu glib.)
#   DSAM_MICRO_VERSION += 1;
#   DSAM_INTERFACE_AGE += 1;
#   DSAM_BINARY_AGE += 1;
# if any (interface) functions have been added, set DSAM_INTERFACE_AGE to 0.
# if backwards compatibility has been broken,
# set DSAM_BINARY_AGE _and_ DSAM_INTERFACE_AGE to 0.
#
# Another good explanation of the libtool versioning
#
# +1 : ? : +1  == new interface that does not break old one
# +1 : ? : 0   == new interface that breaks old one
#  ? : ? : 0   == no new interfaces, but breaks apps
#  ? :+1 : ?   == just some internal changes, nothing breaks but might work
#		  better
# CURRENT: REVISION : AGE
#
DSAM_MAJOR_VERSION=2
<<<<<<< configure.ac
DSAM_MINOR_VERSION=7
DSAM_MICRO_VERSION=0
DSAM_INTERFACE_AGE=0
DSAM_BINARY_AGE=0
=======
DSAM_MINOR_VERSION=6
DSAM_MICRO_VERSION=37
DSAM_INTERFACE_AGE=9
DSAM_BINARY_AGE=9
>>>>>>> 1.3
DSAM_VERSION=$DSAM_MAJOR_VERSION.$DSAM_MINOR_VERSION.$DSAM_MICRO_VERSION
dnl
dnl AC_DIVERT_POP()dnl

AC_SUBST(DSAM_MAJOR_VERSION)
AC_SUBST(DSAM_MINOR_VERSION)
AC_SUBST(DSAM_MICRO_VERSION)
AC_SUBST(DSAM_INTERFACE_AGE)
AC_SUBST(DSAM_BINARY_AGE)
AC_DEFINE_UNQUOTED(DSAM_VERSION,"$DSAM_VERSION", "DSAM version")
AC_SUBST(DSAM_VERSION)

# libtool versioning
LT_RELEASE=$DSAM_MAJOR_VERSION.$DSAM_MINOR_VERSION
LT_CURRENT=`expr $DSAM_MICRO_VERSION - $DSAM_INTERFACE_AGE`
LT_REVISION=$DSAM_INTERFACE_AGE
LT_AGE=`expr $DSAM_BINARY_AGE - $DSAM_INTERFACE_AGE`
AC_SUBST(LT_RELEASE)
AC_SUBST(LT_CURRENT)
AC_SUBST(LT_REVISION)
AC_SUBST(LT_AGE)

DSAM_PACKAGE=dsam
AC_DEFINE_UNQUOTED(DSAM_PACKAGE,"$DSAM_PACKAGE", "DSAM Package name")
AC_SUBST(DSAM_PACKAGE)

AM_INIT_AUTOMAKE($DSAM_PACKAGE, $DSAM_VERSION)
AM_CONFIG_HEADER(DSAMSetup.h:DSAMSetup.hin )

dnl ============================================================================
dnl 'Global' variable declaration
dnl ============================================================================

WX_CONFIG=wx-config

dnl ============================================================================
dnl cross-compiling support: we're cross compiling if the build system is
dnl different from the target one (assume host and target be always the same)
dnl ============================================================================

if test $host != $build; then
	if test "$host_alias" != NONE; then
		CC=$host_alias-gcc
		CXX=$host_alias-c++
		AR=$host_alias-ar
		RANLIB=$host_alias-ranlib
		DLLTOOL=$host_alias-dlltool
		LD=$host_alias-ld
		NM=$host_alias-nm
		STRIP=$host_alias-strip
		WX_CONFIG=$host_alias-wx-config
	fi
fi

dnl Initialize maintainer mode
AM_MAINTAINER_MODE

dnl ============================================================================
dnl Libtool
dnl ============================================================================

AC_DISABLE_SHARED
AC_PROG_LIBTOOL
AC_SUBST(LIBTOOL_DEPS)

dnl ============================================================================
dnl Macros
dnl ============================================================================

dnl ---------------------------------------------------------------------------
dnl WX_CPP_BOOL checks whether the C++ compiler has a built in bool type
dnl
dnl call DSAM_CPP_BOOL - will define HAVE_BOOL if the compiler supports bool
dnl ---------------------------------------------------------------------------

AC_DEFUN(DSAM_CPP_BOOL,
[
  AC_CACHE_CHECK([if C++ compiler supports bool], wx_cv_cpp_bool,
  [
    AC_LANG_SAVE
    AC_LANG_CPLUSPLUS

    AC_TRY_COMPILE(
      [
      ],
      [
        bool b = true;

        return 0;
      ],
      [
        AC_DEFINE(HAVE_BOOL, "bool is defined for compiler")
        dsam_cv_cpp_bool=yes
      ],
      [
        dsam_cv_cpp_bool=no
      ]
    )

    AC_LANG_RESTORE
  ])

  if test "x$dsam_cv_cpp_bool" = xyes; then
    AC_DEFINE(HAVE_BOOL, "bool is defined for compiler")
  fi
])

dnl ---------------------------------------------------------------------------
dnl a slightly better AC_C_BIGENDIAN macro which allows cross-compiling
dnl ---------------------------------------------------------------------------

AC_DEFUN(DSAM_C_BIGENDIAN,
[AC_CACHE_CHECK(whether byte ordering is bigendian, ac_cv_c_bigendian,
[ac_cv_c_bigendian=unknown
# See if sys/param.h defines the BYTE_ORDER macro.
AC_TRY_COMPILE([#include <sys/types.h>
#include <sys/param.h>], [
#if !BYTE_ORDER || !BIG_ENDIAN || !LITTLE_ENDIAN
 bogus endian macros
#endif], [# It does; now see whether it defined to BIG_ENDIAN or not.
AC_TRY_COMPILE([#include <sys/types.h>
#include <sys/param.h>], [
#if BYTE_ORDER != BIG_ENDIAN
 not big endian
#endif], ac_cv_c_bigendian=yes, ac_cv_c_bigendian=no)])
if test $ac_cv_c_bigendian = unknown; then
AC_TRY_RUN([main () {
  /* Are we little or big endian?  From Harbison&Steele.  */
  union
  {
    long l;
    char c[sizeof (long)];
  } u;
  u.l = 1;
  exit (u.c[sizeof (long) - 1] == 1);
}], ac_cv_c_bigendian=no, ac_cv_c_bigendian=yes, ac_cv_c_bigendian=unknown)
fi])
if test $ac_cv_c_bigendian = unknown; then
  AC_MSG_WARN([Assuming little-endian target machine - this may be overriden by adding the line "ac_cv_c_bigendian=${ac_cv_c_bigendian='yes'}" to config.cache file])
fi
if test $ac_cv_c_bigendian = yes; then
  AC_DEFINE(WORDS_BIGENDIAN)
fi
])

dnl ============================================================================
dnl My optional features that need to be done before program checks.
dnl ============================================================================

dnl ----------------------------------------------------------------------------
dnl debug option
dnl ----------------------------------------------------------------------------

AC_ARG_ENABLE(debug,
	[  --enable-debug          Turn on full debugging options],
	[case ${enableval} in
		yes)
			debugFlags="-ggdb3 -Wall -Wstrict-prototypes"
			debug=true
			CFLAGS=${debugFlags}
			CXXFLAGS=${debugFlags}
			;;
		no)		debug=false ;;
		*)	AC_MSG_ERROR(bad value ${enableval} for --enable-debug) ;;
	esac], [debug=false])

dnl ============================================================================
dnl Checks for types
dnl ============================================================================

AC_CHECK_TYPES([int16, int32])

dnl ============================================================================
dnl Checks for programs.
dnl ============================================================================

#AC_PROG_CC
AC_PROG_CPP
AC_PROG_MAKE_SET
AC_PROG_RANLIB
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_YACC
AM_PROG_CC_STDC
AC_PROG_AWK
 
dnl ============================================================================
dnl Checks for libraries.
dnl ============================================================================

dnl ============================================================================
dnl Checks for header files.
dnl ============================================================================

AM_C_PROTOTYPES
AC_HEADER_STDC
AC_CHECK_HEADERS(malloc.h)
AC_PATH_X
AC_PATH_XTRA

dnl ============================================================================
dnl Checks for my header files.
dnl ============================================================================

AC_MSG_CHECKING(for CHIMP environment variables)
if test $CHIMPHOME && test $CHIMP_LIB; then
	mpiAvailable=yes
else
	mpiAvailable=no
fi
AC_MSG_RESULT($mpiAvailable)

dnl ============================================================================
dnl Checks for typedefs, structures, and compiler characteristics.
dnl ============================================================================

AC_C_CONST
AC_STRUCT_TM
AC_LANG_CPLUSPLUS

dnl ============================================================================
dnl Checks for library functions.
dnl ============================================================================

AC_FUNC_VPRINTF
AC_CHECK_FUNCS(strstr)
dnl AC_FUNC_ALLOCA

dnl ============================================================================
dnl Optional features.
dnl ============================================================================

 
dnl ============================================================================
dnl My optional features.
dnl ============================================================================

dnl ----------------------------------------------------------------------------
dnl debug option
dnl ----------------------------------------------------------------------------

AC_ARG_ENABLE(dmalloc,
	[  --enable-dmalloc        Use DMalloc for debugging.],
	[case ${enableval} in
		yes)
			if test x$debug = xfalse; then
				AC_MSG_ERROR(--enable-debug must also be used!)
			fi
			dmalloc=true
			;;
		no)		dmalloc=false ;;
		*)	AC_MSG_ERROR(bad value ${enableval} for --enable-dmalloc) ;;
	esac], [dmalloc=false])
	AM_CONDITIONAL(DMALLOC, test x$dmalloc = xtrue)

dnl ----------------------------------------------------------------------------
dnl wxport
dnl ----------------------------------------------------------------------------

AC_ARG_ENABLE(wxport,
	[  --enable-wxport=ARG     Specify WxWin port [ARG=gtk]],
	[case ${enableval} in
		motif)	AC_DEFINE(wx_motif, 1, "Using motif port") ;;
		msw)	AC_DEFINE(wx_msw, 1, "Using MSW port") ;;
		gtk)	AC_DEFINE(wx_gtk, 1, "Using GTK port") ;;
		*)	AC_MSG_ERROR(bad value ${enableval} for --enable-wxport) ;;
	esac], wxport=gtk)

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
	AC_CHECK_HEADERS(wx/ogl/ogl.h, [ogl=true], [ogl=false])
	if test x$ogl = xtrue; then
		ogl_ldflags="-logl"
	fi
fi

XINCLUDE=$wx_xinclude
AC_SUBST(XINCLUDE)
WXINC=$wx_inc
AC_SUBST(WXINC)
WXBASEINC=$wx_baseinc
AC_SUBST(WXBASEINC)
WXLDFLAGS="$wx_ldflags $ogl_ldflags"
AC_SUBST(WXLDFLAGS)

case "${wxport}" in
	motif)	WXLDLIBS=$wx_motifldlibs
		;;
	msw)	RESFLAGS=$wx_resflags
			AC_SUBST(RESFLAGS)
		;;
	mswg95)	RESFLAGS=$wx_resflags
			AC_SUBST(RESFLAGS)
			RESCOMP=$wx_rescomp
		;;
	*)
		;;
esac

AC_SUBST(WXLDLIBS)

AM_CONDITIONAL(WXGTK, test x$wxport = xgtk)
AM_CONDITIONAL(WXMSW, test x$wxport = xmsw)

dnl ----------------------------------------------------------------------------
dnl Graphics library option
dnl ----------------------------------------------------------------------------

AC_ARG_ENABLE(graphics,
	[  --enable-graphics=ARG   Enable DSAM Graphics support [ARG=no]],
	[case ${enableval} in
		yes)
			if test x$wxwinAvailable = xyes; then
				AC_PROG_CXX
				AC_PROG_CXXCPP
				DSAM_GRAPHICS="-DGRAPHICS_SUPPORT"
				AC_DEFINE(GRAPHICS_SUPPORT, 1, 'Using Graphics Support")
				GR_LIB=GrLib
				graphics=true
			else
				AC_MSG_ERROR(WxWin must be installed!)
				graphics=false
			fi
			;;
		*)	graphics=false ;;
	esac], [ graphics=false ])
AM_CONDITIONAL(DSAM_GRAPHICS, test x$graphics = xtrue)

dnl ----------------------------------------------------------------------------
dnl Parallel library option
dnl ----------------------------------------------------------------------------

DSAM_MPI=
no_MPI_Environment="Cannot enable MPI support - CHIMPHOME and CHIMP_LIB\
 environment variables must be set!"
AC_ARG_ENABLE(parallel,
	[  --enable-parallel=ARG   Enable DSAM parallel support [ARG=no]],
	[parallel=${enableval}
	if test x$mpiAvailable = xyes; then
		DSAM_MPI="-DMPI_SUPPORT"
		mpi=true
		LIBDSAMM=libdsam_m.la
		PAWORKER1M=PaWorker1m.exe
	else
		AC_MSG_ERROR($no_MPI_Environment)
	fi
	], [ mpi=no ])

DSAM_DEFS="$DSAM_GRAPHICS $DSAM_MPI"
AC_SUBST(DSAM_GRAPHICS)
AC_SUBST(DSAM_MPI)
AC_SUBST(DSAM_DEFS)

dnl ============================================================================
dnl Checks for system services or capabilities.
dnl ============================================================================

AC_EXEEXT
AC_OBJEXT
AC_SYS_LONG_FILE_NAMES

AM_CONDITIONAL(MPI, test x$mpi = xyes)
AM_CONDITIONAL(WXWIN, test x$graphics = xyes)
AC_SUBST(LIBDSAM)
AC_SUBST(LIBDSAMM)
AC_SUBST(PAWORKER1M)
AC_SUBST(GR_LIB)
AC_SUBST(MINGW32)

dnl ============================================================================
dnl Final configure outputs.
dnl ============================================================================

AC_OUTPUT([
	dsam.spec
	CfgTools/Makefile
	Makefile
	Include/Makefile
	src/Makefile
	src/MainLib/Makefile
	src/StdLib/Makefile
	src/GrLib/Makefile
	src/GrLib/Bitmaps/Makefile
	ModuleSpecs/Makefile
	Examples/Makefile
	dsam-config
	MSVC/Makefile
	MSVC/DSAM/Makefile
	MSVC/DSAM_std/Makefile
	MSVC/DSAM_g/Makefile
],[chmod +x dsam-config])
