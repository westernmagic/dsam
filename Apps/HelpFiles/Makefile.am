## Process this file with automake to produce Makefile.in
## HelpFiles directory makefile.

AUTOMAKE_OPTIONS = foreign

DIRECTORY	= HelpFiles

EXTRA_DIST	= \
	feval.hhp
	
install-data-local: Makefile.am
		mkdir -p $(pkgdatadir)/$(DIRECTORY)
		for data_file in $(EXTRA_DIST); do \
			$(INSTALL_DATA) $$data_file $(pkgdatadir)/$(DIRECTORY) ; \
		done

uninstall-local:
		cd $(pkgdatadir)/$(DIRECTORY) && rm -rf $(EXTRA_DIST)
