#
# Emdros Win32 Makefile for src directory
#
# Makefile for Microsoft Visual C++ 5.0 (or compat)

# Will build Emdros programs


WIN32DIR = ..\win32

!INCLUDE $(WIN32DIR)\config.mak

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
PROGRAM11 = $(OUTDIR)\negraimport.exe
PROGRAM12 = $(OUTDIR)\renderobjects.exe
PROGRAM13 = $(OUTDIR)\bptdump.exe
PROGRAM14 = $(OUTDIR)\fts_indexer.exe
PROGRAM15 = $(OUTDIR)\fts_harvester.exe
PROGRAM16 = $(OUTDIR)\bptdetach.exe
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
           "$(OUTDIR)\libharvest.lib"  \
           "$(OUTDIR)\libutil_emdros.lib"  \
           "$(OUTDIR)\libemdros_importers.lib"  \
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

PROGRAM1_OBJS= "$(OUTDIR)\mql.obj" 
PROGRAM2_OBJS= "$(OUTDIR)\mqldump.obj" 
PROGRAM3_OBJS= "$(OUTDIR)\manage_indices.obj" 
PROGRAM4_OBJS= "$(OUTDIR)\upgrade_db.obj" 
PROGRAM5_OBJS= "$(OUTDIR)\ubimport.obj" 
PROGRAM6_OBJS= "$(OUTDIR)\pennimport.obj" 
PROGRAM7_OBJS= "$(OUTDIR)\negraimport.obj" 
PROGRAM8_OBJS= "$(OUTDIR)\plaintextimport.obj" 
PROGRAM9_OBJS= "$(OUTDIR)\slashedtextimport.obj" 
PROGRAM10_OBJS= "$(OUTDIR)\sfmimport.obj" 
PROGRAM11_OBJS= "$(OUTDIR)\negraimport.obj" 
PROGRAM12_OBJS= "$(OUTDIR)\renderobjectstool.obj" 
PROGRAM13_OBJS= "$(OUTDIR)\bptdump.obj" 
PROGRAM14_OBJS= "$(OUTDIR)\fts_indexer.obj" 
PROGRAM15_OBJS= "$(OUTDIR)\fts_harvester.obj" 
PROGRAM16_OBJS= "$(OUTDIR)\bptdetach.obj" 

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




!INCLUDE $(WIN32DIR)\body.mak
