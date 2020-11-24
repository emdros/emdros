SUBDIRS = asv test_json

EXTRA_PROGRAMS = htreetestu htmlcanvastestu 


ALWAYSLDADD = @EMDROS_LDADD@ @ZLIB_LDADD@ \
              @SUN_LDFLAGS@  @DARWIN_LDFLAGS@

ALWAYSDEPS = @EMDROS_DEPS@ @ZLIB_DEPS@


if WITH_WX_PROGRAMS

if WX_DO_UNICODE
MAYBE_UNICODE_PRGS = htreetestu htmlcanvastestu 
else
MAYBE_UNICODE_PRGS =
endif

else
MAYBE_UNICODE_PRGS = 
endif


if HOSTISDARWIN
MACLDFLAGS = -static
if WITH_WX_PROGRAMS
maybe_all_local = htreetest.app htmlcanvastest.app 
maybe_distclean_local = MacOSX_distclean_local
else
maybe_all_local = 
maybe_distclean_local = 
endif
else
maybe_all_local = 
maybe_distclean_local = 
endif


bin_PROGRAMS = emdftry mqltry mqllingtry jsontry bpttry emdros_utiltry emdros_qdtry erwtry $(MAYBE_UNICODE_PRGS) 



# emdftry
emdftry_SOURCES = emdftry.cpp 

# mqltry
mqltry_SOURCES = mqltry.cpp 

# bpttry
bpttry_SOURCES = bpttry.cpp 

# mqllingtry
mqllingtry_SOURCES = mqllingtry.cpp 

# qdtry
emdros_qdtry_SOURCES = qdtry.cpp 

# jsontry
jsontry_SOURCES = jsontry.cpp 

# erwtry
erwtry_SOURCES = erwtry.cpp 

# emdros_utiltry
emdros_utiltry_SOURCES = emdros_utiltry.cpp 




emdftry_LDADD = $(ALWAYSLDADD)
emdftry_LDFLAGS = $(MACLDFLAGS)
emdftry_DEPENDENCIES = $(ALWAYSDEPS)


jsontry_LDADD = $(ALWAYSLDADD)
jsontry_LDFLAGS = $(MACLDFLAGS)
jsontry_DEPENDENCIES = $(ALWAYSDEPS)


emdros_qdtry_LDADD = $(ALWAYSLDADD)
emdros_qdtry_LDFLAGS = $(MACLDFLAGS)
emdros_qdtry_DEPENDENCIES = $(ALWAYSDEPS)


mqltry_LDADD = $(ALWAYSLDADD)
mqltry_LDFLAGS = $(MACLDFLAGS)
mqltry_DEPENDENCIES = mqlqa.h \
                      $(ALWAYSDEPS)

erwtry_LDADD = $(ALWAYSLDADD)
erwtry_LDFLAGS = $(MACLDFLAGS)
erwtry_DEPENDENCIES = $(ALWAYSDEPS)

emdros_utiltry_LDADD = $(ALWAYSLDADD)
emdros_utiltry_LDFLAGS = $(MACLDFLAGS)
emdros_utiltry_DEPENDENCIES = $(ALWAYSDEPS)


bpttry_LDADD = $(ALWAYSLDADD)
bpttry_LDFLAGS = $(MACLDFLAGS)
bpttry_DEPENDENCIES = bptqa.h \
                      $(ALWAYSDEPS)

mqllingtry_LDADD = $(ALWAYSLDADD)
mqllingtry_LDFLAGS = $(MACLDFLAGS)
mqllingtry_DEPENDENCIES = $(ALWAYSDEPS)



#
# htreetestu
#
htreetestu_LDADD = $(top_builddir)/wx/libemdrosguiu.a $(ALWAYSLDADD) \
                  @WX_UNICODE_LDFLAGS@ @WX_UNICODE_LIBS@
htreetestu_DEPENDENCIES = $(ALWAYSDEPS) $(top_builddir)/wx/libemdrosguiu.a
htreetestu_CXXFLAGS = $(AM_CXXFLAGS) -DwxUSE_UNICODE=1 @WX_UNICODE_CXXFLAGS@
htreetestu_SOURCES = htreetest.cpp



#
# htmlcanvastestu
#
htmlcanvastestu_LDADD = $(top_builddir)/wx/libemdrosguiu.a $(ALWAYSLDADD) \
                  @WX_UNICODE_LDFLAGS@ @WX_UNICODE_LIBS@
htmlcanvastestu_DEPENDENCIES = $(ALWAYSDEPS) $(top_builddir)/wx/libemdrosguiu.a
htmlcanvastestu_CXXFLAGS = $(AM_CXXFLAGS) -DwxUSE_UNICODE=1 @WX_UNICODE_CXXFLAGS@
htmlcanvastestu_SOURCES = htmlcanvastest.cpp







AM_CPPFLAGS = -I$(top_srcdir)/include -I. -I$(top_srcdir)/

EXTRA_DIST = win32.mak mingw.mak mqlqa.h.in mqlqa.h qa2h.py \
             bptqa.h.in bptqa.h bptqa2h.py \
             Info.plist.htreetest.in \
             Info.plist.htmlcanvastest.in \
             file2h.py win32_testall.bat

CLEANFILES = *~ core testincludes.o
DISTCLEANFILES = \
                Info.plist.htmlcanvastest \
                Info.plist.htreetest \
                config.log

# CXXFLAGS is included on the command-line already
AM_CXXFLAGS = @ADDITIONAL_CXXFLAGS@ @DEBUGFLAG@ @SUN_CXXFLAGS@ @DARWIN_CXXFLAGS@

BUILT_SOURCES = mqlqa.h bptqa.h \
                Info.plist.htmlcanvastest \
                Info.plist.htreetest



mqlqa.h: mqlqa.h.in qa2h.py
	rm -f $@
	python qa2h.py mqlqa.h.in $@


bptqa.h: bptqa.h.in bptqa2h.py
	rm -f $@
	python bptqa2h.py bptqa.h.in $@



# Test that all includes work
testincludes:
	g++ -c -o testincludes.o -I/usr/local/include/emdros ./testincludes.cpp


macdist:
	cp -f $(bin_PROGRAMS) $(MACDISTDIR)/development/bin/


htreetest.app: Info.plist.htreetest htreetestu $(top_srcdir)/wx/blue-E.icns
	rm -rf htreetest.app
	-mkdir htreetest.app
	-mkdir htreetest.app/Contents
	-mkdir htreetest.app/Contents/MacOS
	-mkdir htreetest.app/Contents/Resources
	cp -f $(top_srcdir)/wx/blue-E.icns htreetest.app/Contents/Resources/blue-E.icns
	cp -f Info.plist.htreetest htreetest.app/Contents/Info.plist
	echo -n 'APPL????' > htreetest.app/Contents/PkgInfo
	cp -f htreetestu htreetest.app/Contents/MacOS/htreetestu

Info.plist.htreetest: Info.plist.htreetest.in
	$(top_builddir)/config.status --file=$(srcdir)/Info.plist.htreetest




htmlcanvastest.app: Info.plist.htmlcanvastest htmlcanvastestu $(top_srcdir)/wx/blue-E.icns
	rm -rf htmlcanvastest.app
	-mkdir htmlcanvastest.app
	-mkdir htmlcanvastest.app/Contents
	-mkdir htmlcanvastest.app/Contents/MacOS
	-mkdir htmlcanvastest.app/Contents/Resources
	cp -f $(top_srcdir)/wx/blue-E.icns htmlcanvastest.app/Contents/Resources/blue-E.icns
	cp -f Info.plist.htmlcanvastest htmlcanvastest.app/Contents/Info.plist
	echo -n 'APPL????' > htmlcanvastest.app/Contents/PkgInfo
	cp -f htmlcanvastestu htmlcanvastest.app/Contents/MacOS/htmlcanvastestu

Info.plist.htmlcanvastest: Info.plist.htmlcanvastest.in
	$(top_builddir)/config.status --file=$(srcdir)/Info.plist.htmlcanvastest








all-local: $(maybe_all_local)


distclean-local: $(maybe_distclean_local)


MacOSX_distclean_local:
	rm -rf htreetest.app
	rm -rf htmlcanvastest.app

if SQLITE3
__maybe_emdftry_sqlite3 = ./emdftry -b 3
__maybe_mqltry_sqlite3 = ./mqltry -b 3
__maybe_bpttry_sqlite3 = ./bpttry -b 3
__maybe_mqllingtry_sqlite3 = ./mqllingtry -b 3
endif



test:
	$(MAKE) -C asv test
	$(MAKE) -C test_json test
	./emdros_utiltry
	$(__maybe_emdftry_sqlite3)
	$(__maybe_mqltry_sqlite3)
	$(__maybe_bpttry_sqlite3)
	$(__maybe_mqllingtry_sqlite3)
