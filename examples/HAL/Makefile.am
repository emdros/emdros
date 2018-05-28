SUBDIRS = doc

if HOSTISDARWIN
MACLDFLAGS = -static
endif

AM_CXXFLAGS = @DEBUGFLAG@ @SUN_CXXFLAGS@ @DARWIN_CXXFLAGS@ \
              @ADDITIONAL_CXXFLAGS@ 

ARFLAGS = @ARFLAGS@

EXTRA_DIST = win32.mak mingw.mak hal.cfg hallexer.re

AM_CPPFLAGS = -I$(srcdir) -I$(top_srcdir)/include

bin_PROGRAMS = hal_build_db mqlhal

hal_build_db_SOURCES = halblddb.cpp
hal_build_db_LDADD = -L$(top_builddir)/examples/HAL/ -lhal \
                     $(top_builddir)/util/libutil_emdros.la \
                     $(top_builddir)/MQL/libmql.la @PCRE_LIBADD_SWIG@ \
                     @EMDFLDADDSWIG@ @SUN_LDFLAGS@  @DARWIN_LDFLAGS@ @ZLIB_LIBADD_SWIG@ 
hal_build_db_LDFLAGS = $(MACLDFLAGS)
hal_build_db_DEPENDENCIES = libhal.a \
                            $(top_builddir)/util/libutil_emdros.la \
                            @EMDFDEPSSWIG@ $(top_builddir)/MQL/libmql.la \
                            @PCRE_DEP_SWIG@ @ZLIB_DEP_SWIG@

mqlhal_SOURCES = hal.cpp
mqlhal_LDFLAGS = $(MACLDFLAGS)
mqlhal_LDADD = -L$(top_builddir)/examples/HAL -lhal \
               $(top_builddir)/util/libutil_emdros.la \
               $(top_builddir)/MQL/libmql.la @PCRE_LIBADD_SWIG@ @EMDFLDADDSWIG@ \
               @ZLIB_LIBADD_SWIG@ @SUN_LDFLAGS@  @DARWIN_LDFLAGS@ -lm
mqlhal_DEPENDENCIES = libhal.a \
                      $(top_builddir)/util/libutil_emdros.la \
                      @EMDFDEPSSWIG@ $(top_builddir)/MQL/libmql.la \
                      @PCRE_DEP_SWIG@ @ZLIB_DEP_SWIG@




noinst_HEADERS = \
      HALSpace.h \
      hal_build_database.h \
      hal_defines.h \
      hal_execution_environment.h \
      hal_schema.h \
      hal_string_func.h \
      hallexer.h

CLEANFILES = *~ core 

DISTCLEANFILES = $(srcdir)/hallexer.cpp 


BUILT_SOURCES = $(srcdir)/hallexer.cpp


noinst_LIBRARIES = libhal.a

libhal_a_SOURCES = hallexer.cpp \
                   hal_execution_environment.cpp \
                   hal_schema.cpp \
                   hal_build_database.cpp \
                   hal_string_func.cpp \
                   HALSpace.cpp
if HOSTISSUNCC
libhal_a_AR = $(CXX) -xar -o
else
libhal_a_AR = $(AR) $(ARFLAGS)
endif

$(srcdir)/hallexer.cpp: $(srcdir)/hallexer.re \
                        $(srcdir)/hal_defines.h \
                        $(srcdir)/hal_execution_environment.h
	re2c -s -o$@ $(srcdir)/hallexer.re


haldir = $(prefix)/share/emdros/hal
dist_hal_DATA = hal.cfg

macdist:
	cp $(bin_PROGRAMS) $(MACDISTDIR)/development/bin
	cp $(dist_hal_DATA) $(MACDISTDIR)/development/share/emdros/hal

