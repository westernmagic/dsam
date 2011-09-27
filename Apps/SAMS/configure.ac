dnl Process this file with autoconf to produce a configure script.
dnl CVS ID $Id$
dnl Put in a check for 'bool' for CC and CXX.

AC_REVISION($Revision: 2.13)

AC_INIT(SAMS, 1.3.5)
AC_CONFIG_AUX_DIR(CfgTools)

AC_CANONICAL_HOST

dnl we need to AC_DIVERT_PUSH/AC_DIVERT_POP these variable definitions so they
dnl are available for $ac_help expansion (don't we all *love* autoconf?)
dnl AC_DIVERT_PUSH(AC_DIVERSION_NOTICE)dnl
##
# Making releases: (Adopted from gnu glib.)
#   SAMS_MICRO_VERSION += 1;
#   SAMS_BINARY_AGE += 1;
# if the MICRO_VERSION has changed, then:
# set SAMS_BINARY_AGE to zero.
#
SAMS_INTERFACE_AGE=0
SAMS_BINARY_AGE=0
SAMS_RPM_AGE=0
dnl
dnl AC_DIVERT_POP()dnl

AC_DEFINE_UNQUOTED(SAMS_VERSION,wxT("$PACKAGE_VERSION"), "SAMS version")

# RPM versioning
RPM_RELEASE=$SAMS_MAJOR_VERSION.$SAMS_MINOR_VERSION
RPM_AGE=$SAMS_RPM_AGE
AC_SUBST(RPM_RELEASE)
AC_SUBST(RPM_AGE)

SAMS_PACKAGE=sams
AC_DEFINE_UNQUOTED(SAMS_PACKAGE,"$SAMS_PACKAGE", "SAMS_PACKAGE")
AC_SUBST(SAMS_PACKAGE)
AM_CONFIG_HEADER(SAMSSetup.h:SAMSSetup.hin )

AM_INIT_AUTOMAKE([-Wall -Werror])

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
dnl call SAMS_CPP_BOOL - will define HAVE_BOOL if the compiler supports bool
dnl ---------------------------------------------------------------------------

AC_DEFUN([SAMS_CPP_BOOL],
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

AC_MSG_CHECKING([for DSAM environment ($DSAM_CONFIG)])
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
	SAMS=sams
	AC_SUBST(SAMS)
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
	SAMSRESOBJ=sams_res.$OBJEXT
	AC_SUBST(SAMSRESOBJ)
fi

AC_SUBST(MINGW32)
RESCOMP=$dsam_rescomp
AC_SUBST(RESCOMP)
RESFLAGS=$dsam_res_flags
AC_SUBST(RESFLAGS)

dnl ----------------------------------------------------------------------------
dnl Find server port
dnl ----------------------------------------------------------------------------

AC_MSG_CHECKING(configured server port)
cat >> get_port.$ac_ext << _DSAM_EOF
#include <stdio.h>

#define	HEADER_ONLY		1

#include "GeCommon.h"
#include "GeSignalData.h"
#include "GeEarObject.h"
#include "ExtIPCUtils.h"

int main(void)
{
	printf("%d\n", EXTIPCUTILS_DEFAULT_SERVER_PORT);
	return(0);
}
_DSAM_EOF

$CXX get_port.$ac_ext `$DSAM_CONFIG  --cflags --libs` -DHEADER_ONLY
SERVER_PORT=`./a.out`
AC_MSG_RESULT($SERVER_PORT)
rm -f get_port.$ac_ext
AC_SUBST(SERVER_PORT)

dnl ============================================================================
dnl Final configure outputs.
dnl ============================================================================

AC_OUTPUT(
	Makefile
	sams.spec
	sams.desktop
	manpage.xml
	CfgTools/Makefile
	DEFAULT/Makefile
	HelpFiles/Makefile
	Matlab/Makefile
	MSVC/Makefile
	MSVC/SAMS/Makefile
	MSVC/SAMS_ng/Makefile
	debian/Makefile
	Octave/Makefile
	Tutorials/Makefile
	Tutorials/ANFibreModels/Makefile
	Tutorials/ANFibreModels/ANmodelPSTH/Makefile
	Tutorials/ANFibreModels/ANspikeStream/Makefile
	Tutorials/ANFibreModels/ANmodelISIH/Makefile
	Tutorials/ANFibreModels/ANmodelPeriodH/Makefile
	Tutorials/AuditoryNerve/Makefile
	Tutorials/AuditoryPeriphery/Makefile
	Tutorials/Autocorrelation/Makefile
	Tutorials/BinauralPitch/Makefile
	Tutorials/DRNLinputOutput/Makefile
	Tutorials/GenerateStimulus/Makefile
	Tutorials/ImpulseResponse/Makefile
	Tutorials/Loudness/Makefile
	Tutorials/MeasureIntensity/Makefile
	Tutorials/Pre-emphasisFilter/Makefile
	Tutorials/Precedence/Makefile
)
