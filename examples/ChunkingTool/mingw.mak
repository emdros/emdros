#
# Emdros Win32 Makefile for examples/ChunkingTool directory
#
# Makefile for MinGW

# Will build the Win32 program "EmdrosChunkingToolUnicode.exe".

WIN32DIR = ..\..\win32

include $(WIN32DIR)\config.mak

ifeq ($(WITH_WXWIN),yes)
WXUNICODE_PROGRAM1 = $(OUTDIR_UNICODE)\EmdrosChunkingToolUnicode.exe
endif

TARGETS = $(PROGRAMS)

LOCAL_UNICODE_CPPFLAGS = -I. $(WX_CPPFLAGS_UNICODE) -I$(WXDIR)/contrib/include  -I../../src/qrytool -I../../wx


WXUNICODE_PROGRAM1_OBJS= \
        ctmf.o \
        ctwx.o \
        ctdoc.o 


EMDF_DEPENDENCIES = $(OUTDIR)\libemdf.a
MQL_DEPENDENCIES = $(OUTDIR)\libmql.a

ifeq ($(BPT),1)
BPT_DEPENDENCIES = $(OUTDIR)\libz_emdros.a
else
BPT_DEPENDENCIES = 
endif


PROGRAM_DEPENDENCIES = \
   $(OUTDIR)\libutil_emdros.a  \
   $(MQL_DEPENDENCIES) \
   $(EMDF_DEPENDENCIES) \
   $(DBLIB) \
   $(BPT_DEPENDENCIES) \
   $(EXTRA_DEPENDENCIES) \
   $(PCRELIB) \
   $(OUTDIR)\libteckit.a  \



LOCAL_LIBFLAGS = $(PROGRAM_DEPENDENCIES) 


WXUNICODE_PROGRAM1_DEPENDENCIES = \
       $(OUTDIR_UNICODE)\libemdrosguiu.a \
       $(PROGRAM_DEPENDENCIES) \
       "$(OUTDIR)\EmdrosChunkingToolUnicode.res.o" 



LINK32_WXFLAGS_UNICODE_LOCALFLAGS = \
       $(OUTDIR_UNICODE)\libemdrosguiu.a \
       $(PROGRAM_DEPENDENCIES) 

CLEANFILES = 



include $(WIN32DIR)\mingw-body.mak


"$(OUTDIR)\EmdrosChunkingToolUnicode.res.o":      ctwx.rc $(WXDIR)\include\wx\msw\wx.rc
	$(RSC) -r -I$(WXDIR)\include -I$(WXDIR)\contrib\include ctwx.rc $@ 



