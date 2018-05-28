# You may need to change these
LEMON = ./lemon

# CXXFLAGS is included on the compilation line already
AM_CXXFLAGS = @DEBUGFLAG@ @SUN_CXXFLAGS@ @DARWIN_CXXFLAGS@ \
              @ADDITIONAL_CXXFLAGS@ 

if HOSTISDARWIN
MACLDFLAGS = -static
endif

# lemon
$(srcdir)/lemon: lemon.c
	$(CC) -o lemon lemon.c

lemon.c: $(top_srcdir)/sqlite3/tool/lemon.c
	cp $(top_srcdir)/sqlite3/tool/lemon.c $(srcdir)/

lempar.c: $(top_srcdir)/sqlite3/tool/lempar.c
	cp $(top_srcdir)/sqlite3/tool/lempar.c $(srcdir)/


#
# Sources
#

mqllib_src = mql_yylex.cpp \
       mql_parser.cpp \
       mql_lexer.cpp \
       mql_get_query.cpp \
       mql_query.cpp \
       mql_R.cpp \
       emdros_reserved_words.cpp \
       emdros_environment.cpp \
       mql_database_statements.cpp \
       mql_enumeration_statements.cpp \
       mql_error.cpp \
       mql_execute.cpp \
       mql_execution_environment.cpp \
       mql_features_statements.cpp \
       mql_helper_classes.cpp \
       mql_meta_statements.cpp \
       mql_monads_statements.cpp \
       mql_object.cpp \
       mql_object_statements.cpp \
       mql_object_type_statements.cpp \
       mql_result.cpp \
       mql_segment_statements.cpp \
       mql_select_statements.cpp \
       mql_sheaf.cpp \
       mql_statement.cpp \
       mql_utility.cpp 




if DO_DEVEL
MAYBE_LIBS_MQL = libmql.la
else
MAYBE_LIBS_MQL = 
endif


lib_LTLIBRARIES = $(MAYBE_LIBS_MQL)



#
# libmql.la
#
libmql_la_SOURCES = $(mqllib_src)

libmql_la_LIBADD = @EMDF_LDADD_BACKBACK@ @PCRE2_LDADD_BACKBACK@ 
libmql_la_DEPENDENCIES = @EMDF_DEPS_BACKBACK@ @PCRE2_DEPS_BACKBACK@ 
libmql_la_LDFLAGS = $(MACLDFLAGS) -version-info \
       @LIB_EMDROS_CURRENT@:@LIB_EMDROS_REVISION@:@LIB_EMDROS_AGE@ \
       @SUN_LDFLAGS@ @DARWIN_LDFLAGS@




#
# Misc variables
#
EXTRA_DIST = mql.re mql_lexer.cpp \
             mql_gq.re mql_get_query.cpp \
             mql.yxx mql_parser.cpp mqlpars.h \
             win32.mak mingw.mak unistd.h.win32 

BUILT_SOURCES = $(srcdir)/mql_lexer.cpp \
                $(srcdir)/mql_parser.cpp \
                $(srcdir)/mqlpars.h \
                $(srcdir)/mql_get_query.cpp \
                emdros_reserved_words.cpp


#
# Headers
#

mql_built_headers = $(top_srcdir)/include/mqlpars.h


noinst_HEADERS = \
                 $(mql_built_headers) 

AM_CPPFLAGS = -I$(srcdir)/../include

CLEANFILES = *~ core $(srcdir)/lemon $(top_srcdir)/include/unistd.h \
             $(srcdir)/mql.out \
             $(srcdir)/lemon.c $(srcdir)/lempar.c

DISTCLEANFILES = $(srcdir)/mql_get_query.cpp $(srcdir)/mql_lexer.cpp \
             $(srcdir)/mql_parser.cpp \
             $(srcdir)/mqlpars.h \
             $(top_srcdir)/include/mqlpars.h 


#
# Parsers
#

$(srcdir)/mqlpars.h: $(srcdir)/mql_parser.cpp

$(srcdir)/mql_parser.cpp: $(srcdir)/mql.yxx $(srcdir)/lempar.c $(srcdir)/lemon
	cd $(srcdir)/ ; $(LEMON) -s $(srcdir)/mql.yxx
	sed -e 's_yy_mqlyy_g;s_YY_MQLYY_g;s/mql_mqlyylex.h/mql_yylex.h/g' $(srcdir)/mql.c > $@
	cp $(srcdir)/mql.h $(srcdir)/mqlpars.h

$(srcdir)/mql_parser.o: $(srcdir)/mql_parser.cpp

$(top_srcdir)/include/mqlpars.h: $(srcdir)/mqlpars.h
	cp $(srcdir)/mqlpars.h $(top_srcdir)/include/






#
# Lexers
#

$(srcdir)/mql_get_query.cpp: mql_gq.re
	re2c -s -o$(srcdir)/mql_get_query.cpp $(srcdir)/mql_gq.re

$(srcdir)/mql_lexer.cpp: mql.re
	re2c -s -o$(srcdir)/mql_lexer.cpp $(srcdir)/mql.re



# Dependencies

mql_execute.o: $(top_srcdir)/include/mqlflexr.h


emdros_reserved_words.cpp: ../MQL/mql.re
	rm -f $@
	echo "#include <emdros_reserved_words.h>" >> $@
	echo "const char *emdros_reserved_words[] = {" >> $@
	grep "TOKEN_RETURN(\"[a-zA-Z_]\+\"" ../MQL/mql.re | grep -v "T_IDENTIFIER" | grep -v "T_STRING" | awk -F 'TOKEN_RETURN' '{ print $$2; }' | sed -e 's/[(",]//g' | awk '{ print "\"" $$1 "\","; }' >> $@
	echo " (const char*) 0" >> $@
	echo "};" >> $@


#dist-hook:
#	rm -f $(srcdir)/mql_get_query.cpp \
#              $(srcdir)/mql_parser.cpp \
#              $(srcdir)/mqlpars.h \
#              $(srcdir)/mql.out \
#              lempar.c 


macdist:
	cp -f .libs/libmql.a $(MACDISTDIR)/development/lib/
