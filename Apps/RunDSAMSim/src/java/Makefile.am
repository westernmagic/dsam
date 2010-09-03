## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS = foreign

JAVAROOT = .
jarfile = $(PACKAGE)jni-$(VERSION).jar
jardir = $(datadir)/java

jhdrout = .

$(jarfile): $(dist_noinst_JAVA) 
	$(JAR) cf $(JARFLAGS) $@ *.class

jar_DATA = $(jarfile)

java-headers.stamp: $(dist_noinst_JAVA)
	@list="`echo $(dist_noinst_JAVA) | sed -e 's|\.java||g' -e 's|/|.|g'`"; \
	for class in $$list; do \
		echo "$(JAVAH) -jni -d $(jhdrout) $(JAVAHFLAGS) $$class"; \
		$(JAVAH) -jni -d $(jhdrout) $(JAVAHFLAGS) $$class; \
	done
	@echo "JNI headers generated" > java-headers.stamp

all-local: java-headers.stamp

CLEANFILES = $(jarfile) *.class java-headers.stamp $(jhdrout)/*.h

EXTRA_DIST			= RunDSAMSimJ.h

dist_noinst_JAVA	= RunDSAMSimJ.java
