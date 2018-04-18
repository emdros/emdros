#
# Emdros Win32 Makefile for examples/ChunkingTool directory
#
# Makefile for Microsoft Visual C++ 5.0 (or compat)

# Will build the Win32 program "EmdrosChunkingToolUnicode.exe".

WIN32DIR = ..\..\win32

!INCLUDE $(WIN32DIR)\config.mak

!IF "$(WITH_WXWIN)" == "yes"
WXUNICODE_PROGRAM1 = $(OUTDIR_UNICODE)\EmdrosChunkingToolUnicode.exe
!ENDIF

TARGETS = $(PROGRAMS)

LOCAL_UNICODE_CPPFLAGS = -I. $(WX_CPPFLAGS_UNICODE) -I$(WXDIR)/contrib/include  -I../../src/qrytool -I../../wx


WXUNICODE_PROGRAM1_OBJS= \
        "$(INTDIR_UNICODE)\ctmf.obj" \
        "$(INTDIR_UNICODE)\ctwx.obj" \
        "$(INTDIR_UNICODE)\ctdoc.obj" 


EMDF_DEPENDENCIES = $(OUTDIR)\libemdf.lib
MQL_DEPENDENCIES = $(OUTDIR)\libmql.lib


!IF "$(BPT)" == "1"
BPT_DEPENDENCIES = $(OUTDIR)\libz_emdros.lib
!ELSE
BPT_DEPENDENCIES = 
!ENDIF


PROGRAM_DEPENDENCIES = \
   $(BPT_DEPENDENCIES) \
   "$(OUTDIR)\libteckit.lib"  \
   "$(OUTDIR)\libutil_emdros.lib"  \
   $(DBLIB) \
   $(EMDF_DEPENDENCIES) \
   $(MQL_DEPENDENCIES) \
   $(EXTRA_DEPENDENCIES) \
   $(PCRELIB) 

LOCAL_LIBFLAGS = $(PROGRAM_DEPENDENCIES) 


WXUNICODE_PROGRAM1_DEPENDENCIES = \
       "$(OUTDIR_UNICODE)\libemdrosguiu.lib" \
       $(PROGRAM_DEPENDENCIES) \
       "$(OUTDIR)\EmdrosChunkingToolUnicode.res" 



LINK32_WXFLAGS_UNICODE_LOCALFLAGS = \
       "$(OUTDIR_UNICODE)\libemdrosguiu.lib" \
       $(PROGRAM_DEPENDENCIES) 

CLEANFILES = $(OUTDIR_UNICODE)\emdros.pch $(OUTDIR_UNICODE)\vc60.idb $(OUTDIR_UNICODE)\vc70.idb



!INCLUDE $(WIN32DIR)\body.mak


"$(OUTDIR)\EmdrosChunkingToolUnicode.res" :      ctwx.rc $(WXDIR)\include\wx\msw\wx.rc
    $(RSC) -r /I$(WXDIR)\include /I$(WXDIR)\contrib\include -fo$@ ctwx.rc



