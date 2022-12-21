SWIG_PROGRAM = @SWIG_PROGRAM@

SWIG_NODEJS_SRC = \
      nodejs_emdros_wrap.cxx \
      emdros_amalgamation_1_emdros.cpp \
      emdros_c_amalgamation_1.c \
      emdros_c_amalgamation_2.c \
      emdros-lconfig.h \
      emdros_c.h \
      emdros-config.h \
      emdros.h \
      binding.gyp 


# This is built by node-gyp. The name is defined in binding.gyp.in
RELEASE_EMDROS_NODE = build/Release/Emdros.node


AM_CXXFLAGS = @SUN_CXXFLAGS@ @DARWIN_CXXFLAGS@ @ADDITIONAL_CXXFLAGS@ @DEBUGFLAG@

NODE_CXXFLAGS = @CXXFLAGS@ @SUN_CXXFLAGS@ @DARWIN_CXXFLAGS@ @ADDITIONAL_CXXFLAGS@

NODE_CFLAGS = @CFLAGS@ @ADDITIONAL_CFLAGS@


AM_CPPFLAGS = -I. -I$(top_srcdir) @NODEJS_INCLUDES@

nodejs_emdros_wrap.cxx: nodejs_emdros.i libemdf_nodejs.i libmql_nodejs.i 
	$(SWIG_PROGRAM) -c++ -javascript -node nodejs_emdros.i

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



build/Release/Emdros.node: $(SWIG_NODEJS_SRC)
	$(NODE_GYP_PROGRAM) clean configure build



if CAN_DO_SWIG
MAYBE_BUILT_SRC = $(SWIG_NODEJS_SRC) $(RELEASE_EMDROS_NODE)
endif


if SWIG_WITH_NODEJS_WRAPPERS

install-exec-hook:
	cp $(RELEASE_EMDROS_NODE) $(DESTDIR)$(pkglibdir)/


uninstall-hook:
	rm -f $(DESTDIR)$(pkglibdir)/$(RELEASE_EMDROS_NODE)

macdist:
	cp -fL README_NODEJS.md test_Emdros_Node.js $(RELEASE_EMDROS_NODE) $(MACDISTDIR)/development/SWIG/nodejs/


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
             README_NODEJS.md \
             $(MAYBE_BUILT_SRC)

CLEANFILES = *~ core config.log \
             phpemdrostest


DISTCLEANFILES = $(SWIG_NODEJS_SRC) \
                 binding.gyp 

