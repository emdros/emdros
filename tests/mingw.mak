#
# Emdros Win32 Makefile for tests directory
#
# Makefile for MinGW
#
# Will build Emdros test programs


WIN32DIR = ..\win32

include $(WIN32DIR)\config.mak

PROGRAM1 = $(OUTDIR)\emdftry.exe
PROGRAM2 = $(OUTDIR)\mqltry.exe
PROGRAM3 = $(OUTDIR)\mqllingtry.exe
PROGRAM4 = $(OUTDIR)\jsontry.exe
PROGRAM5 = $(OUTDIR)\bpttry.exe

CLEANFILES = 

TARGETS = $(PROGRAMS)


EMDF_DEPENDENCIES = $(OUTDIR)\libemdf.a
MQL_DEPENDENCIES = $(OUTDIR)\libmql.a
ifeq ($(BPT),1)
BPT_DEPENDENCIES = $(OUTDIR)\libz_emdros.a
else
BPT_DEPENDENCIES = 
endif


PROGRAM_DEPENDENCIES = \
                       $(OUTDIR)\libutil_emdros.a \
                       $(MQL_DEPENDENCIES) \
                       $(EMDF_DEPENDENCIES) \
                       $(PCRELIB) \
                       $(BPT_DEPENDENCIES) \
                       $(EXTRA_DEPENDENCIES)


LOCAL_LIBFLAGS = \
 $(PROGRAM_DEPENDENCIES) $(DBLIB) 

PROGRAM1_OBJS= emdftry.o
PROGRAM2_OBJS= mqltry.o 
PROGRAM3_OBJS= mqllingtry.o 
PROGRAM4_OBJS= jsontry.o 
PROGRAM5_OBJS= bpttry.o 

PROGRAM1_DEPENDENCIES = $(PROGRAM_DEPENDENCIES)
PROGRAM2_DEPENDENCIES = $(PROGRAM_DEPENDENCIES)
PROGRAM3_DEPENDENCIES = $(PROGRAM_DEPENDENCIES)
PROGRAM4_DEPENDENCIES = $(PROGRAM_DEPENDENCIES)
PROGRAM5_DEPENDENCIES = $(PROGRAM_DEPENDENCIES)


include $(WIN32DIR)\mingw-body.mak
