AM_CXXFLAGS = @CXXFLAGS@ @DEBUGFLAG@ @SUN_CXXFLAGS@ @DARWIN_CXXFLAGS@  \
              @ADDITIONAL_CXXFLAGS@ 

ARFLAGS = @ARFLAGS@

SUBDIRS = doc

EXTRA_PROGRAMS = eqtu


if WITH_WX_PROGRAMS

if WX_DO_UNICODE
MAYBE_UNICODE_PRGS = eqtu
else
MAYBE_UNICODE_PRGS =
endif

else
MAYBE_PRGS = 
endif

if HOSTISDARWIN
MACLDFLAGS = -static
if WITH_WX_PROGRAMS
maybe_all_local = EmdrosQueryTool.app
maybe_distclean_local = MacOSX_distclean_local
else
maybe_all_local = 
maybe_distclean_local = 
endif
else
maybe_all_local = 
maybe_distclean_local = 
endif



bin_PROGRAMS = eqtc $(MAYBE_UNICODE_PRGS) 
noinst_LIBRARIES = libquerytool.a 


ALWAYSLDADD = -L$(top_builddir)/src/qrytool -lquerytool \
              @EMDROS_LDADD_BACKBACK@ \
              @SUN_LDFLAGS@  @DARWIN_LDFLAGS@

ALWAYSDEPS = libquerytool.a  @EMDROS_DEPS_BACKBACK@


eqtc_LDADD = $(ALWAYSLDADD)
eqtc_DEPENDENCIES = $(ALWAYSDEPS)
eqtc_LDFLAGS = $(MACLDFLAGS) 



eqtu_LDADD = -L$(top_builddir)/wx -lemdrosguiu \
             $(ALWAYSLDADD) \
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
eqtu_SOURCES = mqlqtwx.cpp qtconnwizard.cpp mainframe.cpp wxilqtout.cpp 


noinst_HEADERS = solution.h mytable.h exec.h \
                 harvest.h output_style.h emdros_wrapper.h \
                 mainframe.h mqlqtwx.h \
                 qtoutput.h consout.h wxilqtout.h qtconnwizard.h

AM_CPPFLAGS = -I$(top_srcdir)/amalgamation \
              -I$(top_srcdir)/wx \
              -I$(top_srcdir)/include \
              -I$(top_srcdir)/ 

CLEANFILES = *~ core config.log EQTUsersGuide.log

QRYTOOLDATA = av.cfg default.cfg penn.cfg \
              wihebrew.cfg wihebrew_manylines.cfg wihebrew_new.cfg \
              bhs4_trees.cfg bhs4_bracketed.cfg \
              Amsterdam.map \
              wh.cfg whutf8.cfg whwordsonly.cfg whwordsonlyutf8.cfg \
              doc/EQTUsersGuide.htb

EXTRA_DIST = win32.mak mingw.mak \
             av.cfg default.cfg wihebrew.cfg wh.cfg \
             mqlqtwx.rc \
             Amsterdam.map \
             Info.plist.in \
             scripturesys-eqtu.desktop \
             doc/EQTUsersGuide.pdf \
             doc/EQTUsersGuide.htb


DISTCLEANFILES = Info.plist

desktopdatadir = $(datadir)/applications
dist_desktopdata_DATA = scripturesys-eqtu.desktop

doc/EQTUsersGuide.htb:
	$(MAKE) -C doc


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



macdist:
	cp eqtc $(MACDISTDIR)/development/bin/
