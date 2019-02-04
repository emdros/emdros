#
# Emdros Win32 Makefile for src/qrytool directory
#
# Makefile for MinGW

# Will build a Win32 library libqrytool.a
# and the eqt(c)(u).exe programs

WIN32DIR = ..\..\win32

include $(WIN32DIR)\config.mak

ifeq ($(WITH_WXWIN),yes)
WXUNICODE_PROGRAM1 = $(OUTDIR_UNICODE)\eqtu.exe
endif

LIBTARGET = libqrytool.a
TARGETS = "$(OUTDIR)\$(LIBTARGET)" $(PROGRAMS)

LOCAL_CPPFLAGS = -I.
LOCAL_UNICODE_CPPFLAGS = $(WX_CPPFLAGS_UNICODE) -I$(WXDIR)/contrib/include   -I../../wx -I.

LIBTARGET_OBJS= \
        solution.o \
        mytable.o \
        qtoutput.o \
        harvest.o \
        output_style.o \
        emdros_wrapper.o \
        consout.o \
        exec.o

WXUNICODE_PROGRAM1_OBJS= \
        mainframe.o \
        mqlqtwx.o \
        qtoutput.o \
        qtconnwizard.o \
        wxilqtout.o 


EMDF_DEPENDENCIES = $(OUTDIR)\libemdf.a
MQL_DEPENDENCIES = $(OUTDIR)\libmql.a
EXTRA_DEPENDENCIES = 
ifeq ($(BPT),1)
BPT_DEPENDENCIES = $(OUTDIR)\libz_emdros.a
else
BPT_DEPENDENCIES = 
endif



PROGRAM_DEPENDENCIES = \
   "$(OUTDIR)\$(LIBTARGET)" \
   $(OUTDIR)\libutil_emdros.a  \
   $(EXTRA_DEPENDENCIES) \
   $(MQL_DEPENDENCIES) \
   $(PCRELIB) \
   $(EMDF_DEPENDENCIES) \
   $(BPT_DEPENDENCIES) \
   $(DBLIB) 



LOCAL_LIBFLAGS = $(PROGRAM_DEPENDENCIES) 

PROGRAM1 = $(OUTDIR)\eqtc.exe
PROGRAM1_DEPENDENCIES = $(PROGRAM_DEPENDENCIES)
PROGRAM1_OBJS = mqlqt.o $(PROGRAM1_DEPENDENCIES)


WXUNICODE_PROGRAM1_DEPENDENCIES = \
       $(OUTDIR_UNICODE)\libemdrosguiu.a \
       $(PROGRAM_DEPENDENCIES) \
       "$(OUTDIR)\eqtu.res.o" 



LINK32_WXFLAGS_UNICODE_LOCALFLAGS = \
       "$(OUTDIR_UNICODE)\libemdrosguiu.a" \
       $(PROGRAM_DEPENDENCIES) 

CLEANFILES = 



include $(WIN32DIR)\mingw-body.mak


"$(OUTDIR)\eqtu.res.o" :      mqlqtwx.rc $(WXDIR)\include\wx\msw\wx.rc
	$(RSC) -r -I$(WXDIR)\include -I$(WXDIR)\contrib\include -J rc mqlqtwx.rc $@ 



