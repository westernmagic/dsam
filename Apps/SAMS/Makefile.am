## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS = foreign

bin_PROGRAMS	= sams_ng @SAMS@

EXTRA_PROGRAMS	= sams

sams_SOURCES		= sams.h sams.cpp
sams_LDADD		= $(SAMSRESOBJ) $(DSAMGUILDFLAGS)

sams_ng_SOURCES	= sams.h
sams_ng_LDADD	= sams_ng.$(OBJEXT) $(DSAMLDFLAGS)

BUILT_SOURCES	= sams.spec

desktopdir		= $(datadir)/applications
desktop_DATA	= sams.desktop

pixmapdir		= $(datadir)/pixmaps
pixmap_DATA		= sams.xpm

man1_MANS		= sams.1

EXTRA_DIST		= autogen.sh sams.par sams.rc samsicon.ico mkunxinst.env \
					sams.xpm sams.desktop SAMS.nsi COPYING

AM_CPPFLAGS		= -DSAMS_DATA_INSTALL_DIR="wxT(\"$(pkgdatadir)\")"

SUBDIRS			= CfgTools DEFAULT Matlab Octave MSVC debian HelpFiles Tutorials

CC = $(CXX)		# Ensure use of C++ compiler for the link stage.

sams.$(OBJEXT):		sams.cpp $(SAMSRESOBJ)
					$(CXXCOMPILE) $(DSAMGUIINC) -c $(srcdir)/sams.cpp -o $@ $(SAMSRESOBJ)

sams_ng.$(OBJEXT):	sams.cpp
					$(CXXCOMPILE) $(DSAMINC) -c $(srcdir)/sams.cpp -o $@

$(SAMSRESOBJ):	Makefile sams.rc
	@if test x$(MINGW32) = xyes; then \
		$(RESCOMP) $(RESFLAGS) $(srcdir)/sams.rc -o $@ ; \
	fi

sams.1: manpage.xml
		xmlto man manpage.xml

sams.spec: sams.spec.in

dist-hook: sams.spec
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
	&& cp sams.spec $(distdir) \
	&& cp SAMSSetup.h $(distdir)
	
