dnl Process this file with autoconf to produce a configure script.
AC_REVISION($Revision: 2.13)
AC_INIT(MODGEN, 1.7.7)
AC_CONFIG_AUX_DIR(CfgTools)

AC_CANONICAL_SYSTEM

dnl we need to AC_DIVERT_PUSH/AC_DIVERT_POP these variable definitions so they
dnl are available for $ac_help expansion (don't we all *love* autoconf?)
dnl AC_DIVERT_PUSH(AC_DIVERSION_NOTICE)dnl
##
# Making releases: (Adopted from gnu glib.)
#   MODGEN_MICRO_VERSION += 1;
#   MODGEN_BINARY_AGE += 1;
# if the MICRO_VERSION has changed, then:
# set MODGEN_BINARY_AGE to zero.
#
MODGEN_MAJOR_VERSION=1
MODGEN_MINOR_VERSION=7
MODGEN_MICRO_VERSION=7
MODGEN_BINARY_AGE=0

dnl
dnl AC_DIVERT_POP()dnl

AC_SUBST(MODGEN_MAJOR_VERSION)
AC_SUBST(MODGEN_MINOR_VERSION)
AC_SUBST(MODGEN_MICRO_VERSION)
AC_SUBST(MODGEN_BINARY_AGE)
AC_DEFINE_UNQUOTED(MODGEN_VERSION,"$PACKAGE_VERSION", "ModGen version")

# RPM versioning
RPM_RELEASE=$MODGEN_MAJOR_VERSION.$MODGEN_MINOR_VERSION
RPM_AGE=$MODGEN_BINARY_AGE
AC_SUBST(RPM_RELEASE)
AC_SUBST(RPM_AGE)

MODGEN_PACKAGE=modgen
AC_DEFINE_UNQUOTED(MODGEN_PACKAGE,"$MODGEN_PACKAGE", "ModGen package")
AC_SUBST(MODGEN_PACKAGE)

AM_CONFIG_HEADER(MGSetup.h:MGSetup.hin )
AM_INIT_AUTOMAKE

dnl ============================================================================
dnl Initialize maintainer mode
dnl ============================================================================

AM_MAINTAINER_MODE

dnl ============================================================================
dnl Checks for programs.
dnl ============================================================================

AC_PROG_CC
AM_PROG_CC_STDC
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_YACC

dnl Checks for libraries.

dnl Checks for header files.
AC_HEADER_STDC
AC_EXEEXT
AC_OBJEXT

dnl Checks for typedefs, structures, and compiler characteristics.

dnl Checks for library functions.
AC_TYPE_SIGNAL

dnl ----------------------------------------------------------------------------
dnl debug option
dnl ----------------------------------------------------------------------------

AC_ARG_ENABLE(debug,
	[  --enable-debug          Turn on full debugging options],
	[case ${enableval} in
		yes)
			debugFlags="-ggdb3 -Wall"
			debug=true
			CFLAGS="${debugFlags} -Wstrict-prototypes"
			CXXFLAGS=${debugFlags}
			;;
		no)		debug=false ;;
		*)	AC_MSG_ERROR(bad value ${enableval} for --enable-debug) ;;
	esac], [debug=false])

AC_OUTPUT(
	modgen.spec
	CfgTools/Makefile
	MSVC/Makefile
	Makefile
)
