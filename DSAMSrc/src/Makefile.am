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
WXWINRESCOMP	= $(RESCOMP)
DSAMRESPATH		= $(bindir)/$(DSAMRES)
endif

LaVersionInfo	= -version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE) \
					-release $(LT_RELEASE) \
					-export-dynamic

GenLaLDFLAGS	= -rpath $(libdir) $(LaVersionIno) -export-dynamic

bin_SCRIPTS		= ccdsam @CCDSAMM@ @CCDSAMG@ @CCDSAMMG@ @DSAMRES@
EXTRA_SCRIPTS	= ccdsamm ccdsamg ccdsammg DSAMRes.o
bin_PROGRAMS	= @PAWORKER1M@
EXTRA_PROGRAMS	= PaWorker1m.exe

lib_LTLIBRARIES = libDSAM.la libDSAMStd.la @LIBDSAMG@ @LIBDSAMM@@LIBDSAMMG@
EXTRA_LTLIBRARIES= libDSAMm.la libDSAMg.la libDSAMmg.la 
EXTRA_DIST		= makefile.b32 graphsup.dos dsam.rc

MYCCCOMPILE		= $(CC) $(CPPFLAGS) $(DEFS) '$$*' -I$(pkgincludedir)
MYCXXCOMPILE	= $(CXX) $(CPPFLAGS) $(DEFS) $(WXINC) '$$*' -I$(pkgincludedir)
YFLAGS			= -dl
INCLUDES		= -I$(top_srcdir)/Include $(MPI_INC) $(WXINC)

An_Sources	= AnAutoCorr.c AnAverages.c AnConvolve.c AnCrossCorr.c \
				AnExperiment.c AnFindBin.c AnFindNextIndex.c AnFourierT.c \
				AnGeneral.c AnHistogram.c AnIntensity.c AnInterSIH.c \
				AnModulusFT.c AnSpikeReg.c AnSynchIndex.c AnSAImage.c
  
Fi_Sources	= FiDataFile.c FiAIFF.c FiASCII.c FiMSWave.c FiRaw.c FiParFile.c

Ge_Sources	= GeEarObject.c GeCommon.c GeModuleMgr.c GeSignalData.c \
				GeNSpecLists.c GeUniParMgr.c

Std_Source	= StdMessage.c StdSignalDisp.c

Gr_Sources	= GrMessage.cpp GrBrushes.cpp GrFonts.cpp GrCanvas.cpp \
				GrLine.cpp GrSignalDisp.cpp GrDisplayS.cpp \
				GrSimMgr.cpp GrUtils.cpp GrIPC.cpp GrPrintDisp.cpp \
				GrRunMgr.cpp GrSimModDialog.cpp GrModParDialog.cpp \
				GrParListInfo.cpp GrDialogInfo.cpp GrDialogList.cpp \
				GrParListInfoList.cpp GrParControl.cpp dsam.xpm
  
Mo_Sources	= MoANSGBinom.c MoANSGCarney.c MoANSGSimple.c MoBM0DRNL.c \
				MoBMCarney.c MoBMCooke.c MoBMDRNL.c MoBMGammaT.c FlLowPass.c \
				MoHCRPCarney.c MoHCRPMeddis.c MoHCRPShamma.c MoIHC86Meddis.c \
				MoIHC86aMeddis.c MoIHC95Meddis.c MoIHCCarney.c MoIHCCooke.c \
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
				UtOptions.c UtString.c UtPause.c UtHalfWRectify.c UtDynaList.c

Tr_Sources	= TrGate.c TrSetDBSPL.c

pkginclude_HEADERS	= UtSSParser.h
BUILT_SOURCES		= UtSSParser.c

libDSAM_la_SOURCES= $(Ut_Sources) $(Ge_Sources) $(An_Sources) $(Fi_Sources) \
					$(Mo_Sources) $(St_Sources) $(Tr_Sources)
libDSAM_la_LDFLAGS=  $(LaVersionIno)

libDSAMStd_la_SOURCES= $(Std_Source)
libDSAMStd_la_LDFLAGS= $(GenLaLDFLAGS)
libDSAMm_la_SOURCES= $(Ut_Sources) $(Ge_Sources) $(An_Sources) $(Fi_Sources) \
					$(Mo_Sources) $(St_Sources) $(Tr_Sources) $(Pa_Sources)
libDSAMm_la_LDFLAGS= $(GenLaLDFLAGS)

libDSAMg_la_SOURCES= $(Gr_Sources)
libDSAMg_la_LDFLAGS= $(GenLaLDFLAGS)

libDSAMmg_la_SOURCES= $(Ut_Sources) $(Ge_Sources) $(An_Sources) $(Fi_Sources) \
					$(Mo_Sources) $(St_Sources) $(Tr_Sources) $(Pa_Sources) \
					$(Gr_Sources)
libDSAMmg_la_LDFLAGS= $(GenLaLDFLAGS)

PaWorker1m_exe_SOURCES	= PaWorker1.c
PaWorker1m_exe_LDADD	= libDSAMm.a -L$(CHIMP_LIB) -lmpi -lchimp -lm

DSAMRes.o:	Makefile
		$(WXWINRESCOMP) $(WXWINRESFLAGS) $(srcdir)/dsam.rc -o $@

ccdsam:	../DSAMSetup.h Makefile
		echo $(MYCCCOMPILE) -DCCDSAM -L$(libdir) -lDSAM -lDSAMStd -lm > $@
		chmod +x $@

ccdsamm:	../DSAMSetup.h Makefile
		echo $(MYCCCOMPILE) -DCCDSAMM -L$(libdir) -lDSAMm \
		  -L$(CHIMP_LIB) -lmpi -lchimp -lm > $@
		chmod +x $@

ccdsamg:	../DSAMSetup.h Makefile
		echo $(MYCXXCOMPILE) -DCCDSAMG $(X_CFLAGS) $(WXWINLDFLAGS) \
		  -L$(libdir) -lDSAM -lDSAMg  $(WXWINLIBS) $(DSAMRESPATH) \
		  $(X_PRE_LIBS) $(X_LIBS) $(X_EXTRA_LIBS) $(WXLDFLAGS) > $@
		chmod +x $@

ccdsammg:	../DSAMSetup.h Makefile
		echo $(MYCOMPILE) -DCCDSAMMG $(X_CFLAGS) $(WXWINLDFLAGS) \
		  -L$(libdir) -lDSAMmg -L$(CHIMP_LIB) -lmpi -lchimp $(WXWINLIBS) \
		  $(X_PRE_LIBS) $(X_LIBS) $(X_EXTRA_LIBS) -lX11 -lXmu -lXt -lXpm \
		  -lXaw -lm > $@
		chmod +x $@

