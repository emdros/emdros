

if HOSTISDARWIN
MACLDFLAGS = -static
endif


lib_LTLIBRARIES = libemdros_amalgamation.la
DO_MACDIST = cp -f .libs/libemdros_amalgamation.a $(MACDISTDIR)/development/lib/

mysourcecode = emdros_amalgamation_1_emdros.cpp \
               emdros_amalgamation_2_importer.cpp \
               emdros_c_amalgamation_1.c \
               emdros_c_amalgamation_2.c

myheaders = emdros.h emdros_c.h emdros-lconfig.h emdros-config.h


nodist_libemdros_amalgamation_la_SOURCES =  $(mysourcecode)


libemdros_amalgamation_la_LDFLAGS = $(MACLDFLAGS) -version-info \
                     @LIB_EMDROS_CURRENT@:@LIB_EMDROS_REVISION@:@LIB_EMDROS_AGE@


BUILT_SOURCES = $(mysourcecode) \
                emdros.h \
                emdros_c.h \
                emdros-lconfig.h \
                emdros-config.h 

nodist_pkginclude_HEADERS = $(myheaders)


CLEANFILES = $(mysourcecode) $(myheaders) *~ core amalgamation_made.txt

EXTRA_DIST = mkamalgamation.py \
             amalgamation.xml \
             windows.mak mingw.mak 

AM_CPPFLAGS = 
AM_CFLAGS = @DEBUGFLAG@ @ADDITIONAL_CFLAGS@

AM_CXXFLAGS = \
              @DEBUGFLAG@ @SUN_CXXFLAGS@ @DARWIN_CXXFLAGS@ \
              $(DEFINE_SQLITE3_EMDFDB_NO_FSYNC) \
              @ADDITIONAL_CXXFLAGS@ -I$(top_srcdir)


$(mysourcecode): amalgamation_made.txt

emdros.h: amalgamation_made.txt

emdros_c.h: amalgamation_made.txt

amalgamation_made.txt: mkamalgamation.py amalgamation.xml
	python mkamalgamation.py amalgamation.xml
	touch amalgamation_made.txt

emdros-lconfig.h: ../include/emdros-lconfig.h
	cp ../include/emdros-lconfig.h $@

emdros-config.h: ../include/emdros-config.h
	cp ../include/emdros-config.h $@

macdist:	
	$(DO_MACDIST)
