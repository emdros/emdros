# Include the SWIG_GENERATED_JAVAFILES macro
include Makefile.inc

SWIG_PROGRAM = @SWIG_PROGRAM@



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

AM_CPPFLAGS = -I$(top_srcdir)/include -I. @JDK_INCLUDES@

gen_files:
	echo "SWIG_GENERATED_JAVAFILES = \\" > ll.txt 
	cd $(srcdir) ; $(SWIG_PROGRAM) -DPOSTGRESQL -DSQLITE -DMYSQL -package jemdros -c++ -java libjemdros.i ; rm -f libjemdros_wrap.cxx
	cd $(srcdir) ; ls -l *.java | grep -v "TableRow.java" | grep -v "TestEmdros.java" | awk '{ print $$9; }' | sed -e 's/^/                /' | sed -e 's/$$/ \\/' >> ll.txt
	cd $(srcdir) ; echo "                TableRow.java" >> ll.txt
	cd $(srcdir) ; rm -f `cat ll.txt`
	@echo "ll.txt created."
	@echo "Now consider:"
	@echo ""
	@echo "   $$ diff Makefile.inc ll.txt"
	@echo "   $$ mv ll.txt Makefile.inc"
	@echo ""







libjemdros_wrap.cxx $(SWIG_GENERATED_JAVAFILES): libjemdros.i libemdfjava.i libmqljava.i
	cd $(srcdir) ; $(SWIG_PROGRAM) -package jemdros -c++ -java libjemdros.i

jemdros.jar: libjemdros_wrap.cxx
	cd $(srcdir) ; @JAVAC@ $(SWIG_GENERATED_JAVAFILES)
	cd $(srcdir) ; if test -d jemdros; then rm -rf jemdros; fi
	cd $(srcdir) ; mkdir jemdros
	cd $(srcdir) ; mv $(SWIG_CLASSFILES) $(SWIG_GENERATED_JAVAFILES) jemdros
	cd $(srcdir) ; @JAR@ cf0 jemdros.jar jemdros/*
	cd $(srcdir) ; rm -rf jemdros


SWIG_CLASSFILES = $(SWIG_GENERATED_JAVAFILES:.java=.class) 



TestEmdros.java: TestEmdros.java.in
	$(top_builddir)/config.status --file=TestEmdros.java


if SWIG_WITH_JAVA_WRAPPERS

SWIG_JAVA_SRC = \
      libjemdros_wrap.cxx \
      jemdros.jar 

MAYBE_BUILT = $(SWIG_JAVA_SRC) 

pkglib_LTLIBRARIES = libjemdros.la



TestEmdros.class: TestEmdros.java
	cd $(srcdir) ; @JAVAC@ -classpath $(prefix)/lib/emdros/jemdros.jar:. TestEmdros.java


class: TestEmdros.class

run:
	cd $(srcdir); @JAVA@ -cp  .:$(prefix)/lib/emdros/jemdros.jar TestEmdros

drop:
	mql -p changeme drop_db.mql


nodist_libjemdros_la_SOURCES = libjemdros_wrap.cxx 

libjemdros_la_LIBADD = $(top_builddir)/harvest/libharvest.la \
                       $(top_builddir)/MQL/libmql.la @PCRE_LIBADD_SWIG@ \
                       @EMDFLDADDSWIG@ @ZLIB_LIBADD_SWIG@ 
libjemdros_la_CXXFLAGS = $(AM_CXXFLAGS)
libjemdros_la_LDFLAGS = @SWIG_EXTRA_LIBS@ \
                        -version-info \
                        @LIB_SWIG_CURRENT@:@LIB_SWIG_REVISION@:@LIB_SWIG_AGE@ \
                        @DARWIN_LDFLAGS@ @SUN_LDFLAGS@
libjemdros_la_DEPENDENCIES = $(top_builddir)/MQL/libmql.la @PCRE_DEP_SWIG@ @ZLIB_DEP_SWIG@ \
                             @EMDFDEPSSWIG@ $(top_builddir)/harvest/libharvest.la


install-exec-hook: jemdros.jar
	cp ./jemdros.jar $(DESTDIR)$(pkglibdir)	
	rm -f $(DESTDIR)$(pkglibdir)/libjemdros.a


uninstall-hook:
	rm -f $(DESTDIR)$(pkglibdir)/jemdros.jar

macdist: 
	cp -fL README-Java jemdros.jar .libs/libjemdros.dylib TestEmdros.java $(MACDISTDIR)/development/SWIG/java/



else

SWIG_JAVA_SRC = 

MAYBE_BUILT = 

# Not SWIG_WITH_JAVA_WRAPPERS

# We must have these dummy install-hooks because they are defined
# in the other section

install-exec-hook:

uninstall-hook:

macdist:




endif # end if SWIG_WITH_JAVA_WRAPPERS

BUILT_SOURCES = $(MAYBE_BUILT) TestEmdros.java 

EXTRA_DIST = libjemdros.i \
             libemdfjava.i libmqljava.i \
             TestEmdros.java.in \
             README-Java drop_db.mql win32.mak \
             Makefile.inc $(BUILT_SOURCES)

DISTCLEANFILES =  $(SWIG_JAVA_SRC)

CLEANFILES = *~ core config.log ll.txt \
             TestEmdros.java \
	     TestEmdros.class TestEmdros$$EMdFObject.class \
             TestEmdros$$Phrase.class TestEmdros$$Word.class \
             libjemdros.la 

