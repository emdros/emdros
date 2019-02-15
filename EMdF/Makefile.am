LEMON = ./lemon
#
# Variables
#
if HOSTISDARWIN
MACLDFLAGS = -static
endif

DEFINE_SQLITE3_EMDFDB_NO_FSYNC = -DSQLITE3_EMDFDB_NO_FSYNC



BUILT_SOURCES = \
                $(srcdir)/json_lexer.cpp \
                $(srcdir)/json_parser.cpp \
                $(srcdir)/jsonpars.h



AM_CXXFLAGS = @DEBUGFLAG@ @SUN_CXXFLAGS@ @DARWIN_CXXFLAGS@ \
              $(DEFINE_SQLITE3_EMDFDB_NO_FSYNC) \
              $(DEFINE_SQLITE3_USE_SYSTEM_SQLITE3) \
              @ADDITIONAL_CXXFLAGS@ 


# lemon
$(srcdir)/lemon.c: $(top_srcdir)/sqlite3/tool/lemon.c
	cp $(top_srcdir)/sqlite3/tool/lemon.c $(srcdir)/

$(srcdir)/lempar.c: $(top_srcdir)/sqlite3/tool/lempar.c
	cp -f $(top_srcdir)/sqlite3/tool/lempar.c $(srcdir)/

$(srcdir)/lemon: $(srcdir)/lemon.c
	cd $(srcdir) ; $(CC) -o lemon lemon.c


#
# Sources
#
emdflib_src = arena.cpp \
       conn.cpp \
       emdf_primes.cpp \
       bptemdfdb.cpp \
       emdfdb.cpp \
       pgemdfdb.cpp \
       mysqlemdfdb.cpp \
       sqlite3emdfdb.cpp \
       pgconn.cpp \
       mysqlconn.cpp \
       sqlite3conn.cpp \
       crc32_emdros.cpp \
       emdf_ffeatures.cpp \
       emdf_hash.cpp \
       emdf_output.cpp \
       emdf_value.cpp \
       encryption.cpp \
       enum_const_cache.cpp \
       infos.cpp \
       inst.cpp \
       inst_object.cpp \
       logging.cpp \
       minidom.cpp \
       monads.cpp \
       qdxml.cpp \
       string_func.cpp \
       string2som_map.cpp \
       string_list.cpp \
       emdf_enums.cpp \
       bpa.cpp \
       string_set_cache.cpp \
       table.cpp \
       utils.cpp \
       smart_vector.cpp \
       json_classes.cpp \
       json_parser.cpp \
       json_lexer.cpp \
       lzma_emdros.cpp 

noinst_HEADERS = jsonpars.h


#
# Targets
#

if DO_DEVEL
MAYBE_LIBS_EMDF = libemdf.la
else
MAYBE_LIBS_EMDF = 
endif


lib_LTLIBRARIES = $(MAYBE_LIBS_EMDF)


#
# Misc variables
#
EXTRA_DIST = win32.mak mingw.mak \
             json.re json_lexer.cpp \
             json.yxx json_parser.cpp jsonpars.h




#
# libemdf.la
#
libemdf_la_SOURCES = $(emdflib_src) 
libemdf_la_LIBADD = @BACKEND_LDADD@ 
libemdf_la_LDFLAGS = $(MACLDFLAGS) -version-info \
                     @LIB_EMDROS_CURRENT@:@LIB_EMDROS_REVISION@:@LIB_EMDROS_AGE@ \
                     @SUN_LDFLAGS@ \
                     @DARWIN_LDFLAGS@

AM_CPPFLAGS = -I$(srcdir)/../include

CLEANFILES = *~ core \
             $(srcdir)/lemon.c $(srcdir)/lempar.c lemon \
             json_lexer.cpp json_parser.cpp json.c jsonpars.h json.out json.h 




macdist:
	cp -f .libs/libemdf.a $(MACDISTDIR)/development/lib/


# JSON parser

$(srcdir)/json.c: $(srcdir)/json.yxx $(srcdir)/lempar.c $(srcdir)/lemon.c $(srcdir)/lemon
	cd $(srcdir)/ ; $(LEMON) -s $(srcdir)/json.yxx

$(srcdir)/json.h: $(srcdir)/json.c

$(srcdir)/json_parser.cpp: $(srcdir)/json.c
	sed -e 's_yy_jsonyy_g;s_YY_JSONYY_g;s/mql_jsonyylex.h/mql_yylex.h/g' $(srcdir)/json.c > $@

$(srcdir)/jsonpars.h: $(srcdir)/json.h
	cp -f $(srcdir)/json.h $(srcdir)/jsonpars.h


$(srcdir)/json_parser.o: $(srcdir)/json_parser.cpp




#
# Lexers
#

$(srcdir)/json_lexer.cpp: json.re
	re2c -s -o$(srcdir)/json_lexer.cpp $(srcdir)/json.re


#dist-hook:
#	rm -f \
#              $(srcdir)/json_parser.cpp \
#              $(srcdir)/jsonpars.h \
#              $(srcdir)/json.out 
