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

else
MAYBE_PRGS = 
endif

EXTRA_PROGRAMS = importtoolu

bin_PROGRAMS = $(MAYBE_UNICODE_PRGS) 

ALWAYSLDADD = \
              @EMDROS_LDADD_BACKBACK@ \
              @SUN_LDFLAGS@  @DARWIN_LDFLAGS@

ALWAYSDEPS = @EMDROS_DEPS_BACKBACK@



importtoolu_LDADD = \
                    -L$(top_builddir)/wx -lemdrosguiu $(ALWAYSLDADD) \
                    @WX_UNICODE_LDFLAGS@ @WX_UNICODE_LIBS@
importtoolu_LDFLAGS = $(MACLDFLAGS)
importtoolu_DEPENDENCIES = $(top_builddir)/wx/libemdrosguiu.a $(ALWAYSDEPS)
importtoolu_CXXFLAGS = $(AM_CXXFLAGS) -DwxUSE_UNICODE=1 @WX_UNICODE_CXXFLAGS@

importtoolu_SOURCES = importtool.cpp


noinst_HEADERS = importtool.h

AM_CPPFLAGS = -I$(top_srcdir)/include -I. -I$(top_srcdir)/wx -I$(top_srcdir)/amalgamation -I$(top_srcdir)/
CLEANFILES = *~ core config.log Info.plist

IMPORTTOOLDATA = doc/ImportToolUsersGuide.htb

desktopdatadir = $(datadir)/applications
dist_desktopdata_DATA = scripturesys-importtool.desktop 


EXTRA_DIST = win32.mak mingw.mak importtool.rc \
             Info.plist.in  \
             scripturesys-importtool.desktop


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
maybe_all_local = EmdrosImportTool.app
maybe_distclean_local = MacOSX_distclean_local
else
maybe_all_local = 
maybe_distclean_local = 
endif
else
maybe_all_local = 
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

