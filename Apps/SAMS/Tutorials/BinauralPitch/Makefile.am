## Tutorials/BinauralPitch directory makefile.

##	Don't forget to find ways to implement Graphics and Parallel

AUTOMAKE_OPTIONS = foreign

DIRECTORY	= Tutorials/BinauralPitch

SimScripts	= pitchEarlyCrossover.sim pitchLateCrossover.sim

SPFFiles	= pitchEarlyCrossover.spf pitchLateCrossover.spf

ParFiles	= acf.par BM_DRNLHuman.par dataFileAif.par dataFileIn.par \
				dataFileOut.par displayACFearly.par displayACFlate.par \
				displayBM.par displayHSR.par displayReceptorPotential.par \
				displayStimulus.par filtIIRMoore.par displaySACF.par \
				filtMultiBandpass.par filtScaleToStapes.par mathOpStapes.par \
				filtStereocilia.par IHC_Meddis2000HSR.par displayACF.par \
				IHCreceptorPotential.par IHCRP_VelIn_GP.par displayACF2.par \
				pitchEarlyCrossover.par rampDown.par rampUp.par \
				stimHarmonic.par UtRedChans.par

MatlabFiles	= Bern.m computeAMS.m dichoticHarmonicStimulus.m \
				dioticHarmonicStimulus.m oddHarmonics.m \
				dichoticHarmonicStimulus.asv dioticHarmonicStimulus.asv

DataFiles	= dichoticF0100H13_22.wav dichoticF0100H3_12.wav \
				dichoticHarmReference.wav dichoticHarms.wav \
				dioticF0100H13_22.wav dioticF0100H3_12.wav \
				dioticHarms100.wav dioticHarms200.wav dioticHarms.wav

EXTRA_DIST	= $(SimScripts) $(ParFiles) $(DataFiles) $(SPFFiles) $(MatlabFiles)

include $(top_srcdir)/mkunxinst.env
