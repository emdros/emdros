EXTRA_DIST = emdros-lconfig.h.in emdros-config.h.in \
             Makefile.inc win32.mak mingw.mak \
             prefix_emdros.h.in \
             get_version.cpp

include Makefile.inc

PACKAGE_INCLUDE_HEADER_FILES_NODIST = \
       pcre_emdros.h zlib_emdros.h

if USE_LOCAL_PCRE
PACKAGE_INCLUDE_HEADER_FILES_NODIST += pcre_config.h
endif

PACKAGE_INCLUDE_HEADER_FILES_NODIST += zconf.h



dist_pkginclude_HEADERS = $(PACKAGE_INCLUDE_HEADER_FILES_DIST)

nodist_pkginclude_HEADERS = $(PACKAGE_INCLUDE_HEADER_FILES_NODIST)

noinst_HEADERS = mql_R.h \
       mql_database_statements.h \
       mql_enumeration_statements.h \
       mql_features_statements.h \
       mql_meta_statements.h \
       mql_monads_statements.h \
       mql_object.h \
       mql_object_statements.h \
       mql_object_type_statements.h \
       mql_segment_statements.h \
       mql_select_statements.h \
       mql_utility.h \
       emdf_ffeatures.h \
       enum_const_cache.h \
       pgconn.h \
       mysqlconn.h \
       sqlite3conn.h \
       lzma_emdros.h \
       pcre_emdros.h zlib_emdros.h \
       prefix_emdros.h \
       sqlite3_emdros.h \
       emdros_reserved_words.h 

#
# pcre_emdros.h is placed here because automake doesn't support 
# placing it in $(srcdir)/../pcre/Makefile.am
BUILT_SOURCES = pcre_emdros.h zlib_emdros.h zconf.h \
                prefix_emdros.h \
                sqlite3_emdros.h \
                emdros_reserved_words.h 

CLEANFILES = *~ pcre_emdros.h zlib_emdros.h \
                prefix_emdros.h \
                sqlite3_emdros.h \
                emdros_reserved_words.h \
                core 

DISTCLEANFILES = emdros-config.h.in

if USE_LOCAL_PCRE
pcre_emdros.h: $(srcdir)/../pcre/pcre.h
	echo '#include "pcre_config.h"' >> ./pcre_emdros.h
	cat $(srcdir)/../pcre/pcre.h >> ./pcre_emdros.h
else
pcre_emdros.h:
	echo "#include <pcre.h>" > pcre_emdros.h
endif

if USE_LOCAL_ZLIB
zconf.h: $(srcdir)/../zlib/zconf.h
	cp $(srcdir)/../zlib/zconf.h $@

zlib_emdros.h: $(srcdir)/../zlib/zlib.h zconf.h
	rm -f $@
	echo '#ifndef ZLIB_EMDROS_H_' >> zlib_emdros.h
	echo '#define ZLIB_EMDROS_H_' >> zlib_emdros.h
	echo '#include "emdros-lconfig.h"' >> zlib_emdros.h
	echo '#if (defined(USE_BPT) && USE_BPT)' >> zlib_emdros.h
	echo '' >> zlib_emdros.h
	echo '#include "zconf.h"' >> ./zlib_emdros.h
	cat $(srcdir)/../zlib/zlib.h >> ./zlib_emdros.h
	echo '#endif /* (defined(USE_BPT) && USE_BPT) */' >> zlib_emdros.h
	echo '#endif /* !defined(ZLIB_EMDROS_H_) */' >> zlib_emdros.h
else
zconf.h: 
	rm -f $@
	echo '#ifndef ZCONF_H_' >> $@
	echo '#define ZCONF_H_' >> $@
	echo '/* Dummy file, necessary for compiling the amalgamation. */' >> $@
	echo '#endif /* !defined(ZCONF_H_) */' >> $@


zlib_emdros.h:
	rm -f $@
	echo '#ifndef ZLIB_EMDROS_H_' >> zlib_emdros.h
	echo '#define ZLIB_EMDROS_H_' >> zlib_emdros.h
	echo '#include "emdros-lconfig.h"' >> zlib_emdros.h
	echo '#if (defined(USE_BPT) && USE_BPT)' >> zlib_emdros.h
	echo "#include <zlib.h>" >> zlib_emdros.h
	echo '#endif /* (defined(USE_BPT) && USE_BPT) */' >> zlib_emdros.h
	echo '#endif /* !defined(ZLIB_EMDROS_H_) */' >> zlib_emdros.h
endif

if USE_LOCAL_SQLITE3
sqlite3_emdros.h: $(srcdir)/../sqlite3/src/sqlite3.h
	rm -f $@
	echo "#ifndef SQLITE3_EMDROS_H_" >> $@
	echo "#define SQLITE3_EMDROS_H_" >> $@
	echo "#include <emdros-lconfig.h>" >> $@
	echo "#if USE_SQLITE3" >> $@
	cat $(srcdir)/../sqlite3/src/sqlite3.h >> $@
	echo "#endif /* USE_SQLITE3 */" >> $@
	echo "#endif /* SQLITE3_EMDROS_H_ */" >> $@
else
sqlite3_emdros.h: 
	rm -f $@
	echo "#ifndef SQLITE3_EMDROS_H_" >> $@
	echo "#define SQLITE3_EMDROS_H_" >> $@
	echo "#include <emdros-lconfig.h>" >> $@
	echo "#if USE_SQLITE3" >> $@
	echo "#include <sqlite3.h>" >> sqlite3_emdros.h
	echo "#endif /* USE_SQLITE3 */" >> $@
	echo "#endif /* SQLITE3_EMDROS_H_ */" >> $@
endif



prefix_emdros.h: prefix_emdros.h.in
	$(top_builddir)/config.status --file=$(srcdir)/prefix_emdros.h


macdist:
	cp -f $(PACKAGE_INCLUDE_HEADER_FILES) $(MACDISTDIR)/development/include/

emdros_reserved_words.h: ../MQL/mql.re
	rm -f $@
	echo "#ifndef EMDROS_RESERVED_WORDS_H_" >> $@
	echo "#define EMDROS_RESERVED_WORDS_H_" >> $@
	echo "static const char *emdros_reserved_words[] = {" >> $@
	grep "TOKEN_RETURN(\"[a-zA-Z_]\+\"" ../MQL/mql.re | grep -v "T_IDENTIFIER" | grep -v "T_STRING" | awk -F 'TOKEN_RETURN' '{ print $$2; }' | sed -e 's/[(",]//g' | awk '{ print "\"" $$1 "\","; }' >> $@
	echo " NULL" >> $@
	echo "};" >> $@
	echo "#endif /* EMDROS_RESERVED_WORDS_H_ */" >> $@

