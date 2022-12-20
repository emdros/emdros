SWIG_PROGRAM = @SWIG_PROGRAM@

SWIG_NODEJS_SRC = \
      nodejs_emdros_wrap.cxx \
      nodejs_emdros_wrap.h \
      emdros_amalgamation_1_emdros.cpp \
      emdros_c_amalgamation_1.c \
      emdros_c_amalgamation_2.c \
      emdros-lconfig.h \
      emdros_c.h \
      emdros-config.h \
      emdros.h



AM_CXXFLAGS = @SUN_CXXFLAGS@ @DARWIN_CXXFLAGS@ @ADDITIONAL_CXXFLAGS@ @DEBUGFLAG@

NODE_CXXFLAGS = @CXXFLAGS@ @SUN_CXXFLAGS@ @DARWIN_CXXFLAGS@ @ADDITIONAL_CXXFLAGS@

NODE_CFLAGS = @CFLAGS@ @ADDITIONAL_CFLAGS@


AM_CPPFLAGS = -I. -I$(top_srcdir) @NODEJS_INCLUDES@

nodejs_emdros_wrap.cxx: nodejs_emdros.i libemdf_nodejs.i libmql_nodejs.i 
	$(SWIG_PROGRAM) -c++ -javascript -node nodejs_emdros.i

nodejs_emdros_wrap.h: nodejs_emdros_wrap.cxx

binding.gyp: Makefile binding.gyp.in
	sed -e 's:%NODE_CXXFLAGS%:$(NODE_CXXFLAGS):g;s:%NODE_CFLAGS%:$(NODE_CFLAGS):g;s:%BACKEND_LIBS_AMALGAMATION%:@BACKEND_LIBS_AMALGAMATION@:g' binding.gyp.in > $@

emdros_amalgamation_1_emdros.cpp: ../../amalgamation/emdros_amalgamation_1_emdros.cpp
	cp ../../amalgamation/emdros_amalgamation_1_emdros.cpp $@

emdros_c_amalgamation_1.c: ../../amalgamation/emdros_c_amalgamation_1.c
	cp ../../amalgamation/emdros_c_amalgamation_1.c $@

emdros_c_amalgamation_2.c: ../../amalgamation/emdros_c_amalgamation_2.c
	cp ../../amalgamation/emdros_c_amalgamation_2.c $@

emdros-lconfig.h: ../../amalgamation/emdros-lconfig.h
	cp ../../amalgamation/emdros-lconfig.h $@

emdros_c.h: ../../amalgamation/emdros_c.h
	cp ../../amalgamation/emdros_c.h $@

emdros-config.h: ../../amalgamation/emdros-config.h
	cp ../../amalgamation/emdros-config.h $@

emdros.h: ../../amalgamation/emdros.h
	cp ../../amalgamation/emdros.h $@





if CAN_DO_SWIG
MAYBE_BUILT_SRC = $(SWIG_NODEJS_SRC) binding.gyp
endif


if SWIG_WITH_NODEJS_WRAPPERS

#pkglib_LTLIBRARIES = EmdrosNode.la

#nodist_EmdrosNode_la_SOURCES = nodejs_emdros_wrap.cxx

EmdrosNode_la_LIBADD = @EMDROS_LDADD_BACKBACK@
EmdrosNode_la_LDFLAGS = -module \
                       -version-info \
                       @LIB_EMDROS_CURRENT@:@LIB_EMDROS_REVISION@:@LIB_EMDROS_AGE@ \
                       @DARWIN_LDFLAGS@ @SUN_LDFLAGS@
EmdrosNode_la_DEPENDENCIES = @EMDROS_DEPS_BACKBACK@



#if [ ! -d $(DESTDIR)/$(Nodejs_INI_DIR) ] ; then mkdir -p $(DESTDIR)/$(Nodejs_INI_DIR) ; fi
#ln -s $(DESTDIR)$(pkglibdir)/EmdrosNode.ini $(DESTDIR)/$(Nodejs_INI_DIR)/

install-exec-hook:
	cp EmdrosNode.ini $(DESTDIR)$(pkglibdir)
	rm -f $(DESTDIR)$(pkglibdir)/EmdrosNode.a

#rm -f $(DESTDIR)/$(Nodejs_INI_DIR)/EmdrosNode.ini
uninstall-hook:
	rm -f $(DESTDIR)$(pkglibdir)/EmdrosNode.so*

macdist:
	cp -fL README-Nodejs .libs/EmdrosNode.so $(MACDISTDIR)/development/SWIG/nodejs_/


else
# Not SWIG_WITH_NODEJS_WRAPPERS

pkglib_LTLIBRARIES =

install-exec-hook:

uninstall-hook:

macdist:


endif # end if SWIG_WITH_NODEJS_WRAPPERS

BUILT_SOURCES = $(MAYBE_BUILT_SRC) binding.gyp


EXTRA_DIST = nodejs_emdros.i \
             libemdf_nodejs.i libmql_nodejs.i \
             binding.gyp.in \
             test_Emdros_Node.js \
             README-Nodejs \
             $(MAYBE_BUILT_SRC)

CLEANFILES = *~ core config.log \
             phpemdrostest


DISTCLEANFILES = $(SWIG_NODEJS_SRC) \
                 binding.gyp 

