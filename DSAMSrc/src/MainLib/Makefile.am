## Process this file with automake to produce Makefile.in
## src/MainLib directory makefile.

AUTOMAKE_OPTIONS = foreign

include $(top_srcdir)/src/makeunix.env

if MPI
MPI_INC = -I$(CHIMPHOME)/include
endif

bin_PROGRAMS	= @PAWORKER1M@
EXTRA_PROGRAMS	= PaWorker1m.exe

lib_LTLIBRARIES = libdsam.la
EXTRA_DIST		= makefile.b32

YFLAGS			= -dl
INCLUDES		= -DLIBRARY_COMPILE $(MAIN_INC)

An_Sources	= AnAutoCorr.c AnAverages.c AnConvolve.c AnCrossCorr.c \
				AnExperiment.c AnFindBin.c AnFindNextIndex.c AnFourierT.c \
				AnGeneral.c AnHistogram.c AnIntensity.c AnInterSIH.c \
				AnSpikeReg.c AnSynchIndex.c AnSAImage.c AnALSR.c
  
Fi_Sources	= FiDataFile.c FiAIFF.c FiASCII.c FiMSWave.c FiRaw.c FiParFile.c

Fl_Sources	= FlBandPass.c FlMultiBPass.c FlFIR.c


Ge_Sources	= GeEarObject.c GeCommon.c GeModuleMgr.c GeSignalData.c \
				GeNSpecLists.c GeUniParMgr.c GeModuleReg.c

Mo_Sources	= MoANSGBinom.c MoANSGCarney.c MoANSGSimple.c MoBM0DRNL.c \
				MoBMCarney.c MoBMCooke.c MoBMDRNL.c MoBMGammaT.c FlLowPass.c \
				MoHCRPCarney.c MoHCRPMeddis.c MoHCRPShamma.c MoIHC86Meddis.c \
				MoIHC86aMeddis.c MoIHCMeddis2000.c MoIHCCarney.c MoIHCCooke.c \
				MoNCArleKim.c MoNCHHuxley.c MoNCMcGregor.c MoBMGammaC.c \
				MoHCRPSham3StVIn.c MoBMZhang.c MoHCRPZhang.c MoIHCZhang.c \
				MoANSGMeddis02.c

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
				UtAppInterface.c UtFFT.c UtParArray.c UtGCFilters.c \
				UtFIRFilters.c UtRemez.c UtDynaBList.c UtLocalChans.c \
				UtSwapLR.c UtPadSignal.c UtMathOp.c UtZhang.c

Tr_Sources	= TrGate.c TrSetDBSPL.c TrCollectChans.c

pkginclude_HEADERS	= UtSSParser.h
BUILT_SOURCES		= UtSSParser.c

libdsam_la_SOURCES= $(Ut_Sources) $(Ge_Sources) $(An_Sources) $(Fi_Sources) \
					$(Mo_Sources) $(Fl_Sources) $(St_Sources) $(Tr_Sources)
libdsam_la_LDFLAGS=  $(GenLaLDFLAGS)

PaWorker1m_exe_SOURCES	= PaWorker1.c
PaWorker1m_exe_LDADD	= libDSAMm.a -L$(CHIMP_LIB) -lmpi -lchimp -lm

