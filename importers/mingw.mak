#
# Emdros Win32 Makefile for util directory
#
# Makefile for MinW

# Will build a Win32 library libemdros_importers.a

WIN32DIR = ..\win32


ifeq ("$(BUILD_FOR_DEVEL)","1")
LIBTARGET = libemdros_importers.a
TARGETS = "$(OUTDIR)\$(LIBTARGET)"
else
LIBTARGET = 
TARGETS = 
endif

CLEANFILES = 

LIBTARGET_OBJS= \
        unboundbibleimporter.o \
        pennparser.o \
        pennlexer.o \
        penn_yylex.o \
        pennimporter.o \
        negraimporter.o \
        tigerxmlimporter.o \
        plaintextimporter.o \
        slashedtextimporter.o \
        sfm.o \
        sfmimporter.o 



include $(WIN32DIR)\config.mak
include $(WIN32DIR)\mingw-body.mak


