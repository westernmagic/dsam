## Process this file with automake to produce Makefile.in
## src/StdLib directory makefile.

AUTOMAKE_OPTIONS = foreign

include $(top_srcdir)/src/makeunix.env

lib_LTLIBRARIES = libdsam_std.la
EXTRA_DIST		= makefile.b32

INCLUDES		= $(MAIN_INC)

libdsam_std_la_SOURCES= StdMessage.c StdSignalDisp.c StdSimThread.c

libdsam_std_la_LDFLAGS= $(GenLaLDFLAGS)
