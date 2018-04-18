#
# Emdros Win32 Makefile for amalgamation directory
#
# Makefile for Microsoft Visual C++ 5.0 (or compat)

WIN32DIR = ..\win32

!INCLUDE $(WIN32DIR)\config.mak


LOCAL_CPPFLAGS = -I. -DHAVE_CONFIG_H -DDEFAULT_BACKEND_ENUM=$(DEFAULT_BACKEND_ENUM)
LIBTARGET = libemdros_amalgamation.lib
TARGETS = emdros_config_h emdros_amalgamation_1.cpp "$(OUTDIR)\$(LIBTARGET)"
CLEANFILES = emdros_amalgamation_1.cpp \
             emdros_amalgamation_2.cpp \
             emdros_c_amalgamation_1.c \
             emdros_c_amalgamation_2.c \
	     emdros.h mkamalgamation.exe

LIBTARGET_OBJS= \
  	"$(INTDIR)\emdros_amalgamation_1.obj" \
  	"$(INTDIR)\emdros_amalgamation_2.obj" \
  	"$(INTDIR)\emdros_c_amalgamation_1.obj" \
  	"$(INTDIR)\emdros_c_amalgamation_2.obj" 

PROGRAM_DEPENDENCIES = "$(OUTDIR)\libemdf.lib"

LOCAL_LIBFLAGS = \
 $(DBLIB) $(PROGRAM_DEPENDENCIES)

PROGRAM1 = .\mkamalgamation.exe
PROGRAM1_OBJS = "$(INTDIR)\mkamalgamation.obj"
PROGRAM1_DEPENDENCIES = $(PROGRAM_DEPENDENCIES)


!INCLUDE $(WIN32DIR)\body.mak


emdros_amalgamation_1.cpp: .\mkamalgamation.exe amalgamation.json
	.\mkamalgamation.exe amalgamation.json

emdros_config_h:
	copy /Y /B ..\win32\config.h.win32 .\emdros-config.h
	copy /Y /B ..\win32\lconfig.h.win32 .\emdros-lconfig.h

