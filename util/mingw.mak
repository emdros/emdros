#
# Emdros Win32 Makefile for util directory
#
# Makefile for MinW

# Will build a Win32 library libutil_emdros.a

WIN32DIR = ..\win32


LIBTARGET = libutil_emdros.a
TARGETS = "$(OUTDIR)\$(LIBTARGET)"
CLEANFILES = 

LIBTARGET_OBJS= \
        conf.o \
        md5_emdros.o \
        opt.o \
        messages.o \
        agxml.o \
        schema.o \
        erw.o \
        mql_exporter.o \
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
        sfmimporter.o \
        bptdumper.o \
        bpt2dumper.o



include $(WIN32DIR)\config.mak
include $(WIN32DIR)\mingw-body.mak


