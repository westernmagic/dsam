## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS = foreign

bin_PROGRAMS	= RunDSAMSim

noinst_HEADERS	= MatMainApp.h

RunDSAMSim_SOURCES	= RunDSAMSim.cpp MatMainApp.cpp

BUILT_SOURCES	= RunDSAMSim.spec

EXTRA_DIST		= RunDSAMSim.rc

SUBDIRS			= CfgTools MSVC

MatMainApp.$(OBJEXT): MatMainApp.cpp
					$(CXXCOMPILE) -fPIC -DLIBRARY_COMPILE $(DSAMINC) -c $< -o $@

RunDSAMSim$(EXEEXT): RunDSAMSim.cpp MatMainApp.$(OBJEXT)
					$(MATCOMPILE) -DLIBRARY_COMPILE -o $@  $< \
					  MatMainApp.$(OBJEXT)


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
