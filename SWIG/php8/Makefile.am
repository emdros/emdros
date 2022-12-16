SWIG_PROGRAM = @SWIG_PROGRAM@

SWIG_PHP8_SRC = \
      php8emdros_wrap.cxx \
      php8emdros_wrap.h \
      php_EmdrosPHP8.h 




AM_CXXFLAGS = @SUN_CXXFLAGS@ @DARWIN_CXXFLAGS@ @ADDITIONAL_CXXFLAGS@ @DEBUGFLAG@

AM_CPPFLAGS = -I. -I$(top_srcdir) @PHP8_INCLUDES@

# swig 4.1.0 introduced PHP8 support. The -php7 switch is how to
# invoke PHP7 or PHP8 support. Could also use -php.
php8emdros_wrap.cxx: php8emdros.i libemdfphp.i libmqlphp.i 
	$(SWIG_PROGRAM) -c++ -php7 php8emdros.i

php_EmdrosPHP8.h: php8emdros_wrap.cxx

php8emdros_wrap.h: php8emdros_wrap.cxx

EmdrosPHP8.ini: Makefile EmdrosPHP8.ini.in
	sed -e 's:@pkglibdir@:$(pkglibdir):g' EmdrosPHP8.ini.in > $@


if CAN_DO_SWIG
if CAN_SWIG_DO_PHP8
MAYBE_BUILT_SRC = $(SWIG_PHP8_SRC)
endif
endif


if SWIG_WITH_PHP8_WRAPPERS

pkglib_LTLIBRARIES = EmdrosPHP8.la

nodist_EmdrosPHP8_la_SOURCES = php8emdros_wrap.cxx 


EmdrosPHP8_la_LIBADD = @EMDROS_LDADD_BACKBACK@
EmdrosPHP8_la_LDFLAGS = -module \
                       -version-info \
                       @LIB_EMDROS_CURRENT@:@LIB_EMDROS_REVISION@:@LIB_EMDROS_AGE@ \
                       @DARWIN_LDFLAGS@ @SUN_LDFLAGS@
EmdrosPHP8_la_DEPENDENCIES = @EMDROS_DEPS_BACKBACK@



install-exec-hook:
	cp EmdrosPHP8.ini $(DESTDIR)$(pkglibdir)
	rm -f $(DESTDIR)$(pkglibdir)/EmdrosPHP8.a

uninstall-hook:
	rm -f $(DESTDIR)$(pkglibdir)/EmdrosPHP8.so* 


macdist:
	cp -fL README-PHP8 .libs/EmdrosPHP8.so $(MACDISTDIR)/development/SWIG/php8/


else
# Not SWIG_WITH_PHP8_WRAPPERS

pkglib_LTLIBRARIES =

install-exec-hook:

uninstall-hook:

macdist:


endif # end if SWIG_WITH_PHP8_WRAPPERS

BUILT_SOURCES = $(MAYBE_BUILT_SRC) EmdrosPHP8.ini


EXTRA_DIST = php8emdros.i \
             libemdfphp.i libmqlphp.i \
             EmdrosPHP8.ini.in \
             test8.php \
             README-PHP8 \
             $(MAYBE_BUILT_SRC)

CLEANFILES = *~ core config.log 

DISTCLEANFILES = $(SWIG_PHP8_SRC) \
                 php8emdros_wrap.cxx php8emdros_wrap.h \
                 php_EmdrosPHP8.h EmdrosPHP8.ini 

