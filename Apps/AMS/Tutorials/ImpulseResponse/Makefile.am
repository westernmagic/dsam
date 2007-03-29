## Tutorials/ImpulseResponse directory makefile.

##	Don't forget to find ways to implement Graphics and Parallel

AUTOMAKE_OPTIONS = foreign

DIRECTORY	= Tutorials/ImpulseResponse

SimScripts	= fileInTest.sim impulseResponseFileIn.sim impulseResponse.sim \
				pureToneResponse.sim

SPFFiles	= impulseResponseFileIn.spf impulseResponse.spf \
				pureToneResponse.spf

ParFiles	= BM_DRNLHumanALN.par dataFileInClick.par dataFileOut.par \
				displayDRNL.par displayMiddleEar.par displayStimulus.par \
				DRNL_TestALN.par filtFIRELP.par filtMultiBandpass.par \
				filtScaleToStapes.par findBin.par Fourier.par intensity.par \
				PTone1.par rampDown.par rampUp.par stimulusClick.par 

MatlabFiles	= DRNL_RIfunction.m

DataFiles	= 4to10kHz.fig

EXTRA_DIST	= $(SimScripts) $(ParFiles) $(DataFiles) $(SPFFiles) $(MatlabFiles)

include $(top_srcdir)/mkunxinst.env
