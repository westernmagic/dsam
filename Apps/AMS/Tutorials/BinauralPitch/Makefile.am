## Tutorials/BinauralPitch directory makefile.

##	Don't forget to find ways to implement Graphics and Parallel

AUTOMAKE_OPTIONS = foreign

DIRECTORY	= Tutorials/BinauralPitch

SimScripts	= pitchEarlyCrossover.sim pitchLateCrossover.sim

SPFFiles	= 

ParFiles	= acf.par BM_DRNLHuman.par dataFileAif.par dataFileIn.par \
				dataFileOut.par displayACFearly.par displayACFlate.par \
				displayBM.par displayHSR.par displayReceptorPotential.par \
				displaysACF.par displayStimulus.par filtIIRMoore.par \
				filtMultiBandpass.par filtScaleToStapes.par \
				filtStereocilia.par IHC_Meddis2000HSR.par \
				IHCreceptorPotential.par IHCRP_VelIn_GP.par mathOpstapes.par \
				pitchEarlyCrossover.par rampDown.par rampUp.par \
				stimHarmonic.par UtRedChans.par

MatlabFiles	= Bern.m computeAMS.m dichoticHarmonicStimulus.m \
				dioticHarmonicStimulus.m oddHarmonics.m \
				dichoticHarmonicStimulus.asv dioticHarmonicStimulus.asv

DataFiles	= dichoticF0100H13_22.wav dichoticF0100H3_12.wav \
				dioticF0100H13_22.wav dioticF0100H3_12.wav

EXTRA_DIST	= $(SimScripts) $(ParFiles) $(DataFiles) $(SPFFiles) $(MatlabFiles)

include $(top_srcdir)/mkunxinst.env
