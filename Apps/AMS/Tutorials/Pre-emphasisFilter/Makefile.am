## Tutorials/Pre-emphasisFilter directory makefile.

##	Don't forget to find ways to implement Graphics and Parallel

AUTOMAKE_OPTIONS = foreign

DIRECTORY	= Tutorials/Pre-emphasisFilter

SimScripts	= auditoryNerveFileIn.sim auditoryNervePreemphFFT.sim
				auditoryNerve.sim PreemphFFT.sim

SPFFiles	= auditoryNervePreemphFFT.spf auditoryNerve.spf clickProblem.spf

ParFiles	= binomial.par BM_DRNL_GP4.par dataFileIn.par dataFileOut.par
				displayAN.par displayBM.par displayClick.par \
				displayReceptorPotential.par displayStimulus.par \
				filtFIRMoore.par filtIIRMoore.par filtMultiBandpass.par
				filtScaleToStapes.par filtStereocilia.par Fourier.par \
				IHC_Meddis2000HSR.par IHC_Meddis2000LSR.par \
				IHCRP_ShammaVelIn.par rampDown.par rampUp.par reduceDt.par \
				stimHarmonic.par stimulusClick.par

MatlabFiles	= 

DataFiles	= Roger.wav

EXTRA_DIST	= $(SimScripts) $(ParFiles) $(DataFiles) $(SPFFiles) $(MatlabFiles)

include $(top_srcdir)/mkunxinst.env
