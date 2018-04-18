SUBDIRS = doc

AM_CXXFLAGS = @CXXFLAGS@ @DEBUGFLAG@ @SUN_CXXFLAGS@ @DARWIN_CXXFLAGS@  \
              @ADDITIONAL_CXXFLAGS@ -DTARGET_RT_BIG_ENDIAN=@BIG_ENDIAN@ 

if HOSTISDARWIN
MACLDFLAGS = -static
endif


CHUNKINGTOOLDATA = tisch.cfg wihebrew.cfg doc/ChunkingToolGuide.htb

if WITH_WX_PROGRAMS

if WX_DO_UNICODE
MAYBE_UNICODE_PRGS = chunkingtoolu
else
MAYBE_UNICODE_PRGS =
endif

if WX_DO_NONUNICODE
MAYBE_NONUNICODE_PRGS = chunkingtool
else
MAYBE_NONUNICODE_PRGS =
endif

else
MAYBE_PRGS = 
endif

EXTRA_PROGRAMS = chunkingtool chunkingtoolu

bin_PROGRAMS = $(MAYBE_NONUNICODE_PRGS) $(MAYBE_UNICODE_PRGS) 

ALWAYSLDADD = $(top_builddir)/src/qrytool/TECkit/libteckit.la -lz \
              $(top_builddir)/util/libutil_emdros.la \
              $(top_builddir)/MQL/libmql.la @EMDFLDADDSWIG@ \
              @PCRE_LIBADD_SWIG@ @ZLIB_LIBADD_SWIG@ \
              @SUN_LDFLAGS@  @DARWIN_LDFLAGS@

ALWAYSDEPS = \
             $(top_builddir)/src/qrytool/TECkit/libteckit.la \
             $(top_builddir)/util/libutil_emdros.la \
             $(top_builddir)/MQL/libmql.la \
              @EMDFDEPSSWIG@ @PCRE_DEP_SWIG@ @ZLIB_DEP_SWIG@


chunkingtool_LDADD = $(top_builddir)/wx/libemdrosgui.a $(ALWAYSLDADD) \
            @WX_NONUNICODE_LDFLAGS@ @WX_NONUNICODE_LIBS@
chunkingtool_LDFLAGS = $(MACLDFLAGS)
chunkingtool_DEPENDENCIES = $(ALWAYSDEPS) \
                   $(top_builddir)/wx/libemdrosgui.a
chunkingtool_CXXFLAGS = $(AM_CXXFLAGS) @WX_NONUNICODE_CXXFLAGS@

chunkingtoolu_LDADD = \
             $(top_builddir)/wx/libemdrosguiu.a $(ALWAYSLDADD) \
             @WX_UNICODE_LDFLAGS@ @WX_UNICODE_LIBS@
chunkingtoolu_LDFLAGS = $(MACLDFLAGS)
chunkingtoolu_DEPENDENCIES = $(ALWAYSDEPS) \
                   $(top_builddir)/wx/libemdrosguiu.a
chunkingtoolu_CXXFLAGS = $(AM_CXXFLAGS) -DwxUSE_UNICODE=1 @WX_UNICODE_CXXFLAGS@

chunkingtool_SOURCES = ctwx.cpp ctmf.cpp ctdoc.cpp
chunkingtoolu_SOURCES = ctwx.cpp ctmf.cpp ctdoc.cpp


noinst_HEADERS = ctmf.h ctwx.h ctdoc.h

AM_CPPFLAGS = -I$(top_srcdir)/include -I. -I$(top_srcdir)/src/qrytool -I$(top_srcdir)/wx
CLEANFILES = *~ core config.log Info.plist

EXTRA_DIST = win32.mak mingw.mak ctwx.rc \
             Info.plist.in  EmdrosChunkingTool.r \
             emergence-chunkingtool.desktop

dist_pkgdata_DATA = emergence-chunkingtool.desktop

#
# NOTE: If you change this directory, then change it in 
# doc/Makefile.am as well!
#
chunkingtooldir = $(prefix)/share/emdros/chunkingtool

# This is only here, however
dist_chunkingtool_DATA = $(CHUNKINGTOOLDATA)

BUILT_SOURCES = Info.plist

if HOSTISDARWIN
if WITH_WX_PROGRAMS
maybe_all_local = RezSetFile EmdrosChunkingTool.app
__macosx__rez = $(WX_REZFLAGS) chunkingtoolu -s $(top_srcdir)/wx EmdrosChunkingTool.r
#__macosx__setfile = SetFile -a C chunkingtoolu
__macosx__setfile = 
maybe_distclean_local = MacOSX_distclean_local
else
maybe_all_local = 
__macosx__rez = 
maybe_distclean_local = 
__macosx__setfile = 
endif
else
maybe_all_local = 
__macosx__rez = 
maybe_distclean_local = 
__macosx__setfile = 
endif

all-local: $(maybe_all_local)


distclean-local: $(maybe_distclean_local)


MacOSX_distclean_local:
	rm -rf EmdrosChunkingTool.app

EmdrosChunkingTool.app: Info.plist chunkingtoolu $(CHUNKINGTOOLDATA) $(top_srcdir)/wx/blue-E.icns
	rm -rf EmdrosChunkingTool.app
	-mkdir EmdrosChunkingTool.app
	-mkdir EmdrosChunkingTool.app/Contents
	-mkdir EmdrosChunkingTool.app/Contents/MacOS
	-mkdir EmdrosChunkingTool.app/Contents/Resources
	-mkdir -p EmdrosChunkingTool.app/Contents/share/emdros/chunkingtool
	cp $(CHUNKINGTOOLDATA) EmdrosChunkingTool.app/Contents/share/emdros/chunkingtool/
	cp -f $(top_srcdir)/wx/blue-E.icns EmdrosChunkingTool.app/Contents/Resources/blue-E.icns
	cp Info.plist EmdrosChunkingTool.app/Contents
	echo -n 'APPL????' > EmdrosChunkingTool.app/Contents/PkgInfo
	cp -f chunkingtoolu EmdrosChunkingTool.app/Contents/MacOS/chunkingtoolu

Info.plist: Info.plist.in
	$(top_builddir)/config.status --file=$(srcdir)/Info.plist

RezSetFile: chunkingtoolu EmdrosChunkingTool.r
	$(__macosx__rez)
	$(__macosx__setfile)

doc/ChunkingToolGuide.htb:
	cd doc ; make ; cd ..
