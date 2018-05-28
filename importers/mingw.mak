#
# Emdros Win32 Makefile for importer directory
#
# Makefile for Microsoft Visual C++ 5.0 (or compat)
#
# Will build a MinGW


#
# Custom macros
#
WIN32DIR = ..\..\win32

include $(WIN32DIR)\config.mak


LIBTARGET = libimporters.a
#TARGETS = "$(OUTDIR)\$(LIBTARGET)"
TARGETS = 
CLEANFILES = 

LOCAL_CPPFLAGS = 


LIBTARGET_OBJS = \
                 startup.o 




include $(WIN32DIR)\mingw-body.mak

