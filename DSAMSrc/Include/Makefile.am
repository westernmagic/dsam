## Process this file with automake to produce Makefile.in
## src directory makefile.

##	Don't forget to find ways to implement Graphics and Parallel

AUTOMAKE_OPTIONS = foreign

An_Headers	= AnAutoCorr.h AnAverages.h AnConvolve.h AnCrossCorr.h \
				AnExperiment.h AnFindBin.h AnFindNextIndex.h AnFourierT.h \
				AnGeneral.h AnHistogram.h AnIntensity.h AnInterSIH.h \
				AnModulusFT.h AnSpikeReg.h AnSynchIndex.h AnSAImage.h
  
Fi_Headers	= FiDataFile.h FiAIFF.h FiASCII.h FiMSWave.h FiRaw.h FiParFile.h

Ge_Headers	= GeEarObject.h GeCommon.h GeModuleMgr.h GeSignalData.h \
				GeNSpecLists.h GeUniParMgr.h StdMessage.h

Gr_Headers	= GrBrushes.h GrItems.h GrCanvas.h GrGraphCtrls.h \
				GrLine.h GrSignalDisp.h GrDisplayS.h GrSimMgr.h GrUtils.h \
				GrIPC.h GrRunMgr.h GrSimModDialog.h GrModParDialog.h \
				GrParListInfo.h GrFonts.h GrDialogInfo.h GrDialogList.h \
				GrDSAMList.h GrParListInfoList.h GrRunParDialog.h
  
Mo_Headers	= MoANSGBinom.h MoANSGCarney.h MoANSGSimple.h MoBM0DRNL.h \
				MoBMCarney.h MoBMCooke.h MoBMDRNL.h MoBMGammaT.h MoDendrite.h \
				MoHCRPCarney.h MoHCRPMeddis.h MoHCRPShamma.h MoIHC86Meddis.h \
				MoIHC86aMeddis.h MoIHC95Meddis.h MoIHCCarney.h MoIHCCooke.h \
				MoNCArleKim.h MoNCHHuxley.h MoNCMcGregor.h MoPEBPass.h \
				MoPEMBPass.h

Pa_Headers	= PaComUtils.h PaGeneral.h PaMaster1.h PaUtilities.h PaWorker1.h

St_Headers	= StAMTone.h StBPTone.h StClick.h StFMTone.h StHarmonic.h \
				StMPPTone.h StMPTone.h StPTone.h StPTone2.h StPulseTrain.h \
				StStepFun.h StWhiteNoise.h StEGatedTone.h

Ut_Headers	= UtAccumulate.h UtAmpMod.h UtBandwidth.h UtBinSignal.h \
				UtCBinauralITD.h UtCFList.h UtCmplxM.h UtCompress.h \
				UtConvMonaural.h UtCreateBinaural.h UtDatum.h UtDebug.h \
				UtDelay.h UtFilters.h UtIonChanList.h UtIterRipple.h \
				UtNameSpecs.h UtRamp.h UtRandom.h UtRedceChans.h UtReduceDt.h \
				UtRefractory.h UtSample.h UtSelectChans.h UtShapePulse.h \
				UtSSSymbols.h UtSimScript.h UtSpikeList.h UtStddise.h \
				UtUIEEEFloat.h UtUPortableIO.h UtCreateJoined.h UtStrobe.h \
				UtOptions.h UtString.h UtPause.h UtHalfWRectify.h UtDynaList.h

Tr_Headers	= TrGate.h TrSetDBSPL.h

pkginclude_HEADERS	= DSAM.h $(St_Headers) $(Mo_Headers) $(Fi_Headers) \
					$(An_Headers) $(Ge_Headers) $(Ut_Headers) $(Tr_Headers) \
					$(Pa_Headers) $(Gr_Headers)

DSAM.h:	$(St_Headers) $(Mo_Headers) $(Fi_Headers) $(An_Headers) $(Ge_Headers) \
					$(Ut_Headers) $(Tr_Headers) $(Pa_Headers) $(Gr_Headers)
		touch DSAM.h