AM_CXXFLAGS = @CXXFLAGS@ @DEBUGFLAG@ @SUN_CXXFLAGS@ @DARWIN_CXXFLAGS@  \
              @ADDITIONAL_CXXFLAGS@ -DTARGET_RT_BIG_ENDIAN=@BIG_ENDIAN@ 

ARFLAGS = @ARFLAGS@

SUBDIRS = doc TECkit

EXTRA_PROGRAMS = eqt eqtu

if WITH_WX_PROGRAMS

if WX_DO_UNICODE
MAYBE_UNICODE_PRGS = eqtu
else
MAYBE_UNICODE_PRGS =
endif

if WX_DO_NONUNICODE
MAYBE_NONUNICODE_PRGS = eqt
else
MAYBE_NONUNICODE_PRGS =
endif

else
MAYBE_PRGS = 
endif

if HOSTISDARWIN
MACLDFLAGS = -static
if WITH_WX_PROGRAMS
maybe_all_local = RezSetFile EmdrosQueryTool.app
__macosx__rez = $(WX_REZFLAGS) eqtu -s $(top_srcdir)/wx EmdrosQueryTool.r
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



bin_PROGRAMS = eqtc $(MAYBE_NONUNICODE_PRGS) $(MAYBE_UNICODE_PRGS) 
noinst_LIBRARIES = libquerytool.a 


ALWAYSLDADD = -L$(top_builddir)/src/qrytool -lquerytool \
              $(top_builddir)/src/qrytool/TECkit/libteckit.la \
              $(top_builddir)/util/libutil_emdros.la \
              @EMDROS_LDADD_SWIG@ \
              @ZLIB_LIBADD_SWIG@ \
              @SUN_LDFLAGS@  @DARWIN_LDFLAGS@

ALWAYSDEPS = libquerytool.a  \
             $(top_builddir)/src/qrytool/TECkit/libteckit.la \
             $(top_builddir)/util/libutil_emdros.la \
             @EMDROS_DEPS_SWIG@ @ZLIB_DEP_SWIG@


eqtc_LDADD = $(ALWAYSLDADD)
eqtc_DEPENDENCIES = $(ALWAYSDEPS)
eqtc_LDFLAGS = $(MACLDFLAGS) 

eqt_LDADD = $(top_builddir)/wx/libemdrosgui.a $(ALWAYSLDADD) \
            @WX_NONUNICODE_LDFLAGS@ @WX_NONUNICODE_LIBS@
eqt_LDFLAGS = $(MACLDFLAGS) 
eqt_DEPENDENCIES = $(ALWAYSDEPS) $(top_builddir)/wx/libemdrosgui.a
eqt_CXXFLAGS = $(AM_CXXFLAGS) @WX_NONUNICODE_CXXFLAGS@

eqtu_LDADD = $(top_builddir)/wx/libemdrosguiu.a $(ALWAYSLDADD) \
             @WX_UNICODE_LDFLAGS@ @WX_UNICODE_LIBS@
eqtu_LDFLAGS = $(MACLDFLAGS) 
eqtu_DEPENDENCIES = $(ALWAYSDEPS) $(top_builddir)/wx/libemdrosguiu.a
eqtu_CXXFLAGS = $(AM_CXXFLAGS) -DwxUSE_UNICODE=1 @WX_UNICODE_CXXFLAGS@

libquerytool_a_SOURCES = solution.cpp mytable.cpp \
                         harvest.cpp output_style.cpp emdros_wrapper.cpp \
                         exec.cpp consout.cpp qtoutput.cpp
if HOSTISSUNCC
libquerytool_a_AR = $(CXX) -xar -o
else
libquerytool_a_AR = $(AR) $(ARFLAGS)
endif

eqtc_SOURCES = mqlqt.cpp
eqt_SOURCES = mqlqtwx.cpp qtconnwizard.cpp mainframe.cpp wxilqtout.cpp
eqtu_SOURCES = mqlqtwx.cpp qtconnwizard.cpp mainframe.cpp wxilqtout.cpp 


noinst_HEADERS = solution.h mytable.h exec.h \
                 harvest.h output_style.h emdros_wrapper.h \
                 mainframe.h mqlqtwx.h \
                 qtoutput.h consout.h wxilqtout.h qtconnwizard.h

AM_CPPFLAGS = -I$(top_srcdir)/include -I$(top_srcdir)/wx
CLEANFILES = *~ core config.log EQTUsersGuide.log

QRYTOOLDATA = av.cfg default.cfg penn.cfg \
              wihebrew.cfg wihebrew_manylines.cfg wihebrew_new.cfg \
              bhs4_trees.cfg bhs4_bracketed.cfg \
              Amsterdam.map \
              wh.cfg whutf8.cfg whwordsonly.cfg whwordsonlyutf8.cfg \
              doc/EQTUsersGuide.htb

EXTRA_DIST = win32.mak mingw.mak djgpp.mak \
             av.cfg default.cfg wihebrew.cfg wh.cfg \
             mqlqtwx.rc \
             Amsterdam.map \
             Info.plist.in EmdrosQueryTool.r \
             emergence-eqtu.desktop

DISTCLEANFILES = Info.plist

dist_pkgdata_DATA = emergence-eqtu.desktop


#
# NOTE: If you change this directory, then change it in 
# doc/Makefile.am as well!
#
qrytooldir = $(prefix)/share/emdros/qrytool

# This is only here, however
dist_qrytool_DATA = $(QRYTOOLDATA)

BUILT_SOURCES = Info.plist

all-local: $(maybe_all_local)


distclean-local: $(maybe_distclean_local)


MacOSX_distclean_local:
	rm -rf EmdrosQueryTool.app

doc/EQTUsersGuide.htb:
	cd doc ; make ; cd ..

EmdrosQueryTool.app: Info.plist eqtu $(QRYTOOLDATA) $(top_srcdir)/wx/blue-E.icns
	rm -rf EmdrosQueryTool.app
	-mkdir EmdrosQueryTool.app
	-mkdir EmdrosQueryTool.app/Contents
	-mkdir EmdrosQueryTool.app/Contents/MacOS
	-mkdir EmdrosQueryTool.app/Contents/Resources
	-mkdir -p EmdrosQueryTool.app/Contents/share/emdros/qrytool
	cp -f $(QRYTOOLDATA) EmdrosQueryTool.app/Contents/share/emdros/qrytool/
	cp -f $(top_srcdir)/wx/blue-E.icns EmdrosQueryTool.app/Contents/Resources/blue-E.icns
	cp -f Info.plist EmdrosQueryTool.app/Contents
	echo -n 'APPL????' > EmdrosQueryTool.app/Contents/PkgInfo
	cp -f eqtu EmdrosQueryTool.app/Contents/MacOS/eqtu

Info.plist: Info.plist.in
	$(top_builddir)/config.status --file=$(srcdir)/Info.plist

RezSetFile: eqtu  EmdrosQueryTool.r Info.plist
	$(__macosx__rez)
	$(__macosx__setfile)


macdist:
	cp eqtc $(MACDISTDIR)/bin
