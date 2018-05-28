# You may need to change these
LEMON = ./lemon

if HOSTISDARWIN
MACLDFLAGS = -static
endif

ARFLAGS = @ARFLAGS@

if DO_DEVEL
MAYBE_LIBS_HARVEST = libharvest.la
else
MAYBE_LIBS_HARVEST = 
endif

lib_LTLIBRARIES = $(MAYBE_LIBS_HARVEST)

libharvest_la_DEPENDENCIES = @MQL_DEPS@ @EMDF_DEPS@
libharvest_la_LIBADD = @MQL_LDADD@ @EMDF_LDADD@
libharvest_la_LDFLAGS = $(MACLDFLAGS) \
                            @SUN_LDFLAGS@ \
                            @DARWIN_LDFLAGS@


libharvest_la_SOURCES = \
                        templatelang_classes.cpp \
                        templatelang_parser.cpp templatelang_lexer.cpp \
                        renderxml.cpp \
                        memobject.cpp \
                        renderobjects.cpp \
                        harvest_fts.cpp \
                        harvest_fts2.cpp \
                        harvest_fts3.cpp

emdroslang_sources = \
                        emdroslang_classes.cpp \
                        emdroslang_parser.cpp emdroslang_lexer.cpp




CLEANFILES = *~ core config.log 

DISTCLEANFILES = \
                 $(srcdir)/lemon $(srcdir)/lemon.c $(srcdir)/lempar.c \
                 templatelang_lexer.cpp \
                 templatelang_parser.cpp templatelangpars.h templatelang.out \
                 emdroslang_lexer.cpp \
                 emdroslang_parser.cpp emdroslangpars.h emdroslang.out


EXTRA_DIST = win32.mak mingw.mak \
             templatelang.re templatelang_lexer.cpp \
             templatelang.yxx templatelang_parser.cpp templatelangpars.h \
             emdroslang.re emdroslang_lexer.cpp \
             emdroslang.yxx emdroslang_parser.cpp emdroslangpars.h




AM_CPPFLAGS = -I$(srcdir) -I$(top_srcdir)

AM_CXXFLAGS = @DEBUGFLAG@ @ADDITIONAL_CXXFLAGS@ 


BUILT_SOURCES = \
                $(srcdir)/templatelang_lexer.cpp \
                $(srcdir)/templatelang_parser.cpp \
                $(srcdir)/templatelangpars.h \
                $(srcdir)/emdroslang_lexer.cpp \
                $(srcdir)/emdroslang_parser.cpp \
                $(srcdir)/emdroslangpars.h



# lemon
$(srcdir)/lemon: $(srcdir)/lemon.c
	$(CC) -o lemon lemon.c

$(srcdir)/lemon.c: $(top_srcdir)/sqlite3/tool/lemon.c
	cp $(top_srcdir)/sqlite3/tool/lemon.c $@

$(srcdir)/templatelangpars.h: $(srcdir)/templatelang_parser.cpp

$(srcdir)/templatelang_parser.cpp: $(srcdir)/templatelang.yxx $(srcdir)/lempar.c $(srcdir)/lemon
	cd $(srcdir)/ ; $(LEMON) -s $(srcdir)/templatelang.yxx
	sed -e 's_yy_tmplgyy_g;s_YY_TMPLGYY_g;s/mql_tmplgyylex.h/mql_yylex.h/g' $(srcdir)/templatelang.c > $@
	mv $(srcdir)/templatelang.h $(srcdir)/templatelangpars.h

$(srcdir)/lempar.c: $(top_srcdir)/sqlite3/tool/lempar.c
	cp $(top_srcdir)/sqlite3/tool/lempar.c $@

$(srcdir)/templatelang_parser.o: $(srcdir)/templatelang_parser.cpp


$(srcdir)/emdroslangpars.h: $(srcdir)/emdroslang_parser.cpp

$(srcdir)/emdroslang_parser.cpp: $(srcdir)/emdroslang.yxx  $(srcdir)/lempar.c $(srcdir)/lemon
	cd $(srcdir)/ ; $(LEMON) -s $(srcdir)/emdroslang.yxx
	sed -e 's_yy_tmplgyy_g;s_YY_TMPLGYY_g;s/mql_tmplgyylex.h/mql_yylex.h/g' $(srcdir)/emdroslang.c > $@
	mv $(srcdir)/emdroslang.h $(srcdir)/emdroslangpars.h

$(srcdir)/emdroslang_parser.o: $(srcdir)/emdroslang_parser.cpp


#
# Lexers
#

$(srcdir)/templatelang_lexer.cpp: templatelang.re
	re2c -s -o$(srcdir)/templatelang_lexer.cpp $(srcdir)/templatelang.re

$(srcdir)/emdroslang_lexer.cpp: emdroslang.re
	re2c -s -o$(srcdir)/emdroslang_lexer.cpp $(srcdir)/emdroslang.re



noinst_HEADERS = templatelangpars.h emdroslangpars.h



macdist:
	cp -f .libs/libharvest.a $(MACDISTDIR)/development/lib/
