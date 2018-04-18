#
# Emdros Win32 Makefile for src/qrytool/TECkit directory
#
# Makefile for MinGW

# Will build a Win32 library libteckit.a

WIN32DIR = ..\..\..\win32

include $(WIN32DIR)\config.mak

LIBTARGET = libteckit.a
TARGETS = "$(OUTDIR)\$(LIBTARGET)" \
          $(OUTDIR)\tccompile.exe $(OUTDIR)\txtcnv.exe
CLEANFILES = 

LOCAL_CPPFLAGS = -DTARGET_RT_BIG_ENDIAN=0 -DBZ_NO_STDIO=1 -DNO_ZLIB
LIBTARGET_OBJS= \
        Compiler.o \
        Engine.o \
        UnicodeNames.o

LOCAL_LIBFLAGS = "$(OUTDIR)\$(LIBTARGET)"

PROGRAM_DEPENDENCIES = "$(OUTDIR)\$(LIBTARGET)"

PROGRAM1 = $(OUTDIR)\tccompile.exe
PROGRAM1_OBJS = TECkit_Compile.o
PROGRAM1_DEPENDENCIES = $(PROGRAM_DEPENDENCIES)

PROGRAM2 = $(OUTDIR)\txtcnv.exe
PROGRAM2_OBJS = TxtConv.o
PROGRAM2_DEPENDENCIES = $(PROGRAM_DEPENDENCIES)

NO_C_IN_THIS_DIRECTORY = 1

include $(WIN32DIR)\mingw-body.mak

TECkit_Compile.o : TECkit_Compile.cpp

TxtConv.o : TxtConv.cpp