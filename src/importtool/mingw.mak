#
# Emdros Win32 Makefile for examples/ImportTool directory
#
# Makefile for MinGW

# Will build the Win32 program "EmdrosImportToolUnicode.exe".

WIN32DIR = ..\..\win32

include $(WIN32DIR)\mingw-config.mak

ifeq ($(WITH_WXWIN),yes)
WXUNICODE_PROGRAM1 = $(OUTDIR_UNICODE)\EmdrosImportToolUnicode.exe
endif

TARGETS = $(PROGRAMS)

LOCAL_UNICODE_CPPFLAGS = -I. $(WX_CPPFLAGS_UNICODE) -I$(WXDIR)/contrib/include  -I../../src/qrytool -I../../wx


WXUNICODE_PROGRAM1_OBJS= \
        importtool.o


EMDF_DEPENDENCIES = $(OUTDIR)\libemdf.a
MQL_DEPENDENCIES = $(OUTDIR)\libmql.a

ifeq ($(BPT),1)
BPT_DEPENDENCIES = $(OUTDIR)\libz_emdros.a
else
BPT_DEPENDENCIES = 
endif

ifeq ($(BUILD_FOR_DEVEL),1)
EMDROS_DEPENDENCIES = \
   $(OUTDIR)\libemdros_importers.a  \
   $(OUTDIR)\libutil_emdros.a  \
   $(MQL_DEPENDENCIES) \
   $(EMDF_DEPENDENCIES) \
   $(PCRELIB) \
   $(DBLIB) \
   $(BPT_DEPENDENCIES) 
else
EMDROS_DEPENDENCIES = \
   $(OUTDIR)\libemdros_amalgamation.lib \
   $(DBLIB) \
   $(BPT_DEPENDENCIES) 
endif


PROGRAM_DEPENDENCIES = \
   $(EXTRA_DEPENDENCIES) 


LOCAL_LIBFLAGS = $(PROGRAM_DEPENDENCIES) 


WXUNICODE_PROGRAM1_DEPENDENCIES = \
       $(OUTDIR_UNICODE)\libemdrosguiu.a \
       $(PROGRAM_DEPENDENCIES) \
       "$(OUTDIR)\EmdrosImportToolUnicode.res.o" 




LINK32_WXFLAGS_UNICODE_LOCALFLAGS = \
       $(OUTDIR_UNICODE)\libemdrosguiu.a \
       $(PROGRAM_DEPENDENCIES) 

CLEANFILES = 



include $(WIN32DIR)\mingw-body.mak


"$(OUTDIR)\EmdrosImportToolUnicode.res.o" :      importtool.rc $(WXDIR)\include\wx\msw\wx.rc
	$(RSC) -r -I$(WXDIR)\include -I$(WXDIR)\contrib\include importtool.rc $@




