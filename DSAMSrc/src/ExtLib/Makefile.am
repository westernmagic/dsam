## Process this file with automake to produce Makefile.in
## src/ExtLib directory makefile.

AUTOMAKE_OPTIONS = foreign

if DSAM_EXTENSIONS

include $(top_srcdir)/src/makeunix.env

lib_LTLIBRARIES = libdsam_ext.la

INCLUDES		= -DLIBRARY_COMPILE $(MAIN_INC) $(WXINC) \
					-I$(top_srcdir)/src/MainLib -I../MainLib

libdsam_ext_la_SOURCES= ExtMainApp.cpp ExtIPCServer.cpp ExtSimThread.cpp \
						ExtIPCUtils.cpp ExtIPCClient.cpp ExtSocket.cpp \
						ExtSocketServer.cpp ExtProcThread.cpp \
						ExtRunThreadedProc.cpp

libdsam_ext_la_LDFLAGS= $(GenLaLDFLAGS)

else
endif
