## Process this file with automake to produce Makefile.in
## Octave directory.

AUTOMAKE_OPTIONS = foreign

DIRECTORY	= Octave
EXTRA_DIST	= AROCarney800.m \
	AROCH35.m \
	ARODRNLCH35.m \
	AROGammaC800.m \
	DispCGFFuncs.m \
	DispCPFFuncs.m \
	DispDProds.m \
	DispFShape.m \
	DispImpResp.m \
	DispIOFuncs.m \
	DispPFFuncs.m \
	DispPIFuncs.m \
	DispRCPFFuncs.m \
	DispRPFFuncs.m \
	DispTCurve.m \
	DispTTSRatio.m \
	DispTTSResp.m \
	DoPhase.m \
	FiltDisp1.m \
	FiltDispCarney800.m \
	FiltDispCH16.m \
	FiltDispCH35.m \
	FiltDispFIR1.m \
	FiltDispGammaC.m \
	FiltDispGammaC1k.m \
	FiltDispGammaC2k.m \
	FiltDispGammaC800.m \
	FiltDispGP2381.m \
	FiltDispHu2k.m \
	FiltDispL113.m \
	LogLogMultiLine.m \
	PlotMultiGraph.m \
	PlotMultiLine.m \
	PrintFigure.m \
	ReadData.m \
	ReadTitledData.m \
	SemiLogXMultiLine.m \
	SemiLogYMultiLine.m \
	SetPrintOutput.m \
	split.m

install-data-local: Makefile.am
		mkdir -p $(pkgdatadir)/$(DIRECTORY)
		for data_file in $(EXTRA_DIST); do \
			$(INSTALL_DATA) $$data_file $(pkgdatadir)/$(DIRECTORY) ; \
		done

uninstall-local:
		cd $(pkgdatadir)/$(DIRECTORY) && rm -rf $(EXTRA_DIST)
