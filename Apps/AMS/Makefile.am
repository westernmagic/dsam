## Process this file with automake to produce Makefile.in

bin_PROGRAMS	= ams ams_ng

ams_SOURCES		= ams.cpp ams.h ams.xpm
ams_LDFLAGS		= $(DSAMGUILDFLAGS)

ams_ng_SOURCES	= ams_ng.c ams.h ams.xpm
ams_ng_LDFLAGS	= $(DSAMLDFLAGS)

INCLUDES		= $-DAMS_DATA_INSTALL_DIR="\"$(pkgdatadir)\""
AM_CFLAGS		= $(DSAMINC)
AM_CXXFLAGS		= $(DSAMGUIINC)

SUBDIRS			= CfgTools DEFAULT

dist-hook: ams.spec
	if test -e $(srcdir)/INSTALL.in && test -e $(srcdir)/README.in ; then \
	     CONFIG_FILES="INSTALL:$(srcdir)/INSTALL.in README:$(srcdir)/README.in"\
	     CONFIG_HEADERS= \
	     $(SHELL) config.status \
	     && cp INSTALL README $(distdir) ; \
	fi \
	&& cp ams.spec $(distdir)
