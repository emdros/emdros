#
# Emdros Win32 Makefile for sqlite3\src directory
#
# Makefile for MinGW

# Will build a Win32 static library (non-debug) libsqlite3_emdros.a
# and sqlite3_emdros.exe

TARGETNAME=libsqlite3_emdros.a
CPP_PROJ_EXTRA=





WIN32DIR = ..\..\win32

include $(WIN32DIR)\mingw-config.mak

LIBTARGET = libsqlite3_emdros.a

CPROGRAM1 = $(OUTDIR)\sqlite3_emdros.exe

TARGETS = sqlite3_localamalgam.c "$(OUTDIR)\$(LIBTARGET)" $(PROGRAMS)

CPROGRAM1_OBJS = shell.o

PROGRAM_DEPENDENCIES = $(OUTDIR)\$(LIBTARGET)

CPROGRAM1_DEPENDENCIES = $(PROGRAM_DEPENDENCIES)

LOCAL_LIBFLAGS = $(PROGRAM_DEPENDENCIES)


LOCAL_CPPFLAGS = -DSQLITE_CORE=1 -DTHREADSAFE=1 \
   -DSQLITE2_OMIT_INMEMORYDB=0 -DSQLITE_TEMP_STORE=2 \
   -DOS_WIN=1 -DOS_UNIX=0

LIBTARGET_OBJS= \
     sqlite3_localamalgam.o

# The reason we need a different directory for SQLite's INTDIR
# is that otherwise sqlite3\src\table.c and EMdF\table.cpp will 
# clash in their .obj files.
INTDIR_SUFFIX = \sqlite3

include $(WIN32DIR)\mingw-body.mak


sqlite3_localamalgam.c: prefix.c  sqlite3.c encryption.c
	copy /Y /B prefix.c +sqlite3.c +encryption.c sqlite3_localamalgam.c
