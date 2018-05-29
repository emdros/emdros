# Include the SWIG_GENERATED_JAVAFILES macro
include Makefile.inc

SWIG_PROGRAM = @SWIG_PROGRAM@

SWIG_JAVA_SRC = \
      libjemdros_wrap.cxx 

#
# On GNU g++, -O2 and above means that strict-aliasing is turned on.
# We want to turn it off for the Java bindings.
#
# The reason is that enforcing strict-aliasing rules does not mix well
# with all of the type-punning that the SWIG Java bindings are doing.
#
if COMPILER_IS_GNU_GPLUSPLUS
EXTRA_GPLUSPLUS_FLAGS = -fno-strict-aliasing
else
EXTRA_GPLUSPLUS_FLAGS =
endif

AM_CXXFLAGS = @SUN_CXXFLAGS@ @DARWIN_CXXFLAGS@ @ADDITIONAL_CXXFLAGS@ @DEBUGFLAG@ $(EXTRA_GPLUSPLUS_FLAGS)

AM_CPPFLAGS = -I. @JDK_INCLUDES@ -I$(top_srcdir)

gen_files:
	echo "SWIG_GENERATED_JAVAFILES = \\" > ll.txt 
	$(SWIG_PROGRAM) -package jemdros -c++ -java libjemdros.i ; rm -f libjemdros_wrap.cxx
	ls -l *.java | grep -v "TableRow.java" | grep -v "TestEmdros.java" | awk '{ print $$9; }' | sed -e 's/^/                /' | sed -e 's/$$/ \\/' >> ll.txt
	echo "                TableRow.java" >> ll.txt
	rm -f `cat ll.txt`
	@echo "ll.txt created."
	@echo "Now consider:"
	@echo ""
	@echo "   $$ diff Makefile.inc ll.txt"
	@echo "   $$ mv ll.txt Makefile.inc"
	@echo ""



if CAN_DO_SWIG
MAYBE_BUILT_SRC = $(SWIG_JAVA_SRC)
else
MAYBE_BUILT_SRC = 
endif




libjemdros_wrap.cxx: libjemdros.i
	$(SWIG_PROGRAM) -package jemdros -c++ -java libjemdros.i

jemdros.jar: libjemdros_wrap.cxx
	@JAVAC@ $(SWIG_GENERATED_JAVAFILES)
	if test -d jemdros; then rm -rf jemdros; fi
	mkdir jemdros
	mv $(SWIG_CLASSFILES) $(SWIG_GENERATED_JAVAFILES) jemdros
	@JAR@ cf0 jemdros.jar jemdros/*
	rm -rf jemdros




SWIG_CLASSFILES = $(SWIG_GENERATED_JAVAFILES:.java=.class) 


TestEmdros.java: TestEmdros.java.in
	$(top_builddir)/config.status --file=TestEmdros.java


if SWIG_WITH_JAVA_WRAPPERS


pkglib_LTLIBRARIES = libjemdros.la


MAYBE_BUILT_SRC += jemdros.jar 


TestEmdros.class: TestEmdros.java
	@JAVAC@ -classpath $(prefix)/lib/emdros/jemdros.jar:. TestEmdros.java


class: TestEmdros.class

run:
	cd $(srcdir); @JAVA@ -cp  .:$(prefix)/lib/emdros/jemdros.jar TestEmdros

drop:
	mql -p changeme drop_db.mql


nodist_libjemdros_la_SOURCES = libjemdros_wrap.cxx 

libjemdros_la_LIBADD = @EMDROS_LDADD_BACKBACK@
libjemdros_la_CXXFLAGS = $(AM_CXXFLAGS)
libjemdros_la_LDFLAGS = -version-info \
                        @LIB_EMDROS_CURRENT@:@LIB_EMDROS_REVISION@:@LIB_EMDROS_AGE@ \
                        @DARWIN_LDFLAGS@ @SUN_LDFLAGS@
libjemdros_la_DEPENDENCIES = @EMDROS_DEPS_BACKBACK@ 

install-exec-hook: jemdros.jar
	cp ./jemdros.jar $(DESTDIR)$(pkglibdir)	
	rm -f $(DESTDIR)$(pkglibdir)/libjemdros.a


uninstall-hook:
	rm -f $(DESTDIR)$(pkglibdir)/jemdros.jar

macdist: 
	cp -fL README-Java jemdros.jar .libs/libjemdros.dylib TestEmdros.java $(MACDISTDIR)/development/SWIG/java/


else

MAYBE_BUILT_SRC += 

# Not SWIG_WITH_JAVA_WRAPPERS

# We must have these dummy install-hooks because they are defined
# in the other section

install-exec-hook:

uninstall-hook:

macdist:




endif # end if SWIG_WITH_JAVA_WRAPPERS

BUILT_SOURCES = $(MAYBE_BUILT_SRC) \
                TestEmdros.java



EXTRA_DIST = libjemdros.i \
             TestEmdros.java.in \
             README-Java drop_db.mql win32.mak \
             Makefile.inc $(BUILT_SOURCES) 

CLEANFILES = *~ core config.log ll.txt \
             TestEmdros.java \
	     TestEmdros.class TestEmdros$$EMdFObject.class \
             TestEmdros$$Phrase.class TestEmdros$$Word.class \
             libjemdros.la \
             $(SWIG_GENERATED_JAVAFILES) \
             $(SWIG_CLASSFILES) \
             $(BUILT_SOURCES)


DISTCLEANFILES =  $(BUILT_SOURCES)
