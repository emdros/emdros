#
# Emdros Win32 Makefile for SWIG\python directory
#
# Makefile for Microsoft Visual C++ 5.0 (or compat)
#
# Will build _EmdrosPy.dll and EmdrosPy.py
#

WIN32DIR = ..\..\win32

!INCLUDE $(WIN32DIR)\config.mak

SWIG_CPP_SRC = pyemdros_wrap.cxx
SWIG_PY_SRC = EmdrosPy.py

DLLTARGET = _EmdrosPy.dll
TARGETS = "$(OUTDIR)\$(DLLTARGET)" "$(OUTDIR)\EmdrosPy.py" "$(OUTDIR)\_EmdrosPy.pyd"

DLLTARGET_OBJS= \
     "$(INTDIR)\pyemdros_wrap.obj"

CLEANFILES = *~ config.log EmdrosPy.pyc EmdrosPy.pyo 

LOCAL_CPPFLAGS = /I "$(PYTHON_INCLUDE)"

HARVEST_DEPENDENCIES = $(OUTDIR)\libharvest.lib
EMDF_DEPENDENCIES = $(OUTDIR)\libemdf.lib
MQL_DEPENDENCIES = $(OUTDIR)\libmql.lib
SQLITE2_DEPENDENCIES = $(OUTDIR)\libsqlite_emdros.lib
SQLITE3_DEPENDENCIES = $(OUTDIR)\libsqlite3_emdros.lib

!IF "$(SQLITE2)" == "1"
EXTRA_DEPENDENCIES1 = $(SQLITE2_DEPENDENCIES)
!ELSE
EXTRA_DEPENDENCIES1 = 
!ENDIF

!IF "$(SQLITE3)" == "1"
EXTRA_DEPENDENCIES2 = $(SQLITE3_DEPENDENCIES)
!ELSE
EXTRA_DEPENDENCIES2 = 
!ENDIF

!IF "$(BPT)" == "1"
BPT_DEPENDENCIES = $(OUTDIR)\libz_emdros.lib
!ELSE
BPT_DEPENDENCIES = 
!ENDIF


LOCAL_LIBFLAGS = \
   $(DBLIB) \
   $(EMDF_DEPENDENCIES) \
   $(MQL_DEPENDENCIES) \
   $(EXTRA_DEPENDENCIES1) \
   $(EXTRA_DEPENDENCIES2) \
   $(HARVEST_DEPENDENCIES) \
   $(PCRELIB) \
   $(PYTHON_LIB) \
   $(BPT_DEPENDENCIES)

!INCLUDE $(WIN32DIR)\body.mak


   

"$(OUTDIR)\EmdrosPy.py" : .\EmdrosPy.py
	-copy .\EmdrosPy.py "$(OUTDIR)\EmdrosPy.py"


"$(OUTDIR)\_EmdrosPy.pyd" : "$(OUTDIR)\$(DLLTARGET)"
        -copy "$(OUTDIR)\$(DLLTARGET)" "$(OUTDIR)\_EmdrosPy.pyd"
