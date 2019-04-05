#
# Emdros Win32 Makefile for src/qrytool directory
#
# Makefile for Microsoft Visual C++ 5.0 (or compat)

# Will build a Win32 library libqrytool.lib
# and the eqt(c)(u).exe programs

WIN32DIR = ..\..\win32

!INCLUDE $(WIN32DIR)\config.mak

!IF "$(WITH_WXWIN)" == "yes"
WXUNICODE_PROGRAM1 = $(OUTDIR_UNICODE)\eqtu.exe
!ENDIF

LIBTARGET = libqrytool.lib
TARGETS = "$(OUTDIR)\$(LIBTARGET)" $(PROGRAMS)

LOCAL_CPPFLAGS = -I.
LOCAL_UNICODE_CPPFLAGS = $(WX_CPPFLAGS_UNICODE) -I$(WXDIR)/contrib/include   -I../../wx -I.

LIBTARGET_OBJS= \
        "$(INTDIR)\solution.obj" \
        "$(INTDIR)\mytable.obj" \
        "$(INTDIR)\qtoutput.obj" \
        "$(INTDIR)\harvest.obj" \
        "$(INTDIR)\output_style.obj" \
        "$(INTDIR)\emdros_wrapper.obj" \
        "$(INTDIR)\consout.obj" \
        "$(INTDIR)\exec.obj"

WXUNICODE_PROGRAM1_OBJS= \
        "$(INTDIR_UNICODE)\mainframe.obj" \
        "$(INTDIR_UNICODE)\mqlqtwx.obj" \
        "$(INTDIR_UNICODE)\qtoutput.obj" \
        "$(INTDIR_UNICODE)\qtconnwizard.obj" \
        "$(INTDIR_UNICODE)\wxilqtout.obj" 


EMDF_DEPENDENCIES = $(OUTDIR)\libemdf.lib
MQL_DEPENDENCIES = $(OUTDIR)\libmql.lib
EXTRA_DEPENDENCIES = 
!IF "$(BPT)" == "1"
BPT_DEPENDENCIES = $(OUTDIR)\libz_emdros.lib
!ELSE
BPT_DEPENDENCIES = 
!ENDIF


!IF "$(BUILD_FOR_DEVEL)" == "1"
EMDROS_DEPENDENCIES = \
   "$(OUTDIR)\libutil_emdros.lib" \
   $(EMDF_DEPENDENCIES) \
   $(MQL_DEPENDENCIES) \
   $(PCRELIB)
!ELSE
EMDROS_DEPENDENCIES = $(OUTDIR)\libemdros_amalgamation.lib
!ENDIF



PROGRAM_DEPENDENCIES = \
   "$(OUTDIR)\$(LIBTARGET)" \
   $(BPT_DEPENDENCIES) \
   $(DBLIB) \
   $(EMDROS_DEPENDENCIES) \
   $(EXTRA_DEPENDENCIES) 



LOCAL_LIBFLAGS = $(PROGRAM_DEPENDENCIES) 

PROGRAM1 = $(OUTDIR)\eqtc.exe
PROGRAM1_DEPENDENCIES = $(PROGRAM_DEPENDENCIES)
PROGRAM1_OBJS = "$(INTDIR)\mqlqt.obj" $(PROGRAM1_DEPENDENCIES)


WXUNICODE_PROGRAM1_DEPENDENCIES = \
       "$(OUTDIR_UNICODE)\libemdrosguiu.lib" \
       $(PROGRAM_DEPENDENCIES) \
       "$(OUTDIR)\eqtu.res" 



LINK32_WXFLAGS_UNICODE_LOCALFLAGS = \
       "$(OUTDIR_UNICODE)\libemdrosguiu.lib" \
       $(PROGRAM_DEPENDENCIES)



CLEANFILES = $(OUTDIR_UNICODE)\emdros.pch $(OUTDIR_UNICODE)\vc60.idb $(OUTDIR_UNICODE)\vc70.idb



!INCLUDE $(WIN32DIR)\body.mak


"$(OUTDIR)\eqtu.res" :      mqlqtwx.rc $(WXDIR)\include\wx\msw\wx.rc
    $(RSC) -r /I$(WXDIR)\include /I$(WXDIR)\contrib\include -fo$@ mqlqtwx.rc



