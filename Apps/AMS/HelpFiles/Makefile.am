## Process this file with automake to produce Makefile.in
## src directory makefile.

AUTOMAKE_OPTIONS = foreign

DIRECTORY	= HelpFiles

EXTRA_DIST	= \
	AMSHE000.gif \
	AMSHE001.gif \
	AMSHE001.htm \
	AMSHE002.gif \
	AMSHE002.htm \
	AMSHE003.gif \
	AMSHE003.htm \
	AMSHE004.gif \
	AMSHE004.htm \
	AMSHE005.gif \
	AMSHE005.htm \
	AMSHE006.gif \
	AMSHE006.htm \
	AMSHE007.gif \
	AMSHE007.htm \
	AMSHE008.gif \
	AMSHE008.htm \
	AMSHE009.gif \
	AMSHE009.htm \
	AMSHE010.gif \
	AMSHE010.htm \
	AMSHE011.gif \
	AMSHE011.htm \
	AMSHE012.gif \
	AMSHE012.htm \
	AMSHE013.htm \
	AMSHE014.htm \
	AMSHE015.htm \
	AMSHE016.htm \
	AMSHE017.htm \
	AMSHE018.htm \
	AMSHE019.htm \
	AMSHE020.htm \
	AMSHE021.htm \
	AMSHE022.htm \
	AMSHE023.htm \
	AMSHE024.htm \
	AMSHE025.htm \
	AMSHE026.htm \
	AMSHE027.htm \
	AMSHE028.htm \
	AMSHE029.htm \
	AMSHE030.htm \
	AMSHE031.htm \
	AMSHE032.htm \
	AMSHE033.htm \
	AMSHE034.htm \
	AMSHE035.htm \
	AMSHE036.htm \
	AMSHE037.htm \
	AMSHE038.htm \
	AMSHelp.hhc \
	ams.hhp
	
install-data-local: Makefile.am
		mkdir -p $(pkgdatadir)/$(DIRECTORY)
		for data_file in $(EXTRA_DIST); do \
			$(INSTALL_DATA) $$data_file $(pkgdatadir)/$(DIRECTORY) ; \
		done

uninstall-local:
		cd $(pkgdatadir)/$(DIRECTORY) && rm -rf $(EXTRA_DIST)
