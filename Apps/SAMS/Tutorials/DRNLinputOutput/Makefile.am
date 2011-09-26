## Tutorials/DRNLinputOutput directory makefile.

##	Don't forget to find ways to implement Graphics and Parallel

AUTOMAKE_OPTIONS = foreign

DIRECTORY	= Tutorials/DRNLinputOutput

SimScripts	= DRNLinpuOutput.sim

SPFFiles	= DRNLinpuOutput.spf

ParFiles	= BM_DRNLHuman.par dataFileOut.par displayDRNL.par \
				displayMiddleEar.par displayStimulus.par filtMultiBandpass.par \
				filtScaleToStapes.par Fourier.par intensity.par rampDown.par \
				rampUp.par stimulusClick.par stimulusTone.par

MatlabFiles	= DRNLinputOutput.m DRNLtest.m findEuclideanDistance.m \
				getRmsFromFile.m impairedFilter.m

DataFiles	= DRNLparams.xls

EXTRA_DIST	= $(SimScripts) $(ParFiles) $(DataFiles) $(SPFFiles) $(MatlabFiles)

include $(top_srcdir)/mkunxinst.env
