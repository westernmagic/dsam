## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS = foreign

#OTHERSRC	= Utils.c TuningCurve.c  PhaseIntFuncs.c TwoTSResp.c \
#				PhaseFreqFuncs.c TwoTSRatio.c \
#				DistProds.c RelPhaseFreqFuncs.c ClickGainFreqFuncs.c \
#				ClickPhaseFreqFuncs.c RelClickPhaseFreqFuncs.c feval.xpm
#
#INCFILES	= feval.h Utils.h TuningCurve.h MultiIOFuncs.h TwoTSResp.h \
#				 PhaseIntFuncs.h PhaseFreqFuncs.h TwoTSRatio.h \
#				 DistProds.h RelPhaseFreqFuncs.h \
#				ClickGainFreqFuncs.h ClickPhaseFreqFuncs.h \
#				RelClickPhaseFreqFuncs.h

OTHERSRC	= feval.xpm Utils.c TuningCurve.c ImpulseResp.c FilterShape.c \
				MultiIOFuncs.c

INCFILES	= feval.h Utils.h TuningCurve.h ImpulseResp.h FilterShape.h \
				MultiIOFuncs.h

bin_PROGRAMS	= feval feval_ng

feval_SOURCES	= feval.cpp $(INCFILES) $(OTHERSRC)
feval_LDADD		= $(fevalRESOBJ) $(DSAMGUILDFLAGS)

feval_ng_SOURCES= $(INCFILES) $(OTHERSRC)
feval_ng_LDADD	= feval_ng.$(OBJEXT) $(DSAMLDFLAGS)

BUILT_SOURCES	= feval.spec

EXTRA_DIST		= autogen.sh

INCLUDES		= -DFEVAL_DATA_INSTALL_DIR="wxT(\"$(pkgdatadir)\")"
AM_CFLAGS		= $(DSAMINC)
AM_CXXFLAGS		= $(DSAMGUIINC)

SUBDIRS			= CfgTools HelpFiles Octave Matlab GammaC

feval.$(OBJEXT):	feval.cpp $(fevalRESOBJ) $(OTHERSRC) $(INCFILES)
					$(CXXCOMPILE) -c feval.cpp -o feval.$(OBJEXT)

feval_ng.$(OBJEXT):	feval.cpp $(OTHERSRC) $(INCFILES)
					$(COMPILE) -c feval.cpp -o feval_ng.$(OBJEXT)

$(fevalRESOBJ):	Makefile feval.rc
	if test x$(MINGW32) = xyes; then \
		$(RESCOMP) $(RESFLAGS) $(srcdir)/feval.rc -o $@ ; \
	fi

feval.spec: feval.spec.in

dist-hook: feval.spec
	if test -e $(srcdir)/INSTALL.in && test -e $(srcdir)/README.in ; then \
	     CONFIG_FILES="INSTALL:$(srcdir)/INSTALL.in README:$(srcdir)/README.in"\
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
	&& cp feval.spec $(distdir) \
	&& cp AMSSetup.h $(distdir)
	
