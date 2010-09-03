## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS = foreign

ACLOCAL_AMFLAGS = -I m4

BUILT_SOURCES	= RunDSAMSim.spec RunDSAMSimSetup.h

EXTRA_DIST		= autogen.sh RunDSAMSim.nsi LICENSE

SUBDIRS			= CfgTools MSVC Examples src/MatLib

if USE_JAVA_SUPPORT
  SUBDIRS += src/java
endif

if LIBRUNDSAMSIM
  SUBDIRS += src/LibRunDSAMSim
endif

if USE_RUNDSAMSIM
  SUBDIRS += src/cpp
endif

RunDSAMSim.spec: RunDSAMSim.spec.in

dist-hook: RunDSAMSim.spec
	if test -e $(srcdir)/INSTALL.in && test -e $(srcdir)/README.in ; then \
	     CONFIG_FILES="INSTALL:$(srcdir)/INSTALL.in README.xml:$(srcdir)/README.in"\
	     CONFIG_HEADERS= \
	     $(SHELL) config.status \
	     && cp INSTALL README.xml $(distdir) \
		 &&	if [ -x /usr/bin/docbook2txt ] ; then \
		 		/usr/bin/docbook2txt README.xml ; \
				cp README.txt $(distdir); \
			fi \
		 &&	if [ -x /usr/bin/docbook2rtf ] ; then \
		 		/usr/bin/docbook2rtf README.xml ; \
				cp README.rtf $(distdir); \
			fi \
	fi \
	&& cp RunDSAMSim.spec $(distdir) \
	&& cp RunDSAMSimSetup.h $(distdir)
