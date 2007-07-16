dnl Process this file with autoconf to produce a configure script.
dnl CVS ID $Id: configure.ac,v 1.7 2003/08/12 12:50:53 lowel Exp $
dnl Put in a check for 'bool' for CC and CXX.

AC_REVISION($Revision: 2.13)

AC_INIT(feval.h)
AC_CONFIG_AUX_DIR(CfgTools)

AC_CANONICAL_HOST

dnl we need to AC_DIVERT_PUSH/AC_DIVERT_POP these variable definitions so they
dnl are available for $ac_help expansion (don't we all *love* autoconf?)
dnl AC_DIVERT_PUSH(AC_DIVERSION_NOTICE)dnl
##
# Making releases: (Adopted from gnu glib.)
#   FEVAL_MICRO_VERSION += 1;
#   FEVAL_BINARY_AGE += 1;
# if the MICRO_VERSION has changed, then:
# set FEVAL_BINARY_AGE to zero.
#
FEVAL_MAJOR_VERSION=0
FEVAL_MINOR_VERSION=2
FEVAL_MICRO_VERSION=4
FEVAL_INTERFACE_AGE=4
FEVAL_BINARY_AGE=4
FEVAL_VERSION=$FEVAL_MAJOR_VERSION.$FEVAL_MINOR_VERSION.$FEVAL_MICRO_VERSION
dnl
dnl AC_DIVERT_POP()dnl

AC_SUBST(FEVAL_MAJOR_VERSION)
AC_SUBST(FEVAL_MINOR_VERSION)
AC_SUBST(FEVAL_MICRO_VERSION)
AC_SUBST(FEVAL_BINARY_AGE)
AC_DEFINE_UNQUOTED(FEVAL_VERSION,wxT("$FEVAL_VERSION"), "FEVAL version")
AC_SUBST(FEVAL_VERSION)

# RPM versioning
RPM_RELEASE=$FEVAL_MAJOR_VERSION.$FEVAL_MINOR_VERSION
RPM_AGE=$FEVAL_BINARY_AGE
AC_SUBST(RPM_RELEASE)
AC_SUBST(RPM_AGE)

FEVAL_PACKAGE=feval
AC_DEFINE_UNQUOTED(FEVAL_PACKAGE,"$FEVAL_PACKAGE", "FEVAL_PACKAGE")
AC_SUBST(FEVAL_PACKAGE)
AM_CONFIG_HEADER(FEvalSetup.h:FEvalSetup.hin )

AM_INIT_AUTOMAKE($FEVAL_PACKAGE, $FEVAL_VERSION, NO-DEFINE)

dnl ============================================================================
dnl 'Global' variable declaration
dnl ============================================================================

DSAM_CONFIG=dsam-config
WX_CONFIG=wx-config
dsam_rescomp=windres

dnl ============================================================================
dnl cross-compiling support: we're cross compiling if the build system is
dnl different from the target one (assume host and target be always the same)
dnl ============================================================================

if test $host != $build; then
	if test "$host_alias" != NONE; then
		CC=$host_alias-gcc
		CXX=$host_alias-c++
		AR=$host_alias-ar
		DLLTOOL=$host_alias-dlltool
		LD=$host_alias-ld
		NM=$host_alias-nm
		STRIP=$host_alias-strip
		DSAM_CONFIG=$host_alias-$DSAM_CONFIG
		dsam_rescomp=$host_alias-$dsam_rescomp
	fi
fi

dnl ============================================================================
dnl Macros
dnl ============================================================================

dnl ---------------------------------------------------------------------------
dnl WX_CPP_BOOL checks whether the C++ compiler has a built in bool type
dnl
dnl call FEVAL_CPP_BOOL - will define HAVE_BOOL if the compiler supports bool
dnl ---------------------------------------------------------------------------

AC_DEFUN([FEVAL_CPP_BOOL],
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
dnl Initialize maintainer mode
dnl ============================================================================

AM_MAINTAINER_MODE

dnl ============================================================================
dnl Checks for programs.
dnl ============================================================================

AC_PROG_CC
AC_PROG_CPP
AC_PROG_CXX
AC_PROG_CXXCPP
AC_PROG_MAKE_SET
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_YACC
AM_PROG_CC_STDC
AC_PROG_AWK
 
dnl ============================================================================
dnl Checks for system services or capabilities.
dnl ============================================================================

AC_EXEEXT
AC_OBJEXT
AC_SYS_LONG_FILE_NAMES

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

 
dnl ----------------------------------------------------------------------------
dnl My optional features.
dnl ----------------------------------------------------------------------------

AC_MSG_CHECKING(for DSAM environment ($DSAM_CONFIG))
if eval "$DSAM_CONFIG --version 2> config.log > /dev/null"; then
	dsamAvailable=yes
else
	dsamAvailable=no
fi

AC_MSG_RESULT($dsamAvailable)

if test x$dsamAvailable = xno; then
	AC_MSG_ERROR(DSAM library must be installed)
fi

dsam_inc=`$DSAM_CONFIG --cflags`
dsam_ldflags=`$DSAM_CONFIG --libs`

AC_MSG_CHECKING(for DSAM graphics support)
if eval "$DSAM_CONFIG --gui 2> config.log > /dev/null"; then
	graphics=yes
	dsam_gui_inc=`$DSAM_CONFIG --gui --cflags`
	dsam_gui_ldflags=`$DSAM_CONFIG --gui --libs`
	FEVAL=feval
	AC_SUBST(FEVAL)
else
	graphics=no
fi
AC_MSG_RESULT($graphics)

if test x$MINGW32 = xyes; then
	dsam_res_flags=`$DSAM_CONFIG --rflags`
fi

DSAMINC=$dsam_inc
AC_SUBST(DSAMINC)
DSAMLDFLAGS=$dsam_ldflags
AC_SUBST(DSAMLDFLAGS)
DSAMGUIINC=$dsam_gui_inc
AC_SUBST(DSAMGUIINC)
DSAMGUILDFLAGS=$dsam_gui_ldflags
AC_SUBST(DSAMGUILDFLAGS)

if test x$MINGW32 = xyes; then
	FEVALRESOBJ=feval_res.$OBJEXT
	AC_SUBST(FEVALRESOBJ)
fi

AC_SUBST(MINGW32)
RESCOMP=$dsam_rescomp
AC_SUBST(RESCOMP)
RESFLAGS=$dsam_res_flags
AC_SUBST(RESFLAGS)

dnl ============================================================================
dnl Final configure outputs.
dnl ============================================================================

AC_OUTPUT(
	Makefile
	feval.spec
	CfgTools/Makefile
	GammaC/Makefile
	Matlab/Makefile
	Octave/Makefile
	HelpFiles/Makefile
)
