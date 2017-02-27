## Process this file with automake to produce Makefile.in
## Top directory makefile.

AUTOMAKE_OPTIONS = foreign


BUILT_SOURCES	= ogl.spec
SUBDIRS		= src include samples

ogl.spec: ogl.spec.in

ogl-config:	ogl-config.in

dist-hook: ogl.spec
	if test -e $(srcdir)/INSTALL.in && test -e $(srcdir)/README.in ; then \
	     CONFIG_FILES="INSTALL:$(srcdir)/INSTALL.in README:$(srcdir)/README.in" \
	     CONFIG_HEADERS= \
	     $(SHELL) config.status \
	     && cp INSTALL README $(distdir) ; \
	fi \
	&& cp ogl.spec $(distdir)
