## Tutorials/MATLABdemo directory makefile.

##	Don't forget to find ways to implement Graphics and Parallel

AUTOMAKE_OPTIONS = foreign

DIRECTORY	= Tutorials/MATLABRunDSAMSim

SimScripts	= gammatoneDemo.sim

SPFFiles	= gammatoneDemo.spf

ParFiles	= BM_gammatone.par filtMultiBandpassGP.par mathOpStapes.par


MatlabFiles	= MatlabDemo.m MatlabDemoRunDSAMsim.asv MatlabDemoRunDSAMsim.m \
				readDatFile.m




DataFiles	= 

EXTRA_DIST	= $(SimScripts) $(ParFiles) $(DataFiles) $(SPFFiles) $(MatlabFiles)

include $(top_srcdir)/mkunxinst.env
