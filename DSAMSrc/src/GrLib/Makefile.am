## Process this file with automake to produce Makefile.in
## src/GrLib directory makefile.

AUTOMAKE_OPTIONS = foreign

if DSAM_GRAPHICS
SUBDIRS		= Bitmaps

include $(top_srcdir)/src/makeunix.env

lib_LTLIBRARIES = libdsam_g.la
EXTRA_DIST		= makefile.b32

INCLUDES		= -DLIBRARY_COMPILE $(MAIN_INC) $(WXINC) \
					-I$(top_srcdir)/src/MainLib -I../MainLib

libdsam_g_la_SOURCES= GrBrushes.cpp GrFonts.cpp GrAxisScale.cpp \
				GrLines.cpp GrCanvas.cpp GrDisplayS.cpp GrSignalDisp.cpp \
				GrSimMgr.cpp GrUtils.cpp GrPrintDisp.cpp GrModParDialog.cpp \
				GrParListInfo.cpp GrDialogInfo.cpp GrParListInfoList.cpp \
				GrParControl.cpp GrSDIDoc.cpp GrIPCServer.cpp GrMainApp.cpp \
				GrSDIPalette.cpp GrSDIView.cpp GrSDIFrame.cpp GrSDICanvas.cpp \
				GrSDIDiagram.cpp GrSDIEvtHandler.cpp GrSDICommand.cpp \
				GrSDIShapes.cpp GrDiagFrame.cpp GrSDISimDoc.cpp \
				GrSDISPFDoc.cpp GrSDIDocManager.cpp

libdsam_g_la_LDFLAGS= $(GenLaLDFLAGS)

else
endif
