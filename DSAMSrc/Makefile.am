## Process this file with automake to produce Makefile.in
## Top directory makefile.

AUTOMAKE_OPTIONS = foreign

pkginclude_HEADERS	= DSAMSetup.h

bin_SCRIPTS		= dsam-config
BUILT_SOURCES	= dsam.spec dsam-config
EXTRA_DIST		= autogen.sh LICENSE DSAM_SDK.nsi
SUBDIRS			= CfgTools Include src ModuleSpecs Examples MSVC

dsam.spec: dsam.spec.in

dsam-config:	dsam-config.in

dist-hook: dsam.spec
	if test -e $(srcdir)/INSTALL.in && test -e $(srcdir)/README.in ; then \
	     CONFIG_FILES="INSTALL:$(srcdir)/INSTALL.in README:$(srcdir)/README.in" \
	     CONFIG_HEADERS= \
	     $(SHELL) config.status \
	     && cp INSTALL README $(distdir) ; \
	fi \
	&& cp dsam.spec $(distdir)
