SWIG_PROGRAM = @SWIG_PROGRAM@

SWIG_PERL_SRC = \
      plemdros_wrap.cxx \
      EmdrosPerl.pm




AM_CXXFLAGS = @SUN_CXXFLAGS@ @DARWIN_CXXFLAGS@ @ADDITIONAL_CXXFLAGS@ @DEBUGFLAG@

AM_CPPFLAGS = -I$(top_srcdir)/include -I. @PERL_INCLUDES@



plemdros_wrap.cxx: plemdros.i libemdfperl.i libmqlperl.i 
	cd $(srcdir) ; $(SWIG_PROGRAM) -c++ -perl -shadow plemdros.i

EmdrosPerl.pm: plemdros_wrap.cxx



test.pl: test.pl.in
	$(top_builddir)/config.status --file=test.pl




if SWIG_WITH_PERL_WRAPPERS

MAYBE_BUILT_SRC = $(SWIG_PERL_SRC)

Makefile.PL: Makefile.PL.in
	$(top_builddir)/config.status --file=Makefile.PL

Makefile.swig: Makefile.PL
	cd $(srcdir) ; $(PERL) Makefile.PL



pkglib_LTLIBRARIES = EmdrosPerl.la

nodist_EmdrosPerl_la_SOURCES = plemdros_wrap.cxx 

EmdrosPerl_la_LIBADD = $(top_builddir)/harvest/libharvest.la \
                       $(top_builddir)/MQL/libmql.la @PCRE_LIBADD_SWIG@ \
                       @EMDFLDADDSWIG@ @ZLIB_LIBADD_SWIG@ 
EmdrosPerl_la_LDFLAGS = @SWIG_EXTRA_LIBS@ -module \
                        -version-info \
                        @LIB_SWIG_CURRENT@:@LIB_SWIG_REVISION@:@LIB_SWIG_AGE@ \
                        @DARWIN_LDFLAGS@ @SUN_LDFLAGS@
EmdrosPerl_la_DEPENDENCIES = $(top_builddir)/harvest/libharvest.la \
                             $(top_builddir)/MQL/libmql.la @PCRE_DEP_SWIG@ @ZLIB_DEP_SWIG@ \
                             @EMDFDEPSSWIG@ 


install-exec-hook: EmdrosPerl.pm
	cp EmdrosPerl.pm $(DESTDIR)$(pkglibdir)
	rm -f $(DESTDIR)$(pkglibdir)/EmdrosPerl.a

uninstall-hook:
	rm -f $(DESTDIR)$(pkglibdir)/EmdrosPerl.pm


macdist:
	cp -fL README-Perl EmdrosPerl.pm .libs/EmdrosPerl.so test.pl $(MACDISTDIR)/development/SWIG/perl/



else
# Not SWIG_WITH_PERL_WRAPPERS

MAYBE_BUILT_SRC = 

pkglib_LTLIBRARIES =

install-exec-hook:

uninstall-hook:

macdist:


endif # end if SWIG_WITH_PERL_WRAPPERS

clean-hook:
	rm -rf blib/

BUILT_SOURCES = $(MAYBE_BUILT_SRC) test.pl 

EXTRA_DIST = plemdros.i libemdfperl.i libmqlperl.i \
             test.pl.in Makefile.PL.in README-Perl $(MAYBE_BUILT_SRC)

CLEANFILES = *~ core config.log EmdrosPerl.la \
             Makefile.swig Makefile.PL test.pl \
             $(BUILT_SOURCES)

DISTCLEANFILES = $(MAYBE_BUILT_SRC) 
