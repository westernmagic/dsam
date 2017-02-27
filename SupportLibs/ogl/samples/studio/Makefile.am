## Process this file with automake to produce Makefile.in
## samples/ogl/ogledit directory makefile.

AUTOMAKE_OPTIONS = foreign

SUBDIRS		= bitmaps manual

includeFiles= cspalette.h dialogs.h doc.h mainfrm.h project.h shapes.h \
				studio.h studio_resources.h symbols.h view.h

srcFiles	= cspalette.cpp csprint.cpp dialogs.cpp doc.cpp mainfrm.cpp\
				project.cpp shapes.cpp studio.cpp symbols.cpp view.cpp

makeFiles	= makefile.unx

supportFiles= studio.rc studio_resources.wxr studio.xpm

miscFiles	= #StudioVC.dsp StudioVC.dsw


EXTRA_DIST	= $(includeFiles) $(srcFiles) $(makeFiles) $(supportFiles) \
				$(miscFiles)

