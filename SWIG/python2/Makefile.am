SWIG_PROGRAM = @SWIG_PROGRAM@

SWIG_PY_SRC = \
      pyemdros_wrap.cxx \
      EmdrosPy.py



AM_CXXFLAGS = @SUN_CXXFLAGS@ @DARWIN_CXXFLAGS@ @ADDITIONAL_CXXFLAGS@ @DEBUGFLAG@

AM_CPPFLAGS = -I. @PYTHON2_INCLUDES@ -I$(top_srcdir)/lib/base -I. -I$(top_srcdir)/lib/base -I$(top_srcdir)/lib/EMdF -I$(top_srcdir)/lib/MQL -I$(top_srcdir)/lib/harvest  -I$(top_srcdir)/amalgamation  -I$(top_srcdir)



pyemdros_wrap.cxx: pyemdros.i
	$(SWIG_PROGRAM) -c++ -python pyemdros.i

EmdrosPy.py: pyemdros_wrap.cxx


if CAN_DO_SWIG
MAYBE_BUILT_SRC = $(SWIG_PY_SRC)
endif


if SWIG_WITH_PYTHON2_WRAPPERS

#EmdrosPy.pyc: EmdrosPy.py
#	export PYTHON=$(PYTHON) ; $(SHELL) $(top_builddir)/py-compile EmdrosPy.py

pkglib_LTLIBRARIES = _EmdrosPy.la

nodist__EmdrosPy_la_SOURCES = pyemdros_wrap.cxx 


_EmdrosPy_la_LIBADD = @EMDROS_LDADD_BACKBACK@
_EmdrosPy_la_LDFLAGS = -module \
                       -version-info \
                       @LIB_EMDROS_CURRENT@:@LIB_EMDROS_REVISION@:@LIB_EMDROS_AGE@ \
                       @DARWIN_LDFLAGS@ @SUN_LDFLAGS@
_EmdrosPy_la_DEPENDENCIES = @EMDROS_DEPS_BACKBACK@



PYTHON2_SO_LINKNAME = @PYTHON2_SITE_PACKAGES_DIR@/_EmdrosPy.so 

PYTHON2_FILENAMES = @PYTHON2_SITE_PACKAGES_DIR@/EmdrosPy.py 
install-exec-hook:
	cp EmdrosPy.py $(DESTDIR)$(pkglibdir)
	rm -f $(DESTDIR)$(pkglibdir)/_EmdrosPy.a

uninstall-hook:
	rm -f $(DESTDIR)$(pkglibdir)/EmdrosPy.py 
	rm -f $(DESTDIR)$(pkglibdir)/_EmdrosPy.so* 


install-python-lib: 
	if test -h $(PYTHON2_SO_LINKNAME) -o -f $(PYTHON2_SO_LINKNAME); then rm -f $(PYTHON2_SO_LINKNAME); fi
	rm -f $(PYTHON2_FILENAMES)
	ln -s $(pkglibdir)/_EmdrosPy.so $(PYTHON2_SO_LINKNAME)
	cp EmdrosPy.py @PYTHON2_SITE_PACKAGES_DIR@/

uninstall-python-lib:
	if test -h $(PYTHON2_SO_LINKNAME) -o -f $(PYTHON2_SO_LINKNAME); then rm -f $(PYTHON2_SO_LINKNAME); fi; 
	rm -f $(PYTHON2_FILENAMES)


macdist:
	cp -fL README-Python2 EmdrosPy.py .libs/_EmdrosPy.so test.py $(MACDISTDIR)/development/SWIG/python/


else
# Not SWIG_WITH_PYTHON2_WRAPPERS

pkglib_LTLIBRARIES =

install-exec-hook:

uninstall-hook:

macdist:


endif # end if SWIG_WITH_PYTHON2_WRAPPERS

BUILT_SOURCES = $(MAYBE_BUILT_SRC)



EXTRA_DIST = pyemdros.i \
             test.py README-Python2 dir.py windows.mak  \
             $(BUILT_SOURCES)

CLEANFILES = *~ core config.log test.pyc test.pyo \
            _EmdrosPy.la \
             $(BUILT_SOURCES)

DISTCLEANFILES = $(SWIG_PY_SRC) 
