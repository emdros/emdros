#
# Emdros Win32 Makefile for SWIG\java directory
#
# Makefile for Microsoft Visual C++ 5.0 (or compat)


WIN32DIR=..\..\win32

!INCLUDE $(WIN32DIR)\config.mak

# Include the SWIG_GENERATED_JAVAFILES macro
!INCLUDE Makefile.inc


SWIG_CPP_SRC=libjemdros_wrap.cxx
SWIG_JAR=jemdros.jar


CLEANFILES1= TestEmdros.class TestEmdros$$EMdFObject.class \
             TestEmdros$$Phrase.class TestEmdros$$Word.class .\jemdros\*

JAVA_INCLUDES=/I "$(SWIG_JDK_DIR)\include" /I "$(SWIG_JDK_DIR)\include\win32"

JAVAC="$(SWIG_JDK_DIR)\bin\javac.exe"
JAR="$(SWIG_JDK_DIR)\bin\jar.exe"



DLLTARGET=jemdros.dll
TARGETS="$(OUTDIR)\$(DLLTARGET)" "$(OUTDIR)\jemdros.jar"


DLLTARGET_OBJS= \
     "$(INTDIR)\libjemdros_wrap.obj"

CLEANFILES=$(CLEANFILES1) $(OUTDIR)\jemdros.dll $(OUTDIR)\jemdros.jar \
           $(INTDIR)\libjemdros_wrap.obj config.log 

LOCAL_CPPFLAGS=$(JAVA_INCLUDES)

HARVEST_DEPENDENCIES = $(OUTDIR)\libharvest.lib
EMDF_DEPENDENCIES=$(OUTDIR)\libemdf.lib
MQL_DEPENDENCIES=$(OUTDIR)\libmql.lib
SQLITE2_DEPENDENCIES=$(OUTDIR)\libsqlite_emdros.lib
SQLITE3_DEPENDENCIES=$(OUTDIR)\libsqlite3_emdros.lib

!IF "$(SQLITE2)" == "1"
EXTRA_DEPENDENCIES1=$(SQLITE2_DEPENDENCIES)
!ELSE
EXTRA_DEPENDENCIES1=
!ENDIF

!IF "$(SQLITE3)" == "1"
EXTRA_DEPENDENCIES2=$(SQLITE3_DEPENDENCIES)
!ELSE
EXTRA_DEPENDENCIES2=
!ENDIF

!IF "$(TARGET_CPU)" == "X86"
ENTRY_POINT = /entry:_DllMainCRTStartup@12 
!ELSE
ENTRY_POINT = /entry:_DllMainCRTStartup
!ENDIF

!IF "$(BPT)" == "1"
BPT_DEPENDENCIES = $(OUTDIR)\libz_emdros.lib
!ELSE
BPT_DEPENDENCIES = 
!ENDIF

LOCAL_LIBFLAGS=\
   $(BPT_DEPENDENCIES) \
   $(DBLIB) \
   $(EMDF_DEPENDENCIES) \
   $(MQL_DEPENDENCIES) \
   $(EXTRA_DEPENDENCIES1) \
   $(EXTRA_DEPENDENCIES2) \
   $(PCRELIB) \
   $(HARVEST_DEPENDENCIES) \
   oldnames.lib \
   $(ENTRY_POINT)

!INCLUDE $(WIN32DIR)\body.mak


"$(INTDIR)\libjemdros_wrap.obj" : .\libjemdros_wrap.cxx
   $(CPP) /c $(CPP_PROJ) /Fo"$(OUTDIR)\libjemdros_wrap.obj" .\libjemdros_wrap.cxx

"$(OUTDIR)\jemdros.jar":  $(SWIG_JAR)
      -copy $(SWIG_JAR) "$(OUTDIR)\jemdros.jar"



