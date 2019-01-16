if HOSTISDARWIN
MACLDFLAGS = -static
endif

VERSION=@SQLITE3_VERSION@

noinst_HEADERS = sqlite3ext.h sqlite3.h

EXTRA_DIST = win32.mak mingw.mak \
             prefix.c encryption.c sqlite3.c sqlite3_localamalgam.c



MAYBE_LIBS_SQLITE3 = libsqlite3_emdros.la
MAYBE_PROGS_SQLITE3 = sqlite3_emdros



ALWAYSLDADD = @SUN_LDFLAGS@  @DARWIN_LDFLAGS@

ALWAYSDEPS = 


bin_PROGRAMS = $(MAYBE_PROGS_SQLITE3)

lib_LTLIBRARIES = $(MAYBE_LIBS_SQLITE3)

sqlite3_emdros_src = sqlite3_emdros_amalgamation.c

#
# libsqlite3_emdros.la
#
libsqlite3_emdros_la_SOURCES = $(sqlite3_emdros_src)
libsqlite3_emdros_la_LIBADD = -lpthread
libsqlite3_emdros_la_LDFLAGS = $(MACLDFLAGS) -version-info \
                     @LIB_EMDROS_CURRENT@:@LIB_EMDROS_REVISION@:@LIB_EMDROS_AGE@ \
                     @SUN_LDFLAGS@ \
                     @DARWIN_LDFLAGS@

sqlite3_emdros_LDADD = $(ALWAYSLDADD) libsqlite3_emdros.la -ldl
sqlite3_emdros_DEPENDENCIES = $(ALWAYSDEPS) libsqlite3_emdros.la
sqlite3_emdros_LDFLAGS = $(MACLDFLAGS) 
sqlite3_emdros_SOURCES = shell.c


DISTCLEANFILES = *~ core sqlite3_localamalgam.c lempar.c


lib_LTLIBRARIES = libsqlite3_emdros.la

libsqlite3_emdros_la_SOURCES = $(LIBSRC)
libsqlite3_emdros_la_LDFLAGS = $(MACLDFLAGS) -version-info "8:6:8" -lpthread -ldl
libsqlite3_emdros_la_CFLAGS = $(AM_CFLAGS) -DTHREADSAFE=1 -DNDEBUG=1



bin_PROGRAMS = sqlite3_emdros 
sqlite3_emdros_SOURCES = shell.c
sqlite3_emdros_LDADD = libsqlite3_emdros.la
sqlite3_emdros_LDFLAGS = $(MACLDFLAGS)
sqlite3_emdros_CFLAGS = $(AM_CFLAGS) -DTHREADSAFE=1 -DNDEBUG=1
sqlite3_emdros_DEPENDENCIES = libsqlite3_emdros.la shell.c sqlite3.h


sqlite3_localamalgam.c: prefix.c sqlite3.c encryption.c
	cat prefix.c sqlite3.c encryption.c > $@
