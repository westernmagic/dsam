## Tutorials/AnfiberModels/ANmodelPSTH directory makefile.

##	Don't forget to find ways to implement Graphics and Parallel

AUTOMAKE_OPTIONS = foreign

DIRECTORY	= Tutorials/AnfiberModels/ANmodelPSTH

SimScripts	= SpikeTimesPSTH.sim

SPFFiles	= SpikeTimesPSTH.spf

ParFiles	= ANrefracCarneySpike.par BM_DRNL_GP4.par datafileOutPSTH.par \
				displayDRNL.par displayIHCRP.par displayIHCRPsingleCH.par \
				displayPSTH.par displayStimulus.par filtMultiBandpassGP.par \
				histPSTH.par IHChsr2000spike.par IHCRP_VelIn_GP.par \
				rampDown.par rampUp.par stimulusTonePSTH.par

DataFiles	= 

EXTRA_DIST	= $(SimScripts) $(ParFiles) $(DataFiles) $(SPFFiles)

include $(top_srcdir)/mkunxinst.env
