## Process this file with automake to produce Makefile.in
## ModuleSpecs directory makefile.

##	Don't forget to find ways to implement Graphics and Parallel

AUTOMAKE_OPTIONS = foreign

An_Modules	= AnAutoCorr.mod AnAverages.mod AnConvolve.mod AnCrossCorr.mod \
				 AnFindBin.mod AnFindNextIndex.mod AnHistogram.mod \
				 AnIntensity.mod AnInterSIH.mod AnSpikeReg.mod AnFourierT.mod \
				 AnSynchIndex.mod AnSAImage.mod AnALSR.mod

Fi_Modules	= FiDataFile.mod

Fl_Modules	= FlBandPass.mod FlMultiBPass.mod FlFIR.mod

Mo_Modules	= MoANSGBinom.mod MoANSGCarney.mod MoANSGSimple.mod \
				MoBMCarney.mod MoBMCooke.mod MoBMGammaT.mod MoBMDRNL.mod \
				MoBM0DRNL.mod FlLowPass.mod MoHCRPCarney.mod MoHCRPMeddis.mod \
				MoHCRPShamma.mod MoIHCCarney.mod MoIHCCooke.mod \
				MoIHC86Meddis.mod MoIHC86aMeddis.mod MoIHCMeddis2000.mod \
				MoNCArleKim.mod MoNCHHuxley.mod MoNCMcGregor.mod \
				MoBMGammaC.mod MoHCRPSham3StVIn.mod MoBMZhang.mod \
				MoHCRPZhang.mod MoIHCZhang.mod MoANSGMeddis02.mod

Pa_Modules	= PaMaster1.mod

St_Modules	= StPTone.mod StFMTone.mod StHarmonic.mod StPulseTrain.mod \
				StEGatedTone.mod StPTone2.mod StAMTone.mod StBPTone.mod \
				StMPTone.mod StMPPTone.mod StStepFun.mod StWhiteNoise.mod \
				StClick.mod

Ut_Modules	= UtAccumulate.mod UtAmpMod.mod UtBinSignal.mod UtTranspose.mod \
				UtCBinauralITD.mod UtCompress.mod UtConvMonaural.mod \
				UtCreateBinaural.mod UtDelay.mod UtIterRipple.mod \
				UtRedceChans.mod UtReduceDt.mod UtRefractory.mod UtSample.mod \
				UtSelectChans.mod UtShapePulse.mod UtSimScript.mod \
				UtStddise.mod UtStrobe.mod UtPause.mod UtHalfWRectify.mod \
				UtLocalChans.mod UtSwapLR.mod UtPadSignal.mod UtMathOp.mod

Tr_Modules	= TrGate.mod TrSetDBSPL.mod TrCollectSignals.mod

EXTRA_DIST	= $(An_Modules) $(Fi_Modules) $(Fl_Modules) $(Mo_Modules) \
				$(Pa_Modules) $(St_Modules) $(Ut_Modules) $(Tr_Modules)
