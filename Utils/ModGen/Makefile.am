## Process this file with automake to produce Makefile.in

bin_PROGRAMS	= modgen
noinst_HEADERS	= ModGen.h MGGeneral.h Strings.h Symbol.h  Token.h \
					WriteCodeUtils.h WriteHeaderCode.h WriteSrcCode.h \
					Init.h
modgen_SOURCES	= Parser.y ModGen.c Init.c MGGeneral.c Strings.c Symbol.c \
					Token.c WriteCodeUtils.c WriteHeaderCode.c \
					WriteSrcCode.c

pkginclude_HEADERS	= Parser.h 
BUILT_SOURCES		= Parser.c
YFLAGS			= -dl
EXTRA_DIST		= makefile.dos modgen.1
man_MANS		= modgen.1
