## Tutorials/AnfiberModels/ANspikeStream directory makefile.

##	Don't forget to find ways to implement Graphics and Parallel

AUTOMAKE_OPTIONS = foreign

DIRECTORY	= Tutorials/AnfiberModels/ANspikeStream

SimScripts	= spikeStream.sim

SPFFiles	= batSpikeStream.spf spikeStream.spf

ParFiles	= ANrefracCarneySpike.par BM_DRNL_GP4.par dataFileOutSpikes.par \
				displayDRNL.par displayIHCRP.par displayPSTH.par \
				displayStimulus.par filtMultiBandpassGP.par \
				IHChsr2000spike.par IHCRP_VelIn_GP.par output.par \
				outputPeriodH.par outputPSTH.par rampDown.par rampUp.par \
				stimulusTone.par

DataFiles	= 

EXTRA_DIST	= $(SimScripts) $(ParFiles) $(DataFiles) $(SPFFiles)

include $(top_srcdir)/mkunxinst.env
