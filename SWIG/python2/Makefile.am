SWIG_PROGRAM = @SWIG_PROGRAM@

SWIG_PY_SRC = \
      pyemdros_wrap.cxx \
      EmdrosPy.py



AM_CXXFLAGS = @SUN_CXXFLAGS@ @DARWIN_CXXFLAGS@ @ADDITIONAL_CXXFLAGS@ @DEBUGFLAG@

AM_CPPFLAGS = -I$(top_srcdir)/include -I. @PYTHON_INCLUDES@



pyemdros_wrap.cxx: pyemdros.i libemdfpython.i libmqlpython.i 
	$(SWIG_PROGRAM) -c++ -python pyemdros.i

EmdrosPy.py: pyemdros_wrap.cxx



if SWIG_WITH_PYTHON_WRAPPERS

MAYBE_BUILT_SRC = $(SWIG_PY_SRC) EmdrosPy.pyc EmdrosPy.pyo

EmdrosPy.pyc EmdrosPy.pyo: EmdrosPy.py
	export PYTHON=$(PYTHON) ; $(SHELL) $(top_builddir)/py-compile EmdrosPy.py

pkglib_LTLIBRARIES = _EmdrosPy.la

nodist__EmdrosPy_la_SOURCES = pyemdros_wrap.cxx 


_EmdrosPy_la_LIBADD = $(top_builddir)/harvest/libharvest.la \
                      $(top_builddir)/MQL/libmql.la @PCRE_LIBADD_SWIG@ \
                      @EMDFLDADDSWIG@ @ZLIB_LIBADD_SWIG@ 
_EmdrosPy_la_LDFLAGS = @SWIG_EXTRA_LIBS@ -module \
                       -version-info \
                       @LIB_SWIG_CURRENT@:@LIB_SWIG_REVISION@:@LIB_SWIG_AGE@ \
                       @DARWIN_LDFLAGS@ @SUN_LDFLAGS@
_EmdrosPy_la_DEPENDENCIES = $(top_builddir)/harvest/libharvest.la \
                            $(top_builddir)/MQL/libmql.la @PCRE_DEP_SWIG@ \
                            @EMDFDEPSSWIG@ @ZLIB_DEP_SWIG@ 



PYTHON_SO_LINKNAME = @PYTHON_SITE_PACKAGES_DIR@/_EmdrosPy.so 

PYTHON_FILENAMES = @PYTHON_SITE_PACKAGES_DIR@/EmdrosPy.py \
                   @PYTHON_SITE_PACKAGES_DIR@/EmdrosPy.pyc \
                   @PYTHON_SITE_PACKAGES_DIR@/EmdrosPy.pyo

install-exec-hook:
	cp EmdrosPy.py EmdrosPy.pyc EmdrosPy.pyo $(DESTDIR)$(pkglibdir)
	rm -f $(DESTDIR)$(pkglibdir)/_EmdrosPy.a

uninstall-hook:
	rm -f $(DESTDIR)$(pkglibdir)/EmdrosPy.py $(DESTDIR)$(pkglibdir)/EmdrosPy.pyc $(DESTDIR)$(pkglibdir)/EmdrosPy.pyo
	rm -f $(DESTDIR)$(pkglibdir)/_EmdrosPy.so* 


install-python-lib: EmdrosPy.pyc
	if test -h $(PYTHON_SO_LINKNAME) -o -f $(PYTHON_SO_LINKNAME); then rm -f $(PYTHON_SO_LINKNAME); fi
	rm -f $(PYTHON_FILENAMES)
	ln -s $(pkglibdir)/_EmdrosPy.so $(PYTHON_SO_LINKNAME)
	cp EmdrosPy.py EmdrosPy.pyc EmdrosPy.pyo @PYTHON_SITE_PACKAGES_DIR@/

uninstall-python-lib:
	if test -h $(PYTHON_SO_LINKNAME) -o -f $(PYTHON_SO_LINKNAME); then rm -f $(PYTHON_SO_LINKNAME); fi; 
	rm -f $(PYTHON_FILENAMES)


macdist:
	cp -fL README-Python EmdrosPy.py .libs/_EmdrosPy.so test.py $(MACDISTDIR)/development/SWIG/python/


else
# Not SWIG_WITH_PYTHON_WRAPPERS

MAYBE_BUILT_SRC = 

pkglib_LTLIBRARIES =

install-exec-hook:

uninstall-hook:

macdist:


endif # end if SWIG_WITH_PYTHON_WRAPPERS

BUILT_SOURCES = $(MAYBE_BUILT_SRC) 




EXTRA_DIST = pyemdros.i \
             libemdfpython.i libmqlpython.i \
             test.py README-Python dir.py win32.mak  \
             $(BUILT_SOURCES)

CLEANFILES = *~ core config.log test.pyc test.pyo \
             EmdrosPy.pyc EmdrosPy.pyo _EmdrosPy.la $(BUILT_SOURCES)

DISTCLEANFILES = $(BUILT_SOURCES)
