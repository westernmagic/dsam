## Process this file with automake to produce Makefile.in
## src directory makefile.

AUTOMAKE_OPTIONS = foreign

include $(top_srcdir)/src/makeunix.env

WXWINLDFLAGS	= $(WXLDFLAGS)
WXWINLIBS		= $(WXLDLIBS)

if WXMSW
WXWINRESFLAGS	= $(RESFLAGS)
endif

lib_LTLIBRARIES = libdsam_g.la
EXTRA_DIST		= makefile.b32

INCLUDES		= $(MAIN_INC) $(WXINC) -I$(top_srcdir)/src/MainLib

libdsam_g_la_SOURCES= GrMessage.cpp GrBrushes.cpp GrFonts.cpp GrCanvas.cpp \
				GrLine.cpp GrSignalDisp.cpp GrDisplayS.cpp GrSimMgr.cpp \
				GrUtils.cpp GrPrintDisp.cpp GrSimModDialog.cpp \
				GrModParDialog.cpp GrParListInfo.cpp GrDialogInfo.cpp \
				GrDialogList.cpp GrParListInfoList.cpp GrParControl.cpp \
				GrSocket.cpp GrSimThread.cpp dsam.xpm

libdsam_g_la_LDFLAGS= $(GenLaLDFLAGS)

