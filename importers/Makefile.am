# You may need to change these
LEMON = ./lemon

if HOSTISDARWIN
MACLDFLAGS = -static
endif

ARFLAGS = @ARFLAGS@

if DO_DEVEL
MAYBE_LIBS_IMPORTERS = libemdros_importers.la
else
MAYBE_LIBS_IMPORTERS = 
endif

lib_LTLIBRARIES = $(MAYBE_LIBS_IMPORTERS)

libemdros_importers_la_DEPENDENCIES = @HARVEST_DEPS@ @UTIL_DEPS@ @MQL_DEPS@ @EMDF_DEPS@ @PCRE_DEPS@
libemdros_importers_la_LIBADD = @HARVEST_LDADD@ @UTIL_LDADD@ @MQL_LDADD@ @EMDF_LDADD@ @PCRE_LDADD@
libemdros_importers_la_LDFLAGS = $(MACLDFLAGS) \
                            @SUN_LDFLAGS@ \
                            @DARWIN_LDFLAGS@

libemdros_importers_la_SOURCES = \
                            unboundbibleimporter.cpp \
                            negraimporter.cpp tigerxmlimporter.cpp \
                            plaintextimporter.cpp slashedtextimporter.cpp \
                            sfm.cpp sfmimporter.cpp \
                            pennparser.cpp pennlexer.cpp penn_yylex.cpp \
                            pennimporter.cpp 


CLEANFILES = *~ core $(srcdir)/lemon simplebibleschema.h pennschema.h \
             $(srcdir)/pennlexer.cpp \
             $(srcdir)/pennparser.cpp \
             $(srcdir)/pennparser.h \
             $(srcdir)/pennparser.out \
             $(srcdir)/lemon.c $(srcdir)/lempar.c \
             $(srcdir)/negraschema.h \
             $(srcdir)/plaintextschema.h \
             $(srcdir)/slashedtextschema.h

EXTRA_DIST = win32.mak mingw.mak \
             simplebibleschema.h simplebibleschema.mql \
             pennschema.h pennschema.mql pennschema_id_d.mql \
             pennparser.yxx pennparser.cpp pennparser.h \
             pennlexer.re pennlexer.cpp \
             negraschema.h negraschema.mql \
             plaintextschema.h plaintextschema.mql \
             slashedtextschema.h slashedtextschema.mql 

AM_CPPFLAGS =
AM_CXXFLAGS = @DEBUGFLAG@ @ADDITIONAL_CXXFLAGS@ 


BUILT_SOURCES = simplebibleschema.h negraschema.h \
                plaintextschema.h slashedtextschema.h \
                pennparser.cpp pennlexer.cpp pennschema.h 



simplebibleschema.h: simplebibleschema.mql
	rm -f $@
	cat ../include/boilerplate_license.h > $@
	echo "" >> $@
	echo "char simple_bible_schema[] = " >> $@
	cat simplebibleschema.mql | sed -e 's_\\_\\\\_g;s_\"_\\"_g;s_^_"_g;s_$$_\\n"_g' >> $@
	echo ";" >> $@

pennschema.h: pennschema.mql pennschema_id_d.mql
	rm -f $@
	cat ../include/boilerplate_license.h > $@
	echo "" >> $@
	echo "char penn_treebank_schema[] = " >> $@
	cat pennschema.mql | sed -e 's_\\_\\\\_g;s_\"_\\"_g;s_^_"_g;s_$$_\\n"_g' >> $@
	echo ";" >> $@
	echo "" >> $@
	echo "char penn_treebank_id_d_schema[] = " >> $@
	cat pennschema_id_d.mql | sed -e 's_\\_\\\\_g;s_\"_\\"_g;s_^_"_g;s_$$_\\n"_g' >> $@
	echo ";" >> $@

negraschema.h: negraschema.mql
	rm -f $@
	cat ../include/boilerplate_license.h > $@
	echo "" >> $@
	echo "char negra_schema[] = " >> $@
	cat negraschema.mql | sed -e 's_\\_\\\\_g;s_\"_\\"_g;s_^_"_g;s_$$_\\n"_g' >> $@
	echo ";" >> $@


plaintextschema.h: plaintextschema.mql
	rm -f $@
	cat ../include/boilerplate_license.h > $@
	echo "" >> $@
	echo "char plaintext_schema[] = " >> $@
	cat plaintextschema.mql | sed -e 's_\\_\\\\_g;s_\"_\\"_g;s_^_"_g;s_$$_\\n"_g' >> $@
	echo ";" >> $@

slashedtextschema.h: slashedtextschema.mql
	rm -f $@
	cat ../include/boilerplate_license.h > $@
	echo "" >> $@
	echo "char slashedtext_schema[] = " >> $@
	cat slashedtextschema.mql | sed -e 's_\\_\\\\_g;s_\"_\\"_g;s_^_"_g;s_$$_\\n"_g' >> $@
	echo ";" >> $@


# lemon
$(srcdir)/lemon.c: $(top_srcdir)/sqlite3/tool/lemon.c
	cp $(top_srcdir)/sqlite3/tool/lemon.c $(srcdir)/

$(srcdir)/lempar.c: $(top_srcdir)/sqlite3/tool/lempar.c
	cp $(top_srcdir)/sqlite3/tool/lempar.c $(srcdir)/

$(srcdir)/lemon: $(srcdir)/lemon.c
	cd $(srcdir) ; $(CC) -o lemon lemon.c


#
# Parser
#

$(srcdir)/pennparser.h: $(srcdir)/pennparser.cpp

$(srcdir)/pennparser.cpp: $(srcdir)/pennparser.yxx $(srcdir)/lemon.c $(srcdir)/lempar.c $(srcdir)/lemon
	cd $(srcdir)/ ; $(LEMON) -s $(srcdir)/pennparser.yxx
	mv $(srcdir)/pennparser.c $(srcdir)/pennparser.cpp

$(srcdir)/pennparser.o: $(srcdir)/pennparser.cpp



#
# Lexers
#

$(srcdir)/pennlexer.cpp: pennlexer.re
	re2c -s -o$@ pennlexer.re


noinst_HEADERS = $(srcdir)/pennparser.h \
                 $(srcdir)/pennschema.h \
                 $(srcdir)/negraschema.h \
                 $(srcdir)/plaintextschema.h \
                 $(srcdir)/slashedtextschema.h 


macdist:
	cp -f .libs/libemdros_importers.a $(MACDISTDIR)/development/lib/
