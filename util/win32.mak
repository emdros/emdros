#
# Emdros Win32 Makefile for util directory
#
# Makefile for Microsoft Visual C++ 5.0 (or compat)

# Will build a Win32 library libutil_emdros.lib

WIN32DIR = ..\win32


!IF "$(BUILD_FOR_DEVEL)" == "1"
LIBTARGET = libutil_emdros.lib
TARGETS = "$(OUTDIR)\$(LIBTARGET)"
!ELSE
LIBTARGET =
TARGETS = 
!ENDIF

CLEANFILES = 

LIBTARGET_OBJS= \
        "$(INTDIR)\conf.obj" \
        "$(INTDIR)\md5_emdros.obj" \
        "$(INTDIR)\opt.obj" \
        "$(INTDIR)\messages.obj" \
        "$(INTDIR)\erw.obj" \
        "$(INTDIR)\schema.obj" \
        "$(INTDIR)\mql_exporter.obj" \
        "$(INTDIR)\bptdumper.obj" 


!INCLUDE $(WIN32DIR)\config.mak
!INCLUDE $(WIN32DIR)\body.mak


