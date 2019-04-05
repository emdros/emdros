#
# Emdros Win32 Makefile for tests directory
#
# Makefile for Microsoft Visual C++ 5.0 (or compat)
#
# Will build Emdros test programs


WIN32DIR = ..\win32

!INCLUDE $(WIN32DIR)\config.mak

PROGRAM1 = $(OUTDIR)\emdftry.exe
PROGRAM2 = $(OUTDIR)\mqltry.exe
PROGRAM3 = $(OUTDIR)\mqllingtry.exe
PROGRAM4 = $(OUTDIR)\jsontry.exe
PROGRAM5 = $(OUTDIR)\bpttry.exe
PROGRAM6 = $(OUTDIR)\emdros_qdtry.exe

CLEANFILES = 

TARGETS = $(PROGRAMS)


EMDF_DEPENDENCIES = $(OUTDIR)\libemdf.lib
MQL_DEPENDENCIES = $(OUTDIR)\libmql.lib
!IF "$(BPT)" == "1"
BPT_DEPENDENCIES = $(OUTDIR)\libz_emdros.lib
!ELSE
BPT_DEPENDENCIES = 
!ENDIF

!IF "$(BUILD_FOR_DEVEL)" == "1"
EMDROS_DEPENDENCIES = \
         "$(OUTDIR)\libutil_emdros.lib"  \
         $(EMDF_DEPENDENCIES) \
         $(PCRELIB) \
         $(MQL_DEPENDENCIES)
!ELSE
EMDROS_DEPENDENCIES = $(OUTDIR)\libemdros_amalgamation.lib
!ENDIF



PROGRAM_DEPENDENCIES = \
                       $(EMDROS_DEPENDENCIES) \
                       $(BPT_DEPENDENCIES) \
                       $(EXTRA_DEPENDENCIES)


LOCAL_LIBFLAGS = \
 $(DBLIB) $(PROGRAM_DEPENDENCIES)

PROGRAM1_OBJS= "$(OUTDIR)\emdftry.obj"
PROGRAM2_OBJS= "$(OUTDIR)\mqltry.obj" 
PROGRAM3_OBJS= "$(OUTDIR)\mqllingtry.obj" 
PROGRAM4_OBJS= "$(OUTDIR)\jsontry.obj" 
PROGRAM5_OBJS= "$(OUTDIR)\bpttry.obj" 
PROGRAM6_OBJS= "$(OUTDIR)\qdtry.obj" 

PROGRAM1_DEPENDENCIES = $(PROGRAM_DEPENDENCIES)
PROGRAM2_DEPENDENCIES = $(PROGRAM_DEPENDENCIES)
PROGRAM3_DEPENDENCIES = $(PROGRAM_DEPENDENCIES)
PROGRAM4_DEPENDENCIES = $(PROGRAM_DEPENDENCIES)
PROGRAM5_DEPENDENCIES = $(PROGRAM_DEPENDENCIES)
PROGRAM6_DEPENDENCIES = $(PROGRAM_DEPENDENCIES)


!INCLUDE $(WIN32DIR)\body.mak

test: all
!IF "$(SQLITE3)" == "1"
    "$(OUTDIR)\emdftry.exe" -b 3
    "$(OUTDIR)\mqltry.exe" -b 3
!ELSE
    echo SQLite3 support not built in. Emdros on SQLite3 not tested.
!ENDIF
!IF "$(SQLITE3)" == "1" && "$(BPT)" == "1"
    "$(OUTDIR)\bpttry.exe" -b 3
!ELSE
    echo BPT support not built in. Emdros on BPT not tested.
!ENDIF