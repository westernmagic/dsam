## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS = foreign

libexec_PROGRAMS	= RunDSAMSim@DLLEXT@

noinst_HEADERS	= MatMainApp.h

RunDSAMSim@DLLEXT@_SOURCES	= RunDSAMSim.cpp MatMainApp.cpp

BUILT_SOURCES	= RunDSAMSim.spec RunDSAMSimSetup.h

EXTRA_DIST		= autogen.sh RunDSAMSim.rc RunDSAMSim.def RunDSAMSim.m RunDSAMSim.ise \
					CompileRunDSAMSim.m

SUBDIRS			= CfgTools MSVC Examples

MatMainApp.$(OBJEXT): MatMainApp.cpp
					$(CXXCOMPILE) -fPIC -DLIBRARY_COMPILE $(DSAMINC) -c $<

RunDSAMSim.$(OBJEXT): RunDSAMSim.cpp
					$(DLLCOMPILE) -DLIBRARY_COMPILE -c $< $(DSAMINC) -o $@

RunDSAMSim@DLLEXT@: $(RunDSAMSim@DLLEXT@_OBJECTS)
					$(DLLCOMPILE) -o $@ $(RunDSAMSim@DLLEXT@_OBJECTS) $(DSAMLDFLAGS)

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
