## Process this file with automake to produce Makefile.in
## samples/ogl/ogledit directory makefile.

AUTOMAKE_OPTIONS = foreign

SUBDIRS		= bitmaps

includeFiles	= doc.cpp ogledit.cpp palette.cpp view.cpp

srcFiles	= doc.h ogledit.h palette.h view.h

makeFiles	= makefile.unx

supportFiles	= ogledit.rc ogl.ico ogl.xpm

miscFiles	= ogledit.dsp


EXTRA_DIST	= $(includeFiles) $(srcFiles) $(makeFiles) $(supportFiles) \
				$(miscFiles)

