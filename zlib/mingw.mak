#
# Emdros Win32 Makefile for z directory
#
# Makefile for MinGW

# Will build a Win32 library libz_emdros.lib

WIN32DIR = ..\win32


LOCAL_UNICODE_CPPFLAGS = -I. -DZ_STATIC -DHAVE_CONFIG_H -DSTD
LIBTARGET = libz_emdros.a
TARGETS = zconfhwin32 "$(OUTDIR)\$(LIBTARGET)"
CLEANFILES = 

LIBTARGET_OBJS= \
     adler32.o \
     crc32.o \
     deflate.o \
     infback.o \
     inffast.o \
     inflate.o \
     inftrees.o \
     trees.o \
     zutil.o \
     compress.o \
     uncompr.o \
     gzclose.o \
     gzlib.o \
     gzread.o \
     gzwrite.o 


INTDIR_SUFFIX = \zlib_emdros

include $(WIN32DIR)\config.mak
include $(WIN32DIR)\mingw-body.mak



zconfhwin32:
	copy /Y zconf.h.mingw .\zconf.h
	copy /Y zconf.h.mingw ..\include\zconf.h

