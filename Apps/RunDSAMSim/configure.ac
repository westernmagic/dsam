dnl Process this file with autoconf to produce a configure script.
dnl CVS ID $Id: configure.ac 957 2006-05-22 12:33:32Z lowel $
dnl Put in a check for 'bool' for CC and CXX.

AC_REVISION($Revision: 2.13)

AC_INIT(RunDSAMSim, 1.1.8)
AC_CONFIG_AUX_DIR(CfgTools)

AC_CANONICAL_HOST

dnl we need to AC_DIVERT_PUSH/AC_DIVERT_POP these variable definitions so they
dnl are available for $ac_help expansion (don't we all *love* autoconf?)
dnl AC_DIVERT_PUSH(AC_DIVERSION_NOTICE)dnl
##
# Making releases: (Adopted from gnu glib.)
#   RUNDSAMSIM_MICRO_VERSION += 1;
#   RUNDSAMSIM_BINARY_AGE += 1;
# if the MICRO_VERSION has changed, then:
# set RUNDSAMSIM_BINARY_AGE to zero.
#
RUNDSAMSIM_MAJOR_VERSION=1
RUNDSAMSIM_MINOR_VERSION=1
RUNDSAMSIM_MICRO_VERSION=8
RUNDSAMSIM_INTERFACE_AGE=8
RUNDSAMSIM_BINARY_AGE=8
RUNDSAMSIM_RPM_AGE=1

dnl
dnl AC_DIVERT_POP()dnl

AC_SUBST(RUNDSAMSIM_MAJOR_VERSION)
AC_SUBST(RUNDSAMSIM_MINOR_VERSION)
AC_SUBST(RUNDSAMSIM_MICRO_VERSION)
AC_SUBST(RUNDSAMSIM_BINARY_AGE)
AC_DEFINE_UNQUOTED(RUNDSAMSIM_VERSION, wxT("$PACKAGE_VERSION"), "RunDSAMSim version")

# RPM versioning
RPM_RELEASE=$RUNDSAMSIM_MAJOR_VERSION.$RUNDSAMSIM_MINOR_VERSION
RPM_AGE=$RUNDSAMSIM_RPM_AGE
AC_SUBST(RPM_RELEASE)
AC_SUBST(RPM_AGE)

RUNDSAMSIM_PACKAGE=RunDSAMSim
AC_DEFINE_UNQUOTED(RUNDSAMSIM_PACKAGE,"$RUNDSAMSIM_PACKAGE", "RUNDSAMSIM_PACKAGE")
AC_SUBST(RUNDSAMSIM_PACKAGE)
AM_CONFIG_HEADER(RunDSAMSimSetup.h:RunDSAMSimSetup.hin )

AM_INIT_AUTOMAKE

dnl ============================================================================
dnl 'Global' variable declaration
dnl ============================================================================

DSAM_CONFIG=dsam-config
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
dnl call RUNDSAMSIM_CPP_BOOL - will define HAVE_BOOL if the compiler supports bool
dnl ---------------------------------------------------------------------------

AC_DEFUN([RUNDSAMSIM_CPP_BOOL],
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

AC_PROG_CPP
AC_PROG_CXX
AC_PROG_CXXCPP
AC_PROG_MAKE_SET
AC_PROG_INSTALL
AC_PROG_LN_S
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

dnl ----------------------------------------------------------------------------
dnl DSAM library
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

dsam_inc=`$DSAM_CONFIG --cflags | sed s/-pthread/-lpthread/ | sed s/-fPIC//`
dsam_ldflags=`$DSAM_CONFIG --libs | sed s/-pthread/-lpthread/`

dnl ----------------------------------------------------------------------------
dnl Octave development environment.
dnl ----------------------------------------------------------------------------

OCTCOMPILE="mkoctfile"

AC_MSG_CHECKING(Octave Development environment ($OCTCOMPILE))
if eval "$OCTCOMPILE --version 2>> config.log > /dev/null"; then
	octaveAvailable=yes
else
	octaveAvailable=no
fi

AC_MSG_RESULT($octaveAvailable)


dnl ============================================================================
dnl Final setups.
dnl ============================================================================

if test x$octaveAvailable = xno -a x$matlabAvailable = xno; then
	AC_MSG_ERROR(Octave or Matlab must be installed)
else
	if test x$matlabAvailable = xyes; then
		DLLCOMPILE=$MATCOMPILE
		DLLEXT=".mexglx"
		dll_inc=""
		DLLDIRNAME=matlab/dsam
	else
		DLLCOMPILE=$OCTCOMPILE
		DLLEXT=".mex"
		dll_inc=`mkoctfile -p INCFLAGS`
		DLLDIR=octave/dsam
	fi
	AC_SUBST(DLLCOMPILE)
	AC_SUBST(DLLEXT)
	AC_SUBST(DLLDIR)
fi
DSAMINC="$dsam_inc $dll_inc"
AC_SUBST(DSAMINC)
DSAMLDFLAGS=$dsam_ldflags
AC_SUBST(DSAMLDFLAGS)

dnl ============================================================================
dnl Final configure outputs.
dnl ============================================================================

AC_CONFIG_FILES(
	Makefile
	RunDSAMSim.spec
	CfgTools/Makefile
	MSVC/Makefile
	Examples/Makefile
)
AC_OUTPUT

