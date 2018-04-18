#
# Emdros Win32 Makefile for harvest directory
#
# Makefile for Microsoft Visual C++ 5.0 (or compat)

# Will build a Win32 library libharvest.lib

WIN32DIR = ..\win32


LIBTARGET = libharvest.lib
TARGETS = "$(OUTDIR)\$(LIBTARGET)"
CLEANFILES = 

LIBTARGET_OBJS= \
        "$(INTDIR)\templatelang_classes.obj" \
        "$(INTDIR)\templatelang_parser.obj" \
        "$(INTDIR)\templatelang_lexer.obj" \
        "$(INTDIR)\renderxml.obj" \
        "$(INTDIR)\renderobjects.obj" \
        "$(INTDIR)\memobject.obj" \
        "$(INTDIR)\harvest_fts.obj" \
        "$(INTDIR)\harvest_fts2.obj" \
        "$(INTDIR)\harvest_fts3.obj"


!INCLUDE $(WIN32DIR)\config.mak
!INCLUDE $(WIN32DIR)\body.mak


