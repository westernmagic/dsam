## Process this file with automake to produce Makefile.in
## src/GrLib directory makefile.

AUTOMAKE_OPTIONS = foreign

include $(top_srcdir)/src/makeunix.env

WXWINLDFLAGS	= $(WXLDFLAGS)
WXWINLIBS		= $(WXLDLIBS)

if WXMSW
WXWINRESFLAGS	= $(RESFLAGS)
endif

lib_LTLIBRARIES = libdsam_g.la
EXTRA_DIST		= makefile.b32

INCLUDES		= -DLIBRARY_COMPILE $(MAIN_INC) $(WXINC) \
					-I$(top_srcdir)/src/MainLib -I../MainLib

libdsam_g_la_SOURCES= GrMessage.cpp GrBrushes.cpp GrFonts.cpp GrAxisScale.cpp \
				GrLines.cpp GrCanvas.cpp GrDisplayS.cpp GrSignalDisp.cpp \
				GrSimMgr.cpp GrUtils.cpp GrPrintDisp.cpp GrSimModDialog.cpp \
				GrModParDialog.cpp GrParListInfo.cpp GrDialogInfo.cpp \
				GrDialogList.cpp GrParListInfoList.cpp GrParControl.cpp \
				GrSocket.cpp GrSimThread.cpp

libdsam_g_la_LDFLAGS= $(GenLaLDFLAGS)

GrSignalDisp.cpp: $(top_srcdir)/src/StdLib/StdSignalDisp.c
	cp $(top_srcdir)/src/StdLib/StdSignalDisp.c GrSignalDisp.cpp
