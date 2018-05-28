SWIG_PROGRAM = @SWIG_PROGRAM@




AM_CXXFLAGS = @SUN_CXXFLAGS@ @DARWIN_CXXFLAGS@ @ADDITIONAL_CXXFLAGS@ @DEBUGFLAG@

AM_CPPFLAGS = -I$(top_srcdir)/include -I. @PHP_INCLUDES@



phpemdros_wrap.cpp: phpemdros.i libemdfphp.i libmqlphp.i 
	$(SWIG_PROGRAM) -c++ -php phpemdros.i

EmdrosPHP.php: phpemdros_wrap.cpp

php_EmdrosPHP.h: phpemdros_wrap.cpp

phpemdros_wrap.h: phpemdros_wrap.cpp

EmdrosPHP.ini: Makefile EmdrosPHP.ini.in
	sed -e 's:@pkglibdir@:$(pkglibdir):g' EmdrosPHP.ini.in > $@


if SWIG_WITH_PHP5_WRAPPERS

SWIG_PHP_SRC = \
      phpemdros_wrap.cpp \
      phpemdros_wrap.h \
      php_EmdrosPHP.h \
      EmdrosPHP.php


MAYBE_BUILT_SRC = $(SWIG_PHP_SRC) 

pkglib_LTLIBRARIES = EmdrosPHP.la

EmdrosPHP_la_SOURCES = phpemdros_wrap.cpp 


EmdrosPHP_la_LIBADD = $(top_builddir)/harvest/libharvest.la \
                      $(top_builddir)/MQL/libmql.la @PCRE_LIBADD_SWIG@ \
                      @EMDFLDADDSWIG@ @ZLIB_LIBADD_SWIG@ 
EmdrosPHP_la_LDFLAGS = @SWIG_EXTRA_LIBS@ -module \
                       -version-info \
                       @LIB_SWIG_CURRENT@:@LIB_SWIG_REVISION@:@LIB_SWIG_AGE@ \
                       @DARWIN_LDFLAGS@ @SUN_LDFLAGS@
EmdrosPHP_la_DEPENDENCIES = $(top_builddir)/harvest/libharvest.la \
                            $(top_builddir)/MQL/libmql.la @PCRE_DEP_SWIG@ \
                            @EMDFDEPSSWIG@ @ZLIB_DEP_SWIG@ 



install-exec-hook:
	cp EmdrosPHP.php $(DESTDIR)$(pkglibdir)
	rm -f $(DESTDIR)$(pkglibdir)/EmdrosPHP.a

uninstall-hook:
	rm -f $(DESTDIR)$(pkglibdir)/EmdrosPHP.php 
	rm -f $(DESTDIR)$(pkglibdir)/EmdrosPHP.so* 


macdist:
	cp -fL README-PHP EmdrosPHP.php .libs/EmdrosPHP.so $(MACDISTDIR)/development/SWIG/php5/


else
# Not SWIG_WITH_PHP5_WRAPPERS

SWIG_PHP_SRC =

MAYBE_BUILT_SRC = 

pkglib_LTLIBRARIES =

install-exec-hook:

uninstall-hook:

macdist:


endif # end if SWIG_WITH_PHP5_WRAPPERS

BUILT_SOURCES = $(MAYBE_BUILT_SRC) EmdrosPHP.ini

EXTRA_DIST = phpemdros.i \
             libemdfphp.i libmqlphp.i \
             EmdrosPHP.ini.in \
             test.php \
             README-PHP \
             $(MAYBE_BUILT_SRC)

CLEANFILES = *~ core config.log $(BUILT_SOURCES)

DISTCLEANFILES = $(SWIG_PHP_SRC) phpemdros_wrap.cpp php_EmdrosPHP.h EmdrosPHP.php EmdrosPHP.ini phpemdros_wrap.h
