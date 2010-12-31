## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS = foreign

bin_PROGRAMS	= ams_ng @AMS@

EXTRA_PROGRAMS	= ams

ams_SOURCES		= ams.h ams.cpp ams.xpm
ams_LDADD		= $(AMSRESOBJ) $(DSAMGUILDFLAGS)

ams_ng_SOURCES	= ams.h ams.xpm
ams_ng_LDADD	= ams_ng.$(OBJEXT) $(DSAMLDFLAGS)

BUILT_SOURCES	= ams.spec

EXTRA_DIST		= autogen.sh ams.par ams.rc amsicon.ico mkunxinst.env AMS.nsi LICENSE

AM_CPPFLAGS		= -DAMS_DATA_INSTALL_DIR="wxT(\"$(pkgdatadir)\")"

SUBDIRS			= CfgTools DEFAULT Matlab Octave MSVC HelpFiles Tutorials

CC = $(CXX)		# Ensure use of C++ compiler for the link stage.

ams.$(OBJEXT):		ams.cpp $(AMSRESOBJ)
					$(CXXCOMPILE) $(DSAMGUIINC) -c $(srcdir)/ams.cpp -o $@ $(AMSRESOBJ)

ams_ng.$(OBJEXT):	ams.cpp
					$(CXXCOMPILE) $(DSAMINC) -c $(srcdir)/ams.cpp -o $@

$(AMSRESOBJ):	Makefile ams.rc
	@if test x$(MINGW32) = xyes; then \
		$(RESCOMP) $(RESFLAGS) $(srcdir)/ams.rc -o $@ ; \
	fi

ams.spec: ams.spec.in

dist-hook: ams.spec
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
	&& cp ams.spec $(distdir) \
	&& cp AMSSetup.h $(distdir)
	
