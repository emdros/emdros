if SQLITE3_EMDFDB_NO_FSYNC
DEFINE_SQLITE3_EMDFDB_NO_FSYNC = -DSQLITE3_EMDFDB_NO_FSYNC
else
DEFINE_SQLITE3_EMDFDB_NO_FSYNC = 
endif


if HOSTISDARWIN
MACLDFLAGS = -static
endif

noinst_PROGRAMS = mkamalgamation
mkamalgamation_SOURCES = mkamalgamation.cpp
mkamalgamation_CPPFLAGS = -I../include
mkamalgamation_LDADD = @EMDROS_LDADD@ @SUN_LDFLAGS@ @DARWIN_LDFLAGS@ @ZLIB_LIBADD@ 


lib_LTLIBRARIES = libemdros_amalgamation.la
DO_MACDIST = cp -f .libs/libemdros_amalgamation.a $(MACDISTDIR)/development/lib/

mysourcecode = emdros_amalgamation_1.cpp \
               emdros_amalgamation_2.cpp \
               emdros_c_amalgamation_1.c \
               emdros_c_amalgamation_2.c

myheaders = emdros.h emdros-lconfig.h emdros-config.h


nodist_libemdros_amalgamation_la_SOURCES =  $(mysourcecode)


libemdros_amalgamation_la_LDFLAGS = $(MACLDFLAGS) -version-info \
                     @LIB_EMDF_CURRENT@:@LIB_EMDF_REVISION@:@LIB_EMDF_AGE@ \
                     @SUN_LDFLAGS@

BUILT_SOURCES = $(mysourcecode) \
                emdros-lconfig.h \
                emdros-config.h 

nodist_noinst_HEADERS = $(myheaders)


CLEANFILES = $(mysourcecode) $(myheaders) *~ core $(noinst_PROGRAMS)

EXTRA_DIST = mkamalgamation.cpp amalgamation.json win32.mak mingw.mak

AM_CPPFLAGS = 
AM_CFLAGS = @DEBUGFLAG@ @ADDITIONAL_CFLAGS@

AM_CXXFLAGS = $(DEFINE_SQLITE3_EMDFDB_NO_FSYNC)



emdros_amalgamation_1.cpp emdros_amalgamation_2.cpp emdros_c_amalgamation_1.c emdros_c_amalgamation_2.c: mkamalgamation amalgamation.json
	./mkamalgamation amalgamation.json

emdros-lconfig.h: ../include/emdros-lconfig.h
	cp ../include/emdros-lconfig.h $@

emdros-config.h: ../include/emdros-config.h
	cp ../include/emdros-config.h $@

macdist:	
	$(DO_MACDIST)
