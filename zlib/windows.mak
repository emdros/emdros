#
# Emdros Win32 Makefile for z directory
#
# Makefile for Microsoft Visual C++ 5.0 (or compat)

# Will build a Win32 library libz_emdros.lib

WIN32DIR = ..\windows\build


LOCAL_CPPFLAGS = -I. -DZ_STATIC -DHAVE_CONFIG_H
LIBTARGET = libz_emdros.lib
TARGETS = zconfhwin32 "$(OUTDIR)\$(LIBTARGET)"
CLEANFILES = chartables.c dftables.exe 

LIBTARGET_OBJS= \
     "$(INTDIR)/adler32.obj" \
     "$(INTDIR)/crc32.obj" \
     "$(INTDIR)/deflate.obj" \
     "$(INTDIR)/infback.obj" \
     "$(INTDIR)/inffast.obj" \
     "$(INTDIR)/inflate.obj" \
     "$(INTDIR)/inftrees.obj" \
     "$(INTDIR)/trees.obj" \
     "$(INTDIR)/zutil.obj" \
     "$(INTDIR)/compress.obj" \
     "$(INTDIR)/uncompr.obj" \
     "$(INTDIR)/gzclose.obj" \
     "$(INTDIR)/gzlib.obj" \
     "$(INTDIR)/gzread.obj" \
     "$(INTDIR)/gzwrite.obj" 


INTDIR_SUFFIX = \zlib_emdros

!INCLUDE $(WIN32DIR)\config.mak
!INCLUDE $(WIN32DIR)\body.mak


zconfhwin32:
        copy /Y zconf.h.win32 .\zconf.h
        copy /Y zconf.h.win32 ..\include\zconf.h

