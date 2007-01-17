## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS = foreign

libexec_PROGRAMS	= RunDSAMSim@DLLEXT@

noinst_HEADERS	= MatMainApp.h

RunDSAMSim@DLLEXT@_SOURCES	= RunDSAMSim.cpp MatMainApp.cpp

BUILT_SOURCES	= RunDSAMSim.spec

EXTRA_DIST		= RunDSAMSim.rc RunDSAMSim.def RunDSAMSim.m

SUBDIRS			= CfgTools MSVC

MatMainApp.$(OBJEXT): MatMainApp.cpp
					$(CXXCOMPILE) -fPIC -DLIBRARY_COMPILE $(DSAMINC) -c $<

RunDSAMSim.$(OBJEXT): RunDSAMSim.cpp
					XTRA_CXXFLAGS="-fPIC -DLIBRARY_COMPILE -w" $(DLLCOMPILE) -c $< \
					 $(DSAMINC) -o $@

RunDSAMSim@DLLEXT@: $(RunDSAMSim@DLLEXT@_OBJECTS)
					$(DLLCOMPILE) -o $@ $(RunDSAMSim@DLLEXT@_OBJECTS) $(DSAMLDFLAGS)

RunDSAMSim.spec: RunDSAMSim.spec.in

dist-hook: RunDSAMSim.spec
	if test -e $(srcdir)/INSTALL.in && test -e $(srcdir)/README.in ; then \
	     CONFIG_FILES="INSTALL:$(srcdir)/INSTALL.in README:$(srcdir)/README.in"\
	     CONFIG_HEADERS= \
	     $(SHELL) config.status \
	     && cp INSTALL README $(distdir) \
		 &&	if [ -x /usr/bin/unix2dos ] ; then \
		 		/usr/bin/unix2dos -n README README.txt ; \
				cp README.txt $(distdir); \
			fi \
	fi \
	&& cp RunDSAMSim.spec $(distdir)
