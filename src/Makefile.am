SUBDIRS = qrytool importtool

if HOSTISDARWIN
MACLDFLAGS = -static 
endif

bin_PROGRAMS = mql mqldump upgrade_db manage_indices \
               ubimport pennimport negraimport tigerxmlimport \
               plaintextimport slashedtextimport sfmimport \
               renderobjects fts_indexer fts_harvester bptdump \
               bptdetach


ALWAYSLDADD = @EMDROS_LDADD@ \
              @ZLIB_LDADD@ \
              @SUN_LDFLAGS@  @DARWIN_LDFLAGS@

ALWAYSDEPS = @EMDROS_DEPS@ @ZLIB_DEPS@

mql_LDADD = $(ALWAYSLDADD)
mqldump_LDADD =  $(ALWAYSLDADD)
upgrade_db_LDADD = $(ALWAYSLDADD)
manage_indices_LDADD = $(ALWAYSLDADD)
ubimport_LDADD =  $(ALWAYSLDADD)
pennimport_LDADD =  $(ALWAYSLDADD)
negraimport_LDADD =  $(ALWAYSLDADD)
tigerxmlimport_LDADD =  $(ALWAYSLDADD)
plaintextimport_LDADD =  $(ALWAYSLDADD)
slashedtextimport_LDADD =  $(ALWAYSLDADD)
sfmimport_LDADD =  $(ALWAYSLDADD)
renderobjects_LDADD = $(ALWAYSLDADD)
fts_indexer_LDADD = $(ALWAYSLDADD)
fts_harvester_LDADD = $(ALWAYSLDADD)
bptdump_LDADD = $(ALWAYSLDADD)
bptdetach_LDADD = $(ALWAYSLDADD)


mql_LDFLAGS = $(MACLDFLAGS)
mqldump_LDFLAGS = $(MACLDFLAGS)
upgrade_db_LDFLAGS = $(MACLDFLAGS)
manage_indices_LDFLAGS = $(MACLDFLAGS)
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


mql_DEPENDENCIES = $(ALWAYSDEPS)
mqldump_DEPENDENCIES = $(ALWAYSDEPS)
upgrade_db_DEPENDENCIES = $(ALWAYSDEPS)
manage_indices_DEPENDENCIES = $(ALWAYSDEPS)
ubimport_DEPENDENCIES = $(ALWAYSDEPS)
pennimport_DEPENDENCIES = $(ALWAYSDEPS)
negraimport_DEPENDENCIES = $(ALWAYSDEPS)
tigerxmlimport_DEPENDENCIES = $(ALWAYSDEPS)
plaintextimport_DEPENDENCIES = $(ALWAYSDEPS)
slashedtextimport_DEPENDENCIES = $(ALWAYSDEPS)
sfmimport_DEPENDENCIES = $(ALWAYSDEPS)
renderobjects_DEPENDENCIES = $(ALWAYSDEPS)	
fts_indexer_DEPENDENCIES = $(ALWAYSDEPS)	
fts_harvester_DEPENDENCIES = $(ALWAYSDEPS)	
bptdump_DEPENDENCIES = $(ALWAYSDEPS)
bptdetach_DEPENDENCIES = $(ALWAYSDEPS)



mql_SOURCES = mql.cpp 

mqldump_SOURCES = mqldump.cpp 

upgrade_db_SOURCES = upgrade_db.cpp

manage_indices_SOURCES = manage_indices.cpp

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
EXTRA_DIST = win32.mak mingw.mak 

CLEANFILES = *~ core 
AM_CXXFLAGS = @CXXFLAGS@ @DEBUGFLAG@ @SUN_CXXFLAGS@ @DARWIN_CXXFLAGS@ \
              @ADDITIONAL_CXXFLAGS@ 



macdist:
	cp $(bin_PROGRAMS) $(MACDISTDIR)/development/bin
	make -C qrytool macdist MACDISTDIR=../$(MACDISTDIR)

