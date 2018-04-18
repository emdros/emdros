#
# Emdros Win32 Makefile for src/qrytool/TECkit directory
#
# Makefile for Microsoft Visual C++ 5.0 (or compat)

# Will build a Win32 library libteckit.lib

WIN32DIR = ..\..\..\win32

!INCLUDE $(WIN32DIR)\config.mak

LIBTARGET = libteckit.lib
TARGETS = "$(OUTDIR)\$(LIBTARGET)" \
          "$(OUTDIR)\tccompile.exe" "$(OUTDIR)\txtcnv.exe"
CLEANFILES = 

LOCAL_CPPFLAGS = /D "TARGET_RT_BIG_ENDIAN=0" /D "BZ_NO_STDIO=1" /D "NO_ZLIB"
LIBTARGET_OBJS= \
        "$(INTDIR)\Compiler.obj" \
        "$(INTDIR)\Engine.obj" \
        "$(INTDIR)\UnicodeNames.obj"

LOCAL_LIBFLAGS = $(OUTDIR)\$(LIBTARGET)

PROGRAM_DEPENDENCIES = $(OUTDIR)\$(LIBTARGET)

PROGRAM1 = $(OUTDIR)\tccompile.exe
PROGRAM1_OBJS = $(INTDIR)\TECkit_Compile.obj
PROGRAM1_DEPENDENCIES = $(PROGRAM_DEPENDENCIES)

PROGRAM2 = $(OUTDIR)\txtcnv.exe
PROGRAM2_OBJS = $(INTDIR)\TxtConv.obj
PROGRAM2_DEPENDENCIES = $(PROGRAM_DEPENDENCIES)

NO_C_IN_THIS_DIRECTORY = 1

!INCLUDE $(WIN32DIR)\body.mak

$(INTDIR)\TECkit_Compile.obj : TECkit_Compile.cpp

$(INTDIR)\TxtConv.obj : TxtConv.cpp