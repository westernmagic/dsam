## Tutorials/BinauralPitch directory makefile.

##	Don't forget to find ways to implement Graphics and Parallel

AUTOMAKE_OPTIONS = foreign

DIRECTORY	= Tutorials/BinauralPitch

SimScripts	= pitchEarlyCrossover.sim pitchLateCrossover.sim

SPFFiles	= pitchEarlyCrossover.spf pitchLateCrossover.spf

ParFiles	= acf.par BM_DRNLHuman.par dataFileAif.par dataFileIn.par \
				dataFileOut.par displayACF2.par displayACFearly.par \
				displayACFlate.par displayACF.par displayBM.par \
				displayReceptorPotential.par displayStimulus.par \
				filtMultiBandpass.par filtScaleToStapes.par \
				filtStereocilia.par IHC_Meddis2000HSR.par \
				IHCreceptorPotential.par rampDown.par rampUp.par \
				stimHarmonic.par UtRedChans.par

MatlabFiles	= Bern.m computeAMS.m dichoticHarmonicStimulus.m \
				dioticHarmonicStimulus.m oddHarmonics.m

DataFiles	= dichoticHarmReference.wav dichoticHarms.wav dioticHarms100.wav \
				dioticHarms200.wav dioticHarms.wav

EXTRA_DIST	= $(SimScripts) $(ParFiles) $(DataFiles) $(SPFFiles) $(MatlabFiles)

include $(top_srcdir)/mkunxinst.env
