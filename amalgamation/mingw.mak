#
# Emdros Win32 Makefile for amalgamation directory
#
# Makefile for MinGW

# Will build a Win32 library libemdros_amalgamation.a

WIN32DIR = ..\win32

include $(WIN32DIR)\mingw-config.mak


LOCAL_CPPFLAGS = -I. -DHAVE_CONFIG_H -DDEFAULT_BACKEND_ENUM=$(DEFAULT_BACKEND_ENUM)
LIBTARGET = libemdros_amalgamation.a
TARGETS = .\emdros-config.h .\emdros-lconfig.h $(PROGRAM1) $(BUILT_SOURCES) "$(OUTDIR)\$(LIBTARGET)"

BUILT_SOURCES = emdros_amalgamation_1_emdros.cpp \
                emdros_amalgamation_2_importers_util.cpp \
                emdros_c_amalgamation_1.c \
                emdros_c_amalgamation_2.c 

CLEANFILES = $(BUILT_SOURCES) \
	     emdros.h emdros_c.h \
	     .\emdros-config.h .\emdros-lconfig.h \
	     $(PROGRAM1) \
	     amalgamation-built.txt

LIBTARGET_OBJS= \
  emdros_amalgamation_1_emdros.o \
  emdros_amalgamation_2_importers_util.o \
  emdros_c_amalgamation_1.o \
  emdros_c_amalgamation_2.o

PROGRAM_DEPENDENCIES = $(OUTDIR)\libemdf.a
 
PROGRAM1 = .\mkamalgamation.exe
PROGRAM1_OBJS = mkamalgamation.o
PROGRAM1_DEPENDENCIES = $(PROGRAM_DEPENDENCIES)

LOCAL_LIBFLAGS = $(PROGRAM_DEPENDENCIES)


include $(WIN32DIR)\mingw-body.mak


$(BUILT_SOURCES): amalgamation-built.txt

amalgamation-built.txt: amalgamation.xml $(PROGRAM1)
	$(PROGRAM1) amalgamation.xml
	echo BUILT > $@

.\emdros-config.h: ..\include\emdros-config.h
	copy /Y /B ..\include\emdros-config.h .\emdros-config.h

.\emdros-lconfig.h: ..\include\emdros-lconfig.h
	copy /Y /B ..\include\emdros-lconfig.h .\emdros-lconfig.h

