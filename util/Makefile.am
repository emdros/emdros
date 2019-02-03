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

libutil_emdros_la_DEPENDENCIES = @HARVEST_DEPS@ @MQL_DEPS@ @EMDF_DEPS@ @PCRE_DEPS@
libutil_emdros_la_LIBADD = @HARVEST_LDADD@ @MQL_LDADD@ @EMDF_LDADD@ @PCRE_LDADD@
libutil_emdros_la_LDFLAGS = $(MACLDFLAGS) \
                            @SUN_LDFLAGS@ \
                            @DARWIN_LDFLAGS@

libutil_emdros_la_SOURCES = mql_exporter.cpp opt.cpp \
                            conf.cpp conf_ini.cpp \
                            schema.cpp \
                            messages.cpp \
                            md5_emdros.cpp \
                            bptdumper.cpp bpt2dumper.cpp erw.cpp


CLEANFILES = *~ core \
             emdfxml_dtd.h

EXTRA_DIST = win32.mak mingw.mak \
             emdfxml_dtd.h emdfxml.dtd

AM_CPPFLAGS =
AM_CXXFLAGS = @DEBUGFLAG@ @ADDITIONAL_CXXFLAGS@ 


BUILT_SOURCES = emdfxml_dtd.h


emdfxml_dtd.h: emdfxml.dtd
	rm -f $@
	echo "char emdfxml_dtd[] = " >> $@
	cat emdfxml.dtd | sed -e 's_\\_\\\\_g;s_\"_\\"_g;s_^_"_g;s_$$_\\n"_g' >> $@
	echo ";" >> $@
	echo "" >> $@



noinst_HEADERS = $(srcdir)/emdfxml_dtd.h 

macdist:
	cp -f .libs/libutil_emdros.a $(MACDISTDIR)/development/lib/
