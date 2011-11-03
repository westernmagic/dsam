## Process this file with automake to produce Makefile.in
## This compiles the generic version of RunDSAMSim which can be used with 
## any FFL access systems.

AUTOMAKE_OPTIONS = foreign

lib_LTLIBRARIES	= libRunDSAMSimGen.la

libRunDSAMSimGen_la_CPPFLAGS= -DLIBRARY_COMPILE -DINTERFACE_DLL $(DSAMINC) -I$(MATLIB_DIR)

libRunDSAMSimGen_la_SOURCES	= libRunDSAMSimGen.cpp

libRunDSAMSimGen_la_LDFLAGS= $(MATLIB_DIR)/libdsam_matlib.a $(DSAMLDFLAGS) -rpath $(libdir) -export-dynamic
