## Process this file with automake to produce Makefile.in
## src directory makefile.

AUTOMAKE_OPTIONS = foreign

DIRECTORY	= HelpFiles

EXTRA_DIST	= \
	AMSHe000.gif \
	AMSHe001.gif \
	AMSHe001.htm \
	AMSHe002.gif \
	AMSHe002.htm \
	AMSHe003.gif \
	AMSHe003.htm \
	AMSHe004.gif \
	AMSHe004.htm \
	AMSHe005.gif \
	AMSHe005.htm \
	AMSHe006.gif \
	AMSHe006.htm \
	AMSHe007.gif \
	AMSHe007.htm \
	AMSHe008.gif \
	AMSHe008.htm \
	AMSHe009.gif \
	AMSHe009.htm \
	AMSHe010.gif \
	AMSHe010.htm \
	AMSHe011.gif \
	AMSHe011.htm \
	AMSHe012.gif \
	AMSHe012.htm \
	AMSHe013.htm \
	AMSHe014.htm \
	AMSHe015.htm \
	AMSHe016.htm \
	AMSHe017.htm \
	AMSHe018.htm \
	AMSHe019.htm \
	AMSHe020.htm \
	AMSHe021.htm \
	AMSHe022.htm \
	AMSHe023.htm \
	AMSHe024.htm \
	AMSHe025.htm \
	AMSHe026.htm \
	AMSHe027.htm \
	AMSHe028.htm \
	AMSHe029.htm \
	AMSHe030.htm \
	AMSHe031.htm \
	AMSHe032.htm \
	AMSHe033.htm \
	AMSHe034.htm \
	AMSHe035.htm \
	AMSHe036.htm \
	AMSHe037.htm \
	AMSHe038.htm \
	AMSHe039.htm \
	AMSHe040.htm \
	AMSHe_c.htm \
	AMSHefc.htm \
	AMSHe.htm \
	AMSHelp.hhc \
	AMSHe_t.htm \
	ams.hhp
	
install-data-local: Makefile.am
		mkdir -p $(pkgdatadir)/$(DIRECTORY)
		for data_file in $(EXTRA_DIST); do \
			$(INSTALL_DATA) $$data_file $(pkgdatadir)/$(DIRECTORY) ; \
		done

uninstall-local:
		cd $(pkgdatadir)/$(DIRECTORY) && rm -rf $(EXTRA_DIST)
