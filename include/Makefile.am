EXTRA_DIST = emdros-lconfig.h.in emdros-config.h.in \
             Makefile.inc windows.mak mingw.mak \
             emdros-version.h.in

include Makefile.inc

if USE_LOCAL_ZLIB
PACKAGE_INCLUDE_HEADER_FILES_NODIST = zconf.h
endif



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
       zlib_emdros.h \
       version-emdros.h \
       sqlite3_emdros.h 

#
BUILT_SOURCES = version-emdros.h 
CLEANFILES = *~  core 

DISTCLEANFILES = emdros-config.h.in



if USE_LOCAL_ZLIB
zconf.h: $(srcdir)/../zlib/zconf.h
	cp $(srcdir)/../zlib/zconf.h $@

endif



version-emdros.h: emdros-version.h.in
	$(top_builddir)/config.status --file=$(srcdir)/emdros-version.h
	mv $(srcdir)/emdros-version.h $(srcdir)/version-emdros.h


macdist:
	cp -f $(PACKAGE_INCLUDE_HEADER_FILES) $(MACDISTDIR)/development/include/

