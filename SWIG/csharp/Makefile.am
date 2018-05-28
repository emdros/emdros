# Include the SWIG_GENERATED_CSHARPFILES macro
include Makefile.inc


MONOCOMPILER = @MONOCOMPILER@

SWIG_PROGRAM = @SWIG_PROGRAM@


SWIG_CSHARP_SRC = \
      libcsemdros_wrap.cxx \
      CSharpSources.zip \
      EmdrosDotNet.dll


TestEmdros.cs: TestEmdros.cs.in
	$(top_builddir)/config.status --file=TestEmdros.cs


gen_files:
	echo "SWIG_GENERATED_CSHARPFILES = \\" > ll.txt 
	$(SWIG_PROGRAM) -namespace Emdros -c++ -csharp libcsemdros.i ; rm -f libcsemdros_wrap.cxx
	ls -l *.cs | grep -v "TableRow.cs" | grep -v "TestEmdros.cs" | awk '{ print $$9; }' | sed -e 's/^/                /' | sed -e 's/$$/ \\/' >> ll.txt
	echo "                TableRow.cs" >> ll.txt
	echo "" >> ll.txt ; echo "" >> ll.txt
#	ls -l *.cs | grep -v "TestEmdros.cs" | awk '{ print $$9 ": libcsemdros_wrap.cxx"; }' >> ll.txt
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



AM_CXXFLAGS = @SUN_CXXFLAGS@ @DARWIN_CXXFLAGS@ @ADDITIONAL_CXXFLAGS@ @DEBUGFLAG@

AM_CPPFLAGS = -I. -I$(top_srcdir)

libcsemdros_wrap.cxx $(SWIG_GENERATED_CSHARPFILES): libcsemdros.i 
	$(SWIG_PROGRAM) -namespace Emdros -c++ -csharp libcsemdros.i

EmdrosDotNet.dll: libcsemdros_wrap.cxx
	$(MONOCOMPILER) -target:library -out:EmdrosDotNet.dll $(SWIG_GENERATED_CSHARPFILES)

CSharpSources.zip: libcsemdros_wrap.cxx
	rm -f CSharpSources.zip
	zip CSharpSources.zip $(SWIG_GENERATED_CSHARPFILES)






if SWIG_WITH_CSHARP_WRAPPERS

MAYBE_BUILT_SRC = $(SWIG_CSHARP_SRC) \
                  TestEmdros.cs 

pkglib_LTLIBRARIES = libcsemdros.la


nodist_libcsemdros_la_SOURCES = libcsemdros_wrap.cxx 

libcsemdros_la_LIBADD = @HARVEST_LDADD_BACKBACK@ \
                        @MQL_LDADD_BACKBACK@ @PCRE2_LDADD_BACKBACK@ \
                        @EMDF_LDADD_BACKBACK@ @ZLIB_LDADD_BACKBACK@ 

libcsemdros_la_LDFLAGS = -version-info \
                         @LIB_EMDROS_CURRENT@:@LIB_EMDROS_REVISION@:@LIB_EMDROS_AGE@ \
                         @DARWIN_LDFLAGS@ @SUN_LDFLAGS@
libcsemdros_la_DEPENDENCIES = @HARVEST_DEPS_BACKBACK@ \
                             @MQL_DEPS_BACKBACK@ @PCRE2_DEPS_BACKBACK@ \
                             @EMDF_DEPS_BACKBACK@ @ZLIB_DEPS_BACKBACK@ 


install-exec-hook: EmdrosDotNet.dll CSharpSources.zip
	cp ./EmdrosDotNet.dll $(DESTDIR)/$(pkglibdir)
	rm -f $(DESTDIR)$(pkglibdir)/libcsemdros.a


uninstall-hook:
	rm -f $(DESTDIR)$(pkglibdir)/EmdrosDotNet.dll

macdist: TestEmdros.cs CSharpSources.zip
	cp -fL README-csharp .libs/libcsemdros.dylib TestEmdros.cs EmdrosDotNet.dll $(MACDISTDIR)/development/SWIG/csharp
	cp -fL CSharpSources.zip $(MACDISTDIR)/development/SWIG/csharp



else


MAYBE_BUILT_SRC = 

# Not SWIG_WITH_CSHARP_WRAPPERS

# We must have these dummy install-hooks because they are defined
# in the other section

install-exec-hook:

uninstall-hook:

macdist:




endif # end if SWIG_WITH_CSHARP_WRAPPERS

BUILT_SOURCES =  $(MAYBE_BUILT_SRC)

EXTRA_DIST = libcsemdros.i \
             README-csharp windows.mak \
             TestEmdros.cs.in \
             $(MAYBE_BUILT)

CLEANFILES = *~ core config.log libcsemdros.la $(BUILT_SOURCES)

DISTCLEANFILES =  $(SWIG_CSHARP_SRC) \
                  TestEmdros.cs CSharpSources.zip \
                  EmdrosDotNet.dll
