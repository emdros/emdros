SWIG_PROGRAM = @SWIG_PROGRAM@

SWIG_PHP7_SRC = \
      php7emdros_wrap.cxx \
      php7emdros_wrap.h \
      php_EmdrosPHP7.h \
      EmdrosPHP7.php



AM_CXXFLAGS = @SUN_CXXFLAGS@ @DARWIN_CXXFLAGS@ @ADDITIONAL_CXXFLAGS@ @DEBUGFLAG@

AM_CPPFLAGS = -I. -I$(top_srcdir) @PHP7_INCLUDES@ 
php7emdros_wrap.cxx: php7emdros.i libemdfphp.i libmqlphp.i 
	$(SWIG_PROGRAM) -c++ -php7 php7emdros.i

EmdrosPHP7.php: php7emdros_wrap.cxx

php_EmdrosPHP7.h: php7emdros_wrap.cxx

php7emdros_wrap.h: php7emdros_wrap.cxx

EmdrosPHP7.ini: Makefile EmdrosPHP7.ini.in
	sed -e 's:@pkglibdir@:$(pkglibdir):g' EmdrosPHP7.ini.in > $@


if SWIG_WITH_PHP7_WRAPPERS



MAYBE_BUILT_SRC = $(SWIG_PHP7_SRC) 

pkglib_LTLIBRARIES = EmdrosPHP7.la

nodist_EmdrosPHP7_la_SOURCES = php7emdros_wrap.cxx 


EmdrosPHP7_la_LIBADD = @EMDROS_LDADD_BACKBACK@
EmdrosPHP7_la_LDFLAGS = -module \
                       -version-info \
                       @LIB_EMDROS_CURRENT@:@LIB_EMDROS_REVISION@:@LIB_EMDROS_AGE@ \
                       @DARWIN_LDFLAGS@ @SUN_LDFLAGS@
EmdrosPHP7_la_DEPENDENCIES = @EMDROS_DEPS_BACKBACK@



install-exec-hook:
	cp EmdrosPHP7.php EmdrosPHP7.ini $(DESTDIR)$(pkglibdir)
	rm -f $(DESTDIR)$(pkglibdir)/EmdrosPHP7.a

uninstall-hook:
	rm -f $(DESTDIR)$(pkglibdir)/EmdrosPHP7.php 
	rm -f $(DESTDIR)$(pkglibdir)/EmdrosPHP7.so* 


macdist:
	cp -fL README-PHP7 EmdrosPHP7.php .libs/EmdrosPHP7.so $(MACDISTDIR)/development/SWIG/php7/


else
# Not SWIG_WITH_PHP7_WRAPPERS

MAYBE_BUILT_SRC = 

pkglib_LTLIBRARIES =

install-exec-hook:

uninstall-hook:

macdist:


endif # end if SWIG_WITH_PHP7_WRAPPERS

BUILT_SOURCES = $(MAYBE_BUILT_SRC) EmdrosPHP7.ini


EXTRA_DIST = php7emdros.i \
             libemdfphp.i libmqlphp.i \
             EmdrosPHP7.ini.in \
             test7.php \
             README-PHP7 \
             $(MAYBE_BUILT_SRC)

CLEANFILES = *~ core config.log 

DISTCLEANFILES = $(SWIG_PHP7_SRC) \
                 php7emdros_wrap.cxx php7emdros_wrap.h \
                 php_EmdrosPHP7.h EmdrosPHP7.php EmdrosPHP7.ini 

