#
# Emdros Win32 Makefile for src directory
#
# Makefile for Microsoft Visual C++ 5.0 (or compat)

# Will build Emdros HAL programs


WIN32DIR = ..\..\win32

!INCLUDE $(WIN32DIR)\config.mak

PROGRAM1 = $(OUTDIR)\hal_build_db.exe
PROGRAM2 = $(OUTDIR)\mqlhal.exe

CLEANFILES = 

LIBTARGET = libhal.lib
TARGETS = "$(OUTDIR)\$(LIBTARGET)" $(PROGRAMS)


LIBTARGET_OBJS= \
        "$(INTDIR)\hal_build_database.obj" \
        "$(INTDIR)\hal_execution_environment.obj" \
        "$(INTDIR)\hallexer.obj" \
        "$(INTDIR)\HALSpace.obj" \
        "$(INTDIR)\hal_schema.obj" \
        "$(INTDIR)\hal_string_func.obj" 



EMDF_DEPENDENCIES = $(OUTDIR)\libemdf.lib
MQL_DEPENDENCIES = $(OUTDIR)\libmql.lib

!IF "$(BPT)" == "1"
BPT_DEPENDENCIES = $(OUTDIR)\libz_emdros.lib
!ELSE
BPT_DEPENDENCIES = 
!ENDIF

PROGRAM_DEPENDENCIES = $(OUTDIR)\$(LIBTARGET) \
                       $(BPT_DEPENDENCIES) \
                       $(EMDF_DEPENDENCIES) \
                       $(PCRELIB) \
                       $(MQL_DEPENDENCIES) \
                       $(EXTRA_DEPENDENCIES) \
                       $(OUTDIR)\libutil_emdros.lib \
                       $(OUTDIR)\libteckit.lib


LOCAL_CPPFLAGS = /I "." /I "..\..\src\qrytool\TECkit"


LOCAL_LIBFLAGS = \
 $(DBLIB) $(PROGRAM_DEPENDENCIES)

PROGRAM1_OBJS = $(OUTDIR)\halblddb.obj
PROGRAM2_OBJS = $(OUTDIR)\hal.obj

PROGRAM2_DEPENDENCIES = $(PROGRAM_DEPENDENCIES)
PROGRAM3_DEPENDENCIES = $(PROGRAM_DEPENDENCIES)


!INCLUDE $(WIN32DIR)\body.mak
