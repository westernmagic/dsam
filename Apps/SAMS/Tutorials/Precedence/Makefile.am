## Tutorials/Precedence directory makefile.

##	Don't forget to find ways to implement Graphics and Parallel

AUTOMAKE_OPTIONS = foreign

DIRECTORY	= Tutorials/Precedence

SimScripts	= precedence.sim

SPFFiles	= 

ParFiles	= BM_DRNLHuman.par ccf.par click_L1.par click_L2.par click_R1.par \
				click_R2.par dataFileOut.par displayACF.par displayBM.par \
				displayCCF.par displayCCFsummary.par \
				displayReceptorPotential.par displaySpikeProb.par \
				displayStimulus.par filtIIRMoore.par filtStereocilia.par \
				IHC_Meddis2000HSR.par IHCRP_VelIn_GP.par mathOpClickLevel.par \
				mathOpstapes.par UtRedChans.par

DataFiles	=

EXTRA_DIST	= $(SimScripts) $(ParFiles) $(DataFiles) $(SPFFiles)

include $(top_srcdir)/mkunxinst.env
