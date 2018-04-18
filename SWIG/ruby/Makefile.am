SWIG_PROGRAM = @SWIG_PROGRAM@

SWIG_RUBY_SRC = \
      rbemdros_wrap.cxx


AM_CPPFLAGS = -I$(top_srcdir)/include -I. @RUBY_INCLUDES@

AM_CXXFLAGS = @SUN_CXXFLAGS@ @DARWIN_CXXFLAGS@ @ADDITIONAL_CXXFLAGS@ @DEBUGFLAG@




test.rb: test.rb.in
	$(top_builddir)/config.status --file=test.rb




rbemdros_wrap.cxx: rbemdros.i libemdfruby.i libmqlruby.i 
	cd $(srcdir) ; $(SWIG_PROGRAM) -c++ -ruby rbemdros.i




if SWIG_WITH_RUBY_WRAPPERS

MAYBE_BUILT_SRC = $(SWIG_RUBY_SRC) 


pkglib_LTLIBRARIES = EmdrosRuby.la



nodist_EmdrosRuby_la_SOURCES = rbemdros_wrap.cxx 


EmdrosRuby_la_LIBADD = $(top_builddir)/harvest/libharvest.la \
                       $(top_builddir)/MQL/libmql.la @PCRE_LIBADD_SWIG@ \
                       @EMDFLDADDSWIG@ @ZLIB_LIBADD_SWIG@ 
EmdrosRuby_la_LDFLAGS = @SWIG_EXTRA_LIBS@ -module \
                        -version-info \
                        @LIB_SWIG_CURRENT@:@LIB_SWIG_REVISION@:@LIB_SWIG_AGE@ \
                        @DARWIN_LDFLAGS@ @SUN_LDFLAGS@
EmdrosRuby_la_DEPENDENCIES = $(top_builddir)/harvest/libharvest.la \
                             $(top_builddir)/MQL/libmql.la @PCRE_DEP_SWIG@ \
                             @EMDFDEPSSWIG@ @ZLIB_DEP_SWIG@ 


RUBY_SO_LINK_FILENAME = @RUBY_SO_DIR@/EmdrosRuby.so


install-exec-hook:
	rm -f $(DESTDIR)$(pkglibdir)/EmdrosRuby.a


install-ruby-lib:
	if test -h $(RUBY_SO_LINK_FILENAME) -o -f $(RUBY_SO_LINK_FILENAME); then rm -f $(RUBY_SO_LINK_FILENAME); fi
	ln -s $(pkglibdir)/EmdrosRuby.so $(RUBY_SO_LINK_FILENAME)

uninstall-ruby-lib:
	if test -h $(RUBY_SO_LINK_FILENAME) -o -f $(RUBY_SO_LINK_FILENAME); then rm -f $(RUBY_SO_LINK_FILENAME); fi

macdist:
	cp -fL README-Ruby .libs/EmdrosRuby.so test.rb $(MACDISTDIR)/development/SWIG/ruby/


else
# Not SWIG_WITH_RUBY_WRAPPERS

MAYBE_BUILT_SRC = 

install-exec-hook:

macdist:


endif # end if SWIG_WITH_RUBY_WRAPPERS


BUILT_SOURCES = $(MAYBE_BUILT_SRC) test.rb




EXTRA_DIST = test.rb.in rbemdros.i \
             libemdfruby.i libmqlruby.i \
             README-Ruby dir.sh $(MAYBE_BUILT_SRC)

CLEANFILES = *~ core test.rb config.log EmdrosRuby.la

DISTCLEANFILES = $(SWIG_RUBY_SRC) rbemdros_wrap.cxx 
