## Tutorials/AuditoryNerve directory makefile.

##	Don't forget to find ways to implement Graphics and Parallel

AUTOMAKE_OPTIONS = foreign

DIRECTORY	= Tutorials/AuditoryNerve

SimScripts	= auditoryNerveFileIn.sim auditoryNervePreemphFFT.sim \
				auditoryNerve.sim

SPFFiles	= auditoryNervePreemphFFT.spf auditoryNerve.spf

ParFiles	= binomial.par BM_DRNL_GP4.par dataFileIn.par dataFileOut.par \
				displayAN.par displayBM.par displayClick.par \
				displayReceptorPotential.par displayStimulus.par \
				filtFIRMoore.par filtMultiBandpass.par filtScaleToStapes.par \
				filtStereocilia.par Fourier.par IHC_Meddis2000HSR.par \
				IHC_Meddis2000LSR.par IHCRP_ShammaVelIn.par rampDown.par \
				rampUp.par reduceDt.par stimHarmonic.par stimulusClick.par

DataFiles	= Roger.wav

EXTRA_DIST	= $(SimScripts) $(ParFiles) $(DataFiles) $(SPFFiles)

include $(top_srcdir)/mkunxinst.env
