if HOSTISDARWIN
MACLDFLAGS = -static
endif

VERSION=@SQLITE3_VERSION@

LIBSRC=sqlite3_localamalgam.c

BUILT_SOURCES = sqlite3_localamalgam.c

noinst_HEADERS = sqlite3ext.h sqlite3.h

EXTRA_DIST = win32.mak mingw.mak \
             prefix.c encryption.c sqlite3.c sqlite3_localamalgam.c

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
