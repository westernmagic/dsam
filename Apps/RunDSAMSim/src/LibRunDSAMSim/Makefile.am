## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS = foreign


if LIBRUNDSAMSIM

lib_LTLIBRARIES = libRunDSAMSim.la

libRunDSAMSim_la_CPPFLAGS	= -DLIBRARY_COMPILE $(DSAMINC) -I$(MATLIB_DIR) -I../java $(JAVA_INC)

EXTRA_DIST		= libRunDSAMSim.def

libRunDSAMSim_la_SOURCES= libRunDSAMSim.cpp

#libRunDSAMSim_la_LDFLAGS= $(DSAMLDFLAGS) -rpath $(libdir) $(LaVersionInfo) -export-dynamic
libRunDSAMSim_la_LDFLAGS= $(MATLIB_DIR)/libdsam_matlib.a $(DSAMLDFLAGS) -rpath $(libdir) -export-dynamic

CLASSPATH	= ../java

else
endif
