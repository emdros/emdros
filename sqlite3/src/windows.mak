#
# Emdros Win32 Makefile for sqlite3\src directory
#
# Makefile for Microsoft Visual C++ 5.0 (or compat)

# Will build a Win32 static library (non-debug) libsqlite3_emdros.lib
# and sqlite3_emdros.exe

TARGETNAME=libsqlite3_emdros
CPP_PROJ_EXTRA=/Gz 





WIN32DIR = ..\..\build\windows

!INCLUDE $(WIN32DIR)\config.mak

LIBTARGET = libsqlite3_emdros.lib

PROGRAM1 = $(OUTDIR)\sqlite3_emdros.exe

TARGETS = sqlite3_localamalgam.c $(OUTDIR)\$(LIBTARGET) $(PROGRAMS)

PROGRAM1_OBJS = $(INTDIR)\shell.obj

PROGRAM_DEPENDENCIES = $(OUTDIR)\$(LIBTARGET)

PROGRAM1_DEPENDENCIES = $(PROGRAM_DEPENDENCIES)

LOCAL_LIBFLAGS = $(PROGRAM_DEPENDENCIES)


LOCAL_CPPFLAGS = /D "SQLITE_CORE=1" /D "THREADSAFE=1" \
   /D SQLITE2_OMIT_INMEMORYDB=0 /D SQLITE_TEMP_STORE=2 \
   /D "OS_WIN=1" /D "OS_UNIX=0" /D "NDEBUG=1"

LIBTARGET_OBJS= \
     "$(INTDIR)\sqlite3_localamalgam.obj" 

# The reason we need a different directory for SQLite's INTDIR
# is that otherwise sqlite3\src\table.c and EMdF\table.cpp will 
# clash in their .obj files.
INTDIR_SUFFIX = \sqlite3

!INCLUDE $(WIN32DIR)\body.mak


sqlite3_localamalgam.c: prefix.c  sqlite3.c encryption.c
     copy /Y prefix.c +sqlite3.c +encryption.c sqlite3_localamalgam.c
