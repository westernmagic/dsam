## Tutorials/AnfiberModels/ANmodelISIH directory makefile.

##	Don't forget to find ways to implement Graphics and Parallel

AUTOMAKE_OPTIONS = foreign

DIRECTORY	= Tutorials/AnfiberModels/ANmodelISIH

SimScripts	= SpikeTimesISIH.sim test.sim

SPFFiles	= SpikeTimesISIH.spf

ParFiles	= ANrefracCarneySpike.par binSignal.par BM_DRNL_GP4.par \
				displayDRNL.par displayIHCRP.par displayISIH.par \
				displayStimulus.par filtMultiBandpassGP.par histISIH.par \
				IHChsr2000spike.par IHCRP_VelIn_GP.par output.par rampDown.par \
				rampUp.par stimulusToneISIH.par

DataFiles	= 

EXTRA_DIST	= $(SimScripts) $(ParFiles) $(DataFiles) $(SPFFiles)

include $(top_srcdir)/mkunxinst.env
