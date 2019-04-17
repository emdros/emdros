#
# Emdros Win32 Makefile for z directory
#
# Makefile for Microsoft Visual C++ 5.0 (or compat)

# Will build a Win32 library libz_emdros.lib

WIN32DIR = ..\win32

!INCLUDE $(WIN32DIR)\config.mak

!IF "$(USE_BPT)" == "1"
LIBTARGET = libz_emdros.lib
TARGETS = zconfhwin32 zlib_emdrosh "$(OUTDIR)\$(LIBTARGET)"
!ELSE
LIBTARGET = 
TARGETS = zconfhwin32 zlib_emdrosh
!ENDIF

CLEANFILES = chartables.c dftables.exe ..\include\zlib_emdros.h

LOCAL_CPPFLAGS = -I. -DZ_STATIC -DHAVE_CONFIG_H


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

!INCLUDE $(WIN32DIR)\body.mak


zlib_emdrosh:
    -del ..\include\zlib_emdros.h
    copy /Y /b zlib_emdros.h.win32 +zlib.h ..\include\zlib_emdros.h


zconfhwin32:
        copy /Y zconf.h ..\include\zconf.h

