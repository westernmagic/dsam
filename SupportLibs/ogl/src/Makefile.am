# Process this file with automake to produce Makefile.in
## src/ogle directory makefile.

AUTOMAKE_OPTIONS = foreign

LaVersionInfo	= -release $(LT_RELEASE) \
					-version-info $(LIBVERSION_CURRENT):$(LIBVERSION_REVISION):\
					$(LIBVERSION_AGE)

GenLaLDFLAGS	= -rpath $(libdir) $(LaVersionInfo) -export-dynamic

WXWINLDFLAGS	= $(WXLDFLAGS)
WXWINLIBS		= $(WXLDLIBS)
MAIN_INC		= 

lib_LTLIBRARIES = libogl.la

AM_CPPFLAGS	= $(MAIN_INC) $(WXINC) -I$(top_srcdir)/include

libogl_la_SOURCES= basic2.cpp basic.cpp bmpshape.cpp canvas.cpp composit.cpp \
					constrnt.cpp divided.cpp drawn.cpp lines.cpp mfutils.cpp \
					oglmisc.cpp ogldiag.cpp


libogl_la_LDFLAGS= $(GenLaLDFLAGS)
