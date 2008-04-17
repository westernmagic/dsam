dnl Process this file with autoconf to produce a configure script.
dnl CVS ID $Id$
dnl Put in a check for 'bool' for CC and CXX.

AC_REVISION($Revision: 2.61)

AC_INIT(DSAM, 2.7.43)
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
DSAM_MINOR_VERSION=7
DSAM_MICRO_VERSION=43
DSAM_INTERFACE_AGE=43
DSAM_BINARY_AGE=43
DSAM_RPM_AGE=1

dnl
dnl AC_DIVERT_POP()dnl

AC_SUBST(DSAM_MAJOR_VERSION)
AC_SUBST(DSAM_MINOR_VERSION)
AC_SUBST(DSAM_MICRO_VERSION)
AC_SUBST(DSAM_INTERFACE_AGE)
AC_SUBST(DSAM_BINARY_AGE)
AC_SUBST(DSAM_RPM_AGE)
AC_DEFINE_UNQUOTED(DSAM_VERSION,wxT("$PACKAGE_VERSION"), "DSAM version")

# libtool versioning
LT_RELEASE=$DSAM_MAJOR_VERSION.$DSAM_MINOR_VERSION
LT_CURRENT=`expr $DSAM_MICRO_VERSION - $DSAM_INTERFACE_AGE`
LT_REVISION=$DSAM_INTERFACE_AGE
LT_AGE=`expr $DSAM_BINARY_AGE - $DSAM_INTERFACE_AGE`
AC_SUBST(LT_RELEASE)
AC_SUBST(LT_CURRENT)
AC_SUBST(LT_REVISION)
AC_SUBST(LT_AGE)

# RPM versioning
RPM_AGE=$DSAM_RPM_AGE
AC_SUBST(RPM_AGE)

DSAM_PACKAGE=dsam
AC_DEFINE_UNQUOTED(DSAM_PACKAGE,wxT("$DSAM_PACKAGE"), "DSAM Package name")
AC_SUBST(DSAM_PACKAGE)

AM_INIT_AUTOMAKE
AM_CONFIG_HEADER(DSAMSetup.h:DSAMSetup.hin )

dnl ============================================================================
dnl 'Global' variable declaration
dnl ============================================================================

WX_CONFIG=wx-config

dnl ============================================================================
dnl Configure ENABLE options
dnl ============================================================================

AC_ARG_ENABLE(dsam_extensions,
   [  --disable-dsam-extensions  Do not compile DSAM extensions support],
   [DSAM_EXT="$enableval"],
   [DSAM_EXT="yes"]
)

AC_ARG_ENABLE(gui,
   [  --disable-gui            Do not compile any wxWindows support],
   [gui="$enableval"],
   [gui="yes"]
)

AC_ARG_ENABLE(portaudio,
   [  --disable-portaudio        Do not compile PortAudio support],
   [enable_portaudio="$enableval"],
   [enable_portaudio="yes"]
)

AC_ARG_ENABLE(unicode,
   [  --disable-unicode        Do not compile Unicode support],
   [enable_unicode="$enableval"],
   [enable_unicode="yes"]
)

AC_ARG_ENABLE(wx_ogl,
   [  --disable-wx_ogl           Do not compile wx OGL support],
   [WX_OGL="$enableval"],
   [WX_OGL="yes"]
)

dnl ============================================================================
dnl Configure WITH options
dnl ============================================================================

AC_ARG_WITH(portaudio-headers,
   [  --with-portaudio-headers=DIR  PortAudio Include files location],
   [PORTAUDIO_HDR_DIR="$withval"]
   [PORTAUDIO_CPPFLAGS="-I$withval"] 
)

AC_ARG_WITH(portaudio-lib,
   [  --with-portaudio-lib=DIR      PortAudio Library location],
   [PORTAUDIO_LDFLAGS="-L$withval"] 
)

AC_ARG_WITH(wx_ogl-headers,
   [  --with-wx_ogl-headers=DIR  wx OGL Include files location],
   [WX_OGL_HDR_DIR="$withval"]
   [WX_OGL_CPPFLAGS="-I$withval"] 
)

AC_ARG_WITH(wx_ogl-lib,
   [  --with-wx_ogl-lib=DIR      wx OGL Library location],
   [WX_OGL_LDFLAGS="-L$withval"] 
)

dnl ============================================================================
dnl cross-compiling support: we're cross compiling if the build system is
dnl different from the target one (assume host and target be always the same)
dnl -
dnl -this needs to be updated LPO 21-10-2003.
dnl -
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

AC_DEFUN([DSAM_CPP_BOOL],
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

AC_DEFUN([DSAM_C_BIGENDIAN],
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
			debugFlags="-ggdb3 -Wall"
			debug=true
			CFLAGS="${debugFlags} -Wstrict-prototypes"
			CXXFLAGS=${debugFlags}
			;;
		no)		debug=false ;;
		*)	AC_MSG_ERROR(bad value ${enableval} for --enable-debug) ;;
	esac], [debug=false])

dnl ============================================================================
dnl Checks for types
dnl ============================================================================

AC_CHECK_SIZEOF(long)
AC_CHECK_TYPES([int16, int32])

dnl ============================================================================
dnl Checks for programs.
dnl ============================================================================

#AC_PROG_CC
AC_PROG_CPP
AC_PROG_MAKE_SET
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
dnl wxWin library (General)
dnl ----------------------------------------------------------------------------

wxwinAvailable=no
if test x$gui = xyes -o x$DSAM_EXT = xyes; then
	AC_MSG_CHECKING(for WXWIN configuration shell script ($WX_CONFIG))
	if eval "$WX_CONFIG --version 2> config.log > /dev/null"; then
		wxwinAvailable=yes
		wx_version=`$WX_CONFIG --version | cut -d. -f 1-2`
		wx_basename=`$WX_CONFIG --basename`
	fi
	AC_MSG_RESULT($wxwinAvailable)
fi

dnl ----------------------------------------------------------------------------
dnl wxBase library
dnl ----------------------------------------------------------------------------

if test x$DSAM_EXT = xyes -a x$wxwinAvailable = xyes; then
	CXXFLAGS_save=$CXXFLAGS
	CXXFLAGS=`$WX_CONFIG --cxxflags base`
	LDFLAGS_save=$LDFLAGS
	LDFLAGS=`$WX_CONFIG --libs base`
	AC_MSG_CHECKING([for wxWidgets base support for threaded extensions])
	AC_LINK_IFELSE(
		[
			AC_LANG_PROGRAM(
				[
					#include <wx/wx.h>
					#include <wx/thread.h>

					class MyThread: public wxThread {
					public:
					  MyThread(void): wxThread()	{ Create(); }
					  virtual  ExitCode Entry()	{ return (ExitCode) 1; }
					};
				],
				[
					MyThread	*thread = new MyThread();
				]
			)
		],
		[
			wx_base_available=yes
		],
		[
			wx_base_available=no
		]
	)
	AC_MSG_RESULT($wx_base_available);
	CXXFLAGS=$CXXFLAGS_save
	LDFLAGS=$LDFLAGS_save
fi

dnl ----------------------------------------------------------------------------
dnl wxWidgets GUI library
dnl ----------------------------------------------------------------------------

if test x$gui = xyes -a x$wxwinAvailable = xyes; then
	AC_MSG_CHECKING([for wxWidgets GUI support])
	CXXFLAGS_save=$CXXFLAGS
	CXXFLAGS=`$WX_CONFIG --cxxflags`
	LDFLAGS_save=$LDFLAGS
	LDFLAGS=`$WX_CONFIG --libs`
	AC_LINK_IFELSE(
		[
			AC_LANG_SOURCE(
				[
					#include "wx/wx.h"

					class MyApp : public wxApp
					{
						public:
						virtual bool OnInit();
					};

					IMPLEMENT_APP(MyApp)
					bool MyApp::OnInit()
					{
						return true;
					}
				]
			)
		],
		[
			using_wxWidgets_gui=yes
		],
		[
			using_wxWidgets_gui=no
		]
	)
	AC_MSG_RESULT($using_wxWidgets_gui);
	CXXFLAGS=$CXXFLAGS_save
	LDFLAGS=$LDFLAGS_save
fi

dnl ----------------------------------------------------------------------------
dnl wx OGL library
dnl ----------------------------------------------------------------------------

if test x$WX_OGL = xyes -a x$using_wxWidgets_gui = xyes; then
	AC_MSG_CHECKING([for wxWidgets OGL support])
	CXXFLAGS_save=$CXXFLAGS
	CXXFLAGS=`$WX_CONFIG --cxxflags`
	LDFLAGS_save=$LDFLAGS
	LDFLAGS=`$WX_CONFIG --libs ogl`
	AC_LINK_IFELSE(
		[
			AC_LANG_SOURCE(
				[
					#include "wx/wx.h"
					#include <wx/ogl/ogl.h>

					class MyApp : public wxApp
					{
						public:
						virtual bool OnInit();
					};

					IMPLEMENT_APP(MyApp)
					bool MyApp::OnInit()
					{
						wxShapeEvtHandler	*handler = new wxShapeEvtHandler();
						return true;
					}
				]
			)
		],
		[
			using_wxWidgets_ogl=yes
		],
		[
			using_wxWidgets_ogl=no
		]
	)
	AC_MSG_RESULT($using_wxWidgets_ogl);
	CXXFLAGS=$CXXFLAGS_save
	LDFLAGS=$LDFLAGS_save
fi

dnl ----------------------------------------------------------------------------
dnl PortAudio library
dnl ----------------------------------------------------------------------------

PORTAUDIO_INCLUDE="-pipe -fPIC -DPA_LITTLE_ENDIAN -Ipa_common  -DSIZEOF_SHORT=2 \
	  -DSIZEOF_INT=4 -DSIZEOF_LONG=8 -DHAVE_LIBPTHREAD=1 -DPA_USE_ALSA=1 -DPA_USE_OSS=1"

if test x$enable_portaudio = xyes; then
	AC_MSG_CHECKING([for PortAudio support for audio i/o])
	CFLAGS_save=$CFLAGS
	CFLAGS=$PORTAUDIO_INCLUDE
	LDFLAGS_save=$LDFLAGS
	LDFLAGS="$PORTAUDIO_LDFLAGS -lportaudio -lpthread"
	AC_LINK_IFELSE(
		[
			AC_LANG_PROGRAM(
				[
					#include <math.h>
					#include <portaudio.h>
				],
				[
					PaError err = Pa_Initialize();
				]
			)
		],
		[
			using_portaudio=yes
		],
		[
			using_portaudio=no
		]
	)
	AC_MSG_RESULT($using_portaudio);
	CFLAGS=$CFLAGS_save
	LDFLAGS=$LDFLAGS_save
fi

dnl ----------------------------------------------------------------------------
dnl libsndfile library
dnl ----------------------------------------------------------------------------

PKG_CHECK_MODULES(SNDFILE, sndfile >= 1.0.2, ac_cv_sndfile=1, ac_cv_sndfile=0)
AC_DEFINE_UNQUOTED([HAVE_SNDFILE],${ac_cv_sndfile},
	[Set to 1 if you have libsndfile.])

AC_SUBST(SNDFILE_CFLAGS)
AC_SUBST(SNDFILE_LIBS)

dnl ----------------------------------------------------------------------------
dnl fftw3 library
dnl ----------------------------------------------------------------------------

PKG_CHECK_MODULES(FFTW3, fftw3 >= 3.0.0, ac_cv_fftw3=1, ac_cv_fftw3=0)
AC_DEFINE_UNQUOTED([HAVE_FFTW3],${ac_cv_fftw3},
	[Set to 1 if you have fftw3.])

AC_SUBST(FFTW3_CFLAGS)
AC_SUBST(FFTW3_LIBS)

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

dnl ----------------------------------------------------------------------------
dnl Unicode option
dnl ----------------------------------------------------------------------------

if test x$enable_unicode = xyes; then
	AC_DEFINE(DSAM_USE_UNICODE, 1, [Use unicode options])
	DSAM_UNICODE="-DDSAM_USE_UNICODE"
fi
AM_CONDITIONAL(DSAM_USE_UNICODE, test x$enable_unicode = xyes)

DSAM_DEFS="$DSAM_GRAPHICS $DSAM_MPI -fPIC $SNDFILE_CFLAGS"
AC_SUBST(DSAM_GRAPHICS)
AC_SUBST(DSAM_MPI)
AC_SUBST(DSAM_UNICODE)
AC_SUBST(DSAM_DEFS)

dnl ----------------------------------------------------------------------------
dnl General option settings.
dnl ----------------------------------------------------------------------------

if test x$wxwinAvailable = xyes; then
	AC_PROG_CXX
	AC_PROG_CXXCPP
fi

if test x$wx_base_available = xyes; then
	AC_DEFINE(USE_EXTENSIONS_LIBRARY, 1, 'Use Extensions Support")
	DSAMEXTINC="-DEXTENSIONS_SUPPORT"
	AC_SUBST(DSAMEXTINC)
	DSAMEXTLIB="-l${DSAM_PACKAGE}_ext"
	AC_SUBST(DSAMEXTLIB)
	WX_BASE_CXXFLAGS="`$WX_CONFIG --cxxflags base`"
	AC_SUBST(WX_BASE_CXXFLAGS)
	WX_BASE_LDFLAGS="`$WX_CONFIG --libs base net xml`"
	AC_SUBST(WX_BASE_LDFLAGS)
fi
AM_CONDITIONAL(DSAM_EXTENSIONS, test x$wx_base_available = xyes)

if test x$using_wxWidgets_ogl = xyes; then
	AC_DEFINE(USE_WX_OGL, 1, [Use wxWidgets OGL Support])
	WXOGLLIBS="`$WX_CONFIG --libs ogl`"
	AC_SUBST(WXOGLLIBS)
fi

if test x$using_wxWidgets_gui = xyes; then
	DSAM_GRAPHICS="-DGRAPHICS_SUPPORT"
	AC_DEFINE(GRAPHICS_SUPPORT, 1, 'Using Graphics Support")
	WXRESFLAGS="--include-dir `$WX_CONFIG --exec-prefix`/include \
	  --include-dir `$WX_CONFIG --exec-prefix`/$host_alias/include"
	AC_SUBST(WXRESFLAGS)
	XINCLUDE=$wx_xinclude
	AC_SUBST(XINCLUDE)
fi
AM_CONDITIONAL(DSAM_GRAPHICS, test x$using_wxWidgets_gui = xyes)

if  test x$using_wxWidgets_gui = xyes -o x$using_wxWidgets_ogl = xyes; then
	WXINC="`$WX_CONFIG --cxxflags`"
	AC_SUBST(WXINC)
	WXLDLIBS="`$WX_CONFIG --libs`"
	AC_SUBST(WXLDLIBS)
fi

if test x$using_portaudio = xyes; then
	AC_DEFINE(USE_PORTAUDIO, 1, [Use PortAudio library])
	PORTAUDIO_LDFLAGS="-lportaudio"
	AC_SUBST(PORTAUDIO_LDFLAGS)
fi
AM_CONDITIONAL(USE_PORTAUDI, test x$using_portaudio = xyes)

dnl ============================================================================
dnl Checks for system services or capabilities.
dnl ============================================================================

AC_EXEEXT
AC_OBJEXT
AC_SYS_LONG_FILE_NAMES

AM_CONDITIONAL(MPI, test x$mpi = xyes)
AC_SUBST(LIBDSAM)
AC_SUBST(LIBDSAMM)
AC_SUBST(PAWORKER1M)
AC_SUBST(GR_LIB)
AC_SUBST(MINGW32)

dnl ============================================================================
dnl Final configure outputs.
dnl ============================================================================

AC_CONFIG_FILES([
	dsam.spec
	CfgTools/Makefile
	Makefile
	Include/Makefile
	src/Makefile
	src/MainLib/Makefile
	src/StdLib/Makefile
	src/ExtLib/Makefile
	src/GrLib/Makefile
	src/GrLib/Bitmaps/Makefile
	ModuleSpecs/Makefile
	Examples/Makefile
	dsam-config
	MSVC/Makefile
	MSVC/DSAM/Makefile
	MSVC/DSAM_ext/Makefile
	MSVC/DSAM_std/Makefile
	MSVC/DSAM_g/Makefile
], [chmod +x dsam-config])

AC_OUTPUT
