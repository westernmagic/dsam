dnl Process this file with autoconf to produce a configure script.
dnl CVS ID $Id: configure.ac 957 2006-05-22 12:33:32Z lowel $
dnl Put in a check for 'bool' for CC and CXX.

AC_REVISION($Revision: 2.13)

AC_INIT(RunDSAMSim, 2.1.2)
AC_CONFIG_AUX_DIR(CfgTools)
AC_CONFIG_MACRO_DIR(m4)

AC_CANONICAL_HOST
AC_CANONICAL_TARGET

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
RUNDSAMSIM_BINARY_AGE=1
RUNDSAMSIM_RPM_AGE=1


dnl
dnl AC_DIVERT_POP()dnl

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

AM_INIT_AUTOMAKE([-Wall -Werror])
AM_PATH_PYTHON([2.5])

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
dnl Libtool
dnl ============================================================================

AC_DISABLE_SHARED
AC_PROG_LIBTOOL
AC_SUBST(LIBTOOL_DEPS)

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
AM_PROG_GCJ
AC_PROG_JAR
AC_PROG_JAVAH
 
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

AC_ARG_ENABLE(java_support,
   [  --enable-java-support  Produce RunDSAMSimJ java interface],
   [javaSupport="$enableval"],
   [javaSupport="no"]
)

AC_ARG_ENABLE(python_support,
   [  --enable-python-support  Install RunDSAMSim python interface],
   [pythonSupport="$enableval"],
   [pythonSupport="no"]
)

AC_ARG_ENABLE(generic_support,
   [  --enable-generic-support  Produce LibRunDSAMSimGen generic DLL interface],
   [genericDLLSupport="$enableval"],
   [genericDLLSupport="yes"]
)

dnl ----------------------------------------------------------------------------
dnl DSAM library
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

dsam_inc=`$DSAM_CONFIG --cflags | sed s/-pthread/-lpthread/ | sed s/-fPIC//`
dsam_ldflags=`$DSAM_CONFIG --libs | sed s/-pthread/-lpthread/`

dnl ----------------------------------------------------------------------------
dnl Octave development environment.
dnl ----------------------------------------------------------------------------

OCTCOMPILE="mkoctfile"

AC_MSG_CHECKING([Octave Development environment ($OCTCOMPILE)])
if eval "$OCTCOMPILE --version 2>> config.log > /dev/null"; then
	octaveAvailable=yes
else
	octaveAvailable=no
fi

AC_MSG_RESULT($octaveAvailable)

dnl ----------------------------------------------------------------------------
dnl Matlab development environment.
dnl ----------------------------------------------------------------------------

MATCOMPILE="mex"

AC_MSG_CHECKING([Matlab Development environment ($MATCOMPILE)])
if eval "$MATCOMPILE -help 2>> config.log > /dev/null"; then
	matlabAvailable=yes
else
	matlabAvailable=no
fi

AC_MSG_RESULT($matlabAvailable)


dnl ============================================================================
dnl Final setups.
dnl ============================================================================

dnl ----------------------------------------------------------------------------
dnl Octave/Matlab Setup
dnl ----------------------------------------------------------------------------

libRunDSAMSim=
use_rundsamsim=

if test x$octaveAvailable = xno -a x$matlabAvailable = xno ; then
	AC_MSG_WARN(Octave/Matlab interface will not be installed)
else
	use_rundsamsim=yes
	if test x$matlabAvailable = xyes; then
		EXEEXT=".mexglx"
		DLLCOMPILE="??"
		MAT_INC=""
		DLLDIRNAME=matlab/dsam
	else
		EXEEXT=""
		DLLCOMPILE="mkoctfile --mex"
		MAT_INC="`mkoctfile -p CPPFLAGS` `mkoctfile -p CXXPICFLAG` `mkoctfile -p ALL_CXXFLAGS`"
		MAT_LDFLAGS="`mkoctfile -p LFLAGS` `mkoctfile -p OCTAVE_LIBS`"
		MAT_LDFLAGS=" $MAT_LDFLAGS `mkoctfile -p LDFLAGS` `mkoctfile -p BLAS_LIBS`"
		MAT_LDFLAGS=" $MAT_LDFLAGS `mkoctfile -p FFTW_LIBS` `mkoctfile -p LIBS`"
		MAT_LDFLAGS=" $MAT_LDFLAGS `mkoctfile -p FLIBS ` `mkoctfile -p DL_LDFLAGS`"
		DLLDIR=octave/dsam
	fi
	AC_SUBST(DLLCOMPILE)
	AC_SUBST(MAT_INC)
	AC_SUBST(MAT_LDFLAGS)
	AC_SUBST(EXEEXT)
	AC_SUBST(DLLDIR)
fi
AM_CONDITIONAL([USE_RUNDSAMSIM], [test "x$use_rundsamsim" = xyes])

DSAM_VERSION="`$DSAM_CONFIG --version`"
AC_SUBST(DSAM_VERSION)
DSAMINC="$dsam_inc"
AC_SUBST(DSAMINC)
DSAMLDFLAGS=$dsam_ldflags
AC_SUBST(DSAMLDFLAGS)
MATLIB_DIR="../MatLib"
AC_SUBST(MATLIB_DIR)

dnl ----------------------------------------------------------------------------
dnl python setup
dnl ----------------------------------------------------------------------------

if test x$pythonSupport = xyes ; then
	use_rundsamsimgen=yes
	enable_shared=yes
	AM_PATH_PYTHON
fi
AM_CONDITIONAL(USE_PYTHON_SUPPORT, test x$pythonSupport = xyes)

dnl ----------------------------------------------------------------------------
dnl Generic DLL library setup
dnl ----------------------------------------------------------------------------

use_rundsamsimGen=

if test x$genericDLLSupport = xyes ; then
	use_rundsamsimgen=yes
	enable_shared=yes
fi

AM_CONDITIONAL([USE_RUNDSAMSIMGEN], [test "x$use_rundsamsimgen" = xyes])

dnl ----------------------------------------------------------------------------
dnl Java setup
dnl ----------------------------------------------------------------------------

if test x$javaSupport = xyes ; then
	libRunDSAMSim=yes
	enable_shared=yes
	dnl case $host_os in
	dnl linux*)
	dnl	JAVA_HOST=linux;;
	dnl esac
	JAVA_HOST=`echo $host_os | $AWK -F- '{ split($0, s); print s[[1]] }'`
	JAVA_INC_DIR=`CfgTools/get_header_path.py jni.h`
	JAVA_INC="-I$JAVA_INC_DIR -I$JAVA_INC_DIR/$JAVA_HOST"
	AC_SUBST(JAVA_INC)
	AM_JAVACFLAGS="-target 1.5 -source 1.5"
	AC_SUBST(AM_JAVACFLAGS)
fi
AM_CONDITIONAL(USE_JAVA_SUPPORT, test x$javaSupport = xyes)

dnl ----------------------------------------------------------------------------
dnl Misc
dnl ----------------------------------------------------------------------------

AM_CONDITIONAL(LIBRUNDSAMSIM, test x$libRunDSAMSim = xyes)

dnl ----------------------------------------------------------------------------
dnl README Docbook Processing processing
dnl ----------------------------------------------------------------------------

if test -f /usr/share/xml/docbook5/schema/rng/5.0/docbook.rng ; then
	RNG_FILE=/usr/share/xml/docbook5/schema/rng/5.0/docbook.rng
	XSL_PREFIX=/usr/share/sgml/docbook/xsl-ns-stylesheets
else
	RNG_FILE=/usr/share/xml/docbook/schema/rng/5.0/docbook.rng
	XSL_PREFIX=/usr/share/xml/docbook/stylesheet/docbook-xsl-ns
fi
AC_SUBST(RNG_FILE)
AC_SUBST(XSL_PREFIX)

dnl ============================================================================
dnl Final configure outputs.
dnl ============================================================================

AC_CONFIG_FILES(
	Makefile
	RunDSAMSim.spec
	CfgTools/Makefile
	debian/Makefile
	MSVC/Makefile
	MSVC/MatLib/Makefile
	MSVC/LibRunDSAMSim/Makefile
	MSVC/LibRunDSAMSimGen/Makefile
	Examples/Makefile
	src/MatLib/Makefile
	src/cpp/Makefile
	src/java/Makefile
	src/LibRunDSAMSim/Makefile
	src/LibRunDSAMSimGen/Makefile
	src/python/Makefile
	src/python/dsam/Makefile
	src/python/dsam/info.py
)
AC_OUTPUT

