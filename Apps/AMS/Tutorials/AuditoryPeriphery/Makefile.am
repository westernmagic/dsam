## Tutorials/AuditoryPeriphery directory makefile.

##	Don't forget to find ways to implement Graphics and Parallel

AUTOMAKE_OPTIONS = foreign

DIRECTORY	= Tutorials/AuditoryPeriphery

SimScripts	= ANperiod.sim ANpsth.sim auditoryNerve.sim filterbankDRNL.sim \
				filterbankGammatone.sim

SPFFiles	= filterbankGammatone.spf O_M_ear.spf

ParFiles	= ANpsthBM.par ANrefractory.par BM_drnlGP1.par BM_drnlGP.par \
				BM_gammatone.par displayBM.par displayFiberResponse.par \
				displayPH.par displayPSTH.par displayRP.par displaySignal.par \
				displaySynapse.par filtMultiBandpassGP.par histogram1.par \
				histogramPH.par histogramPSTH.par IHChsr2000spike.par \
				IHCRP_VelIn_GP.par mathOpstapes.par rampDown.par rampUp.par \
				stimTone.par

MatlabFiles	= 

DataFiles	= 

EXTRA_DIST	= $(SimScripts) $(ParFiles) $(DataFiles) $(SPFFiles) $(MatlabFiles)

include $(top_srcdir)/mkunxinst.env
