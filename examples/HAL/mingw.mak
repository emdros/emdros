#
# Emdros Win32 Makefile for src directory
#
# Makefile for MinGW

# Will build Emdros HAL programs


WIN32DIR = ..\..\win32

include $(WIN32DIR)\config.mak

PROGRAM1 = $(OUTDIR)\hal_build_db.exe
PROGRAM2 = $(OUTDIR)\mqlhal.exe

CLEANFILES = 

LIBTARGET = libhal.a
TARGETS = "$(OUTDIR)\$(LIBTARGET)" $(PROGRAMS)


LIBTARGET_OBJS= \
        hal_build_database.o \
        hal_execution_environment.o \
        hallexer.o \
        HALSpace.o \
        hal_schema.o \
        hal_string_func.o 



EMDF_DEPENDENCIES = $(OUTDIR)\libemdf.a
MQL_DEPENDENCIES = $(OUTDIR)\libmql.a

ifeq ($(BPT),1)
BPT_DEPENDENCIES = $(OUTDIR)\libz_emdros.a
else
BPT_DEPENDENCIES = 
endif

PROGRAM_DEPENDENCIES = $(OUTDIR)\$(LIBTARGET) \
                       $(OUTDIR)\libutil_emdros.a \
                       $(MQL_DEPENDENCIES) \
                       $(EMDF_DEPENDENCIES) \
                       $(BPT_DEPENDENCIES) \
                       $(PCRELIB) \
                       $(EXTRA_DEPENDENCIES) \
                       $(OUTDIR)\libteckit.a


LOCAL_CPPFLAGS = -I. -I..\..\src\qrytool\TECkit


LOCAL_LIBFLAGS = \
 $(PROGRAM_DEPENDENCIES) $(DBLIB) 

PROGRAM1_OBJS = halblddb.o
PROGRAM2_OBJS = hal.o

PROGRAM2_DEPENDENCIES = $(PROGRAM_DEPENDENCIES)
PROGRAM3_DEPENDENCIES = $(PROGRAM_DEPENDENCIES)


include $(WIN32DIR)\mingw-body.mak
