## DEFAULT directory makefile.

##	Don't forget to find ways to implement Graphics and Parallel

AUTOMAKE_OPTIONS = foreign

DIRECTORY	= DEFAULT
SimScripts	= AIM.sim  AIMghs.sim  AIMghsa.sim

ParFiles	= BMMdflt.par Compres1.par Intensity1.par NAPdflt.par SAIdflt.par \
				acfDflt.par dBSPLdflt.par dispDflt.par gateDflt.par \
				gtfDflt.par gtfSmall.par lowpass.par lowpass1200.par \
				medDflt.par midearDflt.par reduceDflt.par stiACdflt.par \
				stiDflt.par strobeACdflt.par strobeDflt.par sumSAIdflt.par \
				waveDflt.par

DataFiles	= cegc_br.raw

EXTRA_DIST	= $(SimScripts) $(ParFiles) $(DataFiles)

install-data-local: Makefile.am
		mkdir -p $(pkgdatadir)/$(DIRECTORY)
		$(INSTALL_DATA) $(EXTRA_DIST) $(pkgdatadir)/$(DIRECTORY)

uninstall-local:
		cd $(pkgdatadir)/$(DIRECTORY) && rm -rf $(EXTRA_DIST)
