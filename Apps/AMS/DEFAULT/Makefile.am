## DEFAULT directory makefile.

##	Don't forget to find ways to implement Graphics and Parallel

AUTOMAKE_OPTIONS = foreign

SimScripts	= AIM.sim  AIMghs.sim  AIMghsa.sim

ParFiles	= BMMdflt.par Compres1.par Intensity1.par NAPdflt.par SAIdflt.par \
				acfDflt.par dBSPLdflt.par dispDflt.par gateDflt.par \
				gtfDflt.par gtfSmall.par lowpass.par lowpass1200.par \
				medDflt.par midearDflt.par reduceDflt.par stiACdflt.par \
				stiDflt.par strobeACdflt.par strobeDflt.par sumSAIdflt.par \
				waveDflt.par

DataFiles	= cegc_br.raw

EXTRA_DIST	= $(SimScripts) $(ParFiles) $(DataFiles)

install-data-local:
		mkdir -p $(prefix)/share/ams/DEFAULT
		$(INSTALL_DATA) $(EXTRA_DIST) $(prefix)/share/ams/DEFAULT
