#
# Emdros Win32 Makefile for src directory
#
# Makefile for MinW

# Will build Emdros programs


WIN32DIR = ..\win32

include $(WIN32DIR)\config.mak

PROGRAM1 = $(OUTDIR)\mql.exe
PROGRAM2 = $(OUTDIR)\mqldump.exe
PROGRAM3 = $(OUTDIR)\manage_indices.exe
PROGRAM4 = $(OUTDIR)\upgrade_db.exe 
PROGRAM5 = $(OUTDIR)\agexport.exe 
PROGRAM6 = $(OUTDIR)\ubimport.exe
PROGRAM7 = $(OUTDIR)\pennimport.exe
PROGRAM8 = $(OUTDIR)\negraimport.exe
PROGRAM9 = $(OUTDIR)\plaintextimport.exe
PROGRAM10 = $(OUTDIR)\slashedtextimport.exe
PROGRAM11 = $(OUTDIR)\sfmimport.exe
PROGRAM12 = $(OUTDIR)\renderobjects.exe
PROGRAM13 = $(OUTDIR)\bptdump.exe
PROGRAM14 = $(OUTDIR)\fts_indexer.exe
PROGRAM15 = $(OUTDIR)\fts_harvester.exe
PROGRAM16 = $(OUTDIR)\bptdetach.exe
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
           "$(OUTDIR)\libharvest.a"  \
           "$(OUTDIR)\libutil_emdros.a"  \
           "$(OUTDIR)\libemdros_importers.a"  \
           $(EMDF_DEPENDENCIES) \
           $(PCRELIB) \
           $(MQL_DEPENDENCIES)
else
EMDROS_DEPENDENCIES = $(OUTDIR)\libemdros_amalgamation.a
endif


PROGRAM_DEPENDENCIES = \
                       $(OUTDIR)\libharvest.a  \
                       $(OUTDIR)\libutil_emdros.a  \
                       $(OUTDIR)\libemdros_importers.a  \
                       $(MQL_DEPENDENCIES) \
                       $(PCRELIB) \
                       $(EMDF_DEPENDENCIES) \
                       $(BPT_DEPENDENCIES) \
                       $(EXTRA_DEPENDENCIES)


LOCAL_LIBFLAGS = \
$(PROGRAM_DEPENDENCIES) $(DBLIB) 

PROGRAM1_OBJS = mql.o
PROGRAM2_OBJS = mqldump.o
PROGRAM3_OBJS = manage_indices.o
PROGRAM4_OBJS = upgrade_db.o
PROGRAM5_OBJS = agexport.o
PROGRAM6_OBJS = ubimport.o
PROGRAM7_OBJS = pennimport.o
PROGRAM8_OBJS = negraimport.o
PROGRAM9_OBJS = plaintextimport.o
PROGRAM10_OBJS = slashedtextimport.o
PROGRAM11_OBJS = sfmimport.o
PROGRAM12_OBJS = renderobjectstool.o
PROGRAM13_OBJS = bptdump.o
PROGRAM14_OBJS = fts_indexer.o
PROGRAM15_OBJS = fts_harvester.o
PROGRAM16_OBJS = bptdetach.o

PROGRAM1_DEPENDENCIES = $(PROGRAM_DEPENDENCIES)
PROGRAM2_DEPENDENCIES = $(PROGRAM_DEPENDENCIES)
PROGRAM3_DEPENDENCIES = $(PROGRAM_DEPENDENCIES)
PROGRAM4_DEPENDENCIES = $(PROGRAM_DEPENDENCIES)
PROGRAM5_DEPENDENCIES = $(PROGRAM_DEPENDENCIES)
PROGRAM6_DEPENDENCIES = $(PROGRAM_DEPENDENCIES)
PROGRAM7_DEPENDENCIES = $(PROGRAM_DEPENDENCIES)
PROGRAM8_DEPENDENCIES = $(PROGRAM_DEPENDENCIES)
PROGRAM9_DEPENDENCIES = $(PROGRAM_DEPENDENCIES)
PROGRAM10_DEPENDENCIES = $(PROGRAM_DEPENDENCIES)
PROGRAM11_DEPENDENCIES = $(PROGRAM_DEPENDENCIES)
PROGRAM12_DEPENDENCIES = $(PROGRAM_DEPENDENCIES)
PROGRAM13_DEPENDENCIES = $(PROGRAM_DEPENDENCIES)
PROGRAM14_DEPENDENCIES = $(PROGRAM_DEPENDENCIES)
PROGRAM15_DEPENDENCIES = $(PROGRAM_DEPENDENCIES)
PROGRAM16_DEPENDENCIES = $(PROGRAM_DEPENDENCIES)



include $(WIN32DIR)\mingw-body.mak
