# Include the SWIG_GENERATED_CSHARPFILES macro
include Makefile.inc


MONOCOMPILER = @MONOCOMPILER@

SWIG_PROGRAM = @SWIG_PROGRAM@


SWIG_CSHARP_SRC = \
      libcsemdros_wrap.cxx \
      EmdrosDotNet.dll \
      CSharpSources.zip

TestEmdros.cs: TestEmdros.cs.in
	$(top_builddir)/config.status --file=TestEmdros.cs


gen_files:
	echo "SWIG_GENERATED_CSHARPFILES = \\" > ll.txt 
	$(SWIG_PROGRAM) -namespace Emdros -c++ -csharp libcsemdros.i ; rm -f libcsemdros_wrap.cxx
	ls -l *.cs | grep -v "TableRow.cs" | grep -v "TestEmdros.cs" | awk '{ print $$9; }' | sed -e 's/^/                /' | sed -e 's/$$/ \\/' >> ll.txt
	echo "                TableRow.cs" >> ll.txt
	echo "" >> ll.txt ; echo "" >> ll.txt
#	#ls -l *.cs | grep -v "TestEmdros.cs" | awk '{ print $$9 ": libcsemdros_wrap.cxx"; }' >> ll.txt
	@echo "ll.txt created."
	@echo "Now consider:"
	@echo ""
	@echo "   $$ diff Makefile.inc ll.txt"
	@echo "   $$ mv ll.txt Makefile.inc"
	@echo ""

test: TestEmdros.cs EmdrosDotNet.dll
	$(MONOCOMPILER) -out:TestEmdros.exe -r:EmdrosDotNet.dll TestEmdros.cs
	@echo "Now run TestEmdros.exe."
	@echo "If on Mono:"
	@echo "     mono TestEmdros.exe"

DISTCLEANFILES =  $(SWIG_CSHARP_SRC) \
                  TestEmdros.cs 


AM_CXXFLAGS = @SUN_CXXFLAGS@ @DARWIN_CXXFLAGS@ @ADDITIONAL_CXXFLAGS@ @DEBUGFLAG@

AM_CPPFLAGS = -I$(top_srcdir)/include -I.

libcsemdros_wrap.cxx $(SWIG_GENERATED_CSHARPFILES): libcsemdros.i libemdf.i libmql.i 
	$(SWIG_PROGRAM) -namespace Emdros -c++ -csharp libcsemdros.i

EmdrosDotNet.dll: libcsemdros_wrap.cxx
	$(MONOCOMPILER) -target:library -out:EmdrosDotNet.dll $(SWIG_GENERATED_CSHARPFILES)

CSharpSources.zip: libcsemdros_wrap.cxx
	rm -f CSharpSources.zip
	zip CSharpSources.zip $(SWIG_GENERATED_CSHARPFILES)



CLEANFILES = *~ core config.log libcsemdros.la



if SWIG_WITH_CSHARP_WRAPPERS

MAYBE_BUILT = $(SWIG_CSHARP_SRC) \
              CSharpSources.zip \
              TestEmdros.cs \
              EmdrosDotNet.dll

pkglib_LTLIBRARIES = libcsemdros.la


nodist_libcsemdros_la_SOURCES = libcsemdros_wrap.cxx 

libcsemdros_la_LIBADD = $(top_builddir)/harvest/libharvest.la \
                        $(top_builddir)/MQL/libmql.la @PCRE_LIBADD_SWIG@ \
                        @EMDFLDADDSWIG@ @ZLIB_LIBADD_SWIG@ 

libcsemdros_la_LDFLAGS = @SWIG_EXTRA_LIBS@ \
                        -version-info \
                        @LIB_SWIG_CURRENT@:@LIB_SWIG_REVISION@:@LIB_SWIG_AGE@ \
                        @DARWIN_LDFLAGS@ @SUN_LDFLAGS@
libcsemdros_la_DEPENDENCIES = $(top_builddir)/MQL/libmql.la @PCRE_DEP_SWIG@ \
                             @ZLIB_DEP_SWIG@ @EMDFDEPSSWIG@ $(top_builddir)/harvest/libharvest.la


install-exec-hook: EmdrosDotNet.dll CSharpSources.zip
	cp ./EmdrosDotNet.dll $(DESTDIR)/$(pkglibdir)
	rm -f $(DESTDIR)$(pkglibdir)/libcsemdros.a


uninstall-hook:
	rm -f $(DESTDIR)$(pkglibdir)/EmdrosDotNet.dll

macdist: TestEmdros.cs CSharpSources.zip
	cp -fL README-csharp .libs/libcsemdros.dylib TestEmdros.cs EmdrosDotNet.dll $(MACDISTDIR)/development/SWIG/csharp
	cp -fL CSharpSources.zip $(MACDISTDIR)/development/SWIG/csharp



else


MAYBE_BUILT = 

# Not SWIG_WITH_CSHARP_WRAPPERS

# We must have these dummy install-hooks because they are defined
# in the other section

install-exec-hook:

uninstall-hook:

macdist:




endif # end if SWIG_WITH_CSHARP_WRAPPERS

BUILT_SOURCES =  $(MAYBE_BUILT)

EXTRA_DIST = libcsemdros.i \
             libemdf.i libmql.i \
             README-csharp win32.mak \
             TestEmdros.cs.in \
             $(MAYBE_BUILT)
