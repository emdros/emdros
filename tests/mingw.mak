#
# Emdros Win32 Makefile for tests directory
#
# Makefile for MinGW
#
# Will build Emdros test programs

# Don't try to make tests in parallel invocation mode
# (make # -j8, for example).  Otherwise, several instances of
# mqltry.exe etc. will be invoked in parallel, thus stepping on
# each other's toes.
.NOTPARALLEL:


WIN32DIR = ..\win32

include $(WIN32DIR)\mingw-config.mak

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

ifeq ($(BUILD_FOR_DEVEL),1)
EMDROS_DEPENDENCIES = \
           $(OUTDIR)\libutil_emdros.a \
           $(MQL_DEPENDENCIES) \
           $(EMDF_DEPENDENCIES) \
           $(PCRELIB)
else
EMDROS_DEPENDENCIES = $(OUTDIR)\libemdros_amalgamation.a
endif


PROGRAM_DEPENDENCIES = \
                       $(EMDROS_DEPENDENCIES) \
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


test: all test_sqlite3 test_bpt

ifeq ($(SQLITE3),1)
test_sqlite3:
	$(OUTDIR)\emdftry.exe -b 3
	$(OUTDIR)\mqltry.exe -b 3
else
	echo SQLite3 support not built in. Emdros on SQLite3 not tested.
endif

ifeq ($(BPT),1)
ifeq ($(SQLITE3),1)
test_bpt:
	$(OUTDIR)\bpttry.exe -b 3
else
test_bpt:
	echo BPT support and SQLite3 support not built in. Emdros on BPT not tested.
endif
else
test_bpt:
	echo BPT support and SQLite3 support not built in. Emdros on BPT not tested.
endif
