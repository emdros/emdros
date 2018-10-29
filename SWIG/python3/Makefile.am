SWIG_PROGRAM = @SWIG_PROGRAM@

SWIG_PY3_SRC = \
      py3emdros_wrap.cxx \
      EmdrosPy3.py


AM_CXXFLAGS = @SUN_CXXFLAGS@ @DARWIN_CXXFLAGS@ @ADDITIONAL_CXXFLAGS@ @DEBUGFLAG@

AM_CPPFLAGS = -I$(top_srcdir)/include -I. @PYTHON3_INCLUDES@



EmdrosPy3.py py3emdros_wrap.cxx: py3emdros.i libemdfpython.i libmqlpython.i 
	cd $(srcdir) ; $(SWIG_PROGRAM) -c++ -python py3emdros.i



if SWIG_WITH_PYTHON3_WRAPPERS

MAYBE_BUILT_SRC = $(SWIG_PY3_SRC) EmdrosPy3.pyc

EmdrosPy3.pyc: EmdrosPy3.py
	$(PYTHON3) -m compileall -f -b EmdrosPy3.py


pkglib_LTLIBRARIES = _EmdrosPy3.la

nodist__EmdrosPy3_la_SOURCES = py3emdros_wrap.cxx 


_EmdrosPy3_la_LIBADD = $(top_builddir)/harvest/libharvest.la \
                      $(top_builddir)/MQL/libmql.la @PCRE_LIBADD_SWIG@ \
                      @EMDFLDADDSWIG@ @ZLIB_LIBADD_SWIG@ 
_EmdrosPy3_la_LDFLAGS = @SWIG_EXTRA_LIBS@ -module \
                       -version-info \
                       @LIB_SWIG_CURRENT@:@LIB_SWIG_REVISION@:@LIB_SWIG_AGE@ \
                       @DARWIN_LDFLAGS@ @SUN_LDFLAGS@
_EmdrosPy3_la_DEPENDENCIES = $(top_builddir)/harvest/libharvest.la \
                            $(top_builddir)/MQL/libmql.la @PCRE_DEP_SWIG@ \
                            @EMDFDEPSSWIG@ @ZLIB_DEP_SWIG@ 



PYTHON3_SO_LINKNAME = @PYTHON_SITE_PACKAGES_DIR@/_EmdrosPy3.so 

PYTHON3_FILENAMES = @PYTHON_SITE_PACKAGES_DIR@/EmdrosPy3.py \
                   @PYTHON_SITE_PACKAGES_DIR@/EmdrosPy3.pyc 


install-exec-hook:
	cp EmdrosPy3.py EmdrosPy3.pyc $(DESTDIR)$(pkglibdir)
	rm -f $(DESTDIR)$(pkglibdir)/_EmdrosPy3.a

uninstall-hook:
	rm -f $(DESTDIR)$(pkglibdir)/EmdrosPy3.py
	rm -f $(DESTDIR)$(pkglibdir)/EmdrosPy3.pyc
	rm -f $(DESTDIR)$(pkglibdir)/_EmdrosPy3.so* 

install-python-lib: EmdrosPy.pyc
	if test -h $(PYTHON3_SO_LINKNAME) -o -f $(PYTHON3_SO_LINKNAME); then rm -f $(PYTHON3_SO_LINKNAME); fi
	rm -f $(PYTHON3_FILENAMES)
	ln -s $(pkglibdir)/_EmdrosPy.so $(PYTHON3_SO_LINKNAME)
	cp EmdrosPy3.py EmdrosPy3.pyc @PYTHON_SITE_PACKAGES_DIR@/

uninstall-python-lib:
	if test -h $(PYTHON3_SO_LINKNAME) -o -f $(PYTHON3_SO_LINKNAME); then rm -f $(PYTHON3_SO_LINKNAME); fi; 
	rm -f $(PYTHON3_FILENAMES)



macdist:
	cp -fL README-Python3 EmdrosPy3.py .libs/_EmdrosPy3.so test3.py $(MACDISTDIR)/development/SWIG/python3/


else
# Not SWIG_WITH_PYTHON_WRAPPERS

MAYBE_BUILT_SRC = 

pkglib_LTLIBRARIES =

install-exec-hook:

uninstall-hook:

macdist:


endif # end if SWIG_WITH_PYTHON_WRAPPERS

BUILT_SOURCES = $(MAYBE_BUILT_SRC) 




EXTRA_DIST = py3emdros.i \
             libemdfpython.i libmqlpython.i \
             test3.py README-Python3 dir.py \
             $(MAYBE_BUILT_SRC)

CLEANFILES = *~ core config.log test3.pyc test3.pyo \
             EmdrosPy3.pyc EmdrosPy3.pyo _EmdrosPy3.la \
             $(BUILT_SOURCES)


DISTCLEANFILES = $(MAYBE_BUILT_SRC)
