SUBDIRS = qrytool importtool

if HOSTISDARWIN
MACLDFLAGS = -static 
endif

bin_PROGRAMS = mql mqldump upgrade_db manage_indices \
               agexport ubimport pennimport negraimport tigerxmlimport \
               plaintextimport slashedtextimport sfmimport \
               renderobjects fts_indexer fts_harvester bptdump \
               bptdetach

mql_LDADD = $(top_builddir)/util/libutil_emdros.la @EMDROS_LDADD@ @SUN_LDFLAGS@ @DARWIN_LDFLAGS@ @ZLIB_LIBADD@ 
mqldump_LDADD =  $(top_builddir)/util/libutil_emdros.la @EMDROS_LDADD@ @SUN_LDFLAGS@ @DARWIN_LDFLAGS@ @ZLIB_LIBADD@ 
upgrade_db_LDADD = $(top_builddir)/util/libutil_emdros.la @EMDROS_LDADD@ @SUN_LDFLAGS@ @DARWIN_LDFLAGS@ @ZLIB_LIBADD@ 
manage_indices_LDADD = $(top_builddir)/util/libutil_emdros.la @EMDFLDADD@ @SUN_LDFLAGS@ @DARWIN_LDFLAGS@ @ZLIB_LIBADD@ 
agexport_LDADD =  $(top_builddir)/util/libutil_emdros.la @EMDROS_LDADD@ @SUN_LDFLAGS@ @DARWIN_LDFLAGS@ @ZLIB_LIBADD@ 
ubimport_LDADD =  $(top_builddir)/util/libutil_emdros.la @EMDROS_LDADD@ @SUN_LDFLAGS@ @DARWIN_LDFLAGS@ @ZLIB_LIBADD@ 
pennimport_LDADD =  $(top_builddir)/util/libutil_emdros.la @EMDROS_LDADD@ @SUN_LDFLAGS@ @DARWIN_LDFLAGS@ @ZLIB_LIBADD@ 
negraimport_LDADD =  $(top_builddir)/util/libutil_emdros.la @EMDROS_LDADD@ @SUN_LDFLAGS@ @DARWIN_LDFLAGS@ @ZLIB_LIBADD@ 
tigerxmlimport_LDADD =  $(top_builddir)/util/libutil_emdros.la @EMDROS_LDADD@ @SUN_LDFLAGS@ @DARWIN_LDFLAGS@ @ZLIB_LIBADD@ 
plaintextimport_LDADD =  $(top_builddir)/util/libutil_emdros.la @EMDROS_LDADD@ @SUN_LDFLAGS@ @DARWIN_LDFLAGS@ @ZLIB_LIBADD@ 
slashedtextimport_LDADD =  $(top_builddir)/util/libutil_emdros.la @EMDROS_LDADD@ @SUN_LDFLAGS@ @DARWIN_LDFLAGS@ @ZLIB_LIBADD@ 
sfmimport_LDADD =  $(top_builddir)/util/libutil_emdros.la @EMDROS_LDADD@ @SUN_LDFLAGS@ @DARWIN_LDFLAGS@ @ZLIB_LIBADD@ 
renderobjects_LDADD = $(top_builddir)/util/libutil_emdros.la @EMDROS_LDADD@ @SUN_LDFLAGS@ @DARWIN_LDFLAGS@ @ZLIB_LIBADD@ 
fts_indexer_LDADD = $(top_builddir)/util/libutil_emdros.la @EMDROS_LDADD@ @SUN_LDFLAGS@ @DARWIN_LDFLAGS@ @ZLIB_LIBADD@ 
fts_harvester_LDADD = $(top_builddir)/util/libutil_emdros.la @EMDROS_LDADD@ @SUN_LDFLAGS@ @DARWIN_LDFLAGS@ @ZLIB_LIBADD@ 
bptdump_LDADD = $(top_builddir)/util/libutil_emdros.la @EMDROS_LDADD@ @SUN_LDFLAGS@ @DARWIN_LDFLAGS@ @ZLIB_LIBADD@ 
bptdetach_LDADD = $(top_builddir)/util/libutil_emdros.la @EMDROS_LDADD@ @SUN_LDFLAGS@ @DARWIN_LDFLAGS@ @ZLIB_LIBADD@ 


mql_LDFLAGS = $(MACLDFLAGS)
mqldump_LDFLAGS = $(MACLDFLAGS)
upgrade_db_LDFLAGS = $(MACLDFLAGS)
manage_indices_LDFLAGS = $(MACLDFLAGS)
agexport_LDFLAGS = $(MACLDFLAGS)
ubimport_LDFLAGS = $(MACLDFLAGS)
pennimport_LDFLAGS = $(MACLDFLAGS)
negraimport_LDFLAGS = $(MACLDFLAGS)
tigerxmlimport_LDFLAGS = $(MACLDFLAGS)
plaintextimport_LDFLAGS = $(MACLDFLAGS)
slashedtextimport_LDFLAGS = $(MACLDFLAGS)
sfmimport_LDFLAGS = $(MACLDFLAGS)
renderobjects_LDFLAGS = $(MACLDFLAGS)
fts_indexer_LDFLAGS = $(MACLDFLAGS)
fts_harvester_LDFLAGS = $(MACLDFLAGS)
bptdump_LDFLAGS = $(MACLDFLAGS)
bptdetach_LDFLAGS = $(MACLDFLAGS)


mql_DEPENDENCIES = @EMDROS_DEPS@ @ZLIB_DEP@ \
                   $(top_builddir)/util/libutil_emdros.la 
mqldump_DEPENDENCIES = @EMDROS_DEPS@ @ZLIB_DEP@ \
                       $(top_builddir)/util/libutil_emdros.la 
upgrade_db_DEPENDENCIES = @EMDROS_DEPS@ @ZLIB_DEP@ \
                   $(top_builddir)/util/libutil_emdros.la 
manage_indices_DEPENDENCIES = @EMDFDEPS@ \
                   $(top_builddir)/util/libutil_emdros.la 
agexport_DEPENDENCIES = @EMDROS_DEPS@ @ZLIB_DEP@ \
                        $(top_builddir)/util/libutil_emdros.la 
ubimport_DEPENDENCIES = @EMDROS_DEPS@ @ZLIB_DEP@ \
                        $(top_builddir)/util/libutil_emdros.la 
pennimport_DEPENDENCIES = @EMDROS_DEPS@ @ZLIB_DEP@ \
                        $(top_builddir)/util/libutil_emdros.la
negraimport_DEPENDENCIES = @EMDROS_DEPS@ @ZLIB_DEP@ \
                        $(top_builddir)/util/libutil_emdros.la 
tigerxmlimport_DEPENDENCIES = @EMDROS_DEPS@ @ZLIB_DEP@ \
                        $(top_builddir)/util/libutil_emdros.la 
plaintextimport_DEPENDENCIES = @EMDROS_DEPS@ @ZLIB_DEP@ \
                               $(top_builddir)/util/libutil_emdros.la 
slashedtextimport_DEPENDENCIES = @EMDROS_DEPS@ @ZLIB_DEP@ \
                                 $(top_builddir)/util/libutil_emdros.la 
sfmimport_DEPENDENCIES = @EMDROS_DEPS@ @ZLIB_DEP@ \
                                 $(top_builddir)/util/libutil_emdros.la
renderobjects_DEPENDENCIES = @EMDROS_DEPS@ @ZLIB_DEP@ \
                                 $(top_builddir)/util/libutil_emdros.la	
fts_indexer_DEPENDENCIES = @EMDROS_DEPS@ @ZLIB_DEP@ \
                                 $(top_builddir)/util/libutil_emdros.la	
fts_harvester_DEPENDENCIES = @EMDROS_DEPS@ @ZLIB_DEP@ \
                                 $(top_builddir)/util/libutil_emdros.la	
bptdump_DEPENDENCIES = @EMDROS_DEPS@ @ZLIB_DEP@ \
                                 $(top_builddir)/util/libutil_emdros.la
bptdetach_DEPENDENCIES = @EMDROS_DEPS@ @ZLIB_DEP@ \
                                 $(top_builddir)/util/libutil_emdros.la



mql_SOURCES = mql.cpp 

mqldump_SOURCES = mqldump.cpp 

upgrade_db_SOURCES = upgrade_db.cpp

manage_indices_SOURCES = manage_indices.cpp

agexport_SOURCES = agexport.cpp 

ubimport_SOURCES = ubimport.cpp

pennimport_SOURCES = pennimport.cpp

negraimport_SOURCES = negraimport.cpp

tigerxmlimport_SOURCES = tigerxmlimport.cpp

plaintextimport_SOURCES = plaintextimport.cpp

slashedtextimport_SOURCES = slashedtextimport.cpp

sfmimport_SOURCES = sfmimport.cpp

renderobjects_SOURCES = renderobjectstool.cpp

fts_indexer_SOURCES = fts_indexer.cpp

fts_harvester_SOURCES = fts_harvester.cpp

bptdump_SOURCES = bptdump.cpp

bptdetach_SOURCES = bptdetach.cpp


AM_CPPFLAGS = -I$(srcdir)/../include
EXTRA_DIST = win32.mak mingw.mak djgpp.mak \
             src.pro common.pro mql.pro mqldump.pro bptdump.pro \
             renderobjects.pro fts_indexer.pro fts_harvester.pro 

CLEANFILES = *~ core 
AM_CXXFLAGS = @CXXFLAGS@ @DEBUGFLAG@ @SUN_CXXFLAGS@ @DARWIN_CXXFLAGS@ \
              @ADDITIONAL_CXXFLAGS@ 



macdist:
	cp $(bin_PROGRAMS) $(MACDISTDIR)/development/bin
	cd qrytool; make macdist MACDISTDIR=../../$(MACDISTDIR); cd ..

