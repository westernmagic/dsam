## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS = foreign

bin_PROGRAMS	= ams ams_ng

ams_SOURCES		= ams.h ams.cpp ams.xpm
ams_LDADD		= $(AMSRESOBJ) $(DSAMGUILDFLAGS)

ams_ng_SOURCES	= ams.h ams.xpm
ams_ng_LDADD	= ams_ng.$(OBJEXT) $(DSAMLDFLAGS)

pkginclude_HEADERS	= AMSSetup.h
BUILT_SOURCES	= ams.spec

EXTRA_DIST		= ams.par ams.rc amsicon.ico

INCLUDES		= -DAMS_DATA_INSTALL_DIR="\"$(pkgdatadir)\""
AM_CFLAGS		= $(DSAMINC)
AM_CXXFLAGS		= $(DSAMGUIINC)

SUBDIRS			= CfgTools DEFAULT HelpFiles Matlab MSVC

ams.$(OBJEXT):		ams.cpp $(AMSRESOBJ)
					$(CXXCOMPILE) -c ams.cpp -o ams.$(OBJEXT) $(AMSRESOBJ)

ams_ng.$(OBJEXT):	ams.cpp
					$(COMPILE) -c ams.cpp -o ams_ng.$(OBJEXT)

$(AMSRESOBJ):	Makefile ams.rc
	@if test x$(MINGW32) = xyes; then \
		$(RESCOMP) $(RESFLAGS) $(srcdir)/ams.rc -o $@ ; \
	fi

ams.spec: ams.spec.in

dist-hook: ams.spec
	if test -e $(srcdir)/INSTALL.in && test -e $(srcdir)/README.in ; then \
	     CONFIG_FILES="INSTALL:$(srcdir)/INSTALL.in README:$(srcdir)/README.in"\
	     CONFIG_HEADERS= \
	     $(SHELL) config.status \
	     && cp INSTALL README $(distdir) ; \
	fi \
	&& cp ams.spec $(distdir)
