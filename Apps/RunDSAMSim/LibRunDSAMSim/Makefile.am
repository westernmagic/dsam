## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS = foreign


if LIBRUNDSAMSIM

lib_LTLIBRARIES = libRunDSAMSim.la

INCLUDES		= -DLIBRARY_COMPILE $(DSAMINC) $(JAVA_INC)

libRunDSAMSim_la_SOURCES= libRunDSAMSim.h libRunDSAMSim.cpp ../MatMainApp.cpp ../MatInSignal.cpp

#libRunDSAMSim_la_LDFLAGS= $(DSAMLDFLAGS) -rpath $(libdir) $(LaVersionInfo) -export-dynamic
libRunDSAMSim_la_LDFLAGS= $(DSAMLDFLAGS)  -rpath $(libdir) -export-dynamic

CLASSPATH	= ../../RunDSAMSimJ/bin
libRunDSAMSim.h: $(CLASSPATH)/RunDSAMSimJ.class
				javah -jni -classpath $(CLASSPATH) -o libRunDSAMSim.h RunDSAMSimJ

else
endif
