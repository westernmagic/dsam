## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS = foreign

bin_PROGRAMS	= modgen
noinst_HEADERS	= ModGen.h MGGeneral.h Strings.h Symbol.h  Token.h \
					WriteCodeUtils.h WriteHeaderCode.h WriteSrcCode.h \
					Init.h
modgen_SOURCES	= Parser.y ModGen.c Init.c MGGeneral.c Strings.c Symbol.c \
					Token.c WriteCodeUtils.c WriteHeaderCode.c \
					WriteSrcCode.c

pkginclude_HEADERS	= Parser.h 
BUILT_SOURCES		= Parser.c modgen.spec
YFLAGS			= -dl
EXTRA_DIST		= makefile.dos modgen.1
man_MANS		= modgen.1

SUBDIRS			= CfgTools

modgen.spec:	modgen.spec.in

dist-hook: modgen.spec
	if test -e $(srcdir)/INSTALL.in && test -e $(srcdir)/README.in ; then \
	     CONFIG_FILES="INSTALL:$(srcdir)/INSTALL.in README:$(srcdir)/README.in"\
	     CONFIG_HEADERS= \
	     $(SHELL) config.status \
	     && cp INSTALL README $(distdir) ; \
	fi \
	&& cp modgen.spec $(distdir)
