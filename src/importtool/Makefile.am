AM_CXXFLAGS = @CXXFLAGS@ @DEBUGFLAG@ @SUN_CXXFLAGS@ @DARWIN_CXXFLAGS@  \
              @ADDITIONAL_CXXFLAGS@ -DTARGET_RT_BIG_ENDIAN=@BIG_ENDIAN@ 

SUBDIRS = doc

if HOSTISDARWIN
MACLDFLAGS = -static
endif


if WITH_WX_PROGRAMS

if WX_DO_UNICODE
MAYBE_UNICODE_PRGS = importtoolu
else
MAYBE_UNICODE_PRGS =
endif

if WX_DO_NONUNICODE
MAYBE_NONUNICODE_PRGS = importtool
else
MAYBE_NONUNICODE_PRGS =
endif

else
MAYBE_PRGS = 
endif

EXTRA_PROGRAMS = importtool importtoolu

bin_PROGRAMS = $(MAYBE_NONUNICODE_PRGS) $(MAYBE_UNICODE_PRGS) 

ALWAYSLDADD = \
              -L$(top_builddir)/wx -lemdrosguiu \
              @EMDROS_LDADD_BACKBACK@ \
              @SUN_LDFLAGS@  @DARWIN_LDFLAGS@

ALWAYSDEPS = @EMDROS_DEPS_BACKBACK@



importtool_LDADD = $(top_builddir)/wx/libemdrosgui.a $(ALWAYSLDADD) \
            @WX_NONUNICODE_LDFLAGS@ @WX_NONUNICODE_LIBS@
importtool_LDFLAGS = $(MACLDFLAGS)
importtool_DEPENDENCIES = $(top_builddir)/wx/libemdrosgui.a $(ALWAYSDEPS) 
importtool_CXXFLAGS = $(AM_CXXFLAGS) @WX_NONUNICODE_CXXFLAGS@

importtoolu_LDADD = \
             $(top_builddir)/wx/libemdrosguiu.a $(ALWAYSLDADD) \
             @WX_UNICODE_LDFLAGS@ @WX_UNICODE_LIBS@
importtoolu_LDFLAGS = $(MACLDFLAGS)
importtoolu_DEPENDENCIES = $(top_builddir)/wx/libemdrosguiu.a $(ALWAYSDEPS)
importtoolu_CXXFLAGS = $(AM_CXXFLAGS) -DwxUSE_UNICODE=1 @WX_UNICODE_CXXFLAGS@

importtool_SOURCES = importtool.cpp
importtoolu_SOURCES = importtool.cpp


noinst_HEADERS = importtool.h

AM_CPPFLAGS = -I$(top_srcdir)/include -I. -I$(top_srcdir)/wx -I$(top_srcdir)/amalgamation -I$(top_srcdir)/
CLEANFILES = *~ core config.log Info.plist

IMPORTTOOLDATA = doc/ImportToolUsersGuide.htb

dist_pkgdata_DATA = emergence-importtool.desktop

EXTRA_DIST = windows.mak mingw.mak importtool.rc \
             Info.plist.in  EmdrosImportTool.r \
             emergence-importtool.desktop


#
# NOTE: If you change this directory, then change it in 
# doc/Makefile.am as well!
#
importtooldir = $(prefix)/share/emdros/importtool

# This is only here, however
dist_importtool_DATA = $(IMPORTTOOLDATA)

BUILT_SOURCES = Info.plist

if HOSTISDARWIN
if WITH_WX_PROGRAMS
maybe_all_local = RezSetFile EmdrosImportTool.app
__macosx__rez = $(WX_REZFLAGS) importtoolu -s $(top_srcdir)/wx EmdrosImportTool.r
__macosx__setfile = 
maybe_distclean_local = MacOSX_distclean_local
else
maybe_all_local = 
__macosx__rez = 
__macosx__setfile = 
maybe_distclean_local = 
endif
else
maybe_all_local = 
__macosx__rez = 
__macosx__setfile = 
maybe_distclean_local = 
endif

all-local: $(maybe_all_local)


distclean-local: $(maybe_distclean_local)


MacOSX_distclean_local:
	rm -rf EmdrosImportTool.app

doc/ImportToolUsersGuide.htb:
	cd doc ; make ; cd ..

EmdrosImportTool.app: Info.plist importtoolu $(IMPORTTOOLDATA) $(top_srcdir)/wx/blue-E.icns
	rm -rf EmdrosImportTool.app
	-mkdir EmdrosImportTool.app
	-mkdir EmdrosImportTool.app/Contents
	-mkdir EmdrosImportTool.app/Contents/MacOS
	-mkdir EmdrosImportTool.app/Contents/Resources
	-mkdir -p EmdrosImportTool.app/Contents/share/emdros/importtool
	cp $(IMPORTTOOLDATA) EmdrosImportTool.app/Contents/share/emdros/importtool/
	cp -f $(top_srcdir)/wx/blue-E.icns EmdrosImportTool.app/Contents/Resources/blue-E.icns
	cp Info.plist EmdrosImportTool.app/Contents
	echo -n 'APPL????' > EmdrosImportTool.app/Contents/PkgInfo
	cp -f importtoolu EmdrosImportTool.app/Contents/MacOS/importtoolu

Info.plist: Info.plist.in
	$(top_builddir)/config.status --file=$(srcdir)/Info.plist

RezSetFile: importtoolu EmdrosImportTool.r
	$(__macosx__rez)
	$(__macosx__setfile)
