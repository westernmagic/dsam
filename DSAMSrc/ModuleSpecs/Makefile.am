## Process this file with automake to produce Makefile.in
## src directory makefile.

##	Don't forget to find ways to implement Graphics and Parallel

AUTOMAKE_OPTIONS = foreign

An_Modules	= AnAutoCorr.mod AnAverages.mod AnConvolve.mod AnCrossCorr.mod \
				 AnFindBin.mod AnFindNextIndex.mod AnHistogram.mod \
				 AnIntensity.mod AnInterSIH.mod AnModulusFT.mod \
				 AnSpikeReg.mod AnSynchIndex.mod AnSAImage.mod

Mo_Modules	= MoANSGBinom.mod MoANSGCarney.mod MoANSGSimple.mod \
				MoBMCarney.mod MoBMCooke.mod MoBMGammaT.mod MoBMDRNL.mod \
				MoBM0DRNL.mod MoDendrite.mod MoHCRPCarney.mod MoHCRPMeddis.mod \
				MoHCRPShamma.mod MoIHCCarney.mod MoIHCCooke.mod \
				MoIHC86Meddis.mod MoIHC86aMeddis.mod MoIHC95Meddis.mod \
				MoNCArleKim.mod MoNCHHuxley.mod MoNCMcGregor.mod MoPEBPass.mod \
				MoPEMBPass.mod

Pa_Modules	= PaMaster1.mod

St_Modules	= StPTone.mod StFMTone.mod StHarmonic.mod StPulseTrain.mod \
				StEGatedTone.mod StPTone2.mod StAMTone.mod StBPTone.mod \
				StMPTone.mod StMPPTone.mod StStepFun.mod StWhiteNoise.mod

Ut_Modules	= UtAccumulate.mod UtAmpMod.mod UtBinSignal.mod \
				UtCBinauralITD.mod UtCompress.mod UtConvMonaural.mod \
				UtCreateBinaural.mod UtDelay.mod UtIterRipple.mod \
				UtRedceChans.mod UtReduceDt.mod UtRefractory.mod UtSample.mod \
				UtSelectChans.mod UtShapePulse.mod UtSimScript.mod \
				UtStddise.mod UtStrobe.mod UtPause.mod UtHalfWRectify.mod

Tr_Modules	= TrGate.mod

EXTRA_DIST	= $(An_Modules) $(Mo_Modules) $(Pa_Modules) $(St_Modules) \
				$(Ut_Modules) $(Tr_Modules)