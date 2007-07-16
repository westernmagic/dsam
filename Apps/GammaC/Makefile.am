## Process this file with automake to produce Makefile.in
## GammaC directory.

AUTOMAKE_OPTIONS = foreign

DIRECTORY	= GammaC
EXTRA_DIST	= Click1.par \
	Display1.par \
	feval.par \
	FindBin1.par \
	FIRMG2v02ms.par \
	GammaC800Hz.par \
	GammaC800Hz.sim \
	GammaC800Hz.spf \
	Intensty1.par \
	PreEmph1.par \
	PTone1.par \
	Ramp1.par \
	cgffunc.dat \
	cpffunc.dat \
	dpfunc.dat \
	fshape.dat \
	iofunc.dat \
	irfunc.dat \
	pffunc.dat \
	pifunc.dat \
	rcpffunc.dat \
	rpffunc.dat \
	tcurve.dat \
	ttsfunc.dat \
	ttsrfunc.dat

install-data-local: Makefile.am
		mkdir -p $(pkgdatadir)/$(DIRECTORY)
		for data_file in $(EXTRA_DIST); do \
			$(INSTALL_DATA) $$data_file $(pkgdatadir)/$(DIRECTORY) ; \
		done

uninstall-local:
		cd $(pkgdatadir)/$(DIRECTORY) && rm -rf $(EXTRA_DIST)
