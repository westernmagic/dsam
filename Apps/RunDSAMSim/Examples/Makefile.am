## Process this file with automake to produce Makefile.in
## CfgTools directory.

AUTOMAKE_OPTIONS = foreign

DIRECTORY	= Examples

SimScripts	= gammatoneDemo.sim

SPFFiles	= gammatoneDemo.spf LGuineaPigCN001_721Hz_HS1.spf \
				LGuineaPigCN001_721Hz.spf GPDRNL57_AN057_OC039.spf

ParFiles	= BM_gammatone.par filtMultiBandpassGP.par mathOpStapes.par


MatlabFiles	= DataXAxis.m DataXAxis2.m MatlabDemoRunDSAMSim.m readDatFile.m \
				ExternalIn1.m Simple1.m PlotPSTH.m PureTone.m \
				DBSPL2MicroPascals.m DataXAxis2.m TimeReport.m \
				PlotRateIntensity.m GetRunDSAMSimJInfo.m

PythonFiles	= DemoRunDSAMSim.py

DataFiles	= 

EXTRA_DIST	= $(SimScripts) $(ParFiles) $(DataFiles) $(SPFFiles) \
				$(MatlabFiles) $(PythonFiles)

include $(top_srcdir)/mkunxinst.env
