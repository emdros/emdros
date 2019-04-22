#
# Emdros Win32 Makefile for z directory
#
# Makefile for MinGW

# Will build a Win32 library libz_emdros.lib

WIN32DIR = ..\win32

include $(WIN32DIR)\config.mak


ifeq ("$(BPT)","1")
LIBTARGET = libz_emdros.a
TARGETS = zconfhwin32 ..\include\zlib_emdros.h "$(OUTDIR)\$(LIBTARGET)"
else
LIBTARGET = libz_emdros.a
TARGETS = zconfhwin32 ..\include\zlib_emdros.h
endif


CLEANFILES = ..\include\zlib_emdros.h

LOCAL_UNICODE_CPPFLAGS = -I. -DZ_STATIC -DHAVE_CONFIG_H -DSTD


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

include $(WIN32DIR)\mingw-body.mak


..\include\zlib_emdros.h: zlib.h zlib_emdros.h.win32
	copy /Y /b zlib_emdros.h.win32 + zlib.h ..\include\zlib_emdros.h


zconfhwin32:
	copy /Y zconf.h ..\include\zconf.h

