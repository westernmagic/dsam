## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS = foreign

lib_LTLIBRARIES	= libdsam_matlib.la

libdsam_matlib_la_CPPFLAGS	= $(DSAMINC) -DLIBRARY_COMPILE # -fPIC

libdsam_matlib_la_SOURCES	= MatLib.h MatMainApp.h MatMainApp.cpp MatInSignal.h MatInSignal.cpp

libdsam_matlib_la_LDFLAGS	= $(DSAMLDFLAGS) -rpath $(libdir) -export-dynamic
