## Tutorials/Autocorrelation directory makefile.

##	Don't forget to find ways to implement Graphics and Parallel

AUTOMAKE_OPTIONS = foreign

DIRECTORY	= Tutorials/Autocorrelation

SimScripts	= acf.sim

SPFFiles	= acf.spf

ParFiles	= acf.par BM_DRNLHuman.par dataFileIn.par dataFileOut.par \
				displayACF.par displayBM.par displayHSR.par \
				displayReceptorPotential.par displayRP.par displayStimulus.par \
				filtIIRMoore.par filtMultiBandpass.par filtScaleToStapes.par \
				filtStereocilia.par IHC_Meddis2000HSR.par \
				IHCreceptorPotential.par IHCRP_VelIn_GP.par mathOpstapes.par \
				rampDown.par rampUp.par reduceChannels.par stimHarmonic.par

MatlabFiles	= MatlabAcfDemo.m


DataFiles	= dichoticHarms.wav dioticHarms.wav

EXTRA_DIST	= $(SimScripts) $(ParFiles) $(DataFiles) $(SPFFiles) $(MatlabFiles)

include $(top_srcdir)/mkunxinst.env
