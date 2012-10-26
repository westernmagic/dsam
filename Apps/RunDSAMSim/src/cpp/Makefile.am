## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS = foreign

libexec_PROGRAMS	= RunDSAMSim

AM_CPPFLAGS			= -DLIBRARY_COMPILE -DINTERFACE_DLL $(DSAMINC) $(MAT_INC) \
						-I$(MATLIB_DIR)

RunDSAMSim_SOURCES	= RunDSAMSim.cpp

RunDSAMSim_DEPENDENCIES = $(MATLIB_DIR)/libdsam_matlib.a

CXXLINK = $(DLLCOMPILE) -o RunDSAMSim$(EXEEXT)

LDADD	= $(DSAMLDFLAGS) -L$(MATLIB_DIR)/.libs -ldsam_matlib

EXTRA_DIST			= RunDSAMSim.rc RunDSAMSim.def RunDSAMSim.m
