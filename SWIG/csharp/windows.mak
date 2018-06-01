#
# Emdros Win32 Makefile for SWIG\csharp directory
#
# Makefile for Microsoft Visual C++ 5.0 (or compat)


WIN32DIR=..\..\win32

!INCLUDE $(WIN32DIR)\config.mak


SWIG_CPP_SRC=libcsemdros_wrap.cxx


CLEANFILES1= *~


DLLTARGET=csemdros.dll
TARGETS="$(OUTDIR)\$(DLLTARGET)" "$(OUTDIR)\EmdrosDotNet.dll"


DLLTARGET_OBJS= \
     "$(INTDIR)\libcsemdros_wrap.obj"

CLEANFILES=$(CLEANFILES1) \
           $(INTDIR)\libcsemdros_wrap.obj config.log 

LOCAL_CPPFLAGS=

HARVEST_DEPENDENCIES = $(OUTDIR)\libharvest.lib
EMDF_DEPENDENCIES=$(OUTDIR)\libemdf.lib
MQL_DEPENDENCIES=$(OUTDIR)\libmql.lib
SQLITE3_DEPENDENCIES=$(OUTDIR)\libsqlite3_emdros.lib

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
   $(HARVEST_DEPENDENCIES) \
   $(PCRELIB) \
   oldnames.lib \
   $(ENTRY_POINT)

!INCLUDE $(WIN32DIR)\body.mak


"$(INTDIR)\libcsemdros_wrap.obj" : .\libcsemdros_wrap.cxx
   $(CPP) /c $(CPP_PROJ) /Fo"$(OUTDIR)\libcsemdros_wrap.obj" .\libcsemdros_wrap.cxx

"$(OUTDIR)\EmdrosDotNet.dll" : .\EmdrosDotNet.dll
  copy .\EmdrosDotNet.dll $(OUTDIR)\EmdrosDotNet.dll

