## Process this file with automake to produce Makefile.in
## src directory makefile.

AUTOMAKE_OPTIONS = foreign

if MPI
MPI_INC = -I$(CHIMPHOME)/include
endif

WXWINLDFLAGS	= $(WXLDFLAGS)
WXWINLIBS		= $(WXLDLIBS)

if WXMSW
WXWINRESFLAGS	= $(RESFLAGS)
endif

LaVersionInfo	= -version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE) \
					-release $(LT_RELEASE)

GenLaLDFLAGS	= -rpath $(libdir) $(LaVersionInfo) -export-dynamic

bin_PROGRAMS	= @PAWORKER1M@
EXTRA_PROGRAMS	= PaWorker1m.exe

lib_LTLIBRARIES = libdsam.la libdsam_std.la @LIBDSAMG@ @LIBDSAMM@@LIBDSAMMG@
EXTRA_LTLIBRARIES= libdsam_m.la libdsam_g.la libdsam_mg.la 
EXTRA_DIST		= makefile.b32 graphsup.dos

YFLAGS			= -dl
INCLUDES		= -I$(top_srcdir)/Include @DSAM_DEFS@ $(MPI_INC) $(WXINC) \
					-DDSAM_DATA_INSTALL_DIR="\"$(pkgdatadir)\""

An_Sources	= AnAutoCorr.c AnAverages.c AnConvolve.c AnCrossCorr.c \
				AnExperiment.c AnFindBin.c AnFindNextIndex.c AnFourierT.c \
				AnGeneral.c AnHistogram.c AnIntensity.c AnInterSIH.c \
				AnSpikeReg.c AnSynchIndex.c AnSAImage.c
  
Fi_Sources	= FiDataFile.c FiAIFF.c FiASCII.c FiMSWave.c FiRaw.c FiParFile.c

Ge_Sources	= GeEarObject.c GeCommon.c GeModuleMgr.c GeSignalData.c \
				GeNSpecLists.c GeUniParMgr.c

Std_Source	= StdMessage.c StdSignalDisp.c

Gr_Sources	= GrMessage.cpp GrBrushes.cpp GrFonts.cpp GrCanvas.cpp GrLine.cpp \
				GrSignalDisp.cpp GrDisplayS.cpp GrSimMgr.cpp GrUtils.cpp \
				GrPrintDisp.cpp GrRunMgr.cpp GrSimModDialog.cpp \
				GrModParDialog.cpp GrParListInfo.cpp GrDialogInfo.cpp \
				GrDialogList.cpp GrParListInfoList.cpp GrParControl.cpp \
				dsam.xpm
  
Mo_Sources	= MoANSGBinom.c MoANSGCarney.c MoANSGSimple.c MoBM0DRNL.c \
				MoBMCarney.c MoBMCooke.c MoBMDRNL.c MoBMGammaT.c FlLowPass.c \
				MoHCRPCarney.c MoHCRPMeddis.c MoHCRPShamma.c MoIHC86Meddis.c \
				MoIHC86aMeddis.c MoIHCMeddis2000.c MoIHCCarney.c MoIHCCooke.c \
				MoNCArleKim.c MoNCHHuxley.c MoNCMcGregor.c FlBandPass.c \
				FlMultiBPass.c

Pa_Sources	= PaComUtils.c PaGeneral.c PaMaster1.c PaUtilities.c

St_Sources	= StAMTone.c StBPTone.c StClick.c StFMTone.c StHarmonic.c \
				StMPPTone.c StMPTone.c StPTone.c StPTone2.c StPulseTrain.c \
				StStepFun.c StWhiteNoise.c StEGatedTone.c

Ut_Sources	= UtSSParser.y UtAccumulate.c UtAmpMod.c UtBandwidth.c \
				UtBinSignal.c UtCBinauralITD.c UtCFList.c UtCmplxM.c \
				UtCompress.c UtConvMonaural.c UtCreateBinaural.c UtDatum.c \
				UtDebug.c UtDelay.c UtFilters.c UtIonChanList.c UtIterRipple.c \
				UtNameSpecs.c UtRamp.c UtRandom.c UtRedceChans.c UtReduceDt.c \
				UtRefractory.c UtSample.c UtSelectChans.c UtShapePulse.c \
				UtSSSymbols.c UtSimScript.c UtSpikeList.c UtStddise.c \
				UtUIEEEFloat.c UtUPortableIO.c UtCreateJoined.c UtStrobe.c \
				UtOptions.c UtString.c UtPause.c UtHalfWRectify.c UtDynaList.c \
				UtAppInterface.c UtFFT.c UtParArray.c

Tr_Sources	= TrGate.c TrSetDBSPL.c

pkginclude_HEADERS	= UtSSParser.h
BUILT_SOURCES		= UtSSParser.c

libdsam_la_SOURCES= $(Ut_Sources) $(Ge_Sources) $(An_Sources) $(Fi_Sources) \
					$(Mo_Sources) $(St_Sources) $(Tr_Sources)
libdsam_la_LDFLAGS=  $(GenLaLDFLAGS)

libdsam_std_la_SOURCES= $(Std_Source)
libdsam_std_la_LDFLAGS= $(GenLaLDFLAGS)
libdsam_m_la_SOURCES= $(Ut_Sources) $(Ge_Sources) $(An_Sources) $(Fi_Sources) \
					$(Mo_Sources) $(St_Sources) $(Tr_Sources) $(Pa_Sources)
libdsam_m_la_LDFLAGS= $(GenLaLDFLAGS)

libdsam_g_la_SOURCES= $(Gr_Sources)
libdsam_g_la_LDFLAGS= $(GenLaLDFLAGS)

libdsam_mg_la_SOURCES= $(Ut_Sources) $(Ge_Sources) $(An_Sources) $(Fi_Sources) \
					$(Mo_Sources) $(St_Sources) $(Tr_Sources) $(Pa_Sources) \
					$(Gr_Sources)
libdsam_mg_la_LDFLAGS= $(GenLaLDFLAGS)

PaWorker1m_exe_SOURCES	= PaWorker1.c
PaWorker1m_exe_LDADD	= libDSAMm.a -L$(CHIMP_LIB) -lmpi -lchimp -lm

