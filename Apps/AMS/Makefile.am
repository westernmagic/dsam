## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS = foreign

bin_PROGRAMS	= ams ams_ng

ams_SOURCES		= ams.cpp ams.h ams.xpm
ams_LDADD		= $(DSAMGUILDFLAGS)

ams_ng_SOURCES	= ams.h ams.xpm
ams_ng_LDADD	= ams_ng.$(OBJEXT) $(DSAMLDFLAGS)

BUILT_SOURCES	= ams.spec

INCLUDES		= -DAMS_DATA_INSTALL_DIR="\"$(pkgdatadir)\""
AM_CFLAGS		= $(DSAMINC)
AM_CXXFLAGS		= $(DSAMGUIINC)

SUBDIRS			= CfgTools DEFAULT HelpFiles

ams.$(OBJEXT):		ams.cpp
					$(CXXCOMPILE) -c ams.cpp -o ams.$(OBJEXT)

ams_ng.$(OBJEXT):	ams.cpp
					$(COMPILE) -c ams.cpp -o ams_ng.$(OBJEXT)

ams.spec: ams.spec.in

dist-hook: ams.spec
	if test -e $(srcdir)/INSTALL.in && test -e $(srcdir)/README.in ; then \
	     CONFIG_FILES="INSTALL:$(srcdir)/INSTALL.in README:$(srcdir)/README.in"\
	     CONFIG_HEADERS= \
	     $(SHELL) config.status \
	     && cp INSTALL README $(distdir) ; \
	fi \
	&& cp ams.spec $(distdir)
