#
# Emdros Win32 Makefile for util directory
#
# Makefile for MinW

# Will build a Win32 library libutil_emdros.a

WIN32DIR = ..\win32

include $(WIN32DIR)\mingw-config.mak



ifeq ("$(BUILD_FOR_DEVEL)","1")
LIBTARGET = libutil_emdros.a
TARGETS = "$(OUTDIR)\$(LIBTARGET)"
else
LIBTARGET = 
TARGETS = 
endif

CLEANFILES = 

LIBTARGET_OBJS= \
        conf.o \
        md5_emdros.o \
        opt.o \
        messages.o \
        schema.o \
        erw.o \
        mql_exporter.o \
        bptdumper.o 



include $(WIN32DIR)\mingw-body.mak


