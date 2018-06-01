#
# Emdros Win32 Makefile for importers directory
#
# Makefile for Microsoft Visual C++ 5.0 (or compat)
#
# Will build a Win32 library libimporters.lib


#
# Custom macros
#
WIN32DIR = ..\win32

!INCLUDE $(WIN32DIR)\config.mak


LIBTARGET = libbase.lib
#TARGETS = "$(OUTDIR)\$(LIBTARGET)"
TARGETS = 
CLEANFILES = 

LOCAL_CPPFLAGS = 

LIBTARGET_OBJS= 





!INCLUDE $(WIN32DIR)\body.mak

