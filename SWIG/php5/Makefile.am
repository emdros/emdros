SWIG_PROGRAM = @SWIG_PROGRAM@

SWIG_PHP5_SRC = \
      phpemdros_wrap.cpp \
      phpemdros_wrap.h \
      php_EmdrosPHP.h \
      EmdrosPHP.php



AM_CXXFLAGS = @SUN_CXXFLAGS@ @DARWIN_CXXFLAGS@ @ADDITIONAL_CXXFLAGS@ @DEBUGFLAG@

AM_CPPFLAGS = -I. -I$(top_srcdir) @PHP5_INCLUDES@  
phpemdros_wrap.cpp: phpemdros.i libemdfphp.i libmqlphp.i 
	$(SWIG_PROGRAM) -c++ -php5 phpemdros.i

EmdrosPHP.php: phpemdros_wrap.cpp

php_EmdrosPHP.h: phpemdros_wrap.cpp

phpemdros_wrap.h: phpemdros_wrap.cpp

EmdrosPHP.ini: Makefile EmdrosPHP.ini.in
	sed -e 's:@pkglibdir@:$(pkglibdir):g' EmdrosPHP.ini.in > $@


if SWIG_WITH_PHP5_WRAPPERS



MAYBE_BUILT_SRC = $(SWIG_PHP5_SRC) 

pkglib_LTLIBRARIES = EmdrosPHP.la

nodist_EmdrosPHP_la_SOURCES = phpemdros_wrap.cpp 


EmdrosPHP_la_LIBADD = @EMDROS_LDADD_BACKBACK@
EmdrosPHP_la_LDFLAGS = -module \
                       -version-info \
                       @LIB_EMDROS_CURRENT@:@LIB_EMDROS_REVISION@:@LIB_EMDROS_AGE@ \
                       @DARWIN_LDFLAGS@ @SUN_LDFLAGS@
EmdrosPHP_la_DEPENDENCIES = @EMDROS_DEPS_BACKBACK@



install-exec-hook:
	cp EmdrosPHP.php EmdrosPHP.ini $(DESTDIR)$(pkglibdir)
	rm -f $(DESTDIR)$(pkglibdir)/EmdrosPHP.a

uninstall-hook:
	rm -f $(DESTDIR)$(pkglibdir)/EmdrosPHP.php 
	rm -f $(DESTDIR)$(pkglibdir)/EmdrosPHP.so* 


macdist:
	cp -fL README-PHP5 EmdrosPHP.php .libs/EmdrosPHP.so $(MACDISTDIR)/development/SWIG/php5/


else
# Not SWIG_WITH_PHP5_WRAPPERS

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
             test7.php \
             README-PHP5 \
             $(MAYBE_BUILT_SRC)

CLEANFILES = *~ core config.log 

DISTCLEANFILES = $(SWIG_PHP5_SRC) \
                 phpemdros_wrap.cpp phpemdros_wrap.h \
                 php_EmdrosPHP.h EmdrosPHP.php EmdrosPHP.ini 

