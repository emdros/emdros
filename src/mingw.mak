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
PROGRAM5 = $(OUTDIR)\ubimport.exe
PROGRAM6 = $(OUTDIR)\pennimport.exe
PROGRAM7 = $(OUTDIR)\negraimport.exe
PROGRAM8 = $(OUTDIR)\plaintextimport.exe
PROGRAM9 = $(OUTDIR)\slashedtextimport.exe
PROGRAM10 = $(OUTDIR)\sfmimport.exe
PROGRAM11 = $(OUTDIR)\renderobjects.exe
PROGRAM12 = $(OUTDIR)\fts_indexer.exe
PROGRAM13 = $(OUTDIR)\fts_harvester.exe
PROGRAM14 = $(OUTDIR)\bptdump.exe
PROGRAM15 = $(OUTDIR)\bptdetach.exe
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
                       $(OUTDIR)\libharvest.a  \
                       $(OUTDIR)\libutil_emdros.a  \
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
PROGRAM5_OBJS = ubimport.o
PROGRAM6_OBJS = pennimport.o
PROGRAM7_OBJS = negraimport.o
PROGRAM8_OBJS = plaintextimport.o
PROGRAM9_OBJS = slashedtextimport.o
PROGRAM10_OBJS = sfmimport.o
PROGRAM11_OBJS = renderobjectstool.o
PROGRAM12_OBJS = fts_indexer.o
PROGRAM13_OBJS = fts_harvester.o
PROGRAM14_OBJS = bptdump.o
PROGRAM15_OBJS = bptdetach.o

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
