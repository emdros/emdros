lib_LTLIBRARIES = 

if HOSTISDARWIN
MACLDFLAGS = -static
endif

if USE_LOCAL_ZLIB
  lib_LTLIBRARIES += libz_emdros.la
  DO_MACDIST = cp -f .libs/libz_emdros.a $(MACDISTDIR)/development/lib/
  maybe_built_sources = $(srcdir)/../include/zconf.h
endif

sourcecode = adler32.c crc32.c deflate.c infback.c inffast.c inflate.c inftrees.c trees.c zutil.c compress.c uncompr.c gzclose.c gzlib.c gzread.c gzwrite.c

noinst_HEADERS = \
   crc32.h    gzguts.h   inffixed.h  inftrees.h  zconf.h  zutil.h \
   deflate.h  inffast.h  inflate.h   trees.h     zlib.h

libz_emdros_la_SOURCES = $(sourcecode)

libz_emdros_la_LDFLAGS = $(MACLDFLAGS) -version-info \
                     @LIB_EMDF_CURRENT@:@LIB_EMDF_REVISION@:@LIB_EMDF_AGE@ \
                     @SUN_LDFLAGS@
BUILT_SOURCES = $(maybe_build_sources)



CLEANFILES = $(srcdir)/../include/zlib.h $(srcdir)/../include/zconf.h *~ core

EXTRA_DIST = windows.mak mingw.mak \
             README.Emdros.txt \
             zlib_emdros.h.win32 zconf.h.win32 zconf.h.mingw \
             ChangeLog CMakeLists.txt configure FAQ README treebuild.xml zconf.h.cmakein zconf.h.in zlib2ansi zlib.3 \
             examples/enough.c examples/fitblk.c examples/gun.c examples/gzappend.c examples/gzjoin.c examples/gzlog.c examples/gzlog.h examples/README.examples examples/zlib_how.html examples/zpipe.c examples/zran.c

AM_CPPFLAGS = 
AM_CFLAGS = @DEBUGFLAG@ @ADDITIONAL_CFLAGS@




$(srcdir)/../include/zconf.h: zconf.h
	cp -pf zconf.h $(srcdir)/../include/zconf.h


macdist:	
	$(DO_MACDIST)


