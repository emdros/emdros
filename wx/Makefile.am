# You may need to change these
LEMON = ./lemon

if HOSTISDARWIN
MACLDFLAGS = -static
endif

AM_CXXFLAGS = @DEBUGFLAG@ @SUN_CXXFLAGS@ @DARWIN_CXXFLAGS@ \
              @ADDITIONAL_CXXFLAGS@ 

if WX_DO_UNICODE
MAYBE_LIBS_UNICODE = libemdrosguiu.a
else
MAYBE_LIBS_UNICODE = 
endif


emdrosguilibsrc = \
                 htreecanvas.cpp \
                 dropdown_widget.cpp \
                 wxorder_chooser.cpp \
                 importerwizard.cpp \
                 configwizard.cpp \
                 browsewidget.cpp \
                 leftrightchooser.cpp \
                 viewmetrics.cpp \
                 layout.cpp \
                 treeitem.cpp \
                 vtreecanvas.cpp \
                 genericdlg.cpp \
                 wxutil_emdros.cpp \
                 conndlg.cpp


# Do not supply this, or Automake will fail miserably!
# It took me a long time to hunt down that this was not
# to be supplied.
#EXTRA_LTLIBRARIES = libemdrosgui.la libemdrosguiu.la

lib_LIBRARIES = $(MAYBE_LIBS_UNICODE)
DO_MACDIST_UNICODE = 


if WX_DO_UNICODE
DO_MACDIST_UNICODE += cp -f .libs/libemdrosguiu.a $(MACDISTDIR)/development/lib/
endif



dist_pkgdata_DATA = \
                    blue-E-16x16.png \
                    blue-E-22x22.png \
                    blue-E-32x32.png \
                    blue-E-48x48.png \
                    blue-E-64x64.png




EXTRA_DIST = win32.mak mingw.mak blue-E.xpm blue-E.ico blue-E.icns \
             blue-E-512x512.png blue-E-256x256.png \
             blue-E-128x128.png blue-E-64x64.png blue-E-48x48.png \
             blue-E-32x32.png blue-E-22x22.png blue-E-16x16.png \
             EmdrosSplashScreen.xpm \
             commit.xpm connect.xpm flash.xpm leftarrow.xpm rightarrow.xpm \
             copy.xpm cut.xpm new.xpm open.xpm paste.xpm save.xpm stop.xpm \
             together.xpm clear.xpm



libemdrosguiu_a_SOURCES = $(emdrosguilibsrc)
libemdrosguiu_a_CXXFLAGS = $(AM_CXXFLAGS) @WX_UNICODE_CXXFLAGS@
#libemdrosguiu_a_LIBADD = @WX_UNICODE_LDFLAGS@ @WX_UNICODE_LIBS@
#libemdrosguiu_a_LDFLAGS = $(MACLDFLAGS) \
#                          @SUN_LDFLAGS@ \
#                          @DARWIN_LDFLAGS@ 

AM_CPPFLAGS = -I$(top_srcdir)/include  -I$(top_srcdir)

CLEANFILES = *~ core 


macdist:
	$(DO_MACDIST_UNICODE)
