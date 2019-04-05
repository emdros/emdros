#
# Emdros Win32 Makefile for harvest directory
#
# Makefile for MinGW

# Will build a Win32 library libharvest.a

WIN32DIR = ..\win32


ifeq ("$(BUILD_FOR_DEVEL)","1")
LIBTARGET = libharvest.a
TARGETS = "$(OUTDIR)\$(LIBTARGET)"
else
LIBTARGET = 
TARGETS = 
endif

CLEANFILES = 

LIBTARGET_OBJS= \
        templatelang_classes.o \
        templatelang_parser.o \
        templatelang_lexer.o \
        renderxml.o \
        renderobjects.o \
        memobject.o \
        harvest_fts.o \
        harvest_fts2.o \
        harvest_fts3.o



include $(WIN32DIR)\config.mak
include $(WIN32DIR)\mingw-body.mak


