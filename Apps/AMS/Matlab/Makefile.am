## DEFAULT directory makefile.

##	Don't forget to find ways to implement Graphics and Parallel

AUTOMAKE_OPTIONS = foreign

DIRECTORY	= Matlab
M_files		= ReadAIFF.m ReadIEEE.m

EXTRA_DIST	= $(M_files)

install-data-local: Makefile.am
		mkdir -p $(pkgdatadir)/$(DIRECTORY)
		for data_file in $(EXTRA_DIST); do \
			$(INSTALL_DATA) $$data_file $(pkgdatadir)/$(DIRECTORY) ; \
		done

uninstall-local:
		cd $(pkgdatadir)/$(DIRECTORY) && rm -rf $(EXTRA_DIST)
