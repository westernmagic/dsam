## Tutorials/Loudness directory makefile.

##	Don't forget to find ways to implement Graphics and Parallel

AUTOMAKE_OPTIONS = foreign

DIRECTORY	= Tutorials/Loudness

SimScripts	= bpFilterTest.sim filterFunction.sim Loudness.sim makeHarmSig.sim

SPFFiles	= bpFilterTest.spf filterFunction.spf Loudness.spf makeHarmSig.spf

ParFiles	= binomialHSR.par binomialLSR.par BM_DRNLHuman.par click.par \
				dataFileIn.par dataFileOut.par dataFileOutSignal.par \
				displayBM.par displayFiltResponse.par displayFourier.par \
				displayHSR.par displayLSR.par displayReceptorPotential.par \
				displayStimulus.par displaySumAN.par filtFIRELP.par \
				filtMBtemporalWindow.par filtScaleToStapes.par Fourier.par \
				IHC_Meddis2000HSR.par IHC_Meddis2000LSR.par \
				IHCRP_ShammaVelIn.par padSignal.par rampDown.par rampUp.par \
				reduceChannels.par stimHarmonic.par

MatlabFiles	= 

DataFiles	= harmsig.aif harmSig.wav

EXTRA_DIST	= $(SimScripts) $(ParFiles) $(DataFiles) $(SPFFiles) $(MatlabFiles)

include $(top_srcdir)/mkunxinst.env
