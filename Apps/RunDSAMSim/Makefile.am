## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS = foreign

ACLOCAL_AMFLAGS = -I m4

BUILT_SOURCES	= RunDSAMSim.spec RunDSAMSimSetup.h

EXTRA_DIST		= autogen.sh RunDSAMSim.nsi LICENSE

DISTCLEANFILES	= *.fo

SUBDIRS			= CfgTools debian MSVC Examples src/MatLib

if USE_JAVA_SUPPORT
  SUBDIRS += src/java
endif

if LIBRUNDSAMSIM
  SUBDIRS += src/LibRunDSAMSim
endif

if USE_RUNDSAMSIM
  SUBDIRS += src/cpp
endif

if USE_RUNDSAMSIMGEN
  SUBDIRS += src/LibRunDSAMSimGen
endif

if USE_PYTHON_SUPPORT
  SUBDIRS += src/python
endif

############################ ReadMe Generation ################################

BASE_FILE	= README
TEMP_DIR	= /tmp
TEMP_FILE	= $(TEMP_DIR)/temp.xml
HTML_XSL	= $(@XSL_PREFIX@)/html/docbook.xsl
FO_XSL		= $(@XSL_PREFIX@)/fo/docbook.xsl
XSLTPROC	= xsltproc
FOP			= fop
XSL_ARGS	= --xinclude --param bibliography.numbered 1 --param section.autolabel 1 \
				--param passivetex.extensions 1 --stringparam paper.type A4

$(BASE_FILE).xml:	$(BASE_FILE).in
		CONFIG_FILES="README.xml:$(srcdir)/README.in"\
	     CONFIG_HEADERS= \
	     $(SHELL) config.status

$(BASE_FILE).fo:		$(BASE_FILE).xml
		$(XSLTPROC) $(XSL_ARGS) $(FO_XSL) $(BASE_FILE).xml > $(BASE_FILE).fo

$(BASE_FILE).rtf:	$(BASE_FILE).fo 
		$(FOP) $(BASE_FILE).fo -rtf $(BASE_FILE).rtf

$(BASE_FILE).pdf:	$(BASE_FILE).fo 
		$(FOP) $(BASE_FILE).fo -pdf $(BASE_FILE).pdf

$(BASE_FILE).txt:	$(BASE_FILE).fo
		$(FOP) $(BASE_FILE).fo -txt $(BASE_FILE).txt
		rm -f $(BASE_FILE).fo

$(BASE_FILE).html:	$(BASE_FILE).xml
		$(XSLTPROC) -o $(BASE_FILE).html $(HTML_XSL) $(BASE_FILE).xml

check-xml:	$(BASE_FILE).xml
		xmllint --xinclude $(BASE_FILE).xml > $(TEMP_FILE)
		jing -t -i $(@RNG_FILE@) $(TEMP_FILE) > jing.log 2>&1

############################ Distribution Hook ################################

RunDSAMSim.spec: RunDSAMSim.spec.in

dist-hook: RunDSAMSim.spec README.rtf README.pdf
	if test -e $(srcdir)/INSTALL.in && test -e $(srcdir)/README.in ; then \
	     CONFIG_FILES="INSTALL:$(srcdir)/INSTALL.in " \
	     CONFIG_HEADERS= \
	     $(SHELL) config.status \
	     && cp INSTALL README.xml $(distdir) \
		 &&	if [ -e README.rtf ] ; then \
				cp README.rtf $(distdir); \
			fi \
		 &&	if [ -e README.pdf ] ; then \
				cp README.pdf $(distdir); \
			fi \
	fi \
	&& cp RunDSAMSim.spec $(distdir) \
	&& cp RunDSAMSimSetup.h $(distdir)
