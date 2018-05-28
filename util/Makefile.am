if HOSTISDARWIN
MACLDFLAGS = -static
endif

ARFLAGS = @ARFLAGS@

if DO_DEVEL
MAYBE_LIBS_UTIL = libutil_emdros.la
else
MAYBE_LIBS_UTIL = 
endif

lib_LTLIBRARIES = $(MAYBE_LIBS_UTIL)

libutil_emdros_la_DEPENDENCIES = @HARVEST_DEPS_BACKBACK@ @MQL_DEPS_BACKBACK@ @EMDF_DEPS_BACKBACK@
libutil_emdros_la_LIBADD = @HARVEST_LDADD_BACKBACK@ @MQL_LDADD_BACKBACK@ @EMDF_LDADD_BACKBACK@
libutil_emdros_la_LDFLAGS = $(MACLDFLAGS) \
                            @SUN_LDFLAGS@ \
                            @DARWIN_LDFLAGS@
libutil_emdros_la_SOURCES = \
                          conf.cpp \
                          conf_ini.cpp \
                          opt.cpp \
                          messages.cpp \
                          erw.cpp \
                          md5_emdros.cpp \
                          mql_exporter.cpp \
                          bpt2dumper.cpp \
                          bptdumper.cpp \
                          schema.cpp 


CLEANFILES = *~ core

EXTRA_DIST = windows.mak mingw.mak


AM_CPPFLAGS = -I$(srcdir)/../MQL -I$(srcdir)/../EMdF -I$(srcdir)/../base -I$(top_srcdir)
AM_CXXFLAGS = @DEBUGFLAG@ @ADDITIONAL_CXXFLAGS@ 


BUILT_SOURCES = 


macdist:
	cp -f .libs/libutil_emdros.a $(MACDISTDIR)/development/lib/
