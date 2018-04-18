SWIG_PROGRAM = @SWIG_PROGRAM@
NODE_GYP_PROGRAM = @NODE_GYP_PROGRAM@

AMALGAMATION_COPIED_SRC = \
      emdros_amalgamation_1.cpp \
      emdros_amalgamation_2.cpp \
      emdros_c_amalgamation_1.c \
      emdros_c_amalgamation_2.c \
      emdros-lconfig.h \
      emdros-config.h \
      emdros.h



AM_CXXFLAGS = @SUN_CXXFLAGS@ @DARWIN_CXXFLAGS@ @ADDITIONAL_CXXFLAGS@ @DEBUGFLAG@

AM_CPPFLAGS = -I$(top_srcdir)/include -I. @NODEJS_INCLUDES@

binding.gyp: binding.gyp.in $(top_builddir)/config.status 
	$(top_builddir)/config.status --file=./binding.gyp


emdrosnode_wrap.cxx: emdrosnode.i libemdfnodejs.i libmqlnodejs.i 
	cd $(srcdir) ; $(SWIG_PROGRAM) -c++ -javascript -node emdrosnode.i



if SWIG_WITH_NODEJS_WRAPPERS

MAYBE_BUILT_SRC = $(AMALGAMATION_COPIED_SRC) emdrosnode_wrap.cxx

MAYBE_BUILT_TARGETS = binding.gyp built.node

install-exec-hook:
	cp build/Release/emdrosnode.node $(DESTDIR)$(pkglibdir)

uninstall-hook:
	rm -f $(DESTDIR)$(pkglibdir)/emdrosnode.node


macdist:
	cp -fL README-NodeJS build/Release/emdrosnode.node test.js $(MACDISTDIR)/development/SWIG/nodejs/


else
# Not SWIG_WITH_NODEJS_WRAPPERS

MAYBE_BUILT_SRC =

MAYBE_BUILT_TARGETS = 

install-exec-hook:

uninstall-hook:

macdist:


endif # end if SWIG_WITH_NODEJS_WRAPPERS





clean-hook:
	rm -rf build/


emdros_amalgamation_1.cpp: ../../amalgamation/emdros_amalgamation_1.cpp
	cp -p ../../amalgamation/emdros_amalgamation_1.cpp $@

emdros_amalgamation_2.cpp: ../../amalgamation/emdros_amalgamation_2.cpp
	cp -p ../../amalgamation/emdros_amalgamation_2.cpp $@

emdros_c_amalgamation_1.c: ../../amalgamation/emdros_c_amalgamation_1.c
	cp -p ../../amalgamation/emdros_c_amalgamation_1.c $@

emdros_c_amalgamation_2.c: ../../amalgamation/emdros_c_amalgamation_2.c
	cp -p ../../amalgamation/emdros_c_amalgamation_2.c $@

emdros-lconfig.h: ../../amalgamation/emdros-lconfig.h
	cp -p ../../amalgamation/emdros-lconfig.h $@

emdros-config.h: ../../amalgamation/emdros-config.h
	cp -p ../../amalgamation/emdros-config.h $@

emdros.h: ../../amalgamation/emdros.h
	cp -p ../../amalgamation/emdros.h $@

# Do the actual build, using NODE_GYP_PROGRAM
built.node: $(AMALGAMATION_COPIED_SRC) binding.gyp emdrosnode_wrap.cxx 
	rm -f $@
	$(NODE_GYP_PROGRAM) rebuild
	touch $@

all-local: $(MAYBE_BUILT_TARGETS)

BUILT_SOURCES = $(MAYBE_BUILT_SRC) 


EXTRA_DIST = binding.gyp.in \
             emdrosnode.i libemdfnodejs.i libmqlnodejs.i \
             README-NodeJS test.js 

CLEANFILES = *~ core config.log binding.gyp built.node \
             $(AMALGAMATION_COPIED_SRC) $(MAYBE_BUILT_SRC)

DISTCLEANFILES = emdrosnode_wrap.cxx 
