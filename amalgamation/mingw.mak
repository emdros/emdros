#
# Emdros Win32 Makefile for amalgamation directory
#
# Makefile for MinGW

# Will build a Win32 library libemdros_amalgamation.a

WIN32DIR = ..\win32

include $(WIN32DIR)\config.mak


LOCAL_CPPFLAGS = -I. -DHAVE_CONFIG_H -DDEFAULT_BACKEND_ENUM=$(DEFAULT_BACKEND_ENUM)
LIBTARGET = libemdros_amalgamation.a
TARGETS = emdros_config_h $(PROGRAM1) $(BUILT_SOURCES) "$(OUTDIR)\$(LIBTARGET)"

BUILT_SOURCES = emdros_amalgamation_1_emdros.cpp \
                emdros_amalgamation_2_importers.cpp \
                emdros_c_amalgamation_1.c \
                emdros_c_amalgamation_2.c 

CLEANFILES = $(BUILT_SOURCES) \
	     emdros.h mkamalgamation.exe

LIBTARGET_OBJS= \
  emdros_amalgamation_1_emdros.o \
  emdros_amalgamation_2_importers.o \
  emdros_c_amalgamation_1.o \
  emdros_c_amalgamation_2.o

PROGRAM_DEPENDENCIES = $(OUTDIR)\libemdf.a
 
PROGRAM1 = .\mkamalgamation.exe
PROGRAM1_OBJS = mkamalgamation.o
PROGRAM1_DEPENDENCIES = $(PROGRAM_DEPENDENCIES)

LOCAL_LIBFLAGS = $(PROGRAM_DEPENDENCIES)


include $(WIN32DIR)\mingw-body.mak


$(BUILT_SOURCES): amalgamation.json
	.\mkamalgamation.exe amalgamation.json

emdros_config_h:
	copy /Y /B ..\include\emdros-config.h .\emdros-config.h
	copy /Y /B ..\include\emdros-lconfig.h .\emdros-lconfig.h

