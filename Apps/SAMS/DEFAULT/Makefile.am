## DEFAULT directory makefile.

##	Don't forget to find ways to implement Graphics and Parallel

AUTOMAKE_OPTIONS = foreign

DIRECTORY	= DEFAULT
SimScripts	= AIM.sim  AIMghs.sim  AIMghsa.sim InputLevel.sim

SPFFiles	= AIM.spf AIMghs.spf AIMghsa.spf InputLevel.spf

ParFiles	= BMMdflt.par Compres1.par Intensity1.par NAPdflt.par SAIdflt.par \
				acfDflt.par dBSPLdflt.par dispDflt.par gateDflt.par \
				gtfDflt.par gtfSmall.par lowpass.par lowpass1200.par \
				medDflt.par midearDflt.par reduceDflt.par stiACdflt.par \
				stiDflt.par strobeACdflt.par strobeDflt.par sumSAIdflt.par \
				waveDflt.par outputDflt.par LevelDFIn.par LevelDisp1.par

DataFiles	= cegc_br.raw PTone57dBSPL.aif

EXTRA_DIST	= $(SimScripts) $(ParFiles) $(DataFiles) $(SPFFiles)

include $(top_srcdir)/mkunxinst.env
