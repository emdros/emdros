SWIG_PROGRAM = @SWIG_PROGRAM@

SWIG_PY3_SRC = \
      py3emdros_wrap.cxx \
      EmdrosPy3.py



AM_CXXFLAGS = @SUN_CXXFLAGS@ @DARWIN_CXXFLAGS@ @ADDITIONAL_CXXFLAGS@ @DEBUGFLAG@

AM_CPPFLAGS = -I. -I$(top_srcdir) @PYTHON3_INCLUDES@



EmdrosPy3.py py3emdros_wrap.cxx: py3emdros.i 
	$(SWIG_PROGRAM) -c++ -python py3emdros.i



if SWIG_WITH_PYTHON3_WRAPPERS

MAYBE_BUILT_SRC = $(SWIG_PY3_SRC) 

pkglib_LTLIBRARIES = _EmdrosPy3.la

nodist__EmdrosPy3_la_SOURCES = py3emdros_wrap.cxx 


_EmdrosPy3_la_LIBADD = @EMDROS_LDADD_BACKBACK@ 
_EmdrosPy3_la_LDFLAGS = -module \
                       -version-info \
                       @LIB_EMDROS_CURRENT@:@LIB_EMDROS_REVISION@:@LIB_EMDROS_AGE@ \
                       @DARWIN_LDFLAGS@ @SUN_LDFLAGS@
_EmdrosPy3_la_DEPENDENCIES = @EMDROS_DEPS_BACKBACK@



PYTHON3_SO_LINKNAME = @PYTHON_SITE_PACKAGES_DIR@/_EmdrosPy3.so 

PYTHON3_FILENAMES = @PYTHON_SITE_PACKAGES_DIR@/EmdrosPy3.py 

install-exec-hook:
	cp EmdrosPy3.py $(DESTDIR)$(pkglibdir)
	rm -f $(DESTDIR)$(pkglibdir)/_EmdrosPy3.a

uninstall-hook:
	rm -f $(DESTDIR)$(pkglibdir)/EmdrosPy3.py
	rm -f $(DESTDIR)$(pkglibdir)/_EmdrosPy3.so* 


macdist:
	cp -fL README-Python3 EmdrosPy3.py .libs/_EmdrosPy3.so test3.py $(MACDISTDIR)/development/SWIG/python3/


else
# Not SWIG_WITH_PYTHON3_WRAPPERS

MAYBE_BUILT_SRC = 

pkglib_LTLIBRARIES =

install-exec-hook:

uninstall-hook:

macdist:


endif # end if SWIG_WITH_PYTHON3_WRAPPERS



BUILT_SOURCES = $(MAYBE_BUILT_SRC)



EXTRA_DIST = py3emdros.i \
             README-Python3 dir.py test3.py \
             $(BUILT_SOURCES)

CLEANFILES = *~ core config.log test3.pyc test3.pyo \
             EmdrosPy3.pyc EmdrosPy3.pyo _EmdrosPy3.la 

DISTCLEANFILES = $(SWIG_PY3_SRC)
